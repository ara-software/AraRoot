//////////////////////////////////////////////////////////////////////////////
/////  AraFileUtility               File Reading Utility                 /////
/////                                                                    /////
/////  Description:                                                      /////
/////     a class to ease the opening of Icrr and Atri style events      /////
/////     returns a raw or useful station event of the correct type      /////
/////  Author: jdavies@hep.ucl.ac.uk / rjn@hep.ucl.ac.uk                 ///// 
//////////////////////////////////////////////////////////////////////////////

#include <cstdio>

#include "AraFileUtility.h"

ClassImp(AraFileUtility);

AraFileUtility::AraFileUtility()    //Default Constructor
{
  rawEvPtr=0;
  rawAtriEvPtr=0;
  rawIcrrEvPtr=0;
}
AraFileUtility::AraFileUtility(TTree *theRawTreePtr)
{
  rawTreePtr=theRawTreePtr;
  rawEvPtr=0;
  rawAtriEvPtr=0;
  rawIcrrEvPtr=0;

  rawTreePtr->SetBranchAddress("event", &rawEvPtr);

  if((treeEntries=rawTreePtr->GetEntries())<1){
    printf("AraFileUtility::AraFileUtility() Error - Number of entries is %i\n", treeEntries);
    return;
  }
  rawTreePtr->GetEntry(0);
  stationId=rawEvPtr->getStationId();
  rawTreePtr->ResetBranchAddresses();

  if(stationId==0||stationId==1) {  
    rawTreePtr->SetBranchAddress("event", &rawIcrrEvPtr); 
    rawEvPtr=rawIcrrEvPtr;
  }
  if(stationId>1) {
    rawTreePtr->SetBranchAddress("event", &rawAtriEvPtr);
    rawEvPtr=rawAtriEvPtr; 
  }
  
  //printf("AraFileUtility::AraFileUtility(TTree *rawTreePtr) - stationId %i\n", stationId);
  
}
AraFileUtility::AraFileUtility(TChain *theRawChainPtr)
{
  rawTreePtr=theRawChainPtr;
  rawEvPtr=0;
  rawAtriEvPtr=0;
  rawIcrrEvPtr=0;

  rawTreePtr->SetBranchAddress("event", &rawEvPtr);

  if((treeEntries=rawTreePtr->GetEntries())<1){
    printf("AraFileUtility::AraFileUtility() Error - Number of entries is %i\n", treeEntries);
    return;
  }
  rawTreePtr->GetEntry(0);
  stationId=rawEvPtr->getStationId();
  rawTreePtr->ResetBranchAddresses();

  if(stationId==0||stationId==1) {  
    rawTreePtr->SetBranchAddress("event", &rawIcrrEvPtr); 
    rawEvPtr=rawIcrrEvPtr;
  }
  if(stationId==2) {
    rawTreePtr->SetBranchAddress("event", &rawAtriEvPtr);
    rawEvPtr=rawAtriEvPtr; 
  }
  
  //printf("AraFileUtility::AraFileUtility(TTree *rawTreePtr) - stationId %i\n", stationId);
  
}

AraFileUtility::~AraFileUtility()
{
   //Default Destructor
}

RawAraStationEvent *AraFileUtility::getRawStationEvent(int entry)
{
  rawTreePtr->GetEntry(entry);
  return rawEvPtr;
}
