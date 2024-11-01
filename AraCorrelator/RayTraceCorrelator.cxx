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
#include "RayTraceCorrelator_detail.h"

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

int RayTraceCorrelator::ConvertAnglesToTH2DGlobalBin(double theta, double phi){

    if(abs(theta) > 90 || isnan(theta)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested theta angle (%e) is not supported. Range should be -90 to 90\n", theta);
        throw std::invalid_argument(errorMessage);
    }

    if(abs(phi)>180 || isnan(phi)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested phi angle (%e) is not supported. Range should be -180 to 180\n", theta);
        throw std::invalid_argument(errorMessage);
    }

    int globalBin = this->dummyMap->FindBin(phi, theta);
    return globalBin;
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

    std::map<int, TH2D> this_arrivalTimes;
    std::map<int, TH2D> this_arrivalThetas;
    std::map<int, TH2D> this_arrivalPhis;
    std::map<int, TH2D> this_launchThetas;
    std::map<int, TH2D> this_launchPhis;

    for(int i=0; i<numAntennas_; i++){
        std::   stringstream ss;

        ss.str(""); ss << "arrival_time_ch_" << i;
        TH2D *hArrivalTime = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_arrivalTimes[i] = *hArrivalTime; // de ref the pointer; give me the memory

        ss.str(""); ss << "arrival_theta_ch" << i;
        TH2D *hArrivalTheta = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_arrivalThetas[i] = *hArrivalTheta; // de ref the pointer; give me the memory

        ss.str(""); ss << "arrival_phi_ch" << i;
        TH2D *hArrivalPhi = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_arrivalPhis[i] = *hArrivalPhi; // de ref the pointer; give me the memory

        ss.str(""); ss << "launch_theta_ch" << i;
        TH2D *hLaunchTheta = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_launchThetas[i] = *hLaunchTheta;// de ref the pointer; give me the memory
        
        ss.str(""); ss << "launch_phi_ch" << i;
        TH2D *hLaunchPhi = (TH2D*)((TH2D*)infile->Get(ss.str().c_str())->Clone()); // clone, so this survives the file closure
        this_launchPhis[i] = *hLaunchPhi; // de ref the pointer; give me the memory
    }
    arrivalTimes_[solNum] = this_arrivalTimes;
    arrivalThetas_[solNum] = this_arrivalThetas;
    arrivalPhis_[solNum] = this_arrivalPhis;
    launchThetas_[solNum] = this_launchThetas;
    launchPhis_[solNum] = this_launchPhis;

    // close up
    infile->Close();

    // useful to have this around for debugging
    // for(int i=0; i<numAntennas_; i++){
    //     std::cout<<"i"<<i<<std::endl;
    //     TCanvas *c = new TCanvas("", "", 1100, 850);
    //     arrivalTimes_.at(solNum).at(i).Draw("colz"); // standard colz projection
    //     char title[500];
    //     sprintf(title,"timing_ant%d_%d.png", i,solNum);
    //     c->SaveAs(title);
    //     delete c;
    // }
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

            for(int phiBin = dummyMap->GetXaxis()->GetFirst(); phiBin <= dummyMap->GetXaxis()->GetLast(); phiBin++){
                for(int thetaBin = dummyMap->GetYaxis()->GetFirst(); thetaBin <= dummyMap->GetYaxis()->GetLast(); thetaBin++){

                    int globalBin  = dummyMap->GetBin(phiBin, thetaBin);
                    double arrival_time1 = arrivalTimes_.at(solNum).at(ant1).GetBinContent(globalBin);
                    double arrival_time2 = arrivalTimes_.at(solNum).at(ant2).GetBinContent(globalBin);
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
        const int numPoints = corrFunctions[pairNum].GetN(); // this won't change for as long as it's alive
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
            if(p0<0 || p0>=numPoints){
                // outside the region of support, force it to zero
                corrVal = 0;
            }
            else{
                corrVal = (yVals[p0 + 1] - yVals[p0]) * ((dt - xVals[p0]) / (xVals[p0 + 1]-xVals[p0])) + yVals[p0];
            }
            summedCorr[globalBin]+=corrVal *scale;

        }
    }

    TH2D histMap("", "", this->numPhiBins_, -180, 180, this->numThetaBins_, -90, 90);

    for(int iterBin=0; iterBin < numGlobalBins; iterBin++){
        histMap.SetBinContent(iterBin, summedCorr[iterBin]);
    }

    return histMap;
}

int RayTraceCorrelator::ValidateAnglesGetGlobalBinNumber(double theta, double phi){
    if(abs(theta) > 90 || isnan(theta)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested theta angle (%e) is not supported. Range should be -90 to 90\n", theta);
        throw std::invalid_argument(errorMessage);
    }

    if(abs(phi)>180 || isnan(phi)){
        char errorMessage[400];
        sprintf(errorMessage,"Requested phi angle (%e) is not supported. Range should be -180 to 180\n", theta);
        throw std::invalid_argument(errorMessage);
    }

    int globalBin  = dummyMap->FindBin(phi, theta);
    return globalBin;
}

double RayTraceCorrelator::LookupArrivalTime(
    int ant, int solNum,
    double theta, double phi
){
    int globalBin = ValidateAnglesGetGlobalBinNumber(theta, phi);
    double arrival_time = arrivalTimes_.at(solNum).at(ant).GetBinContent(globalBin);
    return arrival_time;
}

void RayTraceCorrelator::LookupArrivalAngles(
    int ant, int solNum,
    double theta, double phi,
    double &arrivalTheta, double &arrivalPhi
){
    int globalBin = ValidateAnglesGetGlobalBinNumber(theta, phi);
    arrivalTheta = arrivalThetas_.at(solNum).at(ant).GetBinContent(globalBin);
    arrivalPhi = arrivalPhis_.at(solNum).at(ant).GetBinContent(globalBin);
}

void RayTraceCorrelator::LookupLaunchAngles(
    int ant, int solNum,
    double theta, double phi,
    double &launchTheta, double &launchPhi
){
    int globalBin = ValidateAnglesGetGlobalBinNumber(theta, phi);
    launchTheta = launchThetas_.at(solNum).at(ant).GetBinContent(globalBin);
    launchPhi = launchPhis_.at(solNum).at(ant).GetBinContent(globalBin);
}