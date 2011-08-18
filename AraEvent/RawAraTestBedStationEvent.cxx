//////////////////////////////////////////////////////////////////////////////
/////  RawAraTestBedStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraTestBedStationEvent.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraTestBedStationEvent);

RawAraTestBedStationEvent::RawAraTestBedStationEvent()   
{
  
  //Default Constructor
}

RawAraTestBedStationEvent::~RawAraTestBedStationEvent() {
   //Default Destructor
}


RawAraTestBedStationEvent::RawAraTestBedStationEvent(AraTestBedEventBody_t *bdPtr)
  :head(&(bdPtr->hd)),trig(&(bdPtr->trig)),hk(&(bdPtr->hk))
{
  for(int i=0;i<NUM_DIGITIZED_TESTBED_CHANNELS;i++) {
    chan[i].fillChannel(&(bdPtr->channel[i]));
  }
}

Int_t RawAraTestBedStationEvent::getEarliestSample(Int_t chanIndex)
{
 Int_t lastHitBus=this->getLastHitBus(chanIndex);
 Int_t firstHitbus=this->getFirstHitBus(chanIndex);
 Int_t wrappedHitBus=this->getWrappedHitBus(chanIndex);
 Int_t earliestSample=0;
 if(!wrappedHitBus) {
   earliestSample=lastHitBus+1;
 }
 else {
   earliestSample=firstHitbus+1;
 }
 // if(earliestSample==0) earliestSample=1; //This was needed for ANITA
 if(earliestSample<260) return earliestSample;
 return 1;
}

Int_t RawAraTestBedStationEvent::getLatestSample(Int_t chanIndex)
{
 Int_t lastHitBus=this->getLastHitBus(chanIndex);
 Int_t firstHitbus=this->getFirstHitBus(chanIndex);
 Int_t wrappedHitBus=this->getWrappedHitBus(chanIndex);
 Int_t latestSample=259;
 if(!wrappedHitBus) {
   latestSample=firstHitbus-1;
 }
 else {
   latestSample=lastHitBus-1;
 }
 if(latestSample>0) return latestSample;
 return 259;
}
