//makePedTree -- Run through a pedestal run and put the ADC values in a tree

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

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
  Int_t blockId=0,channel=0,dda=0,sample=0, eventBlockNum=0, entry=0;
  Double_t adc=0;
  TH2D *hSampleLine[4][6]={{0}};
  char histName[100];
  char xAxisName[100];
  char yAxisName[100];
  for(dda=0;dda<no_ddas;dda++){
    for(int i=0;i<6;i++){
      sprintf(histName, "hSampleLine_bit%i_dda%i", i, dda);
      sprintf(xAxisName, "ADC Sample N");
      sprintf(yAxisName, "ADC Sample N+%i", 1<<i);
      hSampleLine[dda][i] = new TH2D(histName, histName, 4096, 0, 4096, 4096, 0, 4096);
      hSampleLine[dda][i]->GetXaxis()->SetTitle(xAxisName);
      hSampleLine[dda][i]->GetYaxis()->SetTitle(yAxisName);
      //      hSampleLine[dda][i]->SetOptStat(0);
    }
  }


  //Insert some large run and run start protection
  Int_t numEntries = eventTree->GetEntries();
  if(numEntries>1000) numEntries=1000;
  Int_t starEvery = numEntries/80;
  if(!starEvery) starEvery = 1;

  for(entry=100;entry<numEntries;entry++){
    if((entry%starEvery)==0) fprintf(stderr, "*");
    eventTree->GetEntry(entry);

    realEvPtr = new UsefulAtriStationEvent(evPtr, AraCalType::kNoCalib);
    for(dda=0;dda<no_ddas;dda++){
      for(channel=0;channel<1;channel++){
	TGraph *grChannel = realEvPtr->getGraphFromElecChan(channel+dda*RFCHAN_PER_DDA);
	Int_t numBlocks = grChannel->GetN() / SAMPLES_PER_BLOCK;
	for(eventBlockNum=0;eventBlockNum<numBlocks;eventBlockNum++){
	  blockId = evPtr->blockVec[no_ddas*eventBlockNum+dda].getBlock();
	  Int_t dda_test = evPtr->blockVec[no_ddas*eventBlockNum+dda].getDda();
	  if(dda!=dda_test) fprintf(stderr, "wrong dda\n");

	  TGraph *grBlock = getBlockGraph(grChannel, eventBlockNum);
	  Double_t *yVals = grBlock->GetY();
	  uint32_t sample=0;
	  for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	    if(sample+1<SAMPLES_PER_BLOCK) hSampleLine[dda][0]->Fill(yVals[sample], yVals[sample+1], 1);
	    if(sample+2<SAMPLES_PER_BLOCK) hSampleLine[dda][1]->Fill(yVals[sample], yVals[sample+2], 1);
	    if(sample+4<SAMPLES_PER_BLOCK) hSampleLine[dda][2]->Fill(yVals[sample], yVals[sample+4], 1);
	    if(sample+8<SAMPLES_PER_BLOCK) hSampleLine[dda][3]->Fill(yVals[sample], yVals[sample+8], 1);
	    if(sample+16<SAMPLES_PER_BLOCK) hSampleLine[dda][4]->Fill(yVals[sample], yVals[sample+16], 1);
	    if(sample+32<SAMPLES_PER_BLOCK) hSampleLine[dda][5]->Fill(yVals[sample], yVals[sample+32], 1);
	  }
	  

	  if(grBlock) delete grBlock;
	}//block
	if(grChannel) delete grChannel;
      }//channel
    }
    
    delete realEvPtr;
    
  }//entry
  fprintf(stderr, "\n");
  // for(dda=0;dda<no_ddas;dda++){
  //   for(int i=0;i<6;i++){
  //      hSampleLine[dda][i]->Write();
  //   }
  // }

  //Make some canvases
  char canName[100];
  char pdfName[200];
  char pngName[200];
  TCanvas *can[6]={0};
  for(int i=0;i<6;i++){
    sprintf(canName, "canBit%i", i);
    can[i]=new TCanvas(canName, canName);
    can[i]->Divide(2,2);
    can[i]->cd(1);
    hSampleLine[0][i]->Draw("COLZ");
    can[i]->cd(2);
    hSampleLine[1][i]->Draw("COLZ");
    can[i]->cd(3);
    hSampleLine[2][i]->Draw("COLZ");
    can[i]->cd(4);
    hSampleLine[3][i]->Draw("COLZ");
    sprintf(pdfName, "%s.bit%i.pdf", outFileName, i);
    sprintf(pngName, "%s.bit%i.png", outFileName, i);
    can[i]->SaveAs(pdfName);
    can[i]->SaveAs(pngName);
    //    can[i]->Write();

  }



  outFile->Write();
  outFile->Close();
  delete outFile;
  
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

