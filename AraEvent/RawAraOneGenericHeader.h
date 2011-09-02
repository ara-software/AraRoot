//////////////////////////////////////////////////////////////////////////////
/////  RawAraOneGenericHeader.h        Raw ARA station event class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that is a wraper for                            /////
/////    AraStationEventEventHeader_t                                    /////
/////    AraStationEventEventChannel_t                                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARAONEGENERICHEADER_H
#define RAWARAONEGENERICHEADER_H

//Includes
#include <TObject.h>
#include "araOneStructures.h"
#include "araSoft.h"


//!  RawAraOneGenericHeader -- The Raw ARA Station Event Class
/*!
  The ROOT implementation of the raw ARA Station Event containing the samples from one event readout of the IRS
  \ingroup rootclasses
*/
class RawAraOneGenericHeader
{
 public:
   RawAraOneGenericHeader(); ///< Default constructor
   RawAraOneGenericHeader(AraGenericHeader_t *gHdr); ///< Assignment constructor
   ~RawAraOneGenericHeader(); ///< Destructor
 

   UChar_t softVerMajor;
   UChar_t softVerMinor;

   AraDataStructureType_t typeId;
   UChar_t verId;
   AraStationId_t stationId;
   UChar_t reserved;
   UShort_t numBytes;
   UShort_t checksum;

  ClassDef(RawAraOneGenericHeader,1);
};




#endif //RAWARAONEGENERICHEADER
