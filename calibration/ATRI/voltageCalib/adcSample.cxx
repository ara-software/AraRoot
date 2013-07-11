////////////////////////////////////////////////////////////////////////
////                                                                ////
////////////////////////////////////////////////////////////////////////


//AraRoot Includes
#include "UsefulAtriStationEvent.h"
#include "RawAtriStationEvent.h"
#include "araSoft.h"


//Root Includes
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TMath.h"
#include "TCanvas.h"


//Standard Includes
#include <iostream>
#include <fstream>


#define MAX_ADC 4096

//Prototype Functions
Int_t adcSample(char*, Int_t, Int_t, Double_t);


int main(int argc, char **argv)
{
  Int_t runNum=0, pedNum=0, dda=0, chan=0;
  char baseName[FILENAME_MAX];

  if(argc<5) {
    std::cerr << "Usage: " << argv[0] << " <baseDir> <runNum> <pedNum> <voltage> \n";
    return 1;
  }
  strncpy(baseName, argv[1],FILENAME_MAX);
  runNum=atoi(argv[2]);
  pedNum=atoi(argv[3]);
  Double_t voltage = atof(argv[4]);
  return adcSample(baseName, runNum, pedNum, voltage);
  //return test(baseName, runNum, pedNum, voltage);

}

Int_t adcSample(char* baseDirName, Int_t runNum, Int_t pedNum, Double_t voltage){
  printf("%s\n", baseDirName);
  printf("%i %i %f\n", runNum, pedNum, voltage);


  char runFileName[FILENAME_MAX];
  char pedFileName[FILENAME_MAX];
  sprintf(runFileName, "%s/root/run%i/event%i.root", baseDirName, runNum, runNum);
  sprintf(pedFileName, "%s/raw_data/run_%06i/pedestalValues.run%06i.dat", baseDirName, pedNum, pedNum);

  fprintf(stderr, "%s\n", runFileName);
  fprintf(stderr, "%s\n", pedFileName);
  
  TFile *fp = new TFile(runFileName);
  if(!fp) {
    std::cerr << "Can't open file\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fp->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't find eventTree\n";
    return -1;
  }
  RawAtriStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);
  Long64_t numEntries=eventTree->GetEntries();

  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  
  Int_t stationId=0;
  eventTree->GetEntry(0);
  stationId= evPtr->stationId;
  AraEventCalibrator *calib = AraEventCalibrator::Instance();
  calib->setAtriPedFile(pedFileName, stationId);
  
  //General output stuff
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/run%i/adcSample.root", baseDirName, runNum);
  TFile *outFile = new TFile(outFileName, "RECREATE");

   std::cout << "Output file " << outFileName << std::endl;

  //Variables what we need
  Int_t chanIndex;
  Int_t capArray;
  Int_t sample;
  Int_t thisCapArray=0;
  Int_t dda=0, chan=0;
  Int_t hasVoltCalib=0;
  Double_t ADC=0;

  TTree *voltageTree = new TTree("voltageTree", "Tree of voltages from run");
  voltageTree->Branch("dda", &dda, "dda/I");
  voltageTree->Branch("chan", &chan, "chan/I");
  voltageTree->Branch("sample", &sample, "sample/I");
  voltageTree->Branch("thisCapArray", &thisCapArray, "thisCapArray/I");
  voltageTree->Branch("ADC", &ADC, "ADC/D");
  voltageTree->Branch("hasVoltCalib", &hasVoltCalib, "hasVoltCalib/I");


  UsefulAtriStationEvent *realEvent=0;
  UsefulAtriStationEvent *realEventNoVoltCalib=0;


  // //  numEntries=1;//FIXME
  for(int entry=10;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);

    realEvent = new UsefulAtriStationEvent(evPtr, AraCalType::kVoltageTime);
    realEventNoVoltCalib = new UsefulAtriStationEvent(evPtr, AraCalType::kJustPed);

    capArray = evPtr->blockVec[0].getCapArray(); //capArray of first block
    
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  	chanIndex=chan+RFCHAN_PER_DDA*dda;


	hasVoltCalib=1;
  	TGraph *gr = realEvent->getGraphFromElecChan(chanIndex);
  	Int_t numSamples = gr->GetN();
  	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	Double_t *yVals=gr->GetY();

  	for(int block=0; block<numBlocks; block++){ 
  	  if(block%2) thisCapArray=1-capArray;
  	  else thisCapArray=capArray;
	  
	  
  	  for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	    ADC = yVals[sample];
	    
	    voltageTree->Fill();
	    
  	  }
  	}//block
  	delete gr;

	hasVoltCalib=0;
  	gr = realEventNoVoltCalib->getGraphFromElecChan(chanIndex);
	numSamples = gr->GetN();
	numBlocks = numSamples/SAMPLES_PER_BLOCK;
	yVals=gr->GetY();

  	for(int block=0; block<numBlocks; block++){ 
  	  if(block%2) thisCapArray=1-capArray;
  	  else thisCapArray=capArray;
	  
	  
  	  for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	    ADC = yVals[sample];
	    
	    voltageTree->Fill();
	    
  	  }
  	}//block
	
  	delete gr;
      }//chan
    }//dda
    delete realEvent;
    delete realEventNoVoltCalib;


  }//entry
  std::cerr << "\n";


  outFile->cd();
  voltageTree->Write();

  outFile->Write();
 

  return 0;
}


