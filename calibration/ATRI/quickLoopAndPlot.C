#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraOneStationEvent.h"
#include "RawAraOneStationEvent.h"
#include "araSoft.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include "TEventList.h"
#include "TMath.h"
#include "TCanvas.h"
#include <TGClient.h>


int quickLoopAndPlot(int event, int dda, int chan0, int chan1);


int quickLoopAndPlot(int event, int dda,int chan0, int chan1)
{
  TFile *fp = new TFile("/Users/rjn/ara/data/ohio2011/root/run366/event366.root");
  if(!fp) {
    std::cerr << "Can't open file\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fp->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't find eventTree\n";
    return -1;
  }
  RawAraOneStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);

  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  
   

  eventTree->GetEntry(event);
  UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kNoCalib);

  TGraph *gr1 =realEvent.getGraphFromElecChan(chan0+8*dda);
  TGraph *gr2 =realEvent.getGraphFromElecChan(chan1+8*dda);
  
  TCanvas *can2 = new TCanvas("can2","can2",800,800);
  can2->Divide(1,2);
  can2->cd(1);
  gr1->Draw("alp");
  gr1->GetXaxis()->SetTitle("Channel 0 Sample");
  gr1->GetYaxis()->SetTitle("Channel 0 (ADC)");
  can2->cd(2);
  gr2->GetXaxis()->SetTitle("Channel 1 Sample");
  gr2->GetYaxis()->SetTitle("Channel 1 (ADC)");
  gr2->Draw("alp");

  TCanvas *can = new TCanvas("can","can");
  TGraph *gr3= new TGraph(gr1->GetN(),gr1->GetY(),gr2->GetY());
  gr3->SetMarkerStyle(22);
  gr3->Draw("ap");
  gr3->GetXaxis()->SetTitle("Channel 0 (ADC)");
  gr3->GetYaxis()->SetTitle("Channel 1 (ADC)");

}
