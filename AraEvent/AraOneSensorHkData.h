//////////////////////////////////////////////////////////////////////////////
/////  AraOneSensorHkData.h        Sensor HK data class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Sensor related housekeeping info    /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAONESENSORHKDATA_H
#define ARAONESENSORHKDATA_H

//Includes
#include <TObject.h>
#include "araOneStructures.h"
#include "araDefines.h"

//!  AraOneSensorHkData -- The HK data
/*!
  The ROOT implementation of the HK data
  \ingroup rootclasses
*/
class AraOneSensorHkData: public TObject
{
 public:
   AraOneSensorHkData(); ///< Default constructor
   AraOneSensorHkData(AraSensorHk_t *theHk); ///< Assignment constructor
   ~AraOneSensorHkData(); ///< Destructor


   UChar_t verId; ///< Software version
   UChar_t stationId; ///< Station Id
   ULong64_t unixTime; ///< Time in seconds (64-bits for future proofing)
   UInt_t unixTimeUs; ///< Time in microseconds (32-bits)

   
   UChar_t atriVoltage; ///< ATRI Voltage (conversion?)
   UChar_t atriCurrent; ///< ATRI Current (conversion?)
   UShort_t ddaTemmp[DDA_PER_ATRI]; ///< DDA Temperature conversion??
   UShort_t tdaTemp[TDA_PER_ATRI]; ///< TDA Temperature conversion??
   UInt_t ddaVoltageCurrent[DDA_PER_ATRI]; ///< 3 bytes only will work out better packing when I know what the numbers mean
   UInt_t tdaVoltageCurrent[DDA_PER_ATRI]; ///< 3 bytes only will work out better packing when I know what the numbers mean
   

   ClassDef(AraOneSensorHkData,1);
};


#endif //ARAHKDATA_H
