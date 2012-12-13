Double_t inter_sample_times[4][8][2][64]={{{{0}}}}; //[dda][chan][capArray][sample]
Int_t inter_sample_index[4][8][2][64]={{{{0}}}}; //[dda][chan][capArray][sample]


void calibChecker(Int_t runNum, Int_t pedNum, char *sampleTimeFileName, Double_t freq, Int_t dda=0, Int_t chan=0, Int_t event=0, Int_t doCalib=0){
  Double_t period=1./freq;
  char runFileName[300];
  char pedFileName[300];
  char baseDirName[300];
  sprintf(baseDirName, "/unix/ara/data/ntu2012/StationTwo");
  sprintf(runFileName, "%s/root/run%i/event%i.root", baseDirName, runNum, runNum);
  sprintf(pedFileName, "%s/raw_data/run_%06i/pedestalValues.run%06d.dat", baseDirName, pedNum, pedNum);
  TFile *runFile = new TFile(runFileName);

  TTree *eventTree = (TTree*) runFile->Get("eventTree");
  RawAtriStationEvent *rawEvPtr = 0;
  eventTree->SetBranchAddress("event", &rawEvPtr);
  AraStationId_t stationId=0;
  eventTree->GetEntry(0);
  stationId= rawEvPtr->stationId;
  printf("stationId %i\n", stationId);
  AraEventCalibrator *calib = AraEventCalibrator::Instance();
  calib->setAtriPedFile(pedFileName, 2);
  delete rawEvPtr;
  
  load_timing_calib(sampleTimeFileName);
  //Change this 
  eventTree->GetEntry(event);
  UsefulAtriStationEvent *useEvPtr = new UsefulAtriStationEvent(rawEvPtr, AraCalType::kJustPed);

  Int_t chanIndex=chan+8*dda;
  Int_t thisCapArray = useEvPtr->getFirstCapArray(0);
  TGraph *grChan0 = useEvPtr->getGraphFromRFChan(chanIndex);
  TGraph *grChan0ZM = zeroMean(grChan0);
  TGraph *grChan0Block0 = getBlockGraph(grChan0ZM, 0);
  TGraph *grBlock0Calibrated0;
  if(doCalib)  grBlock0Calibrated0 = apply_bin_calibration(grChan0Block0, thisCapArray, dda, chan);
  else grBlock0Calibrated0=grChan0Block0;
  TGraph *grBlock0CalibHalf0 = getHalfGraph(grBlock0Calibrated0,0);
  TGraph *grBlock0CalibHalf1 = getHalfGraph(grBlock0Calibrated0,1);
  grBlock0CalibHalf0->SetMarkerColor(kBlack);
  grBlock0CalibHalf0->SetLineColor(kBlack);
  grBlock0CalibHalf1->SetMarkerColor(kBlue);
  grBlock0CalibHalf1->SetLineColor(kBlue);
  grBlock0CalibHalf0->Draw("ALP");
  grBlock0CalibHalf1->Draw("PL");
  Double_t lag0=0, lag1=0;
  Int_t noZCs[2]={0};
  estimate_phase(grBlock0CalibHalf0, period, &lag0, &noZCs[0]);
  printf("phase %f noZCs %i\n", lag0, noZCs[0]);
  estimate_phase(grBlock0CalibHalf1, period, &lag1, &noZCs[1]);
  printf("phase %f noZCs %i\n", lag1, noZCs[1]);

  Double_t deltaLag = lag0-lag1;
  while(TMath::Abs(deltaLag-period)<TMath::Abs(deltaLag))
    deltaLag-=period;
  while(TMath::Abs(deltaLag+period)<TMath::Abs(deltaLag))
    deltaLag+=period;
  printf("deltaLag %f\n", deltaLag);


}

void load_timing_calib(char *fileName){

  std::ifstream inFile(fileName);
  Double_t time=0;
  Int_t dda=0,chan=0,capArray=0,index=0;
  while(inFile >> dda >> chan >> capArray){
    //    printf("%i\t%i\t%i\t", dda, chan, capArray);
    for(int sample=0;sample<64;sample++){
      inFile >> index;
      inter_sample_index[dda][chan][capArray][sample]=index;
      //      printf("%i ", index);
    }
    //    printf("\n");
    inFile >> dda >> chan >> capArray;
    for(int sample=0;sample<64;sample++){
      inFile >> time;
      inter_sample_times[dda][chan][capArray][sample]=time;
      //      printf("%f ", time);
    }
    //    printf("\n");
  }

  

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
  Int_t numBlocks = numSamples / 64;
  if(block > numBlocks) return NULL;
  Double_t *fullX = fullEventGraph->GetX();
  Double_t *fullY = fullEventGraph->GetY();  
  Double_t *blockX = new Double_t[64];
  Double_t *blockY = new Double_t[64];
  for(int sample=0;sample<64; sample++){
    blockY[sample] = fullY[sample + block*64];
    blockX[sample] = fullX[sample];
    //    printf("sample %i x %f y %f\n", sample, blockX[sample], blockY[sample]);
  }
  TGraph *blockGraph = new TGraph(64, blockX, blockY);
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
    if(sample%2!=half){
      //      printf("NOT USED - half %i sample %i x %f y %f\n", half, sample, xFull[sample], yFull[sample]);
      continue;
    }
    newX[sample/2]=xFull[sample];
    newY[sample/2]=yFull[sample];
    //    printf("half %i sample %i x %f y %f\n", half, sample, xFull[sample], yFull[sample]);

  }
  TGraph *halfGraph = new TGraph(numSamples/2, newX, newY);
  for(int sample=0;sample<numSamples/2;sample++){
    //    printf("sample %i newX[%i] %f newY[%i] %f\n", sample, sample, newX[sample], sample, newY[sample]);
  }
   
  delete newX;
  delete newY;
  return halfGraph;
  
}

TGraph* apply_bin_calibration(TGraph* grBlock, Int_t capArray, Int_t dda, Int_t chan){
  Int_t numSamples = grBlock->GetN();
  if(numSamples!=64){

    fprintf(stderr, "%s : wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, 64);
    return NULL;

  }
  
  Double_t *yVals = grBlock->GetY();
  Double_t *xVals = new Double_t[64];
  
  for(Int_t sample=0;sample<64;sample++){
    xVals[sample] = inter_sample_times[dda][chan][capArray][inter_sample_index[dda][chan][capArray][sample]];
    //    printf("sample %i x %f y %f\n", sample, xVals[sample], yVals[sample]);
  }//sample
  //FIXME -- need to take into account the ordering of samples
  //Maybe make a note in the calibration file name
  
  TGraph *grBlockCalibrated = new TGraph(64, xVals, yVals);
  delete xVals;

  return grBlockCalibrated;
}
Int_t save_inter_sample_times(char* name){

  char outName[180];
  sprintf(outName, "%s", name);
  std::ofstream OutFile(outName);
  Int_t capArray, sample;

  for(int dda=0;dda<4;dda++){
    for(int chan=0;chan<8;chan++){
      for(int capArray=0;capArray<2;capArray++) {
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t";   
	for(sample=0;sample<64;sample++) {
	  //Index values
	  OutFile << inter_sample_index[dda][chan][capArray][sample] << " ";
	}
	OutFile << "\n";
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t";   
	for(int sample=0;sample<64;sample++) {
	  //time values
	    OutFile << inter_sample_times[dda][chan][capArray][sample] << " ";
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
  if(numSamples != 64/2){
    fprintf(stderr, "%s : Wrong number of samples %i expected %i\n", __FUNCTION__, numSamples, 64/2);
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
      //      printf("sample %i y1 %f y2 %f x1 %f x2 %f zc%f\n", sample, y1, y2, x1, x2, zc);
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
