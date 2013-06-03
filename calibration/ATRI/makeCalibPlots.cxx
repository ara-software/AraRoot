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

#define USEFUL_RFCHANS 6

int makeCalibPlots(char *calibFileName){

  
  TFile *inFile = TFile::Open(calibFileName);
  if(!inFile){
    printf("Failed to open file %s\n", calibFileName);
    return -1;
  }
  TCanvas *canBinWidths[DDA_PER_ATRI];
  for(int dda=0;dda<DDA_PER_ATRI;dda++){
    
    char canName[100];
    sprintf(canName, "canBinWidths_DDA%i", dda);
    canBinWidths[dda] = new TCanvas(canName);
    canBinWidths[dda]->Divide(3,2);
    for(int chan=0;chan<USEFUL_RFCHANS;chan++){
      for(int half=0;half<2;half++){
	char histName[100];
	sprintf(histName, "histBinWidth_dda%i_chan%i_%i", dda, chan, half);
	TH1* tempHist = (TH1*) inFile->Get(histName);
	canBinWidths[dda]->cd(chan+1);
	if(half){
	  tempHist->SetLineColor(kRed);
	  tempHist->Draw("SAME");
	}
	else{
	  tempHist->SetLineColor(kBlue);
	  tempHist->Draw();
	}


      }
    }
  }
  TCanvas *canInterleave[DDA_PER_ATRI];
  for(int dda=0;dda<DDA_PER_ATRI;dda++){
    char canName[100];
    sprintf(canName, "canInterleave_DDA%i", dda);
    canInterleave[dda] = new TCanvas(canName);
    canInterleave[dda]->Divide(3,2);
    for(int chan=0;chan<USEFUL_RFCHANS;chan++){
      char histName[100];
      sprintf(histName, "lag_hist_dda%i_chan%i", dda, chan);
      TH1* tempHist = (TH1*) inFile->Get(histName);
      canInterleave[dda]->cd(chan+1);
      tempHist->Draw();
    }
  }
  TCanvas *canEpsilon[DDA_PER_ATRI];
    for(int dda=0;dda<DDA_PER_ATRI;dda++){
      char canName[100];
      sprintf(canName, "canEpsilon_DDA%i", dda);
      canEpsilon[dda] = new TCanvas(canName);
      canEpsilon[dda]->Divide(3,2);
      for(int chan=0;chan<USEFUL_RFCHANS;chan++){
	canEpsilon[dda]->cd(chan+1);
	char histName[100];
	sprintf(histName, "epsilon_hist_dda%i_chan%i", dda, chan);
	TH1* tempHist = (TH1*) inFile->Get(histName);
	tempHist->SetLineColor(kBlack);
	tempHist->Draw("");
      }
    }
    

  return 0;

}
