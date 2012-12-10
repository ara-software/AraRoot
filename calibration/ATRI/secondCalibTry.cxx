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
Double_t inter_sample_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2][2][32]={{{{{0}}}}}; //[dda][chan][capArray][half][sample]
Int_t inter_sample_index[DDA_PER_ATRI][RFCHAN_PER_DDA][2][2][32]={{{{{0}}}}}; //[dda][chan][capArray][half][sample]
Double_t epsilon_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}}; //[dda][chan][capArray]


//Prototype Functions
TGraph* zeroMean(TGraph*);
Int_t estimate_phase(TGraph*, Double_t, Double_t*, Int_t*);
TGraph *getBlockGraph(TGraph*, Int_t);

//Need to test these functions
TGraph* apply_bin_calibration(TGraph*, Int_t, Int_t, Int_t);
Int_t save_inter_sample_times(char*);
Int_t save_epsilon_times(char*);
Int_t findLastZC(TGraph*, Double_t, Double_t*);
Int_t findFirstZC(TGraph*, Double_t, Double_t*);

TGraph* getHalfGraph(TGraph*, Int_t);
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
  sprintf(pedFileName, "%s/raw_data/run_%06i/pedestalWidths.run%06d.dat", baseDirName, pedNum, pedNum);

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
  calib->setAtriPedFile(pedFileName, stationId);
  
  //General output stuff
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/run%i/binWidths.root", baseDirName, runNum);
  TFile *outFile = new TFile(outFileName, "RECREATE");
  Int_t capArray=0,thisCapArray=0, block=0,half=0,sample=0;
  Int_t numEvents[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}};

  //BinWidth Histos
  TH1F *histBinWidth[DDA_PER_ATRI][RFCHAN_PER_DDA][2][2]; 
  char histName[FILENAME_MAX];
  for(capArray=0;capArray<2;capArray++) {
    for(half=0;half<2;half++) {
      for(dda=0;dda<DDA_PER_ATRI;dda++){
	for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	  sprintf(histName,"histBinWidth_dda%d_chan%d_%d_%d",dda, chan, capArray,half);
	  histBinWidth[dda][chan][capArray][half] = new TH1F(histName,histName,SAMPLES_PER_BLOCK/2,-0.5,(SAMPLES_PER_BLOCK/2)-0.5);
	}
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
  lagTree->Branch("thisCapArray",&thisCapArray,"thisCapArray/I");
  lagTree->Branch("capArray",&capArray,"capArray/I");
  lagTree->Branch("lag1",&lag1,"lag1/D");
  lagTree->Branch("lag0",&lag0,"lag0/D");
  lagTree->Branch("deltaLag",&deltaLag,"deltaLag/D");
  Double_t lag[DDA_PER_ATRI][RFCHAN_PER_DDA][2] = {{{0}}};
  TH1F *lagHist[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}};
  for(capArray=0;capArray<2;capArray++) {
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	sprintf(histName,"lag_hist_dda%i_chan%i_capArray%i",dda, chan, capArray);
	lagHist[dda][chan][capArray] = new TH1F(histName,histName,200,-1.0,1.0);
      }
    }
  }

  

  //Epsilon Histos
  TTree *epsilonTree = new TTree("epsilonTree", "epsilonTree");
  Double_t epsilon=0;
  Double_t firstZC=0, lastZC=0;
  Int_t firstZCCount=0, lastZCCount=0;
  Int_t atriBlock=0;
  TH1* histEpsilon[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}};
  epsilonTree->Branch("dda",&dda,"dda/I");
  epsilonTree->Branch("chan",&chan,"chan/I");
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
  for(capArray=0;capArray<2;capArray++) {
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	sprintf(histName,"epsilon_hist_dda%i_chan%i_capArray%i",dda, chan, capArray);
	histEpsilon[dda][chan][capArray] = new TH1F(histName,histName,600,-3.0,3.0);
      }
    }
  }


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
	
	
	for(block=1; block<numBlocks; block++){ //FIXME -- Only use blocks > 0
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  TGraph *grBlock = getBlockGraph(grZeroMean, block);
	  numEvents[dda][chan][thisCapArray]++;
	  for(half=0;half<2;half++){
	    TGraph *grHalf = getHalfGraph(grBlock, half);
	    Double_t *yVals = grHalf->GetY();
	    for(sample=0;sample<(SAMPLES_PER_BLOCK)/2-1;sample++){
	      Double_t val1 = yVals[sample];
	      Double_t val2 = yVals[sample+1];
	      if(val1<0 && val2>0){
		histBinWidth[dda][chan][thisCapArray][half]->Fill(sample);
	      }
	      else if(val1>0 && val2<0){
		histBinWidth[dda][chan][thisCapArray][half]->Fill(sample);
	      }
	      else if(val1==0 || val2==0){
		histBinWidth[dda][chan][thisCapArray][half]->Fill(sample, 0.5);
	      }
	    }//sample
	    if(grHalf) delete grHalf;
	  }//half      
	  if(grBlock) delete grBlock;
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
      for(capArray=0;capArray<2;capArray++){
	for(half=0;half<2;half++){
	  histBinWidth[dda][chan][capArray][half]->Scale(1./numEvents[dda][chan][capArray]);
	  histBinWidth[dda][chan][capArray][half]->Scale(0.5*period);
	  histBinWidth[dda][chan][capArray][half]->Write();
	}//half
      }//capArray
    }//chan
  }//dda

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(half=0;half<2;half++){
	  Double_t time=0;
	  for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
	    time+=histBinWidth[dda][chan][capArray][half]->GetBinContent(sample);
	    inter_sample_times[dda][chan][capArray][half][sample]=time;
	    inter_sample_index[dda][chan][capArray][half][sample]=sample;
	    if(debug) printf("capArray %i half %i sample %i time %f\n", capArray, half, sample, time);
	  }//sample
	}//half
      }//capArray
    }//chan
  }//dda

  //Interleave Calibration
  if(debug) numEntries=10;
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
	
	for(block=1; block<numBlocks; block++){ //FIXME -- only use blocks>0
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  TGraph *grBlock = getBlockGraph(grZeroMean, block);
	  TGraph *grBlockCalibrated = apply_bin_calibration(grBlock, thisCapArray, dda, chan);
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
	      if(TMath::Abs(noZCs[0]-noZCs[1])==0) lagHist[dda][chan][thisCapArray]->Fill(deltaLag);
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
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	// 	sprintf(varexp, "deltaLag>>tempHist");
	// 	//	sprintf(selection, "thisCapArray==%i&&noZCs[0]==noZCs[1]&&dda==%i&&chan==%i", capArray, dda, chan);
	// 	sprintf(selection, "thisCapArray==%i&&dda==%i&&chan==%i", capArray, dda, chan);
	// 	sprintf(name, "lag_hist_dda_%i_chan%i_capArray_%i", dda, chan, capArray);
	// 	lagTree->Draw(varexp, selection);
	// 	lagHist[dda][chan][capArray] = (TH1*) gPad->GetPrimitive("tempHist")->Clone(name);
	// 	delete (TH1*) gPad->GetPrimitive("tempHist");
	// 	//	lagHist[dda][chan][capArray]->SetName(name);	
	// 	lagHist[dda][chan][capArray]->SetTitle(name);
	// 	printf("dda %i chan %i capArray %i lag_hist name %s\n", dda, chan, capArray, lagHist[dda][chan][capArray]->GetName());
	// 	outFile->cd();
	// 	lagHist[dda][chan][capArray]->Write();
	lag[dda][chan][capArray] = lagHist[dda][chan][capArray]->GetMean(1);
      }//capArray
    }//chan
  }//dda
  
  
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(half=0;half<2;half++){
	  Double_t time=0;
	  for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
	    inter_sample_times[dda][chan][capArray][half][sample]=inter_sample_times[dda][chan][capArray][half][sample]+lag[dda][chan][capArray]*half;
	    inter_sample_index[dda][chan][capArray][half][sample]=inter_sample_index[dda][chan][capArray][half][sample];
	  }//sample
	}//half
      }//capArray
    }//chan
  }//dda





 
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
	
	for(block=1; block<numBlocks-1; block++){ //FIXME -- only use blocks > 0
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  TGraph *grBlock0 = getBlockGraph(grZeroMean, block);
	  TGraph *grBlockCalibrated0 = apply_bin_calibration(grBlock0, thisCapArray, dda, chan);
	  TGraph *grBlock1 = getBlockGraph(grZeroMean, block+1);
	  TGraph *grBlockCalibrated1 = apply_bin_calibration(grBlock1, 1-thisCapArray, dda, chan);

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
	  if(TMath::Abs(lastZCCount-firstZCCount)==0) histEpsilon[dda][chan][thisCapArray]->Fill(epsilon);
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
      }//chan
    }//dda
  }//entry
  std::cerr << "\n";

  //zCalculate actual epsilon from Tree
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(capArray=0;capArray<2;capArray++){
	epsilon_times[dda][chan][capArray] = histEpsilon[dda][chan][capArray]->GetMean(1);
      }//capArray
    }//chan
  }//dda
  save_inter_sample_times(outFileName);
  save_epsilon_times(outFileName);
  
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

TGraph* apply_bin_calibration(TGraph* grBlock, Int_t capArray, Int_t dda, Int_t chan){
  Int_t numSamples = grBlock->GetN();
  if(numSamples!=SAMPLES_PER_BLOCK){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK);
    return NULL;

  }
  
  Double_t *yVals = grBlock->GetY();
  Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK];
  
  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK;sample++){
    xVals[sample] = inter_sample_times[dda][chan][capArray][sample%2][sample/2];
  }//sample
  //FIXME -- need to take into account the ordering of samples
  //Maybe make a note in the calibration file name
  
  TGraph *grBlockCalibrated = new TGraph(SAMPLES_PER_BLOCK, xVals, yVals);
  delete xVals;

  return grBlockCalibrated;
}
Int_t save_inter_sample_times(char* name){

  char outName[180];
  sprintf(outName, "%s_sample_timing.txt", name);
  std::ofstream OutFile(outName);
  Int_t capArray, sample;

  for(int dda=0;dda<DDA_PER_ATRI;dda++){
    for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(int capArray=0;capArray<2;capArray++) {
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t";   
	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
	  //Index values
	  if(sample%2==0)
	    OutFile << inter_sample_index[dda][chan][capArray][0][sample/2]*2 << " ";
	  if(sample%2)
	    OutFile << inter_sample_index[dda][chan][capArray][1][sample/2]*2+1 << " ";
	}
	OutFile << "\n";
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t";   
	for(int sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
	  //time values
	  if(sample%2==0)
	    OutFile << inter_sample_times[dda][chan][capArray][0][sample/2] << " ";
	  if(sample%2)
	    OutFile << inter_sample_times[dda][chan][capArray][1][sample/2] << " ";
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


Int_t save_epsilon_times(char* name){

  char outName[180];
  sprintf(outName, "%s_epsilon_timing.txt", name);
  std::ofstream OutFile(outName);
  Int_t capArray, sample;

  for(Int_t dda=0;dda<DDA_PER_ATRI;dda++){
    for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(int capArray=0;capArray<2;capArray++){
	OutFile <<  dda << "\t"
		<< chan << "\t" 
		<< capArray << "\t";
	OutFile << epsilon_times[dda][chan][capArray] << "\n";
      }
    }
  }
  OutFile.close();
 
  return 0;
 
}
