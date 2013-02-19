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

#include "AraGeomTool.h"
#include "araAtriStructures.h"
#include "RawAtriStationEvent.h"  

void process();
void makeTree(char *inputName, char *outDir);

AraStationEventHeader_t theEventHeader;

char *dataBuffer;
TFile *theFile;
TTree *eventTree;
RawAtriStationEvent *theEvent=0;
char outName[FILENAME_MAX];
UInt_t realTime;
Int_t runNumber;
//Int_t lastRunNumber;
Int_t stationIdInt;
AraStationId_t stationId;

int main(int argc, char **argv) {
  dataBuffer = new char[200000];
  theEvent=0;
  if(argc<3) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out dir>" << std::endl;
    return -1;
  }
  if(argc>=4) 
     runNumber=atoi(argv[3]); //To override runNumber
  if(argc>=5) 
     stationIdInt=atoi(argv[4]);  //To override station id
  stationId=AraGeomTool::getAtriStationId(stationIdInt);
  makeTree(argv[1],argv[2]);
  delete [] dataBuffer;
  return 0;
}
  

void makeTree(char *inputName, char *outFile) {
  cout << inputName << "\t" << outFile << endl;
  strncpy(outName,outFile,FILENAME_MAX);
  theEvent = new RawAtriStationEvent();
  //    cout << sizeof(AraStationEventHeader_t) << endl;
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
      numBytes=gzread(infile,&theEventHeader,sizeof(AraStationEventHeader_t));
      //      std::cout << numBytes << "\n";
      if(numBytes==0) break;
      if(numBytes!=sizeof(AraStationEventHeader_t)) {
	if(numBytes)
	  cerr << "Read problem: " <<numBytes << " of " << sizeof(AraStationEventHeader_t) << endl;
	error=1;
	break;
      }
      if(stationIdInt!=0)
	 theEventHeader.gHdr.stationId=stationId;
      
      //      std::cout << (int)theEventHeader.gHdr.stationId << "\t" << (int)stationId << "\n";

      if(theEventHeader.gHdr.numBytes>0) {
	//	std::cout << "Num bytes: " << theEventHeader.gHdr.numBytes << "\t" << theEventHeader.numBytes << "\n";
	//	std::cout << "Event number: " << theEventHeader.eventNumber << "\t" << theEventHeader.unixTime << "\t" << theEventHeader.unixTimeUs << "\n";
	
	
	Int_t numDataBytes=theEventHeader.gHdr.numBytes-sizeof(AraStationEventHeader_t);
	numBytes=gzread(infile,dataBuffer,numDataBytes);
	//	std::cout << numBytes << "\n";
	if(numBytes==0) break;
	if(numBytes!=numDataBytes) {
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " <<  numDataBytes << endl;
	  error=1;
	  break;
	}
	process();
     	//	exit(0);
      }
      else {
	std::cerr << "How can gHdr.numBytes = " << theEventHeader.gHdr.numBytes << "\n";
	error=1;
	break;
      }

      //      cout << ": " << theEventHeader.unixTime << endl;
     }
    gzclose(infile);
    //	if(error) break;
  }
  if(eventTree)
    eventTree->AutoSave();
  //    theFile->Close();
}


void process() {
  //  cout << "process:\t" << theEventHeader.eventNumber << endl;
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
    eventTree->Branch("event","RawAtriStationEvent",&theEvent);
    
    doneInit=1;
  }  
  //  cout << "Here: "  << theEvent.eventNumber << endl;
  if(theEvent) delete theEvent;
  
  theEvent = new RawAtriStationEvent(&theEventHeader,dataBuffer);
  eventTree->Fill();  
  //  lastRunNumber=runNumber;
  //  delete theEvent;
}
