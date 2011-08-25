//////////////////////////////////////////////////////////////////////////////
/////  AraTestBedHkData.h        HK data class                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing housekeeping info                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARATESTBEDHKDATA_H
#define ARATESTBEDHKDATA_H

//Includes
#include <TObject.h>
#include "araTestbedStructures.h"
#include "araDefines.h"

//!  AraTestBedHkData -- The HK data
/*!
  The ROOT implementation of the HK data
  \ingroup rootclasses
*/
class AraTestBedHkData: public TObject
{
 public:
   AraTestBedHkData(); ///< Default constructor
   AraTestBedHkData(AraTestBedHkDataStruct_t *theHk); ///< Assignment constructor
   ~AraTestBedHkData(); ///< Destructor
   
   static char getDacLetter(int index);

   double getRFPowerDiscone(int discId); ///< Get the scaled RF power
   double getRFPowerBatwing(int batId); ///<Get the scaled RF power
   double getTemperature(int tempId);///< Get the temperature in some units


   //From AraTestBedAraTestBedTemperatureDataStruct_t
   unsigned short temp[8]; ///< Temperatures
   
   //From AraTestBedAraTestBedRFPowerDataStruct_t   
  unsigned short rfpDiscone[8]; ///< RF Power
  unsigned short rfpBatwing[8]; ///< RF Power

  //From AraTestBedAraTestBedDACDataStruct_t
  unsigned short dac[6][4]; ///< DAC for what?

  //From AraTestBedAraTestBedSimpleScalerStruct_t
  unsigned short sclDiscone[8];
  unsigned short sclBatPlus[8];
  unsigned short sclBatMinus[8];
  unsigned short sclTrigL1[12];
  unsigned short sclGlobal;

   ClassDef(AraTestBedHkData,1);
};


#endif //ARAHKDATA_H
