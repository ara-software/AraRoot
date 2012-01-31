//////////////////////////////////////////////////////////////////////////////
/////  RawAraOneStationBlock.h        Raw ARA station block class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that is a wraper for                            /////
/////    AraStationEventBlockHeader_t                                    /////
/////    AraStationEventBlockChannel_t                                   /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARAONESTATIONBLOCK_H
#define RAWARAONESTATIONBLOCK_H

//Includes
#include <vector>
#include <TObject.h>
#include "araOneStructures.h"
#include "araSoft.h"




//!  RawAraOneStationBlock -- The Raw ARA Station Block Class
/*!
  The ROOT implementation of the raw ARA Station Block containing the samples from one block readout of the IRS
  \ingroup rootclasses
*/
class RawAraOneStationBlock: public TObject
{
 public:
   RawAraOneStationBlock(); ///< Default constructor
   RawAraOneStationBlock(AraStationEventBlockHeader_t *hdPtr, AraStationEventBlockChannel_t *channels); ///< Assignment constructor
   ~RawAraOneStationBlock(); ///< Destructor


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


  ClassDef(RawAraOneStationBlock,1);
};




#endif //RAWARAONESTATIONBLOCK