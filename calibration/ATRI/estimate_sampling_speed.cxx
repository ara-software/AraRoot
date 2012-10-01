//////////////////////////////////////////////////////////////////////////////////////////
////                                                                                  ////
////  estimate_sampling_speed jdavies@hep.ucl.ac.uk Jonathan Davies                   ////
////                                                                                  ////
////  estimates the sampling speed using an inputed sinewave frequency and channel    ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAtriStationEvent.h"
#include "RawAtriStationEvent.h"
#include "AraEventCalibrator.h"
#include "araSoft.h"

//ROOT Includes
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"

TGraph* zeroMean(TGraph*);
TGraph *getBlockGraph(TGraph*, Int_t);
TGraph* getHalfGraph(TGraph*, Int_t);

int estimate_sampling_speed(char *runFile, char *outFile, char* pedFile, Double_t freq, Int_t dda, Int_t chan, bool debug=false);

using namespace std;
int main(int argc, char **argv)
{
  if(argc<7) {
    std::cerr << "Usage: " << argv[0] << " <runFile> <outFile> <pedFile> <freq in GHz> <dda> <chan>\n";
    return 1;
  }
  return estimate_sampling_speed(argv[1], argv[2], argv[3], atof(argv[4]),atoi(argv[5]),atoi(argv[6]), atoi(argv[7]));
}

int estimate_sampling_speed(char *runFile, char *outFile, char *pedFile, Double_t frequency,Int_t dda, Int_t chan, bool debug)
{
  printf("RunFile %s Frequency %f dda %i channel %i\n", runFile, frequency, dda, chan);

  Int_t chanIndex=chan+RFCHAN_PER_DDA*dda;
  Double_t period = 1./frequency;
  Double_t average_bin_width=0;
  
  //1.Set up the input and output files
  char inName[180];
  sprintf(inName,"%s", runFile);

  TFile *fp = new TFile(inName);
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
  Int_t numEntries = eventTree->GetEntries();
  Int_t starEvery = numEntries / 80;

  //Now set the pedfile
  Int_t stationId=0;
  eventTree->GetEntry(0);
  stationId= evPtr->stationId;
  //  std::cerr << "stationId " << stationId << "\n";
  AraEventCalibrator *calib = AraEventCalibrator::Instance();
  calib->setAtriPedFile(pedFile, stationId);//FIXME
  
  char histName[180];
  sprintf(histName, "%s", outFile);
  TFile *histFile = new TFile(histName,"RECREATE");

  //2. Set up the trees and variables

  TTree *binWidthsTree = new TTree("binWidthsTree", "binWidthsTree");
  Int_t capArray=0, half=0, sample=0, noZCs[2]={0}, noSWs[2]={0};
  Int_t block, thisCapArray;
  Double_t binWidth=0;
  binWidthsTree->Branch("capArray", &capArray, "capArray/I");
  binWidthsTree->Branch("half", &half, "half/I");
  binWidthsTree->Branch("sample", &sample, "sample/I");
  binWidthsTree->Branch("binWidth", &binWidth, "binWidth/D");
  Int_t numEvents[2]={0}; //used to scale the bin widths
  Double_t zcCount[2][2][32]={{{0}}};

  TH1F *histBinWidth[2][2]; 
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      sprintf(histName,"histBinWidth%d_%d",capArray,half);
      histBinWidth[capArray][half] = new TH1F(histName,histName,SAMPLES_PER_BLOCK/2,-0.5,(SAMPLES_PER_BLOCK/2)-0.5);
    }
  }

    
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
    
    TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
    TGraph *grZeroMean = zeroMean(gr);
    Int_t numSamples = grZeroMean->GetN();
    Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;

    for(block=1; block<numBlocks; block++){ //FIXME -- Only use blocks > 0
      if(block%2) thisCapArray=1-capArray;
      else thisCapArray=capArray;
      TGraph *grBlock = getBlockGraph(grZeroMean, block);
      numEvents[thisCapArray]++;
      for(half=0;half<2;half++){
	TGraph *grHalf = getHalfGraph(grBlock, half);
	Double_t *yVals = grHalf->GetY();
	for(sample=0;sample<(SAMPLES_PER_BLOCK)/2-1;sample++){
	  Double_t val1 = yVals[sample];
	  Double_t val2 = yVals[sample+1];
	  if(val1<0 && val2>0){
	    histBinWidth[thisCapArray][half]->Fill(sample);
	    zcCount[thisCapArray][half][sample]+=1;
	  }
	  else if(val1>0 && val2<0){
	    histBinWidth[thisCapArray][half]->Fill(sample);
	    zcCount[thisCapArray][half][sample]+=1;
	  }
	  else if(val1==0 || val2==0){
	    histBinWidth[thisCapArray][half]->Fill(sample, 0.5);
	    zcCount[thisCapArray][half][sample]+=0.5;
	  }
	}//sample
	if(grHalf) delete grHalf;
      }//half      
      if(grBlock) delete grBlock;
    }//block
    
    delete gr;
    delete grZeroMean;
    
  }//entry
  std::cerr << "\n";

  //Scale the ZC and calculate the bin widths
  for(capArray=0;capArray<2;capArray++){
    for(half=0;half<2;half++){
      histBinWidth[capArray][half]->Scale(1./numEvents[capArray]);
      histBinWidth[capArray][half]->Scale(0.5*period);
      histBinWidth[capArray][half]->Write();
    }//half
  }//capArray

  for(capArray=0;capArray<2;capArray++){
    for(half=0;half<2;half++){
      for(sample=0;sample<(SAMPLES_PER_BLOCK/2)-1;sample++){ //we only calculate 31 intersample times
	binWidth = zcCount[capArray][half][sample]/numEvents[capArray];
	binWidth = binWidth*0.5*period;
	average_bin_width += binWidth/(2*(SAMPLES_PER_BLOCK-1));
	//	printf("capArray %i half %i sample %i binWidth %f\n", capArray, half, sample, binWidth);
	binWidthsTree->Fill();
      }//sample
    }//half
  }//capArray


  //  save_inter_sample_times(outFile, dda, chan);


  binWidthsTree->AutoSave();
  histFile->Write();
  
  //We have calculated the binWidths between adjacent event or adjacent odd samples, so the nominal
  //inter-sample width is half this
  printf("Average binWidth %f ns\tAverage sampling speed %f GSamples/s\n", 0.5*average_bin_width, 2./average_bin_width);

  cout << "\n";
  return 0;
}

TGraph* zeroMean(TGraph* gr){
  Double_t *xVals=gr->GetX();
  Double_t *yVals=gr->GetY();
  Int_t maxN = gr->GetN();

  if(maxN<1) return NULL;

  Double_t mean=0;
  for(int i=0;i<maxN; i++){
    mean+=yVals[i]/maxN;
  }
  Double_t *yValsNew = new Double_t[maxN];
  for(int i=0;i<maxN; i++){
    yValsNew[i]=yVals[i]-mean;
  }
  TGraph *grZeroMeaned = new TGraph(maxN, xVals, yValsNew);
  
  delete yValsNew;
  return grZeroMeaned;
  
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

TGraph *getHalfGraph(TGraph *fullGraph, Int_t half){
  Int_t numSamples = fullGraph->GetN();
  Double_t *xFull  = fullGraph->GetX();
  Double_t *yFull  = fullGraph->GetY();
  Double_t *newX = new Double_t[numSamples/2];
  Double_t *newY = new Double_t[numSamples/2];

  for(Int_t sample=0;sample<numSamples;sample++){
    if(sample%2!=half) continue;
    newX[sample/2]=xFull[sample];
    newY[sample/2]=yFull[sample];
    //    printf("half %i sample/2 %i sample %i\n", half, sample/2, sample);

  }
  TGraph *halfGraph = new TGraph(numSamples/2, newX, newY);
  delete newX;
  delete newY;
  return halfGraph;
  
}
