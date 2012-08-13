//////////////////////////////////////////////////////////////////////////////
/////  RawAraGenericHeader.h        Raw ARA station event class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that is a wraper for                            /////
/////    AraStationEventEventHeader_t                                    /////
/////    AraStationEventEventChannel_t                                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARAGENERICHEADER_H
#define RAWARAGENERICHEADER_H

//Includes
#include <TObject.h>
#include "araAtriStructures.h"
#include "araSoft.h"


//!  Part of AraEvent library. The base class that stores the stationId and AraRoot version used to produce a Raw / Useful event.
/*!
  This class is a base class that contains header information common to all Raw and Useful event types. 
  \ingroup rootclasses
*/
class RawAraGenericHeader
{
 public:
   RawAraGenericHeader(); ///< Default constructor
   RawAraGenericHeader(AtriGenericHeader_t *gHdr); ///< Assignment constructor
   RawAraGenericHeader(UInt_t thisStationId); ///< Assignment constructor using the stationID -- this is implemented for ICRR type stations
   ~RawAraGenericHeader(); ///< Destructor

   AraStationId_t getStationId() {return stationId;}
 
   UChar_t softVerMajor; //< Software version running on the DAQ SBC
   UChar_t softVerMinor; //< Software  version running on the DAQ SBC

   AraDataStructureType_t typeId;
   UChar_t verId;        //< Version of AraRoot
   UChar_t subVerId;
   AraStationId_t stationId; //< stationID 0x0 TestBed, 0x01 Station1...
   UShort_t reserved;
   UInt_t numBytes;
   UShort_t checksum;

  ClassDef(RawAraGenericHeader,1);
};




#endif //RAWARAGENERICHEADER
