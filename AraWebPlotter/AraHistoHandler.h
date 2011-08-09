////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle the config fil                       /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- December 2010                         /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ARAHISTOHANDLER_H
#define ARAHISTOHANDLER_H

#include "TFile.h"
#include "TDirectory.h"
#include "TNamed.h"
#include "AraPlotUtils.h"
#include "TH1.h"
#include "TH2.h"
//#include <zlib.h>

#include <map>


class AraHistoHandler 
{
 public:

  //  typedef std::pair<Int_t,TH1D> histoPair;
  typedef std::map<UInt_t,TH1D*> histoMap;
  typedef std::map<UInt_t,Int_t> countMap;
  AraHistoHandler(TFile *fpHist,const char *name, const char *title );
  virtual ~AraHistoHandler();
  void addFile(TFile *fpNext);
  void addHisto(UInt_t unixTime, TH1D *histo);
  void Write();
  char *GetName() { return fName;}

   UInt_t getLastTime();
   TH1D *getTimeHisto(UInt_t firstTime, UInt_t lastTime);
   TH1D *getTimeHisto(AraPlotTime::AraPlotTime_t plotTime);
   TH1D *getCurrentTimeHisto(AraPlotTime::AraPlotTime_t plotTime);
   TH2D *getTimeColourHisto(UInt_t firstTime, UInt_t lastTime, Int_t numPoints=10);
   TH2D *getTimeColourHisto(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints=10);
   TH2D *getCurrentTimeColourHisto(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints=10);
 private:

   char fName[180];
   char fTitle[180];
   TFile *fHistoFile;
   TDirectory *fDirectory;
   histoMap theHistoMap;
   countMap theCountMap;
   countMap hasChangedMap;

  ClassDef(AraHistoHandler,1);

};


#endif //ARAHISTOHANDLER_H
