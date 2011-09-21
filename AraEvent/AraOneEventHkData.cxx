//////////////////////////////////////////////////////////////////////////////
/////  AraOneEventHkData.cxx        Definition of the AraOneEventHkData            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AraOneEventHkData                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraOneEventHkData.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraOneEventHkData);

AraOneEventHkData::AraOneEventHkData() 
{
   //Default Constructor
}

AraOneEventHkData::~AraOneEventHkData() {
   //Default Destructor
}


AraOneEventHkData::AraOneEventHkData(AraEventHk_t *theHk)
  :RawAraOneGenericHeader(&(theHk->gHdr))
{
  unixTime=theHk->unixTime; ///< Time in seconds (64-bits for future proofing)
  unixTimeUs=theHk->unixTimeUs; ///< Time in microseconds (32-bits)
  firmwareVersion=theHk->firmwareVersion; ///< Firmware version
  memcpy(wilkinsonCounter,theHk->wilkinsonCounter,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson counter one per DDA
  memcpy(wilkinsonDelay,theHk->wilkinsonDelay,DDA_PER_ATRI*sizeof(UShort_t)); ///< Wilkinson delay?? one per DDA
  ppsCounter=theHk->ppsCounter; ///< Pulse per second counter
  clockCounter=theHk->clockCounter; ///< Clock counter (which clock?)
  memcpy(l1Scaler,theHk->l1Scaler,TDA_PER_ATRI*ANTS_PER_TDA*sizeof(UShort_t)); ///< L1 scaler, am I correct in decoding this need to check mapping
  memcpy(l2Scaler,theHk->l2Scaler,TDA_PER_ATRI*L2_PER_TDA*sizeof(UShort_t)); ///< L2 scaler
  l3Scaler=theHk->l3Scaler; ///< L3 scaler
  memcpy(deadTime,theHk->deadTime,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(avgOccupancy,theHk->avgOccupancy,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(maxOccupancy,theHk->maxOccupancy,DDA_PER_ATRI*sizeof(UChar_t));
  memcpy(vdlyDac,theHk->vdlyDac,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(vadjDac,theHk->vadjDac,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(thresholdDac,theHk->thresholdDac,TDA_PER_ATRI*ANTS_PER_TDA*sizeof(UShort_t));
  memcpy(l1ScalerSurface,theHk->l1ScalerSurface,ANTS_PER_TDA*sizeof(UShort_t));
  memcpy(surfaceThresholdDac,theHk->surfaceThresholdDac,ANTS_PER_TDA*sizeof(UShort_t));
  l2ScalerAllTda12=theHk->l2ScalerAllTda12;
  l2ScalerAllTda34=theHk->l2ScalerAllTda34;
  l3ScalerSurface=theHk->l3ScalerSurface;
}
