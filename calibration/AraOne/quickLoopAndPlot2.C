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


int quickLoopAndPlot2(int event, int dda, int chan);


int quickLoopAndPlot2(int event, int dda,int chan)
{
  TFile *fp = new TFile("/Users/rjn/ara/data/ohio2011/root/run367/event367.root");
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

  TGraph *gr1 =realEvent.getGraphFromElecChan(chan+8*dda);

  Double_t *volts =gr1->GetY();
  Double_t *residual = new Double_t [gr1->GetN()];
  for(int i=0;i<gr1->GetN();i++) {
    residual[i]=volts[i]-volts[i%64];
  }

  TGraph *grRes = new TGraph(gr1->GetN(),gr1->GetX(),residual);
  grRes->Draw("alp");

  

}
