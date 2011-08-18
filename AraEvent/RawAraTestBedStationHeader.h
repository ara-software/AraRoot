//////////////////////////////////////////////////////////////////////////////
/////  RawAraTestBedStationHeader        Raw ARA header class                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA headers in a TTree          /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARATESTBEDSTATIONHEADER_H
#define RAWARATESTBEDSTATIONHEADER_H

//Includes
#include <TObject.h>
#include "araDefines.h"
#include "araStructures.h"

//!  RawAraTestBedStationHeader -- The Raw ARA Event Header
/*!
  The ROOT implementation of the raw ARA event header
  \ingroup rootclasses
*/
class RawAraTestBedStationHeader: public TObject
{
 public:
   RawAraTestBedStationHeader(); ///< Default constructor
   RawAraTestBedStationHeader(AraTestBedEventHeader_t *hdPtr); ///< Assignment constructor
   ~RawAraTestBedStationHeader(); ///< Destructor

  unsigned int unixTime;       ///< unix UTC sec
  unsigned int unixTimeUs;     ///< unix UTC microsec 
  //!  GPS timestamp
  /*!
     the GPS fraction of second (in ns) 
     (for the X events per second that get 
     tagged with it, note it now includes
     second offset from unixTime)
  */
  int gpsSubTime;    
  unsigned int eventNumber;    ///< Global event number 
  unsigned short calibStatus;   ///< Were we flashing the pulser? 
  unsigned char priority; ///< priority and other
  //!  Error Flag
  /*!
    Bit 1 means sync slip between TURF and software
    Bit 2 is sync slip between SURF 1 and software
    Bit 3 is sync slip between SURF 10 and SURF 1
    Bit 4 is non matching TURF test pattern
    Bit 5 is startBitGood (1 is good, 0 is bad);
    Bit 6 is stopBitGood (1 is good, 0 is bad);
    Bit 7-8 TURFIO photo shutter output
  */
  unsigned char errorFlag; 

  ClassDef(RawAraTestBedStationHeader,1);
};


#endif //RAWARATESTBEDSTATIONHEADER_H
