//////////////////////////////////////////////////////////////////////////////
/////  AraEvenetConditioner.h        Conditioner                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class for conditioningevents                           /////
/////  Author: Brian Clark (clark.2668@osu.edu)                          /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAEVENTCONDITIONER_H
#define ARAEVENTCONDITIONER_H

//Includes
#include <TObject.h>
#include "araSoft.h"
#include "araAtriStructures.h"
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"

class UsefulAtriStationEvent;

//!  Part of AraEvent library. The conditioner takes real ATRI pointer and applies some conditioning after calibration
/*!
  The Ara Event Conditioner
  \ingroup rootclasses
*/
class AraEventConditioner : public TObject
{
  public:
    AraEventConditioner(); ///< Default constructor
    ~AraEventConditioner(); ///< Destructor

   //Instance generator
   static AraEventConditioner*  Instance(); ///< Generates an instance of AraEventConditioner to use

   //function to condition an event
   void conditionEvent(UsefulAtriStationEvent *theEvent);

  private:
    void invertA3Chans(UsefulAtriStationEvent *theEvent);

  protected:
    static AraEventConditioner *fgInstance;  ///< protect against multiple instances

  ClassDef(AraEventConditioner,1);
};

#endif //ARAEVENTCONDITIONER_H
