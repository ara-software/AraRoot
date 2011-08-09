////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle the config fil                       /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- December 2010                         /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ARATIMEHISTOHANDLER_H
#define ARATIMEHISTOHANDLER_H

#include "TNamed.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "AraPlotUtils.h"
//#include <zlib.h>

#include <map>

typedef std::pair<Int_t,Double_t> variablePair;
typedef std::map<UInt_t,variablePair> variableMap;

class AraTimeHistoHandler : public TNamed
{
 public:
   AraTimeHistoHandler();
   AraTimeHistoHandler(const char *name, const char *title, Int_t binWidth=60);
   void addVariable(UInt_t unixTime, Double_t variable);
   
   void addAraTimeHistoHandler(AraTimeHistoHandler *other);

  //  TGraph *getAllTimeGraph(int numPoints=60);
   UInt_t getLastTime();
   TGraph *getTimeGraph(UInt_t firstTime, UInt_t lastTime, Int_t numPoints=60);
   TGraph *getTimeGraph(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints=60);
   TGraph *getCurrentTimeGraph(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints=60);


 private:
   Int_t fBinWidthInSeconds;
   variableMap theMap;
   variableMap theMapSq;
   
   
  ClassDef(AraTimeHistoHandler,1);

};

#endif //ARATIMEHISTOHANDLER_H
