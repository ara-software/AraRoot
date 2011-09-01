//////////////////////////////////////////////////////////////////////////////
/////  RawAraOneStationEvent.h        Raw ARA station event class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that is a wraper for                            /////
/////    AraStationEventEventHeader_t                                    /////
/////    AraStationEventEventChannel_t                                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARAONESTATIONEVENT_H
#define RAWARAONESTATIONEVENT_H

//Includes
#include <vector>
#include <TObject.h>
#include "RawAraOneStationBlock.h"
#include "araOneStructures.h"
#include "araSoft.h"




//!  RawAraOneStationEvent -- The Raw ARA Station Event Class
/*!
  The ROOT implementation of the raw ARA Station Event containing the samples from one event readout of the IRS
  \ingroup rootclasses
*/
class RawAraOneStationEvent: public TObject
{
 public:
   RawAraOneStationEvent(); ///< Default constructor
   RawAraOneStationEvent(); ///< Assignment constructor
   ~RawAraOneStationEvent(); ///< Destructor

   //   AraGenericHeader_t gHdr;

   ULong64_t unixTime; ///< Software event time in seconds (64-bits for future proofing)
   UInt_t unixTimeUs; ///< Software event time in microseconds (32-bits)
   UInt_t eventNumber; ///< Software event number
   UInt_t ppsNumber; ///< For matching up with thresholds etc.
   UInt_t numBytes; ///<Bytes in station readout
   UInt_t recordId; ///< Record Id
   ULong64_t timeStamp; ///< Timestamp
   UInt_t eventId; ///< Event Id
   UShort_t numReadoutBlocks; ///< Number of readout blocks which follow header

   std::vector<RawAraOneStationBlock> blockVec;

  ClassDef(RawAraOneStationEvent,1);
};




#endif //RAWARAONESTATIONEVENT
