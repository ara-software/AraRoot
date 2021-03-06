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

#include "AraGeomTool.h"
#include "araAtriStructures.h"
#include "AtriSensorHkData.h"  

void processHk();
void makeHkTree(char *inputName, char *outDir);

AraSensorHk_t theSensorHkStruct;
TFile *theFile;
TTree *sensorHkTree;
AtriSensorHkData *theSensorHk=0;
char outName[FILENAME_MAX];
UInt_t realTime;
Int_t runNumber;
Int_t lastRunNumber;
Int_t stationIdInt;
AraStationId_t stationId;


int main(int argc, char **argv) {
  if(argc<3) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out dir>" << std::endl;
    return -1;
  }
  if(argc>=4) 
    runNumber=atoi(argv[3]);
  if(argc>=5) {
    stationIdInt=atoi(argv[4]);  //To override station id
    stationId=AraGeomTool::getAtriStationId(stationIdInt);
  }
  makeHkTree(argv[1],argv[2]);
  return 0;
}
  

void makeHkTree(char *inputName, char *outFile) {
  cout << inputName << "\t" << outFile << endl;
  strncpy(outName,outFile,FILENAME_MAX);
  theSensorHk = new AtriSensorHkData();
  //    cout << sizeof(AraSensorHk_t) << endl;
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
      numBytes=gzread(infile,&theSensorHkStruct,sizeof(AraSensorHk_t));
      //      std::cout << numBytes << "\n";


	if(stationIdInt!=0)
	   theSensorHkStruct.gHdr.stationId=stationId;


      if(numBytes==0) break;
      if(numBytes!=sizeof(AraSensorHk_t)) {
	if(numBytes)
	  cerr << "Read problem: " <<numBytes << " of " << sizeof(AraSensorHk_t) << endl;
	error=1;
	break;
      }
      //      cout << "Hk: " << theSensorHkStruct.unixTime << endl;
      processHk();
    }
    gzclose(infile);
    //	if(error) break;
  }
  if(sensorHkTree)
    sensorHkTree->AutoSave();
  //    theFile->Close();
}


void processHk() {
  //  cout << "processHk:\t" << theSensorHkStruct.eventNumber << endl;
  static int doneInit=0;
  
  if(!doneInit) {
    //    char dirName[FILENAME_MAX];
    //    char fileName[FILENAME_MAX];
    //    sprintf(dirName,"%s/run%d",outDirName,runNumber);
    //    gSystem->mkdir(dirName,kTRUE);
    //    sprintf(fileName,"%s/eventFile%d.root",dirName,runNumber);
    cout << "Creating File: " << outName << endl;
    theFile = new TFile(outName,"RECREATE");
    sensorHkTree = new TTree("sensorHkTree","Tree of ARA Hks");
    sensorHkTree->Branch("run",&runNumber,"run/I");
    sensorHkTree->Branch("sensorHk","AtriSensorHkData",&theSensorHk);
    
    doneInit=1;
  }  
  //  cout << "Here: "  << theSensorHk.eventNumber << endl;
  if(theSensorHk) delete theSensorHk;
  theSensorHk = new AtriSensorHkData(&theSensorHkStruct);
  sensorHkTree->Fill();  
  lastRunNumber=runNumber;
  //  delete theSensorHk;
}
