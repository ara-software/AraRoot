//////////////////////////////////////////////////////////////////////////////
/////  AraQualCuts.h       ARA Quality cuts tool                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class imposing some event qualtiy cuts                 /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAQUALCUTS_H
#define ARAQUALCUTS_H

//Includes
#include "UsefulAtriStationEvent.h"


//! Part of AraEvent library. Can report on if there is a quality cut problem with an event
/*!
  The Ara event quality cuts tool
  \ingroup rootclasses
*/
class AraGeomTool
{
  public:
    AraQualCuts(); ///< Default constructor
    ~AraQualCuts(); ///< Destructor

    //Instance generator
    static AraGeomTool*  Instance();

    static bool isGoodEvent(UsefulAtriStationEvent *ev);

  
  protected:
    static AraGeomTool *fgInstance; // protect against multiple instances
    
  private:
    //nothing private for right now
};


#endif //ARAQUALCUTS_H
