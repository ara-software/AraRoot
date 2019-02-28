//////////////////////////////////////////////////////////////////////////////
/////  AraEventConditioner.h        Conditioner                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class for conditioningevents                           /////
/////  Author: Brian Clark (clark.2668@osu.edu)                          /////
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "UsefulAtriStationEvent.h"
#include "AraEventConditioner.h"
#include "AraGeomTool.h"

#include "TGraph.h"

ClassImp(AraEventConditioner);

AraEventConditioner * AraEventConditioner::fgInstance=0;

AraEventConditioner::AraEventConditioner()
{
  //nothing right now
}

AraEventConditioner::~AraEventConditioner()
{
  //nothing right now
}

AraEventConditioner*  AraEventConditioner::Instance()
{
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraEventConditioner();
  return fgInstance;
}

void AraEventConditioner::conditionEvent(UsefulAtriStationEvent *theEvent)
{

  AraStationId_t thisStationId = theEvent->stationId;

  if(theEvent->stationId==ARA_STATION3){
    AraEventConditioner::invertA3Chans(theEvent);
  }
  // theEvent->fIsConditioned = true; //mark the event as conditioned
  //now we're done
}


//! Inverts channels 0, 4, and 8 on A3
/*!
  \param ev the useful atri event pointer
  \return void
*/
void AraEventConditioner::invertA3Chans(UsefulAtriStationEvent *theEvent){
  int chan;
  
  chan=0;

}
