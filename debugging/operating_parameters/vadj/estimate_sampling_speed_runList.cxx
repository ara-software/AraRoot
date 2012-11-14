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

Double_t estimate_sampling_speed(char *, char*, Int_t, Int_t, Double_t);

using namespace std;
int main(int argc, char **argv)
{
  if(argc<2) {
    std::cerr << "Usage: " << argv[0] << " <runList>\n";
    return 1;
  }

  printf("runList %s\n", argv[1]);

  std::fstream runDescriptionFile(argv[1]);

  if(!runDescriptionFile){
    printf("Unable to open runList\n");
    return -1;
  }
  char *runFileBaseName = new char[100];
  char *pedFileBaseName = new char[100];
  char *outFileName = new char[200];
  Int_t runNumber=0, pedNumber=0, dda=0, channel=0, vadj=0;
  Double_t frequency=0;
  Double_t speedDda[4][10]={{0}};
  Double_t vadjDda[4][10]={{0}};
  Int_t loopDda[4]={0};


  runDescriptionFile >> runFileBaseName >> pedFileBaseName >> outFileName;
  while(runDescriptionFile >> runNumber >> pedNumber >> vadj >> frequency >> channel >> dda){

    char runFileName[100], pedFileName[100];
    sprintf(runFileName, "%s/run%i/event%i.root", runFileBaseName, runNumber, runNumber);
    sprintf(pedFileName, "%s/run_000%i/pedestalValues.run000%i.dat", pedFileBaseName, pedNumber, pedNumber);

    // printf("run %s ped %s\n", runFileName, pedFileName);
    //    printf("vadj %i channel %i dda %i frequency %f\n", vadj, channel, dda, frequency);

    Double_t speed = estimate_sampling_speed(runFileName, pedFileName, dda, channel, frequency);
    speedDda[dda][loopDda[dda]] = speed;
    vadjDda[dda][loopDda[dda]] = vadj;
    loopDda[dda]++;

  }
  
  TFile *fpOut = new TFile(outFileName, "RECREATE");
  char grName[100];
  char grTitle[100];
  for(dda=0;dda<4;dda++){

    TGraph *grDda = new TGraph(loopDda[dda], speedDda[dda],vadjDda[dda]);
    sprintf(grName, "grDda%i", dda);
    sprintf(grTitle, "DDA%i", dda);
    grDda->SetName(grName);
    grDda->SetTitle(grTitle);
    grDda->GetXaxis()->SetTitle("speed (GS/s)");
    grDda->GetYaxis()->SetTitle("vadj DAC");
    grDda->Write();
    delete    grDda;

  }
  
  fpOut->Write();


  return 0;

  //  return estimate_sampling_speed(argv[1]);
}

Double_t estimate_sampling_speed(char *runFile, char* pedFile, Int_t dda, Int_t chan, Double_t frequency)
{
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

  //2. Set up the trees and variables

  Int_t capArray=0, half=0, sample=0, noZCs[2]={0}, noSWs[2]={0};
  Int_t block, thisCapArray;
  Double_t binWidth=0;
  Int_t numEvents[2]={0}; //used to scale the bin widths
  Double_t zcCount[2][2][32]={{{0}}};

    
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
	    zcCount[thisCapArray][half][sample]+=1;
	  }
	  else if(val1>0 && val2<0){
	    zcCount[thisCapArray][half][sample]+=1;
	  }
	  else if(val1==0 || val2==0){
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

  for(capArray=0;capArray<2;capArray++){
    for(half=0;half<2;half++){
      for(sample=0;sample<(SAMPLES_PER_BLOCK/2)-1;sample++){ //we only calculate 31 intersample times
	binWidth = zcCount[capArray][half][sample]/numEvents[capArray];
	binWidth = binWidth*0.5*period;
	average_bin_width += binWidth/(2*(SAMPLES_PER_BLOCK-1));

      }//sample
    }//half
  }//capArray

  //  printf("Average binWidth %f ns\tAverage sampling speed %f GSamples/s\n", 0.5*average_bin_width, 2./average_bin_width);
  
  cout << "\n";
  return 2./average_bin_width;
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
