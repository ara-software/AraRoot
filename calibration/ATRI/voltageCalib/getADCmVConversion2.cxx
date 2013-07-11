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
#include "TF1.h"
#include "TTree.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TGraph.h"
#include "TMultiGraph.h"


//Standard Includes
#include <iostream>
#include <fstream>


#define MAX_ADC 4096

//Prototype Functions
Int_t getADCmVConversion(char *baseName, Int_t runLow, Int_t runHigh, Int_t thisBlock);
TGraph *getZeroSubGraph(TGraph *gr, Int_t factor);
TGraph *addGraphs(TGraph *grOne, TGraph *grTwo);


int main(int argc, char **argv)
{
  Int_t runLow, runHigh, block;
  char baseName[FILENAME_MAX];

  if(argc<5){
    std::cerr << "Usage: " << argv[0] << " <baseDir> <runLow> <runHigh> <block>  \n";
    return 1;
  }
  strncpy(baseName, argv[1],FILENAME_MAX);
  runLow=atoi(argv[2]);
  runHigh=atoi(argv[3]);
  block=atoi(argv[4]);
  getADCmVConversion(baseName, runLow, runHigh, block);

}






Int_t getADCmVConversion(char *baseName, Int_t runLow, Int_t runHigh, Int_t thisBlock){

  TChain *chain = new TChain("maxPosNegTree");
  char runName[FILENAME_MAX];
  for(int run=runLow; run<=runHigh;run++){
    sprintf(runName, "%s/root/run%i/adcSampleBlockNoPed.root", baseName, run);
    chain->Add(runName);
  }
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/voltageCalib2/ADCmVConversion.root", baseName);
  
  Double_t *****theADC = new Double_t****[DDA_PER_ATRI];
  Double_t *****theVoltage = new Double_t****[DDA_PER_ATRI];
  Int_t ****index = new Int_t***[DDA_PER_ATRI];
  Double_t maxNegValue=0, maxPosValue=0, voltage=0;
  Int_t dda=0,chan=0,sample=0,block=0;
  Int_t numVoltages=60;

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    theADC[dda] = new Double_t***[RFCHAN_PER_DDA];
    theVoltage[dda] = new Double_t***[RFCHAN_PER_DDA];
    index[dda] = new Int_t**[RFCHAN_PER_DDA];
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      theADC[dda][chan] = new Double_t**[SAMPLES_PER_BLOCK];
      theVoltage[dda][chan] = new Double_t**[SAMPLES_PER_BLOCK];
      index[dda][chan] = new Int_t*[SAMPLES_PER_BLOCK];
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	theADC[dda][chan][sample] = new Double_t*[BLOCKS_PER_DDA];
	theVoltage[dda][chan][sample] = new Double_t*[BLOCKS_PER_DDA];
	index[dda][chan][sample] = new Int_t[BLOCKS_PER_DDA];
  	for(block=0;block<BLOCKS_PER_DDA;block++){
  	  theADC[dda][chan][sample][block]= new Double_t[numVoltages];
  	  theVoltage[dda][chan][sample][block]=new Double_t[numVoltages];
	  index[dda][chan][sample][block]=0;
  	}
      }
    }
  }
  

  //The stuff we want from the tree

  chain->SetBranchAddress("dda", &dda);
  chain->SetBranchAddress("chan", &chan);
  chain->SetBranchAddress("sample", &sample);
  chain->SetBranchAddress("block", &block);

  chain->SetBranchAddress("maxPosValue", &maxPosValue);
  chain->SetBranchAddress("maxNegValue", &maxNegValue);
  chain->SetBranchAddress("voltage", &voltage);
  
  Int_t numEntries = chain->GetEntries();
  Int_t starEvery = numEntries/80;
   for(int entry=0;entry<numEntries;entry++){
     if(entry%starEvery==0) std::cerr << "*";
     chain->GetEntry(entry);
     theADC[dda][chan][sample][block][index[dda][chan][sample][block]]=maxPosValue;
     theVoltage[dda][chan][sample][block][index[dda][chan][sample][block]]=voltage/2.;
     index[dda][chan][sample][block]++;

     theADC[dda][chan][sample][block][index[dda][chan][sample][block]]=maxNegValue;
     theVoltage[dda][chan][sample][block][index[dda][chan][sample][block]]=-1*voltage/2.;
     index[dda][chan][sample][block]++;

   }
   std::cerr << "\n";
  
   TFile *fpOut = new TFile(outFileName, "RECREATE");
   fpOut->cd();
   TGraph *****grADCmV = new TGraph****[DDA_PER_ATRI];
   char grName[FILENAME_MAX];
   char grTitle[FILENAME_MAX];
   Int_t entry=0;
   starEvery=(DDA_PER_ATRI*RFCHAN_PER_DDA*SAMPLES_PER_BLOCK*BLOCKS_PER_DDA)/80;
   for(dda=0;dda<DDA_PER_ATRI;dda++){
     grADCmV[dda] = new TGraph***[RFCHAN_PER_DDA];
     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
       grADCmV[dda][chan] = new TGraph**[SAMPLES_PER_BLOCK];
       for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	 grADCmV[dda][chan][sample] = new TGraph*[BLOCKS_PER_DDA];
	 for(block=0;block<BLOCKS_PER_DDA;block++){
	   entry++;
	   if(entry%starEvery==0) std::cerr << "*";
	   grADCmV[dda][chan][sample][block] = new TGraph(index[dda][chan][sample][block],
							  theADC[dda][chan][sample][block],
							  theVoltage[dda][chan][sample][block]);
	   sprintf(grName, "grADCVolt_%i_%i_%i_%i", dda, chan,sample,block);
	   sprintf(grTitle, "grADCVolt_%i_%i_%i_%i", dda, chan,sample,block);
		   
	   grADCmV[dda][chan][sample][block]->SetName(grName);
	   grADCmV[dda][chan][sample][block]->SetTitle(grTitle);
	   grADCmV[dda][chan][sample][block]->Write();
	   
	   delete [] theADC[dda][chan][sample][block];
	   delete [] theVoltage[dda][chan][sample][block];
	   delete grADCmV[dda][chan][sample][block];
	 }
       }
     }
   }
   std::cerr << "\n";

   fpOut->Write();

  fprintf(stderr, "Deleting!\n");
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	
	delete [] theADC[dda][chan][sample];
	delete [] index[dda][chan][sample];	
	delete [] grADCmV[dda][chan][sample];
	delete [] theVoltage[dda][chan][sample];
	
      }
      ;
      delete [] theADC[dda][chan];
      delete [] index[dda][chan];
      delete [] grADCmV[dda][chan];
      delete [] theVoltage[dda][chan];
    }
    ;
    delete [] theADC[dda];
    delete [] index[dda];
    delete [] grADCmV[dda];
    delete [] theVoltage[dda];
  }
  
  delete [] theADC;
  delete [] index;
  delete [] grADCmV;
  delete [] theVoltage;

  fprintf(stderr, "Done!\n");
  return 0;

}



TGraph *getZeroSubGraph(TGraph *gr, Int_t factor){
  Int_t numEntries = gr->GetN();
  Double_t *yVals=gr->GetY();
  Double_t *xVals=gr->GetX();
  
  if(numEntries<=0) return NULL;
  Double_t *xValsNew = new Double_t[numEntries];
  Double_t xValZero=0;

  for(int entry=0;entry<numEntries;entry++){
    if(yVals[entry]==0){
      xValZero=xVals[entry];
      break;
    }
  }
  for(int entry=0;entry<numEntries;entry++){
    Double_t newX = TMath::Sqrt( TMath::Power(xVals[entry],2) - TMath::Power(xValZero,2) );
    xValsNew[entry] = factor*newX;
  }

  TGraph *grNew = new TGraph(numEntries, xValsNew, yVals);


  delete [] xValsNew;

  return grNew;

}

TGraph *addGraphs(TGraph *grOne, TGraph *grTwo){
  Int_t numEntriesOne = grOne->GetN();
  Int_t numEntriesTwo = grTwo->GetN();
  Double_t *yVals1 = grOne->GetY();
  Double_t *xVals1 = grOne->GetX();
  Double_t *yVals2 = grTwo->GetY();
  Double_t *xVals2 = grTwo->GetX();

  Double_t *yValsNew = new Double_t[numEntriesOne+numEntriesTwo];
  Double_t *xValsNew = new Double_t[numEntriesOne+numEntriesTwo];

  for(int entry = 0;entry<numEntriesOne;entry++){
    xValsNew[entry] = xVals1[entry];
    yValsNew[entry] = yVals1[entry];
  }
  for(int entry = 0;entry<numEntriesTwo;entry++){
    xValsNew[entry+numEntriesOne] = xVals2[entry];
    yValsNew[entry+numEntriesOne] = yVals2[entry];
  }

  TGraph *grNew = new TGraph(numEntriesOne+numEntriesTwo, xValsNew, yValsNew);

  delete [] yValsNew;
  delete [] xValsNew;

  return grNew;
}

