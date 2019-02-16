//////////////////////////////////////////////////////////////////////////////
/////  AraStationInfo.h       ARA Station Information                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about an Ara Stations   /////
/////  Author: Jonathan Davies (jdavies@hep.ucl.ac.uk)                   /////
/////          & Ryan Nichol (rjn@hep.ucl.ac.uk)                         /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARASTATIONINFO_H
#define ARASTATIONINFO_H

//Includes
#include <TObject.h>
#include "araIcrrDefines.h"
#include "AraAntennaInfo.h"
#include "AraCalAntennaInfo.h"
#include "araSoft.h"

#include <vector>
//!  Part of the AraEvent library. Stores information about the station's location and the antenna information. Populated by AraGeomTool
/*!
  A simple class for storing information about an Ara Station
  \ingroup rootclasses
*/

class AraStationInfo: public TObject
{
  
 public:
  
  AraStationInfo(); ///< Default constructor
  AraStationInfo(Double_t unixtime, AraStationId_t stationId); ///< Assignment constructor ///unixtime argument added by UAL 01/25/2019
  ~AraStationInfo(); ////< Destructor

  Int_t getNumCalAnts() {return fNumberCalAntennas;}
  Int_t getNumAnts() {return fNumberAntennas;}
  void setNumAnts(int numAnts) { fNumberAntennas=numAnts;}

  Int_t getNumRFChans() {return numberRFChans;}
  void setNumRFChans(int numChans) { numberRFChans=numChans;}

  Double_t getCableDelay(int rfChanNum);
  AraAntennaInfo *getAntennaInfoForTrigChan(int trigChan);
  AraAntennaInfo *getAntennaInfo(int antNum, AraAntPol::AraAntPol_t polType) { return getAntennaInfo(getRFChanByPolAndAnt(antNum,polType));}
  AraAntennaInfo *getAntennaInfo(int rfChanNum);
  AraCalAntennaInfo *getCalAntennaInfo(int calAntId); ///< Simple 0-3 numeric id


  Int_t getRFChanByPolAndAnt(Int_t antNum, AraAntPol::AraAntPol_t polType);
  Int_t getElecChanFromRFChan(Int_t rfChan);
  Int_t getNumAntennasByPol(AraAntPol::AraAntPol_t polType) {return fAntIndexVec[polType].size();}

  //Should add some error checking at some point
  Double_t getLowPassFilter(int rfChan) { return fAntInfo[rfChan].lowPassFilterMhz; }
  Double_t getHighPassFilter(int rfChan) { return fAntInfo[rfChan].highPassFilterMhz; }


  //Below are just for ICRR station
   //Maybe they should live elsewhere?
  int getIcrrChanIndex(AraLabChip::AraLabChip_t chip, int chan) {return chip*CHANNELS_PER_LAB3 +chan;}
  AraLabChip::AraLabChip_t getLabChipForChan(int rfChan) {return fAntInfo[rfChan].labChip;}
  int getNumLabChansForChan(int rfChan) { return fAntInfo[rfChan].numLabChans;}
  int getFirstLabChanForChan(int rfChan) { return fAntInfo[rfChan].labChans[0];}
  int getSecondLabChanForChan(int rfChan) { return fAntInfo[rfChan].labChans[1];}
  int getFirstLabChanIndexForChan(int rfChan) { return getIcrrChanIndex(getLabChipForChan(rfChan),getFirstLabChanForChan(rfChan));}
  int getSecondLabChanIndexForChan(int rfChan) { return getIcrrChanIndex(getLabChipForChan(rfChan),getSecondLabChanForChan(rfChan));}
  int isDiplexed(int rfChan) {return fAntInfo[rfChan].isDiplexed;}
  
  
  //Labelling for TDA channels just for the ATRI stations
  const char *getAtriSingleChannelLabel(Int_t tda, Int_t channel);
  const char *getAtriL2Label(Int_t index);
  const char *getAtriL3Label(Int_t index);
  const char *getAtriL4Label(Int_t index);
  static Int_t getTrigChan(Int_t tda, Int_t channel) {
     return channel + ANTS_PER_TDA*tda;
  }
  


  
  //At some point will make these private
  AraStationId_t fStationId;
  std::vector<AraAntennaInfo> fAntInfo; ///< One object per antenna
  std::vector<AraCalAntennaInfo> fCalAntInfo; ///< Vector of local station Calibration Pulsers


  int numberRFChans;  ///< Currently a copy of fNumberAntennas
  int fNumberAntennas; ///<This is the number of antennas on an ATRI
  int fNumberCalAntennas; ///< Number of calibration antennas
  
  std::vector<int> fAntIndexVec[3]; ///<The antenna to logical channel index one vector per polarisation
  std::vector<int> fTrigChanVec; ///< The index that converts trigger channel to


 private:
  ///These are helper functions that should not be called
  AraAntennaInfo *getNewAntennaInfo(int rfChanNum);
  AraCalAntennaInfo *getNewCalAntennaInfo(int antCalId);
  void fillAntIndexVec();
  void fillTrigChanVec();
  void readChannelMapDbAtri();
  void readChannelMapDbAtri_2(Int_t yrtime);
  void readChannelMapDbIcrr();


  void readCalPulserDb();

  ClassDef(AraStationInfo,1);
};

#endif //ARASTATIONINFO_H
