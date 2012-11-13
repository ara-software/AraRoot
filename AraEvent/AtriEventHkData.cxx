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
  memcpy(wilkinsonCounter,theHk->wilkinsonCounter,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson counter one per DDA
  memcpy(wilkinsonDelay,theHk->wilkinsonDelay,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson delay?? one per DDA
  ppsCounter=theHk->ppsCounter; ///< Pulse per second counter
  clockCounter=theHk->clockCounter; ///< Clock counter (which clock?)


  //New Stuff
 memcpy(l1ScalerSurface,theHk->l1ScalerSurface,ANTS_PER_TDA*sizeof(uint16_t)); ///< L1 surface scaler
  memcpy(l1Scaler,theHk->l1Scaler,NUM_L1_SCALERS*sizeof(uint16_t)); ///< L1 scaler, am I correct in decoding this need to check mapping
  memcpy(l2Scaler,theHk->l2Scaler,NUM_L2_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(l3Scaler,theHk->l3Scaler,NUM_L3_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(l4Scaler,theHk->l4Scaler,NUM_L4_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(t1Scaler,theHk->t1Scaler,NUM_T1_SCALERS*sizeof(uint16_t)); ///< L2 scaler
  memcpy(thresholdDac,theHk->thresholdDac,NUM_L1_SCALERS*sizeof(uint16_t));
  memcpy(surfaceThresholdDac,theHk->surfaceThresholdDac,ANTS_PER_TDA*sizeof(uint16_t)); 


  //Old stuff
  // l2ScalerAllTda12=theHk->l2ScalerAllTda12;
  // l2ScalerAllTda34=theHk->l2ScalerAllTda34;
  // l3ScalerSurface=theHk->l3ScalerSurface;
  //  l3Scaler=theHk->l3Scaler; ///< L3 scaler

  memcpy(deadTime,theHk->deadTime,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(avgOccupancy,theHk->avgOccupancy,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(maxOccupancy,theHk->maxOccupancy,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(vdlyDac,theHk->vdlyDac,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(vadjDac,theHk->vadjDac,DDA_PER_ATRI*sizeof(UShort_t));


}

AtriEventHkData::AtriEventHkData(AraEventHk2_7_t *theHk)
  :RawAraGenericHeader(&(theHk->gHdr))
{
  unixTime=theHk->unixTime; ///< Time in seconds (64-bits for future proofing)
  unixTimeUs=theHk->unixTimeUs; ///< Time in microseconds (32-bits)
  firmwareVersion=theHk->firmwareVersion; ///< Firmware version
  memcpy(wilkinsonCounter,theHk->wilkinsonCounter,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson counter one per DDA
  memcpy(wilkinsonDelay,theHk->wilkinsonDelay,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson delay?? one per DDA
  ppsCounter=theHk->ppsCounter; ///< Pulse per second counter
  clockCounter=theHk->clockCounter; ///< Clock counter (which clock?)


  //New Stuff

  // memcpy(l1Scaler,theHk->l1Scaler,NUM_L1_SCALERS*sizeof(UShort_t)); ///< L1 scaler, am I correct in decoding this need to check mapping
  // memcpy(l2Scaler,theHk->l2Scaler,NUM_L2_SCALERS*sizeof(UShort_t)); ///< L2 scaler
  // memcpy(l3Scaler,theHk->l3Scaler,NUM_L3_SCALERS*sizeof(UShort_t)); ///< L2 scaler
  // memcpy(l4Scaler,theHk->l4Scaler,NUM_L4_SCALERS*sizeof(UShort_t)); ///< L2 scaler
  // memcpy(t4Scaler,theHk->t4Scaler,NUM_T4_SCALERS*sizeof(UShort_t)); ///< L2 scaler
  //  memcpy(l1ScalerSurface,theHk->l1ScalerSurface,ANTS_PER_TDA*sizeof(UShort_t)); 
  //  memcpy(surfaceThresholdDac,theHk->surfaceThresholdDac,ANTS_PER_TDA*sizeof(UShort_t));
  // memcpy(thresholdDac,theHk->thresholdDac,NUM_L1_SCALERS*sizeof(UShort_t));

  memcpy(deadTime,theHk->deadTime,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(avgOccupancy,theHk->avgOccupancy,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(maxOccupancy,theHk->maxOccupancy,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(vdlyDac,theHk->vdlyDac,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(vadjDac,theHk->vadjDac,DDA_PER_ATRI*sizeof(UShort_t));


}


Double_t AtriEventHkData::wilkinsonCounterNs(Int_t dda){
  if(dda >= DDA_PER_ATRI) return -1;
  Double_t wilkNs = wilkinsonCounter[dda]*(20.83/64.);
  return wilkNs;

}
