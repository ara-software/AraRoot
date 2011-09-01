//////////////////////////////////////////////////////////////////////////////
/////  UsefulAraStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAraStationEvent.h"
#include "FFTtools.h"
#include "AraGeomTool.h"
#include "TH1.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(UsefulAraStationEvent);

UsefulAraStationEvent::UsefulAraStationEvent() 
{
   //Default Constructor
}

UsefulAraStationEvent::~UsefulAraStationEvent() {
   //Default Destructor
}


TGraph *UsefulAraStationEvent::getGraphFromElecChan(int /*chan*/)
{
  std::cerr << "Error calling UsefulAraStationEvent::getGraphFromElecChan()\n";
  return NULL;
}

TGraph *UsefulAraStationEvent::getGraphFromRFChan(int /*chan*/)
{
  std::cerr << "Error calling UsefulAraStationEvent::getGraphFromRFChan()\n";
  return NULL;
}

Int_t UsefulAraStationEvent::getNumElecChannels()
{
  std::cerr << "Error calling UsefulAraStationEvent::getNumElecChannels()\n";
  return -1;
  
}

Int_t UsefulAraStationEvent::getNumRFChannels()
{
  std::cerr << "Error calling UsefulAraStationEvent::getNumRFChannels()\n";
  return -1;
  
}
