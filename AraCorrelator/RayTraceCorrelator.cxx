//C/C++ includes
#include <iostream>
#include <numeric>
#include <stdio.h>
#include <sys/stat.h>
#include <stdexcept>
#include <sstream>
#include <math.h>

//ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TCanvas.h"

// AraRoot includes
#include "AraGeomTool.h"
#include "RayTraceCorrelator.h"
// #include "RayTraceCorrelator_detail.h"

RayTraceCorrelator::RayTraceCorrelator(int stationID,
    int numAntennas,
    double radius, 
    double angularSize,
    const std::string &dirSolTablePath, 
    const std::string &refSolTablePath
    ){

    // initialize and sanitize the input immediately
    // afterwards, we can (safely!) only refer to private variables
    this->SetupStationInfo(stationID, numAntennas);
    this->SetAngularConfig(angularSize);
    this->SetRadius(radius);
    this->SetTablePaths(dirSolTablePath, refSolTablePath);

}

RayTraceCorrelator::~RayTraceCorrelator()
{
	//Default destructor
}

void RayTraceCorrelator::SetupStationInfo(int stationID, int numAntennas) { 
    char errorMessage[400];

    // set station ID
    if((stationID>5 || stationID<0 || isnan(stationID)) && stationID!=100){ // testbed through A5 is supported, as well as 2018 redeployment of A1 (stationID=100)
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
    dummyMap = std::shared_ptr<TH2D>(new TH2D("","",numPhiBins_, -180, 180, numThetaBins_, -90, 90));
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

void RayTraceCorrelator::LoadTables(){
    
    // load the arrival time tables
    this->LoadArrivalTimeTables(dirSolTablePath_, 0);
    this->LoadArrivalTimeTables(refSolTablePath_, 1);
}

void RayTraceCorrelator::LoadArrivalTimeTables(const std::string &filename, int solNum){
    char errorMessage[400];
    
    // try to open the file
    TFile * infile = TFile::Open(filename.c_str(), "READ");
    if(!infile){
        sprintf(errorMessage, "Opening of the table (%s) was unsuccessful\n",filename.c_str());
        throw std::runtime_error(errorMessage);
    }
    
    std::map<int, TH2D> this_arrvialTimes;
    std::map<int, TH2D> this_arrivalThetas;
    std::map<int, TH2D> this_arrivalPhis;
    std::map<int, TH2D> this_launchThetas;
    std::map<int, TH2D> this_launchPhis;

    for(int i=0; i<numAntennas_; i++){
        std::   stringstream ss;

        ss.str(""); ss << "arrival_time_ch_" << i;
        TH2D *hArrivalTime = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_arrvialTimes[i] = *hArrivalTime; // de ref the pointer; give me the memory

        ss.str(""); ss << "arrival_theta_ch" << i;
        TH2D *hArrivalTheta = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_arrivalThetas[i] = *hArrivalTheta; // wrap in unique ptr for safety

        ss.str(""); ss << "arrival_phi_ch" << i;
        TH2D *hArrivalPhi = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_arrivalThetas[i] = *hArrivalPhi; // wrap in unique ptr for safety

        ss.str(""); ss << "launch_theta_ch" << i;
        TH2D *hLaunchTheta = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_launchThetas[i] = *hLaunchTheta; // wrap in unique ptr for safety

        ss.str(""); ss << "launch_phi_ch" << i;
        TH2D *hLaunchPhi = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_launchPhis[i] = *hLaunchPhi; // wrap in unique ptr for safety
    }
    arrvialTimes_[solNum] = this_arrvialTimes;
    arrivalThetas_[solNum] = this_arrivalThetas;
    arrivalPhis_[solNum] = this_arrivalPhis;
    launchThetas_[solNum] = this_launchThetas;
    launchPhis_[solNum] = this_launchPhis;

    // close up
    infile->Close();

    // for(int i=0; i<numAntennas_; i++){
    //     std::cout<<"i"<<i<<std::endl;
    //     TCanvas *c = new TCanvas("", "", 1100, 850);
    //     arrvialTimes_.at(solNum).at(i).Draw("colz"); // standard colz projection
    //     char title[500];
    //     sprintf(title,"timing_ant%d_%d.png", i,solNum);
    //     c->SaveAs(title);
    //     delete c;
    // }

}