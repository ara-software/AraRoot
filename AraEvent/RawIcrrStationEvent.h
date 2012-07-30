//////////////////////////////////////////////////////////////////////////////
/////  RawIcrrStationEvent.h        Raw ARA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWICRRSTATIONEVENT_H
#define RAWICRRSTATIONEVENT_H

//Includes
#include <TObject.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include "RawAraStationEvent.h"
#include "RawIcrrStationHeader.h"
#include "AraRawIcrrRFChannel.h"
#include "IcrrTriggerMonitor.h"
#include "IcrrHkData.h"

//!  RawIcrrStationEvent -- The Raw ARA Event Data
/*!
  The ROOT implementation of the raw ARA event data for the Icrr electronics type stations (TestBed and Station1). Contained in this class are the header information, raw ADC values, house keeping, trigger information etc. There are functions for returning the raw electronics waveforms.
  \ingroup rootclasses
*/
class RawIcrrStationEvent: public RawAraStationEvent
{
 public:
   RawIcrrStationEvent(); ///< Default constructor
   RawIcrrStationEvent(IcrrEventBody_t *theBody); ///< Assignment constructor
   RawIcrrStationEvent(IcrrEventBody_t *theBody, UInt_t stationId); ///< Assignment constructor to create RawAraGenericHeader


   ~RawIcrrStationEvent(); ///< Destructor

   //Important Stuff
   UInt_t whichPeds; ///< Timestamp of pedestals used in subtraction

   //The data
   RawIcrrStationHeader head; ///< The header
   AraRawIcrrRFChannel chan[NUM_DIGITIZED_ICRR_CHANNELS]; ///< RawRF data
   IcrrTriggerMonitor trig; ///< The trigger
   IcrrHkData hk; ///< The hk
   
   Int_t getNumChannels() {return NUM_DIGITIZED_ICRR_CHANNELS;}

   Int_t getLabChip(Int_t chanIndex) {
     return chan[chanIndex].getLabChip();
   } ///< Returns the LABRADOR number
   Int_t getRCO(Int_t chanIndex) {
     return chan[chanIndex].getRCO();
   } ///< Returns the RCO phase
   Int_t getFirstHitBus(Int_t chanIndex) {
     return chan[chanIndex].getFirstHitBus();
   } ///< Returns the firstHitbus value for the channel
   Int_t getLastHitBus(Int_t chanIndex) {
     return chan[chanIndex].getLastHitBus();
   } ///< Returns the lastHitbus value for the channel
   Int_t getWrappedHitBus(Int_t chanIndex) {
     return chan[chanIndex].getWrappedHitBus();
   } ///< Return the wrapped hitbus flag for the channel. When the HITBUS is wrapped the waveform runs from firstHitbus+1 to lastHitbus-1, otherwise it runs from lastHitbus+1 to firstHitbus-1 (crossing the 259-->0 boudnary).

   Int_t getLatestSample(Int_t chanIndex); ///<Returns the latest sample in the waveform
   Int_t getEarliestSample(Int_t chanIndex); ///< Returns the earliest sample in the waveform
   Int_t isInTrigPattern(int bit) {return trig.isInTrigPattern(bit);} ///<Returns whetehr or not bit is in the trigger pattern
   UInt_t getRubidiumTriggerTime() {return trig.getRubidiumTriggerTime();} ///<Returns the 32-bit (well maybe 28-bit) trigger time
   Double_t getRubidiumTriggerTimeInSec() {return trig.getRubidiumTriggerTimeInSec();} ///< Returns the rubidium time divided by 280M


  ClassDef(RawIcrrStationEvent,1);
};




#endif //RAWARAEVENT_H
