//////////////////////////////////////////////////////////////////////////////
/////  AraTriggerMonitor.cxx        Definition of the AraTriggerMonitor            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AraTriggerMonitor                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraTriggerMonitor.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraTriggerMonitor);



AraTriggerMonitor::AraTriggerMonitor() 
{
   //Default Constructor
}

AraTriggerMonitor::~AraTriggerMonitor() {
   //Default Destructor
}


AraTriggerMonitor::AraTriggerMonitor(AraTriggerMonitorStruct_t *theTrig)
{
  trigType=theTrig->trigType;
  ppsNum=theTrig->ppsNum;
  deadTime1=theTrig->deadTime1;
  deadTime2=theTrig->deadTime2;
  trigPattern=theTrig->trigPattern;
  rovdd[0]=theTrig->rovdd[0];
  rovdd[1]=theTrig->rovdd[1];
  rovdd[2]=theTrig->rovdd[2];
  rcoCount[0]=theTrig->rcoCount[0];
  rcoCount[1]=theTrig->rcoCount[1];
  rcoCount[2]=theTrig->rcoCount[2];
}

Int_t AraTriggerMonitor::isInTrigPattern(int bit)
{
  if(bit<0 || bit>15) return 0;
  if(trigPattern & (1<<bit))
    return 1;
  return 0;
}

Int_t AraTriggerMonitor::isInTrigType(int bit)
{
  if(bit<0 || bit>2) return 0;
  if(trigType & (1<<bit))
    return 1;
  return 0;
}

Double_t AraTriggerMonitor::getDeadtime()
{
  return ((deadTime1*65536.0+deadTime2*1.0)/10000000.0);
}

UInt_t AraTriggerMonitor::getRubidiumTriggerTime()
{
  UInt_t lsb=rovdd[1];
  UInt_t msb=rovdd[0];
  lsb+=(rovdd[0]<<16);
  return lsb;
} ///<Returns the 32-bit (well maybe 28-bit) trigger time

Double_t AraTriggerMonitor::getRubidiumTriggerTimeInSec()
{
  UInt_t rbTime=getRubidiumTriggerTime();
  if(rbTime>=RUBIDIUM_FREQUENCY) {
    std::cout << "Abnormal Rubidium time of " << rbTime << "\n";
    rbTime=RUBIDIUM_FREQUENCY-1;
  }
  return Double_t(rbTime)/RUBIDIUM_FREQUENCY;

}
