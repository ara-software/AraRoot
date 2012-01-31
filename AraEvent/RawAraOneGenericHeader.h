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
#include "araOneStructures.h"
#include "araSoft.h"


//!  RawAraGenericHeader -- The Raw ARA Station Event Class
/*!
  The ROOT implementation of the raw ARA Station Event containing the samples from one event readout of the IRS
  \ingroup rootclasses
*/
class RawAraGenericHeader
{
 public:
   RawAraGenericHeader(); ///< Default constructor
   RawAraGenericHeader(AraGenericHeader_t *gHdr); ///< Assignment constructor
   RawAraGenericHeader(UInt_t stationId); ///< Assignment constructor for ICRR type
   ~RawAraGenericHeader(); ///< Destructor
 

   UChar_t softVerMajor;
   UChar_t softVerMinor;

   AraDataStructureType_t typeId;
   UChar_t verId;
   UChar_t subVerId;
   AraStationId_t stationId;
   UShort_t reserved;
   UInt_t numBytes;
   UShort_t checksum;

  ClassDef(RawAraGenericHeader,1);
};




#endif //RAWARAGENERICHEADER
