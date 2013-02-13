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

//!  Part of AraEvent library. ATRI specific Useful event class.
/*!
  The ROOT implementation of the useful ARA ATRI station event data

  The raw ADC values from a RawAtriStationEvent object are converted into calibrated voltage-time arrays using one of the calibration types defined in AraEventCalibrator. Utility functions are provided to access these arrays as TGraphs, or in the frequency domain.

  Both ATRI and ICRR specific useful events inherit from a common bas class (UsefulAraStationEvent). It is intended that analysis code process objects of the type UsefulAraStationEvent, using the provided overloaded functions (getGraphFromRFChannel etc.). Code will then be able to process both ICRR and ATRI type events instead of being station specific.

  \ingroup rootclasses
*/
class UsefulAtriStationEvent: public RawAtriStationEvent, public UsefulAraStationEvent
{
 public:
   UsefulAtriStationEvent(); ///< Default constructor
   UsefulAtriStationEvent(RawAtriStationEvent *rawEvent, AraCalType::AraCalType_t calType=AraCalType::kVoltageTime); ///< Constructor from RawAtriStationEvent object. This uses AraEventCalibrator to apply calibrations to the event.
   ~UsefulAtriStationEvent(); ///< Destructor


   Int_t getNumElecChannels() {return fNumChannels;} ///< Returns the number of electronics channels
   Int_t getNumRFChannels() {return fNumChannels;} ///< Returns the number of RF channels - NB this may differ from the number of electronics channels
   TGraph *getGraphFromElecChan(int chanId); ///< Returns the voltages-time graph for the appropriate electronics channel
   TGraph *getGraphFromRFChan(int chanId); ///< Returns the voltage-time graph for the appropriate rf channel
   TGraph *getFFTForRFChan(int chan); ///<Utility function for webplotter, all channels are interpolated to 0.5 ns - the returned TGraph is from FFTtools::makePowerSpectrumMilliVoltsNanoS$
   TH1D *getFFTHistForRFChan(int chan); ///< Utility function for webplotter -- produces a TH1D form of getFFTForRFChan(int chan)
   int fillFFTHistoForRFChan(int chan, TH1D *histFFT); ///< Utility function for webplotter


   //Calibrated data
   Int_t fNumChannels; ///< The number of channels
   std::map< Int_t, std::vector <Double_t> > fTimes; ///< The times of samples
   std::map< Int_t, std::vector <Double_t> > fVolts; ///< The voltages of samples


  ClassDef(UsefulAtriStationEvent,1);
};


#endif //USEFULATRISTATIONEVENT_H
