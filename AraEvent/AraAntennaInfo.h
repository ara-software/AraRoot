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

   void printAntennaInfoIcrr();
   void printAntennaInfoAtri();
   const char *getDaqBoxChan();

   const char *getRFChanName();

   Double_t getCableDelay() {return cableDelay;}  
   Double_t *getLocationXYZ() {return antLocation;}  ///< Returns the calibrated station-centric coordinates
   Double_t *getLocationENU() { return antLocationArray;}
   void fillArrayCoords();

   Int_t getTrigChan() { return daqTrigChan;}


   AraStationId_t fStationId;

   //ICRR Specific Numbers

   AraDaqChanType::AraDaqChanType_t daqChanType;
   Int_t numLabChans;
   AraLabChip::AraLabChip_t labChip;
   Int_t labChans[2]; ///<These will count from 0
   Int_t isDiplexed; ///< Depricated attempt at un-diplexing//FIXME
   Int_t diplexedChans[2]; ///< Depricated attempt at un-diplexing//FIXME
   Int_t preAmpNum;
   Int_t rcvrNum;

   char locationName[4];
   Double_t antLocationArray[3]; ///< Array-centric antenna location x,y,z in m
   AraAntDir::AraAntDir_t antDir; 

   Double_t debugHolePosition[3]; ////< x,y,z in m
   Double_t debugPreAmpDz; ///< in m
   Double_t debugHolePositionZft; ///< in ft
   Double_t debugHolePositionZm; ///< in m
   Double_t debugTrueAsBuiltPositon[3]; ///< x,y,z in m
   Double_t debugCableDelay2; //in ns
   Double_t debugFeedPointDelay; //in ns
   Double_t debugTotalCableDelay; //in ns
     



   //ATRI & ICRR -- Common data types
   Int_t chanNum; ///< RFChannel number - 0-3 TV 4-7 BV 8-11 TH 12-15 BH 16-19 Surface 
   AraAntPol::AraAntPol_t polType; ///< Polarisation of antenna (vertical, horizontal, surface);
   Int_t antPolNum; ///< The antenna number for this polarisation (0-7 for HPol, 0-7 for VPol and 0-3 for Surface)

   Int_t daqChanNum; ///< The electronics channel number for this antenna (0-7 DDA1, 8-15 DDA2, 16-23 DDA3, 24-31 DDA4)
   Int_t daqTrigChan; ///< The trigger channel number -- Need to confirm should be (daqChanNum % 8 ) + (daqChanNum / 8 )
   Int_t foamId; ///< ID of the Fibre Optic Amplification Module (FOAM) for this channel
   Int_t foamChanNum; ///< FOAM channel for this antenna
   AraAntType::AraAntType_t antType; ///< Type of antenna used

   Double_t antLocation[3]; ///< x,y,z in m
   Double_t calibAntLocation[3]; //< x,y,z in m from calibration
   Double_t cableDelay; ///< In ns
   Double_t calibCableDelay; ///< In ns from calibration


   Double_t antOrient[3]; ///< The orientation of surface antennas

   Double_t highPassFilterMhz; ///< High Pass filter in the DAQ box signal chain
   Double_t lowPassFilterMhz; ///< Low pass filter in the DAQ box signal chain

   Double_t avgNoiseFigure; ///< Some measure of signal chain average noise figure

   //RJN change to fix bug
   char designator[12]; ///< Hole+Antenna (e.g. BH2TVPol)

   //ATRI -- new data
   char holeName[6];
   char antName[6];
   


   //More information 
   
   



  ClassDef(AraAntennaInfo,1);
};


#endif //ARAANTENNAINFO_H
