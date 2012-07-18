#include <cstdio>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include <libgen.h>     
#include <cstdlib>
 
using namespace std;

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"

#define HACK_FOR_ROOT

#include "araAtriStructures.h"
#include "RawAtriSimpleStationEvent.h"  

void process();
void makeTree(char *inputName, char *outDir);

AraSimpleStationEvent_t theEventStruct;
TFile *theFile;
TTree *eventTree;
RawAtriSimpleStationEvent *theEvent=0;
char outName[FILENAME_MAX];
UInt_t realTime;
Int_t runNumber;
Int_t lastRunNumber;


int main(int argc, char **argv) {
  if(argc<3) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out dir>" << std::endl;
    return -1;
  }
  if(argc==4) 
    runNumber=atoi(argv[3]);
  makeTree(argv[1],argv[2]);
  return 0;
}
  

void makeTree(char *inputName, char *outFile) {
  cout << inputName << "\t" << outFile << endl;
  strncpy(outName,outFile,FILENAME_MAX);
  theEvent = new RawAtriSimpleStationEvent();
  //    cout << sizeof(AraSimpleStationEvent_t) << endl;
  ifstream SillyFile(inputName);

  int numBytes=0;
  char fileName[180];
  int error=0;
  //    int eventNumber=1;
  int counter=0;
  while(SillyFile >> fileName) {
    if(counter%100==0) 
      cout << fileName << endl;
    counter++;
    
    //    const char *subDir = gSystem->DirName(fileName);
    //    const char *subSubDir = gSystem->DirName(subDir);
    //    const char *eventDir = gSystem->DirName(subSubDir);
    //    const char *runDir = gSystem->DirName(eventDir);
    //    const char *justRun = gSystem->BaseName(runDir);
    //	cout << justRun << endl;
    //    sscanf(justRun,"run%d",&runNumber);
    
    gzFile infile = gzopen (fileName, "rb");    
    //    std::cout << "gzeof: " << gzeof(infile) << "\n";
    for(int i=0;i<1000;i++) {	
      //      cout << i << endl;
      numBytes=gzread(infile,&theEventStruct,sizeof(AraSimpleStationEvent_t));
      //      std::cout << numBytes << "\n";
      if(numBytes==0) break;
      if(numBytes!=sizeof(AraSimpleStationEvent_t)) {
	if(numBytes)
	  cerr << "Read problem: " <<numBytes << " of " << sizeof(AraSimpleStationEvent_t) << endl;
	error=1;
	break;
      }
      //      cout << ": " << theEventStruct.unixTime << endl;
      process();
    }
    gzclose(infile);
    //	if(error) break;
  }
  if(eventTree)
    eventTree->AutoSave();
  //    theFile->Close();
}


void process() {
  //  cout << "process:\t" << theEventStruct.eventNumber << endl;
  static int doneInit=0;
  
  if(!doneInit) {
    //    char dirName[FILENAME_MAX];
    //    char fileName[FILENAME_MAX];
    //    sprintf(dirName,"%s/run%d",outDirName,runNumber);
    //    gSystem->mkdir(dirName,kTRUE);
    //    sprintf(fileName,"%s/eventFile%d.root",dirName,runNumber);
    cout << "Creating File: " << outName << endl;
    theFile = new TFile(outName,"RECREATE");
    eventTree = new TTree("eventTree","Tree of ARA Event's");
    eventTree->Branch("run",&runNumber,"run/I");
    eventTree->Branch("event","RawAtriSimpleStationEvent",&theEvent);
    
    doneInit=1;
  }  
  //  cout << "Here: "  << theEvent.eventNumber << endl;
  if(theEvent) delete theEvent;
  theEvent = new RawAtriSimpleStationEvent(&theEventStruct);
  eventTree->Fill();  
  lastRunNumber=runNumber;
  //  delete theEvent;
}
