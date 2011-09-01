//////////////////////////////////////////////////////////////////////////////
/////  RawAraStationBlock.cxx        ARA header reading class   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAraOneStationBlock.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(RawAraOneStationBlock);

RawAraOneStationBlock::RawAraOneStationBlock()   
{  
  //Default Constructor
}

RawAraOneStationBlock::~RawAraOneStationBlock() {
   //Default Destructor
}


RawAraOneStationBlock::RawAraOneStationBlock(AraStationEventBlockHeader_t *hdPtr, AraStationEventBlockChannel_t channels[]) ///< Assignment constructor
{
  irsBlockNumber=hdPtr->irsBlockNumber;
  channelMask=hdPtr->channelMask;
  atriDdaNumber=hdPtr->atriDdaNumber;
  
  numChannels=0;
  UChar_t mask;
  for(int bit=0;bit<8;bit++) {
    mask=(1<<bit);
    if(channelMask&mask)
      numChannels++;
  }

  //Now loop over and fill in the data
  for(int chan=0;chan<numChannels;chan++) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      data[chan][samp]=channels[chan].samples[samp];
    }
  }

}
