//////////////////////////////////////////////////////////////////////////////
/////  RawIcrrStationHeader.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawIcrrStationHeader.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawIcrrStationHeader);

RawIcrrStationHeader::RawIcrrStationHeader() 
{
   //Default Constructor
}

RawIcrrStationHeader::~RawIcrrStationHeader() {
   //Default Destructor
}


RawIcrrStationHeader::RawIcrrStationHeader(IcrrEventHeader_t *hdPtr)
{
   unixTime=hdPtr->unixTime;
   unixTimeUs=hdPtr->unixTimeUs;
   gpsSubTime=hdPtr->gpsSubTime;
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   errorFlag=hdPtr->errorFlag;
}

