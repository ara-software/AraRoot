//////////////////////////////////////////////////////////////////////////////
/////  RawAraStationEvent.h        Raw ARA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Base class for ARA Event data. Dummy class, should use inherited 
/////     class instead.
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARASTATIONEVENT_H
#define RAWARASTATIONEVENT_H

//Includes
#include <TObject.h>
#include <RawAraGenericHeader.h>

//!  RawAraStationEvent -- Base Class for Station Event Data
/*!
  The ROOT implementation of a base class for station event data. Should not be used directly instead one of the inherited classes should be used.
  \ingroup rootclasses
*/
class RawAraStationEvent: public TObject, public RawAraGenericHeader {

 public:
  RawAraStationEvent(AraStationId_t stationId); ///< Constructor to be called for Icrr type stations
  RawAraStationEvent(AraGenericHeader_t *gHdr); ///< Constructor to be called for Atri type stations
  RawAraStationEvent();///< Default constructor
  ~RawAraStationEvent(); ///< Destructor
  
  Int_t getNumChannels(){ return 0; };

  ClassDef(RawAraStationEvent,1);
};


#endif //RAWARASTATIONEVENT_H
