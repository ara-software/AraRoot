//////////////////////////////////////////////////////////////////////////////
/////  FullIcrrHkEvent.h        Raw ARA hk class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef FULLICRRHKEVENT_H
#define FULLICRRHKEVENT_H

//Includes
#include <TObject.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include "IcrrTriggerMonitor.h"
#include "IcrrHkData.h"

//!  FullIcrrHkEvent -- The ARA Hk Data
/*!
  The ROOT implementation of the raw ARA housekeeping data
  \ingroup rootclasses
*/
class FullIcrrHkEvent: public TObject
{
 public:
   FullIcrrHkEvent(); ///< Default constructor
   FullIcrrHkEvent(IcrrHkBody_t *theHk); ///< Assignment constructor
   ~FullIcrrHkEvent(); ///< Destructor

   unsigned int unixTime;
   unsigned int unixTimeUs;
   unsigned int eventNumber;
   unsigned int errorFlag;
   
   IcrrTriggerMonitor trig; ///< The trigger
   IcrrHkData hk; ///< The hk
   
   

  ClassDef(FullIcrrHkEvent,1);
};


#endif //FULLICRRHKEVENT_H
