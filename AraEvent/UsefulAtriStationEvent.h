//////////////////////////////////////////////////////////////////////////////
/////  UsefulAtriStationEvent.h        Useful ARA event class          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing useful ARA events in a TTree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef USEFULATRISTATIONEVENT_H
#define USEFULATRISTATIONEVENT_H

//Includes
#include <TObject.h>
#include <TGraph.h>
#include <TH1.h>
#include <vector>
#include <map>

#include "UsefulAraStationEvent.h"
#include "RawAtriStationEvent.h"
#include "AraEventCalibrator.h"


//!  UsefulAtriStationEvent -- The Useful ARA Event Data
/*!
  The ROOT implementation of the useful ARA ATRI station event data

  This is the base class of the useful class for analysing the ARA data. The raw ADC values from a RawAtriStationEvent object are converted into calibrated voltage-time arrays using one of the calibration types defined in AraEventCalibrator. Utility functions are provided to access these arrays as TGraphs, or in the frequency domain.

  \ingroup rootclasses
*/
class UsefulAtriStationEvent: public RawAtriStationEvent, public UsefulAraStationEvent
{
 public:
   UsefulAtriStationEvent(); ///< Default constructor
   UsefulAtriStationEvent(RawAtriStationEvent *rawEvent, AraCalType::AraCalType_t calType=AraCalType::kVoltageTime);
   ~UsefulAtriStationEvent(); ///< Destructor


   Int_t getNumElecChannels() {return fNumChannels;} ///< Or may not be the same
   Int_t getNumRFChannels() {return fNumChannels;} ///< Is the same
   TGraph *getGraphFromElecChan(int chanId); ///< Returns the voltages-time graph for the appropriate electronics channel
   TGraph *getGraphFromRFChan(int chanId); ///< Returns the voltage-time graph for the appropriate rf channel

   //Calibrated data
   Int_t fNumChannels; ///< The number of channels
   std::map< Int_t, std::vector <Double_t> > fTimes; ///< The times of samples
   std::map< Int_t, std::vector <Double_t> > fVolts; ///< The voltages of samples


  ClassDef(UsefulAtriStationEvent,1);
};


#endif //USEFULATRISTATIONEVENT_H
