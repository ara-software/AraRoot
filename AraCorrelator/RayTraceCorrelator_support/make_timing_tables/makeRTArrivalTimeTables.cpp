#include <iostream>
#include <sstream>
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
    double &arrivalTime, double &arrivalTheta, double &arrivalPhi, double &launchTheta, double &launchPhi
    );

int main(int argc, char **argv)
{

    gStyle->SetOptStat(0);
    
    if(argc<4) {
        std::cout << "Usage\n" << argv[0] << " <station> <radius> <output location>\n";
        std::cout << "e.g.\n" << argv[0] << " 2 300 /path/to/my/home/dir \n";
        return 0;
    }

    int station = atoi(argv[1]);
    double radius = atof(argv[2]);
    
    double angular_size = 1.;
    int iceModelidx = 50;
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

    int numThetaBins = theCorrelator->GetNumThetaBins();
    int numPhiBins = theCorrelator->GetNumPhiBins();
    int numAnts = theCorrelator->GetNumAntennas();
    double radius = theCorrelator->GetRadius();
    auto templateMap = theCorrelator->GetTemplateMap();

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

    // open output file BEFORE making root containers
    char fileName[500];
    sprintf(fileName, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_%d.root",
        tableDir.c_str(), theCorrelator->GetStationID(), iceModelidx,
        theCorrelator->GetRadius(), theCorrelator->GetAngularSize(), solNum
    );

    TFile *outfile = 0;
    TTree *tArrivalTimes = 0;
    outfile = new TFile(fileName, "RECREATE");
    // tArrivalTimes = new TTree("tArrivalTimes", "tArrivalTimes");

    // make containers
    std::vector<TH2D> arrivalTimeMaps;
    std::vector<TH2D> arrivalThetaMaps;
    std::vector<TH2D> arrivalPhiMaps;
    std::vector<TH2D> launchThetaMaps;
    std::vector<TH2D> launchPhiMaps;

    for(int i=0; i<numAnts; i++){
        stringstream ss;

        ss.str(""); ss << "arrival_time_ch_" << i;
        arrivalTimeMaps.push_back(TH2D(ss.str().c_str(), ss.str().c_str(), numPhiBins, -180, 180, numThetaBins, -90, 90));

        ss.str(""); ss << "arrival_theta_ch" << i;
        arrivalThetaMaps.push_back(TH2D(ss.str().c_str(), ss.str().c_str(), numPhiBins, -180, 180, numThetaBins, -90, 90));

        ss.str(""); ss << "arrival_phi_ch" << i;
        arrivalPhiMaps.push_back(TH2D(ss.str().c_str(), ss.str().c_str(), numPhiBins, -180, 180, numThetaBins, -90, 90));

        ss.str(""); ss << "launch_theta_ch" << i;
        launchThetaMaps.push_back(TH2D(ss.str().c_str(), ss.str().c_str(), numPhiBins, -180, 180, numThetaBins, -90, 90));

        ss.str(""); ss << "launch_phi_ch" << i;
        launchPhiMaps.push_back(TH2D(ss.str().c_str(), ss.str().c_str(), numPhiBins, -180, 180, numThetaBins, -90, 90));
    }

    // fill up the containers
    for (int ant_temp = 0; ant_temp < numAnts; ant_temp++) {

        std::cout<<"Solving antenna " << ant_temp<<std::endl;
        Position antPosition = antennaLocations.find(ant_temp)->second;

        for(int phiBin = templateMap->GetXaxis()->GetFirst(); phiBin <= templateMap->GetXaxis()->GetLast(); phiBin++){
        
            double phi = templateMap->GetXaxis()->GetBinCenter(phiBin);
            // printf("  Solving phi Bin %d, Phi Bin Center %.2f\n", phiBin, phi);

            for(int thetaBin = templateMap->GetYaxis()->GetFirst(); thetaBin <= templateMap->GetYaxis()->GetLast(); thetaBin++){
            
                double theta = templateMap->GetYaxis()->GetBinCenter(thetaBin);
                // printf("    Solving theta bin %d, theta bin center %.2f\n", thetaBin, theta);

                double arrivalTime_temp;
                double arrivalTheta_temp;
                double arrivalPhi_temp;
                double launchTheta_temp;
                double launchPhi_temp;
                CalculateArrivalInformation(
                    raySolver, iceModel, settings,
                    antPosition, stationCOG,
                    phi * TMath::DegToRad(), theta*TMath::DegToRad(), radius, solNum,
                    arrivalTime_temp, arrivalTheta_temp, arrivalPhi_temp, launchTheta_temp, launchPhi_temp
                );
                arrivalTimeMaps[ant_temp].SetBinContent(phiBin,thetaBin,arrivalTime_temp);
                arrivalThetaMaps[ant_temp].SetBinContent(phiBin,thetaBin,arrivalTheta_temp);
                arrivalPhiMaps[ant_temp].SetBinContent(phiBin,thetaBin,arrivalPhi_temp);
                launchThetaMaps[ant_temp].SetBinContent(phiBin,thetaBin,launchTheta_temp);
                launchPhiMaps[ant_temp].SetBinContent(phiBin,thetaBin,launchPhi_temp);

            }
        }

        // TCanvas *c = new TCanvas("", "", 1100, 850);
        // arrivalTimeMaps[ant_temp].Draw("colz"); // standard colz projection
        // char title[500];
        // sprintf(title,"timing_ant%d_%d.png", ant_temp,solNum);
        // c->SaveAs(title);
        // delete c;

    }

    for (int ant_temp = 0; ant_temp < numAnts; ant_temp++) {
        arrivalTimeMaps[ant_temp].Write();
        arrivalThetaMaps[ant_temp].Write();
        arrivalPhiMaps[ant_temp].Write();
        launchThetaMaps[ant_temp].Write();
        launchPhiMaps[ant_temp].Write();
    }

    outfile->Close();

}

void CalculateArrivalInformation(
    RaySolver *raySolver,
    IceModel *iceModel,
    Settings *settings,
    Position antennaLocation, Position stationCOG,
    double phiWave, double thetaWave, double R, int solNum,
    double &arrivalTime, double  &arrivalTheta, double &arrivalPhi, double &launchTheta, double &launchPhi
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
            
            double launchAngle = outputs[1][solNum];
            Vector launchVector = target.Rotate( launchAngle, source.Cross(target) ).Unit();
            launchTheta = launchVector.Theta();
            launchPhi = launchVector.Phi();
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
