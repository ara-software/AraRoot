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
#include "araStructures.h"
#include "araDefines.h"
#include "AraAntennaInfo.h"


//!  AraGeomTool -- The Ara Geometry and numbering tool
/*!
  The Ara geometry and numbering tool
  \ingroup rootclasses
*/
class AraGeomTool
{
 public:
   AraGeomTool(); ///< Default constructor
   ~AraGeomTool(); ///< Destructor

   //   AraAntennaInfo *getAntByRfChan(int chan);
   //   AraAntennaInfo *getAntByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum);
   int getChanIndex(AraLabChip::AraLabChip_t chip, int chan) {return chip*CHANNELS_PER_CHIP +chan;} ///< Convert labchip and channel to logical raw channel index

   AraLabChip::AraLabChip_t getLabChipForChan(int chan) {return fAntInfo[chan].labChip;} ///< For a given RF channel get associated Labrador chip
   int getNumLabChansForChan(int chan) { return fAntInfo[chan].numLabChans;} ///< For a given RF channel get the number of associated Labrador channels
   int getFirstLabChanForChan(int chan) { return fAntInfo[chan].labChans[0];} ///< Get the first (or only) Labrador channel for an RF channel
   int getSecondLabChanForChan(int chan) { return fAntInfo[chan].labChans[1];} ///< Get the second (if it exists) Labrador channel for an RF channel
   int getFirstLabChanIndexForChan(int chan) { return getChanIndex(getLabChipForChan(chan),getFirstLabChanForChan(chan));} ///< Get the first (or only) raw logical channel index for an RF channel
   int getSecondLabChanIndexForChan(int chan) { return getChanIndex(getLabChipForChan(chan),getSecondLabChanForChan(chan));} ///< Get the second (if exists) raw logical channel index for an RF channel
   
   int getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum); ///< Get the RF channel number for a given anetnna polarisation and number (i.e V1) 

   
   Double_t calcDeltaTInfinity(Double_t ant1[3], Double_t ant2[3],Double_t phiWave, Double_t thetaWave); ///< Calculate deltaT betweeen two locations in ARA Testbed coordinates under the plane wave hypothesis that the source is at infinity
   Double_t calcDeltaTR(Double_t ant1[3], Double_t ant2[3], Double_t phiWave, Double_t thetaWave,Double_t R); ///< Calculate deltaT betweeen two locations in ARA Testbed coordinates under the spherical wave hypothesis
   Double_t calcDeltaTInfinity(Int_t chan1, Int_t chan2,Double_t phiWave, Double_t thetaWave); ///< Calculate deltaT betweeen two RF channels (antennas) under the plane wave hypothesis that the source is at infinity
   Double_t calcDeltaTR(Int_t chan1, Int_t chan2, Double_t phiWave, Double_t thetaWave,Double_t R); ///< Calculate deltaT betweeen two RF channels (antennas) under the spherical wave hypothesis


   //Instance generator
   static AraGeomTool*  Instance(); ///< Get an instance of the AraGeomTool utility class
   
   AraAntennaInfo fAntInfo[TOTAL_ANTS]; ///< Array of antenna information from Testbed.csv (taken from <a href="http://www.phys.hawaii.edu:8080/ARA/72">ARA:Elog 72</a>
   int fAntLookupTable[3][8]; ///<Utility array used getRFChanByPolAndAnt
   //At som point should lose the magic numbers
   
   //Some variables to do with ice properties
   static Double_t nTopOfIce; ///< Effectively a gloabl variable for the refractive index of the local ice around the Testbed. Defined in AraGeomTool.cxx to be 1.48



  
 protected:
   static AraGeomTool *fgInstance;  ///<The protected instance
   // protect against multiple instances

 private:
   void readChannelMap(); ///<Read channel map


};


#endif //ARAGEOMTOOL_H
