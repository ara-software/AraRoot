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

AraGeomTool * AraGeomTool::fgInstance=0;

AraGeomTool::AraGeomTool() 
{
	//for the moment, this doesn't do anything intelligent...
}

AraGeomTool::~AraGeomTool() {
	//for the moment, this doesn't need to do anything intelligent
}

//______________________________________________________________________________
AraGeomTool*  AraGeomTool::Instance()
{
  //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraGeomTool();
  return fgInstance;
}

