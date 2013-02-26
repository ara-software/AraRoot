//////////////////////////////////////////////////////////////////////////////
/////  AtriEventHkData.cxx        Definition of the AtriEventHkData            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AtriEventHkData                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AtriEventHkData.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AtriEventHkData);

AtriEventHkData::AtriEventHkData() 
{
  //Default Constructor
}

AtriEventHkData::~AtriEventHkData() {
  //Default Destructor
}
 

AtriEventHkData::AtriEventHkData(AraEventHk_t *theHk)
  :RawAraGenericHeader(&(theHk->gHdr))
{
  unixTime=theHk->unixTime; ///< Time in seconds (64-bits for future proofing)
  unixTimeUs=theHk->unixTimeUs; ///< Time in microseconds (32-bits)
  firmwareVersion=theHk->firmwareVersion; ///< Firmware version
  firmwareVersionMaj = (firmwareVersion & 0xf000) >> 12;
  firmwareVersionMid = (firmwareVersion & 0xf00) >> 8;
  firmwareVersionMin = (firmwareVersion & 0xff);

  memcpy(wilkinsonCounter,theHk->wilkinsonCounter,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson counter one per DDA
  memcpy(wilkinsonDelay,theHk->wilkinsonDelay,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson delay?? one per DDA
  ppsCounter=theHk->ppsCounter; ///< Pulse per second counter
  clockCounter=theHk->clockCounter; ///< Clock counter (which clock?)

  memcpy(l1ScalerSurface,theHk->l1ScalerSurface,ANTS_PER_TDA*sizeof(uint16_t)); ///< L1 surface scaler
  memcpy(l1Scaler,theHk->l1Scaler,NUM_L1_SCALERS*sizeof(uint16_t)); ///< L1 scaler, am I correct in decoding this need to check mapping
  memcpy(l2Scaler,theHk->l2Scaler,NUM_L2_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(l3Scaler,theHk->l3Scaler,NUM_L3_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(l4Scaler,theHk->l4Scaler,NUM_L4_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(t1Scaler,theHk->t1Scaler,NUM_T1_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(thresholdDac,theHk->thresholdDac,NUM_L1_SCALERS*sizeof(uint16_t));
  memcpy(surfaceThresholdDac,theHk->surfaceThresholdDac,ANTS_PER_TDA*sizeof(uint16_t)); 
  memcpy(vdlyDac,theHk->vdlyDac,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(vadjDac,theHk->vadjDac,DDA_PER_ATRI*sizeof(UShort_t));

  if(firmwareVersionMaj > 0 || firmwareVersionMid >= 10){
    evReadoutError = theHk->evReadoutError;
    evReadoutCountAvg = theHk->evReadoutCountAvg;
    evReadoutCountMin = theHk->evReadoutCountMin;
    blockBuffCountAvg = theHk->blockBuffCountAvg;
    blockBuffCountMax = theHk->blockBuffCountMax;
    digDeadTime = theHk->digDeadTime;
    buffDeadTime = theHk->buffDeadTime;
    totalDeadTime = theHk->totalDeadTime;
  }
  else{
    evReadoutError=0;
    evReadoutCountAvg=0;
    evReadoutCountMin=0;
    blockBuffCountAvg=0;
    blockBuffCountMax=0;
    digDeadTime=0;
    buffDeadTime=0;
    totalDeadTime=0;
  }


}

AtriEventHkData::AtriEventHkData(AraEventHk2_7_t *theHk)
  :RawAraGenericHeader(&(theHk->gHdr))
{
  unixTime=theHk->unixTime; ///< Time in seconds (64-bits for future proofing)
  unixTimeUs=theHk->unixTimeUs; ///< Time in microseconds (32-bits)
  firmwareVersion=theHk->firmwareVersion; ///< Firmware version
  firmwareVersion=theHk->firmwareVersion; ///< Firmware version
  firmwareVersionMaj = (firmwareVersion & 0xf000) >> 12;
  firmwareVersionMid = (firmwareVersion & 0xf00) >> 8;
  firmwareVersionMin = (firmwareVersion & 0xff);

  memcpy(wilkinsonCounter,theHk->wilkinsonCounter,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson counter one per DDA
  memcpy(wilkinsonDelay,theHk->wilkinsonDelay,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson delay?? one per DDA
  ppsCounter=theHk->ppsCounter; ///< Pulse per second counter
  clockCounter=theHk->clockCounter; ///< Clock counter (which clock?)

  memcpy(vdlyDac,theHk->vdlyDac,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(vadjDac,theHk->vadjDac,DDA_PER_ATRI*sizeof(UShort_t));
 
  evReadoutError=0;
  evReadoutCountAvg=0;
  evReadoutCountMin=0;
  blockBuffCountAvg=0;
  blockBuffCountMax=0;
  digDeadTime=0;
  buffDeadTime=0;
  totalDeadTime=0;

}

Double_t AtriEventHkData::wilkinsonCounterNs(Int_t dda){
  if(dda >= DDA_PER_ATRI || dda < 0) return -1;
  Double_t wilkNs = wilkinsonCounter[dda]*(20.83/64.);
  return wilkNs;

}


Double_t AtriEventHkData::getSingleChannelRateHz(Int_t tda, Int_t channel){
  if(tda >= TDA_PER_ATRI || tda < 0) return -1;
  if(channel>= ANTS_PER_TDA || tda <0) return -1;
  uint8_t preScale=32;

  return l1Scaler[tda*ANTS_PER_TDA+channel] * preScale;
  
}
Double_t AtriEventHkData::getOneOfFourRateHz(Int_t tda){
  if(tda >= TDA_PER_ATRI || tda < 0) return -1;
  uint8_t preScale=1;
  if(firmwareVersionMaj > 0 || firmwareVersionMid > 9 || firmwareVersionMin >= 40) preScale=64; 

  return l2Scaler[tda*4]*preScale;


}
Double_t AtriEventHkData::getTwoOfFourRateHz(Int_t tda){
  if(tda >= TDA_PER_ATRI || tda < 0) return -1;
  return l2Scaler[tda*4+1];


}
Double_t AtriEventHkData::getThreeOfFourRateHz(Int_t tda){
  if(tda >= TDA_PER_ATRI || tda < 0) return -1;
  return l2Scaler[tda*4+2];


}
Double_t AtriEventHkData::getThreeOfEightRateHz(Int_t tda_pair){
  if(tda_pair > 1 || tda_pair <0) return -1;
  return l3Scaler[tda_pair];

}
  
UInt_t AtriEventHkData::getSingleChannelThreshold(Int_t tda, Int_t channel)
{
  ///< Gets the single channel rate in Hz for tda channel
 
  if(tda >= TDA_PER_ATRI || tda < 0) return -1;
  if(channel>= ANTS_PER_TDA || tda <0) return -1;
  return thresholdDac[channel+(tda*ANTS_PER_TDA)];
       
}
