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
	_NumOffsetBlocksCut=11;
	_OffsetBlocksTimeWindowCut=40.;
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
	bool this_hasOffsetBlocks = false;
	if(!this_hasBlockGap && !this_hasTimingError && !this_hasTooFewBlocks){
		this_hasOffsetBlocks = hasOffsetBlocks(realEvent);
	}
	
	bool this_hasA2FirstEventCorruption = hasA2FirstEventCorruption(realEvent);

	if(this_hasBlockGap 
		|| this_hasTimingError 
		|| this_hasTooFewBlocks 
		|| this_hasOffsetBlocks
		|| this_hasA2FirstEventCorruption){
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
		This is currently only tuned for A2
		So "bounce out" if someone tries to use it for another station
	*/
	if(realEvent->stationId!=ARA_STATION2){
		return hasOffsetBlocks;
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

	for(int chan=0; chan<realEvent->getNumRFChannels(); chan++){
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

	double timeRange;
	int noffsetBlockString=0;
	for(int string=0; string<4; string++){
		if (nChanBelowThresh_V[string]==2){
			timeRange = *max_element(maxTimeVec[string][0].begin(), maxTimeVec[string][0].end())
					  - *min_element(maxTimeVec[string][0].begin(), maxTimeVec[string][0].end());
		}
		if(timeRange<_OffsetBlocksTimeWindowCut){
			if(nChanBelowThresh_H[string]>0){
				timeRange = *max_element(maxTimeVec[string][0].begin(), maxTimeVec[string][0].end())
						  - *min_element(maxTimeVec[string][0].begin(), maxTimeVec[string][0].end());
				if(timeRange < _OffsetBlocksTimeWindowCut){
					noffsetBlockString++;
				}
			}
		}
	}

	if(noffsetBlockString>1){
		hasOffsetBlocks=true;
	}
	return hasOffsetBlocks;
}



// //! Returns if a real atri event has an offset block probelm
// /*!
// 	\param realEvent the real atri event pointer
// 	\return does the event have offset blocks?
// */
// bool AraQualCuts::hasOffsetBlocks(UsefulAtriStationEvent *realEvent)
// {

// 	/*
// 		We want to loop over every waveform
// 		For every waveform, figure out if it has a "offset block"
// 		And record when it happens
// 		Then, we will check at the end if we have >_NumOffsetBlocksCut offset blocks
// 		And we will them check if they all happen in the same time window (_OffsetBlocksTimeWindowCut)
// 		If so, this is an offset block event
// 	*/

// 	bool hasOffsetBlocks=false;
// 	int nChanBelowThresh=0;
// 	std::vector<double> maxTimeVec;

// 	AraAntPol::AraAntPol_t Vpol = AraAntPol::kVertical;
// 	AraAntPol::AraAntPol_t Hpol = AraAntPol::kHorizontal;

// 	//loop over all RF chans
// 	for(int chan=0; chan<realEvent->getNumRFChannels(); chan++){

// 		TGraph* grRaw = realEvent->getGraphFromRFChan(chan); //get the waveform

// 		AraAntPol::AraAntPol_t this_pol = AraGeomTool::Instance()->getPolByRFChan(chan,realEvent->stationId);

// 		//set up two polarization parameters
// 		//the interpolation time step (V vs H)
// 		//and the voltage threshold for a bad block

// 		double deltaT; //interpolation time step
// 		double this_thresh; //the voltage threshold for a bad block

// 		if(this_pol==Vpol){
// 			deltaT=_VdeltaT;
// 			this_thresh=_VOffsetThresh;
// 		}
// 		else if(this_pol==Hpol){
// 			deltaT=_HdeltaT;
// 			this_thresh=_HOffsetThresh;
// 		}
// 		else{ //fallback to V (more conservative)
// 			deltaT=.5;
// 			this_thresh=_VOffsetThresh;
// 		}

// 		//get interpolated waveform
// 		TGraph *grInt = FFTtools::getInterpolatedGraph(grRaw, deltaT); 
// 		//then, get the rolling mean graph
// 		TGraph *grMean = getRollingMean(grInt,SAMPLES_PER_BLOCK); //SAMPLES_PER_BLOCK=64, in araSoft.h
// 		//get the max of the rolling mean, and when it occurs
// 		double maxTime;
// 		double meanMax = getMax(grMean, &maxTime);
// 		//check to see if we have a threshold violation and record when it happened
// 		if(-1.*fabs(meanMax)<this_thresh){
// 			nChanBelowThresh++;
// 			maxTimeVec.push_back(maxTime);
// 		}
// 		delete grMean;
// 		delete grInt;
// 		delete grRaw;
// 	}
// 	//check all the threshold violations we found, and see if they all happen near eachother
// 	//where "near eachother" means within the _OffsetBlocksTimeWindowCut amount of time
// 	double timeRange;
// 	if(nChanBelowThresh>=_NumOffsetBlocksCut){
// 		timeRange = *max_element(maxTimeVec.begin(), maxTimeVec.end()) - *min_element(maxTimeVec.begin(), maxTimeVec.end());
// 		if(timeRange < _OffsetBlocksTimeWindowCut)
// 			hasOffsetBlocks=true;
// 	}
// 	return hasOffsetBlocks;
// }

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

//! Returns if a real atri event has A2 corruption issues
/*!
	\param rawEvent the raw atri event pointer
	\return if the event was one of the first four readout in A2 after unixtime 1448485911
*/
bool AraQualCuts::hasA2FirstEventCorruption(RawAtriStationEvent *rawEvent)
{

	/*
		The first four events readout in A2 after unixTime 1448485911 are corrupted
		And should be marked "bad"
	*/

	bool hasA2FirstEventCorruption=false;
	if(rawEvent->stationId==ARA_STATION2
		&& rawEvent->unixTime>=1448485911 
		&& rawEvent->eventNumber<4){
		hasA2FirstEventCorruption=true;
	}
	return hasA2FirstEventCorruption;
}
