//////////////////////////////////////////////////////////////////////////////
/////  RawAraHeader.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraHeader.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraHeader);

RawAraHeader::RawAraHeader() 
{
   //Default Constructor
}

RawAraHeader::~RawAraHeader() {
   //Default Destructor
}


RawAraHeader::RawAraHeader(AraEventHeader_t *hdPtr)
{
   unixTime=hdPtr->unixTime;
   unixTimeUs=hdPtr->unixTimeUs;
   gpsSubTime=hdPtr->gpsSubTime;
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   errorFlag=hdPtr->errorFlag;
}

