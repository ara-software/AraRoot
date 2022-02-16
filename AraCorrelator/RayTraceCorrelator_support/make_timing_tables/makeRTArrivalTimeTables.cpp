#include <iostream>
#include <sys/stat.h>

// ROOT Includes
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"

// ARA Includes
#include "AraGeomTool.h"
#include "RayTraceCorrelator.h"

// AraSim includes
#include "Position.h"
#include "Vector.h"
#include "IceModel.h"
#include "RaySolver.h"
#include "Settings.h"

std::map<int, Position> GetAntLocationsInEarthCoords(int station, IceModel *iceModel);
void CalculateTables(RayTraceCorrelator *theCorrelator, int solNum, int iceModelidx, const std::string &tableDir);
Position CalculateStationCOG(std::map<int, Position> antennaLocations);
void CalculateArrivalInformation(
    RaySolver *raySolver,
    IceModel *iceModel,
    Settings *settings,
    Position antennaLocation, Position stationCOG,
    double phiWave, double thetaWave, double R, int solNum,
    double &arrivalTime, double &arrivalTheta, double &arrivalPhi
    );

int main(int argc, char **argv)
{
    
    if(argc<4) {
        std::cout << "Usage\n" << argv[0] << " <station> <radius> <output location>\n";
        std::cout << "e.g.\n" << argv[0] << " 2 300 /path/to/my/home/dir \n";
        return 0;
    }

    int station = atoi(argv[1]);
    double radius = atof(argv[2]);
    
    double angular_size = 1.;
    int iceModelidx = 0;
    int unixTime = 0;
    int numAntennas = 16;


    std::string tempFileName = "temp.txt"; // the tables don't exist yet, so we feed it a dummy path
    RayTraceCorrelator *theCorrelator = new RayTraceCorrelator(station, numAntennas, 
        radius, angular_size, tempFileName, tempFileName
    );

    CalculateTables(theCorrelator, 0, iceModelidx, argv[3]);
    CalculateTables(theCorrelator, 1, iceModelidx, argv[3]);

}

void CalculateTables(RayTraceCorrelator *theCorrelator, int solNum, int iceModelidx, const std::string &tableDir){

    char fileName[500];
    sprintf(fileName, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_%d.root",
        tableDir.c_str(), theCorrelator->GetStationID(), iceModelidx,
        theCorrelator->GetRadius(), theCorrelator->GetAngularSize(), solNum
    );

    TFile *outfile = 0;
    TTree *tArrivalTimes = 0;

    outfile = new TFile(fileName, "RECREATE");
    tArrivalTimes = new TTree("tArrivalTimes", "tArrivalTimes");

    int ant, phiBin, thetaBin;
    double phi, theta;
    double arrivalTime, arrivalTheta, arrivalPhi;
    tArrivalTimes -> Branch("ant", & ant);
    tArrivalTimes -> Branch("phiBin", & phiBin);
    tArrivalTimes -> Branch("thetaBin", & thetaBin);
    tArrivalTimes -> Branch("arrivalTime", & arrivalTime);
    tArrivalTimes -> Branch("arrivalTheta", & arrivalTheta);
    tArrivalTimes -> Branch("arrivalPhi", & arrivalPhi);
    tArrivalTimes -> Branch("phi", & phi);
    tArrivalTimes -> Branch("theta", & theta);

    int numThetaBins = theCorrelator->GetNumThetaBins();
    int numPhiBins = theCorrelator->GetNumPhiBins();
    int numAnts = theCorrelator->GetNumAntennas();
    double radius = theCorrelator->GetRadius();
    std::vector<double> phiAngles = theCorrelator->GetPhiAngles();
    std::vector<double> thetaAngles = theCorrelator->GetThetaAngles();

    /*
    Set up ice model
    To load the AraSim ice model, it has to read some bedmap/depth files 
    located in the AraSim "data" directory
    If those files are missing, it errors out *very* unhelpfully.
    So we implement a check here, waiting for the day when AraSim handles
    this more intelligently.
    */
    struct stat buffer;
    bool dirExists = (stat("data", &buffer) == 0);
    if(!dirExists){
        throw std::runtime_error("The AraSim data directory is missing! Please make it, e.g. ln -s /path/to/AraSim/data data");
    }
    // need ice model
    IceModel *iceModel = new IceModel(0 + 1*10, 0, 0);

    // get the antenna locations in a way that AraSim likes
    std::map<int, Position> antennaLocations = GetAntLocationsInEarthCoords(theCorrelator->GetStationID(), iceModel);
    Position stationCOG = CalculateStationCOG(antennaLocations);

    // setup a ray solver
    RaySolver *raySolver = new RaySolver;

    // and settings
    Settings *settings = new Settings();
    
    // turn up the accuracy on the ray solving
    settings->Z_THIS_TOLERANCE = 1;
    settings->Z_TOLERANCE = 0.05;
    
    settings->NOFZ=1; // make sure n(z) is turned on
    settings->RAY_TRACE_ICE_MODEL_PARAMS = iceModelidx; // set the ice model as user requested

    for (int thetaBin_temp = 0; thetaBin_temp < numThetaBins; thetaBin_temp++) {

        printf("Solving theta strip at %.2f deg \n",thetaAngles[thetaBin_temp]*TMath::RadToDeg());

        for (int phiBin_temp = 0; phiBin_temp < numPhiBins; phiBin_temp++) {

            for (int ant_temp = 0; ant_temp < numAnts; ant_temp++) {

                Position antPosition = antennaLocations.find(ant_temp)->second;

                double arrivalTime_temp;
                double arrivalTheta_temp;
                double arrivalPhi_temp;
                CalculateArrivalInformation(
                    raySolver, iceModel, settings,
                    antPosition, stationCOG,
                    phiAngles[phiBin_temp], thetaAngles[thetaBin_temp], radius, solNum,
                    arrivalTime_temp, arrivalTheta_temp, arrivalPhi_temp

                );
                
                arrivalTime = arrivalTime_temp;
                arrivalTheta = arrivalTheta_temp;
                arrivalPhi = arrivalPhi_temp;
                // printf("  Phi %d, Ant %d, Arrival Time %.2f, Arrival Theta %.2f, Arrival Phi %.2f \n",
                //     phiBin_temp, ant_temp, arrivalTime_temp, 
                //     TMath::RadToDeg()*arrivalTheta_temp, TMath::RadToDeg()*arrivalPhi_temp
                // );
                ant = ant_temp;
                phiBin = phiBin_temp;
                thetaBin = thetaBin_temp;
                phi = phiAngles[phiBin];
                theta = thetaAngles[thetaBin];
                tArrivalTimes -> Fill();
            }
        }
    }
    outfile->Write();
    outfile->Close();
}

void CalculateArrivalInformation(
    RaySolver *raySolver,
    IceModel *iceModel,
    Settings *settings,
    Position antennaLocation, Position stationCOG,
    double phiWave, double thetaWave, double R, int solNum,
    double &arrivalTime, double  &arrivalTheta, double &arrivalPhi
    ){

    // first, configure the source position relative to detector center of gravity (COG)
    double xs = R * TMath::Cos(thetaWave) * TMath::Cos(phiWave);
    double ys = R * TMath::Cos(thetaWave) * TMath::Sin(phiWave);
    double zs = R * TMath::Sin(thetaWave);
    xs += stationCOG.GetX();
    ys += stationCOG.GetY();
    zs += stationCOG.GetZ();
    Position source;
    source.SetXYZ(xs, ys, zs);

    // set the target to the 
    Position target;
    target.SetXYZ(antennaLocation.GetX(), antennaLocation.GetY(), antennaLocation.GetZ());

    // outputs holder
    std::vector < std::vector < double > > outputs;
    vector < vector < vector <double> > > RayStep;
    raySolver->Solve_Ray(source, target, iceModel, outputs, settings, RayStep);

    if ((outputs.size() > 0)) {
        if (outputs[4].size() > solNum) {
            arrivalTime = outputs[4][solNum];
            arrivalTime *= 1e9;
            if (arrivalTime != arrivalTime) {
                arrivalTime = -2000;
            }

            // need to do math
            double receiveAngle = outputs[2][solNum];
            Vector receive_vector = target.Rotate( receiveAngle, source.Cross(target) ).Unit();
            // receive_vector = receive_vector.Unit();
            // receive_vector tells you where the signal is *going*
            // flip it around so that it specifies where the signal *came from*
            // which is more intuitive for calculating things like the gain
            Vector flip_receive_vector = -1. * receive_vector;
            arrivalTheta = flip_receive_vector.Theta();
            arrivalPhi = flip_receive_vector.Phi();

        }
        else{
            arrivalTime = -1500;
            arrivalTheta = -1500;
            arrivalPhi = -1500;
        }
    }
    else{
        arrivalTime = -1000;
        arrivalTheta = -1500;
        arrivalPhi = -1500;
    }
}

std::map<int, Position> GetAntLocationsInEarthCoords(int station, IceModel *iceModel){

    // store all of the locations
    std::map<int, Position> antennaLocations;

    std::map<int, int> map_ant_to_string;

    // string locations are set by the top hpol antenna on the string (for good or for ill...)
    std::map<int, Position> stringLocations;

    double core_x = 10000.;
    double core_y = 10000.;
    int num_strings = 0;

    AraGeomTool *geomTool = AraGeomTool::Instance();
    for(int ch=0; ch<16; ch++){
        Position temp;
        double *antLocation = geomTool->getStationInfo(station)->getAntennaInfo(ch)->antLocation;
        temp.SetXYZ(antLocation[0]+core_x, antLocation[1]+core_y, antLocation[2]);
        antennaLocations[ch] = temp;

        // VERY NAUGHTY THING TO DO!!! WHICH IS TO ASSUME THE CHANNEL MAPPING
        // THIS IS ALMOST CERTAINLY A BAD IDEA, BUT ARASIM LEAVES ME LITTLE CHOICE
        int string = ch%4;
        map_ant_to_string[ch] = string;

        if(ch==8 || ch==9 || ch==10 || ch==11){
            Position temp2;
            temp2.SetXYZ(antLocation[0]+core_x, antLocation[1]+core_y, antLocation[2]);
            stringLocations[num_strings] = temp2;
            num_strings++;
        }
    }

    double R1 = iceModel -> Surface(0., 0.);

    for(int i=0; i<4; i++){
        Position location = stringLocations.find(i)->second;

        double dist = sqrt(pow(location.GetX(), 2.) + pow(location.GetY(), 2.));
        double theta_tmp = dist/R1;
        double phi_tmp = atan2(location.GetY(), location.GetX());
        if (phi_tmp < 0.) phi_tmp += 2. * TMath::Pi(); // phase wrapping

        //update the location
        Position updated_location;
        updated_location.SetThetaPhi(theta_tmp, phi_tmp);
        updated_location.SetR(iceModel->Surface(updated_location.Lon(), updated_location.Lat()));

        auto itr = stringLocations.find(i);
        itr->second = updated_location;
    }

    double lowest_surface = 1.E7;
    for(int i=0; i<4; i++){
        double depth = stringLocations.find(i)->second.R();
        if(lowest_surface > depth){
            lowest_surface = depth;
        }
    }

    // now, update the antenna locations like AraSim does
    // see https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/Detector.cc#L3580
    for(int i=0; i<16; i++){
        Position location = antennaLocations.find(i)->second;
        // printf("Antenna %d before:, X/Y/Z %.2f, %.2f, %.2f\n",i, location[0], location[1], location[2]);

        int string = map_ant_to_string.find(i)->second;
        Position string_location = stringLocations.find(string)->second;

        Position updated_location;
        updated_location.SetRThetaPhi(lowest_surface + location.GetZ(), 
            string_location.Theta(), string_location.Phi()
        );

        auto itr = antennaLocations.find(i);
        itr->second = updated_location;
        Position junk = antennaLocations.find(i)->second;
    }
    return antennaLocations;
}

Position CalculateStationCOG(std::map<int, Position> antennaLocations){
    double average[3] = {0., 0., 0.};
    for(auto iter = antennaLocations.begin(); iter != antennaLocations.end(); ++iter){
        Position temp = iter->second;
        average[0] += temp.GetX();
        average[1] += temp.GetY();
        average[2] += temp.GetZ();
    }
    average[0]/=double(antennaLocations.size());
    average[1]/=double(antennaLocations.size());
    average[2]/=double(antennaLocations.size());
    Position cog;
    cog.SetXYZ(average[0], average[1], average[2]);
    return cog;
}
