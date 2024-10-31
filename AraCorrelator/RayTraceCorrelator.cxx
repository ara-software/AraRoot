//C/C++ includes
#include <iostream>
#include <numeric>
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
// #include "RayTraceCorrelator_detail.h"

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
    this->SetupStationInfo(stationID, numAntennas);
    this->SetAngularConfig(angularSize);
    this->SetRadius(radius);
    this->SetTablePaths(dirSolTablePath, refSolTablePath);

}

void RayTraceCorrelator::LoadTables(){
    
    // load the arrival time tables
    // this->LoadArrivalTimeTables(dirSolTablePath_, 0);
    // this->LoadArrivalTimeTables(refSolTablePath_, 1);
}
