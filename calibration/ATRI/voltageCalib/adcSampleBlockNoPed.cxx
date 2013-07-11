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

  std::cout << "Number of entries in file is " << numEntries << std::endl;

  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  
  Int_t stationId=0;
  eventTree->GetEntry(0);
  stationId= evPtr->stationId;
  AraEventCalibrator *calib = AraEventCalibrator::Instance();
  calib->setAtriPedFile(pedFileName, stationId);
  
  
  
  //General output stuff
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/run%i/adcSampleBlockNoPed.root", baseDirName, runNum);
  TFile *outFile = new TFile(outFileName, "RECREATE");

  //  std::cout << "Output file " << outFileName << std::endl;

  //Variables what we need
  Int_t chanIndex;
  Int_t capArray;
  Int_t block;
  Int_t logicalBlock;
  Int_t sample;
  Int_t thisCapArray;
  Int_t dda=0, chan=0;
  Double_t ****maxPos = new Double_t***[DDA_PER_ATRI];
  Double_t ****maxNeg = new Double_t***[DDA_PER_ATRI];

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    maxPos[dda] = new Double_t**[RFCHAN_PER_DDA];
    maxNeg[dda] = new Double_t**[RFCHAN_PER_DDA];
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      maxPos[dda][chan] = new Double_t*[SAMPLES_PER_BLOCK];
      maxNeg[dda][chan] = new Double_t*[SAMPLES_PER_BLOCK];
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	maxPos[dda][chan][sample] = new Double_t[BLOCKS_PER_DDA];
	maxNeg[dda][chan][sample] = new Double_t[BLOCKS_PER_DDA];
  	for(block=0;block<BLOCKS_PER_DDA;block++){
  	  maxPos[dda][chan][sample][block]=-4096;
  	  maxNeg[dda][chan][sample][block]=4096;
  	}
      }
    }
  }
  
  // //  numEntries=1;//FIXME
  for(int entry=10;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kNoCalib);
    capArray = evPtr->blockVec[0].getCapArray(); //capArray of first block
    
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  	chanIndex=chan+RFCHAN_PER_DDA*dda;
  	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
  	Int_t numSamples = gr->GetN();
  	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	Double_t *yVals=gr->GetY();

  	for(block=0; block<numBlocks; block++){ 
  	  if(block%2) thisCapArray=1-capArray;
  	  else thisCapArray=capArray;
  	  logicalBlock=evPtr->blockVec[block*DDA_PER_ATRI+dda].getBlock();
  	  for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
  	    if(yVals[sample+block*SAMPLES_PER_BLOCK]>maxPos[dda][chan][sample][logicalBlock]) maxPos[dda][chan][sample][logicalBlock]=yVals[sample+block*SAMPLES_PER_BLOCK];
  	    if(yVals[sample+block*SAMPLES_PER_BLOCK]<maxNeg[dda][chan][sample][logicalBlock]) maxNeg[dda][chan][sample][logicalBlock]=yVals[sample+block*SAMPLES_PER_BLOCK];
  	  }
  	}//block
	
  	delete gr;
      }//chan
    }//dda

  }//entry
  std::cerr << "\n";

  TTree *maxPosNegTree = new TTree("maxPosNegTree", "Tree of maximum positive and negative values");
  maxPosNegTree->Branch("dda", &dda, "dda/I");
  maxPosNegTree->Branch("chan", &chan, "chan/I");
  maxPosNegTree->Branch("sample", &sample, "sample/I");
  maxPosNegTree->Branch("block", &block, "block/I");
  Double_t maxNegValue=0, maxPosValue=0;
  maxPosNegTree->Branch("maxPosValue", &maxPosValue, "maxPosValue/D");
  maxPosNegTree->Branch("maxNegValue", &maxNegValue, "maxNegValue/D");
  maxPosNegTree->Branch("voltage", &voltage, "voltage/D");
  
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
  	for(block=0;block<BLOCKS_PER_DDA;block++){
  	  maxPosValue=maxPos[dda][chan][sample][block];
  	  maxNegValue=maxNeg[dda][chan][sample][block];
  	  maxPosNegTree->Fill();
  	}
      }
    }
  }

  maxPosNegTree->Write();

  outFile->Write();

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	delete [] maxPos[dda][chan][sample];
	delete [] maxNeg[dda][chan][sample];
      }
      delete [] maxPos[dda][chan];
      delete [] maxNeg[dda][chan];
    }
    delete [] maxPos[dda];
    delete [] maxNeg[dda];
 }
 

  return 0;
}


