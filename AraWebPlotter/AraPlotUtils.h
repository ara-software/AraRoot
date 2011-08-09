////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple utiltities for making the web plots                  /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- December 2010                         /////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


#ifndef ARAPLOTUTILS_H
#define ARAPLOTUTILS_H
#include "TObject.h"

#define NUM_TIME_RANGES 5 

class TCanvas;
class TGraph;
class TMultiGraph;


namespace AraPlotTime {
  typedef enum EAraPlotTime{
    kFullTime =1,
    kOneHour,
    kSixHours,
    kTwelveHours,
    kOneDay,
    kNoTime
  } AraPlotTime_t;

  const char * getTimeString(AraPlotTime_t plotTime);
  const char * getTimeTitleString(AraPlotTime_t plotTime);
  UInt_t getStartTime(UInt_t currentTime,AraPlotTime_t plotTime);
}

namespace AraPlotUtils {
  void setDefaultStyle();
  int getNiceColour(int index);
  int getNiceMarker(int index);
  TMultiGraph* plotMultigraph(TCanvas *can, TGraph *gr[], 
		     Int_t numGraphs, 
		     const char *plotTitle=0, const char *xTitle=0, const char *yTitle=0,
		     int timeDisplay=0);
  int updateTouchFile(char *touchFile, UInt_t unixTime);
  
}

//AraPlotTime_t operator++ (const AraPlotTime_t& x, int);

#endif //ARAPLOTUTILS_H
