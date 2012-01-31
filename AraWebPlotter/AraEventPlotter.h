////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle making plots from UsefulIcrrStationEvent     /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- December 2010                         /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ARAEVENTPLOTTER_H
#define ARAEVENTPLOTTER_H

#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "AraTimeHistoHandler.h"
#include "AraHistoHandler.h"
#include "RawIcrrStationEvent.h"
#include "UsefulIcrrStationEvent.h"
#include "araTestBedDefines.h"

class AraEventPlotter
{
 public:
  AraEventPlotter(char *plotDir, char *dataDir);
  ~AraEventPlotter();
  void addEvent(int runNumber,RawIcrrStationEvent *rawEvent);
  void makePlots();
  void makeLatestRunPlots();
  void saveFiles();
  void plotEvent(Int_t runNumber,UsefulIcrrStationEvent *usefulEvent);
  void setEventPlotFlag(int flag) { fEventPlotFlag=flag;}

  void loadAllTimeHists();
 private:
  void initialiseCurrentRunTimeHists(); ///<Creates a new file for the current run
  void initialiseTimeHists(Int_t binWidth=60); ///< Opens all the AraHistoHandler and AraTimeHistoHandler
  TFile *fHistoFile;
  //The time histo handlers
  AraTimeHistoHandler *fEventNumberHisto;
  AraTimeHistoHandler *fEventRateHisto;
  AraTimeHistoHandler *fPriorityHisto;
  AraTimeHistoHandler *fCalibStatusBitHistos[8];
  AraTimeHistoHandler *fErrorFlagBitHistos[8];
  //The trig time histo handlers
  AraTimeHistoHandler *fTrigTypeBitHistos[8];
  AraTimeHistoHandler *fPpsNumHisto;
  AraTimeHistoHandler *fDeadTimeHisto;
  AraTimeHistoHandler *fTrigPatternHisto[16];
  AraTimeHistoHandler *fRoVddHisto[3];
  AraTimeHistoHandler *fRcoCountHisto[3];
  
  //Now the event FFT averages
  AraHistoHandler *fAverageFFTHisto[ANTS_PER_TESTBED];
  AraHistoHandler *fAverageTriggerPattern;
  AraHistoHandler *fAverageUnixTimeUs;

  //Now some simple event analysis stuff
  AraTimeHistoHandler *fWaveformRMSHisto[ANTS_PER_TESTBED];
  AraTimeHistoHandler *fWaveformSNRHisto[ANTS_PER_TESTBED];
  
  //The histogram pointers
  TH1D *histTrigPat;
  TH1D *fftHist;
  TH1D *histUnixTimeUs;


  //Run summary plotting nonsense
  Int_t fCurrentRun;
  UInt_t fEarliestTime;
  UInt_t fLatestTime;

  char fTouchFile[180];
  char fPlotDir[180];
  char fDataDir[180];  

  //Event plotting fun  
  TCanvas *fAraDisplayCanvas;
  TPad  *fAraDisplayEventInfoPad;
  TPad  *fAraDisplayMainPad;
  Int_t fEventPlotFlag;
};

#endif //ARAEVENTPLOTTER_H
