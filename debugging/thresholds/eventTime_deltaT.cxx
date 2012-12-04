#include <iostream>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "RawAtriStationEvent.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"

using namespace std;

UInt_t getCorrectedTime(UInt_t rawTime, UInt_t ppsNumber, bool fudgeOn);

int main(int argc, char **argv)
{

  if(argc<3) {
    std::cout << "Usage\n" << argv[0] << " <input file> <output file>\n";
    std::cout << "e.g.\n" << argv[0] << " http://www.hep.ucl.ac.uk/uhen/ara/monitor/root/run1841/event1841.root outfile.root\n";
    return 0;
  }

  TFile *fpOut = new TFile(argv[2], "RECREATE");
  TTree *evTimeTree = new TTree("eventTimeTree", "Tree of event times and times between consecutive events");
  UInt_t timeStampGray=0, lastTimeStampGray=0, eventNumber=0, ppsNumber=0, unixTimeUs=0;
  Int_t deltaTimeStampGray=0, dtsgPos=0;
  UInt_t correctedTimeStampGray=0, correctedTimeStampGrayFudge=0;
  evTimeTree->Branch("timeStampGray", &timeStampGray, "timeStampGray/i");
  evTimeTree->Branch("dtsgPos", &dtsgPos, "dtsgPos/i");
  evTimeTree->Branch("correctedTimeStampGray", &correctedTimeStampGray, "correctedTimeStampGray/i");
  evTimeTree->Branch("correctedTimeStampGrayFudge", &correctedTimeStampGrayFudge, "correctedTimeStampGrayFudge/i");
  evTimeTree->Branch("deltaTimeStampGray", &deltaTimeStampGray, "deltaTimeStampGray/I");
  evTimeTree->Branch("eventNumber", &eventNumber, "eventNumber/i");
  evTimeTree->Branch("ppsNumber", &ppsNumber, "ppsNumber/i");
  evTimeTree->Branch("unixTimeUs", &unixTimeUs, "unixTimeUs/i");


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


   RawAtriStationEvent *rawEvPtr=0;
   eventTree->SetBranchAddress("event", &rawEvPtr);

   Int_t numEntries=eventTree->GetEntries();
   //   numEntries=100; //FIXME

   Int_t starEvery=numEntries/80;
   if(starEvery==0) starEvery++;

   for(Int_t event=0;event<numEntries;event++) {
     if(event%starEvery==0) {
       std::cerr << "*";       
     }
     
     eventTree->GetEntry(event);
     timeStampGray=rawEvPtr->timeStamp;
     eventNumber=rawEvPtr->eventNumber;
     ppsNumber=rawEvPtr->ppsNumber;
     unixTimeUs=rawEvPtr->unixTimeUs;
     
     deltaTimeStampGray=timeStampGray-lastTimeStampGray;
     if(deltaTimeStampGray<0){
       deltaTimeStampGray+=100000000;
       dtsgPos=0;
     }
     else dtsgPos=1;
     correctedTimeStampGray = getCorrectedTime(timeStampGray, ppsNumber, false);
     correctedTimeStampGrayFudge = getCorrectedTime(timeStampGray, ppsNumber, true);
     
     lastTimeStampGray=timeStampGray;
     evTimeTree->Fill();

     
   }//event
   std::cerr << std::endl;       

   fpOut->Write();   

   return 0;
}


UInt_t getCorrectedTime(UInt_t rawTime, UInt_t ppsNumber, bool fudgeOn){
  Long64_t fudge=0;
  if(fudgeOn) fudge=1.68e6;

  Long64_t correctedTime=0;
  correctedTime = (Long64_t)rawTime - ((Long64_t)ppsNumber-14456.6)/(-5.95664e-7);
  
  while(correctedTime<(fudge)) {
    correctedTime+=(1e8-fudge);//FUDGE FACTOR
  }
  while(correctedTime>=(1e8-fudge)) {
    correctedTime-=(1e8-fudge);//FUDGE FACTOR
  }

  return (UInt_t)correctedTime;

}
