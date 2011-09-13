//////////////////////////////////////////////////////////////////////////////
/////  UsefulAraOneStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAraOneStationEvent.h"
#include "AraEventCalibrator.h"
#include "FFTtools.h"
#include "AraGeomTool.h"
#include "TH1.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(UsefulAraOneStationEvent);

AraEventCalibrator *fCalibrator;

UsefulAraOneStationEvent::UsefulAraOneStationEvent() 
{
   //Default Constructor
  fNumChannels=0;
  fCalibrator=0;
}

UsefulAraOneStationEvent::~UsefulAraOneStationEvent() {
   //Default Destructor
  fNumChannels=0;
  fCalibrator=0;
}

UsefulAraOneStationEvent::UsefulAraOneStationEvent(RawAraOneStationEvent *rawEvent, AraCalType::AraCalType_t calType)
 :RawAraOneStationEvent(*rawEvent)
{
  fCalibrator=AraEventCalibrator::Instance();
  fNumChannels=0;
  fCalibrator->calibrateEvent(this,calType);
}


TGraph *UsefulAraOneStationEvent::getGraphFromElecChan(int chanId)
{
  std::map< Int_t, std::vector <Double_t> >::iterator timeMapIt;
  timeMapIt=fTimes.find(chanId);
  if(timeMapIt==fTimes.end()) return NULL;
  
  TGraph *gr = new TGraph(fTimes[chanId].size(),&(fTimes[chanId][0]),&(fVolts[chanId][0]));
  return gr;
}

TGraph *UsefulAraOneStationEvent::getGraphFromRFChan(int chan)
{
  return getGraphFromElecChan(chan);
  std::cerr << "Error calling UsefulAraOneStationEvent::getGraphFromRFChan()\n";
  return NULL;
}
