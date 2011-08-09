//////////////////////////////////////////////////////////////////////////////
/////  AraEventCalibrator.h        Calibrator                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class for calibrating events                           /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAEVENTCALIBRATOR_H
#define ARAEVENTCALIBRATOR_H

//Includes
#include <TObject.h>
#include "araStructures.h"
#include "araDefines.h"


#define ADCMV 0.939   /* mV/adc, per Gary's email of 05/04/2006 */
#define SATURATION 1300 
#define NSPERSAMP 0.95


//!  AraCalType -- The Calibration Type
/*!
  There are a number of calibration options available to create a UsefulAraEvent.
  \ingroup rootclasses
*/
namespace AraCalType {
  typedef enum EAraCalType {
    kNoCalib                        = 0x00, ///<The 260 samples straight from raw data
    kJustUnwrap                     = 0x01, ///<The X good samples from raw data (260-hitbus)
    kADC                            = 0x02, ///<Same as kNoCalib -- i.e. useless
    kVoltageTime                    = 0x03, ///<Using 1 and 2.6    
    kJustPed                        = 0x04, ///<Just subtract peds
    kFirstCalib                     = 0x05, ///<First attempt at a calibration by RJN
    kFirstCalibPlusCables           = 0x06, ///< Same as First Calib but also doing the cable delays
    kSecondCalib                    = 0x07, ///< Same as first calib but also doing the clock alignment
    kSecondCalibPlusCables          = 0x08,///< Same as second calib but also doing the clock alignment
    kLatestCalib                    = 0x08 ///< Currenly this is kSecondCalibPlusCables
  } AraCalType_t;

  Bool_t hasCableDelays(AraCalType::AraCalType_t calType); ///< Does the calibration type ccount for the cable delays?
  Bool_t hasBinWidthCalib(AraCalType::AraCalType_t calType); ///< Does the calibration type perform the bin-by-bin calibration
  Bool_t hasInterleaveCalib(AraCalType::AraCalType_t calType); ///< Does the calibration type perform an interleave calibration
  Bool_t hasClockAlignment(AraCalType::AraCalType_t calType); ///< Does the calibration type perform a clock alignment calibration

} 

class UsefulAraEvent;
class TGraph; 

//!  AraEventCalibrator -- The Ara Event Calibrator
/*!
  The Ara Event Calibrator
  \ingroup rootclasses
*/
class AraEventCalibrator : public TObject
{
 public:
   AraEventCalibrator(); ///< Default constructor
   ~AraEventCalibrator(); ///< Destructor

   //Instance generator
   static AraEventCalibrator*  Instance(); ///< Generates an instance of AraEventCalibrator to use
   

   void setPedFile(char fileName[]); ///< Manually sets the pedestal file
   void calibrateEvent(UsefulAraEvent *theEvent, AraCalType::AraCalType_t calType=AraCalType::kJustUnwrap); ///< Apply the calibration to a UsefulAraEvent, called from UsefulAraEvent constructor
   int doBinCalibration(UsefulAraEvent *theEvent, int chanIndex,int overrideRCO=-1); ///<This sorts out the bin calibration for the channel, overrideRCO is used in the RCO guess part
   
   void fillRCOGuessArray(UsefulAraEvent *theEvent, int rcoGuess[ACTIVE_CHIPS]); ///< Utility function called by UsefulAraEvent
   Double_t estimateClockPeriod(Int_t numPoints,Double_t &rms);

   void calcClockAlignVals(UsefulAraEvent *theEvent, AraCalType::AraCalType_t calType); ///< Calculate the clock alignment calibration values
   Double_t estimateClockLag(TGraph *grClock); ///< Worker function used in the clock alignment
   


   void loadPedestals(); ///< Loads the pedestals from a file
   void loadCalib(); ///< Loads the various calibration constants
   int gotPedFile; ///<Flag to indicate whether a specific pedesal file has been selected
   char pedFile[FILENAME_MAX]; ///< Filename of the pedesal file
   float pedestalData[ACTIVE_CHIPS][CHANNELS_PER_CHIP][MAX_NUMBER_SAMPLES]; ///<Array to hold the pedestal data
   double binWidths[ACTIVE_CHIPS][2][MAX_NUMBER_SAMPLES]; ///< Array to hold the bin width calibration constants
   double epsilonVals[ACTIVE_CHIPS][2]; ///<Array to hold the wrap-around calibration constants
   double interleaveVals[8]; ///< There are only 8 interleaved channels
   double clockAlignVals[ACTIVE_CHIPS]; //Well by default clock align 0 is 0



   ///These are just utility arrays that are used in the calibration
   double v[MAX_NUMBER_SAMPLES]; //Calibrated wrapped
   double calwv[MAX_NUMBER_SAMPLES]; //Calibrated unwrapped
   double rawadc[MAX_NUMBER_SAMPLES]; //Uncalibrated unwrapped
   double pedsubadc[MAX_NUMBER_SAMPLES]; //Pedestal subtracted unwrapped
   double sampNums[MAX_NUMBER_SAMPLES]; //Sample numbers as doubles
   double timeNums[MAX_NUMBER_SAMPLES]; /// time numbers
   double tempTimeNums[MAX_NUMBER_SAMPLES]; ///temporary array
   double calTimeNums[MAX_NUMBER_SAMPLES]; /// calibrated time numbers
   double calVoltNums[MAX_NUMBER_SAMPLES]; /// calibrated volt numbers
   int indexNums[MAX_NUMBER_SAMPLES]; /// for time sorting


 protected:
   static AraEventCalibrator *fgInstance;  ///< protect against multiple instances

   ClassDef(AraEventCalibrator,1);

};


#endif //ARAEVENTCALIBRATOR_H
