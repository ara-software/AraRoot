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
 
   UChar_t softVerMajor; //< Version of AraRoot used to build this ROOT file
   UChar_t softVerMinor; //< Version of AraRoot used to build this ROOT file

   AraDataStructureType_t typeId;
   UChar_t verId;        //< Software version running on the DAQ SBC
   UChar_t subVerId;     //< Software  version running on the DAQ SBC
   AraStationId_t stationId; //< stationID 0x0 TestBed, 0x01 Station1...
   UShort_t reserved; ///< Now used as the filterFlag -- see getFilterFlag()
   UInt_t numBytes;
   UShort_t checksum;

   UShort_t getFilterFlag(){return reserved;} ///< returns the filterFlag. See class member documentation for details.
   /*!
     filterFlag useses reserved as 16 bits to store filter information. From LSB to MSB

     bit 0 - filter on?

     bit 1-5 - filterVersion

     bit 6-15 filter flags - one bit per filter

     filterFlags

     bit 0 - Random Filter

     bit 1 - Min-Bias

     bit 2 - Time Sequence Filter

     bit 3 - Track Engine filter

     bit 4 - 9 - Reserved for future filters
   */

   Int_t getFilterVersion();
   Int_t hasFilterFlag();
   Int_t hasBitSetInFilterFlag(Int_t bit);

  ClassDef(RawAraGenericHeader,2);
};




#endif //RAWARAGENERICHEADER
