//////////////////////////////////////////////////////////////////////////////
/////  UsefulAtriStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAtriStationEvent.h"
#include "AraEventCalibrator.h"
#include "FFTtools.h"
#include "AraGeomTool.h"
#include "TH1.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(UsefulAtriStationEvent);

AraEventCalibrator *fCalibrator;

UsefulAtriStationEvent::UsefulAtriStationEvent() 
{
   //Default Constructor
  fNumChannels=0;
  fCalibrator=0;
}

UsefulAtriStationEvent::~UsefulAtriStationEvent() {
   //Default Destructor
  fNumChannels=0;
  fCalibrator=0;
}

UsefulAtriStationEvent::UsefulAtriStationEvent(RawAtriStationEvent *rawEvent, AraCalType::AraCalType_t calType)
 :RawAtriStationEvent(*rawEvent)
{
  fCalibrator=AraEventCalibrator::Instance();
  fNumChannels=0;
  fCalibrator->calibrateEvent(this,calType);
  //  fprintf(stderr, "UsefulAtriStationEvent::UsefulAtriStationEvent() -- finished constructing event\n");  //DEBUG

}


TGraph *UsefulAtriStationEvent::getGraphFromElecChan(int chanId)
{
  std::map< Int_t, std::vector <Double_t> >::iterator timeMapIt;
  timeMapIt=fTimes.find(chanId);
  if(timeMapIt==fTimes.end()) {
    // This channel doesn't exist. We don't return a null pointer,
    // we return an empty graph.
    return new TGraph;
  }
  
  TGraph *gr = new TGraph(fTimes[chanId].size(),&(fTimes[chanId][0]),&(fVolts[chanId][0]));
  return gr;
}

TGraph *UsefulAtriStationEvent::getGraphFromRFChan(int chan)
{
  return getGraphFromElecChan(chan);
  std::cerr << "Error calling UsefulAtriStationEvent::getGraphFromRFChan()\n";
  return NULL;
}
