////////////////////////////////////////////////////////////////////////
////   secondCalibTry -- let's calibrate the ATRI stations!         ////
////   author - jonathan davies jdavies@hep.ucl.ac.uk               ////
////                                                                ////
////   calculate the timing calibrations for ATRI stations using    ////
////   IRS 2 digitising chips                                       ////
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


//Global Variables
Double_t inter_sample_times[2][2][32]={{{0}}}; //[capArray][half][sample]
Int_t inter_sample_index[2][2][32]={{{0}}};


//Prototype Functions
TGraph* zeroMean(TGraph*);
Int_t estimate_phase(TGraph*, Double_t, Double_t*, Int_t*);
TGraph *getBlockGraph(TGraph*, Int_t);

//Need to test these functions
TGraph* apply_bin_calibration(TGraph*, Int_t);
Int_t save_inter_sample_times(char*, Int_t, Int_t);
Int_t findLastZC(TGraph*, Double_t, Double_t*);
Int_t findFirstZC(TGraph*, Double_t, Double_t*);

TGraph* getHalfGraph(TGraph*, Int_t);
Int_t calibrateDdaChan(char*, Int_t, Int_t, Double_t, Int_t, Int_t, bool);


int main(int argc, char **argv)
{
  Int_t runNum=0, pedNum=0, dda=0, chan=0;
  Double_t freq=0;
  char baseName[FILENAME_MAX];
  bool debug=false;

  if(argc<7) {
    std::cerr << "Usage: " << argv[0] << " <baseDir> <runNum> <pedNum> <freq in GHz> <dda> <chan>\n";
    return 1;
  }
  sprintf(baseName, argv[1]);
  runNum=atoi(argv[2]);
  pedNum=atoi(argv[2]);
  freq=atof(argv[4]);
  dda=atoi(argv[5]);
  chan=atoi(argv[6]);
  if(argc>7){
    if(atoi(argv[7])) debug=true;
  }

  return calibrateDdaChan(baseName, runNum, pedNum, freq, dda, chan, debug);

}


Int_t calibrateDdaChan(char* baseDirName, Int_t runNum, Int_t pedNum, Double_t freq, Int_t dda, Int_t chan, bool debug=false){
  Double_t period=1./freq;
  Int_t chanIndex=chan+RFCHAN_PER_DDA*dda;

  char runFileName[FILENAME_MAX];
  char pedFileName[FILENAME_MAX];
  sprintf(runFileName, "%s/root/run%i/event%i.root", baseDirName, runNum, runNum);
  sprintf(pedFileName, "%s/raw_data/run_%06i/pedestalWidths.run%06d.dat", baseDirName, pedNum, pedNum);

  // /unix/ara/data/ntu2012/StationTwo/raw_data/run_000464/pedestalWidths.run000464.dat 
  printf("runFileName %s\npedFileName %s\nfreq %f dda %i chan %i\n", runFileName, pedFileName, freq, dda, chan);
  
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

  if(debug)  std::cout << "Number of entries in file is " << numEntries << std::endl;

  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  
  Int_t stationId=0;
  eventTree->GetEntry(0);
  stationId= evPtr->stationId;
  if(debug)  std::cerr << "stationId " << stationId << "\n";
  AraEventCalibrator *calib = AraEventCalibrator::Instance();
  calib->setAtriPedFile(pedFileName, stationId);
  
  //General output stuff
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/run%i/binWidths.root", baseDirName, runNum);
  TFile *outFile = new TFile(outFileName, "RECREATE");
  Int_t capArray=0,thisCapArray=0, block=0,half=0,sample=0;
  Int_t numEvents[2]={0};

  //BinWidth Histos
  TH1F *histBinWidth[2][2]; 
  char histName[FILENAME_MAX];
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      sprintf(histName,"histBinWidth%d_%d",capArray,half);
      histBinWidth[capArray][half] = new TH1F(histName,histName,SAMPLES_PER_BLOCK/2,-0.5,(SAMPLES_PER_BLOCK/2)-0.5);
    }
  }

  //Interleave Histos
  TTree *lagTree = new TTree("lagTree","lagTree");
  Int_t noZCs[2]={0};
  Double_t lag1,lag0,deltaLag;

  lagTree->Branch("block",&block,"block/I");
  lagTree->Branch("noZCs", &noZCs, "noZCs[2]/I");
  lagTree->Branch("thisCapArray",&thisCapArray,"thisCapArray/I");
  lagTree->Branch("capArray",&capArray,"capArray/I");
  lagTree->Branch("lag1",&lag1,"lag1/D");
  lagTree->Branch("lag0",&lag0,"lag0/D");
  lagTree->Branch("deltaLag",&deltaLag,"deltaLag/D");
  Double_t lag[2] = {0};
  TH1D *lagHist[2]={0};

  //Epsilon Histos
  TTree *epsilonTree = new TTree("epsilonTree", "epsilonTree");
  Double_t epsilon=0;
  Double_t firstZC=0, lastZC=0;
  Int_t firstZCCount=0, lastZCCount=0;
  Int_t atriBlock=0;
  epsilonTree->Branch("block",&block,"block/I");
  epsilonTree->Branch("thisCapArray",&thisCapArray,"thisCapArray/I");
  epsilonTree->Branch("capArray",&capArray,"capArray/I");
  epsilonTree->Branch("epsilon",&epsilon,"epsilon/D");
  epsilonTree->Branch("firstZC",&firstZC,"firstZC/D");
  epsilonTree->Branch("lastZC",&lastZC,"lastZC/D");
  epsilonTree->Branch("lastZCCount",&lastZCCount,"lastZCCount/I");
  epsilonTree->Branch("firstZCCount",&firstZCCount,"firstZCCount/I");
  epsilonTree->Branch("half", &half, "half/I");
  epsilonTree->Branch("atriBlock", &atriBlock, "atriBlock/I");


  //BinWidth Calibration
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[0].getCapArray(); //capArray of first block
    
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
	  }
	  else if(val1>0 && val2<0){
	    histBinWidth[thisCapArray][half]->Fill(sample);
	  }
	  else if(val1==0 || val2==0){
	    histBinWidth[thisCapArray][half]->Fill(sample, 0.5);
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
      Double_t time=0;
      for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
	time+=histBinWidth[capArray][half]->GetBinContent(sample);
	inter_sample_times[capArray][half][sample]=time;
	inter_sample_index[capArray][half][sample]=sample;
	if(debug) printf("capArray %i half %i sample %i time %f\n", capArray, half, sample, time);
      }//sample
    }//half
  }//capArray
  




  //Interleave Calibration
  if(debug) numEntries=10;
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
    
    TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
    TGraph *grZeroMean = zeroMean(gr);
    Int_t numSamples = grZeroMean->GetN();
    Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;

    for(block=1; block<numBlocks; block++){ //FIXME -- only use blocks>0
      if(block%2) thisCapArray=1-capArray;
      else thisCapArray=capArray;
      TGraph *grBlock = getBlockGraph(grZeroMean, block);
      TGraph *grBlockCalibrated = apply_bin_calibration(grBlock, thisCapArray);
      TGraph *grHalf0 = getHalfGraph(grBlockCalibrated, 0);
      TGraph *grHalf1 = getHalfGraph(grBlockCalibrated, 1);
      Int_t retVal = estimate_phase(grHalf0, period, &lag0, &noZCs[0]);
      if(retVal==0){
	retVal = estimate_phase(grHalf1, period, &lag1, &noZCs[1]);
	if(retVal==0){
	  deltaLag = lag0-lag1;//FIXME
	  while(TMath::Abs(deltaLag-period)<TMath::Abs(deltaLag))
	    deltaLag-=period;
	  while(TMath::Abs(deltaLag+period)<TMath::Abs(deltaLag))
	    deltaLag+=period;
	  lagTree->Fill();
	}
      }

      if(grHalf0) delete grHalf0;
      if(grHalf1) delete grHalf1;
      if(grBlockCalibrated) delete grBlockCalibrated;
      if(grBlock) delete grBlock;
    }//block

    
    delete gr;
    delete grZeroMean;
    
  }//entry
  std::cerr << "\n";

  //Now calculate the lag
  for(capArray=0;capArray<2;capArray++){
    char varexp[100];
    char selection[100];
    char name[100];
    sprintf(varexp, "deltaLag>>tempHist");
    sprintf(selection, "thisCapArray==%i&&noZCs[0]==noZCs[1]", capArray);
    sprintf(name, "lag_hist_capArray_%i", capArray);
    lagTree->Draw(varexp, selection);
    lagHist[capArray] = (TH1D*) gPad->GetPrimitive("tempHist");
    lagHist[capArray]->SetName(name);
    lagHist[capArray]->SetTitle(name);
    lagHist[capArray]->Write();
    lag[capArray] = lagHist[capArray]->GetMean(1);
  }//capArray
  
  for(capArray=0;capArray<2;capArray++){
    for(half=0;half<2;half++){
      Double_t time=0;
      for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
	inter_sample_times[capArray][half][sample]=inter_sample_times[capArray][half][sample]+lag[capArray]*half;
	inter_sample_index[capArray][half][sample]=inter_sample_index[capArray][half][sample];
      }//sample
    }//half
  }//capArray

 
  //Now calculate epsilon
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
    atriBlock = evPtr->blockVec[dda].getBlock();
    TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
    TGraph *grZeroMean = zeroMean(gr);
    Int_t numSamples = grZeroMean->GetN();
    Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
    
    for(block=1; block<numBlocks-1; block++){ //FIXME -- only use blocks > 0
      if(block%2) thisCapArray=1-capArray;
      else thisCapArray=capArray;
      TGraph *grBlock0 = getBlockGraph(grZeroMean, block);
      TGraph *grBlockCalibrated0 = apply_bin_calibration(grBlock0, thisCapArray);
      TGraph *grBlock1 = getBlockGraph(grZeroMean, block+1);
      TGraph *grBlockCalibrated1 = apply_bin_calibration(grBlock1, 1-thisCapArray);

      // lastZCCount = findLastZC(grBlockCalibrated0, period, &lastZC);
      // firstZCCount = findFirstZC(grBlockCalibrated1, period, &firstZC);
      // Double_t *tVals = grBlockCalibrated0->GetX();
      // Double_t lastSample = tVals[SAMPLES_PER_BLOCK-1];
      // epsilon = firstZC-lastZC+lastSample;
      // epsilonTree->Fill();

      TGraph *grBlock0Half0 = getHalfGraph(grBlockCalibrated0, 0);
      TGraph *grBlock0Half1 = getHalfGraph(grBlockCalibrated0, 1);
      TGraph *grBlock1Half0 = getHalfGraph(grBlockCalibrated1, 0);
      TGraph *grBlock1Half1 = getHalfGraph(grBlockCalibrated1, 1);

      lastZCCount = findLastZC(grBlock0Half0, period, &lastZC);
      firstZCCount = findFirstZC(grBlock1Half0, period, &firstZC);
      half = 0;
      Double_t *tVals = grBlockCalibrated0->GetX();
      Double_t lastSample = tVals[SAMPLES_PER_BLOCK-1];
      epsilon = -firstZC+lastZC-lastSample+period;
      
      if(epsilon < -1*period) epsilon+=period;
      if(epsilon > +1*period) epsilon-=period;

      epsilonTree->Fill();
      lastZCCount = findLastZC(grBlock0Half1, period, &lastZC);
      firstZCCount = findFirstZC(grBlock1Half1, period, &firstZC);
      half = 1;
      epsilon = -firstZC+lastZC-lastSample+period;

      if(epsilon < -1*period) epsilon+=period;
      if(epsilon > +1*period) epsilon-=period;
      epsilonTree->Fill();
      if(grBlock0Half0) delete grBlock0Half0;
      if(grBlock0Half1) delete grBlock0Half1;
      if(grBlock1Half0) delete grBlock1Half0;
      if(grBlock1Half1) delete grBlock1Half1;
      
      if(grBlockCalibrated0) delete grBlockCalibrated0;
      if(grBlock0) delete grBlock0;
      if(grBlockCalibrated1) delete grBlockCalibrated1;
      if(grBlock1) delete grBlock1;
    }//block
    
    delete gr;
    delete grZeroMean;
    
  }//entry
  std::cerr << "\n";

  outFile->Write();

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
    blockX[sample] = fullX[sample];
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
    //printf("half %i sample/2 %i sample %i\n", half, sample/2, sample);

  }
  TGraph *halfGraph = new TGraph(numSamples/2, newX, newY);
  // for(int sample=0;sample<numSamples/2;sample++){
  //   printf("sample %i newX[%i] %f newY[%i] %f\n", sample, sample, newX[sample], sample, newY[sample]);
  // }
   
  delete newX;
  delete newY;
  return halfGraph;
  
}

TGraph* apply_bin_calibration(TGraph* grBlock, Int_t capArray){
  Int_t numSamples = grBlock->GetN();
  if(numSamples!=SAMPLES_PER_BLOCK){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK);
    return NULL;

  }
  
  Double_t *yVals = grBlock->GetY();
  Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK];
  
  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK;sample++){
    xVals[sample] = inter_sample_times[capArray][sample%2][sample/2];
  }//sample
  //FIXME -- need to take into account the ordering of samples
  //Maybe make a note in the calibration file name
  
  TGraph *grBlockCalibrated = new TGraph(SAMPLES_PER_BLOCK, xVals, yVals);
  delete xVals;

  return grBlockCalibrated;
}
Int_t save_inter_sample_times(char* name, Int_t dda, Int_t channel){

  char outName[180];
  sprintf(outName, "%s_sample_timing.txt", name);
  std::ofstream OutFile(outName);
  Int_t capArray, sample;


  for(int capArray=0;capArray<2;capArray++) {
    OutFile << dda << "\t" << channel << "\t" << capArray << "\t";   
    for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
      //Index values
      if(sample%2==0)
	OutFile << inter_sample_index[capArray][0][sample/2] << " ";
      if(sample%2)
	OutFile << inter_sample_index[capArray][1][sample/2] << " ";
    }
    OutFile << "\n";
    OutFile << dda << "\t" << channel << "\t" << capArray << "\t";   
    for(int sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
      //time values
      if(sample%2==0)
	OutFile << inter_sample_times[capArray][0][sample/2] << " ";
      if(sample%2)
	OutFile << inter_sample_times[capArray][1][sample/2] << " ";
    }
    OutFile << "\n";
  }
  OutFile.close();





}


Int_t estimate_phase(TGraph *gr, Double_t period, Double_t *meanPhase, Int_t *totalZCs){
  Double_t *yVals = gr->GetY();
  Double_t *xVals = gr->GetX();
  Int_t numSamples = gr->GetN();
  if(numSamples != SAMPLES_PER_BLOCK/2){
    fprintf(stderr, "%s : Wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK/2);
    return -1;
  }
  Double_t phase=0;
  Int_t numZCs=0;

  for(int sample=0;sample<numSamples-1;sample++){
    Double_t y1=yVals[sample];
    Double_t y2=yVals[sample+1];
    if(y1<0 && y2>0){
      Double_t x1=xVals[sample]; 
      Double_t x2=xVals[sample+1]; 
      Double_t zc=((0-y1)/(y2-y1))*(x2-x1)+x1;
      //      if(zc<0.6) printf("sample %i y1 %f y2 %f x1 %f x2 %f\n", sample, y1, y2, x1, x2);
      phase+=zc-numZCs*period;
      //      printf("zc num %i val %f adjusted val %f\n", numZCs, zc, zc-numZCs*period);
      numZCs++;
    }
  }//sample

  if(!numZCs)
    phase=0;
  else phase=phase/numZCs;
  
  *totalZCs = numZCs;
  *meanPhase = phase;
  return 0;
  
  
}

Int_t findFirstZC(TGraph *graph, Double_t period, Double_t *lastAvZC){
  Double_t *postWrap[2], thisZC=0, lastZC=0, meanZC=0;
  Int_t noZCs=0;
  Int_t noSamples=graph->GetN();
  postWrap[0]=graph->GetX();
  postWrap[1]=graph->GetY();

  //  if(debug>1)  printf("Finding last ZC\n");

  for(Int_t Sample=0; Sample<noSamples-1; Sample++){
    if(postWrap[1][Sample]<0&&postWrap[1][Sample+1]>0){
      Double_t x1=postWrap[0][Sample];
      Double_t x2=postWrap[0][Sample+1];
      Double_t y1=postWrap[1][Sample];
      Double_t y2=postWrap[1][Sample+1];
      thisZC=y1*(x1-x2)/(y2-y1)+(x1);  
      if(!noZCs){
	lastZC=thisZC;
      }
      //      printf("zc %i position %f adj position %f\n", noZCs+1, thisZC, thisZC-noZCs*period);
      thisZC-=noZCs*period;
      meanZC+=thisZC;
      noZCs++;
    }
  }
  
  //  if(debug>1)  printf("Average value is %f\n", meanZC/noZCs);
  
  if(noZCs){
    *lastAvZC=meanZC/noZCs;
    return noZCs;
  }
  return -1;
}
Int_t findLastZC(TGraph *graph, Double_t period, Double_t *lastAvZC){
  Double_t *preWrap[2], thisZC=0, lastZC=0, meanZC=0;
  Int_t noZCs=0;
  Int_t noSamples=graph->GetN();
  preWrap[0]=graph->GetX();
  preWrap[1]=graph->GetY();

  //  if(debug>1)  printf("Finding last ZC\n");

  for(Int_t Sample=noSamples-1; Sample>0; --Sample){
    if(preWrap[1][Sample-1]<0&&preWrap[1][Sample]>0){
      Double_t x1=preWrap[0][Sample-1];
      Double_t x2=preWrap[0][Sample];
      Double_t y1=preWrap[1][Sample-1];
      Double_t y2=preWrap[1][Sample];
     
      thisZC=y1*(x1-x2)/(y2-y1)+(x1);  
      if(!noZCs){
	lastZC=thisZC;
      }
      //      printf("zc %i position %f adj position %f\n", noZCs+1, thisZC, thisZC+noZCs*period);
      thisZC+=noZCs*period;
      meanZC+=thisZC;
      noZCs++;
    }
  }
  
  //  if(debug>1)  printf("Average value is %f\n", meanZC/noZCs);
  
  if(noZCs){
    *lastAvZC=meanZC/noZCs;
    return noZCs;
  }
  return -1;
}
