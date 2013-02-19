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
  ~AraStationInfo(); ////< Destructor

  Int_t getNumAnts() {return fNumberAntennas;}
  void setNumAnts(int numAnts) { fNumberAntennas=numAnts;}

  Int_t getNumRFChans() {return numberRFChans;}
  void setNumRFChans(int numChans) { numberRFChans=numChans;}

  Double_t getCableDelay(int rfChanNum);
  AraAntennaInfo *getAntennaInfo(int rfChanNum);
  AraAntennaInfo *getNewAntennaInfo(int rfChanNum);

  Int_t getRFChanByPolAndAnt(Int_t antNum, AraAntPol::AraAntPol_t polType);
  Int_t getElecChanFromRFChan(Int_t rfChan);

  void fillAntIndexVec();

  
  //At some point will make these private
  std::vector<AraAntennaInfo> fAntInfo; ///< One object per antenna
  Double_t stationLocation[3]; ///< array-centric co-ordinates of the station
  int numberRFChans;  ///
  int fNumberAntennas; ///<This is the numebr of antennas on an ATRI
  
  std::vector<int> fAntIndexVec[3]; ///<The antenna to logical channel index one vector per polarisation

  ClassDef(AraStationInfo,1);
};

#endif //ARASTATIONINFO_H
