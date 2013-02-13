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
#include "TMath.h"
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
   versionId=hdPtr->versionNumber;
   timeStamp=hdPtr->timeStamp;
   timeStamp ^= (timeStamp >> 1);
   timeStamp ^= (timeStamp >> 2);
   timeStamp ^= (timeStamp >> 4);
   timeStamp ^= (timeStamp >> 8);
   timeStamp ^= (timeStamp >> 16);


   eventId=hdPtr->eventId;
   //   std::cerr << eventNumber << "\t" << versionId << "\t" << ppsNumber << "\t" << timeStamp << "\t" << eventId << std::endl;
   numReadoutBlocks=hdPtr->numReadoutBlocks; 
   
   for(int trig=0;trig<MAX_TRIG_BLOCKS;trig++) {
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
  versionId=hdPtr->versionNumber;
  ppsNumber=hdPtr->ppsNumber;
  numStationBytes=hdPtr->numBytes;
  //   std::cerr << eventNumber << "\t" << ppsNumber << "\t" << numStationBytes;
   timeStamp=hdPtr->timeStamp;
   timeStamp ^= (timeStamp >> 1);
   timeStamp ^= (timeStamp >> 2);
   timeStamp ^= (timeStamp >> 4);
   timeStamp ^= (timeStamp >> 8);
   timeStamp ^= (timeStamp >> 16);



   eventId=hdPtr->eventId;
   numReadoutBlocks=hdPtr->numReadoutBlocks; 
   
   for(int trig=0;trig<MAX_TRIG_BLOCKS;trig++) {
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

Int_t RawAtriStationEvent::getFirstCapArray(Int_t dda)
{

  for(int i=0;i<DDA_PER_ATRI;i++){
    int this_dda = this->blockVec.at(i).getDda();
    if(this_dda==dda) return this->blockVec.at(i).getCapArray();
  }  
  return -1;
  

}



bool RawAtriStationEvent::isCalpulserEvent(){
  Int_t pulserTime=0;

  if(stationId==ARA_STATION1B) pulserTime=254;
  else if(stationId==ARA_STATION2) pulserTime=245;
  else if(stationId==ARA_STATION3) pulserTime=245;
  else return false;

  if(TMath::Abs((Int_t)timeStamp-pulserTime)<1e4){
    //    fprintf(stderr, "%s - stationId %d pulserTime %d timeStamp-pulserTime %d\n", __FUNCTION__, stationId, pulserTime, timeStamp-pulserTime);

    return true;
  }
  else{
    //    fprintf(stderr, "%s - stationId %d pulserTime %d timeStamp-pulserTime %d\n", __FUNCTION__, stationId, pulserTime, timeStamp-pulserTime);

    return false;

  }
}
