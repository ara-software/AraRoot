//////////////////////////////////////////////////////////////////////////////
/////  RawAraStationEvent.cxx        Raw ARA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Base class for ARA Event data. Dummy class, should use inherited 
/////     class instead.
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#include "RawAraStationEvent.h"


ClassImp(RawAraStationEvent);



RawAraStationEvent::RawAraStationEvent(UInt_t stationId) 
{
  fStationId=stationId;
}

RawAraStationEvent::RawAraStationEvent() 
{
  fStationId=0x0;
}

RawAraStationEvent::~RawAraStationEvent()
{

}
