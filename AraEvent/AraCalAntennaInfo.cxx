//////////////////////////////////////////////////////////////////////////////
/////  AraCalAntennaInfo.h       ARA Antenna Information                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about an Ara Antenna    /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraCalAntennaInfo.h"
#include "AraGeomTool.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraCalAntennaInfo);


AraCalAntennaInfo::AraCalAntennaInfo() 
{
   //Default Constructor
}

AraCalAntennaInfo::~AraCalAntennaInfo() {
   //Default Destructor
}

void AraCalAntennaInfo::printAntennaInfo()
{
  std::cout << "*************************************************************\n";
  std::cout << "Antenna Info for calAntId " << calAntId << "\n";
  std::cout << antName << " at " << locationName << "\n";
  std::cout << AraAntType::antTypeAsString(antType) << "\t" << AraAntPol::antPolAsString(polType) << " polarisation\n";
  std::cout << antLocation[0] << "," << antLocation[1] << "," << antLocation[2] << " m\n";
  std::cout << "Delay " << cableDelay << " ns\n";
  std::cout << "*************************************************************\n";

}


void AraCalAntennaInfo::fillArrayCoords()
{
  AraGeomTool *fGeomTool=AraGeomTool::Instance();
  fGeomTool->convertStationToArrayCoords(fStationId,antLocation,antLocationArray);
  fGeomTool->convertStationToArrayCoords(fStationId,antLocationCalib,antLocationArrayCalib); 
}


const char *AraCalAntennaInfo::getCalAntName()
{
  static char calName[20];
  sprintf(calName, "%s %s", locationName, antPolAsString(polType));
  return calName;
}
