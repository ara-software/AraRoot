//////////////////////////////////////////////////////////////////////////////
/////  RawAraOneStationEvent.cxx        ARA header reading class   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraOneStationEvent.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraOneStationEvent);

RawAraOneStationEvent::RawAraOneStationEvent()   
{  
  //Default Constructor
}

RawAraOneStationEvent::~RawAraOneStationEvent() {
   //Default Destructor
}


RawAraOneStationEvent::RawAraOneStationEvent(AraStationEventHeader_t *hdPtr, char *dataBuffer) ///< Assignment constructor
  :RawAraOneGenericHeader(&(hdPtr->gHdr)),RawAraStationEvent(hdPtr->gHdr.stationId)
{
  
   unixTime=hdPtr->unixTime; ///< Software event time in seconds (64-bits for future proofing)
   unixTimeUs=hdPtr->unixTimeUs; ///< Software event time in microseconds (32-bits)
   eventNumber=hdPtr->eventNumber; ///< Software event number
   ppsNumber=hdPtr->ppsNumber; ///< For matching up with thresholds etc.
   numStationBytes=hdPtr->numBytes; ///<Bytes in station readout
   recordId=hdPtr->recordId; ///< Record Id
   timeStamp=hdPtr->timeStamp; ///< Timestamp
   eventId=hdPtr->eventId; ///< Event Id
   numReadoutBlocks=hdPtr->numReadoutBlocks; ///< Number of readout blocks which follow header

   int uptoByte=0;
   //   std::cerr << "numReadoutBlocks " << numReadoutBlocks << "\n";
   for(int block=0;block<numReadoutBlocks;block++) {
     AraStationEventBlockHeader_t *blkPtr = (AraStationEventBlockHeader_t*)&dataBuffer[uptoByte];
     uptoByte+=sizeof(AraStationEventBlockHeader_t);
     AraStationEventBlockChannel_t *chanPtr = (AraStationEventBlockChannel_t*)&dataBuffer[uptoByte];
     RawAraOneStationBlock blocky(blkPtr,chanPtr);
     blockVec.push_back(blocky);
     int numChan=blocky.getNumChannels();
     //     std::cout << "numChan " << numChan << "\n";
     uptoByte+=sizeof(AraStationEventBlockChannel_t)*numChan;
   }
   //   std::cerr << sizeof(AraStationEventHeader_t) << "\n";
   if(uptoByte!=int(numStationBytes-(sizeof(AraStationEventHeader_t)-EXTRA_SOFTWARE_HEADER_BYTES))) 
     std::cerr << "Error assigned " << uptoByte <<  " bytes out of " << numStationBytes-(sizeof(AraStationEventHeader_t)-EXTRA_SOFTWARE_HEADER_BYTES) << "\n";
}
