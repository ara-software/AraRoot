//////////////////////////////////////////////////////////////////////////////
/////  AraCalAntennaInfo.h       ARA Cal Pulser Antenna Informatio       /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about an Ara            /////
//////   Calibration Antennas                                            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARACALANTENNAINFO_H
#define ARACALANTENNAINFO_H

//Includes
#include <TObject.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include "RawIcrrStationEvent.h"
#include "AraEventCalibrator.h"
#include "AraAntennaInfo.h"

//!  Part of AraEvent library. A class for storing information about the Antennae in the ARA stations.
/*!
  A simple class for storing information about an Ara Antenna
  \ingroup rootclasses
*/


class AraCalAntennaInfo: public TObject
{
 public:
   AraCalAntennaInfo(); ///< Default constructor
   ~AraCalAntennaInfo(); ///< Destructor


   const char *getCalAntName();
   void printAntennaInfo();  ///< Printssome information about the antenna
   Double_t getCableDelay() {return cableDelayCalib;}   ///< Returns the calibrated cableDelay
   Double_t *getLocationXYZ() {return antLocationCalib;}  ///< Returns the calibrated station-centric coordinates
   Double_t *getLocationENU() { return antLocationArrayCalib;}
   void fillArrayCoords();


   AraStationId_t fStationId;
   Int_t calAntId; ///< Just a logical antenna numbering
   AraAntType::AraAntType_t antType; ///< What kind of antenna is it
   AraAntPol::AraAntPol_t polType; ///< What is the polarisation
   char locationName[CAL_ANTS_PER_ATRI];  ///< Which hole e.g. BH5 or BH6
   char antName[CAL_ANTS_PER_ATRI]; ///< Simple antenna name e.g. CH1
   char pulserName[CAL_ANTS_PER_ATRI]; ///< Simple pulser name e.g. P1
   Double_t antLocation[3]; ///< Default x,y,z in m
   Double_t antLocationCalib[3]; ///< Calibrated x,y,z in m
   Double_t antLocationArray[3]; ///< Default x,y,z in m
   Double_t antLocationArrayCalib[3]; ///< Calibrated x,y,z in m
   Double_t cableDelay; ///< Default cable delay in ns
   Double_t cableDelayCalib; ///< Calibrated cable delay in ns
    

  ClassDef(AraCalAntennaInfo,1);
};


#endif //ARACALANTENNAINFO_H
