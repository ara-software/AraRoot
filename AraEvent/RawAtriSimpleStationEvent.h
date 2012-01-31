//////////////////////////////////////////////////////////////////////////////
/////  RawAtriSimpleStationEvent.h        Raw ARA event class          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWATRISIMPLESTATIONEVENT_H
#define RAWATRISIMPLESTATIONEVENT_H

//Includes
#include <TObject.h>
#include "araOneStructures.h"
#include "araIcrrDefines.h"
#include "RawAraStationEvent.h"

//!  RawAtriSimpleStationEvent -- The Raw ARA Event Data
/*!
  The ROOT implementation of the raw ARA event data
  \ingroup rootclasses
*/
class RawAtriSimpleStationEvent: public RawAraStationEvent
{
 public:
   RawAtriSimpleStationEvent(); ///< Default constructor
   RawAtriSimpleStationEvent(AraSimpleStationEvent_t *theBody); ///< Assignment constructor
   ~RawAtriSimpleStationEvent(); ///< Destructor

   Int_t getNumChannels() {return NUM_DIGITIZED_ICRR_CHANNELS;}


   //The data   
   ULong64_t unixTime; ///<The time in seconds
   UInt_t unixTimeUs; ///<The time in microseconds
   UInt_t eventNumber; ///< The event number
   UInt_t eventId[DDA_PER_ATRI]; ///< Another event number
   UShort_t blockId[DDA_PER_ATRI]; ///< Block Id
   UShort_t samples[DDA_PER_ATRI][512]; ///<The samples


  ClassDef(RawAtriSimpleStationEvent,1);
};




#endif //RAWARAEVENT_H
