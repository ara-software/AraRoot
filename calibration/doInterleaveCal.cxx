#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraEvent.h"
#include "RawAraEvent.h"
#include "AraGeomTool.h"
#include "AraEventCalibrator.h"
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
#include "TAxis.h"
#include <TGClient.h>

#include <zlib.h>


Double_t estimateLag(TGraph *grIn, Double_t freq);
Double_t estimateLagLast(TGraph *grIn, Double_t freq);
Double_t mySineWave(Double_t *t, Double_t *par) ;
int gotPedFile=0;
char pedFile[FILENAME_MAX];
float pedestalData[ACTIVE_CHIPS][CHANNELS_PER_CHIP][MAX_NUMBER_SAMPLES];
Double_t binWidths[ACTIVE_CHIPS][2][MAX_NUMBER_SAMPLES];

int main(int argc, char **argv)
{
   char inputFile[FILENAME_MAX];
   Double_t ampVal=200;
   Double_t freqVal=0.2; //200 Mhz

   if(argc>1) {
      strncpy(inputFile,argv[1],FILENAME_MAX);
   }
   else {
      strncpy(inputFile,"/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/sine_wave_data/root/event250MHz_331mV.root",FILENAME_MAX);
   }
   if(argc>2) {
      freqVal=atof(argv[2]);
   }
   if(argc>3) {
      ampVal=atof(argv[3]);
   }


   //   TFile *fp = new TFile("/Users/rjn/ara/data/root/event_frozen_200MHz.root");
   TFile *fp = new TFile(inputFile);
  if(!fp) {
    std::cerr << "Can't open file\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fp->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't find eventTree\n";
    return -1;
  }
  //  AraEventCalibrator::Instance()->setPedFile("/Users/rjn/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
  AraEventCalibrator::Instance()->setPedFile("/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
  RawAraEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);
  
  Double_t period=1./freqVal;
  
  
  char outName[180];
  sprintf(outName,"interleaveFile_%3.0f_%3.0f.root",1000*freqVal,ampVal);
  TFile *fpInterleave = new TFile(outName,"RECREATE");
  TH1F *histInterleave[2];
  TH1F *histInterleaveChan[2][4];
  char histName[180];

  for(int chip=0;chip<2;chip++) {
      sprintf(histName,"histInterleave_%d",chip);
      histInterleave[chip] = new TH1F(histName,histName,10000,-period,period);
      for(int chan=0;chan<4;chan++) {
	sprintf(histName,"histInterleaveChan_%d_%d",chip,chan);
	histInterleaveChan[chip][chan] = new TH1F(histName,histName,10000,-period,period);
    }
    
  }
  AraGeomTool *tempGeom = AraGeomTool::Instance();

  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  for(Long64_t i=0;i<numEntries;i++) {
     
     
     if(i%starEvery==0) {
	std::cerr << "*";

	std::ofstream EvNum("lastEvent");
	EvNum << i << "\n";
	EvNum.close();
     }
    eventTree->GetEntry(i);
    //    std::cerr << i << "\t" << evPtr->head.unixTime << "\n";
    //    for(int chan=0;chan<NUM_DIGITIZED_CHANNELS;chan++) {
    if(evPtr->getFirstHitBus(18)!=evPtr->getFirstHitBus(19)) {
       std::cerr << "Bad channel?\n"; 
       continue;
    }
    if(evPtr->head.unixTime>2e9) continue;


    UsefulAraEvent realEvent(evPtr,AraCalType::kFirstCalib);
    
    for(int rfChan=0;rfChan<8;rfChan++) {
      int ci1=tempGeom->getFirstLabChanIndexForChan(rfChan);
      int ci2=tempGeom->getSecondLabChanIndexForChan(rfChan);
      int chip=ci1/9;

      Int_t firstSamp=evPtr->getEarliestSample(ci1);
      if(firstSamp<20 || firstSamp>250) continue;

      TGraph *grFirst = realEvent.getGraphFromElecChan(ci1);
      TGraph *grSecond = realEvent.getGraphFromElecChan(ci2);
      Double_t lag1=estimateLag(grFirst,freqVal);
      Double_t lag2=estimateLag(grSecond,freqVal);
      Double_t shift=lag1-lag2;
      if(TMath::Abs(shift+period)<TMath::Abs(shift)) shift+=period;
      if(TMath::Abs(shift-period)<TMath::Abs(shift)) shift-=period;
      histInterleave[chip]->Fill(shift);
      histInterleaveChan[chip][rfChan%4]->Fill(shift);
  //     Double_t *xVals = grSecond->GetX();
//       Double_t *yVals = grSecond->GetY();
//       Int_t numVals=grSecond->GetN();
//       for(int i=0;i<numVals;i++) {
// 	xVals[i]+=shift;
//       }
     //  TGraph *grSecondCor = new TGraph(numVals,xVals,yVals);
//       std::cout << lag1 << "\t" << lag2 << "\n";
//       TCanvas *can = new TCanvas();
//       grFirst->Draw("alp");
//       grSecondCor->SetMarkerColor(8);
//       grSecondCor->SetLineColor(8);
//       grSecondCor->Draw("lp");
      delete grFirst;
      delete grSecond;
    }	
  }
   
  fpInterleave->Write();
  sprintf(outName,"interleaveFile_%3.0f_%3.0f.txt",1000*freqVal,ampVal);  
  std::ofstream IntFile(outName);
  for(int chip=0;chip<2;chip++) {
    for (int chan=0;chan<4;chan++) {
      histInterleaveChan[chip][chan]->GetXaxis()->SetRangeUser(-1,1);
      IntFile << chip << "\t"  << chan << "\t" <<  histInterleaveChan[chip][chan]->GetMean() << "\n";
    }
  }
  IntFile.close();

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
     //     std::cout << i << "\t" << zc[i] << "\n";     
  }
  //  TCanvas *can = new TCanvas();
  //  TGraph *gr = new TGraph(countZC,rawZc,zc);
  //  gr->Draw("ap");

  //  std::cout << "\n";
  meanZC/=countZC;
  
  //  std::cout << zc << "\n";
  return meanZC;

}



Double_t estimateLagLast(TGraph *grIn, Double_t freq)
{

  // This funciton estimates the lag by just using the first negative-positive zero crossing
  // To resolve quadrant ambiguity in the ASin function, the first zero crossing is used as a test of lag
  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t period=1/freq;
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
