//////////////////////////////////////////////////////////////////////////////
/////  AtriEventHkData.h        Event HK data class                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Event related housekeeping info     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ATRIEVENTHKDATA_H
#define ATRIEVENTHKDATA_H

//Includes
#include <TObject.h>
#include "RawAraGenericHeader.h"
#include "araIcrrStructures.h"


//!  Part of AraEvent library. The ATRI Housekeeping data class.
/*!
  The ROOT implementation of the HK data
  \ingroup rootclasses
*/
class AtriEventHkData: public RawAraGenericHeader, public TObject
{
 public:
   AtriEventHkData(); ///< Default constructor
   AtriEventHkData(AraEventHk_t *theHk); ///< Assignment constructor
   AtriEventHkData(AraEventHk2_7_t *theHk); ///< Assignment constructor

   ~AtriEventHkData(); ///< Destructor

   ULong64_t unixTime; ///< Time in seconds (64-bits for future proofing)
   UInt_t unixTimeUs; ///< Time in microseconds (32-bits)
   UInt_t firmwareVersion; ///< Firmware version
   UShort_t wilkinsonCounter[DDA_PER_ATRI]; ///< Wilkinson counter one per DDA
   UShort_t wilkinsonDelay[DDA_PER_ATRI]; ///< Wilkinson delay?? one per DDA
   UInt_t ppsCounter; ///< Pulse per second counter
   UInt_t clockCounter; ///< Clock counter (which clock?)

   //new stuff
   uint16_t l1Scaler[NUM_L1_SCALERS]; ///< L1 scaler, am I correct in decoding this need to check mapping prescaled by 32
   uint16_t l2Scaler[NUM_L2_SCALERS]; ///< L2 scaler not prescaled
   uint16_t l3Scaler[NUM_L3_SCALERS]; ///< L3 scaler not prescaled
   uint16_t l4Scaler[NUM_L4_SCALERS]; ///< L4 scaler not prescaled
   uint16_t t1Scaler[NUM_T1_SCALERS]; ///< T1 scaler not prescaled
   uint16_t thresholdDac[NUM_L1_SCALERS]; ///< Value the thresholds are set to 
   uint16_t l1ScalerSurface[ANTS_PER_TDA]; ///< The Surface L1 scaler 
   uint16_t surfaceThresholdDac[ANTS_PER_TDA]; ///< The surface thresholds 

   //Old stuff
   /* UShort_t l2ScalerAllTda12; ///< L2 for 3of8 in DTA 1&2 */
   /* UShort_t l2ScalerAllTda34; ///< L2 for 3of8 in DTA 3&4 */
   /* UShort_t l1Scaler[TDA_PER_ATRI][ANTS_PER_TDA]; ///< L1 scaler, am I correct in decoding this need to check mapping -- prescaled by 32 */
   /* UShort_t l1ScalerSurface[ANTS_PER_TDA]; ///< The Surface L1 scaler */
   /* UShort_t l2Scaler[TDA_PER_ATRI][L2_PER_TDA]; ///< L2 scaler -- not prescaled */
   /* UShort_t l3Scaler; ///< L3 scaler */
   /* UShort_t l3ScalerSurface; ///< L3 scaler for surface trigger */
   UChar_t deadTime[DDA_PER_ATRI]; ///< Dead time  8-bit measures of deadtime (multiply by 4096, divide by 1e6).
   UChar_t avgOccupancy[DDA_PER_ATRI]; ///< Average occupancy over last 16 milliseconds
   UChar_t maxOccupancy[DDA_PER_ATRI]; ///< Maximum occupancy in last second
   UShort_t vdlyDac[DDA_PER_ATRI]; ///< Value the vdly is set to
   UShort_t vadjDac[DDA_PER_ATRI]; ///< Value the vdly is set to

   ClassDef(AtriEventHkData,3);
};


#endif //ARAHKDATA_H
