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
  AraStationInfo(AraStationId_t stationId); ///< Assignment constructor
  ~AraStationInfo(); ////< Destructor

  Int_t getNumCalAnts() {return fNumberCalAntennas;}
  Int_t getNumAnts() {return fNumberAntennas;}
  void setNumAnts(int numAnts) { fNumberAntennas=numAnts;}

  Int_t getNumRFChans() {return numberRFChans;}
  void setNumRFChans(int numChans) { numberRFChans=numChans;}

  Double_t getCableDelay(int rfChanNum);
  AraAntennaInfo *getAntennaInfo(int antNum, AraAntPol::AraAntPol_t polType) { return getAntennaInfo(getRFChanByPolAndAnt(antNum,polType));}
  AraAntennaInfo *getAntennaInfo(int rfChanNum);
  AraCalAntennaInfo *getCalAntennaInfo(int calAntId); ///< Simple 0-3 numeric id


  Int_t getRFChanByPolAndAnt(Int_t antNum, AraAntPol::AraAntPol_t polType);
  Int_t getElecChanFromRFChan(Int_t rfChan);
  Int_t getNumAntennasByPol(AraAntPol::AraAntPol_t polType) {return fAntIndexVec[polType].size();}


  
  //At some point will make these private
  AraStationId_t fStationId;
  std::vector<AraAntennaInfo> fAntInfo; ///< One object per antenna
  std::vector<AraCalAntennaInfo> fCalAntInfo; ///< Vector of local station Calibration Pulsers

  Double_t fStationLocation[3]; ///< array-centric co-ordinates of the station
  Double_t fRotationMatrix[3][3]; ///< Rotation matrix to convert from array centric to station centric

  int numberRFChans;  ///< Currently a copy of fNumberAntennas
  int fNumberAntennas; ///<This is the number of antennas on an ATRI
  int fNumberCalAntennas; ///< Number of calibration antennas
  
  std::vector<int> fAntIndexVec[3]; ///<The antenna to logical channel index one vector per polarisation


 private:
  ///These are helper functions that should not be called
  AraAntennaInfo *getNewAntennaInfo(int rfChanNum);
  AraCalAntennaInfo *getNewCalAntennaInfo(int antCalId);
  void fillAntIndexVec();
  void readChannelMapDbAtri();
  void readChannelMapDbIcrr();
  void readCalPulserDb();

  ClassDef(AraStationInfo,1);
};

#endif //ARASTATIONINFO_H
