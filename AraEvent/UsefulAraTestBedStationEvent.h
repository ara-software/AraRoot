//////////////////////////////////////////////////////////////////////////////
/////  UsefulAraTestBedStationEvent.h        Useful ARA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing useful ARA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef USEFULARATESTBEDSTATIONEVENT_H
#define USEFULARATESTBEDSTATIONEVENT_H

//Includes
#include <TObject.h>
#include <TGraph.h>
#include <TH1.h>
#include "araTestbedStructures.h"
#include "araDefines.h"
#include "RawAraTestBedStationEvent.h"
#include "AraEventCalibrator.h"
#include "UsefulAraStationEvent.h"

//!  UsefulAraTestBedStationEvent -- The Useful ARA Event Data
/*!
  The ROOT implementation of the useful ARA event data

  This is the most useful class for analysing the ARA data. The raw ADC values from a RawAraTestBedStationEvent object are converted into calibrated voltage-time arrays using one of the calibration types defined in AraEventCalibrator. Utility functions are provided to access these arrays as TGraphs, or in the frequency domain.

  \ingroup rootclasses
*/
class UsefulAraTestBedStationEvent: public RawAraTestBedStationEvent, public UsefulAraStationEvent
{
 public:
   UsefulAraTestBedStationEvent(); ///< Default constructor
   UsefulAraTestBedStationEvent(RawAraTestBedStationEvent *rawEvent, AraCalType::AraCalType_t calType=AraCalType::kVoltageTime); ///< Assignment constructor
   ~UsefulAraTestBedStationEvent(); ///< Destructor

   Double_t fVolts[NUM_DIGITIZED_TESTBED_CHANNELS][MAX_NUMBER_SAMPLES_LAB3]; ///< Array of the calibrated voltages
   Double_t fTimes[NUM_DIGITIZED_TESTBED_CHANNELS][MAX_NUMBER_SAMPLES_LAB3]; ///<Array of the times
   Int_t fNumPoints[NUM_DIGITIZED_TESTBED_CHANNELS]; ///< Array of the number of valid samples
   Double_t fVoltsRF[RFCHANS_PER_TESTBED][2*MAX_NUMBER_SAMPLES_LAB3]; ///< Array of the calibrated, interleaved voltages for the RF input channels
   Double_t fTimesRF[RFCHANS_PER_TESTBED][2*MAX_NUMBER_SAMPLES_LAB3]; ///< Array of the calibrated, interleaved times for the RF input channels
   Int_t fNumPointsRF[RFCHANS_PER_TESTBED]; ///< Number of valid samples for thecalibrated, interleaved waveforms from the RF input channels

   TGraph *getGraphFromElecChan(int chan); ///< Returns the voltages-time graph for the appropriate electronics channel
   TGraph *getGraphFromRFChan(int chan); ///< Returns the voltage-time graph for the appropriate rf channel
   TGraph *getFFTForRFChan(int chan); ///<Utility function for webplotter, all channels are interpolated to 0.5 ns
   TH1D *getFFTHistForRFChan(int chan); ///< Utility function for webplotter
   int fillFFTHistoForRFChan(int chan, TH1D *histFFT); ///< Utility function for webplotter
   int guessRCO(int chanIndex); ///< Looks at clock channel to try and guess which RCO phase we are in.
   int getRCO(int chanIndex) { return guessRCO(chanIndex);} ///< Note that this function now calls guessRCO rather than relying on the predetermined RCO phase
   TGraph *getFFTForClock(int clock_number); ///< Returns the FFT for the clock channel on lab A, B, or C for clock_number 0 ,1 , or 2 respectively.
   TH1D   *getFFTHistForClock(int clock_number); ///<Utility function for webplotter
   int     fillFFTHistoForClock(int clock_number, TH1D *histFFT); ///<Utility function for webplotter
   bool    isCalPulserEvent( ); ///< Returns whether (true) or not (false) the event is a calpulser event based on a cut applied to the Rb clock counter

   Int_t getRawRCO(Int_t chanIndex) {
     return chan[chanIndex].getRCO();
   } ///< Returns the RCO phase


   AraEventCalibrator *fCalibrator; ///< Pointer to the AraEventCalibrator

  ClassDef(UsefulAraTestBedStationEvent,1);
};


#endif //USEFULARATESTBEDSTATIONEVENT_H
