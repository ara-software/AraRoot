#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraTestBedStationEvent.h"
#include "RawAraTestBedStationEvent.h"
#include "AraGeomTool.h"
#include "AraEventCalibrator.h"
#include "araTestbedDefines.h"
#include "FFTtools.h"

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

Double_t estimateLag(TGraph *grIn, Double_t freq, Double_t &rms);
Double_t estimatePeriod(TGraph *grIn, Double_t &rms);
Double_t getSimpleDeltat(TGraph *gr1, TGraph *gr2, Double_t period) ;
TGraph *justPositive(TGraph *gr1);
TGraph *generateCombFunction(Double_t period, Double_t dt, Int_t numPoints);
TGraph *combFilter(TGraph *grIn, TGraph *grComb, Double_t period);
int debug=0;

int main(int argc, char **argv)
{

  if(argc<2) {
    std::cout << "Usage\n" << argv[0] << " <input file>\n";
    return 0;
  }

  TFile *fp = TFile::Open(argv[1]);
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
   //   AraEventCalibrator::Instance()->setPedFile("/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
   RawAraTestBedStationEvent *evPtr=0;
   eventTree->SetBranchAddress("event",&evPtr);
   
   
   char outName[180];
   sprintf(outName,"testClockPeriod.root");
   TFile *fpOut = new TFile(outName,"RECREATE");
   Int_t firstSamp[3];
   Int_t rco[3];
   Int_t myRco[3];
   Double_t period[2][3];
   Double_t rms[2][3];
   TTree *clockTree = new TTree("clockTree","Tree of clock calib fun");
   clockTree->Branch("period",period,"period[2][3]/D");
   clockTree->Branch("rms",rms,"rms[2][3]/D");
   clockTree->Branch("firstSamp",firstSamp,"firstSamp[3]/i");
   clockTree->Branch("rco",rco,"rco[3]/i");
   clockTree->Branch("myRco",myRco,"myRco[3]/i");

   AraGeomTool *tempGeom = AraGeomTool::Instance();
   AraEventCalibrator *fCalibrator = AraEventCalibrator::Instance();

   Long64_t numEntries=eventTree->GetEntries();
   numEntries=10000;
   Long64_t starEvery=numEntries/80;
   if(starEvery==0) starEvery++;
   //   numEntries=2;

   for(Long64_t event=0;event<numEntries;event++) {
     
     if(event%starEvery==0) {
       std::cerr << "*";       
     }
     eventTree->GetEntry(event);
     
     firstSamp[0]=evPtr->getEarliestSample(8);
     firstSamp[1]=evPtr->getEarliestSample(17);
     firstSamp[2]=evPtr->getEarliestSample(26);

     UsefulAraTestBedStationEvent realEvent(evPtr,AraCalType::kFirstCalib);


     TGraph *grClock[3];
     for(int chip=0;chip<3;chip++) {
	 int chanIndex=8+9*chip;
	 rco[chip]=realEvent.getRCO(chanIndex);
	 //	 std::cout << rco[chip] << "\t" << realEvent.getRawRCO(chanIndex) << "\n";
	 

	 for(int rcoGuess=0;rcoGuess<2;rcoGuess++) {
	   int numValid=fCalibrator->doBinCalibration(&realEvent,chanIndex,rcoGuess);	   
	   grClock[chip]=new TGraph(numValid,fCalibrator->calTimeNums,fCalibrator->calVoltNums);
	   //	   std::cout << event << "\t" << chip << "\t" << rcoGuess << "\n";
	   period[rcoGuess][chip]=estimatePeriod(grClock[chip],rms[rcoGuess][chip]);
	   //	   char canName[180];
	   //	   sprintf(canName,"can%d_%d_%d",event,chip,rcoGuess);
	   //	   new TCanvas(canName,canName);
	   //	   grClock[chip]->Draw("alp");

	   delete grClock[chip];
	 }     
	 Double_t testVal=(TMath::Abs(period[0][chip]-25)-TMath::Abs(period[1][chip]-25))+0.5*(rms[0][chip]-rms[1][chip]);
	 	 
	 if(testVal<=0) {
	   myRco[chip]=0;
	 }
	 else {
	   myRco[chip]=1;
	 }
	 if(myRco[chip]!=rco[chip]) {
	   std::cout << event << "\t" << realEvent.head.eventNumber << "\t" << chip << "\n";
	   std::cout << period[0][chip] << "\t" << period[1][chip] << "\n";
	   std::cout << rms[0][chip] << "\t" << rms[1][chip] << "\n";
	 }
     }
     clockTree->Fill();     
   }
   std::cerr << "\n";

   clockTree->AutoSave();
   fpOut->Close();

}

Double_t getSimpleDeltat(TGraph *gr1, TGraph *gr2, Double_t period) 
{
  static int counter=0;
  TGraph *grCor = FFTtools::getCorrelationGraph(gr1,gr2);
  Int_t peakBin=FFTtools::getPeakBin(grCor);
  Double_t *dtVals=grCor->GetX();
  Double_t dt=dtVals[peakBin];
  while(dt>0.5*period) dt-=period;
  while(dt<-0.5*period) dt+=period;

  if(debug) {
    char graphName[180];
    sprintf(graphName,"grCor%d_%d",counter/3,counter%3);
    grCor->SetName(graphName);
    grCor->SetTitle(graphName);
    grCor->Write();
  }
  counter++;
  delete grCor;
  return dt;
}



TGraph *justPositive(TGraph *gr1)
{
  Int_t numPoints=gr1->GetN();
  Double_t *newY= new Double_t[numPoints];
  Double_t *oldY=gr1->GetY();
  Double_t *oldX=gr1->GetX();
  for(int i=0;i<numPoints;i++) {
    newY[i]=oldY[i];
    if(newY[i]<0) newY[i]=0;
  }

  TGraph *gr = new TGraph(numPoints,oldX,newY);
  delete [] newY;
  return gr;
}

TGraph *generateCombFunction(Double_t period, Double_t dt, Int_t numPoints)
{
  Double_t *xVals = new Double_t[numPoints];
  Double_t *yVals = new Double_t[numPoints];
  
  for(int i=0;i<numPoints;i++) {
    xVals[i]=dt*i;
    yVals[i]=0;
    Double_t temp=(xVals[i]+0.5*period);
    //Calculate remainder must be a more efficient way
    temp/=period;
    Int_t tempi=Int_t(temp);
    temp-=tempi;
    temp*=period;
    if(temp<0.5*dt || TMath::Abs(temp-period)<0.5*dt) {
      //      std::cout << i << "\t" << xVals[i] << "\t" << temp << "\n";
      yVals[i]=1;
    }
  }
  TGraph *grComb = new TGraph(numPoints,xVals,yVals);
  delete [] xVals;
  delete [] yVals;
  return grComb;

}

TGraph *combFilter(TGraph *grIn, TGraph *grComb, Double_t period)
{
  TGraph *grCor = FFTtools::getCorrelationGraph(grIn,grComb);
  Double_t *xVals=grCor->GetX();
  Int_t zeroBin=(0-xVals[0])/(xVals[1]-xVals[0]);
  Int_t periodBins=period/(xVals[1]-xVals[0]);
  
  Int_t peakBin=FFTtools::getPeakBin(grCor,zeroBin-0.5*periodBins,zeroBin+0.5*periodBins);
  //  std::cout << peakBin << "\t" << peakBin-zeroBin << "\t" << xVals[peakBin] << "\n";
  Int_t offset=peakBin-zeroBin;

  Double_t *rawX = grIn->GetX();
  Double_t *rawY = grIn->GetY();
  Int_t numPoints=grIn->GetN();
  Int_t numPointsComb=grComb->GetN();
  Double_t *combY=grComb->GetY();
  Double_t *combX=grComb->GetX();
  Double_t *newY = new Double_t[numPoints];
  Int_t *mapY = new Int_t[numPoints];
  for(int i=0;i<numPoints;i++) {    
    mapY[i]=0;
    int combBin=i-offset;
    if(combBin>=0 && combBin<numPointsComb) {
      if(combY[combBin]>0) {
	//Got one
	for(int j=i;j>=i-10;j--) {
	  if(j<0) break;
	  mapY[j]=1;
	}
	int tempi=i;
	for(i=tempi;i<tempi+10;i++) {
	  if(i>=numPoints) break;
	  mapY[i]=1;
	}
	if(i<numPoints) mapY[i]=0;
      }
    }
  }



  static int firstOne=1;

  for(int i=0;i<numPoints;i++) {
    int combBin=i-offset;
    newY[i]=0;
    //    if(firstOne) {
    //      std::cout << i << "\t" << rawX[i] << "\t" << rawY[i] << "\t" << mapY[i] << "\t" << combY[combBin] << "\n";
    //    }
    if(mapY[i]) newY[i]=rawY[i];
  }
  
  TGraph *grRet = new TGraph(numPoints,rawX,newY);
  delete [] newY ;
  delete [] mapY ;
  delete grCor;
  firstOne=0;
  return grRet;
}


Double_t estimateLag(TGraph *grIn, Double_t freq, Double_t &rms)
{
  // This funciton estimates the lag by just using all the negative-positive zero crossing
 
  Double_t mean=grIn->GetMean(2);
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
      Double_t y1=vVals[i-1]-mean;
      Double_t y2=vVals[i]-mean;      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      Double_t zcTime=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      if(countZC>0) {
	if((zcTime-zc[countZC-1])<10)
	  continue;
      }
      zc[countZC]=zcTime;
      rawZc[countZC]=zc[countZC];
      countZC++;
      //      if(countZC==1)
      //      break;
    }
       
  }

  Double_t firstZC=zc[0];
  while(firstZC>period) firstZC-=period;
  Double_t meanZC=0;
  Double_t meanZC2=0;
  for(int i=0;i<countZC;i++) {
     while((zc[i]-firstZC)>period) zc[i]-=period;
     if(TMath::Abs((zc[i]-period)-firstZC)<TMath::Abs(zc[i]-firstZC))
       zc[i]-=period;
     if(TMath::Abs((zc[i]+period)-firstZC)<TMath::Abs(zc[i]-firstZC))
       zc[i]+=period;
     meanZC+=zc[i];
     meanZC2+=zc[i]*zc[i];
     //     std::cout << i << "\t" << zc[i] << "\t" << rawZc[i] << "\n";     
  }
  //  TCanvas *can = new TCanvas();
  //  TGraph *gr = new TGraph(countZC,rawZc,zc);
  //  gr->Draw("ap");

  //  std::cout << "\n";
  meanZC/=countZC;
  meanZC2/=countZC;
  rms=TMath::Sqrt(meanZC2-meanZC*meanZC);
  //  std::cout << meanZC << "\t" << rms << "\n";
  return meanZC;

}



Double_t estimatePeriod(TGraph *grIn, Double_t &rms)
{
  // This funciton estimates the period by just using all the negative-positive zero crossing
 
  Double_t mean=grIn->GetMean(2);
  Int_t numPoints=grIn->GetN();
  if(numPoints<3) return 0;
  Double_t *tVals=grIn->GetX();
  Double_t *vVals=grIn->GetY();

  Double_t zc[1000]={0};
  Double_t periods[1000]={0};
  int countZC=0;
  for(int i=2;i<numPoints;i++) {
    Double_t x1=tVals[i-1];
    Double_t x2=tVals[i];
    Double_t y1=vVals[i-1]-mean;
    Double_t y2=vVals[i]-mean;      
    if(vVals[i-1]<0 && vVals[i]>0) {

      Double_t zcTime=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      // std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\t" << zcTime << "\n";
      if(countZC>0) {
	if((zcTime-zc[countZC-1])<10)
	  continue;
      }
      zc[countZC]=zcTime;
      //      zc[countZC]=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      countZC++;
      //      if(countZC==1)
      //      break;
    }
       
  }

  if(countZC<2) return 0;
  
  
  int countPeriods=0;
  Double_t meanPeriod=0;
  Double_t meanPeriodSq=0;
  for(int i=1;i<countZC;i++) {
    periods[countPeriods]=zc[i]-zc[i-1];
    meanPeriod+=periods[countPeriods];
    meanPeriodSq+=periods[countPeriods]*periods[countPeriods];
    //std::cout << countPeriods << "\t" << periods[countPeriods] << "\n";
    countPeriods++;


  }
  meanPeriod/=countPeriods;
  meanPeriodSq/=countPeriods;
  rms=TMath::Sqrt(meanPeriodSq-meanPeriod*meanPeriod);
  
  return meanPeriod;

}
