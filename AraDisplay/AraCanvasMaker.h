//////////////////////////////////////////////////////////////////////////////
/////  AraCanvasMaker.h        ARA Canvas Maker                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARACANVASMAKER_H
#define ARACANVASMAKER_H

//Includes
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TH1.h>
#include <TVector3.h>
#include "AraDisplayConventions.h"
#include "UsefulAraEvent.h"
#include "AraEventCalibrator.h"
#include "AraEventCorrelator.h"

class TPad;
class TFile;

//!  The event display canvas maker.
/*!
  This is where all the heavy lifting regarding the main event display canvas takes place. Lots of things are hard-coded and unchangeable in order to make the display look reasonable (on my computer at least). 
*/
class AraCanvasMaker 
{
 public:
  //Something of a sleazy hack for the web plotter.
  TH1D *getFFTHisto(int ant);

  void setCorrelatorType(AraCorrelatorType::AraCorrelatorType_t corType) {fCorType=corType;} ///<Select the correlator type to use
  void setCanvasLayout(AraDisplayCanvasLayoutOption::AraDisplayCanvasLayoutOption_t canOption) {fCanvasLayout=canOption;} ///< Set the canvas layout mode
  void setWaveformFormat(AraDisplayFormatOption::AraDisplayFormatOption_t waveOption) {fWaveformOption=waveOption;} ///<Set the waveform format
  void resetAverage(); ///< Resets the average for the FFT mode
  void setNumAntsInMap(int numAnts) {fNumAntsInMap=numAnts;} ///<set the number of antennas to use in the interferometric map


  Int_t fNumAntsInMap; ///<The number of antennas to use in the interferometric map
   Double_t fMinVoltLimit; ///< The minimum voltage. 
   Double_t fMaxVoltLimit; ///< The maximum voltage.
   Int_t fAutoScale; ///< Fixed or auto-scaling y-axis?
   Double_t fMinClockVoltLimit; ///< The minimum voltage in the clock channels.
   Double_t fMaxClockVoltLimit; ///< The maximum voltage in the clock channels.
   Double_t fMinTimeLimit; ///< The minimum time in the waveform (typically 0).
   Double_t fMaxTimeLimit; ///< The maximum time in the waveform (typically 100).
   Double_t fMinPowerLimit; ///< The minimum power in the PSDs.
   Double_t fMaxPowerLimit; ///< The maximum power in the PSDs.
   Double_t fMinFreqLimit; ///< The minimum frequency in the PSDs (typically 0).
   Double_t fMaxFreqLimit; ///< The maximum frequency in the PSDs (typically 1200).

   AraCorrelatorType::AraCorrelatorType_t fCorType; ///<Which kind of correlation should we use
   AraDisplayCanvasLayoutOption::AraDisplayCanvasLayoutOption_t fCanvasLayout; ///<Which canvas layout were we in
   AraDisplayFormatOption::AraDisplayFormatOption_t fWaveformOption; ///< Are we viewing PSDs, waveforms or other stuff?
   Int_t fRedoEventCanvas; ///< Do we neeed to redraw the event canvas (eg. switching from phi to SURF)?
   //Int_t fRedoSurfCanvas;
   AraDisplayCanvasLayoutOption::AraDisplayCanvasLayoutOption_t fLastCanvasView; ///<Which canvas layout were we in
   AraDisplayFormatOption::AraDisplayFormatOption_t fLastWaveformFormat; ///< What did we plot last time??
   Int_t fNewEvent; ///< Is this a new event?
  
   //!  The main event view canvas getter.
   /*!
     Actually draws all the little squiggly lines.
     /param evPtr Pointer to the UsefulAraEvent we want to draw.
     /param hdPtr Pointer to the header of the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getEventViewerCanvas(UsefulAraEvent *evPtr, TPad *useCan=0);
   
   //!  The main event view canvas getter used by QnDWBOM.
   /*!
     Actually draws all the little squiggly lines.
     /param evPtr Pointer to the UsefulAraEvent we want to draw.
     /param hdPtr Pointer to the header of the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *quickGetEventViewerCanvasForWebPlottter(UsefulAraEvent *evPtr, TPad *useCan=0);
   
  
  

   //!  The event info canvas getter.
   /*!
     Fills in run number and times and all those good things.
     /param hdPtr Pointer to the header of the event we want to draw
     /param useCan Optional pointer to a canvas to draw all the TPaveText's (if zero a canvas will be created).
   */
   TPad *getEventInfoCanvas(UsefulAraEvent *evPtr, TPad *useCan=0, Int_t runNumber=0);
  
   
   void setupElecPadWithFrames(TPad *plotPad); ///< Worker function to setup the electronics view canvas.
   void setupRFChanPadWithFrames(TPad *plotPad); ///< Worker function to setup the surf view canvas.
   void setupAntPadWithFrames(TPad *plotPad); ///< Worker function to setup the surf view canvas.

   void deleteTGraphsFromElecPad(TPad *paddy,int chan); ///< Worker fucntion to delete a specific graph from a pad.
   void deleteTGraphsFromRFPad(TPad *paddy,int ant); ///< Worker fucntion to delete specific graphs from a pad.
   
   //!  The assignment constructor.
   /*!
     Creates an AraCanvasMaker object/
     /param calType The calibration type used (only needed to set the time axis for ns vs samples).
   */
   AraCanvasMaker(AraCalType::AraCalType_t calType=AraCalType::kVoltageTime);
   ~AraCanvasMaker(); ///<Destructor.
   
   //Instance generator
   static AraCanvasMaker*  Instance(); ///< The instance generator.
   
   void setPassBandFilterFlag( int flag) { fPassBandFilter=flag;} ///<Flags the pass band filter on or off
   void setNotchFilterFlag( int flag) { fNotchFilter=flag;} ///<Flags the notch filter on or off
   void setPassBandLimits(Double_t low, Double_t high)
   { fLowPassEdge=low; fHighPassEdge=high;} ///<Sets the limits of the pass band (200-1200) in MHz
   void setNotchBandLimits(Double_t low, Double_t high)
   { fLowNotchEdge=low; fHighNotchEdge=high;} ///<Sets the limits of the notched band (235-500) in MHz

   
   AraCalType::AraCalType_t fCalType; ///< The calibration type.
   
 protected:
   static AraCanvasMaker *fgInstance; ///< To protect against multiple instances.

 private:
   Int_t fWebPlotterMode;
   Int_t fPhiMax;
   TFile *fAraGeomFile;
   Int_t fPassBandFilter; ///< Whether or not to pass band filter the interpolated waves;
   Int_t fNotchFilter; ///< Whether or not to notch filter;
   Double_t fLowPassEdge; ///< The lower edge of the pass band
   Double_t fHighPassEdge; ///< The higher edge of the pass band
   Double_t fLowNotchEdge; ///< The lower edge of the notch band
   Double_t fHighNotchEdge; ///< The higher edge of the notch band
   //!  A worker function to draw the l canvas -- shouldn't be called directly.
   /*!
     /param evPtr Pointer to the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getElectronicsCanvas(UsefulAraEvent *evPtr, TPad *useCan=0);
   //!  A worker function to draw the Antenna canvas -- shouldn't be called directly.
   /*!
     /param evPtr Pointer to the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getAntennaCanvas(UsefulAraEvent *evPtr, TPad *useCan=0);
   //!  A worker function to draw the Interferometric Map canvas -- shouldn't be called directly.
   /*!
     /param evPtr Pointer to the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getIntMapCanvas(UsefulAraEvent *evPtr, TPad *useCan=0);
   //!  A worker function to draw the RF Channel canvas -- shouldn't be called directly.
   /*!
     /param evPtr Pointer to the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getRFChannelCanvas(UsefulAraEvent *evPtr, TPad *useCan=0);

   //!  A worker function to draw the canvas for QnDWBOM -- shouldn't be called directly.
   /*!
     /param evPtr Pointer to the event we want to draw
     /param useCan Optional pointer to a canvas to draw the squiggly ines in (if zero a canvas will be created).
   */
   TPad *getCanvasForWebPlotter(UsefulAraEvent *evPtr, TPad *useCan=0);

};


#endif //ARACANVASMAKER_H
