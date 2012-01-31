//////////////////////////////////////////////////////////////////////////////
/////  FullIcrrHkEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "FullIcrrHkEvent.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(FullIcrrHkEvent);

FullIcrrHkEvent::FullIcrrHkEvent() 
{
   //Default Constructor
}

FullIcrrHkEvent::~FullIcrrHkEvent() {
   //Default Destructor
}


FullIcrrHkEvent::FullIcrrHkEvent(IcrrHkBody_t *hkBody)
  :trig(&(hkBody->trig)),hk(&(hkBody->hk))
{
  unixTime=hkBody->hd.unixTime;
  unixTimeUs=hkBody->hd.unixTime;
  eventNumber=hkBody->hd.eventNumber;
  errorFlag=hkBody->hd.errorFlag;


}

