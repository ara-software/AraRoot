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
#include <iostream>


ClassImp(AraStationInfo);

AraStationInfo::AraStationInfo()
  :fAntInfo(ANTS_PER_ATRI)
{
  numberRFChans=0;
  fNumberAntennas=0;

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

AraAntennaInfo *AraStationInfo::getNewAntennaInfo(int antNum){
  //Assume this creates a new AraAntennaInfo; 
  fNumberAntennas++;
  numberRFChans++;
  //  std::cout << "getNewAntennaInfo(" << antNum << ") fNumberAntennas=" << fNumberAntennas << "\n";
  //Magic lines below, may remove at some point
  int temp=fAntInfo[antNum].chanNum;
  fAntInfo[antNum].chanNum=temp;
  return &fAntInfo[antNum];
}

void AraStationInfo::fillAntIndexVec() {
  //For now will use hard coded numbers
  fAntIndexVec[0].resize(20);
  fAntIndexVec[1].resize(20);
  fAntIndexVec[2].resize(20);

  Int_t countPols[3]={0};
  for(int ant=0;ant<fNumberAntennas;++ant){
    int polTypeInt=(int)fAntInfo[ant].polType;
    int antNum=fAntInfo[ant].antPolNum;
    int daqNum=fAntInfo[ant].daqChanNum;
    //    std::cerr << ant << "\t" << polTypeInt << "\t" << antNum << "\t" << daqNum << "\n";
    fAntIndexVec[polTypeInt][antNum]=ant;//daqNum;
    countPols[polTypeInt]++;
  }
  for(int pol=0;pol<3;pol++) {
    fAntIndexVec[pol].resize(countPols[pol]);
  }
}

Int_t AraStationInfo::getRFChanByPolAndAnt(Int_t antNum, AraAntPol::AraAntPol_t polType) {
  //RJN will add checking here at some point
  if(polType>=0 && polType<=2) {
    if(antNum>=0 && antNum<=fAntIndexVec[polType].size()) {
      return fAntIndexVec[polType][antNum];
    }
  }
  return -1;
}


Int_t AraStationInfo::getElecChanFromRFChan(Int_t rfChan) {
  //Should add error checking here
  return fAntInfo[rfChan].daqChanNum;
}
