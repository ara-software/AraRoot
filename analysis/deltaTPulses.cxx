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
#include "AraGeomTool.h"
#include "AraStationInfo.h"
#include "AraAntennaInfo.h"
#include "AraCalAntennaInfo.h"


#include "FFTtools.h"

//ROOT Includes
#include "TTree.h"
#include "TMath.h"
#include "TFile.h"
#include "TGraph.h"


RawAtriStationEvent *rawAtriEvPtr;
UsefulAtriStationEvent *realAtriEvPtr;
Double_t interSample=0.1;
Double_t getMaxX(TGraph *gr);

int main(int argc, char **argv)
{

  if(argc<3) {
    std::cout << "Usage\n" << argv[0] << " <input file>  <output file>\n";
    return 0;
  }

  printf("------------------------------------------------------------------------------------------------------------------------------------------\n");
  printf("\t%s\n", argv[0]);
  printf("------------------------------------------------------------------------------------------------------------------------------------------\n");

  printf("input file %s\n", argv[1]);
  printf("out file %s\n", argv[2]);

  TFile *fpOut = new TFile(argv[2], "RECREATE");

  TTree *outTree = new TTree("tDeltaT", "tree of cross correlations");
  Double_t deltaT=0;
  UInt_t thisTimeStamp=0, pps=0;
  Int_t chan1=0, chan2=0;
  AraAntennaInfo *antInfo1=0, *antInfo2=0;

  outTree->Branch("deltaT", &deltaT, "deltaT/D");
  outTree->Branch("timeStamp", &thisTimeStamp, "thisTimeStamp/I");
  outTree->Branch("pps", &pps, "pps/I");
  outTree->Branch("chan1", &chan1, "chan1/I");
  outTree->Branch("chan2", &chan2, "chan2/I");
  outTree->Branch("antInfo1","AraAntennaInfo",&antInfo1);
  outTree->Branch("antInfo2","AraAntennaInfo",&antInfo2);


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
   AraStationInfo *tempStationInfo = new AraStationInfo(stationId);
   
   for(Long64_t event=0;event<numEntries;event++) {
     if(event%starEvery==0) {
       std::cerr << "*";       
     }

     //This line gets the RawIcrr or RawAtri Event
     eventTree->GetEntry(event);

     thisTimeStamp = rawAtriEvPtr->timeStamp;
     pps = rawAtriEvPtr->ppsNumber;

     if(!(rawAtriEvPtr->isCalpulserEvent())) continue;

     realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kFirstCalibPlusCables);
     
     for(chan1=0;chan1<ANTS_PER_ATRI;chan1++){
       antInfo1 = tempStationInfo->getAntennaInfo(chan1);

       for(chan2=chan1+1;chan2<ANTS_PER_ATRI;chan2++){
	 
	 antInfo2 = tempStationInfo->getAntennaInfo(chan2);

	 grChan1 = realAtriEvPtr->getGraphFromRFChan(chan1);
	 grChan2 = realAtriEvPtr->getGraphFromRFChan(chan2);

	 grChan1Int=FFTtools::getInterpolatedGraph(grChan1,interSample);
	 
	 grChan2Int=FFTtools::getInterpolatedGraph(grChan2,interSample);
	 
	 grCorr = FFTtools::getInterpolatedCorrelationGraph(grChan1Int,grChan2Int,interSample);
	 //     grCorr=FFTtools::getCorrelationGraph(grChan1Int,grChan2Int);      
	 
	 
	 deltaT = getMaxX(grCorr);
	 
	 outTree->Fill();
     
	 delete grChan1Int;
	 delete grChan2Int;
	 delete grCorr;
	 delete grChan1;
	 delete grChan2;
       }
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
