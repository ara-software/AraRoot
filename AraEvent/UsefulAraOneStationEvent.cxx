//////////////////////////////////////////////////////////////////////////////
/////  UsefulAraOneStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAraOneStationEvent.h"
#include "FFTtools.h"
#include "AraGeomTool.h"
#include "TH1.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(UsefulAraOneStationEvent);

UsefulAraOneStationEvent::UsefulAraOneStationEvent() 
{
   //Default Constructor
  fNumChannels=0;
}

UsefulAraOneStationEvent::~UsefulAraOneStationEvent() {
   //Default Destructor
  fNumChannels=0;
}

UsefulAraOneStationEvent::UsefulAraOneStationEvent(RawAraOneStationEvent *rawEvent, AraCalType::AraCalType_t calType)
 :RawAraOneStationEvent(*rawEvent)
{
  fNumChannels=0;
}


TGraph *UsefulAraOneStationEvent::getGraphFromElecChan(int /*chan*/)
{
  std::cerr << "Error calling UsefulAraOneStationEvent::getGraphFromElecChan()\n";
  return NULL;
}

TGraph *UsefulAraOneStationEvent::getGraphFromRFChan(int /*chan*/)
{
  std::cerr << "Error calling UsefulAraOneStationEvent::getGraphFromRFChan()\n";
  return NULL;
}
