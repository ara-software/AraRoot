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
   int getChanIndex(AraLabChip::AraLabChip_t chip, int chan) {return chip*CHANNELS_PER_LAB3 +chan;}

   AraLabChip::AraLabChip_t getLabChipForChan(int chan) {return fAntInfo[chan].labChip;}
   int getNumLabChansForChan(int chan) { return fAntInfo[chan].numLabChans;}
   int getFirstLabChanForChan(int chan) { return fAntInfo[chan].labChans[0];}
   int getSecondLabChanForChan(int chan) { return fAntInfo[chan].labChans[1];}
   int getFirstLabChanIndexForChan(int chan) { return getChanIndex(getLabChipForChan(chan),getFirstLabChanForChan(chan));}
   int getSecondLabChanIndexForChan(int chan) { return getChanIndex(getLabChipForChan(chan),getSecondLabChanForChan(chan));}
   
   int getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum);

   
   Double_t calcDeltaTInfinity(Double_t ant1[3], Double_t ant2[3],Double_t phiWave, Double_t thetaWave);
   Double_t calcDeltaTR(Double_t ant1[3], Double_t ant2[3], Double_t phiWave, Double_t thetaWave,Double_t R);
   Double_t calcDeltaTInfinity(Int_t chan1, Int_t chan2,Double_t phiWave, Double_t thetaWave);
   Double_t calcDeltaTR(Int_t chan1, Int_t chan2, Double_t phiWave, Double_t thetaWave,Double_t R);


   //Instance generator
   static AraGeomTool*  Instance();
   
   AraAntennaInfo fAntInfo[TOTAL_ANTS_PER_ICRR];
   int fAntLookupTable[3][8]; //At som point should lose the magic numbers
   
   //Some variables to do with ice properties
   static Double_t nTopOfIce;



  
 protected:
   static AraGeomTool *fgInstance;  
   // protect against multiple instances

 private:
   void readChannelMap();


};


#endif //ARAGEOMTOOL_H
