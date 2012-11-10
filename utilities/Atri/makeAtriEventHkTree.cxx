#include <cstdio>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include <libgen.h>     
#include <stdlib.h>
 
using namespace std;

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"

#define HACK_FOR_ROOT

#include "araAtriStructures.h"
#include "AtriEventHkData.h"  

void processHk(Int_t newFormat);
void makeHkTree(char *inputName, char *outDir);

AraEventHk_t theEventHkStruct;
AraEventHk2_7_t theEventHkStruct_2_7;
TFile *theFile;
TTree *eventHkTree;
AtriEventHkData *theEventHk=0;
char outName[FILENAME_MAX];
UInt_t realTime;
Int_t runNumber;
Int_t lastRunNumber;

Int_t newHkFormat=0;

int main(int argc, char **argv) {
  if(argc<3) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out dir>" << std::endl;
    return -1;
  }
  if(argc==4) 
    runNumber=atoi(argv[3]);
  makeHkTree(argv[1],argv[2]);
  return 0;
}
  

void makeHkTree(char *inputName, char *outFile) {
  cout << inputName << "\t" << outFile << endl;
  strncpy(outName,outFile,FILENAME_MAX);
  theEventHk = new AtriEventHkData();
  //    cout << sizeof(AraEventHk_t) << endl;
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
    



    AtriGenericHeader_t theGenericHeader;
    gzFile infile = gzopen (fileName, "rb");    
    numBytes=gzread(infile, &theGenericHeader, sizeof(AtriGenericHeader_t));

    //    fprintf(stderr, "read %i bytes\n", numBytes);

    //    fprintf(stderr, "verId %i subVerId %i\n", theGenericHeader.verId, theGenericHeader.subVerId);
    if(theGenericHeader.verId>2){
      newHkFormat=1;      
      //      fprintf(stderr, "Use AraEventHk_t\n");
    }
    else if(theGenericHeader.verId==2 && theGenericHeader.subVerId >7){
      newHkFormat=1;      
      //      fprintf(stderr, "Use AraEventHk_t\n");
    }
    else{
      newHkFormat=0;      
      //      fprintf(stderr, "Use AraEventHk2_7_t\n");
    }

    gzclose(infile);


    infile = gzopen (fileName, "rb");    
    //    std::cout << "gzeof: " << gzeof(infile) << "\n";

    if(newHkFormat){
      for(int i=0;i<1000;i++) {	
	//      cout << i << endl;
	numBytes=gzread(infile,&theEventHkStruct,sizeof(AraEventHk_t));
	//      std::cout << numBytes << "\n";
	if(numBytes==0) break;
	if(numBytes!=sizeof(AraEventHk_t)) {
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " << sizeof(AraEventHk_t) << endl;
	error=1;
	break;
	}
	//      cout << "Hk: " << theEventHkStruct.unixTime << endl;
	processHk(1);
      }
    }//newHkFormat
    else{
      for(int i=0;i<1000;i++) {	
	//      cout << i << endl;
	numBytes=gzread(infile,&theEventHkStruct_2_7,sizeof(AraEventHk2_7_t));
	//      std::cout << numBytes << "\n";
	if(numBytes==0) break;
	if(numBytes!=sizeof(AraEventHk2_7_t)) {
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " << sizeof(AraEventHk2_7_t) << endl;
	  error=1;
	  break;
	}
	//      cout << "Hk: " << theEventHkStruct.unixTime << endl;
	processHk(0);
      }
    }//OldHkFormat

    gzclose(infile);
    //	if(error) break;
  }
  if(eventHkTree)
    eventHkTree->AutoSave();
  //    theFile->Close();
}


void processHk(Int_t newFormat) {
  //  cout << "processHk:\t" << theEventHkStruct.eventNumber << endl;
  static int doneInit=0;
  
  if(!doneInit) {
    //    char dirName[FILENAME_MAX];
    //    char fileName[FILENAME_MAX];
    //    sprintf(dirName,"%s/run%d",outDirName,runNumber);
    //    gSystem->mkdir(dirName,kTRUE);
    //    sprintf(fileName,"%s/eventFile%d.root",dirName,runNumber);
    cout << "Creating File: " << outName << endl;
    theFile = new TFile(outName,"RECREATE");
    eventHkTree = new TTree("eventHkTree","Tree of ARA Hks");
    eventHkTree->Branch("run",&runNumber,"run/I");
    eventHkTree->Branch("eventHk","AtriEventHkData",&theEventHk);
    
    doneInit=1;
  }  
  //  cout << "Here: "  << theEventHk.eventNumber << endl;

  if(newFormat){

    if(theEventHk) delete theEventHk;
    theEventHk = new AtriEventHkData(&theEventHkStruct);
    eventHkTree->Fill();  
    lastRunNumber=runNumber;
    //  delete theEventHk;
  }//newFormat
  else{

    if(theEventHk) delete theEventHk;
    theEventHk = new AtriEventHkData(&theEventHkStruct_2_7);
    eventHkTree->Fill();  
    lastRunNumber=runNumber;
  }//Old Format
}
