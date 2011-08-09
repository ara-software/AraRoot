//////////////////////////////////////////////////////////////////////////////
/////  RawAraHeader.h        Raw ARA header class                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ARA headers in a TTree          /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWARAHEADER_H
#define RAWARAHEADER_H

//Includes
#include <TObject.h>
#include "araDefines.h"
#include "araStructures.h"

//!  RawAraHeader -- The Raw ARA Event Header
/*!
  The ROOT implementation of the raw ARA event header
  \ingroup rootclasses
*/
class RawAraHeader: public TObject
{
 public:
   RawAraHeader(); ///< Default constructor
   RawAraHeader(AraEventHeader_t *hdPtr); ///< Assignment constructor
  ~RawAraHeader(); ///< Destructor

  unsigned int unixTime;       ///< unix UTC sec
  unsigned int unixTimeUs;     ///< unix UTC microsec 
  //!  GPS timestamp
  /*!
    //Unused?
  */
  int gpsSubTime;    
  unsigned int eventNumber;    ///< Global event number 
  unsigned short calibStatus;   ///< Unused?
  unsigned char priority; ///< Unused?
  //!  Error Flag
  /*!
    //Unknown and or unused?
  */
  unsigned char errorFlag; 

  ClassDef(RawAraHeader,1);
};


#endif //RAWARAHEADER_H
