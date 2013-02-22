//////////////////////////////////////////////////////////////////////////////
/////  AraAntennaInfo.h       ARA Antenna Information                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about an Ara Antenna    /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAANTENNAINFO_H
#define ARAANTENNAINFO_H

//Includes
#include <TObject.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include "RawIcrrStationEvent.h"
#include "AraEventCalibrator.h"


namespace AraAntType {
  typedef enum EAraAntType {
    kBicone = 1,
    kBowtieSlot = 2,
    kDiscone = 3,
    kBatwing = 4,
    kFatDipole =5,
    kQuadSlot = 6
  } AraAntType_t;
  const char *antTypeAsString(AraAntType::AraAntType_t antType); ///< Returns the antenna type as a string

}

namespace AraAntPol {
  typedef enum EAraAntPol {
    kVertical = 0,
    kHorizontal = 1,
    kSurface = 2,
    kNotAPol = 3
  } AraAntPol_t;
  const char *antPolAsString(AraAntPol::AraAntPol_t antPol); ///<Returns the antenna polarisation as a string
}

namespace AraDaqChanType {
  typedef enum EAraDaqChanType {
    kDisconeChan =1,
    kBatwingChan =2
  } AraDaqChanType_t;
}

namespace AraLabChip {
  typedef enum EAraLabChip {
    kA = 0,
    kB = 1,
    kC = 2
  } AraLabChip_t;
  const char *labChipAsString(AraLabChip::AraLabChip_t labChip);
}

namespace AraAntDir {
  typedef enum EAraAntDir {
    kReceiver = 1,
    kTransmitter = 2
  } AraAntDir_t;
}
    

//JPD Depricated
/* namespace AraSurfaceOrientation { */
/*   typedef enum EAraSurfaceOrientation { */
/*     kNorthSouth =1, */
/*     kEastWest =2 */
/*   } AraSurfaceOrientation_t; */
/* } */

//!  Part of AraEvent library. A class for storing information about the Antennae in the ARA stations.
/*!
  A simple class for storing information about an Ara Antenna
  \ingroup rootclasses
*/


class AraAntennaInfo: public TObject
{
 public:
   AraAntennaInfo(); ///< Default constructor
   ~AraAntennaInfo(); ///< Destructor

   void printAntennaInfo();
   const char *getDaqBoxChan();
   Double_t getCableDelay() {return cableDelay;}  
   Double_t *getLocationXYZ() {return antLocation;}  ///< Returns the calibrated station-centric coordinates
   Double_t *getLocationENU() { return antLocationArray;}
   void fillArrayCoords();

   AraStationId_t fStationId;

   Int_t chanNum;
   AraDaqChanType::AraDaqChanType_t daqChanType;
   Int_t daqChanNum;
   Double_t highPassFilterMhz;
   Double_t lowPassFilterMhz;

   //ICRR Specific Numbers
   Int_t daqTrigChan;
   Int_t numLabChans;
   AraLabChip::AraLabChip_t labChip;
   Int_t labChans[2]; ///<These will count from 0
   Int_t isDiplexed; ///< Depricated attempt at un-diplexing//FIXME
   Int_t diplexedChans[2]; ///< Depricated attempt at un-diplexing//FIXME
   
   //ATRI Specific Numbers
   Int_t ddaNum; ///< Counting from 0;
   Int_t ddaChanNum; //< Counting from 0


   Int_t preAmpNum;
   Double_t avgNoiseFigure;
   Int_t rcvrNum;
   char designator[3];
   Int_t antPolNum;
   AraAntType::AraAntType_t antType;
   AraAntPol::AraAntPol_t polType;
   char locationName[4];
   Double_t antLocation[3]; ///< Station-centric antenna location x,y,z in m
   Double_t antLocationArray[3]; ///< Array-centric antenna location x,y,z in m
   Double_t cableDelay; ///< In ns
   AraAntDir::AraAntDir_t antDir;
   // JPD depricated - now using an array of doubles
   //   AraSurfaceOrientation::AraSurfaceOrientation_t antOrient; ///<Only for surface antennas 
   Double_t antOrient[3];

   Double_t debugHolePosition[3]; ////< x,y,z in m
   Double_t debugPreAmpDz; ///< in m
   Double_t debugHolePositionZft; ///< in ft
   Double_t debugHolePositionZm; ///< in m
   Double_t debugTrueAsBuiltPositon[3]; ///< x,y,z in m
   Double_t debugCableDelay2; //in ns
   Double_t debugFeedPointDelay; //in ns
   Double_t debugTotalCableDelay; //in ns
     
       

  ClassDef(AraAntennaInfo,1);
};


#endif //ARAANTENNAINFO_H
