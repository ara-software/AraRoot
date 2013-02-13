#include <cstdio>
#include <fstream>
#include <iostream>
#include <zlib.h>
#include <libgen.h>     
#include <cstdlib>
 


#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"

#include "araAtriStructures.h"
#include "RawAtriStationEvent.h"  
#include "UsefulAtriStationEvent.h"  


extern "C" {
   #include "util.h"
   #include "syslog.h"
}

using namespace std;

void processEvent();
int filterEvent(UsefulAtriStationEvent *evPtr);
void processFileList(char *inputName, char *pedName, char *outDir);
bool oneInTenSelection(UsefulAtriStationEvent *evPtr);


AraStationEventHeader_t theEventHeader;
char *inBuffer;
char *outBuffer;
TFile *theFile;
TTree *eventTree;
RawAtriStationEvent *theEvent=0;
UsefulAtriStationEvent *theUsefulEvent=0;
AraEventCalibrator *theCalibrator=0;
char outName[FILENAME_MAX];

Int_t runNumber;
Int_t lastRunNumber;

ARAWriterStruct_t eventWriter;

 

int main(int argc, char **argv) {
  inBuffer = new char[200000];
  outBuffer = new char[200000];
  theEvent=0;
  if(argc<5) {
    std::cout << "Usage: " << basename(argv[0]) << " <file list> <ped file> <out dir> <run Number>" << std::endl;
    return -1;
  }

  runNumber=atoi(argv[4]);

  processFileList(argv[1],argv[2], argv[3]);
  delete [] inBuffer;
  delete [] outBuffer;
  return 0;
}
  

void processFileList(char *inputName, char *pedName, char *outDir) {
  
  //strncpy(outName,outDir,FILENAME_MAX);
  sprintf(outName, "%s/run_%06d/event", outDir, runNumber);

  cout << inputName << "\t" << outName << endl;

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

    //Work out the station Id for this event file
    gzFile infile = gzopen (fileName, "rb");    
    numBytes=gzread(infile,&theEventHeader,sizeof(AraStationEventHeader_t));
    int stationId = theEventHeader.gHdr.stationId;
    theCalibrator = new AraEventCalibrator();
    theCalibrator->setAtriPedFile(pedName,stationId);
    gzclose(infile);
    printf("Got file %s stationId %d\n", fileName, stationId);//DEBUG

    infile = gzopen (fileName, "rb");    

    //    std::cout << "gzeof: " << gzeof(infile) << "\n";
    for(int i=0;i<1000;i++) {	 //FIXME? -- are we limiting the number of events in a file here?
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
      if(theEventHeader.gHdr.numBytes>0) {
	//	std::cout << "Num bytes: " << theEventHeader.gHdr.numBytes << "\t" << theEventHeader.numBytes << "\n";
	//	std::cout << "Event number: " << theEventHeader.eventNumber << "\t" << theEventHeader.unixTime << "\t" << theEventHeader.unixTimeUs << "\n";
	
	
	Int_t numDataBytes=theEventHeader.gHdr.numBytes-sizeof(AraStationEventHeader_t);
	//	fprintf(stderr, "numBytes %d\n", theEventHeader.gHdr.numBytes);
	numBytes=gzread(infile,inBuffer,numDataBytes);
	//	std::cout << numBytes << "\n";
	if(numBytes==0) break;
	if(numBytes!=numDataBytes) {
	  if(numBytes)
	    cerr << "Read problem: " <<numBytes << " of " <<  numDataBytes << endl;
	  error=1;
	  break;
	}
	//	fprintf(stderr, "readBytes %d\n", numBytes+sizeof(AraStationEventHeader_t));

	 processEvent();
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
   closeWriter(&eventWriter);
 }


 void processEvent() {
   //  cout << "process:\t" << theEventHeader.eventNumber << endl;
   static int doneInit=0;
   static int new_file_flag=0; //FIXME
   if(!doneInit) {
      initWriter(&eventWriter,
		runNumber,
		5,
		100,
		100,
		EVENT_FILE_HEAD,
		outName,
		NULL);    


     doneInit=1;
   }  
   //  cout << "Here: "  << theEvent.eventNumber << endl;
   if(theEvent) delete theEvent;
   if(theUsefulEvent) delete theUsefulEvent;

   theEvent = new RawAtriStationEvent(&theEventHeader,inBuffer);
   theUsefulEvent = new UsefulAtriStationEvent(theEvent, AraCalType::kLatestCalib);



   if(filterEvent(theUsefulEvent)==1){
     int numToCopy = theEventHeader.gHdr.numBytes;
     int upToByte = sizeof(AraStationEventHeader_t);
     memcpy(  &outBuffer[0],&theEventHeader , sizeof(AraStationEventHeader_t)); 
     memcpy( &outBuffer[upToByte], inBuffer, numToCopy);

    int retVal = writeBuffer( &eventWriter, outBuffer, numToCopy, &(new_file_flag));



     //do something
    //   fprintf(stderr, "filtered\n");

  }



  //  delete theEvent;
}

int filterEvent(UsefulAtriStationEvent *evPtr){

  //do something more clever here
  return 1;

}

bool oneInTenSelection( UsefulAtriStationEvent *evPtr)//FIXME 
{

   bool retcode = false;

   int  random_number_between_1_and_10 = (rand() % 10 + 1);
   if ( random_number_between_1_and_10 < 2 ) {
      retcode = true;
   } else {
      retcode = false;
   }

   return retcode;

}
