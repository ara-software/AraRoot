//C/C++ includes
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <stdexcept>
#include <math.h>

//ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"

// AraRoot includes
#include "AraGeomTool.h"
#include "RayTraceCorrelator.h"
#include "RayTraceCorrelator_detail.h"

void RayTraceCorrelator::SetupStationInfo(int stationID, int unixTime) { 
    char errorMessage[400];

    // set station ID
    if(stationID>5 || stationID<0 || isnan(stationID)){ // testbed through A5 is supported
        sprintf(errorMessage,"Requested station (%d) is not supported\n", stationID);
        throw std::invalid_argument(errorMessage);
    }
    stationID_ = stationID;

    // set unixTime
    if(unixTime<0 || isnan(unixTime)){ // testbed through A5 is supported
        sprintf(errorMessage,"Requested unixTime (%d) is not supported\n", unixTime);
        throw std::invalid_argument(errorMessage);
    }
    unixTime_ = unixTime;

    // save the station info object
    // and count the number of non-surface channels
    AraGeomTool *geomTool = AraGeomTool::Instance();
    auto araStationInfo = geomTool->getStationInfo(stationID_, unixTime);
    int numAnts = (int) araStationInfo->getNumRFChans();
    int num_not_surface = 0;
    for(int ant=0; ant<numAnts; ant++){
        auto pol = araStationInfo->getAntennaInfo(ant)->polType;
        if(pol!= AraAntPol::kSurface){
            num_not_surface++;
        }
    }
    numAntennas_ = num_not_surface;
}

void RayTraceCorrelator::SetAngularConfig(double angularSize){
    if(angularSize<0 || isnan(angularSize)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested angular binning (%e) is not supported\n", angularSize);
        throw std::invalid_argument(errorMessage);        
    }
    angularSize_ = angularSize;
    numPhiBins_ = int(360. / angularSize);
    numThetaBins_ = int(180. / angularSize);

    // now fill up the phi and theta angles
    double PhiWaveDeg, ThetaWaveDeg;
    for (int i = 0; i < numPhiBins_; i++) {
        PhiWaveDeg = -180 + 0.5 * angularSize_ + angularSize_ * i;
        phiAngles_.push_back(PhiWaveDeg * TMath::DegToRad());
    }
    for (int i = 0; i < numThetaBins_; i++) {
        ThetaWaveDeg = -90 + 0.5 * angularSize_ + angularSize_ * i;
        thetaAngles_.push_back(ThetaWaveDeg * TMath::DegToRad());
    }
}

void RayTraceCorrelator::SetRadius(double radius){
    if(radius<0 || isnan(radius)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested radius (%e) is not supported\n", radius);
        throw std::invalid_argument(errorMessage);        
    }
    radius_ = radius;
}

void RayTraceCorrelator::SetIceModel(int iceModel){
    if(iceModel<0 || isnan(iceModel)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested icemodel (%e) is has an\n", iceModel);
        throw std::invalid_argument(errorMessage);        
    }
    iceModel_ = iceModel;
}

void RayTraceCorrelator::ConfigureArrivalTimesVector(){
    arrivalTimes_.resize(2);
    for(int sol=0; sol<2; sol++){
        arrivalTimes_[sol].resize(numThetaBins_);
        for(int thetaBin=0; thetaBin<numThetaBins_; thetaBin++){
            arrivalTimes_[sol][thetaBin].resize(numPhiBins_);
        }
        for(int thetaBin=0; thetaBin<numThetaBins_; thetaBin++){
            for(int phiBin=0; phiBin<numPhiBins_; phiBin++){
                arrivalTimes_[sol][thetaBin][phiBin].resize(numAntennas_);
            }
        }
    }
}

void RayTraceCorrelator::LoadArrivalTimeTables(const std::string &filename, int solNum){
    char errorMessage[400];
    
    // try to open the file
    TFile * infile = TFile::Open(filename.c_str(), "READ");
    if(!infile){
        sprintf(errorMessage, "Opening of the table (%s) was unsuccessful\n",filename.c_str());
        throw std::runtime_error(errorMessage);
    }
    
    // then, try to load the arrival times tables
    TTree * tTree = (TTree*) infile->Get("tArrivalTimes");
    if(!tTree){
        infile->Close();
        sprintf(errorMessage, "Arrival time tables could not be found in the file (%s)\n",filename.c_str());
        throw std::runtime_error(errorMessage);
    }

    // if the file is good, load the variables from the branches
    int ant, phiBin, thetaBin;
    double arrivalTime, phi, theta;
    tTree -> SetBranchAddress("ant", & ant);
    tTree -> SetBranchAddress("phiBin", & phiBin);
    tTree -> SetBranchAddress("thetaBin", & thetaBin);
    tTree -> SetBranchAddress("arrivalTime", & arrivalTime);
    tTree -> SetBranchAddress("phi", & phi);
    tTree -> SetBranchAddress("theta", & theta);

    // and put those values into the vector
    int nEntries = tTree -> GetEntries();
    for (int i = 0; i < nEntries; i++) {
        tTree -> GetEntry(i);
        arrivalTimes_[solNum][thetaBin][phiBin][ant] = arrivalTime;
    }

    // close up
    infile->Close();
}

RayTraceCorrelator::~RayTraceCorrelator()
{
	//Default destructor
}

RayTraceCorrelator::RayTraceCorrelator(int stationID, 
    double radius, 
    double angularSize,
    int iceModel,
    int unixTime
    ){

    // initialize and sanitize the input immediately
    // afterwards, we can (safely!) only refer to private variables
    this->SetAngularConfig(angularSize);
    this->SetupStationInfo(stationID, unixTime);
    this->SetRadius(radius);
    this->SetIceModel(iceModel);
}

void RayTraceCorrelator::LoadTables(const std::string &tableDir){
    char errorMessage[400];
    
    // verify the directory containing the tables exists
    struct stat buffer;
    bool dirExists = (stat(tableDir.c_str(), &buffer) == 0);
    if(!dirExists){
        sprintf(errorMessage,"Tables directory (%s) does not exist! Abort!\n", tableDir.c_str());
        throw std::runtime_error(errorMessage);
    }

    // set the table file names
    char directFileName[500];
    char reflecFileName[500];
    sprintf(directFileName, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_0.root",
        tableDir.c_str(), stationID_, iceModel_, radius_, angularSize_);
    sprintf(reflecFileName, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_1.root",
        tableDir.c_str(), stationID_, iceModel_, radius_, angularSize_);
    
    // throw errors if these files don't exist
    bool dirFileExists = (stat(directFileName, &buffer) == 0);
    bool refFileExists = (stat(reflecFileName, &buffer) == 0);
    if (!dirFileExists || !refFileExists){
        sprintf(errorMessage,"An arrival times table is missing (dir file exists %d, ref file exists %d) ", dirFileExists, refFileExists);
        throw std::runtime_error(errorMessage);       
    }

    // load the arrival time tables
    this->ConfigureArrivalTimesVector();
    this->LoadArrivalTimeTables(directFileName, 0);
    this->LoadArrivalTimeTables(reflecFileName, 1);
}

std::map< int, std::vector<int> > RayTraceCorrelator::SetupPairs(
    AraAntPol::AraAntPol_t polSelection, 
    std::vector<int> excludedChannels){
    
    // first, figure out what set of antennas is viable to form pairs
    std::vector < int > viableAntennas;
    
    AraGeomTool *geomTool = AraGeomTool::Instance();
    auto araStationInfo = geomTool->getStationInfo(this->stationID_, this->unixTime_);
    for(int ant=0; ant<this->numAntennas_; ant++){

        // first, check if this event is allowed
        // if ant *is* in the list of excluded channels, don't use it
        bool isExcluded = (std::find(excludedChannels.begin(), excludedChannels.end(), ant) != excludedChannels.end());
        if(isExcluded){
            continue;
        }

        // then, see if this is a polarization we intended to select
        if(!isExcluded){
            auto pol = araStationInfo->getAntennaInfo(ant)->polType;
            if(pol==polSelection){
                viableAntennas.push_back(ant);
            }
        }
    }

    // form pairs
    std::map<int, std::vector<int> > pairs;
    int num_pairs = 0;
    for(int index1 = 0; index1 < viableAntennas.size() - 1; index1++){
        for(int index2 = index1 +1; index2 < viableAntennas.size(); index2++){
            int ant1 = viableAntennas[index1];
            int ant2 = viableAntennas[index2];
            std::vector<int> pair_temp;
            pair_temp.push_back(ant1);
            pair_temp.push_back(ant2);
            pairs[num_pairs] = pair_temp;
            num_pairs++;
        }
    }
    return pairs;
}

TH2D* RayTraceCorrelator::GetInterferometricMap(
    std::map<int, TGraph*> interpolatedWaveforms, 
    std::map<int, std::vector<int> > pairs,
    int solNum,
    bool applyHilbertEnvelope
    ){
        
    // first, calculate all of the correlation functions

    // for performance reasons, it's actually better to store
    // the correlation functions as a vector
    std::vector<TGraph*> corrFunctions;
    for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
        int pairNum = iter->first;
        int ant1 = iter->second[0];
        int ant2 = iter->second[1];

        TGraph *grCorr = getCorrelationGraph_WFweight(
            interpolatedWaveforms.find(ant1)->second,
            interpolatedWaveforms.find(ant2)->second
        );
        if(applyHilbertEnvelope){
            TGraph *grCorrHil = FFTtools::getHilbertEnvelope(grCorr);
            corrFunctions.push_back(grCorrHil);
            delete grCorr;
        }
        else{
            corrFunctions.push_back(grCorr);
        }
    }

    double scale = 1./double(pairs.size());
    TH2D *histMap = new TH2D("", "", 
        this->numPhiBins_, -180, 180, 
        this->numThetaBins_, -90, 90
    );

    // now, make the map
    for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
        int pairNum = iter->first;
        int ant1 = iter->second[0];
        int ant2 = iter->second[1];

        for(int phiBin=0; phiBin < this->numPhiBins_; phiBin++){
            for(int thetaBin=0; thetaBin < this->numThetaBins_; thetaBin++){
                
                int globalBin = (phiBin + 1) + (thetaBin + 1) * (this->numPhiBins_ + 2);
                double arrival_time1 = this->arrivalTimes_[solNum][thetaBin][phiBin][ant1];
                double arrival_time2 = this->arrivalTimes_[solNum][thetaBin][phiBin][ant2];
                double dt = arrival_time1 - arrival_time2;

                // sanity check
                if (arrival_time1 < -100 || arrival_time2 < -100 || histMap -> GetBinContent(globalBin) == -1000) {
                    histMap -> SetBinContent(globalBin, -1000);
                }
                else{
                    double corrVal = fastEvalForEvenSampling(corrFunctions[pairNum], dt);
                    corrVal *= scale;
                    double binVal = histMap -> GetBinContent(globalBin);
                    if (corrVal == corrVal){ // not a nan
                        histMap -> SetBinContent(globalBin, binVal + corrVal);
                    }
                }
            }
        }
    }

    // bit of sanity checking
    for (int phiBin = 0; phiBin < this->numPhiBins_; phiBin++) {
        for (int thetaBin = 0; thetaBin < this->numThetaBins_; thetaBin++) {
            Int_t globalBin = (phiBin + 1) + (thetaBin + 1) * (this->numPhiBins_ + 2);
            if (histMap -> GetBinContent(globalBin) == -1000) {
                histMap -> SetBinContent(globalBin, 0);
            }
        }
    }

    // cleanup
    // if you're facing a memory leak, try making sure all the grCorr's
    // above got cleaned up correctly. As written, this should work...
    for(int i=0; i<corrFunctions.size(); i++){
        delete corrFunctions[i];
    }

    return histMap;

}