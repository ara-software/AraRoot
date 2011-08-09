////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle making plots from UsefulAraHk     /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- December 2010                         /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ARAHKPLOTTER_H
#define ARAHKPLOTTER_H

#include "TFile.h"
#include "AraTimeHistoHandler.h"
#include "AraHkData.h"

class AraHkPlotter
{
 public:
  AraHkPlotter(char *plotDir, char *dataDir);
  ~AraHkPlotter();
  void addHk(Int_t runNumber,UInt_t unixTime, AraHkData *hkData);
  void makePlots();
  void makeLatestRunPlots();
  void saveFiles();


  void loadAllTimeHists();
 private:
  void initialiseCurrentRunTimeHists(); ///<Creates a new file for the current run
  void initialiseTimeHists(Int_t binWidth=60); ///< Opens all the AraHistoHandler and AraTimeHistoHandler
  TFile *fHistoFile;
  AraTimeHistoHandler *fTempHistos[8];
  AraTimeHistoHandler *fRfpDisconeHistos[8];
  AraTimeHistoHandler *fRfpBatwingHistos[8];
  AraTimeHistoHandler* fDacHistos[6][4];
  AraTimeHistoHandler *fSclDisconeHistos[8];
  AraTimeHistoHandler *fSclBatPlusHistos[8];
  AraTimeHistoHandler *fSclBatMinusHistos[8];
  AraTimeHistoHandler *fSclTrigL1Histos[12];
  AraTimeHistoHandler *fSclGlobalHisto;
  

  //Run summary plotting nonsense
  Int_t fCurrentRun;
  UInt_t fEarliestTime;
  UInt_t fLatestTime;

  char fTouchFile[180];
  char fPlotDir[180];
  char fDataDir[180];
};

#endif //ARAHKPLOTTER_H
