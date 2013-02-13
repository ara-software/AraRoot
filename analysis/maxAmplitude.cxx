////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////  maxAmplitude.cxx 
////      find the maximum voltage in a waveform and put in a Tree
////
////    jdavies@hep.ucl.ac.uk
////////////////////////////////////////////////////////////////////////////////

//Includes
#include <iostream>

//AraRoot Includes
#include "RawAtriStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "UsefulAtriStationEvent.h"

//Include FFTtools.h if you want to ask the correlation, etc. tools

#include "FFTtools.h"

//ROOT Includes
#include "TTree.h"
#include "TMath.h"
#include "TFile.h"
#include "TGraph.h"


RawAtriStationEvent *rawAtriEvPtr;
UsefulAtriStationEvent *realAtriEvPtr;

Double_t getMaxX(TGraph *gr);
Double_t getMaxY(TGraph *gr);

int main(int argc, char **argv)
{

  if(argc<4) {
    std::cout << "Usage\n" << argv[0] << " <input file> <ped file> <output file>\n";
    return 0;
  }

  printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
  printf("\t\t%s\n", argv[0]);
  printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

  printf("input file %s\n", argv[1]);
  printf("ped file %s\n", argv[2]);
  printf("out file %s\n", argv[3]);
  
  

  TFile *fpOut = new TFile(argv[3], "RECREATE");

  TTree *outTree = new TTree("tMaxADC", "tree of maximum ADCs in a waveform");
  Double_t maxADC=0;
  UInt_t thisTimeStamp=0, pps=0;
  Int_t chan=0;

  outTree->Branch("maxADC", &maxADC, "maxADC/D");
  outTree->Branch("chan", &chan, "chan/I");
  outTree->Branch("timeStamp", &thisTimeStamp, "thisTimeStamp/I");
  outTree->Branch("pps", &pps, "pps/I");

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

   eventTree->SetBranchAddress("event", &rawAtriEvPtr);
   Long64_t numEntries=eventTree->GetEntries();
   Long64_t starEvery=numEntries/80;
   if(starEvery==0) starEvery++;

   TGraph *grChan;

   eventTree->GetEntry(0);
   
   Int_t stationId = rawAtriEvPtr->stationId;

   AraEventCalibrator::Instance()->setAtriPedFile(argv[2],stationId);


   for(Long64_t event=0;event<numEntries;event++) {
     if(event%starEvery==0) {
       std::cerr << "*";       
     }

     //This line gets the RawIcrr or RawAtri Event
     eventTree->GetEntry(event);


     thisTimeStamp = rawAtriEvPtr->timeStamp;
     pps = rawAtriEvPtr->ppsNumber;
    
     realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kFirstCalib);

     for(chan=0;chan<CHANNELS_PER_ATRI;chan++){
       grChan = realAtriEvPtr->getGraphFromElecChan(chan);
       maxADC = getMaxY(grChan);
       outTree->Fill();
       delete grChan;

     }

     delete realAtriEvPtr;
     
   }
   std::cerr << "\n";

   fpOut->Write();


}


Double_t getMaxX(TGraph *gr){
  Int_t size = gr->GetN();
  Double_t *xVals = gr->GetX();
  Double_t *yVals = gr->GetY();
  
  Double_t maxX=0;
  Double_t maxY=0;
  for(int n=0;n<size;n++){
    if(TMath::Abs(yVals[n])>TMath::Abs(maxY)){
      maxX=xVals[n];
      maxY=yVals[n];

    }
    

  }
  
  return maxX;
}


Double_t getMaxY(TGraph *gr){
  Int_t size = gr->GetN();
  Double_t *xVals = gr->GetX();
  Double_t *yVals = gr->GetY();
  
  Double_t maxX=0;
  Double_t maxY=0;
  for(int n=0;n<size;n++){
    if(TMath::Abs(yVals[n])>TMath::Abs(maxY)){
      maxX=xVals[n];
      maxY=yVals[n];

    }
    

  }
  
  return maxY;
}
