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
#include <numeric>

#include "UsefulAtriStationEvent.h"
#include "AraEventConditioner.h"
#include "AraGeomTool.h"
#include "araSoft.h"

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

  if(theEvent->stationId==ARA_STATION3){
    AraEventConditioner::invertA3Chans(theEvent);
  }

  trimFirstBlock(theEvent);

  makeMeanZero(theEvent);
  
  theEvent->fIsConditioned = true; //mark the event as conditioned
}

//! Make the mean of the voltage samples zero again; to be used *after* trimFirstBlock
/*!
  \param ev the useful atri event pointer
  \return void
*/
void AraEventConditioner::makeMeanZero(UsefulAtriStationEvent *theEvent){
  for(Int_t chan=0; chan<theEvent->fTimes.size(); chan++){
    //compute the mean, and let C++ help by doing the addition for us
    Double_t mean = std::accumulate(theEvent->fVolts[chan].begin(), theEvent->fVolts[chan].end(), 0.0);
    mean/=double(theEvent->fVolts[chan].size()); //divide by N to make it a mean
    for(Int_t samp=0; samp<theEvent->fTimes[chan].size(); samp++){
      theEvent->fVolts[chan][samp]-=mean;
    }
    mean = std::accumulate(theEvent->fVolts[chan].begin(), theEvent->fVolts[chan].end(), 0.0);
  }
  //record the making of the zero mean
  std::stringstream ss;
  ss<<"make_mean_zero_all_chans";
  theEvent->fConditioningList.push_back(ss.str());
}

//! Trim the first block (SAMPLES_PER_BLOCK) from all graphs
/*!
  \param ev the useful atri event pointer
  \return void
*/
void AraEventConditioner::trimFirstBlock(UsefulAtriStationEvent *theEvent){
  bool hasTooFewBlocks=false;
  for(Int_t chan=0; chan<theEvent->fTimes.size(); chan++){
    if(theEvent->fTimes[chan].size()<64){
      hasTooFewBlocks=true;
      break;
    }
  }
  if(hasTooFewBlocks) return;
  for(Int_t chan=0; chan<theEvent->fTimes.size(); chan++){
    theEvent->fTimes[chan].erase(theEvent->fTimes[chan].begin(), theEvent->fTimes[chan].begin()+SAMPLES_PER_BLOCK);
    theEvent->fVolts[chan].erase(theEvent->fVolts[chan].begin(), theEvent->fVolts[chan].begin()+SAMPLES_PER_BLOCK);
  }
  //record the trimming
  std::stringstream ss;
  ss<<"trim_first_blocks_all_chans";
  theEvent->fConditioningList.push_back(ss.str());
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