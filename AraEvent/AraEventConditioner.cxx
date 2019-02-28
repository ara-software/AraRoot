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
#include <sstream>

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
  theEvent->fIsConditioned = true; //mark the event as conditioned
  //now we're done
}


//! Inverts channels 0, 4, and 8 on A3
/*!
  \param ev the useful atri event pointer
  \return void
*/
void AraEventConditioner::invertA3Chans(UsefulAtriStationEvent *theEvent){
  //make a list of channels we want to invert
  std::vector<Int_t> list_to_invert;
  list_to_invert.push_back(0);
  list_to_invert.push_back(4);
  list_to_invert.push_back(8);

  //loop over that list and invert them (multiply by -1)
  for(int i=0; i<list_to_invert.size(); i++){
    //get the elec chan
    Int_t rf_chan = list_to_invert[i];
    Int_t elec_chan = AraGeomTool::Instance()->getElecChanFromRFChan(rf_chan, theEvent->stationId);
    
    //perform inversion on every sample
    for(Int_t samp=0; samp<theEvent->fTimes[elec_chan].size(); samp++){
      theEvent->fVolts[elec_chan][samp]*=-1.;
    }

    //record the inversion
    std::stringstream ss;
    ss<<"invert_ch"<<rf_chan;
    theEvent->fConditioningList.push_back(ss.str());
  }
}