#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraEvent.h"
#include "RawAraEvent.h"
#include "araDefines.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include "TEventList.h"
#include "TMath.h"
#include <TGClient.h>

#include <zlib.h>

Double_t estimateLag(TGraph *grIn, Double_t freq);
Double_t estimateLagFirst(TGraph *grIn, Double_t freq);
Double_t estimateLagLast(TGraph *grIn, Double_t freq);
void loadCalib();
void loadPedestals();   
Double_t mySineWave(Double_t *t, Double_t *par) ;
int gotPedFile=0;
char pedFile[FILENAME_MAX];
float pedestalData[ACTIVE_CHIPS][CHANNELS_PER_CHIP][MAX_NUMBER_SAMPLES];
Double_t binWidths[ACTIVE_CHIPS][2][MAX_NUMBER_SAMPLES];

//int main(int argc, char *argv)
int plotEpsilonCal()
{
   //  TFile *fp = new TFile("/Users/rjn/ara/data/root/event_frozen_200MHz.root");
  TFile *fp = new TFile("/unix/anita1/ara/calibration/Minus54C/sine_wave_data/root/event200MHz_317mV.root");
  if(!fp) {
    std::cerr << "Can't open file\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fp->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't find eventTree\n";
    return -1;
  }
  RawAraEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);
  strcpy(pedFile,"/unix/anita1/ara/data/fromWisconsin/root/peds_1294924296.869787.run001202.dat");
  //  strcpy(pedFile,"/Users/rjn/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
  gotPedFile=1;
  loadPedestals();
  loadCalib();

  Double_t ampVal=200;
  Double_t freqVal=0.2; //200 Mhz
  Double_t period=1./freqVal;

  //  TF1 *fitSine1 = new TF1("fitSine1",mySineWave,-100,200,3);
  //  fitSine1->SetNpx(10000);
  //  fitSine1->SetParameters(ampVal,freqVal,0.8);
  //  fitSine1->SetParLimits(0,ampVal,ampVal);
  //  fitSine1->SetParLimits(1,freqVal,freqVal);
  //  fitSine1->SetParLimits(2,0,1/freqVal);

  //  TF1 *fitSine2 = new TF1("fitSine2",mySineWave,-100,200,3);
  //  fitSine2->SetNpx(10000);
  //  fitSine2->SetParameters(ampVal,freqVal,0.8);
  //  fitSine2->SetParLimits(0,ampVal,ampVal);
  //  fitSine2->SetParLimits(1,freqVal,freqVal);
  //  fitSine2->SetParLimits(2,0,1/freqVal);

  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  for(Long64_t i=9;i<10;i++) {
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    UsefulAraEvent realEvent(evPtr,AraCalType::kFirstCalib);
    //    for(int chanIndex=0;chanIndex<NUM_DIGITIZED_CHANNELS;chanIndex++) {
    for(int chanIndex=0;chanIndex<1;chanIndex++) {
      int chip=chanIndex/9;
      int chan=chanIndex%9;
      if(chan==8) continue;
      if(chip==2 && chan==7) continue;
      int rco=evPtr->getRCO(chanIndex);
      int firstSamp=evPtr->getEarliestSample(chanIndex);
      int lastSamp=evPtr->getLatestSample(chanIndex);

      std::cout << "Channel: " << chanIndex << "\t" << firstSamp << "\t" << lastSamp << "\t" << rco << "\n";

      Float_t data[260];
      Float_t maxVal=-1e9;
      Float_t minVal=+1e9;
      for(int samp=0;samp<260;samp++) {
	data[samp]=evPtr->chan[chanIndex].data[samp]-pedestalData[chip][chan][samp];
	if(data[samp]>maxVal) maxVal=data[samp];
	if(data[samp]<minVal) minVal=data[samp];
      }
      //Zero mean the waveform
      Float_t mean=TMath::Mean(260,data);
      for(int samp=0;samp<260;samp++)
	data[samp]-=mean;
      //      std::cout << mean << "\t" << minVal << "\t" << maxVal << "\n";

      minVal-=mean;
      maxVal-=mean;
      Double_t amp=TMath::Abs(minVal);
      //Set the amplitude
      if(TMath::Abs(maxVal)>amp) amp=TMath::Abs(maxVal);
      

      if(firstSamp<lastSamp) {
	//One RCO lag
	continue;
      }
      else {
	//Two RCO lags
	//Do the first lag which runs from firstSamp to 259
	 Int_t numFirst=260-firstSamp;
	 Double_t tVals[2][260]={{0}};
	 Double_t vVals[2][260]={{0}};
	 Double_t vValErrs[2][260]={{10}};
	 Double_t curTVal=0;
	 for(int samp=0;samp<numFirst;samp++) {
	   int cap=samp+firstSamp;
	   tVals[0][samp]=curTVal;
	   vVals[0][samp]=data[cap];
	   if(samp<(numFirst-1))
	     curTVal+=binWidths[chip][1-rco][cap];
	 }

	 Int_t numSecond=lastSamp+1;
	 for(int samp=0;samp<numSecond;samp++) {
	   int cap=samp;
	   tVals[1][samp]=curTVal;
	   vVals[1][samp]=data[cap];
	   curTVal+=binWidths[chip][rco][cap];
	 }
	 TGraph *grAll = realEvent.getGraphFromElecChan(chanIndex);
	 TGraphErrors *grFirst = new TGraphErrors(numFirst,tVals[0],vVals[0],0,vValErrs[0]);
	 TGraphErrors *grSecond = new TGraphErrors(numSecond,tVals[1],vVals[1],0,vValErrs[1]);
	 Double_t lagAll=estimateLag(grAll,freqVal);
	 Double_t lag1=estimateLagLast(grFirst,freqVal);
	 Double_t lag1a=estimateLag(grFirst,freqVal);
	 std::cout << "lag1 guess: " << lag1 << "\n";
	 std::cout << "lag1a guess: " << lag1a << "\n";
	 Double_t lag2=estimateLagFirst(grSecond,freqVal);
	 Double_t lag3=estimateLag(grSecond,freqVal);

	 TCanvas *can = new TCanvas();
	 TH1F *framey = can->DrawFrame(0,-1.2*amp,260,1.2*amp);
	 grFirst->SetMarkerColor(50);
	 grFirst->SetLineColor(50);
	 grFirst->Draw("lp");
// 	 fitSine1->SetParameter(0,amp);
// 	 fitSine1->SetParLimits(0,amp,amp);
// 	 fitSine1->SetRange(tVals[0][0],tVals[0][numFirst-1]);

// 	 fitSine1->SetParameter(2,lag1);
// 	 fitSine1->SetParLimits(2,lag1,lag1);
// 	 //	 grFirst->Fit("fitSine1","R");
// 	 grSecond->SetMarkerColor(8);
// 	 grSecond->SetLineColor(8);
// 	 //	 grSecond->Draw("lp");
// 	 fitSine2->SetParameter(0,amp);
// 	 fitSine2->SetParLimits(0,amp,amp);
// 	 fitSine2->SetRange(tVals[1][0],tVals[1][numSecond-1]);

// 	 fitSine2->SetParameter(2,lag2);
// 	 fitSine2->SetParLimits(2,lag2,lag2);
	 //	 grSecond->Fit("fitSine2","R");
	 
	 Double_t deltaLag=lag1-lag2;
	 if(TMath::Abs(deltaLag+period)<TMath::Abs(deltaLag))
	   deltaLag+=period;
	 if(TMath::Abs(deltaLag-period)<TMath::Abs(deltaLag))
	   deltaLag-=period;
	 std::cout << "Lag: "<< deltaLag << "\t" << lag1 << "\t" << lag2   << "\n";
	 for(int samp=0;samp<numSecond;samp++) {
	   tVals[1][samp]+=deltaLag;
	 }

	 TGraphErrors *grSecondCor = new TGraphErrors(numSecond,tVals[1],vVals[1],0,vValErrs[1]);
	 grSecondCor->SetMarkerColor(8);
	 grSecondCor->SetLineColor(8);
	 grSecondCor->Draw("lp");
	 
	 

      }
    }	
  }
  
}



void loadPedestals()
{
  if(!gotPedFile) {
    char calibDir[FILENAME_MAX];
    char *calibEnv=getenv("ARA_CALIB_DIR");
    if(!calibEnv) {
      char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
      if(!utilEnv)
        sprintf(calibDir,"calib");
      else
        sprintf(calibDir,"%s/share/araCalib",utilEnv);
    }
    else {
      strncpy(calibDir,calibEnv,FILENAME_MAX);
    }  
    sprintf(pedFile,"%s/peds_1286989711.394723.dat",calibDir);
  }
  FullLabChipPedStruct_t peds;
  gzFile inPed = gzopen(pedFile,"r");
  if( !inPed ){
    fprintf(stderr,"Failed to open pedestal file %s.\n",pedFile);
    return;
  }

  int nRead = gzread(inPed,&peds,sizeof(FullLabChipPedStruct_t));
  if( nRead != sizeof(FullLabChipPedStruct_t)){
    int numErr;
    fprintf(stderr,"Error reading pedestal file %s; %s\n",pedFile,gzerror(inPed,&numErr));
    gzclose(inPed);
    return;
  }

  int chip,chan,samp;
  for(chip=0;chip<ACTIVE_CHIPS;++chip) {
    for(chan=0;chan<CHANNELS_PER_CHIP;++chan) {
      int chanIndex = chip*CHANNELS_PER_CHIP+chan;
      for(samp=0;samp<MAX_NUMBER_SAMPLES;++samp) {
	pedestalData[chip][chan][samp]=peds.chan[chanIndex].pedMean[samp];
      }
    }
  }
  gzclose(inPed);
}




Double_t mySineWave(Double_t *t, Double_t *par) 
{
  Double_t amp=par[0]; //In mV
  Double_t freq=par[1]; //In GHz
  Double_t lag=par[2]; //In ns

  return amp*TMath::Sin(TMath::TwoPi()*freq*(t[0]-lag));

}

void loadCalib()
{
  std::ifstream BinFile("binWidths.txt");
  int chip,rco;
  double width;
  while(BinFile >> chip >> rco) {
    for(int samp=0;samp<MAX_NUMBER_SAMPLES;samp++) {
      BinFile >> width;
      binWidths[chip][rco][samp]=width;
    }
  }
}


Double_t estimateLagFirst(TGraph *grIn, Double_t freq)
{

  // This funciton estimates the lag by just using the first negative-positive zero crossing
  // To resolve quadrant ambiguity in the ASin function, the first zero crossing is used as a test of lag
  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t period=1./freq;
  Double_t *tVals=grIn->GetX();
  Double_t *vVals=grIn->GetY();

  Double_t zc=0;
  for(int i=2;i<numPoints;i++) {
    if(vVals[i-1]<0 && vVals[i]>0) {
      Double_t x1=tVals[i-1];
      Double_t x2=tVals[i];
      Double_t y1=vVals[i-1];
      Double_t y2=vVals[i];      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      zc=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      break;
    }
  }
  while(zc>period) zc-=period;
  //  std::cout << zc << "\n";
  return zc;

}



Double_t estimateLagLast(TGraph *grIn, Double_t freq)
{

  // This funciton estimates the lag by just using the first negative-positive zero crossing
  // To resolve quadrant ambiguity in the ASin function, the first zero crossing is used as a test of lag
  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t period=1./freq;
  Double_t *tVals=grIn->GetX();
  Double_t *vVals=grIn->GetY();

  Double_t zc=0;
  for(int i=2;i<numPoints;i++) {
    if(vVals[i-1]<0 && vVals[i]>0) {
      Double_t x1=tVals[i-1];
      Double_t x2=tVals[i];
      Double_t y1=vVals[i-1];
      Double_t y2=vVals[i];      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      zc=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      //      break;
    }
  }
  while(zc>period) zc-=period;
  //  std::cout << zc << "\n";
  return zc;

}

Double_t estimateLag(TGraph *grIn, Double_t freq)
{
  // This funciton estimates the lag by just using all the negative-positive zero crossing
 
  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t period=1./freq;
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
     std::cout << i << "\t" << zc[i] << "\n";     
  }
  TCanvas *can = new TCanvas();
  TGraph *gr = new TGraph(countZC,rawZc,zc);
  gr->Draw("ap");

  //  std::cout << "\n";
  meanZC/=countZC;
  
  //  std::cout << zc << "\n";
  return meanZC;

}
