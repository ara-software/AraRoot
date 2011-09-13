#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraTestBedStationEvent.h"
#include "RawAraTestBedStationEvent.h"
#include "AraGeomTool.h"
#include "AraEventCalibrator.h"
#include "araTestbedDefines.h"

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
float pedestalData[LAB3_PER_TESTBED][CHANNELS_PER_LAB3][MAX_NUMBER_SAMPLES_LAB3];
Double_t binWidths[LAB3_PER_TESTBED][2][MAX_NUMBER_SAMPLES_LAB3];

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


   //TFile *fp = new TFile("/Users/rjn/ara/data/root/event_frozen_200MHz.root");
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
  RawAraTestBedStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);
  //strcpy(pedFile,"/Users/rjn/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
 strcpy(pedFile,"/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291303459/peds_1291303459.323022.dat");

 gotPedFile=1;
  loadPedestals();
  loadCalib();

  //  Double_t ampVal=200;
  //  Double_t freqVal=0.2; //200 Mhz
  Double_t period=1./freqVal;
  char outName[180];
  sprintf(outName,"epsilonFile_%3.0fMHz_%3.0fmV.root",freqVal*1000,ampVal);

  TFile *fpEpsilon = new TFile(outName,"RECREATE");
  TH1F *histEpsilon[3][2];
  TH1F *histEpsilonChan[3][8][2];
  char histName[180];
  TTree *epsTree = new TTree("epsTree","Tree of Epsilons");
  Int_t chip,chan,rco,firstSamp,lastSamp,otherChip;
  Double_t epsilon,lag1,lag2;
  epsTree->Branch("rco",&rco,"rco/I");
  epsTree->Branch("chan",&chan,"chan/I");
  epsTree->Branch("chip",&chip,"chip/I");
  epsTree->Branch("epsilon",&epsilon,"epsilon/D");
  epsTree->Branch("lag1",&lag1,"lag1/D");
  epsTree->Branch("lag2",&lag2,"lag2/D");
  epsTree->Branch("firstSamp",&firstSamp,"firstSamp/I");
  epsTree->Branch("lastSamp",&lastSamp,"lastSamp/I");
  epsTree->Branch("otherChip",&otherChip,"otherChip/I");

  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {
      sprintf(histName,"histEpsilon_%d_%d",chip,rco);
      histEpsilon[chip][rco] = new TH1F(histName,histName,6000,-3,3);
      for(int chan=0;chan<8;chan++) {
	sprintf(histName,"histEpsilonChan_%d_%d_%d",chip,chan,rco);
	histEpsilonChan[chip][chan][rco] = new TH1F(histName,histName,6000,-3,3);
      }
    }
    
  }

  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  //  numEntries=10000;
  for(Long64_t i=0;i<numEntries;i++) {
    if(i%starEvery==0) 
      std::cerr << "*";
    eventTree->GetEntry(i);
    
    for(int chanIndex=0;chanIndex<NUM_DIGITIZED_TESTBED_CHANNELS;chanIndex++) {
      chip=chanIndex/9;
      chan=chanIndex%9;
      if(chan==8) continue;
      if(chip==2 && chan==7) continue;
      rco=evPtr->getRCO(chanIndex);
      otherChip=evPtr->getLabChip(chanIndex);
      firstSamp=evPtr->getEarliestSample(chanIndex);
      lastSamp=evPtr->getLatestSample(chanIndex);

      //To get a cleaner sample of RCO zero and one  uncomment the following line
      //      if(firstSamp<20) continue;

      //Check to see which rco phase we and if we need to switch
      //      if(lastSamp<8) rco=1-rco;

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
	//One RCO phase
	continue;
      }
      else {
	//Two RCO phases
	//Do the first phase which runs from firstSamp to 259
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
	 TGraphErrors *grFirst = new TGraphErrors(numFirst,tVals[0],vVals[0],0,vValErrs[0]);
	 TGraphErrors *grSecond = new TGraphErrors(numSecond,tVals[1],vVals[1],0,vValErrs[1]);
	 //
	 lag1=estimateLag(grFirst,freqVal);
	 lag2=estimateLag(grSecond,freqVal);

	 
	 Double_t deltaLag=lag1-lag2;
	 if(TMath::Abs(deltaLag+period)<TMath::Abs(deltaLag))
	   deltaLag+=period;
	 if(TMath::Abs(deltaLag-period)<TMath::Abs(deltaLag))
	   deltaLag-=period;
	 epsilon=deltaLag;
	 if(lag1!=0 & lag2!=0) {
	    //	   epsTree->Fill();
	   if(firstSamp>20 && firstSamp<250) {
	      if(i>10) {
		 Double_t mean=histEpsilon[chip][rco]->GetMean();
		 if(TMath::Abs((deltaLag+period)-mean)<TMath::Abs(deltaLag-mean))
		    deltaLag+=period;
		 if(TMath::Abs((deltaLag-period)-mean)<TMath::Abs(deltaLag-mean))
		    deltaLag-=period;
	      }
	      histEpsilon[chip][rco]->Fill(deltaLag);
	      histEpsilonChan[chip][chan][rco]->Fill(deltaLag);
	   }
	 }
// 	 TCanvas *can = new TCanvas();
// 	 TH1F *framey = can->DrawFrame(0,-1.2*amp,260,1.2*amp);
// 	 grFirst->SetMarkerColor(kRed+3);
// 	 grFirst->Draw("p");
// 	 grSecond->SetMarkerColor(kBlue+3);
// 	 grSecond->Draw("p");

	 delete grFirst;
	 delete grSecond;
	 
      }
    }	
  }
  epsTree->AutoSave();
  fpEpsilon->Write();
  

  sprintf(outName,"epsilonFile_%3.0fMHz_%3.0fmV.txt",freqVal*1000,ampVal);
  std::ofstream EpsFile(outName);
  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {
      EpsFile << chip << "\t" << rco << "\t" << histEpsilon[chip][rco]->GetMean() << "\n";
    }
  }
  EpsFile.close();

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
  for(chip=0;chip<LAB3_PER_TESTBED;++chip) {
    for(chan=0;chan<CHANNELS_PER_LAB3;++chan) {
      int chanIndex = chip*CHANNELS_PER_LAB3+chan;
      for(samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
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
    for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
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
