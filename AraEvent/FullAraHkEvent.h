//////////////////////////////////////////////////////////////////////////////
/////  FullAraHkEvent.h        Raw ARA hk class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef FULLARAHKEVENT_H
#define FULLARAHKEVENT_H

//Includes
#include <TObject.h>
#include "araStructures.h"
#include "araDefines.h"
#include "AraTriggerMonitor.h"
#include "AraHkData.h"

//!  FullAraHkEvent -- The ARA Hk Data
/*!
  The ROOT implementation of the raw ARA housekeeping data
  \ingroup rootclasses
*/
class FullAraHkEvent: public TObject
{
 public:
   FullAraHkEvent(); ///< Default constructor
   FullAraHkEvent(AraHkBody_t *theHk); ///< Assignment constructor
   ~FullAraHkEvent(); ///< Destructor

   unsigned int unixTime;
   unsigned int unixTimeUs;
   unsigned int eventNumber;
   unsigned int errorFlag;
   
   AraTriggerMonitor trig; ///< The trigger
   AraHkData hk; ///< The hk
   
   

  ClassDef(FullAraHkEvent,1);
};


#endif //FULLARAHKEVENT_H
