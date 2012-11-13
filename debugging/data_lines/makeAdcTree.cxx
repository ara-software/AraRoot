//makePedTree -- Run through a pedestal run and put the ADC values in a tree

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"

#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"

#include <iostream>

TGraph *getBlockGraph(TGraph*, Int_t);

int main(int argc, char **argv){

  if(argc<4){

    printf("Usage : %s <runFile> <outPutFile> <is mATRI>\n", argv[0]);
    return -1;

  }

  char runFileName[FILENAME_MAX];
  sprintf(runFileName, "%s", argv[1]);
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s", argv[2]);
  Int_t is_mATRI=atoi(argv[3]);
  Int_t no_ddas=4;
  if(is_mATRI) no_ddas=1;
  printf("inFileName %s\noutFileName %s\n", runFileName, outFileName);
  
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
  UsefulAtriStationEvent *realEvPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);

  TFile *outFile = new TFile(outFileName, "RECREATE");
  TTree *outTree = new TTree("adcTree", "adcTree");
  Int_t blockId=0,channel=0,dda=0,sample=0, eventBlockNum=0, entry=0;
  Double_t adc=0;
  outTree->Branch("blockId", &blockId, "blockId/I");
  outTree->Branch("channel", &channel, "channel/I");
  outTree->Branch("dda", &dda, "dda/I");
  outTree->Branch("sample", &sample, "sample/I");
  outTree->Branch("entry", &entry, "entry/I");
  outTree->Branch("adc", &adc, "adc/D");
  outTree->Branch("eventBlockNum", &eventBlockNum, "eventBlockNum/I");

  Int_t numEntries = eventTree->GetEntries();
  if(numEntries>2000) numEntries=2000;
  Int_t starEvery = numEntries/80;

  if(!starEvery) starEvery = 1;

  for(entry=100;entry<numEntries;entry++){
    if((entry%starEvery)==0) fprintf(stderr, "*");
    eventTree->GetEntry(entry);

    realEvPtr = new UsefulAtriStationEvent(evPtr, AraCalType::kNoCalib);
    for(dda=0;dda<no_ddas;dda++){
      for(channel=0;channel<RFCHAN_PER_DDA;channel++){
	TGraph *grChannel = realEvPtr->getGraphFromElecChan(channel+dda*RFCHAN_PER_DDA);
	Int_t numBlocks = grChannel->GetN() / SAMPLES_PER_BLOCK;
	for(eventBlockNum=0;eventBlockNum<numBlocks;eventBlockNum++){
	  blockId = evPtr->blockVec[no_ddas*eventBlockNum+dda].getBlock();
	  Int_t dda_test = evPtr->blockVec[no_ddas*eventBlockNum+dda].getDda();

	  if(dda!=dda_test) fprintf(stderr, "wrong dda\n");

	  TGraph *grBlock = getBlockGraph(grChannel, eventBlockNum);
	  Double_t *yVals = grBlock->GetY();
	  for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	    adc = yVals[sample];
	    outTree->Fill();
	  }//sample
	  if(grBlock) delete grBlock;
	}//block
	if(grChannel) delete grChannel;
      }//channel
    }
 
    delete realEvPtr;
    
  }//entry
  fprintf(stderr, "\n");



  outTree->Write();
  outFile->Write();
  outFile->Close();
  delete outFile;
  
  TFile *inFile=new TFile(outFileName, "UPDATE");
  TTree *adcTree = (TTree*) gDirectory->Get("adcTree");
  TH1* hists[4]={0};
  for(dda=0;dda<4;dda++){
    char exp[100];
    sprintf(exp, "adc:sample>>tempHist");
    char cut[100];
    sprintf(cut, "dda==%i&&channel==1", dda);
    adcTree->Draw(exp, cut, "COLZ");
    hists[dda] = (TH1*) gDirectory->Get("tempHist");
    char histName[100];
    sprintf(histName, "adc_vs_sample_dda_%i", dda);
    hists[dda]->SetName(histName);
    hists[dda]->SetTitle(histName);
    char xAxis[100];
    sprintf(xAxis, "Sample #");
    hists[dda]->GetXaxis()->SetTitle(xAxis);
    char yAxis[100];
    sprintf(yAxis, "ADC");
    hists[dda]->GetYaxis()->SetTitle(yAxis);
    
    hists[dda]->Write();
  }
  TH1 *hStuckBit[4][11];
  TCanvas *canStuckBit[4];
  char canName[100];
  for(int dda=0;dda<4;dda++){
    sprintf(canName, "dda_%i", dda);
    canStuckBit[dda] = new TCanvas(canName, canName);      
    canStuckBit[dda]->Divide(3,4);
    for(int bit=0;bit<11;bit++){
      canStuckBit[dda]->cd(bit+1);
      char histName[100];
      sprintf(histName, "dda_%i_bits_%i_&_%i", dda, bit, bit+1);      
      char histExp[110];
      sprintf(histExp, "(adc&%i)/%i:(adc&%i)/%i>>tempHist(2,-0.5,1.5,2,-0.5,1.5)", 1<<bit, 1<<bit, 1<<(bit+1), 1<<(bit+1));
      char histCut[110];
      sprintf(histCut, "channel==1&&dda==%i", dda);
      char histYaxisTitle[100];
      sprintf(histYaxisTitle, "bit %i", bit);
      char histXaxisTitle[100];
      sprintf(histXaxisTitle, "bit %i", bit+1);
      adcTree->Draw(histExp, histCut, "COLZTEXT");
      
      hStuckBit[dda][bit] = (TH1*) gDirectory->Get("tempHist");
      hStuckBit[dda][bit]->SetName(histName);      
      hStuckBit[dda][bit]->SetTitle(histName);      
      hStuckBit[dda][bit]->SetMarkerColor(kBlack);
      hStuckBit[dda][bit]->SetMarkerSize(4);
      hStuckBit[dda][bit]->GetYaxis()->SetTitle(histYaxisTitle);
      hStuckBit[dda][bit]->GetXaxis()->SetTitle(histXaxisTitle);
      hStuckBit[dda][bit]->GetYaxis()->CenterTitle();
      hStuckBit[dda][bit]->GetXaxis()->CenterTitle();
      Double_t maxZ = hStuckBit[dda][bit]->GetEntries();
      hStuckBit[dda][bit]->GetZaxis()->SetRangeUser(0,maxZ);

      hStuckBit[dda][bit]->Draw("COLZTEXT");
      delete (TH1*) gDirectory->Get("tempHist");

    }//bit
    canStuckBit[dda]->Write();
  }//dda

  
  return 0;
}

TGraph *getBlockGraph(TGraph *fullEventGraph, Int_t block){
  Int_t numSamples = fullEventGraph->GetN();
  Int_t numBlocks = numSamples / SAMPLES_PER_BLOCK;
  if(block > numBlocks) return NULL;
  Double_t *fullX = fullEventGraph->GetX();
  Double_t *fullY = fullEventGraph->GetY();  
  Double_t *blockX = new Double_t[SAMPLES_PER_BLOCK];
  Double_t *blockY = new Double_t[SAMPLES_PER_BLOCK];
  for(int sample=0;sample<SAMPLES_PER_BLOCK; sample++){
    blockY[sample] = fullY[sample + block*SAMPLES_PER_BLOCK];
    blockX[sample] = fullX[sample + block*SAMPLES_PER_BLOCK];
  }
  TGraph *blockGraph = new TGraph(SAMPLES_PER_BLOCK, blockX, blockY);
  delete blockX;
  delete blockY;
  return blockGraph;
}

