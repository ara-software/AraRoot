////////////////////////////////////////////////////////////////////////
////   thirdCalibTry -- let's calibrate the ATRI stations!         ////
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
Double_t inter_sample_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2][SAMPLES_PER_BLOCK*2]={{{{0}}}}; //[dda][chan][capArray][sample]
Double_t half_inter_sample_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2][2][SAMPLES_PER_BLOCK]={{{{{0}}}}}; //[dda][chan][capArray][half][sample]
Int_t inter_sample_index[DDA_PER_ATRI][RFCHAN_PER_DDA][2][SAMPLES_PER_BLOCK*2]={{{{0}}}}; //[dda][chan][capArray][sample]
Double_t epsilon_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}}; //[dda][chan][capArray]
Double_t epsilon_times_half[DDA_PER_ATRI][RFCHAN_PER_DDA][2][2]={{{{0}}}}; //[dda][chan][capArray][2]


//Prototype Functions
TGraph* zeroMean(TGraph*);
Int_t estimate_phase(TGraph*, Double_t, Double_t*, Int_t*);
Int_t estimate_phase_two_blocks(TGraph*, Double_t, Double_t*, Int_t*);
TGraph *getBlockGraph(TGraph*, Int_t);
TGraph *getTwoBlockGraph(TGraph*, Int_t);

TGraph* apply_bin_calibration(TGraph*, Int_t, Int_t, Int_t);
TGraph* apply_bin_calibration_half(TGraph*, Int_t, Int_t, Int_t, Int_t);
TGraph* apply_bin_calibration_two_blocks(TGraph*, Int_t, Int_t, Int_t);
Int_t save_inter_sample_times(char*, int goodSampleTiming[DDA_PER_ATRI][RFCHAN_PER_DDA]);
Int_t save_epsilon_times(char*, int goodSampleTiming[DDA_PER_ATRI][RFCHAN_PER_DDA]);
Int_t findLastZC(TGraph*, Double_t, Double_t*);
Int_t findFirstZC(TGraph*, Double_t, Double_t*);

TGraph* getHalfGraph(TGraph*, Int_t);
TGraph* getHalfGraphTwoBlocks(TGraph*, Int_t);
Int_t calibrateDdaChan(char*, Int_t, Int_t, Double_t, bool);


int main(int argc, char **argv)
{
  Int_t runNum=0, pedNum=0;
  Double_t freq=0;
  char baseName[FILENAME_MAX];
  bool debug=false;

  if(argc<5) {
    std::cerr << "Usage: " << argv[0] << " <baseDir> <runNum> <pedNum> <freq in GHz>\n";
    return 1;
  }
  sprintf(baseName, argv[1]);
  runNum=atoi(argv[2]);
  pedNum=atoi(argv[3]);
  freq=atof(argv[4]);
  if(argc>5){
    if(atoi(argv[5])) debug=true;
  }

  return calibrateDdaChan(baseName, runNum, pedNum, freq, debug);

}


Int_t calibrateDdaChan(char* baseDirName, Int_t runNum, Int_t pedNum, Double_t freq, bool debug=false){
  Double_t period=1./freq;
  Int_t chanIndex=0;
  Int_t dda=0, chan=0;
  char runFileName[FILENAME_MAX];
  char pedFileName[FILENAME_MAX];
  sprintf(runFileName, "%s/root/run%i/event%i.root", baseDirName, runNum, runNum);
  sprintf(pedFileName, "%s/raw_data/run_%06i/pedestalValues.run%06d.dat", baseDirName, pedNum, pedNum);

  // /unix/ara/data/ntu2012/StationTwo/raw_data/run_000464/pedestalWidths.run000464.dat 
  printf("runFileName %s\npedFileName %s\nfreq %f GHz\n", runFileName, pedFileName, freq);
  
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
  calib->setAtriPedFile(pedFileName, 2);
  
  //General output stuff
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/run%i/calibRJN.root", baseDirName, runNum);
  TFile *outFile = new TFile(outFileName, "RECREATE");
  Int_t capArray=0,thisCapArray=0, block=0,half=0,sample=0;
  Int_t numEvents[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};

  //BinWidth Histos
  TH1F *histBinWidth[DDA_PER_ATRI][RFCHAN_PER_DDA][2]; 
  char histName[FILENAME_MAX];
  for(half=0;half<2;half++) {
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	sprintf(histName,"histBinWidth_dda%d_chan%d_%d",dda, chan,half);
	histBinWidth[dda][chan][half] = new TH1F(histName,histName,SAMPLES_PER_BLOCK,-0.5,SAMPLES_PER_BLOCK-0.5);
	
      }
    }
  }

  //Interleave Histos
  TTree *lagTree = new TTree("lagTree","lagTree");
  Int_t noZCs[2]={0};
  Double_t lag1,lag0,deltaLag;
  lagTree->Branch("dda",&dda,"dda/I");
  lagTree->Branch("chan",&chan,"chan/I");
  lagTree->Branch("block",&block,"block/I");
  lagTree->Branch("noZCs", &noZCs, "noZCs[2]/I");
  lagTree->Branch("lag1",&lag1,"lag1/D");
  lagTree->Branch("lag0",&lag0,"lag0/D");
  lagTree->Branch("deltaLag",&deltaLag,"deltaLag/D");
  Double_t lag[DDA_PER_ATRI][RFCHAN_PER_DDA] = {{0}};
  TH1F *lagHist[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      sprintf(histName,"lag_hist_dda%i_chan%i",dda, chan);
      lagHist[dda][chan] = new TH1F(histName,histName,200,-1.0,1.0);
    }
  }


  

  //Epsilon Histos
  TTree *epsilonTree = new TTree("epsilonTree", "epsilonTree");
  Double_t epsilon=0;
  Double_t firstZC=0, lastZC=0;
  Int_t firstZCCount=0, lastZCCount=0;
  Int_t atriBlock=0;
  TH1* histEpsilon[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}}; //[dda][chan];
  TH1* histEpsilonHalf[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}}; //[dda][chan];
  epsilonTree->Branch("dda",&dda,"dda/I");
  epsilonTree->Branch("chan",&chan,"chan/I");
  epsilonTree->Branch("block",&block,"block/I");
  epsilonTree->Branch("epsilon",&epsilon,"epsilon/D");
  epsilonTree->Branch("firstZC",&firstZC,"firstZC/D");
  epsilonTree->Branch("lastZC",&lastZC,"lastZC/D");
  epsilonTree->Branch("lastZCCount",&lastZCCount,"lastZCCount/I");
  epsilonTree->Branch("firstZCCount",&firstZCCount,"firstZCCount/I");
  epsilonTree->Branch("half", &half, "half/I");
  epsilonTree->Branch("atriBlock", &atriBlock, "atriBlock/I");
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      sprintf(histName,"epsilon_hist_dda%i_chan%i",dda, chan);
      histEpsilon[dda][chan] = new TH1F(histName,histName,600,-3.0,3.0);
      for(int half=0;half<2;half++) {
	sprintf(histName,"epsilon_hist_dda%i_chan%i_half%d",dda, chan,half);
	histEpsilonHalf[dda][chan][half] = new TH1F(histName,histName,600,-3.0,3.0);
      }
    }
  }
  


  Double_t time=0;
  Int_t index=0;
  TTree *binWidthsTree = new TTree("binWidthsTree", "binWidthsTree");
  binWidthsTree->Branch("dda", &dda, "dda/I");
  binWidthsTree->Branch("chan", &chan, "chan/I");
  binWidthsTree->Branch("capArray", &capArray, "capArray/I");
  binWidthsTree->Branch("sample", &sample, "sample/I");
  binWidthsTree->Branch("time", &time, "time/D");
  binWidthsTree->Branch("index", &index, "index/I");
  binWidthsTree->Branch("epsilon", &epsilon, "epsilon/D");
 
 
  Int_t zcNum;
  Double_t zcEst;
  Double_t zcMeas;
  TTree *zcTree = new TTree("zcTree", "zcTree");
  zcTree->Branch("dda", &dda, "dda/I");
  zcTree->Branch("chan", &chan, "chan/I");
  zcTree->Branch("capArray", &capArray, "capArray/I");
  zcTree->Branch("sample", &sample, "sample/I");
  zcTree->Branch("zcNum", &zcNum, "zcNum/I");
  zcTree->Branch("zcEst",&zcEst,"zcEst/D");
  zcTree->Branch("zcFirst",&zcFirst,"zcFirst/D");
  zcTree->Branch("zcMeas",&zcMeas,"zcMeas/D");
  
  


  //BinWidth Calibration
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[0].getCapArray(); //capArray of first block
    
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	chanIndex=chan+RFCHAN_PER_DDA*dda;
	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
	TGraph *grZeroMean = zeroMean(gr);
	Int_t numSamples = grZeroMean->GetN();
	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	
	
	for(block=0; block<numBlocks-1; block++){ //FIXME -- Only use blocks > 0
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  if(thisCapArray==1) continue;
	  TGraph *grTwoBlock = getTwoBlockGraph(grZeroMean, block);
	  numEvents[dda][chan]++;
	  for(half=0;half<2;half++){
	    TGraph *grHalf = getHalfGraphTwoBlocks(grTwoBlock, half);
	    Double_t *yVals = grHalf->GetY();
	    for(sample=0;sample<SAMPLES_PER_BLOCK-1;sample++){
	      Double_t val1 = yVals[sample];
	      Double_t val2 = yVals[sample+1];
	      if(val1<0 && val2>0){
		histBinWidth[dda][chan][half]->Fill(sample);
	      }
	      else if(val1>0 && val2<0){
		histBinWidth[dda][chan][half]->Fill(sample);
	      }
	      else if(val1==0 || val2==0){
		histBinWidth[dda][chan][half]->Fill(sample, 0.5);
	      }
	    }//sample
	    if(grHalf) delete grHalf;
	  }//half      
	  if(grTwoBlock) delete grTwoBlock;
	}//block
	
	delete gr;
	delete grZeroMean;
      }//chan
    }//dda

  }//entry
  std::cerr << "\n";

  //Scale the ZC and calculate the bin widths

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(half=0;half<2;half++){
	histBinWidth[dda][chan][half]->Scale(1./numEvents[dda][chan]);
	histBinWidth[dda][chan][half]->Scale(0.5*period);
	histBinWidth[dda][chan][half]->Write();
      }//half
    }//chan
  }//dda

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(half=0;half<2;half++){
	time=0;
	for(capArray=0;capArray<2;capArray++){
	  for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
	    half_inter_sample_times[dda][chan][capArray][half][sample]=time;
	    inter_sample_times[dda][chan][capArray][2*sample+half]=time;   
	    inter_sample_index[dda][chan][capArray][2*sample+half]=2*sample+half;
	    if(debug&&dda==0&&chan==3) printf("capArray %i half %i sample %i index %d time %f\n", capArray, half, sample, 2*sample+half, time);
	    time+=histBinWidth[dda][chan][half]->GetBinContent(sample+SAMPLES_PER_BLOCK/2*capArray+1);
	    if(debug&&dda==0&&chan==0) printf("dda %i chan %i capArray %i half %d time %f\n", dda, chan, capArray,half, half_inter_sample_times[dda][chan][capArray][half][sample]);
	  }//sample
	}//capArray
      }//half
    }//chan
  }//dda






  //Interleave Calibration
  //if(debug) numEntries=10;
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[0].getCapArray(); //capArray of first block
    
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	chanIndex=chan+RFCHAN_PER_DDA*dda;
	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
	TGraph *grZeroMean = zeroMean(gr);
	Int_t numSamples = grZeroMean->GetN();
	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	
	for(block=0; block<numBlocks-1; block++){ //FIXME -- only use blocks>0
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  if(thisCapArray==1) continue;
	  TGraph *grBlock = getTwoBlockGraph(grZeroMean, block);
	  TGraph *grBlockCalibrated = apply_bin_calibration_two_blocks(grBlock, thisCapArray, dda, chan);
	  TGraph *grHalf0 = getHalfGraphTwoBlocks(grBlockCalibrated, 0);
	  TGraph *grHalf1 = getHalfGraphTwoBlocks(grBlockCalibrated, 1);
	  Int_t retVal = estimate_phase_two_blocks(grHalf0, period, &lag0, &noZCs[0]);
	  if(retVal==0){
	    retVal = estimate_phase_two_blocks(grHalf1, period, &lag1, &noZCs[1]);
	    if(retVal==0){
	      deltaLag = lag0-lag1;//FIXME
	      while(TMath::Abs(deltaLag-period)<TMath::Abs(deltaLag))
		deltaLag-=period;
	      while(TMath::Abs(deltaLag+period)<TMath::Abs(deltaLag))
		deltaLag+=period;
	      lagTree->Fill();
	      if(TMath::Abs(noZCs[0]-noZCs[1])==0) lagHist[dda][chan]->Fill(deltaLag);
	    }
	  }
	  if(grHalf0) delete grHalf0;
	  if(grHalf1) delete grHalf1;
	  if(grBlockCalibrated) delete grBlockCalibrated;
	  if(grBlock) delete grBlock;
	}//block
	
	
	delete gr;
	delete grZeroMean;
      }//chan
    }//dda

  }//entry
  std::cerr << "\n";

  char varexp[100];
  char selection[100];
  char name[100];
  //Now calculate the lag
  Int_t goodSampleTiming[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};


  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      lag[dda][chan] = lagHist[dda][chan]->GetMean(1);
      if((lagHist[dda][chan]->GetRMS())>0.1) {
	printf("dda %i chan %i rms %f\n", dda, chan, lagHist[dda][chan]->GetRMS());
	goodSampleTiming[dda][chan]=0;
      }
      else {
	goodSampleTiming[dda][chan]=1;
      }
      //	printf("dda %i chan %i capArray %i lag %f\n", dda, chan, capArray ,lag[dda][chan][capArray]);	
    }//chan
  }//dda
  
  
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(half=0;half<2;half++){
	  Double_t time=0;
	  for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){

	    if(lag[dda][chan]>0) {
	      inter_sample_times[dda][chan][capArray][2*sample+half]=inter_sample_times[dda][chan][capArray][2*sample+half]+(lag[dda][chan])*half;
	      half_inter_sample_times[dda][chan][capArray][half][sample]+=(lag[dda][chan])*half;
	    }
	    else {
	      inter_sample_times[dda][chan][capArray][2*sample+half]=inter_sample_times[dda][chan][capArray][2*sample+half]-(lag[dda][chan])*(1-half);
	      half_inter_sample_times[dda][chan][capArray][half][sample]-=(lag[dda][chan])*(1-half);
	    }
	  }//sample
	}//half
      }//capArray
    }//chan
  }//dda

  //now sort the times
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	TMath::Sort(SAMPLES_PER_BLOCK,inter_sample_times[dda][chan][capArray],inter_sample_index[dda][chan][capArray],kFALSE);
	
	if(debug&&dda==0&&chan==3) {
	  for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	    int index=inter_sample_index[dda][chan][capArray][sample];
	    double time=inter_sample_times[dda][chan][capArray][index];
	    printf("capArray %i sample %i index %d time %f\n", capArray, sample, index,time);
	  }
	}
      }
    }
  }

  //Now revert the inter_sample times to start at zero for each capArray;
  //This here is all buggered because quite regular the firstTime in the capArray is index [1] not index [0]
  Double_t firstTime=0;
  Double_t secondTime=0;
  Double_t lastTime=0;
  Double_t penultimateTime=0;
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      firstTime=inter_sample_times[dda][chan][1][0];
      secondTime=inter_sample_times[dda][chan][1][0];
      if(secondTime<firstTime) firstTime=secondTime;
      lastTime=inter_sample_times[dda][chan][0][SAMPLES_PER_BLOCK-1];
      penultimateTime=inter_sample_times[dda][chan][0][SAMPLES_PER_BLOCK-2];
      if(lastTime<penultimateTime) lastTime=penultimateTime;
      epsilon_times[dda][chan][1]=firstTime-lastTime;
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	inter_sample_times[dda][chan][1][sample]-=firstTime;
      }
      if(debug ) {
	std::cout << "epsilon_times[ " << dda << "][" << chan << "][1]= " << epsilon_times[dda][chan][1] << "\n";
      }
      for(int half=0;half<2;half++) {
	Double_t firstTime=half_inter_sample_times[dda][chan][1][half][0];
	Double_t lastTime=half_inter_sample_times[dda][chan][0][half][(SAMPLES_PER_BLOCK/2)-1];
	epsilon_times_half[dda][chan][1][half]=firstTime-lastTime;
	for(int sample=0;sample>(SAMPLES_PER_BLOCK/2);sample++) {
	  half_inter_sample_times[dda][chan][capArray][half][sample]-=firstTime;
	}
      }
      
    }
  }
  
  //  if(debug) numEntries=10;
 
  char graphName[180];

  //Now calculate epsilon
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);
    capArray = evPtr->blockVec[0].getCapArray(); //capArray of first block
    atriBlock = evPtr->blockVec[0].getBlock();

    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	chanIndex=chan+RFCHAN_PER_DDA*dda;
	
	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
	TGraph *grZeroMean = zeroMean(gr);
	Int_t numSamples = grZeroMean->GetN();
	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	
	for(block=0; block<numBlocks-1; block++){ 
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  if(thisCapArray==0) continue;
	  TGraph *grBlock0 = getBlockGraph(grZeroMean, block);
	  TGraph *grBlockCalibrated0 = apply_bin_calibration(grBlock0, thisCapArray, dda, chan);
	  TGraph *grBlock1 = getBlockGraph(grZeroMean, block+1);
	  TGraph *grBlockCalibrated1 = apply_bin_calibration(grBlock1, 1-thisCapArray, dda, chan);
	  
	  //	  TCanvas *can = new TCanvas();
	  //	  grBlockCalibrated0->Draw("al");
	  //	  grBlockCalibrated1->SetLineColor(kBlue+2);
	  //	  grBlockCalibrated1->Draw("l");
// 	  if(dda==0 &&chan==0) {
// 	    sprintf(graphName,"grBlock0_%d_%d",entry,block);
// 	    grBlockCalibrated0->SetName(graphName);
// 	    grBlockCalibrated0->Write();
// 	    sprintf(graphName,"grBlock1_%d_%d",entry,block);
// 	    grBlockCalibrated1->SetName(graphName);
// 	    grBlockCalibrated1->Write();
// 	  }

	  half=-1;
	  lastZCCount = findLastZC(grBlockCalibrated0, period, &lastZC);
	  firstZCCount = findFirstZC(grBlockCalibrated1, period, &firstZC);
	  Double_t *tVals = grBlockCalibrated0->GetX(); //FIXME -- is this really the last sample?
	  Double_t lastSample = tVals[SAMPLES_PER_BLOCK-1];
	  epsilon = -firstZC+lastZC-lastSample+period;
	  if(epsilon < -0.5*period) epsilon+=period;
	  if(epsilon > +0.5*period) epsilon-=period;
	  if(TMath::Abs(lastZCCount-firstZCCount)==0) histEpsilon[dda][chan]->Fill(epsilon);
	  epsilonTree->Fill();

	  for(half=0;half<2;half++) {
	    TGraph *grBlock0Half = getHalfGraph(grBlock0, half);
	    TGraph *grBlock1Half = getHalfGraph(grBlock1, half);
	    TGraph *grFirst=apply_bin_calibration_half(grBlock0Half,thisCapArray,dda,chan,half);
	    TGraph *grSecond=apply_bin_calibration_half(grBlock1Half,1-thisCapArray,dda,chan,half);
	    
	    lastZCCount = findLastZC(grFirst, period, &lastZC);
	    firstZCCount = findFirstZC(grSecond, period, &firstZC);

	    Double_t *tVals = grFirst->GetX(); //FIXME -- is this really the last sample?
	    Double_t lastSample = tVals[(SAMPLES_PER_BLOCK/2)-1];
	  //   if(dda==0 && chan==0) {
// 	      std::cout <<entry << "\t" << half << "\t" <<lastZC << "\t" << lastSample << "\t" << grFirst->GetN() << "\n";

// 	    }
	    epsilon = -firstZC+lastZC-lastSample+period;
	    if(epsilon < -0.5*period) epsilon+=period;
	    if(epsilon > +0.5*period) epsilon-=period;
	    epsilonTree->Fill();
	    if(TMath::Abs(lastZCCount-firstZCCount)==0) histEpsilonHalf[dda][chan][half]->Fill(epsilon);

	    if(grBlock0Half) delete grBlock0Half;
	    if(grBlock1Half) delete grBlock1Half;
	    if(grFirst) delete grFirst;
	    if(grSecond) delete grSecond;
	  }


	  

	  
	  if(grBlockCalibrated0) delete grBlockCalibrated0;
	  if(grBlock0) delete grBlock0;
	  if(grBlockCalibrated1) delete grBlockCalibrated1;
	  if(grBlock1) delete grBlock1;
	}//block
	
	delete gr;
	delete grZeroMean;
      }//chan
    }//dda
  }//entry
  std::cerr << "\n";

  //zCalculate actual epsilon from Tree
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      //      for(capArray=0;capArray<2;capArray++){
      //	Double_t deltaT=(inter_sample_times[dda][chan][1-capArray][inter_sample_index[dda][chan][1-capArray][63]]-inter_sample_times[dda][chan][1-capArray][inter_sample_index[dda][chan][1-capArray][62]]);
	Double_t deltaT=(inter_sample_times[dda][chan][0][inter_sample_index[dda][chan][0][63]]-inter_sample_times[dda][chan][0][inter_sample_index[dda][chan][0][62]]);
	deltaT=0; //FIXME
	epsilon_times[dda][chan][0] = histEpsilon[dda][chan]->GetMean(1)+deltaT; //FIXME -- only using one half here
	if((histEpsilon[dda][chan]->GetRMS())>0.1) printf("Bad full dda %i chan %i  half 0 rms %f\n", dda, chan, histEpsilon[dda][chan]->GetRMS());
	for(int half=0;half<2;half++) {
	  epsilon_times_half[dda][chan][0][half]= histEpsilonHalf[dda][chan][half]->GetMean();
	  if((histEpsilonHalf[dda][chan][half]->GetRMS())>0.1) printf("Bad half dda %i chan %i  half %d rms %f\n", dda, chan,half, histEpsilonHalf[dda][chan][half]->GetRMS());
	}
	//      }//capArray
    }//chan
  }//dda
  
  
  
  //Assume that within the same half everything is fine
  //So we will include all the even samples and some of the odd
  for(int dda=0;dda<DDA_PER_ATRI;dda++){
    for(int chan=0;chan<6;chan++){
      for(int capArray=0;capArray<2;capArray++) {
	Double_t minDiff=1e9;
	Int_t badIndexArray[SAMPLES_PER_BLOCK]={0};
	Int_t numBadPoints=0;
	for(int sample=1;sample<SAMPLES_PER_BLOCK;sample++) {
	  Int_t index1=inter_sample_index[dda][chan][capArray][sample];
	  Int_t index0=inter_sample_index[dda][chan][capArray][sample-1];
	  Double_t time1=inter_sample_times[dda][chan][capArray][index1];
	  Double_t time0=inter_sample_times[dda][chan][capArray][index0];
	  if((time1-time0)<minDiff) minDiff=time1-time0;
	  
	  if((time1-time0)<0.1) {
	    //Arbitrary for now
	    if(index0%2==1) {
	      if(!badIndexArray[index0]) {
		badIndexArray[index0]=1;
		numBadPoints++;
	      }
	    }
	    else if(index1%2==1) {
	      if(!badIndexArray[index1]) {
		badIndexArray[index1]=1;
		numBadPoints++;
	      }
	    }
	  }
	}
	std::cout <<  "Min Diff " << dda << "\t" << chan << "\t" << capArray << "\t" << minDiff << "\t" << numBadPoints << "\n";
      }
    }
  }
  
  
  




  save_inter_sample_times(outFileName,goodSampleTiming);
  save_epsilon_times(outFileName,goodSampleTiming);
  
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	  time=inter_sample_times[dda][chan][capArray][inter_sample_index[dda][chan][capArray][sample]];
	  index=inter_sample_index[dda][chan][capArray][sample];
	  epsilon=epsilon_times[dda][chan][capArray];
	  binWidthsTree->Fill();
	}//sample
      }//capArray
    }//chan
  }//dda



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

TGraph *getTwoBlockGraph(TGraph *fullEventGraph, Int_t block){
  Int_t numSamples = fullEventGraph->GetN();
  Int_t numBlocks = numSamples / SAMPLES_PER_BLOCK;
  if(block >= numBlocks-1) return NULL;
  Double_t *fullX = fullEventGraph->GetX();
  Double_t *fullY = fullEventGraph->GetY();  
  Double_t *blockX = new Double_t[SAMPLES_PER_BLOCK*2];
  Double_t *blockY = new Double_t[SAMPLES_PER_BLOCK*2];
  for(int sample=0;sample<SAMPLES_PER_BLOCK*2; sample++){
    blockY[sample] = fullY[sample + block*SAMPLES_PER_BLOCK];
    blockX[sample] = fullX[sample];
  }
  TGraph *blockGraph = new TGraph(SAMPLES_PER_BLOCK*2, blockX, blockY);
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
  }
  TGraph *halfGraph = new TGraph(numSamples/2, newX, newY);
   
  delete newX;
  delete newY;
  return halfGraph;
  
}

TGraph *getHalfGraphTwoBlocks(TGraph *fullGraph, Int_t half){
  Int_t numSamples = fullGraph->GetN();
  Double_t *xFull  = fullGraph->GetX();
  Double_t *yFull  = fullGraph->GetY();

  if(numSamples != 2*SAMPLES_PER_BLOCK){
    fprintf(stderr, "Wrong number of samples got %d expected %d\n", numSamples, SAMPLES_PER_BLOCK);
    return NULL;
  }

  Double_t *newX = new Double_t[SAMPLES_PER_BLOCK];
  Double_t *newY = new Double_t[SAMPLES_PER_BLOCK];

  for(Int_t sample=0;sample<2*SAMPLES_PER_BLOCK;sample++){
    if(sample%2!=half) continue;
    newX[sample/2]=xFull[sample];
    newY[sample/2]=yFull[sample];
  }
  TGraph *halfGraph = new TGraph(SAMPLES_PER_BLOCK, newX, newY);
   
  delete newX;
  delete newY;
  return halfGraph;
  
}



TGraph* apply_bin_calibration_half(TGraph* grBlock, Int_t capArray, Int_t dda, Int_t chan, Int_t half){
  Int_t numSamples = grBlock->GetN();
  if(numSamples!=SAMPLES_PER_BLOCK/2){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK/2);
    return NULL;

  }
  
  Double_t *yVals = grBlock->GetY();
  Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK/2];
  
  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
    xVals[sample] = half_inter_sample_times[dda][chan][capArray][half][sample] + epsilon_times_half[dda][chan][capArray][half];  //RJN do we need to add the epsilon???
  }//sample
  
  TGraph *grHalfCalibrated = new TGraph(SAMPLES_PER_BLOCK/2, xVals, yVals);
  delete xVals;

  return grHalfCalibrated;
}


TGraph* apply_bin_calibration(TGraph* grBlock, Int_t capArray, Int_t dda, Int_t chan){
  Int_t numSamples = grBlock->GetN();
  if(numSamples!=SAMPLES_PER_BLOCK){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK);
    return NULL;

  }
  
  Double_t *yValsIn = grBlock->GetY();
  Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK];
  Double_t *yVals = new Double_t[SAMPLES_PER_BLOCK];
  
  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK;sample++){
    int index=inter_sample_index[dda][chan][capArray][sample];
    xVals[sample] = inter_sample_times[dda][chan][capArray][index] + epsilon_times[dda][chan][capArray];  //RJN do we need to add the epsilon???
    yVals[sample] = yValsIn[index];
  }//sample
  //FIXME -- need to take into account the ordering of samples
  //Maybe make a note in the calibration file name
  
  TGraph *grBlockCalibrated = new TGraph(SAMPLES_PER_BLOCK, xVals, yVals);
  delete xVals;
  delete yVals;

  return grBlockCalibrated;
}

TGraph* apply_bin_calibration_two_blocks(TGraph* grBlock, Int_t capArray, Int_t dda, Int_t chan){
  //RJN -- Note this will not work if inter_sample_index is not in order as it doesn't switch the yVals at the same time
  //Will come back and fix this if necessary

  Int_t numSamples = grBlock->GetN();
  if(numSamples!=SAMPLES_PER_BLOCK*2){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK*2);
    return NULL;

  }
  
  Double_t *yVals = grBlock->GetY();
  Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK*2];
  
  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK;sample++){
    xVals[sample] = inter_sample_times[dda][chan][capArray][inter_sample_index[dda][chan][capArray][sample]];
  }//sample

  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK;sample++){
    xVals[sample+SAMPLES_PER_BLOCK] = inter_sample_times[dda][chan][1-capArray][inter_sample_index[dda][chan][1-capArray][sample]];
  }//sample
  
  TGraph *grBlockCalibrated = new TGraph(2*SAMPLES_PER_BLOCK, xVals, yVals);
  delete xVals;

  return grBlockCalibrated;
}

Int_t save_inter_sample_times(char* name, int goodSampleTiming[DDA_PER_ATRI][RFCHAN_PER_DDA]){

  char outName[180];
  sprintf(outName, "%s_sample_timing.txt", name);
  std::ofstream OutFile(outName);
  Int_t capArray, sample;
  
  int lastGoodChan=-1;
  for(int dda=0;dda<DDA_PER_ATRI;dda++){
    for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(int capArray=0;capArray<2;capArray++) {
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t"  << SAMPLES_PER_BLOCK << "\t";   
	
	int useDda=dda;
	int useChan=chan;
	if(goodSampleTiming[dda][chan]) lastGoodChan=chan;
	else {
	  useChan=lastGoodChan;
	  std::cout << "Replacing " << dda << "," << chan << " with " << useDda << "," << useChan << "\n";
	}

	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
	  //Index values
	  OutFile << inter_sample_index[useDda][useChan][capArray][sample] << " ";
	}
	OutFile << "\n";
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t"  << SAMPLES_PER_BLOCK << "\t";   
	for(int sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
	  //time values
	    OutFile << inter_sample_times[useDda][useChan][capArray][inter_sample_index[useDda][useChan][capArray][sample]] << " ";
	}
	OutFile << "\n";
      }
    }
  }
  OutFile.close();

  return 0;
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
      //      if(zc<0.6) //printf("sample %i y1 %f y2 %f x1 %f x2 %f\n", sample, y1, y2, x1, x2);
      phase+=zc-numZCs*period;
      //printf("zc num %i val %f adjusted val %f\n", numZCs, zc, zc-numZCs*period);
      numZCs++;
      //return zc;
    }
  }//sample

  if(!numZCs)
    phase=0;
  else phase=phase/numZCs;
  
  *totalZCs = numZCs;
  *meanPhase = phase;
  return 0;
  
  
}

Int_t estimate_phase_two_blocks(TGraph *gr, Double_t period, Double_t *meanPhase, Int_t *totalZCs){
  Double_t *yVals = gr->GetY();
  Double_t *xVals = gr->GetX();
  Int_t numSamples = gr->GetN();
  Double_t phase=0;
  Int_t numZCs=0;


  Double_t zcArray[1000]={0};
  Int_t countZC=0;
  
  for(int sample=0;sample<numSamples-1;sample++){
    Double_t y1=yVals[sample];
    Double_t y2=yVals[sample+1];
    if(y1<0 && y2>0){
      Double_t x1=xVals[sample]; 
      Double_t x2=xVals[sample+1]; 
      Double_t zc=((0-y1)/(y2-y1))*(x2-x1)+x1;
      //      if(zc<0.6) //printf("sample %i y1 %f y2 %f x1 %f x2 %f\n", sample, y1, y2, x1, x2);
      zcArray[countZC]=zc;
      countZC++;
      phase+=zc-numZCs*period;
      //printf("zc num %i val %f adjusted val %f\n", numZCs, zc, zc-numZCs*period);
      numZCs++;
      //return zc;
    }
  }//sample


  Double_t meanZC=0;
  
  if(countZC>0) {
    Double_t firstZC=zcArray[0];
    while(firstZC>period) firstZC-=period;
    for(int i=0;i<countZC;i++) {
      while((zcArray[i]-firstZC)>period) zcArray[i]-=period;
      if(TMath::Abs((zcArray[i]-period)-firstZC)<TMath::Abs(zcArray[i]-firstZC))
	zcArray[i]-=period;
      meanZC+=zcArray[i];
      //     std::cout << i << "\t" << zc[i] << "\n";     
    }
    meanZC/=countZC;
  }
  *totalZCs = countZC;
  *meanPhase = meanZC;
  return 0;
}


Int_t findFirstZC(TGraph *grIn, Double_t period, Double_t *firstAvZC){


  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t *tVals=grIn->GetX();
  Double_t *vVals=grIn->GetY();

  Double_t zc[1000]={0};
  Double_t rawZc[1000]={0};
  int countZC=0;
  for(int i=2;i<numPoints;i++) {
    if(vVals[i-1]<0 && vVals[i]>0) {
      Double_t x1=tVals[i-1];
      Double_t x2=tVals[i];
      Double_t y1=vVals[i-1];
      Double_t y2=vVals[i];      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      zc[countZC]=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      rawZc[countZC]=zc[countZC];
      countZC++;
      //      if(countZC==1)
      //      break;
    }
  }

  Double_t firstZC=zc[0];
  while(firstZC>period) firstZC-=period;
  Double_t meanZC=0;
  for(int i=0;i<countZC;i++) {
     while((zc[i]-firstZC)>period) zc[i]-=period;
     if(TMath::Abs((zc[i]-period)-firstZC)<TMath::Abs(zc[i]-firstZC))
       zc[i]-=period;
     meanZC+=zc[i];
     //std::cout << i << "\t" << zc[i] << "\n";     
  }
  //  TCanvas *can = new TCanvas();
  //  TGraph *gr = new TGraph(countZC,rawZc,zc);
  //  gr->Draw("ap");

  //  std::cout << "\n";
  meanZC/=countZC;
  
  //  std::cout << zc << "\n";
  
  *firstAvZC=meanZC;
  return countZC;

}

Int_t findLastZC(TGraph *grIn, Double_t period, Double_t *lastAvZC){

 // This funciton estimates the lag by just using all the negative-positive zero crossing
 
  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t *tVals=grIn->GetX();
  Double_t *vVals=grIn->GetY();

  Double_t zc[1000]={0};
  Double_t rawZc[1000]={0};
  int countZC=0;
  for(int i=numPoints-1;i>1;i--) {
    if(vVals[i-1]<0 && vVals[i]>0) {
      Double_t x1=tVals[i-1];
      Double_t x2=tVals[i];
      Double_t y1=vVals[i-1];
      Double_t y2=vVals[i];      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      zc[countZC]=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      rawZc[countZC]=zc[countZC];
      countZC++;
      //      if(countZC==1)
      //      break;
    }
  }

  Double_t lastZC=zc[0];
  while(lastZC<(tVals[numPoints-1]-period)) lastZC+=period;
  Double_t meanZC=0;
  for(int i=0;i<countZC;i++) {
     while((lastZC-zc[i])>period) zc[i]+=period;
     // if(TMath::Abs((zc[i]-period)-firstZC)<TMath::Abs(zc[i]-firstZC))
     //   zc[i]-=period;]
     if((zc[i]+period-lastZC)<(lastZC-zc[i]))
       zc[i]+=period;
    
     meanZC+=zc[i];
     //     std::cout << i << "\t" << zc[i] << "\n";     
  }
  //  TCanvas *can = new TCanvas();
  //  TGraph *gr = new TGraph(countZC,rawZc,zc);
  //  gr->Draw("ap");

  //  std::cout << "\n";
  meanZC/=countZC;
  *lastAvZC=meanZC;

  //  std::cout << lastZC << "\t" << meanZC << "\n";
  return countZC;
  
}



Int_t save_epsilon_times(char* name,int goodSampleTiming[DDA_PER_ATRI][RFCHAN_PER_DDA]){

  char outName[180];
  sprintf(outName, "%s_epsilon_timing.txt", name);
  std::ofstream OutFile(outName);
  Int_t capArray, sample;

  int lastGoodChan=-1;
  for(Int_t dda=0;dda<DDA_PER_ATRI;dda++){
    for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(int capArray=0;capArray<2;capArray++){
	OutFile <<  dda << "\t"
		<< chan << "\t" 
		<< capArray << "\t";
	
	int useDda=dda;
	int useChan=chan;
	if(goodSampleTiming[dda][chan]) lastGoodChan=chan;
	else {
	  useChan=lastGoodChan;
	  //	  std::cout << "Replacing " << dda << "," << chan << " with " << useDda << "," << useChan << "\n";
	}
	
	
	OutFile << epsilon_times[useDda][useChan][capArray] << "\n";
      }
    }
  }
  OutFile.close();
 
  return 0;
 
}
