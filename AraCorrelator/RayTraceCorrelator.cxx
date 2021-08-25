//C/C++ includes
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include <stdexcept>
#include <math.h>

//ROOT includes
#include <TObject.h>
#include "TFile.h"
#include "TTree.h"
#include "AraGeomTool.h"
#include "RayTraceCorrelator.h"

// TODO:
// - Make sure unixTime is getting handled correctly

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
    auto araStationInfo_ = geomTool->getStationInfo(stationID_, unixTime);
    int numAnts = (int) araStationInfo_->getNumRFChans();
    int num_not_surface = 0;
    for(int ant=0; ant<numAnts; ant++){
        auto pol = araStationInfo_->getAntennaInfo(ant)->polType;
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
}

void RayTraceCorrelator::SetRadius(double radius){
    if(radius<0 || isnan(radius)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested radius (%e) is not supported\n", radius);
        throw std::invalid_argument(errorMessage);        
    }
    radius_ = radius;
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
    const std::string &tableDir,
    int unixTime
    ){

    char errorMessage[400];

    // initialize and sanitize the input immediately
    // afterwards, we can (safely!) only refer to private variables
    this->SetAngularConfig(angularSize);
    this->SetupStationInfo(stationID, unixTime);
    this->SetRadius(radius);

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
        tableDir.c_str(), stationID, iceModel, radius, angularSize );
    sprintf(reflecFileName, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_1.root",
        tableDir.c_str(), stationID, iceModel, radius, angularSize );
    
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

}
