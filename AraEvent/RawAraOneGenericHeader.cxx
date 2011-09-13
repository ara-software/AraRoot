//////////////////////////////////////////////////////////////////////////////
/////  RawAraOneGenericHeader.cxx        ARA Generic Header Class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that stores AraGenericHeader_t stuff            ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraOneGenericHeader.h"
#include "AraRootVersion.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraOneGenericHeader);

RawAraOneGenericHeader::RawAraOneGenericHeader()   
  :softVerMajor(ARA_ROOT_MAJOR),softVerMinor(ARA_ROOT_MINOR)
{  
  //Default Constructor
}

RawAraOneGenericHeader::~RawAraOneGenericHeader() {
   //Default Destructor
}


RawAraOneGenericHeader::RawAraOneGenericHeader(AraGenericHeader_t *gHdr) ///< Assignment constructor
  :softVerMajor(ARA_ROOT_MAJOR),softVerMinor(ARA_ROOT_MINOR)
{
  typeId=gHdr->typeId;
  verId=gHdr->verId;
  subVerId=gHdr->subVerId;
  stationId=gHdr->stationId;
  reserved=gHdr->reserved;
  numBytes=gHdr->numBytes;
  checksum=gHdr->checksum;
}
