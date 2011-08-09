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
#include "UsefulAraEvent.h"
#include "RawAraEvent.h"
//Include FFTtools.h if you want to ask the correlation, etc. tools

//ROOT Includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"


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
   
   RawAraEvent *evPtr=0;
   eventTree->SetBranchAddress("event",&evPtr);
   
   Long64_t numEntries=eventTree->GetEntries();
   Long64_t starEvery=numEntries/80;
   if(starEvery==0) starEvery++;


   for(Long64_t event=0;event<numEntries;event++) {
     if(event%starEvery==0) {
       std::cerr << "*";       
     }
     //This line gets the RawAraEvent
     eventTree->GetEntry(event);

     //This line creates the UsefulAraEvent
     UsefulAraEvent realEvent(evPtr,AraCalType::kLatestCalib);
     
     //Now you can do whatever analysis you want
     //e.g.
     TGraph *chan1 = realEvent.getGraphFromRFChan(0);
     // Do something
     delete chan1;

   
   }
   std::cerr << "\n";

}
