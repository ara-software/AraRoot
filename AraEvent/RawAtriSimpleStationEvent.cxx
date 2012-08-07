//////////////////////////////////////////////////////////////////////////////
/////  RawAtriSimpleStationEvent.cxx        ARA header reading class     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces      ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAtriSimpleStationEvent.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAtriSimpleStationEvent);

RawAtriSimpleStationEvent::RawAtriSimpleStationEvent()   
{
  
  //Default Constructor
}

RawAtriSimpleStationEvent::~RawAtriSimpleStationEvent() {
   //Default Destructor
}


RawAtriSimpleStationEvent::RawAtriSimpleStationEvent(AraSimpleStationEvent_t *bdPtr)
{
  unixTime=bdPtr->unixTime;
  unixTimeUs=bdPtr->unixTimeUs;
  eventNumber=bdPtr->eventNumber;
  memcpy(eventId,bdPtr->eventId,DDA_PER_ATRI*sizeof(UInt_t));
  memcpy(blockId,bdPtr->blockId,DDA_PER_ATRI*sizeof(UShort_t));
  memcpy(samples,bdPtr->samples,DDA_PER_ATRI*512*sizeof(UShort_t));
}
