//////////////////////////////////////////////////////////////////////////////
/////  AraHkData.h        HK data class                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing housekeeping info                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAHKDATA_H
#define ARAHKDATA_H

//Includes
#include <TObject.h>
#include "araStructures.h"
#include "araDefines.h"

//!  AraHkData -- The HK data
/*!
  The ROOT implementation of the HK data
  \ingroup rootclasses
*/
class AraHkData: public TObject
{
 public:
   AraHkData(); ///< Default constructor
   AraHkData(AraHkDataStruct_t *theHk); ///< Assignment constructor
   ~AraHkData(); ///< Destructor
   
   static char getDacLetter(int index);

   double getRFPowerDiscone(int discId); ///< Get the scaled RF power
   double getRFPowerBatwing(int batId); ///<Get the scaled RF power
   double getTemperature(int tempId);///< Get the temperature in some units


   //From TemperatureDataStruct_t
   unsigned short temp[8]; ///< Temperatures
   
   //From RFPowerDataStruct_t   
  unsigned short rfpDiscone[8]; ///< RF Power
  unsigned short rfpBatwing[8]; ///< RF Power

  //From DACDataStruct_t
  unsigned short dac[6][4]; ///< DAC for what?

  //From SimpleScalerStruct_t
  unsigned short sclDiscone[8];
  unsigned short sclBatPlus[8];
  unsigned short sclBatMinus[8];
  unsigned short sclTrigL1[12];
  unsigned short sclGlobal;

   ClassDef(AraHkData,1);
};


#endif //ARAHKDATA_H
