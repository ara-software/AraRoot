//////////////////////////////////////////////////////////////////////////////
/////  RawAtriStationEvent.h        Raw ARA station event class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that is a wraper for                            /////
/////    AraStationEventEventHeader_t                                    /////
/////    AraStationEventEventChannel_t                                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWATRISTATIONEVENT_H
#define RAWATRISTATIONEVENT_H

//Includes
#include <vector>
#include <TObject.h>
#include "RawAraStationEvent.h"
#include "RawAtriStationBlock.h"
#include "araAtriStructures.h"
#include "araSoft.h"




//! Part of AraEvent library. This is the ATRI specific Raw Event class, inheriting from RawAraStationEvent.

/*!
  The ROOT implementation of the raw ARA Station Event from an ATRI type station containing the samples from one event readout of the IRS
  \ingroup rootclasses
*/
class RawAtriStationEvent: public RawAraStationEvent
{
 public:
   RawAtriStationEvent(); ///< Default constructor
   RawAtriStationEvent(AraStationEventHeader_t *hdPtr, char *dataBuffer); ///< Assignment constructor
   RawAtriStationEvent(AraStationEventHeader_t *hdPtr, char *dataBuffer, AraStationId_t forcedStationId); ///< Assignment constructor, forcing the stationId to the value forced stationId



   ~RawAtriStationEvent(); ///< Destructor

   Int_t getNumChannels() { return 0; }


   ULong64_t unixTime; ///< Software event time in seconds (64-bits for future proofing)
   UInt_t unixTimeUs; ///< Software event time in microseconds (32-bits)
   UInt_t eventNumber; ///< Software event number
   UInt_t ppsNumber; ///< For matching up with thresholds etc.
   UInt_t numStationBytes; ///<Bytes in station readout
   UInt_t timeStamp; ///< Timestamp

   UInt_t eventId; ///< Event Id
   UShort_t versionId; ///< Version Id for event header
   UShort_t numReadoutBlocks; ///< Number of readout blocks which follow header

   UInt_t triggerInfo[MAX_TRIG_BLOCKS]; ///< The trigger pattern for the future
   UChar_t triggerBlock[MAX_TRIG_BLOCKS]; ///< Which block the triggers occured in

   std::vector<RawAtriStationBlock> blockVec;
   
   inline static int getPedIndex(int dda, int block, int chan, int sample)
   {
     return sample+(chan*SAMPLES_PER_BLOCK)+(block*RFCHAN_PER_DDA*SAMPLES_PER_BLOCK)+(dda*BLOCKS_PER_DDA*RFCHAN_PER_DDA*SAMPLES_PER_BLOCK);
   }

   Int_t getFirstCapArray(Int_t dda); ///< Function for asking the block vector the capArray
   


  ClassDef(RawAtriStationEvent,1);
};




#endif //RAWATRISTATIONEVENT
