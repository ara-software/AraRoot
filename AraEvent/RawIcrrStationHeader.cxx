//////////////////////////////////////////////////////////////////////////////
/////  RawAraTestBedStationHeader.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraTestBedStationHeader.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraTestBedStationHeader);

RawAraTestBedStationHeader::RawAraTestBedStationHeader() 
{
   //Default Constructor
}

RawAraTestBedStationHeader::~RawAraTestBedStationHeader() {
   //Default Destructor
}


RawAraTestBedStationHeader::RawAraTestBedStationHeader(AraTestBedEventHeader_t *hdPtr)
{
   unixTime=hdPtr->unixTime;
   unixTimeUs=hdPtr->unixTimeUs;
   gpsSubTime=hdPtr->gpsSubTime;
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   errorFlag=hdPtr->errorFlag;
}

