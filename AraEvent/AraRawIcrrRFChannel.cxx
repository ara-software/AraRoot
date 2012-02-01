//////////////////////////////////////////////////////////////////////////////
/////  AraRawIcrrRFChannel.cxx        Definition of the AraRawIcrrRFChannel            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AraRawIcrrRFChannel                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraRawIcrrRFChannel.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraRawIcrrRFChannel);

AraRawIcrrRFChannel::AraRawIcrrRFChannel() 
{
   //Default Constructor
}

AraRawIcrrRFChannel::~AraRawIcrrRFChannel() {
   //Default Destructor
}


AraRawIcrrRFChannel::AraRawIcrrRFChannel(IcrrRFChannelFull_t *rfPtr)
{
  fillChannel(rfPtr);
}

void AraRawIcrrRFChannel::fillChannel(IcrrRFChannelFull_t *rfPtr)
{
  chanId=rfPtr->header.chanId;
  chipIdFlag=rfPtr->header.chipIdFlag;
  firstHitbus=rfPtr->header.firstHitbus;
  lastHitbus=rfPtr->header.lastHitbus;
  memcpy(data,rfPtr->data,MAX_NUMBER_SAMPLES_LAB3*sizeof(UShort_t));
}

Int_t AraRawIcrrRFChannel::getEarliestSample()
{
 Int_t lastHitBus=this->getLastHitBus();
 Int_t firstHitbus=this->getFirstHitBus();
 Int_t wrappedHitBus=this->getWrappedHitBus();
 Int_t earliestSample=0;
 if(!wrappedHitBus) {
   earliestSample=lastHitBus+1;
 }
 else {
   earliestSample=firstHitbus+1;
 }
 if(earliestSample==0) earliestSample=1;
 if(earliestSample<260) return earliestSample;
 return 1;
}

Int_t AraRawIcrrRFChannel::getLatestSample()
{
 Int_t lastHitBus=this->getLastHitBus();
 Int_t firstHitbus=this->getFirstHitBus();
 Int_t wrappedHitBus=this->getWrappedHitBus();
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

