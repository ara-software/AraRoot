//////////////////////////////////////////////////////////////////////////////
/////  AraTestBedTriggerMonitor.h        Trigger Monitor class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing trigger monitor                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARATESTBEDTRIGGERMONITOR_H
#define ARATESTBEDTRIGGERMONITOR_H

//Includes
#include <TObject.h>
#include "araStructures.h"
#include "araDefines.h"

//!  AraTestBedTriggerMonitor -- The Trigger Monitor
/*!
  The ROOT implementation of the Trigger Monitor
  \ingroup rootclasses
*/
class AraTestBedTriggerMonitor: public TObject
{
 public:
   AraTestBedTriggerMonitor(); ///< Default constructor
   AraTestBedTriggerMonitor(AraTestBedTriggerMonitorStruct_t *theTrig); ///< Assignment constructor
   ~AraTestBedTriggerMonitor(); ///< Destructor
   
   //!  The trigger type
   /*!
    Here we are counting bits from 0 to 7 (lsb to msb)
    -  "Bit 0" RF trigger       (bit latched by ICRR FPGA) [BDF]
    -  "Bit 1" PPS trigger      (bit latched by ICRR FPGA/is not active) [BDF]
    -  "Bit 2" Software trigger (bit set by DAQ but latched by ICRR FPGA/no reliable) [BDF]
    -  "Bit 3" Random trigger?
    -  "Bit 4" not used
    -  "Bit 5" not used
    -  "Bit 6" Software trigger (bit set and latched by DAQ/reliable) [BDF]
    -  "Bit 7" Random trigger   (bit set and latched by DAQ/reliable) [BDF]

    NOTE: care should be taken when using the trigger type since it is not always
          latched correctly by the ICRR board as there seems to be timing issues
          in the FGPA.
   */
   unsigned char trigType;   ///<Trig type bit masks
   unsigned short ppsNum;    ///< 1PPS
   unsigned short deadTime1; ///< fraction = deadTime/64400
   unsigned short deadTime2; ///< fraction = deadTime/64400


   //!  Trigger Pattern
   /*!
    Here we are counting bits from 0 to 15 (lsb to msb)
    -  "Bit 0" discone facet 0
    -  "Bit 1" discone facet 1
    -  "Bit 2" discone facet 2
    -  "Bit 3" discone facet 3
    -  "Bit 4" discone facet 4
    -  "Bit 5" discone facet 5
    -  "Bit 6" batwing facet 0
    -  "Bit 7" batwing facet 1
    -  "Bit 8" batwing facet 2
    -  "Bit 9" any 3 discones
    -  "Bit 10" any 3 batwings
    -  "Bit 11" surface coincidence
    -  "Bits 12-15" unused?
   */
   unsigned short trigPattern; 
   unsigned short rovdd[3];  //< Actually rovdd[0] and rovdd[1] are thetrigger time
   unsigned short rcoCount[3]; ///< RCO counter
    
   Int_t isInTrigPattern(int bit); ///< Returns 1 if the bit is in the pattern
   Int_t isInTrigType(int bit); ///< Returns 1 if the bit is in trig type
   Double_t getDeadtime(); ///< Returns the deadtime as a fraction (I think)
   UInt_t getRubidiumTriggerTime(); ///<Returns the 32-bit (well maybe 29-bit) trigger time
   Double_t getRubidiumTriggerTimeInSec();///< Returns the rubidium time divided by 280M

   ClassDef(AraTestBedTriggerMonitor,1);
};


#endif //ARATESTBEDTRIGGERMONITOR_H
