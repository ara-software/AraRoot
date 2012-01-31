//////////////////////////////////////////////////////////////////////////////
/////  AraRawTestBedRFChannel.cxx        Definition of the AraRawTestBedRFChannel            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AraRawTestBedRFChannel                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraRawTestBedRFChannel.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraRawTestBedRFChannel);

AraRawTestBedRFChannel::AraRawTestBedRFChannel() 
{
   //Default Constructor
}

AraRawTestBedRFChannel::~AraRawTestBedRFChannel() {
   //Default Destructor
}


AraRawTestBedRFChannel::AraRawTestBedRFChannel(AraTestBedAraTestBedRFChannelFull_t *rfPtr)
{
  fillChannel(rfPtr);
}

void AraRawTestBedRFChannel::fillChannel(AraTestBedAraTestBedRFChannelFull_t *rfPtr)
{
  chanId=rfPtr->header.chanId;
  chipIdFlag=rfPtr->header.chipIdFlag;
  firstHitbus=rfPtr->header.firstHitbus;
  lastHitbus=rfPtr->header.lastHitbus;
  memcpy(data,rfPtr->data,MAX_NUMBER_SAMPLES_LAB3*sizeof(UShort_t));
}

Int_t AraRawTestBedRFChannel::getEarliestSample()
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

Int_t AraRawTestBedRFChannel::getLatestSample()
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

