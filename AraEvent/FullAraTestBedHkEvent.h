//////////////////////////////////////////////////////////////////////////////
/////  FullAraTestBedHkEvent.h        Raw ARA hk class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef FULLARATESTBEDHKEVENT_H
#define FULLARATESTBEDHKEVENT_H

//Includes
#include <TObject.h>
#include "araTestbedStructures.h"
#include "araDefines.h"
#include "AraTestBedTriggerMonitor.h"
#include "AraTestBedHkData.h"

//!  FullAraTestBedHkEvent -- The ARA Hk Data
/*!
  The ROOT implementation of the raw ARA housekeeping data
  \ingroup rootclasses
*/
class FullAraTestBedHkEvent: public TObject
{
 public:
   FullAraTestBedHkEvent(); ///< Default constructor
   FullAraTestBedHkEvent(AraTestBedHkBody_t *theHk); ///< Assignment constructor
   ~FullAraTestBedHkEvent(); ///< Destructor

   unsigned int unixTime;
   unsigned int unixTimeUs;
   unsigned int eventNumber;
   unsigned int errorFlag;
   
   AraTestBedTriggerMonitor trig; ///< The trigger
   AraTestBedHkData hk; ///< The hk
   
   

  ClassDef(FullAraTestBedHkEvent,1);
};


#endif //FULLARATESTBEDHKEVENT_H
