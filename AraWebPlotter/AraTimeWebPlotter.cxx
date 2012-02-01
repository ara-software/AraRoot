#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include <libgen.h>     
 
//Root includes
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TApplication.h"

#define HACK_FOR_ROOT

//ARA includes
#include "araIcrrStructures.h"
#include "RawIcrrStationEvent.h"  
#include "FullIcrrHkEvent.h"  
#include "AraWebPlotterConfig.h"
#include "AraHkPlotter.h"
#include "AraEventPlotter.h"

int openRootFiles(char *eventFileName, char *hkFileName);

Int_t runNumber;
TFile *theEventFile;
TChain *eventTree;
RawIcrrStationEvent *theEvent=0;


TFile *theHkFile;
TChain *hkTree;
FullIcrrHkEvent *theHk=0;



int main(int argc, char **argv) {
  //  theEvent = new RawIcrrStationEvent();
  //Step one is to read the config file.
   gROOT->SetBatch();
  AraWebPlotterConfig *araConfig = new AraWebPlotterConfig();
  std::cout << "Root File Dir: " << araConfig->getRootFileDir() << "\n";
  std::cout << "Plot Dir: " << araConfig->getPlotDir() << "\n";
  std::cout << "Make Event Plots " << araConfig->getEventPlotFlag() << "\n";

    


  //Step two is to make the plotter
  AraHkPlotter *hkPlotter = new AraHkPlotter(araConfig->getPlotDir(),araConfig->getRootFileDir());
  AraEventPlotter *eventPlotter = new AraEventPlotter(araConfig->getPlotDir(),araConfig->getRootFileDir());
  eventPlotter->setEventPlotFlag(araConfig->getEventPlotFlag());

 //Now we need to load the time hists
  hkPlotter->loadAllTimeHists();
  eventPlotter->loadAllTimeHists();

  //Step three is to make the time plots
  hkPlotter->makePlots();
  std::cout << "Done hkPlotter->makePlots()" << "\n";
  eventPlotter->makePlots();
  std::cout << "Done eventPlotter->makePlots()" << "\n";

  //A bit nasty but this gets us out of there quickly, probably does horrible things to the operating system
  //  abort();
  return 0;
}

Long64_t getLastEntry(char *fLastHkProcessedFile)
{
  std::ifstream LastHk(fLastHkProcessedFile);
  if(LastHk) {
    Long64_t temp;
    LastHk >> temp;
    if(temp>0) return temp;
    LastHk.close();
  }
  return 0;
}


void setLastEntry(char *fLastHkProcessedFile, Long64_t lastHkEntry)
{
  std::ofstream LastHk(fLastHkProcessedFile);
  if(LastHk) {
    LastHk << lastHkEntry << "\n";
    LastHk.close();
  }
  else {
    std::cerr << "Can't open " << fLastHkProcessedFile << "\n";
  }

}




int openRootFiles(char *eventInName, char *hkInName)
{
  FileStat_t staty;
  if(eventInName) {
    theEvent=0;
    if(!eventTree)
      eventTree = new TChain("eventTree");
    if(gSystem->GetPathInfo(eventInName,staty)) {
      std::cout << "Can't find File: " << eventInName << std::endl;
      return -1;
    }
    else {
      //     theEventFile = new TFile(eventInName,"OLD");
      eventTree->Add(eventInName);
      if(eventTree->GetEntries()>0) {	
	eventTree->SetBranchAddress("run",&runNumber);
	eventTree->SetBranchAddress("event",&theEvent);
	
      }
      else { 
	std::cout << "Can't find eventTree\n";
	return -1;
      }
    }
  }
  
  if(hkInName) {
    theHk=0;
    hkTree = new TChain("hkTree");
    if(gSystem->GetPathInfo(hkInName,staty)) {
      std::cout << "Can't open File: " << hkInName << std::endl;
      return -1;
    }
    else {
      hkTree->Add(hkInName);
      if(hkTree->GetEntries()>0) {	
	hkTree->SetBranchAddress("fullhk",&theHk);
	hkTree->SetBranchAddress("run",&runNumber);
     }
      else {
	std::cout << "Can't find hkTree\n";
	return -1;
      }
    }
  }
  
  
  return 0;
}
