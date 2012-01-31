//////////////////////////////////////////////////////////////////////////////
/////  IcrrHkData.h        HK data class                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing housekeeping info                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ICRRHKDATA_H
#define ICRRHKDATA_H

//Includes
#include <TObject.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"

//!  IcrrHkData -- The HK data
/*!
  The ROOT implementation of the HK data
  \ingroup rootclasses
*/
class IcrrHkData: public TObject
{
 public:
   IcrrHkData(); ///< Default constructor
   IcrrHkData(IcrrHkDataStruct_t *theHk); ///< Assignment constructor
   ~IcrrHkData(); ///< Destructor
   
   static char getDacLetter(int index);

   double getRFPowerDiscone(int discId); ///< Get the scaled RF power
   double getRFPowerBatwing(int batId); ///<Get the scaled RF power
   double getTemperature(int tempId);///< Get the temperature in some units


   //From IcrrIcrrTemperatureDataStruct_t
   unsigned short temp[8]; ///< Temperatures
   
   //From IcrrIcrrRFPowerDataStruct_t   
  unsigned short rfpDiscone[8]; ///< RF Power
  unsigned short rfpBatwing[8]; ///< RF Power

  //From IcrrIcrrDACDataStruct_t
  unsigned short dac[6][4]; ///< DAC for what?

  //From IcrrIcrrSimpleScalerStruct_t
  unsigned short sclDiscone[8];
  unsigned short sclBatPlus[8];
  unsigned short sclBatMinus[8];
  unsigned short sclTrigL1[12];
  unsigned short sclGlobal;

   ClassDef(IcrrHkData,1);
};


#endif //ARAHKDATA_H
