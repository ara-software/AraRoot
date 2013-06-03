////////////////////////////////////////////////////////////////////////
////   fourthCalibTry -- let's calibrate the ATRI stations!         ////
////   author - jonathan davies jdavies@hep.ucl.ac.uk               ////
////                                                                ////
////   calculate the timing calibrations for ATRI stations using    ////

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
Double_t inter_sample_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2][SAMPLES_PER_BLOCK]={{{{0}}}}; //[dda][chan][capArray][sample]
Double_t times_two_blocks_one_half[DDA_PER_ATRI][RFCHAN_PER_DDA][2][SAMPLES_PER_BLOCK]={{{{0}}}}; //[dda][chan][half][sample]
Double_t times_one_block_one_half[DDA_PER_ATRI][RFCHAN_PER_DDA][2][2][SAMPLES_PER_BLOCK/2]={{{{{0}}}}}; //[dda][chan][half][capArray][sample]
Double_t epsilon_times[DDA_PER_ATRI][RFCHAN_PER_DDA][2]={{{0}}}; //[dda][chan][capArray] capArray = 0 is cap1 -> cap0


//Prototype Functions
TGraph* zeroMean(TGraph*);
Int_t estimate_phase(TGraph*, Double_t, Double_t*, Int_t*);
Int_t estimate_phase_two_blocks(TGraph*, Double_t, Double_t*, Int_t*);
TGraph *getBlockGraph(TGraph*, Int_t);
TGraph *getTwoBlockGraph(TGraph*, Int_t);

Int_t findLastZC(TGraph*, Double_t, Double_t*);
Int_t findFirstZC(TGraph*, Double_t, Double_t*);

TGraph* getHalfGraph(TGraph*, Int_t);
TGraph* getHalfGraphTwoBlocks(TGraph*, Int_t);

TGraph *applyCalibToHalfGraphTwoBlocks(TGraph*, Int_t, Int_t, Int_t);
TGraph *applyCalibToHalfGraphOneBlock(TGraph*, Int_t, Int_t, Int_t, Int_t);

Int_t calibrate(char*, Int_t, Int_t, Double_t, bool);


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
  sprintf(baseName, "%s", argv[1]);
  runNum=atoi(argv[2]);
  pedNum=atoi(argv[3]);
  freq=atof(argv[4]);
  if(argc>5){
    if(atoi(argv[5])) debug=true;
  }

  return calibrate(baseName, runNum, pedNum, freq, debug);

}


Int_t calibrate(char* baseDirName, Int_t runNum, Int_t pedNum, Double_t freq, bool debug=false){
  Double_t period=1./freq;
  Int_t chanIndex=0;
  Int_t dda=0, chan=0;
  char runFileName[FILENAME_MAX];
  char pedFileName[FILENAME_MAX];
  sprintf(runFileName, "%s/root/run%i/event%i.root", baseDirName, runNum, runNum);
  sprintf(pedFileName, "%s/raw_data/run_%06i/pedestalValues.run%06d.dat", baseDirName, pedNum, pedNum);

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
  sprintf(outFileName, "%s/root/run%i/calibFourthTryNoVoltageCalib.root", baseDirName, runNum);
  TFile *outFile = new TFile(outFileName, "RECREATE");
  Int_t capArray=0,thisCapArray=0, block=0,half=0,sample=0;
  Int_t numEventsCap0[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};
  Int_t numEventsCap1[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};
  Double_t epsilonEstCap1Cap0[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};

  //BinWidth Histos
  TH1D *histBinWidth[DDA_PER_ATRI][RFCHAN_PER_DDA][2]; 
  char histName[FILENAME_MAX];
  for(half=0;half<2;half++) {
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	sprintf(histName,"histBinWidth_dda%d_chan%d_%d",dda, chan,half);
	histBinWidth[dda][chan][half] = new TH1D(histName,histName,SAMPLES_PER_BLOCK,-0.5,SAMPLES_PER_BLOCK-0.5);
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
  Double_t epsilon_corrected=0;
  Double_t firstZC=0, lastZC=0;
  Double_t oneZCLast=0, oneZCFirst=0;
  Int_t firstZCCount=0, lastZCCount=0;
  Double_t epsilon2=0;
  Int_t atriBlock=0;
  TH1* histEpsilon[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}}; //[dda][chan];
  epsilonTree->Branch("dda",&dda,"dda/I");
  epsilonTree->Branch("chan",&chan,"chan/I");
  epsilonTree->Branch("block",&block,"block/I");
  epsilonTree->Branch("epsilon",&epsilon,"epsilon/D");
  epsilonTree->Branch("epsilon2",&epsilon2,"epsilon2/D");
  epsilonTree->Branch("epsilon_corrected",&epsilon_corrected,"epsilon_corrected/D");
  epsilonTree->Branch("firstZC",&firstZC,"firstZC/D");
  epsilonTree->Branch("lastZC",&lastZC,"lastZC/D");
  epsilonTree->Branch("oneZCLast",&oneZCLast,"oneZCLast/D");
  epsilonTree->Branch("oneZCFirst",&oneZCFirst,"oneZCFirst/D");


  epsilonTree->Branch("lastZCCount",&lastZCCount,"lastZCCount/I");
  epsilonTree->Branch("firstZCCount",&firstZCCount,"firstZCCount/I");
  epsilonTree->Branch("half", &half, "half/I");
  epsilonTree->Branch("atriBlock", &atriBlock, "atriBlock/I");
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      sprintf(histName,"epsilon_hist_dda%i_chan%i",dda, chan);
      histEpsilon[dda][chan] = new TH1F(histName,histName,600,-3.0,3.0);
    }
  }

  //Check Epsilon Tree
  TTree *checkEpsilonTree = new TTree("checkEpsilonTree", "checkEpsilonTree");
  Double_t zcEst=0;
  Double_t zcMeas=0;
  Int_t firstBlock=0;
  Int_t zcNo=0;
  Int_t missedOne=0;
  Int_t firstBadZC=0;
  checkEpsilonTree->Branch("dda", &dda, "dda/I");
  checkEpsilonTree->Branch("chan", &chan, "chan/I");
  checkEpsilonTree->Branch("block", &block, "block/I");
  checkEpsilonTree->Branch("firstBlock", &firstBlock, "firstBlock/I");
  checkEpsilonTree->Branch("zcNo", &zcNo, "zcNo/I");
  checkEpsilonTree->Branch("capArray", &capArray, "capArray/I");
  checkEpsilonTree->Branch("sample", &sample, "sample/I");
  checkEpsilonTree->Branch("half", &half, "half/I");
  checkEpsilonTree->Branch("firstBadZC", &firstBadZC, "firstBadZC/I");

  checkEpsilonTree->Branch("zcEst", &zcEst ,"zcEst/D");
  checkEpsilonTree->Branch("zcMeas", &zcMeas ,"zcMeas/D");
  checkEpsilonTree->Branch("firstZC", &firstZC ,"firstZC/D");

  


  //BinWidth Calibration
  //numEntries=1;//FIXME
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);


    
    for(dda=0;dda<DDA_PER_ATRI;dda++){
      capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	chanIndex=chan+RFCHAN_PER_DDA*dda;
	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
	TGraph *grZeroMean = zeroMean(gr);
	Int_t numSamples = grZeroMean->GetN();
	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
		
	for(block=0; block<numBlocks-1; block++){ 
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  if(thisCapArray==0){
	    TGraph *grTwoBlock = getTwoBlockGraph(grZeroMean, block);
	    numEventsCap0[dda][chan]++;
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
		// else if(val1==0 || val2==0){
		//   histBinWidth[dda][chan][half]->Fill(sample, 0.5);
		// }
	      }//sample
	      if(grHalf) delete grHalf;
	    }//half      
	    if(grTwoBlock) delete grTwoBlock;
	  }
	  // else{//thisCapArray==1 //This is sanity check for the epsilon calculation
	  //   numEventsCap1[dda][chan]++;
	  //   TGraph *grTwoBlock = getTwoBlockGraph(grZeroMean, block);	    
	  //   TGraph *grHalf = getHalfGraphTwoBlocks(grTwoBlock, 0);
	  //   Double_t *yVals = grHalf->GetY();
	  //   Double_t val1 = yVals[SAMPLES_PER_BLOCK/2-1];//last sample not necessarily before first one use sample 30 to 0 not 31 to 0
	  //   Double_t val2 = yVals[SAMPLES_PER_BLOCK/2];
	  //   if(val1<0 && val2>0){
	  //     epsilonEstCap1Cap0[dda][chan]+=1;
	  //   }
	  //   else if(val1>0 && val2<0){
	  //     epsilonEstCap1Cap0[dda][chan]+=1;
	  //   }
	  //   else if(val1==0 || val2==0){
	  //     epsilonEstCap1Cap0[dda][chan]+=0.5;
	  //   }
	  //   if(grHalf) delete grHalf;
	  //   if(grTwoBlock) delete grTwoBlock;
	  // }
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
	histBinWidth[dda][chan][half]->Scale(1./numEventsCap0[dda][chan]);
	histBinWidth[dda][chan][half]->Scale(0.5*period);
	histBinWidth[dda][chan][half]->Write();
      }//half
      Double_t numZCs=epsilonEstCap1Cap0[dda][chan];
      epsilonEstCap1Cap0[dda][chan]=(numZCs / numEventsCap1[dda][chan])*(0.5*period);
      //      epsilonEstCap1Cap0[dda][chan]-=histBinWidth[dda][chan][0]->GetBinContent(SAMPLES_PER_BLOCK-2+1);//Take off the gap from sample 30 to 31
      //      printf("%f\n",  epsilonEstCap1Cap0[dda][chan]);

    }//chan
  }//dda

  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(half=0;half<2;half++){
	Double_t time=0;
	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	  times_two_blocks_one_half[dda][chan][half][sample]=time;
	  time+=histBinWidth[dda][chan][half]->GetBinContent(sample+1);
	  // if(chan==0){
	  //   printf("%i %i %i %f %f\n", dda, chan, half, times_two_blocks_one_half[dda][chan][half][sample], time);
	  // }
	}//sample
      }//half
    }//chan
  }//dda

  Int_t goodSampleTiming[DDA_PER_ATRI][RFCHAN_PER_DDA]={{1,1,1,1,1,1,0,0},{1,1,1,1,0,0,0,0},{1,1,1,1,0,0,0,0},{1,1,1,1,1,1,0,0}};

  //CheckBinWidths
  //numEntries=1;//FIXME
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);

    for(dda=0;dda<DDA_PER_ATRI;dda++){
      capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
      for(chan=0;chan<RFCHAN_PER_DDA;chan++){
	chanIndex=chan+RFCHAN_PER_DDA*dda;
	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
	TGraph *grZeroMean = zeroMean(gr);
	Int_t numSamples = grZeroMean->GetN();
	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	
	for(block=0; block<numBlocks-1; block++){ 
	  if(block%2) thisCapArray=1-capArray;
	  else thisCapArray=capArray;
	  if(thisCapArray!=0) continue;
	  TGraph *grTwoBlock = getTwoBlockGraph(grZeroMean, block);
	  for(half=0;half<2;half++){
	    TGraph *grTwoBlockHalf = getHalfGraphTwoBlocks(grTwoBlock, half);
	    Double_t *yVals = grTwoBlockHalf->GetY();
	    zcNo=0;
	    
	    for(sample=0;sample<SAMPLES_PER_BLOCK-1;sample++){
	      Double_t y1=yVals[sample];
	      Double_t y2=yVals[sample+1];
	      Double_t x1=times_two_blocks_one_half[dda][chan][half][sample];
	      Double_t x2=times_two_blocks_one_half[dda][chan][half][sample+1];
	      if((y1<0 && y2>0)){
		if(zcNo==0){
		  firstZC=((0-y1)/(y2-y1))*(x2-x1)+x1;
		}
		zcMeas=((0-y1)/(y2-y1))*(x2-x1)+x1;
		zcEst=firstZC+period*1.*zcNo;
		checkEpsilonTree->Fill();
		zcNo++;
	      }
	    }
	    if(grTwoBlockHalf) delete grTwoBlockHalf;
	  }




	  if(grTwoBlock) delete grTwoBlock;
	}//block
	
	
	if(grZeroMean) delete grZeroMean;
	if(gr) delete gr;
      }//chan
    }//dda
  }
  std::cout << std::endl;





  //Interleave
  //  numEntries=1;//FIXME
  // for(int entry=0;entry<numEntries;entry++){
  //   if(entry%starEvery==0) std::cerr <<"*";
  //   eventTree->GetEntry(entry);
  //   UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);

    
  //   for(dda=0;dda<DDA_PER_ATRI;dda++){
  //     capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
  //     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  // 	chanIndex=chan+RFCHAN_PER_DDA*dda;
  // 	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
  // 	TGraph *grZeroMean = zeroMean(gr);
  // 	Int_t numSamples = grZeroMean->GetN();
  // 	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
  // 	for(block=0; block<numBlocks-1; block++){
  // 	  if(block%2) thisCapArray=1-capArray;
  // 	  else thisCapArray=capArray;
  // 	  if(thisCapArray==1) continue;
  // 	  TGraph *grTwoBlocks = getTwoBlockGraph(grZeroMean, block);
  // 	  TGraph *grTwoBlocksEvenHalf = getHalfGraphTwoBlocks(grTwoBlocks, 0);
  // 	  TGraph *grTwoBlocksOddHalf = getHalfGraphTwoBlocks(grTwoBlocks, 1);

  // 	  //Now apply the bin calib to produce calibrated TGraphs
  // 	  TGraph *grCalibEven = applyCalibToHalfGraphTwoBlocks(grTwoBlocksEvenHalf, dda, chan, 0);
  // 	  TGraph *grCalibOdd = applyCalibToHalfGraphTwoBlocks(grTwoBlocksOddHalf, dda, chan, 1);

  // 	  //Estimate phase
  // 	  Int_t retVal = estimate_phase_two_blocks(grCalibEven, period, &lag0, &noZCs[0]);
  // 	  if(retVal==0){
  // 	    retVal = estimate_phase_two_blocks(grCalibOdd, period, &lag1, &noZCs[1]);
  // 	    if(retVal==0){
  // 	      deltaLag = lag0-lag1;//With no interleave done the half that is later in time has a smaller lag
  // 	      while(TMath::Abs(deltaLag-period)<TMath::Abs(deltaLag))
  // 		deltaLag-=period;
  // 	      while(TMath::Abs(deltaLag+period)<TMath::Abs(deltaLag))
  // 		deltaLag+=period;
  // 	      lagTree->Fill();
  // 	      if(TMath::Abs(noZCs[0]-noZCs[1])==0) lagHist[dda][chan]->Fill(deltaLag);
  // 	    }
  // 	  }
  // 	  if(grCalibEven) delete grCalibEven;
  // 	  if(grCalibOdd) delete grCalibOdd;
  // 	  if(grTwoBlocks) delete  grTwoBlocks;
  // 	  if(grTwoBlocksEvenHalf) delete grTwoBlocksEvenHalf;
  // 	  if(grTwoBlocksOddHalf) delete grTwoBlocksOddHalf;
  // 	}//block
  // 	if(grZeroMean) delete grZeroMean;
  // 	if(gr) delete gr;
  //     }//chan
  //   }//dda
  // }//entry
  // std::cerr << "\n";

  // //Calculate lag

  // Int_t useAllSamples[DDA_PER_ATRI][RFCHAN_PER_DDA]={{0}};
  // for(dda=0;dda<DDA_PER_ATRI;dda++){
  //   for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  //     if(chan<2) useAllSamples[dda][chan]=1;
  //     lag[dda][chan] = lagHist[dda][chan]->GetMean(1);
  //     if((lagHist[dda][chan]->GetRMS())>0.1&&chan<6) {
  // 	printf("dda %i chan %i rms %f\n", dda, chan, lagHist[dda][chan]->GetRMS());
  // 	goodSampleTiming[dda][chan]=0;
  //     }    
  //     else {
  // 	goodSampleTiming[dda][chan]=1;
  //     }
  //     //printf("dda %i chan %i lag %f\n", dda, chan ,lag[dda][chan]);	
  //   }//chan
  // }//dda

  // //Now adjust the sample timing with the calculated lag
  // for(dda=0;dda<DDA_PER_ATRI;dda++){
  //   for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  //     for(half=0;half<2;half++){
  // 	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
  // 	  // if(lag[dda][chan]>0) {
  // 	  //   Double_t time=times_two_blocks_one_half[dda][chan][half][sample];
  // 	  //   times_two_blocks_one_half[dda][chan][half][sample]=time+(lag[dda][chan])*half;
  // 	  // }
  // 	  // else {
  // 	  //   Double_t time=times_two_blocks_one_half[dda][chan][half][sample];
  // 	  //   times_two_blocks_one_half[dda][chan][half][sample]=time-(lag[dda][chan])*(1-half);
  // 	  // }
  // 	  times_two_blocks_one_half[dda][chan][half][sample]+=half*lag[dda][chan];


  // 	}//sample
  //     }//half
  //   }//chan
  // }//dda
  
  // //Now make sample timing for each cap Array
  // for(dda=0;dda<DDA_PER_ATRI;dda++){
  //   for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  //     for(half=0;half<2;half++){
  // 	for(capArray=0;capArray<2;capArray++){
  // 	  Double_t offset=0;
  // 	  // if(capArray==1){
  // 	  //   offset=-1.*(times_two_blocks_one_half[dda][chan][half][SAMPLES_PER_BLOCK/2]);
  // 	  // }
  // 	  for(sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
  // 	    times_one_block_one_half[dda][chan][half][capArray][sample]=times_two_blocks_one_half[dda][chan][half][sample+(capArray*SAMPLES_PER_BLOCK/2)]+offset;
  // 	  }//sample
  // 	}//capArray
  //     }//half
  //   }//chan
  // }//dda
  

  // //Epsilon
  // //  numEntries=1;//FIXME
  // for(int entry=0;entry<numEntries;entry++){
  //   if(entry%starEvery==0) std::cerr <<"*";
  //   eventTree->GetEntry(entry);
  //   UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);

  //   for(dda=0;dda<DDA_PER_ATRI;dda++){
  //     capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
  //     atriBlock = evPtr->blockVec[dda].getBlock();

  //     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  // 	chanIndex=chan+RFCHAN_PER_DDA*dda;
	
  // 	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
  // 	TGraph *grZeroMean = zeroMean(gr);
  // 	Int_t numSamples = grZeroMean->GetN();
  // 	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
	
  // 	for(block=0; block<numBlocks-1; block++){ 
  // 	  if(block%2) thisCapArray=1-capArray;
  // 	  else thisCapArray=capArray;
  // 	  if(thisCapArray==0) continue; //We've already done this one -- it was done as part of the binWidths
  // 	  TGraph *grBlockOne = getBlockGraph(grZeroMean, block);
  // 	  TGraph *grBlockTwo = getBlockGraph(grZeroMean, block+1);
  // 	  TGraph *grBlockOneEven = getHalfGraph(grBlockOne, 0);
  // 	  TGraph *grBlockTwoEven = getHalfGraph(grBlockTwo, 0);
  // 	  TGraph *grBlockOneEvenCalib = applyCalibToHalfGraphOneBlock(grBlockOneEven, dda, chan, 1, 0); //block one is capArray 1
  // 	  TGraph *grBlockTwoEvenCalib = applyCalibToHalfGraphOneBlock(grBlockTwoEven, dda, chan, 0, 0); //block two is capArray 0
	  
  // 	  lastZCCount = findLastZC(grBlockOneEvenCalib, period, &lastZC);
  // 	  firstZCCount = findFirstZC(grBlockTwoEvenCalib, period, &firstZC);
  // 	  Double_t *tVals = grBlockOneEvenCalib->GetX(); 
  // 	  Double_t lastSample = tVals[SAMPLES_PER_BLOCK/2-1];
  // 	  //epsilon = (firstZC+lastSample)-lastZC-period;//jpd - surely this is correct? -ve epsilon means first sample before last
  // 	  epsilon = -1.*((firstZC+lastSample)-lastZC-period);
  // 	  epsilon_corrected=epsilon;
  // 	  while(epsilon_corrected < -0.5*period) epsilon_corrected+=period;
  // 	  while(epsilon_corrected > +0.5*period) epsilon_corrected-=period;
  // 	  epsilonTree->Fill();
  // 	  if(TMath::Abs(lastZCCount-firstZCCount)==0) histEpsilon[dda][chan]->Fill(epsilon_corrected);
  // 	  /*
	    
  // 	   */



  // 	  if(grBlockOne) delete grBlockOne;
  // 	  if(grBlockTwo) delete grBlockTwo;
  // 	  if(grBlockOneEven) delete grBlockOneEven;
  // 	  if(grBlockTwoEven) delete grBlockTwoEven;
  // 	  if(grBlockOneEvenCalib) delete grBlockOneEvenCalib;
  // 	  if(grBlockTwoEvenCalib) delete grBlockTwoEvenCalib;

  // 	}//block
  // 	if(gr) delete gr;
  // 	if(grZeroMean) delete grZeroMean;
  //     }//chan
  //   }//dda
  // }//entry
  // std::cout << std::endl;

  // //Calculate Epsilon
  // for(dda=0;dda<DDA_PER_ATRI;dda++){
  //   for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  //     if(goodSampleTiming[dda][chan]==1){
  // 	Double_t lastTime=times_two_blocks_one_half[dda][chan][0][31];
  // 	Double_t firstTime=times_two_blocks_one_half[dda][chan][0][32];
  // 	epsilon_times[dda][chan][1]=firstTime-lastTime;
  // 	epsilon_times[dda][chan][0]=histEpsilon[dda][chan]->GetMean(1);
  // 	printf("dda %i chan %i epsilon %f %f %f\n", dda, chan, epsilon_times[dda][chan][0], epsilon_times[dda][chan][1],epsilonEstCap1Cap0[dda][chan] );
  //     }
  //   }//chan
  // }//dda
  
  // //Check Calibration
  // for(int entry=0;entry<numEntries;entry++){
  //   if(entry%starEvery==0) std::cerr <<"*";
  //   eventTree->GetEntry(entry);
  //   UsefulAtriStationEvent realEvent(evPtr, AraCalType::kJustPed);

  //   for(dda=0;dda<DDA_PER_ATRI;dda++){
  //     capArray = evPtr->blockVec[dda].getCapArray(); //capArray of first block
  //     atriBlock = evPtr->blockVec[dda].getBlock();
  //     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
  // 	chanIndex=chan+RFCHAN_PER_DDA*dda;
  // 	TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
  // 	TGraph *grZeroMean = zeroMean(gr);
  // 	Int_t numSamples = grZeroMean->GetN();
  // 	Int_t numBlocks = numSamples/SAMPLES_PER_BLOCK;
  // 	Int_t gotFirstBlock=0;
  // 	zcNo=0;
  // 	missedOne=0;
  // 	firstBadZC=0;
  // 	Double_t lastSampleVal=0;

  // 	for(block=0; block<numBlocks-1; block++){ 
  // 	  if(block%2) thisCapArray=1-capArray;
  // 	  else thisCapArray=capArray;
  // 	  if(thisCapArray==1) continue;
  // 	  TGraph *grTwoBlocks = getTwoBlockGraph(grZeroMean, block);
  // 	  TGraph *grHalf = getHalfGraphTwoBlocks(grTwoBlocks, 0);
  // 	  Double_t *yVals = grHalf->GetY();
  // 	  if(!gotFirstBlock){
  // 	    gotFirstBlock=1;
  // 	    firstBlock=block;
  // 	    for(sample=0;sample<SAMPLES_PER_BLOCK-1;sample++){
  // 	      Double_t y1=yVals[sample];
  // 	      Double_t y2=yVals[sample+1];
  // 	      Double_t x1=times_two_blocks_one_half[dda][chan][0][sample];
  // 	      Double_t x2=times_two_blocks_one_half[dda][chan][0][sample+1];
  // 	      if((y1<0 && y2>0)){//|| (y1>0 && y2<0) || (y1==0 || y2==0)){
  // 		firstZC=((0-y1)/(y2-y1))*(x2-x1)+x1;
  // 		break;//FIXME does this actually break?
  // 	      }
  // 	    }//sample
  // 	  }
  // 	  // if(lastSampleVal<0 && yVals[0]>0){
  // 	  //   zcNo++; //~64th of the time we miss a zc between end of last "double-block" and begninig of next
  // 	  //   missedOne=1;
  // 	  // }
  // 	  for(sample=0;sample<SAMPLES_PER_BLOCK-1;sample++){//FIXME start at second sample
  // 	    Double_t offset=(times_two_blocks_one_half[dda][chan][0][SAMPLES_PER_BLOCK-1]+epsilon_times[dda][chan][0])*((block-firstBlock)/2);
  // 	    Double_t y1=yVals[sample];
  // 	    Double_t y2=yVals[sample+1];
  // 	    Double_t x1=times_two_blocks_one_half[dda][chan][0][sample]+offset;
  // 	    Double_t x2=times_two_blocks_one_half[dda][chan][0][sample+1]+offset;
	    
  // 	    if((y1<0 && y2>0)){// || (y1>0 && y2<0) || (y1==0 |y2==0)){
  // 	      zcMeas=((0-y1)/(y2-y1))*(x2-x1)+x1;
  // 	      //	      zcEst=firstZC+period*0.5*zcNo;
  // 	      zcEst=firstZC+period*1*zcNo;
  // 	      if(TMath::Abs(zcMeas-zcEst) > 0.5*period&&firstBadZC==0){
  // 		firstBadZC=zcNo;
  // 	      }
	      
  // 	      checkEpsilonTree->Fill();
  // 	      zcNo++;
	      
  // 	    }
  // 	  }//sample
  // 	  lastSampleVal=yVals[SAMPLES_PER_BLOCK-1];
  // 	  if(grHalf) delete grHalf;
  // 	  if(grTwoBlocks) delete grTwoBlocks;
  // 	}//block
  // 	if(grZeroMean) delete grZeroMean;
  // 	if(gr) delete gr;
  //     }//chan
  //   }//dda
  // }//entry
  // std::cout << std::endl;
  

  
  char interSampleTimes[200];
  sprintf(interSampleTimes, "%s_interSampleTimes.txt", outFileName);
  std::ofstream interSampleTimesFile(interSampleTimes);
  int lastGoodChan=0;
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(half=0;half<2;half++){
  	interSampleTimesFile << dda << "\t" << chan << "\t" << half << "\t"  << SAMPLES_PER_BLOCK << "\t";   
  	int useDda=dda;
  	int useChan=chan;

  	if(goodSampleTiming[dda][chan]) lastGoodChan=chan;
  	else {
  	  useChan=lastGoodChan;
  	  std::cout << "Replacing " << dda << "," << chan << " with " << useDda << "," << useChan << "\n";
  	}

  	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
  	  //Index values
  	  interSampleTimesFile << sample*2+half << " ";
  	}
  	interSampleTimesFile << "\n";
  	interSampleTimesFile << dda << "\t" << chan << "\t" << half << "\t"  << SAMPLES_PER_BLOCK << "\t";   
  	for(int sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
  	  //time values
  	  interSampleTimesFile << times_two_blocks_one_half[dda][chan][half][sample] << " ";
  	}
  	interSampleTimesFile << "\n";
      }
    }
  }
  interSampleTimesFile.close();

  
  
  
  
  
  
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
    blockX[sample] = fullX[sample + block*SAMPLES_PER_BLOCK];
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
    blockX[sample] = fullX[sample + block*SAMPLES_PER_BLOCK];
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
  for(int i=1;i<numPoints;i++) {
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

TGraph *applyCalibToHalfGraphTwoBlocks(TGraph* gr, Int_t dda, Int_t chan, Int_t half){

  if(half<0 || half >1){

    fprintf(stderr, "%s : half invalid %i \n", __FUNCTION__, half);
    return NULL;

  }

  Int_t numSamples = gr->GetN();
  if(numSamples!=SAMPLES_PER_BLOCK){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK);
    return NULL;

  }
  
  Double_t *yVals = gr->GetY();
  Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK];
  
  for(Int_t sample=0;sample<SAMPLES_PER_BLOCK;sample++){
    xVals[sample] = times_two_blocks_one_half[dda][chan][half][sample];
  }//sample
  
  TGraph *grHalfCalibrated = new TGraph(SAMPLES_PER_BLOCK, xVals, yVals);
  delete xVals;

  return grHalfCalibrated;
}

 TGraph *applyCalibToHalfGraphOneBlock(TGraph* gr, Int_t dda, Int_t chan, Int_t capArray, Int_t half){

   if(half<0 || half >1){

     fprintf(stderr, "%s : half invalid %i \n", __FUNCTION__, half);
     return NULL;

   }

   Int_t numSamples = gr->GetN();
   if(numSamples!=SAMPLES_PER_BLOCK/2){

     fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, SAMPLES_PER_BLOCK/2);
     return NULL;

   }
  
   Double_t *yVals = gr->GetY();
   Double_t *xVals = new Double_t[SAMPLES_PER_BLOCK/2];
  
   for(Int_t sample=0;sample<SAMPLES_PER_BLOCK/2;sample++){
     xVals[sample] = times_one_block_one_half[dda][chan][half][capArray][sample];
   }//sample
  
   TGraph *grHalfCalibrated = new TGraph(SAMPLES_PER_BLOCK/2, xVals, yVals);
   delete xVals;

   return grHalfCalibrated;
 }






