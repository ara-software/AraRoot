////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple Class to handle the config fil                       /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- December 2010                         /////////
////////////////////////////////////////////////////////////////////////////

#ifndef ARAWEBPLOTTERCONFIG_H
#define ARAWEBPLOTTERCONFIG_H
#include <TObject.h>

class AraWebPlotterConfig
{
 public:
  AraWebPlotterConfig();
  
  //Should change these so they are safer
  //char *getEventLinkDir() {return fEventLinkDir;}

  //  char *getHkLinkDir() {return fHkLinkDir;}

  char *getRootFileDir() {return fRootFileDir;}

  char *getPlotDir() {return fPlotDir;}

  int getEventPlotFlag() {return fMakeEventPlots;}

 private:
  void readConfigFile();
  //  char fEventLinkDir[FILENAME_MAX];
  //  char fHkLinkDir[FILENAME_MAX];
  char fRootFileDir[FILENAME_MAX];
  char fPlotDir[FILENAME_MAX];
  int fMakeEventPlots;

};

#endif //ARAWEBPLOTTERCONFIG_H
