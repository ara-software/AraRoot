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

//!  AraStationInfo -- ARA Station Information
/*!
  A simple class for storing information about an Ara Station
  \ingroup rootclasses
*/

class AraStationInfo: public TObject
{
  
 public:
  
  AraStationInfo(); ///< Default constructor
  ~AraStationInfo(); ////< Destructor
  
  AraAntennaInfo fAntInfo[TOTAL_ANTS_PER_ICRR]; ///< One object per antenna
  Double_t stationLocation[3]; ///< array-centric co-ordinates of the station


  ClassDef(AraStationInfo,1);
};

#endif //ARASTATIONINFO_H
