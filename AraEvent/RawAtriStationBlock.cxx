//////////////////////////////////////////////////////////////////////////////
/////  RawAraStationBlock.cxx        ARA header reading class   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAtriStationBlock.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAtriStationBlock);

RawAtriStationBlock::RawAtriStationBlock()   
{  
  //Default Constructor
}

RawAtriStationBlock::~RawAtriStationBlock() {
   //Default Destructor
}


RawAtriStationBlock::RawAtriStationBlock(AraStationEventBlockHeader_t *hdPtr, AraStationEventBlockChannel_t *channels) ///< Assignment constructor
{
  irsBlockNumber=hdPtr->irsBlockNumber;
  channelMask=hdPtr->channelMask;
  
  numChannels=0;
  UChar_t mask;
  for(int bit=0;bit<8;bit++) {
    mask=(1<<bit);
    if(channelMask&mask)
      numChannels++;
  }

  //Now loop over and fill in the data
  for(int chan=0;chan<numChannels;chan++) {
    std::vector <UShort_t> tempVec;
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tempVec.push_back(channels[chan].samples[samp]);
      //      data[chan][samp]=channels[chan].samples[samp];
    }
    data.push_back(tempVec);
  }

}
