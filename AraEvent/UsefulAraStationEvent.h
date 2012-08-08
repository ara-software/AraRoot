//////////////////////////////////////////////////////////////////////////////
/////  UsefulAraStationEvent.h        Useful ARA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing useful ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef USEFULARASTATIONEVENT_H
#define USEFULARASTATIONEVENT_H

//Includes
#include <TObject.h>
#include <TGraph.h>
#include <TH1.h>

//!  UsefulAraStationEvent -- The Useful ARA Event Data
/*!
  The ROOT implementation of the useful ARA event data

  This is the base class of the useful class for analysing the ARA data. Both the UsefulAtriStationEvent and UsefulIcrrStationEvent inherit from this class. Utility functions are provided to access these arrays as TGraphs, or in the frequency domain.

  \ingroup rootclasses
*/
class UsefulAraStationEvent
{
 public:
   UsefulAraStationEvent(); ///< Default constructor
   ~UsefulAraStationEvent(); ///< Destructor

   virtual Int_t getNumElecChannels()=0; ///< Returns the number of electronics channels
   virtual Int_t getNumRFChannels()=0; ///< Returns the number of Antennae - this may not be the same as the number of electronics channels (interleaving / di-plexing)
   virtual TGraph *getGraphFromElecChan(int chan)=0; ///< Returns the voltages-time graph for the appropriate electronics channel
   virtual TGraph *getGraphFromRFChan(int chan)=0; ///< Returns the voltage-time graph for the appropriate rf channel
   virtual bool isCalPulserEventWithIndex(int pulserIndex); ///< Check to see if this event is from a particular calibration pulser -- NB This is not yet implemented, it is a placeholder / virtual function //FIXME -- Is this what we want for AraSim?



  ClassDef(UsefulAraStationEvent,1);
};


#endif //USEFULARASTATIONEVENT_H
