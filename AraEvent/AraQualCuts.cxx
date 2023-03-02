//////////////////////////////////////////////////////////////////////////////
/////  AraQualCuts.h       ARA Quality cuts tool                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class imposing some event qualtiy cuts                 /////
//////////////////////////////////////////////////////////////////////////////

//C++ includes
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

//class definition includes
#include "AraQualCuts.h"

//AraRoot Includes
#include "araSoft.h"
#include "FFTtools.h"
#include "AraAntennaInfo.h"
#include "AraGeomTool.h"

//ROOT includes
#include "TGraph.h"

AraQualCuts * AraQualCuts::fgInstance=0;

AraQualCuts::AraQualCuts() 
{
    _VdeltaT=0.4;
    _HdeltaT=0.625;
    _VOffsetThresh=-20.;
    _HOffsetThresh=-12.;
    _OffsetBlocksTimeWindowCut=10.;
    //for the moment, this doesn't do anything intelligent...
}

AraQualCuts::~AraQualCuts() {
    //for the moment, this doesn't need to do anything intelligent
}

AraQualCuts*  AraQualCuts::Instance()
{
    //static function
    if(fgInstance)
        return fgInstance;

    fgInstance = new AraQualCuts();
    return fgInstance;
}

//! Returns if a real atri event has a quality problem
/*!
    \param ev the useful atri event pointer
    \return if the event has a block gap
*/
bool AraQualCuts::isGoodEvent(UsefulAtriStationEvent *realEvent)
{
    bool isGoodEvent=true;
    
    bool this_hasBlockGap = hasBlockGap(realEvent);
    bool this_hasTimingError = hasTimingError(realEvent);
    bool this_hasTooFewBlocks = hasTooFewBlocks(realEvent);
    bool this_hasTooFewSamples = hasTooFewSamples(realEvent) && !realEvent->isSoftwareTrigger();
    bool this_hasOffsetBlocks = false;
    if(!this_hasBlockGap && !this_hasTimingError && !this_hasTooFewBlocks && !this_hasTooFewSamples){
        this_hasOffsetBlocks = hasOffsetBlocks(realEvent);
    }
    
    bool this_hasFirstEventCorruption = hasFirstEventCorruption(realEvent);

    if(this_hasBlockGap 
        || this_hasTimingError 
        || this_hasTooFewBlocks
        || this_hasTooFewSamples
        || this_hasOffsetBlocks
        || this_hasFirstEventCorruption){
        isGoodEvent=false;
    }
    return isGoodEvent;
}

//! Returns if a real atri event has an offset block probelm
/*!
    \param realEvent the real atri event pointer
    \return does the event have offset blocks?
*/
bool AraQualCuts::hasOffsetBlocks(UsefulAtriStationEvent *realEvent)
{

    bool hasOffsetBlocks=false;

    /*
        This is currently only tuned for A2, and sort of for A3
        So "bounce out" if someone tries to use it for another station
    */
    if(realEvent->stationId!=ARA_STATION2 && realEvent->stationId!=ARA_STATION3){
        return hasOffsetBlocks;
    }
    int numStringsToCheck=4;

    if(realEvent->stationId==ARA_STATION3){

        // if string 4 has gone "bad" in A3 (which we think happened at run 1901)
        // then reduce the strings we scan over for a mistake
        if(realEvent->unixTime > 1387451885 ){
            numStringsToCheck=3;
        }
        _OffsetBlocksTimeWindowCut=20.; // bump this for A3 only; actually makes a little more sense. this says "anywhere within a block"
    }

    int nChanBelowThresh_V[4]={0};
    int nChanBelowThresh_H[4]={0};
    std::vector< std::vector< std::vector<double> > > maxTimeVec;
    maxTimeVec.resize(4);
    for(int string=0; string<4; string++){
        maxTimeVec[string].resize(2);
    }

    AraAntPol::AraAntPol_t Vpol = AraAntPol::kVertical;
    AraAntPol::AraAntPol_t Hpol = AraAntPol::kHorizontal;

    for(int chan=0; chan<16; chan++){
        TGraph* grRaw = realEvent->getGraphFromRFChan(chan); //get the waveform

        AraAntPol::AraAntPol_t this_pol = AraGeomTool::Instance()->getPolByRFChan(chan,realEvent->stationId);
        double deltaT; //interpolation time step
        double this_thresh; //the voltage threshold for a bad block

        if(this_pol==Vpol){
            deltaT=_VdeltaT;
            this_thresh=_VOffsetThresh;
        }
        else if(this_pol==Hpol){
            deltaT=_HdeltaT;
            this_thresh=_HOffsetThresh;
        }
        else{ //fallback to V (more conservative)
            deltaT=_VdeltaT;
            this_thresh=_VOffsetThresh;
        }
        //get interpolated waveform
        TGraph *grInt = FFTtools::getInterpolatedGraph(grRaw, deltaT); 
        //then, get the rolling mean graph
        TGraph *grMean = getRollingMean(grInt,SAMPLES_PER_BLOCK); //SAMPLES_PER_BLOCK=64, in araSoft.h
        double maxTime;
        double meanMax = getMax(grMean, &maxTime);
        // printf("Chan %d: maxTime %.2f and meanMax %.2f \n", chan, maxTime, meanMax);

        if(-1.*fabs(meanMax)<this_thresh){
            if(this_pol==Vpol){
                nChanBelowThresh_V[chan%4]+=1;
                maxTimeVec[chan%4][0].push_back(maxTime);
            }
            else if (this_pol==Hpol){
                nChanBelowThresh_H[chan%4]+=1;
                maxTimeVec[chan%4][1].push_back(maxTime);
            }
        }

        delete grMean;
        delete grInt;
        delete grRaw;
    }

    /* Check for offset block
        Criteria: at least 2 offset block strings. An offset block string is defined as having offset blocks in both Vpols and at least 1
        Hpol, and their offset time is within timeRangeCut for Vpol & Hpol resoectively.
    */

    double timeRange;
    int noffsetBlockString_startV=0;
    for(int string=0; string<numStringsToCheck; string++){
      // printf("String %d, nChanBelowThresh_V is %d \n",string,nChanBelowThresh_V[string]);
        if (nChanBelowThresh_V[string]==2){
            timeRange = *max_element(maxTimeVec[string][0].begin(), maxTimeVec[string][0].end())
                      - *min_element(maxTimeVec[string][0].begin(), maxTimeVec[string][0].end());
            // printf("    timeRange for string %d is %.2f \n", string, timeRange);
            if(timeRange<=_OffsetBlocksTimeWindowCut){
	           // printf("        nChanBelowThresh_H is %d \n",nChanBelowThresh_H[string]);
                if(nChanBelowThresh_H[string]>0){
                    timeRange = *max_element(maxTimeVec[string][1].begin(), maxTimeVec[string][1].end())
                              - *min_element(maxTimeVec[string][1].begin(), maxTimeVec[string][1].end());
                    // printf("            second timeRange for string %d is %.2f\n", string, timeRange);
                    if(timeRange <= _OffsetBlocksTimeWindowCut){
                        noffsetBlockString_startV++;
                        // printf("               noffsetBlockString_startV is %d \n",noffsetBlockString_startV);
                    }
                }
            }
        }
    }

    // also, check the reverse
    // where you look for at least hpol + 1 vpol
    // currently unused
    int noffsetBlockString_startH=0;

    if(noffsetBlockString_startV>1 || noffsetBlockString_startH>1){
        // printf("Yes! Has offset blocks!\n");
        hasOffsetBlocks=true;
    }
    return hasOffsetBlocks;
}

//! Returns the rolling mean graph with a window size samplePerBlock
/*!
    \param grInt an interpolated waveform
    \param samplePerBlock the window size
    \return TGraph the rolling mean graph
*/
TGraph* AraQualCuts::getRollingMean(TGraph *grInt, int samplePerBlock){
    int nSamp = grInt->GetN();
    double t0, t1, v0, v1;
    grInt->GetPoint(0, t0, v0);
    grInt->GetPoint(1, t1, v1);
    double wInt = t1 - t0;
 
    TGraph *grCrop;
    TGraph *grMean = new TGraph();
    for(int i=0; i<nSamp-samplePerBlock; i++){
        grCrop = FFTtools::cropWave(grInt, t0+i*wInt, t0+(i+samplePerBlock)*wInt);
        grMean->SetPoint(i, wInt*i, getMean(grCrop));
        delete grCrop;
    }
     return grMean;
}

//! Returns the max value (pos or neg) of a waveform
/*!
    \param gr the tgraph
    \param maxTime a pointer to the max time
    \return the max value (pos or neg) of a waveform
*/
double AraQualCuts::getMax(TGraph *gr, double *maxTime){
    double max=0.;
    double x,y;
    double _maxTime;
    for(int i=0; i<gr->GetN(); i++){
        gr->GetPoint(i,x,y);
        if(fabs(y)>fabs(max)){
            max = y;
            _maxTime = x;
        }
    }
    if(maxTime) *maxTime = _maxTime;
    return max;
}

//! Returns the mean value of a waveform
/*!
    \param gr the tgraph
    \return the mean of the waveform
*/
double AraQualCuts::getMean(TGraph *gr){
    double v, t, mean;
    mean=0;
    for(int i=0; i<gr->GetN(); i++){
        gr->GetPoint(i,t,v);
        mean+=v;
    }
    mean = mean / double(gr->GetN());
    return mean;
}

//! Returns if a raw atri event has a block gap
/*!
    \param rawEvent the raw atri event pointer
    \return if the event has a block gap
*/
bool AraQualCuts::hasBlockGap(RawAtriStationEvent *rawEvent)
{

    /*
        In normal digitizer readout, blocks are read out sequentially
        In the order "DDA1, DDA2, DDA3, DDA4"
        So if we read out three blocks, we'd see
        24 24 24 24 25 25 25 25 26 26 26 26
        If the readout is *not* sequential, we should not analyze the event
        So, we check to see if final block = first block + 4*num_blocks
    */

    bool hasBlockGap=false;
    int lenBlockVec = rawEvent->blockVec.size();
    int numDDA=DDA_PER_ATRI; //defined in araSoft.h
    int numBlocks = BLOCKS_PER_DDA; //defined in araSoft.h
    if(rawEvent->blockVec[lenBlockVec-1].getBlock() != (rawEvent->blockVec[0].getBlock() + lenBlockVec/numDDA -1 ) ){
        if( numBlocks-rawEvent->blockVec[0].getBlock() + rawEvent->blockVec[lenBlockVec-1].getBlock() != lenBlockVec/numDDA-1){
            hasBlockGap=true;
        }
    }
    return hasBlockGap;
}

//! Returns if a real atri event has a timing error
/*!
    \param realEvent the useful atri event pointer
    \return if the event has a timing error
*/
bool AraQualCuts::hasTimingError(UsefulAtriStationEvent *realEvent)
{

    /*
        In an analyzable waveform, later samples should have later times
        so we check to see if ever x_j < x_j+1
        which is acausal and will cause the interpolator to fail
    */

    bool hasTimingError=false;
    for(int chan=0; chan<realEvent->getNumRFChannels(); chan++){
        TGraph* gr = realEvent->getGraphFromRFChan(chan); //get the waveform
        Double_t *xVals = gr->GetX(); //get the time array
        for(int i=1; i<gr->GetN(); i++){
            if(xVals[i]<xVals[i-1]){
                hasTimingError=true;
                break;
            }
        }
        delete gr;
    }
    return hasTimingError;
}

//! Returns if a real atri event has two few blocks
/*!
    \param realEvent the useful atri event pointer
    \return if the event has two few blocks/samples to be analyzed
*/
bool AraQualCuts::hasTooFewBlocks(UsefulAtriStationEvent *realEvent)
{

    /*
        In an analyzable waveform, it should have at leat 1 fully read-out block (64 samples)
        (This is SAMPLES_PER_BLOCK from araSoft.h)
        If not, we shouldn't analyze this event
    */

    bool hasTooFewBlocks=false;
    for(int chan=0; chan<realEvent->getNumRFChannels(); chan++){
        TGraph* gr = realEvent->getGraphFromRFChan(chan); //get the waveform
        int N = gr->GetN();
        delete gr;
        if(N<SAMPLES_PER_BLOCK){
            hasTooFewBlocks=true;
            break;
        }
    }
    return hasTooFewBlocks;
}

//! Returns if a real atri event contains a waveform whose number of samples is less than 500.
/*!
    \param realEvent the useful atri event pointer
    \return if the event contains a waveform that has too few samples (<500) to be analyzed
*/
bool AraQualCuts::hasTooFewSamples(UsefulAtriStationEvent *realEvent)
{

    /*
        In an analyzable waveform, the number of samples should be greater than 500.
        If not, we shouldn't analyze this event
    */

    const int numSampThreshold = 500; 
    bool hasTooFewSamples=false;
    for(int chan=0; chan<realEvent->getNumRFChannels(); chan++){
        TGraph* gr = realEvent->getGraphFromRFChan(chan); //get the waveform
        int N = gr->GetN();
        delete gr;
        if(N<numSampThreshold){
            hasTooFewSamples=true;
            break;
        }
    }
    return hasTooFewSamples;
}

//! Returns if a real atri event has first event corruption issues
/*!
    \param rawEvent the raw atri event pointer
    \return if the event was one of the first four read out for some subset of A2 and A3 which are known to be corrupted
*/
bool AraQualCuts::hasFirstEventCorruption(RawAtriStationEvent *rawEvent)
{

    /*
        The first four events readout in A2 after unixTime 1448485911 are corrupted
        And should be marked "bad"

        And, we mark any of the first four events in A3.
    */

    bool hasFirstEventCorruption=false;

    if(rawEvent->stationId==ARA_STATION2
        && rawEvent->unixTime>=1448485911 
        && rawEvent->eventNumber<4)
    {
        hasFirstEventCorruption=true;
    }
    else if(rawEvent->stationId==ARA_STATION3
        && rawEvent->eventNumber<4)
    {
        hasFirstEventCorruption=true;
    }
    return hasFirstEventCorruption;
}

//! Returns the livetime configuration number corresponding to a given run number and station 
/*!
    \param runNumber the station run number
    \param stationId the station Id number 
    \return config the livetime configuration number 
*/
int AraQualCuts::getLivetimeConfiguration(const int runNumber, int stationId) 
{

    if(stationId == 100) // simplify ARA1 station id
      stationId = 1;
    
    int start, end, config;

    std::vector<int> configStart;
    std::vector<int> configNum;

    // read in log file
    char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
    char configLogFileName[256];
    sprintf(configLogFileName,"%s/share/livetimeConfigs/a%d_livetimeConfigs.txt",utilEnv,stationId);

    std::ifstream configLogFile(configLogFileName);
    if(!configLogFile.is_open())
      throw std::runtime_error("Livetime configuration log not found!");
    std::string line;
    while(getline(configLogFile, line)) {
      std::stringstream str(line);
      std::string word;
      std::vector<std::string> words;
      
      while(getline(str, word, ','))
        words.push_back(word);
      if(words.size() != 2)
        throw std::runtime_error("Livetime config log file not formatted correctly! \
                                  \nSee AraEvent/livetimeConfigs/README.md");
      if(words[0] == "RunNo") // header, skip this line
        continue;
      start = std::stoi(words[0]);  
      config = std::stoi(words[1]);
      
      // check for unexpected data types
      //// check for entries like: "1.5" and "11x" that might have silently converted to int
      if(std::to_string(start) != words[0] || std::to_string(config) != words[1]) // should be able to convert back fine if this was an integer
        throw std::runtime_error("Unexpected data type in livetime config log file! \
                                 \nSee AraEvent/livetimeConfigs/README.md");
      //// check for negative entries
      if(start < 0 || config < 0)
        throw std::runtime_error("Livetime config log file has a negative entry! \
                                 \nSee AraEvent/livetimeConfigs/README.md");

      // if everything looks okay append and move on!
      configStart.push_back(start);
      configNum.push_back(config);
    }
    configLogFile.close();

    // find the right configuration
    for(unsigned int i = 0; i < configStart.size(); ++i) {
      start = configStart[i];
      if(i+1 == configStart.size()) // if this is the last config start assume all future runs are part of the last config
        end = int(1e10);
      else // if this isn't the last config start, assume this config continues until the next config begins 
        end = configStart[i+1]; 

      if(end <= start)
        throw std::runtime_error("Something is wrong in the livetime configuration log \
                                  file: " + std::string(configLogFileName) +
                                  "\nSee AraEvent/livetimeConfigs/README.md");

      if(runNumber >= start && runNumber < end) {
        config = configNum[i]; 
        break;
      }
    }

    return config;
}
