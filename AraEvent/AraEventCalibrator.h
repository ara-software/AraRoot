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
#include "araSoft.h"
#include "araAtriStructures.h"
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include <map>

#define ADCMV 0.939   /* mV/adc, per Gary's email of 05/04/2006 */
#define SATURATION 1300 
#define NSPERSAMP_ICRR 0.95
#define NSPERSAMP_ATRI 0.3125

//!  AraCalType -- The Calibration Type
/*!
    There are a number of calibration options available to create a UsefulIcrrStationEvent.
    \ingroup rootclasses
*/
namespace AraCalType {
    typedef enum EAraCalType {
        kNoCalib                        = 0x00, ///<The 260 samples straight from raw data
        kJustUnwrap                     = 0x01, ///<The X good samples from raw data (260-hitbus)   
        kJustPed                        = 0x02, ///<Just subtract peds
        kADC                            = 0x03, ///<Same as kNoCalib -- i.e. useless
        kVoltageTime                    = 0x04, ///<Using 1 and 2.6 
        kFirstCalib                     = 0x05, ///<First attempt at a calibration by RJN
        kFirstCalibPlusCables           = 0x06, ///< Same as First Calib but also doing the cable delays
        kSecondCalib                    = 0x07, ///< Same as first calib but also doing the clock alignment
        kSecondCalibPlusCables          = 0x08,///< Same as second calib but also doing the clock alignment
        kSecondCalibPlusCablesUnDiplexed = 0x09,///< Same as secondCalibPlusCableDelays but with the undiplexing of diplexed channels in ARA_STATION1

        kLatestCalib                    = 0x09, ///< Currenly this is kSecondCalibPlusCables
        kLatestCalib14to20_Bug          = 0x0A, ///<new calibration type: everything except voltage calibration. Will reproduce "kLatestCalib" bug present from between ~2014 to September 2020. Use with caution!

        //! Useful CalType for debugging, 19-12-2021 -MK-
        kLatestCalibWithOutZeroMean          = 0x0B, ///< Performs every calibration except the ADC and Voltage zero meaning
        kOnlyPed                             = 0X0C, ///< Get the pedestal values for the corresponding raw WF
        kOnlyPedWithOut1stBlock              = 0x0D, ///< Get the pedestal values for the corresponding raw WF without 1st block. It is required when user makes pedestal by their onw custom repeder scripts
        kOnlyPedWithOut1stBlockAndBadSamples = 0X0E, ///< Get the pedestal values for the corresponding raw WF without 1st block and bad samples
        kOnlyADCWithOut1stBlock              = 0x0F, ///< Get the raw ADC WF without 1st block
        kOnlyADCWithOut1stBlockAndBadSamples = 0x10  ///< Get the raw ADC WF without 1st block and bad samples

    } AraCalType_t;

    Bool_t hasTrimFirstBlock(AraCalType::AraCalType_t calType); ///< Does the calibration type trim the first block (Apply Brian's conditioner inside of calibration) -MK-
    Bool_t hasInvertA3Chans(AraCalType::AraCalType_t calType); ///< Does the calibration invert A3 channels (Apply Brian's conditioner inside of calibration) -MK-
    Bool_t hasCableDelays(AraCalType::AraCalType_t calType); ///< Does the calibration type ccount for the cable delays?
    Bool_t hasBinWidthCalib(AraCalType::AraCalType_t calType); ///< Does the calibration type perform the bin-by-bin calibration
    Bool_t hasInterleaveCalib(AraCalType::AraCalType_t calType); ///< Does the calibration type perform an interleave calibration
    Bool_t hasClockAlignment(AraCalType::AraCalType_t calType); ///< Does the calibration type perform a clock alignment calibration
    Bool_t hasPedestalSubtraction(AraCalType::AraCalType_t calType); ///<Does the calibration type perform a pedestal subtraction
    Bool_t hasCommonMode(AraCalType::AraCalType_t calType); ///<Does the calibration type perform a common mode correction
    Bool_t hasUnDiplexing(AraCalType::AraCalType_t calType); ///<Does the calibration type perform un-diplexing
    Bool_t hasADCZeroMean(AraCalType::AraCalType_t calType); ///< Does the calibration type zero mean the ADC waveform -MK-
    Bool_t hasVoltZeroMean(AraCalType::AraCalType_t calType); ///< Does the calibration type zero mean the voltage waveform -MK-
    Bool_t hasVoltCal(AraCalType::AraCalType_t calType); ///< Does the calibration type convert ADC to volts-THM-
} 

class UsefulAtriStationEvent;
class UsefulIcrrStationEvent;
class TGraph; 

//!  Part of AraEvent library. The calibrator takes Raw ATRI / ICRR events and applies Voltage, timing and bandpass filter calibrations to produce Useful ATRI / ICRR events.
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
     

    //Icrr calibrations
    void setPedFile(char fileName[], AraStationId_t stationId); ///< Manually sets the pedestal file
    void calibrateEvent(UsefulIcrrStationEvent *theEvent, AraCalType::AraCalType_t calType=AraCalType::kJustUnwrap); ///< Apply the calibration to a UsefulIcrrStationEvent, called from UsefulIcrrStationEvent constructor
    int doBinCalibration(UsefulIcrrStationEvent *theEvent, int chanIndex,int overrideRCO=-1); ///<This sorts out the bin calibration for the channel, overrideRCO is used in the RCO guess part
    void fillRCOGuessArray(UsefulIcrrStationEvent *theEvent, int rcoGuess[LAB3_PER_ICRR]); ///< Utility function called by UsefulIcrrStationEvent
    Double_t estimateClockPeriod(Int_t numPoints,Double_t &rms);
    void calcClockAlignVals(UsefulIcrrStationEvent *theEvent, AraCalType::AraCalType_t calType); ///< Calculate the clock alignment calibration values
    Double_t estimateClockLag(TGraph *grClock); ///< Worker function used in the clock alignment
    void loadIcrrPedestals(AraStationId_t stationId); ///< Loads the pedestals from a file
    void loadIcrrCalib(AraStationId_t stationId); ///< Loads the various calibration constants according to stationId -- only does it once
    int gotIcrrCalibFile[ICRR_NO_STATIONS]; ///<Flag to indicate whether a station's calib file has been loaded
    int gotIcrrPedFile[ICRR_NO_STATIONS]; ///<Flag to indicate whether a station's pedestal file has been loaded
    char IcrrPedFile[ICRR_NO_STATIONS][FILENAME_MAX]; ///< Filename of the pedestal file
 
    float pedestalData[ICRR_NO_STATIONS][LAB3_PER_ICRR][CHANNELS_PER_LAB3][MAX_NUMBER_SAMPLES_LAB3]; ///<Array to hold the pedestal data
    double binWidths[ICRR_NO_STATIONS][LAB3_PER_ICRR][2][MAX_NUMBER_SAMPLES_LAB3]; ///< Array to hold the bin width calibration constants
    double epsilonVals[ICRR_NO_STATIONS][LAB3_PER_ICRR][2]; ///<Array to hold the wrap-around calibration constants
    double interleaveVals[ICRR_NO_STATIONS][8]; ///< There are only 8 interleaved channels
    double clockAlignVals[ICRR_NO_STATIONS][LAB3_PER_ICRR]; //Well by default clock align 0 is 0
    double clockLagVals[ICRR_NO_STATIONS][LAB3_PER_ICRR]; //For debugging

    ///These are just utility arrays that are used in the calibration
    double v[MAX_NUMBER_SAMPLES_LAB3]; //Calibrated wrapped
    double calwv[MAX_NUMBER_SAMPLES_LAB3]; //Calibrated unwrapped
    double rawadc[MAX_NUMBER_SAMPLES_LAB3]; //Uncalibrated unwrapped
    double pedsubadc[MAX_NUMBER_SAMPLES_LAB3]; //Pedestal subtracted unwrapped
    double sampNums[MAX_NUMBER_SAMPLES_LAB3]; //Sample numbers as doubles
    double timeNums[MAX_NUMBER_SAMPLES_LAB3]; /// time numbers
    double tempTimeNums[MAX_NUMBER_SAMPLES_LAB3]; ///temporary array
    double calTimeNums[MAX_NUMBER_SAMPLES_LAB3]; /// calibrated time numbers
    double calVoltNums[MAX_NUMBER_SAMPLES_LAB3]; /// calibrated volt numbers
    int indexNums[MAX_NUMBER_SAMPLES_LAB3]; /// for time sorting

    //Atri Calibrations
    UShort_t *fAtriPeds; ///< Storage array to hold the ATRI pedestal data
    Int_t fGotAtriPedFile[ATRI_NO_STATIONS]; ///< Flag to indicate whether the ATRI pedestals have been loaded and for which station
    Int_t fGotAtriCalibFile[ATRI_NO_STATIONS]; ///< Flag to indicate whether the ATRI calib have been loaded and for which station
    char fAtriPedFile[ATRI_NO_STATIONS][FILENAME_MAX]; ///< Filename of the ATRI pedestal file
    Int_t fAtriSampleIndex[DDA_PER_ATRI][RFCHAN_PER_DDA][2][SAMPLES_PER_BLOCK]; ///<The sample order
    Double_t fAtriSampleADCVoltsConversion[DDA_PER_ATRI][RFCHAN_PER_DDA][512][64][9];//added for voltage conversion -THM-
    Double_t fAtriSampleHighADCVoltsConversion[DDA_PER_ATRI][RFCHAN_PER_DDA][512][64][5];//added for high voltages -THM-
    Double_t fAtriSampleTimes[DDA_PER_ATRI][RFCHAN_PER_DDA][2][SAMPLES_PER_BLOCK]; ///<The sample timings
    Double_t fAtriEpsilonTimes[DDA_PER_ATRI][RFCHAN_PER_DDA][2]; ///< The timing between blocks the capArray number is the number of the second block
    Int_t fAtriNumSamples[DDA_PER_ATRI][RFCHAN_PER_DDA][2]; ///< The number of samples per block in a particular dda, chan, capArray

    void checkAtriSampleTiming();
    void calibrateEvent(UsefulAtriStationEvent *theEvent, AraCalType::AraCalType_t calType=AraCalType::kVoltageTime); ///< Apply the calibration to a UsefulAtriStationEvent, called from UsefulAtriStationEvent constructor
    Double_t convertADCtoMilliVolts(Double_t adcCountsIn, int dda, int inBlock, int chan, int sample, AraStationId_t stationId); //A conversion module from ADC counts to millivolts  -THM-
    void setAtriPedFile(char *filename, AraStationId_t stationId); ///< Allows the user to force a specific pedestal file into the calibrator instead of the default. The pedestals may vary as a function of time so using a pedestal file from a time close the the event / run is a good idea
    void loadAtriPedestals(AraStationId_t stationId); ///< Internally used function that loads the pedestals into memory.
    void loadAtriCalib(AraStationId_t stationId); ///< Internally used fuction that loads the calibration values into memory.
     
    Bool_t fileExists(char *fileName); ///< Helper function to check whether a file exists
    Int_t numberOfPedestalValsInFile(char *fileName); ///< Helper function to check number of pedestal values in a pedestal file. This is to identify corrupted pedestal files

    //! Modulates calibration step -MK-
    void UnpackDAQFormatToElecChanFormat(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, std::vector<std::vector<int> > *sampleList, std::vector<std::vector<int> > *capArrayList); ///< Converts DAQ data format to Electronic channel format
    Bool_t TrimFirstBlock(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, std::vector<std::vector<int> > *sampleList, std::vector<std::vector<int> > *capArrayList, Bool_t hasTimingCalib); ///< Erase first block that currupted by trigger
    Bool_t TimingCalibrationAndBadSampleReomval(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, std::vector<std::vector<int> > *sampleList, std::vector<std::vector<int> > *capArrayList, Bool_t hasTrimFirstBlk); ///< Trims samples using fAtriSampleTimes table
    void PedestalSubtraction(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::vector<std::vector<int> > *sampleList, AraCalType::AraCalType_t calType); ///< Subtracts pedestal from raw data
    void CommonMode(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt);
    void InvertA3Chans(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, AraStationId_t thisStationId); ///< Inverts only RF channels = 0,4,8 in A3
    void ApplyZeroMean(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::vector<std::vector<int> > *capArrayList, Bool_t hasTrimFirstBlk, Bool_t hasTimingCalib); ///< Zeroing WF by subtracting mean. ADC or Voltage. If 1st block is still in the WF, exclude the samplesin the 1st block from mean calculation
    void VoltageCalibration(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::vector<std::vector<int> > *sampleList, AraStationId_t thisStationId); ///< Converts ADC to Voltage using conversion table
    void ApplyCableDelay(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, Double_t unixtime, AraStationId_t thisStationId); ///< Remove knwon cable delay

    protected:
        static AraEventCalibrator *fgInstance;  ///< protect against multiple instances

    ClassDef(AraEventCalibrator,1);
};


#endif //ARAEVENTCALIBRATOR_H
