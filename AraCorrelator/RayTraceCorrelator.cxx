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
#include "RayTraceCorrelator_detail.h"

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

void RayTraceCorrelator::ConvertAngleToBins(double theta, double phi, 
    int &thetaBin, int &phiBin
    ){

    if(abs(theta) > 91 || isnan(theta)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested theta angle (%e) is not supported. Range should be -91 to 91\n", theta);
        throw std::invalid_argument(errorMessage);
    }

    if(abs(phi)>181 || isnan(phi)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested phi angle (%e) is not supported. Range should be -181 to 181\n", theta);
        throw std::invalid_argument(errorMessage);
    }

    double angularSize = this->GetAngularSize();
    thetaBin = int((theta + 90. - (0.5 * angularSize))/angularSize);
    phiBin = int((phi + 180. - (0.5 * angularSize))/angularSize);
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


void RayTraceCorrelator::ConfigureArrivalVectors(){
    arrivalTimes_.resize(2);
    arrivalThetas_.resize(2);
    arrivalPhis_.resize(2);
    launchThetas_.resize(2);
    launchPhis_.resize(2);    
    for(int sol=0; sol<2; sol++){
        arrivalTimes_[sol].resize(numThetaBins_);
        arrivalThetas_[sol].resize(numThetaBins_);
        arrivalPhis_[sol].resize(numThetaBins_);
        launchThetas_[sol].resize(numThetaBins_);
        launchPhis_[sol].resize(numThetaBins_);        
        for(int thetaBin=0; thetaBin<numThetaBins_; thetaBin++){
            arrivalTimes_[sol][thetaBin].resize(numPhiBins_);
            arrivalThetas_[sol][thetaBin].resize(numPhiBins_);
            arrivalPhis_[sol][thetaBin].resize(numPhiBins_);
            launchThetas_[sol][thetaBin].resize(numPhiBins_);
            launchPhis_[sol][thetaBin].resize(numPhiBins_);            
        }
        for(int thetaBin=0; thetaBin<numThetaBins_; thetaBin++){
            for(int phiBin=0; phiBin<numPhiBins_; phiBin++){
                arrivalTimes_[sol][thetaBin][phiBin].resize(numAntennas_);
                arrivalThetas_[sol][thetaBin][phiBin].resize(numAntennas_);
                arrivalPhis_[sol][thetaBin][phiBin].resize(numAntennas_);
                launchThetas_[sol][thetaBin][phiBin].resize(numAntennas_);
                launchPhis_[sol][thetaBin][phiBin].resize(numAntennas_);                
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
    double phi, theta;
    double arrivalTime, arrivalTheta, arrivalPhi, launchTheta, launchPhi;
    tTree -> SetBranchAddress("ant", & ant);
    tTree -> SetBranchAddress("phiBin", & phiBin);
    tTree -> SetBranchAddress("thetaBin", & thetaBin);
    tTree -> SetBranchAddress("arrivalTime", & arrivalTime);
    tTree -> SetBranchAddress("arrivalTheta", & arrivalTheta);
    tTree -> SetBranchAddress("arrivalPhi", & arrivalPhi);
    tTree -> SetBranchAddress("launchTheta", & launchTheta);
    tTree -> SetBranchAddress("launchPhi", & launchPhi);    
    tTree -> SetBranchAddress("phi", & phi);
    tTree -> SetBranchAddress("theta", & theta);

    // and put those values into the vector
    int nEntries = tTree -> GetEntries();
    for (int i = 0; i < nEntries; i++) {
        tTree -> GetEntry(i);
        arrivalTimes_[solNum][thetaBin][phiBin][ant] = arrivalTime;
        arrivalThetas_[solNum][thetaBin][phiBin][ant] = arrivalTheta;
        arrivalPhis_[solNum][thetaBin][phiBin][ant] = arrivalPhi;
        launchThetas_[solNum][thetaBin][phiBin][ant] = launchTheta;
        launchPhis_[solNum][thetaBin][phiBin][ant] = launchPhi;        
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
    this->ConfigureArrivalVectors();
    this->LoadArrivalTimeTables(dirSolTablePath_, 0);
    this->LoadArrivalTimeTables(refSolTablePath_, 1);
}



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

std::vector<TGraph> RayTraceCorrelator::GetCorrFunctions(
    std::map<int, std::vector<int> > pairs,
    std::map<int, TGraph*> interpolatedWaveforms,
    bool applyHilbertEnvelope
    ){

    char errorMessage[400];

    // first, calculate all of the correlation functions
    // for performance reasons, it's actually better to store
    // the correlation functions as a vector
    std::vector<TGraph> corrFunctions;
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

        // It's a bit inefficient to reget the normalized traces every time,
        // and not push this into the waveform getter. But shouldn't be a huge waste.
        auto graph1_normed = getNormalisedGraphByRMS(gr1_iter->second);
        auto graph2_normed = getNormalisedGraphByRMS(gr2_iter->second);

        // get the correlation graph
        std::unique_ptr<TGraph> grCorr{FFTtools::getCorrelationGraph(graph1_normed.get(), graph2_normed.get())};

        // store the correlation function, with a hilbert envelope applied (if requested)
        if(applyHilbertEnvelope){
            std::unique_ptr<TGraph> grCorrHil{FFTtools::getHilbertEnvelope(grCorr.get())};

            // drop this into a non-pointered object
            // unique pointer will clean up grCorrHil for us!
            std::vector<double> tVals;
            std::vector<double> vVals;
            double *xvals = grCorrHil->GetX();
            double *yvals = grCorrHil->GetY();
            int nPoints = grCorrHil->GetN();
            for (int isamp=0; isamp<nPoints; isamp++){
                tVals.push_back(xvals[isamp]);
                vVals.push_back(yvals[isamp]);
            }
            TGraph grCorrHilOut(nPoints, &tVals[0], &vVals[0]);
            corrFunctions.push_back(grCorrHilOut);
        }
        else{
            
            // drop this into a non-pointered object
            // unique pointer will clean up grCorr for us
            std::vector<double> tVals;
            std::vector<double> vVals;
            double *xvals = grCorr->GetX();
            double *yvals = grCorr->GetY();
            int nPoints = grCorr->GetN();
            for (int isamp=0; isamp<nPoints; isamp++){
                tVals.push_back(xvals[isamp]);
                vVals.push_back(yvals[isamp]);
            }            
            TGraph grCorrOut(nPoints, &tVals[0], &vVals[0]);
            corrFunctions.push_back(grCorrOut);
        }
    }
    return corrFunctions;
}

std::pair< 
    std::vector< std::vector< std::vector< int > > >,
    std::vector< std::vector< std::vector< double > > > > RayTraceCorrelator::GetArrivalDelays(std::map<int, std::vector<int > > pairs){

    std::vector< std::vector< std::vector< double > > > delays;
    std::vector< std::vector< std::vector< int > > > bins;

    for(int solNum=0; solNum<2; solNum++){

        std::vector< std::vector<double> > delays_this_sol;
        std::vector< std::vector<int> > bins_this_sol;

        for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
            int pairNum = iter->first;
            int ant1 = iter->second[0];
            int ant2 = iter->second[1];

            std::vector<double> this_delays;
            std::vector<int> this_global_bins;

            for(int phiBin=0; phiBin < this->numPhiBins_; phiBin++){
                for(int thetaBin=0; thetaBin < this->numThetaBins_; thetaBin++){
                    
                    int globalBin = (phiBin + 1) + (thetaBin + 1) * (this->numPhiBins_ + 2);

                    double arrival_time1 = LookupArrivalTimes(ant1, solNum, thetaBin, phiBin);
                    double arrival_time2 = LookupArrivalTimes(ant2, solNum, thetaBin, phiBin);
                    double dt = arrival_time1 - arrival_time2;

                    // sanity check
                    if (arrival_time1 < -100 || arrival_time2 < -100) {
                        dt = -1E6;  // large negative number
                    }

                    this_global_bins.push_back(globalBin);
                    this_delays.push_back(dt);
                }
            }

            // follow this method (https://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of)
            // for reordering the vectors

            // first, work out the permutation p that is required to reorder 
            // the delay array from smallest to largest
            std::vector<std::size_t> p(this_delays.size());
            std::iota(p.begin(), p.end(), 0);
            std::sort(p.begin(), p.end(),
                    [&](std::size_t i, std::size_t j) { return this_delays[i] < this_delays[j]; });

            // and now reorder both the bin indices and the delays by that permutation
            std::vector<int> sorted_global_bins(this_global_bins.size());
            std::transform(p.begin(), p.end(), sorted_global_bins.begin(),
                [&](std::size_t i){return this_global_bins[i];});
            std::vector<double> sorted_delays(this_delays.size());
            std::transform(p.begin(), p.end(), sorted_delays.begin(),
                [&](std::size_t i){return this_delays[i];});

            delays_this_sol.push_back(sorted_delays);
            bins_this_sol.push_back(sorted_global_bins);

        }
        delays.push_back(delays_this_sol);
        bins.push_back(bins_this_sol);
    }

    std::pair< 
        std::vector< std::vector< std::vector< int > > >,
        std::vector< std::vector< std::vector< double > > > > delay_info;

    delay_info.first = bins;
    delay_info.second = delays;
    return delay_info;

}


void RayTraceCorrelator::LookupArrivalAngles(
    int ant, int solNum,
    int thetaBin, int phiBin,
    double &arrivalTheta, double &arrivalPhi
){
    arrivalTheta = this->arrivalThetas_[solNum][thetaBin][phiBin][ant];
    arrivalPhi = this->arrivalPhis_[solNum][thetaBin][phiBin][ant];
}

void RayTraceCorrelator::LookupLaunchAngles(
    int ant, int solNum,
    int thetaBin, int phiBin,
    double &launchTheta, double &launchPhi
){
    launchTheta = this->launchThetas_[solNum][thetaBin][phiBin][ant];
    launchPhi = this->launchPhis_[solNum][thetaBin][phiBin][ant];
}

double RayTraceCorrelator::LookupArrivalTimes(
    int ant, int solNum,
    int thetaBin, int phiBin
){
    return this->arrivalTimes_[solNum][thetaBin][phiBin][ant];
}


TH2D RayTraceCorrelator::GetInterferometricMap(
    const std::map<int, std::vector<int> > pairs,
    const std::vector<TGraph> &corrFunctions,
    const std::pair< std::vector< std::vector< std::vector< int > > >, std::vector< std::vector< std::vector< double > > > > &arrivalDelays,
    const int solNum,
    std::map<int, double> weights
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

    // make sure number of pairs agrees with size of corrFunctions
    if(pairs.size()!=corrFunctions.size()){
        sprintf(errorMessage,"Mismatch in size of provided corr functions (%d) and provided pairs (%d)\n",corrFunctions.size(), pairs.size());
        throw std::invalid_argument(errorMessage);
    }

    const int numGlobalBins = int((this->numThetaBins_+2)*(this->numPhiBins_+2));

    // a variable to store the summed correlation value
    // this needs to have the same size as the eventual TH2D
    std::vector<double> summedCorr(numGlobalBins, 0);

    // and a variable to control the loop over bins we actually populated
    // this is NOT the same thing as the number of bins in the TH2D
    // since the number of bins in the 2D hist is different than the number of bins we have cached
    // because of overflow and underflow bins
    const int nGlobalBinsToIter = arrivalDelays.first[0][0].size();

    // now, make the map
    for(auto iter = pairs.begin(); iter != pairs.end(); ++iter){
        int pairNum = iter->first;

        // get the weight for this pair
        const double scale = weights.at(pairNum);

        // for performance reasons, we do the correlation right here
        // so we draw out the x and y values of the correlation function to be used later
        int numPoints = corrFunctions[pairNum].GetN();
        auto xVals = corrFunctions[pairNum].GetX();
        auto yVals = corrFunctions[pairNum].GetY();
        double dx = xVals[1] - xVals[0];

        // get the global bins and delays for this solution
        auto& it_bins = arrivalDelays.first[solNum][pairNum];
        auto& it_delays = arrivalDelays.second[solNum][pairNum];

        int p0 = 0;
        double corrVal = 0;

        for(int iterBin = 0; iterBin < nGlobalBinsToIter; iterBin++){
            
            int globalBin = it_bins[iterBin];
            double dt = it_delays[iterBin];

            p0 = int((dt - xVals[0]) / dx);
            if (p0 < 0) p0 = 0;
            if (p0 >= numPoints) p0 = numPoints - 2;
            corrVal = (yVals[p0 + 1] - yVals[p0]) * ((dt - xVals[p0]) / (xVals[p0 + 1]-xVals[p0])) + yVals[p0];
            summedCorr[globalBin]+=corrVal *scale;

        }
    }

    TH2D histMap("", "", this->numPhiBins_, -180, 180, this->numThetaBins_, -90, 90);

    for(int iterBin=0; iterBin < numGlobalBins; iterBin++){
        histMap.SetBinContent(iterBin, summedCorr[iterBin]);
    }

    return histMap;
}
