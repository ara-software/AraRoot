//////////////////////////////////////////////////////////////////////////////
/////  AraQualCuts.h       ARA Quality cuts tool                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class imposing some event qualtiy cuts                 /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAQUALCUTS_H
#define ARAQUALCUTS_H

//Includes
#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"

//! Part of AraEvent library. Can report on if there is a quality cut problem with an event
/*!
  The Ara event quality cuts tool
  \ingroup rootclasses
*/
class AraQualCuts
{
  public:
    AraQualCuts(); ///< Default constructor
    ~AraQualCuts(); ///< Destructor

    //Instance generator
    static AraQualCuts*  Instance();

    static bool isGoodEvent(UsefulAtriStationEvent *realEvent);
    
    static bool hasBlockGap(RawAtriStationEvent *rawEvent);
    static bool hasTimingError(UsefulAtriStationEvent *realEvent);
    static bool hasTooFewBlocks(UsefulAtriStationEvent *realEvent);
  
  protected:
    static AraQualCuts *fgInstance; // protect against multiple instances
    
  private:
    //nothing private for right now
};

#endif //ARAQUALCUTS_H
