#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAtriStationEvent.h"
#include "RawAtriStationEvent.h"
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


int plotSampleAdc(char *fileName,int numEvents, int dda, int chan);


int plotSampleAdc(char *fileName,int numEvents, int dda,int chan)
{
  //  TFile *fp = new TFile("/Users/rjn/ara/data/ohio2011/root/run386/event386.root");
  TFile *fp = new TFile(fileName);
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
  
  if(numEntries<numEvents) numEvents=numEntries;

  TH1F *histAdc[128];
  char histName[180];
  for(int samp=0;samp<128;samp++) {
    sprintf(histName,"histAdc%d",samp);
    histAdc[samp] = new TH1F(histName,histName,1000,-500,500);
  }

  for(int event=10;event<numEvents;event++) {
    
    eventTree->GetEntry(event);
    UsefulAtriStationEvent realEvent(evPtr,AraCalType::kVoltageTime);
    
    TGraph *gr1 =realEvent.getGraphFromElecChan(chan+8*dda);
    //    std::cout << event << "\t" << gr1->GetN() << "\n";
    Double_t *volts =gr1->GetY();
    if(gr1->GetN()<128) {
      delete gr1;
      continue;
    }
    
    for(int samp=0;samp<128;samp++) {
      //      cout << samp << "\t" << volts[samp] << "\n";
      histAdc[samp]->Fill(volts[samp]);
    }    
    delete gr1;
  }

  Double_t mean[128];
  Double_t rms[128];
  Double_t samp[128];
  for(int i=0;i<128;i++)
    samp[i]=i;
  Int_t sample=0;
  for(int canvas=0;canvas<8;canvas++) {
    TCanvas *can = new TCanvas();
    can->Divide(4,4);
    for(int pad=0;pad<16;pad++) {
      can->cd(pad+1);
      histAdc[sample]->Draw();
      mean[sample]=histAdc[sample]->GetMean();
      rms[sample]=histAdc[sample]->GetRMS();
      sample++;
    }
  }

  TCanvas *can = new TCanvas();
  can->Divide(1,2);
  can->cd(1);
  TGraph *gr = new TGraph(128,samp,mean);
  gr->SetMarkerStyle(22);
  gr->SetTitle("");
  gr->Draw("alp");
  gr->GetXaxis()->SetTitle("Sample #");
  gr->GetYaxis()->SetTitle("Mean (ADC)");
  can->cd(2);
  TGraph *gr2 = new TGraph(128,samp,rms);
  gr2->SetMarkerStyle(22);
  gr2->SetTitle("");
  gr2->Draw("alp");
  gr2->GetXaxis()->SetTitle("Sample #");
  gr2->GetYaxis()->SetTitle("RMS (ADC)");

}
