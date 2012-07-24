////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////  exampleLoop.cxx 
////      Just a very simple example that loops over RawAraEvent objects 
////      calibrating them to make a UsefulAraEvent
////
////    Feb 2011,  rjn@hep.ucl.ac.uk 
////////////////////////////////////////////////////////////////////////////////

//Includes
#include <iostream>

//AraRoot Includes
#include "RawIcrrStationEvent.h"
#include "RawAtriStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "UsefulIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"

//Include FFTtools.h if you want to ask the correlation, etc. tools

//ROOT Includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"


RawIcrrStationEvent *rawIcrrEvPtr;
RawAtriStationEvent *rawAtriEvPtr;
RawAraStationEvent *rawEvPtr;
UsefulIcrrStationEvent *realIcrrEvPtr;
UsefulAtriStationEvent *realAtriEvPtr;

int main(int argc, char **argv)
{

  if(argc<2) {
    std::cout << "Usage\n" << argv[0] << " <input file>\n";
    std::cout << "e.g.\n" << argv[0] << " http://www.hep.ucl.ac.uk/uhen/ara/monitor/root/run1841/event1841.root\n";
    return 0;
  }

  TFile *fp = TFile::Open(argv[1]);
  if(!fp) {
    std::cerr << "Can't open file\n";
     return -1;
   }
   TTree *eventTree = (TTree*) fp->Get("eventTree");
   if(!eventTree) {
     std::cerr << "Can't find eventTree\n";
     return -1;
   }
   
   //Now check the electronics type of the station


   int isIcrrEvent=0;
   int isAtriEvent=0;

   //Check an event in the run Tree and see if it is station1 or TestBed (stationId<2)
   eventTree->SetBranchAddress("event",&rawEvPtr);
   eventTree->GetEntry(0);

   if((rawEvPtr->stationId)<2){
     isIcrrEvent=1;
     isAtriEvent=0;
   }
   else{
     isIcrrEvent=0;
     isAtriEvent=1; 
   }
   eventTree->ResetBranchAddresses();

   //Now set the appropriate branch addresses
   //The Icrr case
   if(isIcrrEvent){

     eventTree->SetBranchAddress("event", &rawIcrrEvPtr);
     std::cerr << "Set Branch address to Icrr\n";

   }
   //The Atri case
   else{

     eventTree->SetBranchAddress("event", &rawAtriEvPtr);
     std::cerr << "Set Branch address to Atri\n";

   }

   //Now we set up out run list
   Long64_t numEntries=eventTree->GetEntries();
   Long64_t starEvery=numEntries/80;
   if(starEvery==0) starEvery++;

   //jpd print to screen some info
   std::cerr << "isAtri " << isAtriEvent << " isIcrr " << isIcrrEvent << " number of entries is " <<  numEntries << std::endl;


   for(Long64_t event=0;event<numEntries;event++) {
     if(event%starEvery==0) {
       std::cerr << "*";       
     }

     //This line gets the RawIcrr or RawAtri Event
     eventTree->GetEntry(event);

     //Here we create a useful event Either an Icrr or Atri event

     if(isIcrrEvent){
       realIcrrEvPtr = new UsefulIcrrStationEvent(rawIcrrEvPtr, AraCalType::kLatestCalib);
     }
     else if(isAtriEvent){
       realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kLatestCalib);
     }
     
     //Now you can do whatever analysis you want
     //e.g.
     TGraph *chan1;
     if(isIcrrEvent){

       //Do stuff
       //       std::cerr << "Doing stuff with the Icrr event\n";
       chan1 = realIcrrEvPtr->getGraphFromRFChan(0);
       //Check the short waveform problems

       int shortWaveform=0;
       for(int chip=0; chip<3;chip++){
	 shortWaveform=realIcrrEvPtr->shortWaveform(chip);
	 if(shortWaveform){
	   fprintf(stderr, "shortWaveform on labChip %i\n", chip);
	 }

       }
       
     }
     else if(isAtriEvent){

       //Do stuff
       //       std::cerr << "Doing stuff with the Atri event\n";
       chan1 = realAtriEvPtr->getGraphFromRFChan(0);

       


     }

     delete chan1;

   
   }
   std::cerr << "\n";

}
