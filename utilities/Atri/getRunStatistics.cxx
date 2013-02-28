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
TFile *theFile;
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
UInt_t unixTimeEnd=0;




int main(int argc, char **argv) {
  dataBuffer = new char[200000];
  theEvent=0;
  if(argc<3) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <out dir>" << std::endl;
    return -1;
  }
  if(argc>=4) 
     runNumber=atoi(argv[3]); //To override runNumber
  if(argc>=5) {
    stationIdInt=atoi(argv[4]);  //To override station id
    stationId=AraGeomTool::getAtriStationId(stationIdInt);
  }
  //  std::cout << argc << "\t" << stationIdInt << "\t" << (int)stationId << "\n";

  processFileList(argv[1],argv[2]);
  delete [] dataBuffer;
  return 0;
}
  

void processFileList(char *inputName, char *outFile) {
  cout << inputName << "\t" << outFile << endl;
  strncpy(outName,outFile,FILENAME_MAX);
  theEvent = new RawAtriStationEvent();

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
  //Do something with stats
  if(numEvents > 0 && unixTimeEnd > unixTimeStart)
    eventRate = (1.*numEvents) / (unixTimeEnd - unixTimeStart);
  else
    eventRate = 0;

  printf("Run statistics ****\n\n");
  printf("unixTimeStart\t%u\tunixTimeEnd\t%u\n", unixTimeStart, unixTimeEnd);
  printf("numEvents\t\t%i\n", numEvents);
  printf("numEvents_CPU\t%i\n", numEvents_CPU);
  printf("numEvents_RF0\t%i\n", numEvents_RF0);
  printf("numEvents_CALPULSER\t%i\n", numEvents_CALPULSER);
  printf("eventRate\t\t%f\n", eventRate);

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
    firstTime=0;
  }
  numEvents++;
  if(theEvent->numReadoutBlocks<80) numEvents_CPU++;
  else numEvents_RF0++;
  if(theEvent->isCalpulserEvent()) numEvents_CALPULSER++;
  unixTimeEnd=theEvent->unixTime;

}
