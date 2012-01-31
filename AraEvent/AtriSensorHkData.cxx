//////////////////////////////////////////////////////////////////////////////
/////  AtriSensorHkData.cxx        Definition of the AtriSensorHkData            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AtriSensorHkData                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AtriSensorHkData.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AtriSensorHkData);

AtriSensorHkData::AtriSensorHkData() 
{
   //Default Constructor
}

AtriSensorHkData::~AtriSensorHkData() {
   //Default Destructor
}


AtriSensorHkData::AtriSensorHkData(AraSensorHk_t *theHk)
  :RawAraGenericHeader(&(theHk->gHdr))
{

  unixTime=theHk->unixTime; ///< Time in seconds (64-bits for future proofing)
  unixTimeUs=theHk->unixTimeUs; ///< Time in microseconds (32-bits)

  atriVoltage=theHk->atriVoltage; ///< ATRI Voltage (conversion?)
  atriCurrent=theHk->atriCurrent; ///< ATRI Current (conversion?)
  memcpy(ddaTemp,theHk->ddaTemp,sizeof(UShort_t)*DDA_PER_ATRI); ///< DDA Temperature conversion??
  memcpy(tdaTemp,theHk->tdaTemp,sizeof(UShort_t)*TDA_PER_ATRI); ///< TDA Temperature conversion??
  memcpy(ddaVoltageCurrent,theHk->ddaVoltageCurrent,sizeof(UInt_t)*DDA_PER_ATRI); ///< 3 bytes only will work out better packing when I know what the numbers mean
  memcpy(tdaVoltageCurrent,theHk->tdaVoltageCurrent,sizeof(UInt_t)*DDA_PER_ATRI); ///< 3 bytes only will work out better packing when I know what the numbers mean

}
