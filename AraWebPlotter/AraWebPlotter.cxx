#include <cstdio>
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
#include "araTestbedStructures.h"
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
  if(argc<2) {
    std::cout << "Usage:\n";
    std::cout << gSystem->BaseName(argv[0]) << " <event file> <hk file>";
    return -1;
  }
  char *eventFileName=argv[1];
  char *hkFileName=0;
  if(argc>2) 
    hkFileName=argv[3];

  //  theEvent = new RawIcrrStationEvent();
  //Step one is to read the config file.
   gROOT->SetBatch();
  AraWebPlotterConfig *araConfig = new AraWebPlotterConfig();
  std::cout << "Root File Dir: " << araConfig->getRootFileDir() << "\n";
  std::cout << "Plot Dir: " << araConfig->getPlotDir() << "\n";
  std::cout << "Make Event Plots " << araConfig->getEventPlotFlag() << "\n";

    

  //Step two is to open the ROOT Files
  if(openRootFiles(eventFileName,hkFileName)<0) {
    std::cerr << "Can't open root files!\n";
    return -1;
  }

  //Step three is to make the plotter
  AraHkPlotter *hkPlotter = new AraHkPlotter(araConfig->getPlotDir(),araConfig->getRootFileDir());
 AraEventPlotter *eventPlotter = new AraEventPlotter(araConfig->getPlotDir(),araConfig->getRootFileDir());
 eventPlotter->setEventPlotFlag(araConfig->getEventPlotFlag());

  //Now we can try and read in the data
  if(eventTree) {
     //     eventTree->Refresh();
     std::cout << "Event tree has: " << eventTree->GetEntries() << "\n";
     Long64_t numEvents=eventTree->GetEntries();
     //    numEvents=1000;
     Long64_t starEvery=(numEvents)/80;
     if(starEvery==0)starEvery++;
     for(Long64_t i=0;i<numEvents;i++) {
	if(i%starEvery==0) std::cerr<<"*";
	eventTree->GetEntry(i);
	eventPlotter->addEvent(runNumber,theEvent);
	hkPlotter->addHk(runNumber,theEvent->head.unixTime,&(theEvent->hk));    
     }
     std::cerr << "\n";
//     hkPlotter->makePlots();
     hkPlotter->saveFiles();
     hkPlotter->makeLatestRunPlots();
//     eventPlotter->makePlots();
     eventPlotter->makeLatestRunPlots();
     eventPlotter->saveFiles();

  }
  
  if(hkTree) {
     //hkTree->Refresh();
    std::cout << "Hk tree has: " << hkTree->GetEntries() <<  "\n";
    Long64_t numHks=hkTree->GetEntries();
    Long64_t starEvery=(numHks)/80;
    if(starEvery==0) starEvery++;
     for(Long64_t i=0;i<numHks;i++) {
       if(i%starEvery==0) std::cerr  << "*";
	hkTree->GetEntry(i);
	hkPlotter->addHk(runNumber,theHk->unixTime,&(theHk->hk));   
     }
     std::cerr << "\n";
     std::cout << "Read entries\n";
//     hkPlotter->makePlots();
     hkPlotter->saveFiles();
     hkPlotter->makeLatestRunPlots();
  }
  
  //  delete hkPlotter;
  //  delete eventPlotter;
  //  gApplication->Terminate();


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
