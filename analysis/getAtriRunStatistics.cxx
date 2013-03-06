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
void processFileList(char *inputName, char *outDir);

AraStationEventHeader_t theEventHeader;

char *dataBuffer;
TFile *outFile;
TTree *outTree;
RawAtriStationEvent *theEvent=0;
char outName[FILENAME_MAX];
UInt_t realTime;
Int_t runNumber;
Int_t stationIdInt;
AraStationId_t stationId;

//For statistics

Int_t numEvents=0;
Int_t numEvents_CPU=0;
Int_t numEvents_RF0=0;
Int_t numEvents_CALPULSER=0;
Double_t eventRate=0;
UInt_t unixTimeStart=0;

Int_t lastNumEvents=0;
Int_t lastNumEvents_CPU=0;
Int_t lastNumEvents_RF0=0;
Int_t lastNumEvents_CALPULSER=0;


//For Tree
UInt_t lastUnixTime=0;
UInt_t thisUnixTime=0;
Double_t calPulserRate=0;
Double_t RF0Rate=0;
Double_t CPURate=0;
Int_t lastTime=0;


int main(int argc, char **argv) {
  dataBuffer = new char[200000];
  theEvent=0;
  if(argc<3) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out dir>" << std::endl;
    return -1;
  }

  processFileList(argv[1],argv[2]);
  delete [] dataBuffer;
  return 0;
}
  

void processFileList(char *inputName, char *outFileName) {
  cout << inputName << "\t" << outFileName << endl;

  theEvent = new RawAtriStationEvent();

  ifstream SillyFile(inputName);
  outFile = new TFile(outFileName, "RECREATE");
  outTree = new TTree("runStatsTree", "Tree of run statistics");
  outTree->Branch("lastUnixTime", &lastUnixTime, "lastUnixTime/i");
  outTree->Branch("thisUnixTime", &thisUnixTime, "thisUnixTime/i");
  outTree->Branch("calPulserRate", &calPulserRate, "calPulserRate/D");
  outTree->Branch("RF0Rate", &RF0Rate, "RF0Rate/D");
  outTree->Branch("CPURate", &CPURate, "CPURate/D");

  outTree->Branch("eventRate", &eventRate, "eventRate/D");
  outTree->Branch("unixTimeStart", &unixTimeStart, "unixTimeStart/i");
  outTree->Branch("numEvents", &numEvents, "numEvents/I");
  outTree->Branch("numEvents_CALPULSER", &numEvents_CALPULSER, "numEvents_CALPULSER/I");
  outTree->Branch("numEvents_RF0", &numEvents_RF0, "numEvents_RF0/I");
  outTree->Branch("numEvents_CPU", &numEvents_CPU, "numEvents_CPU/I");
  outTree->Branch("stationId", &stationId, "stationId/I");
  outTree->Branch("lastTime", &lastTime, "lastTime/I");

  int numBytes=0;
  char fileName[180];
  int error=0;
  //    int eventNumber=1;
  int counter=0;
  while(SillyFile >> fileName) {
    if(counter%100==0) 
      cout << fileName << endl;
    counter++;
    
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
//   //Do something with stats
//   if(numEvents > 0 && thisUnixTime > unixTimeStart)
//     eventRate = (1.*numEvents) / (thisUnixTime - unixTimeStart);
//   else
//     eventRate = 0;

//   printf("Run statistics ****\n\n");
//   printf("unixTimeStart\t%u\tthisUnixTime\t%u\n", unixTimeStart, thisUnixTime);
//   printf("numEvents\t\t%i\n", numEvents);
//   printf("numEvents_CPU\t%i\n", numEvents_CPU);
//   printf("numEvents_RF0\t%i\n", numEvents_RF0);
//   printf("numEvents_CALPULSER\t%i\n", numEvents_CALPULSER);
//   printf("eventRate\t\t%f\n", eventRate);

  process();

  outFile->Write();

}


void process() {
  static int doneInit=0;
  static int firstTime=1;
  if(!doneInit) {
    doneInit=1;
  }  
  if(theEvent) delete theEvent;
  
  theEvent = new RawAtriStationEvent(&theEventHeader,dataBuffer);


  //Create stats
  if(firstTime){
    unixTimeStart=theEvent->unixTime;
    lastUnixTime=unixTimeStart;
    firstTime=0;
  }
  thisUnixTime=theEvent->unixTime;
  numEvents++;
  if(theEvent->numReadoutBlocks<80) numEvents_CPU++;
  else numEvents_RF0++;
  if(theEvent->isCalpulserEvent()) numEvents_CALPULSER++;


  if(thisUnixTime >= lastUnixTime + 60*30 || lastTime){
    
    if(thisUnixTime > lastUnixTime){

      eventRate= (1.*(numEvents - lastNumEvents)) / (thisUnixTime - lastUnixTime);
      calPulserRate= (1.*(numEvents_CALPULSER - lastNumEvents_CALPULSER)) / (thisUnixTime - lastUnixTime);
      RF0Rate= (1.*(numEvents_RF0 - lastNumEvents_RF0)) / (thisUnixTime - lastUnixTime);
      CPURate= (1.*(numEvents_CPU - lastNumEvents_CPU)) / (thisUnixTime - lastUnixTime);

    }
    //Otherwise they are just the last rate

    outTree->Fill();
    lastUnixTime=thisUnixTime;
    lastNumEvents=numEvents;
    lastNumEvents_CPU=numEvents_CPU;
    lastNumEvents_RF0=numEvents_RF0;
    lastNumEvents_CALPULSER=numEvents_CALPULSER;


  }


}
