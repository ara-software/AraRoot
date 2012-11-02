//////////////////////////////////////////////////////////////////////////////
/////  AraGeomTool.h       ARA Geometry tool                             /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class working out what is where                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAGEOMTOOL_H
#define ARAGEOMTOOL_H

//Includes
#include <TObject.h>
#include <TMath.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include "araAtriStructures.h"
#include "AraAntennaInfo.h"
#include "AraStationInfo.h"

//! Part of AraEvent library. Loads and stores information about each station's geometry as well as information about the antennae (filters, positions, channels etc...).
/*!
  The Ara geometry and numbering tool
  \ingroup rootclasses
*/
class AraGeomTool
{
 public:
   AraGeomTool(); ///< Default constructor
   ~AraGeomTool(); ///< Destructor

   //   AraAntennaInfo *getAntByRfChan(int chan);//FIXME
   //   AraAntennaInfo *getAntByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum);//FIXME
   int getChanIndex(AraLabChip::AraLabChip_t chip, int chan) {return chip*CHANNELS_PER_LAB3 +chan;}

   AraLabChip::AraLabChip_t getLabChipForChan(int chan, AraStationId_t stationId) {return fStationInfo[stationId].fAntInfo[chan].labChip;}

   int getNumLabChansForChan(int chan, AraStationId_t stationId) { return fStationInfo[stationId].fAntInfo[chan].numLabChans;}
   int getFirstLabChanForChan(int chan, AraStationId_t stationId) { return fStationInfo[stationId].fAntInfo[chan].labChans[0];}
   int getSecondLabChanForChan(int chan, AraStationId_t stationId) { return fStationInfo[stationId].fAntInfo[chan].labChans[1];}


   int getFirstLabChanIndexForChan(int chan, AraStationId_t stationId) { return getChanIndex(getLabChipForChan(chan, stationId),getFirstLabChanForChan(chan, stationId));}


   int getSecondLabChanIndexForChan(int chan, AraStationId_t stationId) { return getChanIndex(getLabChipForChan(chan, stationId),getSecondLabChanForChan(chan, stationId));}

   int isDiplexed(int chan, AraStationId_t stationId) {return fStationInfo[stationId].fAntInfo[chan].isDiplexed;}

   Double_t getLowPassFilter(int chan, AraStationId_t stationId) { return fStationInfo[stationId].fAntInfo[chan].lowPassFilterMhz; }

   Double_t getHighPassFilter(int chan, AraStationId_t stationId) { return fStationInfo[stationId].fAntInfo[chan].highPassFilterMhz; }


   //This is the new version this function
   int getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum, AraStationId_t stationId);

   //FIXME -- Only used by web-plotter. Should be fixed when the Web-Plotter is up and running
   int getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum);//FIXME

   AraAntPol::AraAntPol_t getPolByRFChan(int rfChan, AraStationId_t stationId);
   Int_t getAntNumByRFChan(int rfChan, AraStationId_t stationId);

   
   Double_t calcDeltaTInfinity(Double_t ant1[3], Double_t ant2[3],Double_t phiWave, Double_t thetaWave);
   Double_t calcDeltaTR(Double_t ant1[3], Double_t ant2[3], Double_t phiWave, Double_t thetaWave,Double_t R);
   
   Double_t calcDeltaTInfinity(Int_t chan1, Int_t chan2,Double_t phiWave, Double_t thetaWave, AraStationId_t stationId);
   Double_t calcDeltaTR(Int_t chan1, Int_t chan2, Double_t phiWave, Double_t thetaWave,Double_t R, AraStationId_t stationId);
   

   //Utility functions
   static bool isIcrrStation(AraStationId_t stationId); ///< Returns TRUE if the station is an ICRR station and false otherwise

   static bool isAtriStation(AraStationId_t stationId); ///< Returns TRUE if the station is an ATRIA station and false otherwise

   static Int_t getStationCalibIndex(AraStationId_t stationId); ///< Used by the calibrator. This function returns the calibration and pedestal index for a stationId (TESTBED==0, STATION1==1, STATION1A==0, STATION2==1, STATION3==2...)

   static void printStationName(AraStationId_t stationId); ///< Print to stdout the station Name

   static char *getStationName(AraStationId_t stationId); ///< Return char* with the station Name from the stationId

   //Instance generator
   static AraGeomTool*  Instance();
   
   AraStationInfo fStationInfo[ICRR_NO_STATIONS]; //station info contains the antenna info and station information
   int fAntLookupTable[ICRR_NO_STATIONS][3][8]; //At some point should lose the magic numbers
   
   //Some variables to do with ice properties
   static Double_t nTopOfIce;

  
 protected:
   static AraGeomTool *fgInstance;  
   // protect against multiple instances

 private:
   void readChannelMapDb(AraStationId_t stationId);

};


#endif //ARAGEOMTOOL_H
