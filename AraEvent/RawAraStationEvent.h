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
  RawAraStationEvent(UInt_t stationId) ;
  RawAraStationEvent(UInt_t stationId, AraGenericHeader_t *gHdr) ;
  RawAraStationEvent();///< Default constructor
  ~RawAraStationEvent(); ///< Destructor
  
  UInt_t getStationId() {return fStationId;}
  Int_t getNumChannels() {return 0;}
  

  UInt_t fStationId; //Icrr1 = 0x0, ARA1 =0x1, ...


  ClassDef(RawAraStationEvent,1);
};


#endif //RAWARASTATIONEVENT_H
