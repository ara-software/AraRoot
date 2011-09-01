//////////////////////////////////////////////////////////////////////////////
/////  RawAraOneSimpleStationEvent.h        Raw ARA event class          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARAONESIMPLESTATIONEVENT_H
#define RAWARAONESIMPLESTATIONEVENT_H

//Includes
#include <TObject.h>
#include "araOneStructures.h"
#include "araDefines.h"
#include "RawAraStationEvent.h"
#include "RawAraTestBedStationHeader.h"
#include "AraRawTestBedRFChannel.h"
#include "AraTestBedTriggerMonitor.h"
#include "AraTestBedHkData.h"

//!  RawAraOneSimpleStationEvent -- The Raw ARA Event Data
/*!
  The ROOT implementation of the raw ARA event data
  \ingroup rootclasses
*/
class RawAraOneSimpleStationEvent: public RawAraStationEvent
{
 public:
   RawAraOneSimpleStationEvent(); ///< Default constructor
   RawAraOneSimpleStationEvent(AraSimpleStationEvent_t *theBody); ///< Assignment constructor
   ~RawAraOneSimpleStationEvent(); ///< Destructor

   Int_t getNumChannels() {return NUM_DIGITIZED_TESTBED_CHANNELS;}


   //The data   
   ULong64_t unixTime; ///<The time in seconds
   UInt_t unixTimeUs; ///<The time in microseconds
   UInt_t eventNumber; ///< The event number
   UInt_t eventId[DDA_PER_ATRI]; ///< Another event number
   UShort_t blockId[DDA_PER_ATRI]; ///< Block Id
   UShort_t samples[DDA_PER_ATRI][512]; ///<The samples


  ClassDef(RawAraOneSimpleStationEvent,1);
};




#endif //RAWARAEVENT_H
