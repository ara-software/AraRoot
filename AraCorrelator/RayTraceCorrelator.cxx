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

void RayTraceCorrelator::SetupStationInfo(int stationID, int numAntennas) { 
    char errorMessage[400];

    // set station ID
    if(stationID>5 || stationID<0 || isnan(stationID)){ // testbed through A5 is supported
        sprintf(errorMessage,"Requested station (%d) is not supported\n", stationID);
        throw std::invalid_argument(errorMessage);
    }
    stationID_ = stationID;

    if(numAntennas < 1 || isnan(numAntennas)){
        sprintf(errorMessage,"Number of antennas (%d) is not supported\n", numAntennas);
        throw std::invalid_argument(errorMessage);
    }
    numAntennas_ = numAntennas;
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

void RayTraceCorrelator::SetTablePaths(const std::string &dirPath, const std::string &refPath){

    char errorMessage[400];
    struct stat buffer;

    // throw errors if these files don't exist
    bool dirFileExists = (stat(dirPath.c_str(), &buffer) == 0);
    if (!dirFileExists ){
        sprintf(errorMessage,"Direct solution arrival times table is missing (dir file exists %d) ", dirFileExists);
        throw std::runtime_error(errorMessage);       
    }
    dirSolTablePath_ = dirPath;

    bool refFileExists = (stat(refPath.c_str(), &buffer) == 0);
    if (!refFileExists){
        sprintf(errorMessage,"Reflected/refracted solution arrival times table is missing (dir file exists %d) ", refFileExists);
        throw std::runtime_error(errorMessage);       
    }
    refSolTablePath_ = refPath;
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
    int numAntennas,
    double radius, 
    double angularSize,
    const std::string &dirSolTablePath, 
    const std::string &refSolTablePath
    ){

    // initialize and sanitize the input immediately
    // afterwards, we can (safely!) only refer to private variables
    this->SetAngularConfig(angularSize);
    this->SetupStationInfo(stationID, numAntennas);
    this->SetRadius(radius);
    this->SetTablePaths(dirSolTablePath, refSolTablePath);

}

void RayTraceCorrelator::LoadTables(){
    char errorMessage[400];
    
    // load the arrival time tables
    this->ConfigureArrivalTimesVector();
    this->LoadArrivalTimeTables(dirSolTablePath_, 0);
    this->LoadArrivalTimeTables(refSolTablePath_, 1);
}

// FIXME
std::map< int, std::vector<int> > RayTraceCorrelator::SetupPairs(
    int stationID,
    AraGeomTool *geomTool,
    AraAntPol::AraAntPol_t polSelection,
    std::vector<int> excludedChannels){
    
    // first, figure out what set of antennas is viable to form pairs
    std::vector < int > viableAntennas;
    
    auto araStationInfo = geomTool->getStationInfo(stationID);
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
    std::map<int, double> weights,
    bool applyHilbertEnvelope
    ){

    char errorMessage[400];

    // first, sort out the weights to apply to each pair
    if(weights.size()>0){
        // if the user provided weights, make sure they provided the right number
        if(weights.size()!=pairs.size()){
            sprintf(errorMessage,"Mismatch in size of provided weights (%d) and provided pairs (%d)\n",weights.size(), pairs.size());
            throw std::invalid_argument(errorMessage);
        }
    }
    else{
        // otherwise, assume the user wanted equal weighting; which means 1/num_pairs
        for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
            int pairNum = iter->first;
            weights[pairNum] = 1./double(pairs.size());
        }
    }
        
    // first, calculate all of the correlation functions
    // for performance reasons, it's actually better to store
    // the correlation functions as a vector
    std::vector<TGraph*> corrFunctions;
    for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
        int pairNum = iter->first;
        int ant1 = iter->second[0];
        int ant2 = iter->second[1];

        // make sure these antennas are in the waveforms map
        auto gr1_iter = interpolatedWaveforms.find(ant1);
        auto gr2_iter = interpolatedWaveforms.find(ant2);
        if(gr1_iter==interpolatedWaveforms.end()){
            sprintf(errorMessage,
                    "Antenna %d in pair %d is not in the supplied waveforms\n",
                    ant1, pairNum);
            throw std::invalid_argument(errorMessage);
        }
        if(gr2_iter==interpolatedWaveforms.end()){
            sprintf(errorMessage,
                    "Antenna %d in pair %d is not in the supplied waveforms\n",
                    ant2, pairNum);
            throw std::invalid_argument(errorMessage);
        }

        // get the correlation function
        TGraph *grCorr = getCorrelationGraph_WFweight(
            gr1_iter->second, 
            gr2_iter->second
            );

        // store the correlation function, with a hilbert envelope applied (if requested)
        if(applyHilbertEnvelope){
            TGraph *grCorrHil = FFTtools::getHilbertEnvelope(grCorr);
            corrFunctions.push_back(grCorrHil);
            delete grCorr;
        }
        else{
            corrFunctions.push_back(grCorr);
        }
    }

    // create output histogram
    TH2D *histMap = new TH2D("", "", 
        this->numPhiBins_, -180, 180, 
        this->numThetaBins_, -90, 90
    );

    // now, make the map
    for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
        int pairNum = iter->first;
        int ant1 = iter->second[0];
        int ant2 = iter->second[1];

        // get the weight for this pair
        auto weight_iter = weights.find(pairNum);
        if(weight_iter==weights.end()){
            sprintf(errorMessage,"Weights for pair %d not found\n",pairNum);
            throw std::invalid_argument(errorMessage);
        }
        double scale = weight_iter->second;

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