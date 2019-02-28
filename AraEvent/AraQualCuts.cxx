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

//ROOT includes
#include "TGraph.h"

AraQualCuts * AraQualCuts::fgInstance=0;

AraQualCuts::AraQualCuts() 
{
	//for the moment, this doesn't do anything intelligent...
}

AraQualCuts::~AraQualCuts() {
	//for the moment, this doesn't need to do anything intelligent
}

//______________________________________________________________________________
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
  
  bool hasBlockGap = AraQualCuts::hasBlockGap(realEvent);
  bool hasTimingError = AraQualCuts::hasTimingError(realEvent);
  bool hasTooFewBlocks = AraQualCuts::hasTooFewBlocks(realEvent);
  
  if(hasBlockGap || hasTimingError || hasTooFewBlocks){
    isGoodEvent=false;
  }
  return isGoodEvent;
}

//! Returns if a raw atri event has a block gap
/*!
  \param rawEvent the raw atri event pointer
  \return if the event has a block gap
*/
bool AraQualCuts::hasBlockGap(RawAtriStationEvent *rawEventv)
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
  int lenBlockVec = rawEventv->blockVec.size();
  int numDDA=DDA_PER_ATRI; //defined in araSoft.h
  int numBlocks = BLOCKS_PER_DDA; //defined in araSoft.h
  if(rawEventv->blockVec[lenBlockVec-1].getBlock() != (rawEventv->blockVec[0].getBlock() + lenBlockVec/numDDA -1 ) ){
    if( numBlocks-rawEventv->blockVec[0].getBlock() + rawEventv->blockVec[lenBlockVec-1].getBlock() != lenBlockVec/numDDA-1){
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