//////////////////////////////////////////////////////////////////////////////
/////  AraStationInfo.h       ARA Station Information                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about an Ara Stations   /////
/////  Author: Jonathan Davies (jdavies@hep.ucl.ac.uk)                   /////
/////          & Ryan Nichol (rjn@hep.ucl.ac.uk)                         /////
//////////////////////////////////////////////////////////////////////////////

//Includes
#include "AraStationInfo.h"



ClassImp(AraStationInfo);

AraStationInfo::AraStationInfo()
{

}
AraStationInfo::~AraStationInfo()
{

}

Double_t AraStationInfo::getCableDelay(int rfChanNum) {
  if(rfChanNum>=0 && rfChanNum<numberRFChans) {
    return fAntInfo[rfChanNum].getCableDelay();
  }
  return 0;
}

AraAntennaInfo *AraStationInfo::getAntennaInfo(int antNum) {
  if(antNum>=0 && antNum<numberRFChans) {
    return &fAntInfo[antNum];
  }
  return NULL;
}

