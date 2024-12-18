//////////////////////////////////////////////////////////////////////////////
/////  RawIcrrStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawIcrrStationEvent.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawIcrrStationEvent);

RawIcrrStationEvent::RawIcrrStationEvent()   
  :RawAraStationEvent(ARA_TESTBED)
{
  //Default Constructor
}

RawIcrrStationEvent::~RawIcrrStationEvent() {
   //Default Destructor
}


RawIcrrStationEvent::RawIcrrStationEvent(IcrrEventBody_t *bdPtr)
  :RawAraStationEvent(ARA_TESTBED),head(&(bdPtr->hd)),trig(&(bdPtr->trig)),hk(&(bdPtr->hk))
{
  for(int i=0;i<NUM_DIGITIZED_ICRR_CHANNELS;i++) {
    chan[i].fillChannel(&(bdPtr->channel[i]));
  }
}

RawIcrrStationEvent::RawIcrrStationEvent(IcrrEventBody_t *bdPtr, AraStationId_t stationId)
  :RawAraStationEvent(stationId),head(&(bdPtr->hd)),trig(&(bdPtr->trig)),hk(&(bdPtr->hk))
{
  for(int i=0;i<NUM_DIGITIZED_ICRR_CHANNELS;i++) {
    chan[i].fillChannel(&(bdPtr->channel[i]));
  }
}



Int_t RawIcrrStationEvent::getEarliestSample(Int_t chanIndex)
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

Int_t RawIcrrStationEvent::getLatestSample(Int_t chanIndex)
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
