//////////////////////////////////////////////////////////////////////////////
/////  RawAraGenericHeader.cxx        ARA Generic Header Class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that stores AtriGenericHeader_t stuff            ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraGenericHeader.h"
#include "AraRootVersion.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraGenericHeader);

RawAraGenericHeader::RawAraGenericHeader()   
  :softVerMajor(ARA_ROOT_MAJOR),softVerMinor(ARA_ROOT_MINOR)
{  
  //Default Constructor
}

RawAraGenericHeader::~RawAraGenericHeader() {
   //Default Destructor
}


RawAraGenericHeader::RawAraGenericHeader(AtriGenericHeader_t *gHdr) ///< Assignment constructor
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
RawAraGenericHeader::RawAraGenericHeader(UInt_t thisStationId) ///< Assignment constructor for ICRR type
  :softVerMajor(ARA_ROOT_MAJOR),softVerMinor(ARA_ROOT_MINOR)
{
  if(thisStationId==ARA_TESTBED||thisStationId==ARA_STATION1){ //Is this an Icrr type station?
    typeId=ARA_ICRR_EVENT_TYPE;
    stationId=thisStationId;
    verId=0;
    subVerId=0;
    reserved=0;
    numBytes=0;
    checksum=0;
  }
}
/*
   LSB to MSB
   bit 0 - filter on?
   bit 1-5 - filter version
   
   bits 6-15 - filterFlags
   bit 6 - Random Filter
   bit 7 - Min-Bias
   bit 8 - Time Sequence Filter
   bit 9 - Track Engine Filter
   bit 10 - 15 - Other filters
*/
Int_t RawAraGenericHeader::getFilterVersion(){
  if(hasFilterFlag()==0) return -1;
  else{
    Int_t version=0;
    UShort_t filterFlag = getFilterFlag();
    Int_t flag = filterFlag >> 1;
    version = flag & 0x001f;
    return version;
  }
}
Int_t RawAraGenericHeader::hasFilterFlag(){
  //LSB is the filter on flag

  UShort_t filterFlag = getFilterFlag();

  if((filterFlag & 0x0001) == 0x0001) return 1;
  else return 0;
  
}
Int_t RawAraGenericHeader::hasBitSetInFilterFlag(Int_t bit){
  if(bit > 9 || hasFilterFlag()==0){
    return -1;
  }
  UShort_t filterFlag = getFilterFlag();
  Int_t bitMask = (0x01) << bit;
  Int_t theFlag = (filterFlag >> 6) &0x3ff;
  
  if( (bitMask & theFlag) == bitMask ) return 1;
  else return 0;
  
}


