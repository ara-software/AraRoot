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
Int_t stationIdInt;
AraStationId_t stationId;

Int_t newHkFormat=0;

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
  std::cout << argc << "\t" << stationIdInt << "\t" << (int)stationId << "\n";
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
    //    fprintf(stderr, "typeId %i verId %i subVerId %i\n", theGenericHeader.typeId, theGenericHeader.verId, theGenericHeader.subVerId);
    //    fprintf(stderr, "numBytes %i checksum %i numBytes(2_7) %lu numBytes(new) %lu genericHeaderSize %lu\n", theGenericHeader.numBytes, theGenericHeader.checksum,sizeof(AraEventHk2_7_t), sizeof(AraEventHk_t), sizeof(AtriGenericHeader_t) );
    

    if(theGenericHeader.verId>2){
      newHkFormat=1;      
    }
    else if(theGenericHeader.verId==2 && theGenericHeader.subVerId >7){
      newHkFormat=1;      
    }
    else{
      newHkFormat=0;      
    }
    int loopCount=0;
    while(newHkFormat && theGenericHeader.numBytes != sizeof(AraEventHk_t)){
      fprintf(stderr, "error - wrong numBytes (%d) for newHkFormat (%lu) oldFormat (%lu)\n", theGenericHeader.numBytes, sizeof(AraEventHk_t), sizeof(AraEventHk2_7_t));
      if(theGenericHeader.numBytes==0) theGenericHeader.numBytes=sizeof(AraEventHk2_7_t);
      

      numBytes = gzread(infile, &theEventHkStruct, theGenericHeader.numBytes-sizeof(AtriGenericHeader_t));
      numBytes = gzread(infile, &theGenericHeader, sizeof(AtriGenericHeader_t));
      if(numBytes!=sizeof(AtriGenericHeader_t)) break;
      // fprintf(stderr, "read %i bytes\n", numBytes);
      // fprintf(stderr, "typeId %i verId %i subVerId %i\n", theGenericHeader.typeId, theGenericHeader.verId, theGenericHeader.subVerId);
      // fprintf(stderr, "numBytes %i checksum %i numBytes(2_7) %lu numBytes(new) %lu genericHeaderSize %lu\n", theGenericHeader.numBytes, theGenericHeader.checksum,sizeof(AraEventHk2_7_t), sizeof(AraEventHk_t), sizeof(AtriGenericHeader_t) );
      
      if(theGenericHeader.numBytes == sizeof(AraEventHk2_7_t)){ 
	newHkFormat=0;
	fprintf(stderr, "Forcing event format to AraEventHk2_7_t\n");
      }
      loopCount++;
      if(loopCount>10) break;
    }        
    //      newHkFormat=0;    
    while(newHkFormat==0 && theGenericHeader.numBytes != sizeof(AraEventHk2_7_t)){
      fprintf(stderr, "error - wrong numBytes (%d) for oldHkFormat (%lu) new format (%lu)\n", theGenericHeader.numBytes, sizeof(AraEventHk2_7_t), sizeof(AraEventHk_t));
      if(theGenericHeader.numBytes==0) theGenericHeader.numBytes=sizeof(AraEventHk_t);

      numBytes = gzread(infile, &theEventHkStruct, theGenericHeader.numBytes-sizeof(AtriGenericHeader_t));
      numBytes = gzread(infile, &theGenericHeader, sizeof(AtriGenericHeader_t));
      if(numBytes!=sizeof(AtriGenericHeader_t)) break;
      // fprintf(stderr, "read %i bytes\n", numBytes);
      // fprintf(stderr, "typeId %i verId %i subVerId %i\n", theGenericHeader.typeId, theGenericHeader.verId, theGenericHeader.subVerId);
      // fprintf(stderr, "numBytes %i checksum %i numBytes(2_7) %lu numBytes(new) %lu genericHeaderSize %lu\n", theGenericHeader.numBytes, theGenericHeader.checksum,sizeof(AraEventHk2_7_t), sizeof(AraEventHk_t), sizeof(AtriGenericHeader_t) );

      if(theGenericHeader.numBytes == sizeof(AraEventHk_t)){
	newHkFormat=1;
	fprintf(stderr, "Forcing event format to AraEventHk_t\n");
      }

      //      fprintf(stderr, "Forcing event format to AraEventHk_t\n");
      //      newHkFormat=1;
      loopCount++;
      if(loopCount>10) break;
    }

    
    gzclose(infile);

    infile = gzopen (fileName, "rb");    
    //    std::cout << "gzeof: " << gzeof(infile) << "\n";

    if(loopCount>=10) {
      std::cout << "Broken file -- giving up\n";
      return;
    }

    if(newHkFormat){
      for(int i=0;i<1000;i++) {	
	//      cout << i << endl;
	numBytes=gzread(infile,&theEventHkStruct,sizeof(AraEventHk_t));
	//      std::cout << numBytes << "\n";
	// fprintf(stderr, "read %i bytes\n", numBytes);
	//	fprintf(stderr, "verId %i subVerId %i\n", theEventHkStruct.gHdr.verId, theEventHkStruct.gHdr.subVerId);

	if(stationIdInt!=0)
	   theEventHkStruct.gHdr.stationId=stationId;

	if(numBytes==0) break;
	if(numBytes!=sizeof(AraEventHk_t)) {
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " << sizeof(AraEventHk_t) << endl;
	error=1;
	break;
	}
	//      cout << "Hk: " << theEventHkStruct.gHdr.unixTime << endl;
	processHk(1);
      }
    }//newHkFormat
    else{
      for(int i=0;i<1000;i++) {	
	//      cout << i << endl;
	numBytes=gzread(infile,&theEventHkStruct_2_7,sizeof(AraEventHk2_7_t));
	//      std::cout << numBytes << "\n";
	
	// fprintf(stderr, "read %i bytes\n", numBytes);
	//	fprintf(stderr, "verId %i subVerId %i\n", theEventHkStruct_2_7.gHdr.verId, theEventHkStruct_2_7.gHdr.subVerId);
	
	if(stationIdInt!=0)
	   theEventHkStruct.gHdr.stationId=stationId;

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
