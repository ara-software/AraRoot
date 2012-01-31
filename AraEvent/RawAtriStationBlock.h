//////////////////////////////////////////////////////////////////////////////
/////  RawAtriStationBlock.h        Raw ARA station block class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that is a wraper for                            /////
/////    AraStationEventBlockHeader_t                                    /////
/////    AraStationEventBlockChannel_t                                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWATRISTATIONBLOCK_H
#define RAWATRISTATIONBLOCK_H

//Includes
#include <vector>
#include <TObject.h>
#include "araOneStructures.h"
#include "araSoft.h"




//!  RawAtriStationBlock -- The Raw ARA Station Block Class
/*!
  The ROOT implementation of the raw ARA Station Block containing the samples from one block readout of the IRS
  \ingroup rootclasses
*/
class RawAtriStationBlock: public TObject
{
 public:
   RawAtriStationBlock(); ///< Default constructor
   RawAtriStationBlock(AraStationEventBlockHeader_t *hdPtr, AraStationEventBlockChannel_t *channels); ///< Assignment constructor
   ~RawAtriStationBlock(); ///< Destructor


   //Extra stuff
   UChar_t numChannels;

   //The header data
   UShort_t irsBlockNumber;
   UShort_t channelMask;

   //The samples
   std::vector< std::vector<UShort_t> > data;

   int getNumChannels() {return (int) numChannels;}

   int getDda() {return (channelMask&0x300)>>8;}
   int getBlock() {return irsBlockNumber&0x1ff;}
   int getCapArray() { return (irsBlockNumber&0x4)>>2;}


  ClassDef(RawAtriStationBlock,1);
};




#endif //RAWATRISTATIONBLOCK
