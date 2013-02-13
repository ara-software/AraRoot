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
   UInt_t firmwareVersion; ///< Firmware version [31:28] ATRI version [27:24] month [23:16] day [15:12] vMajor [11:8] vMiddle [7:0] vMinor 
   uint8_t firmwareVersionMaj; ///< Split up the firmware version into more inteligable parts
   uint8_t firmwareVersionMid; ///< Split up the firmware version into more inteligable parts
   uint8_t firmwareVersionMin; ///< Split up the firmware version into more inteligable parts

   UShort_t wilkinsonCounter[DDA_PER_ATRI]; ///< Wilkinson counter one per DDA
   UShort_t wilkinsonDelay[DDA_PER_ATRI]; ///< Wilkinson delay?? one per DDA
   UInt_t ppsCounter; ///< Pulse per second counter
   UInt_t clockCounter; ///< Clock counter (which clock?)

   Double_t wilkinsonCounterNs(Int_t dda); ///< Convert wilkinsonCounter to ns

   //new scaler format
   uint16_t l1Scaler[NUM_L1_SCALERS]; ///< L1 scaler, pre-scale depends on revision of firmware -- check helper functions
   uint16_t l2Scaler[NUM_L2_SCALERS]; ///< L2 scaler not prescaled
   uint16_t l3Scaler[NUM_L3_SCALERS]; ///< L3 scaler not prescaled
   uint16_t l4Scaler[NUM_L4_SCALERS]; ///< L4 scaler not prescaled
   uint16_t t1Scaler[NUM_T1_SCALERS]; ///< T1 scaler not prescaled
   uint16_t thresholdDac[NUM_L1_SCALERS]; ///< Value the thresholds are set to 
   uint16_t l1ScalerSurface[ANTS_PER_TDA]; ///< The Surface L1 scaler 
   uint16_t surfaceThresholdDac[ANTS_PER_TDA]; ///< The surface thresholds 

   uint8_t evReadoutError; ///< Error code from event readout processor
   uint16_t evReadoutCountAvg; ///< Average number of Bytes available in event count fifo
   uint16_t evReadoutCountMin; ///< Minimum number of Bytes available in event count fifo
   uint16_t blockBuffCountAvg; ///< Average number of blocks available in block buffer
   uint16_t blockBuffCountMax; ///< Maximum number of blocks available in block buffer
   uint16_t digDeadTime; ///< Deadtime originating from digitisation in us, prescale 16
   uint16_t buffDeadTime; ///< Deadtime originating from buffer full in us, prescale 16
   uint16_t totalDeadTime; ///< Total deadtime in us, prescale 16

   UShort_t vdlyDac[DDA_PER_ATRI]; ///< Value the vdly is set to
   UShort_t vadjDac[DDA_PER_ATRI]; ///< Value the vdly is set to

   
   Double_t getSingleChannelRateHz(Int_t tda, Int_t channel); ///< Gets the single channel rate in Hz for tda channel
   Double_t getOneOfFourRateHz(Int_t tda); ///< Gets the rate in Hz of one of four channels on tda
   Double_t getTwoOfFourRateHz(Int_t tda); ///< Gets the rate in Hz of two of four channels on tda
   Double_t getThreeOfFourRateHz(Int_t tda); ///< Gets the rate in Hz of three of four channels on tda
   Double_t getThreeOfEightRateHz(Int_t tda_pair); ///< Gets the rate in Hz of three of eight channels on tda_pair (0 is stack 1 and 2, 0 is stack 3 and 4)
   

   ClassDef(AtriEventHkData,3);
};


#endif //ARAHKDATA_H
