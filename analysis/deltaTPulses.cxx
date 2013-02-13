////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////  deltaTPulses.cxx 
////      simple code to cross-correlate two channels to find the correlation 
////      time
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

int main(int argc, char **argv)
{

  if(argc<8) {
    std::cout << "Usage\n" << argv[0] << " <input file> <ped file> <output file> <channel 1> <channel 2> <trigTime> <useTrigTime>\n";
    return 0;
  }
  Int_t chan1 = atoi(argv[4]);
  Int_t chan2 = atoi(argv[5]);
  UInt_t trigTime = atoi(argv[6]);
  Int_t useTrigTime = atoi(argv[7]);

  printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
  printf("\t\t%s\n", argv[0]);
  printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

  printf("input file %s\n", argv[1]);
  printf("ped file %s\n", argv[2]);
  printf("out file %s\n", argv[3]);
  printf("channel1 %d channel2 %d trigTime %d useTrigTime\n", chan1, chan2, trigTime, useTrigTime);
  
  

  TFile *fpOut = new TFile(argv[3], "RECREATE");

  TTree *outTree = new TTree("tDeltaT", "tree of cross correlations");
  Double_t deltaT=0;
  UInt_t thisTimeStamp=0, pps=0;
  

  outTree->Branch("deltaT", &deltaT, "deltaT/D");
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

   TGraph *grChan1;
   TGraph *grChan2;
   TGraph *grChan1Int;
   TGraph *grChan2Int;
   TGraph * grCorr;

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
     //     printf("timeStamp %d timeStamp*10 - trigTime %d countinue ? %i\n", thisTimeStamp, thisTimeStamp*10-trigTime, TMath::Abs(Int_t(thisTimeStamp*10-trigTime))>1000);

     //     if((TMath::Abs(Int_t(thisTimeStamp*10-trigTime))>10000)&&useTrigTime) continue;
          
     realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kFirstCalib);

     grChan1 = realAtriEvPtr->getGraphFromElecChan(chan1);
     grChan2 = realAtriEvPtr->getGraphFromElecChan(chan2);

     grChan1Int=FFTtools::getInterpolatedGraph(grChan1,0.5);

     grChan2Int=FFTtools::getInterpolatedGraph(grChan2,0.5);
     
     grCorr = FFTtools::getInterpolatedCorrelationGraph(grChan1Int,grChan2Int,0.5);
     //     grCorr=FFTtools::getCorrelationGraph(grChan1Int,grChan2Int);      


     deltaT = getMaxX(grCorr);
     

     outTree->Fill();
     

     delete grChan1Int;
     delete grChan2Int;
     delete grCorr;
     delete grChan1;
     delete grChan2;

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
