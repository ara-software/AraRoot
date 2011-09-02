//////////////////////////////////////////////////////////////////////////////
/////  AraOneEventHkData.h        Event HK data class                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Event related housekeeping info     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAONEEVENTHKDATA_H
#define ARAONEEVENTHKDATA_H

//Includes
#include <TObject.h>
#include "RawAraOneGenericHeader.h"
#include "araOneStructures.h"


//!  AraOneEventHkData -- The HK data
/*!
  The ROOT implementation of the HK data
  \ingroup rootclasses
*/
class AraOneEventHkData: public RawAraOneGenericHeader, public TObject
{
 public:
   AraOneEventHkData(); ///< Default constructor
   AraOneEventHkData(AraEventHk_t *theHk); ///< Assignment constructor
   ~AraOneEventHkData(); ///< Destructor

   ULong64_t unixTime; ///< Time in seconds (64-bits for future proofing)
   UInt_t unixTimeUs; ///< Time in microseconds (32-bits)
   UInt_t firmwareVersion; ///< Firmware version
   UShort_t wilkinsonCounter[DDA_PER_ATRI]; ///< Wilkinson counter one per DDA
   UShort_t wilkinsonDelay[DDA_PER_ATRI]; ///< Wilkinson delay?? one per DDA
   UInt_t ppsCounter; ///< Pulse per second counter
   UInt_t clockCounter; ///< Clock counter (which clock?)
   UShort_t l1Scaler[DDA_PER_ATRI][RFCHAN_PER_DDA]; ///< L1 scaler, am I correct in decoding this need to check mapping
   UShort_t l2Scaler[DDA_PER_ATRI]; ///< L2 scaler
   UShort_t l3Scaler; ///< L3 scaler
   UShort_t triggerScaler; ///< Trigger scaler (what is this?)
   

   ClassDef(AraOneEventHkData,2);
};


#endif //ARAHKDATA_H
