//////////////////////////////////////////////////////////////////////////////
/////  AraRawRFChannel.cxx        Definition of the AraRawRFChannel            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AraRawRFChannel                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraRawRFChannel.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraRawRFChannel);

AraRawRFChannel::AraRawRFChannel() 
{
   //Default Constructor
}

AraRawRFChannel::~AraRawRFChannel() {
   //Default Destructor
}


AraRawRFChannel::AraRawRFChannel(RFChannelFull_t *rfPtr)
{
  fillChannel(rfPtr);
}

void AraRawRFChannel::fillChannel(RFChannelFull_t *rfPtr)
{
  chanId=rfPtr->header.chanId;
  chipIdFlag=rfPtr->header.chipIdFlag;
  firstHitbus=rfPtr->header.firstHitbus;
  lastHitbus=rfPtr->header.lastHitbus;
  memcpy(data,rfPtr->data,MAX_NUMBER_SAMPLES*sizeof(UShort_t));
}

Int_t AraRawRFChannel::getEarliestSample()
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

Int_t AraRawRFChannel::getLatestSample()
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

