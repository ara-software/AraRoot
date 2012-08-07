//////////////////////////////////////////////////////////////////////////////
/////  RawAtriStationEvent.cxx        ARA header reading class   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAtriStationEvent.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAtriStationEvent);

RawAtriStationEvent::RawAtriStationEvent()   
{  
  //Default Constructor
}

RawAtriStationEvent::~RawAtriStationEvent() {
   //Default Destructor
}


RawAtriStationEvent::RawAtriStationEvent(AraStationEventHeader_t *hdPtr, char *dataBuffer) // Assignment constructor
  :RawAraStationEvent(&(hdPtr->gHdr))
{
  
   unixTime=hdPtr->unixTime;
   unixTimeUs=hdPtr->unixTimeUs;
   eventNumber=hdPtr->eventNumber;
   ppsNumber=hdPtr->ppsNumber;
   numStationBytes=hdPtr->numBytes;
   //   std::cerr << eventNumber << "\t" << ppsNumber << "\t" << numStationBytes;
   timeStamp=hdPtr->timeStamp;
   eventId=hdPtr->eventId;
   numReadoutBlocks=hdPtr->numReadoutBlocks; 
   
   for(int trig=0;trig<MAX_TRIG_BLOCKS;trig++) {
     triggerPattern[trig]=hdPtr->triggerPattern[trig];
     triggerInfo[trig]=hdPtr->triggerInfo[trig];
     triggerBlock[trig]=hdPtr->triggerBlock[trig];
   }

   int uptoByte=0;
   //   std::cerr << "numReadoutBlocks " << numReadoutBlocks << "\n";
   for(int block=0;block<numReadoutBlocks;block++) {
     AraStationEventBlockHeader_t *blkPtr = (AraStationEventBlockHeader_t*)&dataBuffer[uptoByte];     
     uptoByte+=sizeof(AraStationEventBlockHeader_t);
     AraStationEventBlockChannel_t *chanPtr = (AraStationEventBlockChannel_t*)&dataBuffer[uptoByte];
     RawAtriStationBlock blocky(blkPtr,chanPtr);
     blockVec.push_back(blocky);
     int numChan=blocky.getNumChannels();

     // std::cout << "block " << block << " numChan " << numChan 
     // 	       <<  " irsBlockNumber " << (blkPtr->irsBlockNumber&0x1ff)
     // 	       << " channelMask " << blkPtr->channelMask << "\t"
     // 	       << " uptoByte " << uptoByte << "\n";
     // numChan=8; //HArd wire for testing
     uptoByte+=sizeof(AraStationEventBlockChannel_t)*numChan;
   }
   //   std::cerr << sizeof(AraStationEventHeader_t) << "\n";
   if(uptoByte!=int(numStationBytes))    
     std::cerr << "Error assigned " << uptoByte <<  " bytes out of " << numStationBytes << "\n";
}

RawAtriStationEvent::RawAtriStationEvent(AraStationEventHeader_t *hdPtr, char *dataBuffer, AraStationId_t forcedStationId) // Assignment constructor
  :RawAraStationEvent(&(hdPtr->gHdr))
{
  //JPD The same as above but forcing the stationId value
  stationId = forcedStationId;

  
  unixTime=hdPtr->unixTime;
  unixTimeUs=hdPtr->unixTimeUs;
  eventNumber=hdPtr->eventNumber;
  ppsNumber=hdPtr->ppsNumber;
  numStationBytes=hdPtr->numBytes;
  //   std::cerr << eventNumber << "\t" << ppsNumber << "\t" << numStationBytes;
   timeStamp=hdPtr->timeStamp;
   eventId=hdPtr->eventId;
   numReadoutBlocks=hdPtr->numReadoutBlocks; 
   
   for(int trig=0;trig<MAX_TRIG_BLOCKS;trig++) {
     triggerPattern[trig]=hdPtr->triggerPattern[trig];
     triggerInfo[trig]=hdPtr->triggerInfo[trig];
     triggerBlock[trig]=hdPtr->triggerBlock[trig];
   }

   int uptoByte=0;
   //   std::cerr << "numReadoutBlocks " << numReadoutBlocks << "\n";
   for(int block=0;block<numReadoutBlocks;block++) {
     AraStationEventBlockHeader_t *blkPtr = (AraStationEventBlockHeader_t*)&dataBuffer[uptoByte];     
     uptoByte+=sizeof(AraStationEventBlockHeader_t);
     AraStationEventBlockChannel_t *chanPtr = (AraStationEventBlockChannel_t*)&dataBuffer[uptoByte];
     RawAtriStationBlock blocky(blkPtr,chanPtr);
     blockVec.push_back(blocky);
     int numChan=blocky.getNumChannels();

     // std::cout << "block " << block << " numChan " << numChan 
     // 	       <<  " irsBlockNumber " << (blkPtr->irsBlockNumber&0x1ff)
     // 	       << " channelMask " << blkPtr->channelMask << "\t"
     // 	       << " uptoByte " << uptoByte << "\n";
     // numChan=8; //HArd wire for testing
     uptoByte+=sizeof(AraStationEventBlockChannel_t)*numChan;
   }
   //   std::cerr << sizeof(AraStationEventHeader_t) << "\n";
   if(uptoByte!=int(numStationBytes))    
     std::cerr << "Error assigned " << uptoByte <<  " bytes out of " << numStationBytes << "\n";
}
