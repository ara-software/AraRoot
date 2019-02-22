//////////////////////////////////////////////////////////////////////////////
/////  AraQualCuts.h       ARA Quality cuts tool                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class imposing some event qualtiy cuts                 /////
//////////////////////////////////////////////////////////////////////////////

#include "AraQualCuts.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "TGraph.h"

AraQualCuts * AraQualCuts::fgInstance=0;

AraQualCuts::AraQualCuts() 
{
	//for the moment, this doesn't do anything intelligent...
}

AraQualCuts::~AraQualCuts() {
	//for the moment, this doesn't need to do anything intelligent
}

//______________________________________________________________________________
AraQualCuts*  AraQualCuts::Instance()
{
  //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraQualCuts();
  return fgInstance;
}

bool AraQualCuts::isGoodEvent(UsefulAtriStationEvent *ev)
{
  return true;
}