//////////////////////////////////////////////////////////////////////////////
/////  AraDisplay.h        AraDisplay                                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////      Based on the Marvellous ANITA Graphical Interface and         /////
/////      Classy Display (Magic Display) this a simple event display    /////
/////      for ARA.                                                      ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARADISPLAY_H
#define ARADISPLAY_H

//Includes
#include "TChain.h"
#include "AraDisplayConventions.h"
#include "AraEventCalibrator.h"
#include "AraEventCorrelator.h"
#include "AraControlPanel.h"
#include "araIcrrDefines.h"
#include "araAtriStructures.h"
#include "AraIcrrCanvasMaker.h"
#include "AraAtriCanvasMaker.h"

#include "araSoft.h"

class TCanvas;
class TPad;
class UsefulIcrrStationEvent;
class RawIcrrStationEvent;

class UsefulAtriStationEvent;
class RawAtriStationEvent;

class RawAraStationEvent;

class TButton;
class TTreeIndex;
class TFile;
class TEventList;

//!  Part of AraDisplay library. AraDisplay based on the Marvellous ANITA Graphical Interface and Class Display (Magic Display)
/*!
  AraDisplay is the main class that controls the display. This class is responsible for opening files and reading trees and owns the various canvases and that sort of thing.

Typically one starts AraDisplay by giving the constructor the base directory that contains the runXXXX directories and the desired start run.

Most of the functions are called by pressig one of the buttons on the display. They can, of course, be called directly through AraDisplay::Instance()->functionName().
*/
class AraDisplay 
{
 public:
  
 //! The assignment constructor. Most AraDisplay sessions start with a call to this.
  /*!
    \param baseDir The directory that contains the runXXXX directories (that contain the ROOT files
    \param run The run number to start with
    \param calType The calibration option desired (see <a HREF="/uhen/anita/eventReader/">the event reader documentation for the different available calibration options</A>)
  */
  AraDisplay(char *baseDir, unsigned int time, AraCalType::AraCalType_t calType=AraCalType::kVoltageTime);
  AraDisplay(char *eventFile,  AraCalType::AraCalType_t calType=AraCalType::kVoltageTime);
  AraDisplay(); ///< Default constructor
  ~AraDisplay(); ///< Destructor
  
  //Control Panel Functions
  void startControlPanel(); ///<Starts the (currently highly buggy) control panel
  
  Int_t getCurrentFileTime() 
  {return fCurrentFileTime;} ///< Returns the current file time
  UInt_t getCurrentEvent(); ///< Returns the current event number

  void startEventPlaying(); ///< Starts event playback mode, normally called by pressing the "Play" button the display.
  void startEventPlayingReverse(); ///< Starts the event playback in reverse mode, normally called by pressing the "Rev" button on the main display.
  void stopEventPlaying(); ///< Stops the event playback, normally called by pressing the "Stop" button on the main display.

  void closeCurrentFile(); ///< Closes all currently opened TFiles, in preparation for changing to a new File.

  int loadEventTree(char *eventFile=0); ///< Loads the event tree corresponding to <i>fCurrentFileTime</i>
  void startEventDisplay(); ///< Starts the main event display. This is typically the first thing that gets done after creating a AraDisplay object in the runAraDisplay macro.
  //! Displays a specified event number. This is an increddibly useful and well hidden function. From the ROOT prompt you can call this by AraDisplay::Instance()->displayThisEvent(10001,3000);
  /*!
    \param eventNumber The event number to display
    \return Zero on success
  */
  int displayThisEvent(UInt_t eventNumber); 
  int displayNextEvent(); ///< Displays the next event in the file, normally called by pressing the "Next" button.
  int displayFirstEvent(); ///< Displays the first event in the file, normally called by pressing the "First" button.
  int displayLastEvent(); ///< Displays the last event in the file, normally called by pressing the "Last" button.
  int displayPreviousEvent(); ///< Displays the previous event in the file, normally called by pressing the "Previous" button.
  void refreshEventDisplay(); ///< Refresh the event display and redraw the graphs, this is called everytime a new event is displayed.
  int getEventEntry(); ///< Tries to retrieve the event corresponding to entry <i>fEventEntry</i> from the evnt file. Returns zero on success.
  void drawEventButtons(); ///< Worker function to draw the buttons on the main event display canvas.
  
  void setCanvasLayout(AraDisplayCanvasLayoutOption::AraDisplayCanvasLayoutOption_t option);
  //! Toggles between waveform and FFT view modes
  /*!
    \param waveformView See AraDisplayFormatOption for options.
  */
  void setWaveformFormat(AraDisplayFormatOption::AraDisplayFormatOption_t waveformView); 
  void applyCut(char *cutString); ///< Applies a cut to the head tree

  void setCorrelatorType(AraCorrelatorType::AraCorrelatorType_t corType) 
  { if(fAtriEventCanMaker) fAtriEventCanMaker->setCorrelatorType(corType);}
  void setNumAntsInMap(int numAnts) 
  { if(fAtriEventCanMaker) fAtriEventCanMaker->setNumAntsInMap(numAnts);}
  //! Returns a pointer to the active AraDisplay. This is very useful if you want to access the TTree's directly or if you want to explicitly call one of the methods.
  /*!
    \return A pointer to the active AraDisplay.
  */
  static AraDisplay*  Instance();
  

  TFile *fEventFile; ///< A pointer to the current event file.

  //Here are the data managers
  TChain *fEventTree; ///< A pointer to the current event tree.
  

  //And some useful info to keep track of what is where
  Long64_t fEventCutListEntry; ///< The current cut index
  Long64_t fEventTreeIndexEntry; ///< The current entry in the TTreeIndex
  Long64_t fEventEntry; ///< The current event+header entry.

  TTreeIndex *fEventIndex; ///< Reused
  UInt_t fCurrentFileTime; ///< The current file time
  Char_t fCurrentBaseDir[180]; ///< The base directory for the ROOT files.
  

 protected:
  static AraDisplay *fgInstance;  ///< The pointer to the current AraDisplay
   // protect against multiple instances

 private:
  void zeroPointers();
  AraDisplayCanvasLayoutOption::AraDisplayCanvasLayoutOption_t fCanvasLayout;
  AraDisplayFormatOption::AraDisplayFormatOption_t fWaveformFormat; ///< The format for displaying waveforms.

  AraIcrrCanvasMaker *fIcrrEventCanMaker;
  AraAtriCanvasMaker *fAtriEventCanMaker;


  AraControlPanel *fControlPanel;

   TCanvas *fAraCanvas; ///< The main event display canvas.
   TPad *fAraMainPad; ///< The main event display pad.
   TPad *fAraEventInfoPad; ///< The event display info pad.

   UsefulIcrrStationEvent *fIcrrUsefulEventPtr; ///< Pointer to the calibrated event.
   UsefulIcrrStationEvent *fIcrrRawEventPtr; ///< Pointer to the raw event.
   UsefulAtriStationEvent *fAtriUsefulEventPtr; ///< Pointer to the calibrated event.
   UsefulAtriStationEvent *fAtriRawEventPtr; ///< Pointer to the raw event.
   Int_t fCurrentRun; ///<Run number
   RawAraStationEvent *fRawStationEventPtr; ///< Pointer to raw event base class - used to identify the electronics type (Atri vs. Icrr)

      
   TButton *fElecViewButton; ///< The vertical polarisation button.
   TButton *fRFViewButton; ///< The horizontal polarisation button.
   TButton *fAntViewButton; ///< The both polarisations button.
   TButton *fMapViewButton; ///< The Map view button.
   //TButton *fPayloadButton; ///< The payload view button.

   TButton *fWaveformButton; ///< The waveform view button.
   TButton *fPowerButton; ///< The FFT view button.
   TButton *fHilbertButton; ///< The Hilbert envelope view button.
   TButton *fAverageFFTButton; ///< The average FFT button.
   //   TButton *fTimeEntryButton; ///< The button that toggles between events and times


   Int_t fInEventPlayMode; ///< Flag that indicates playback mode
   Int_t fEventPlaySleepMs; ///< Length of sleep between events in playback mode.
   Int_t fIcrrData;
   Int_t fApplyEventCut; ///< Apply an event cut
   TEventList *fCutEventList; ///<The cut eventlist
   
   AraCalType::AraCalType_t fCalType; ///< The waveform calibration type.

};


#endif //ARADISPLAY_H
