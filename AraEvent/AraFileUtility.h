//////////////////////////////////////////////////////////////////////////////
/////  AraFileUtility               File Reading Utility                 /////
/////                                                                    /////
/////  Description:                                                      /////
/////     a class to ease the opening of Icrr and Atri style events      /////
/////     returns a raw or useful station event of the correct type      /////
/////  Author: jdavies@hep.ucl.ac.uk / rjn@hep.ucl.ac.uk                 ///// 
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAFILEUTILITY_H
#define ARAFILEUTILITY_H

//Includes
#include "TObject.h"
#include "TTree.h"
#include "TChain.h"

#include "RawAtriStationEvent.h"
#include "RawIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "UsefulIcrrStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "RawAraStationEvent.h"

//!  AraFileUtility - File Reading Utility
/*!
  The ROOT implementation of the useful ARA event data

  Description

  \ingroup rootclasses
*/
class AraFileUtility
{
 public:

  AraFileUtility(); //default constructor
  AraFileUtility(TTree *theRawTreePtr); 
  AraFileUtility(TChain *theRawChainPtr); 

  ~AraFileUtility(); //default destructor

  RawAraStationEvent *getRawStationEvent(int entry);

 private:

  TTree *rawTreePtr;
  TChain *rawChainPtr;
  RawAraStationEvent *rawEvPtr;
  RawAtriStationEvent *rawAtriEvPtr;
  RawIcrrStationEvent *rawIcrrEvPtr;

  UInt_t stationId;
  UInt_t treeEntries;

  ClassDef(AraFileUtility,1)
};


#endif //ARAFILEUTILITY_H
