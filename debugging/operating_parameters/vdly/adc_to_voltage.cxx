////////////////////////////////////////////////////////////////////////////////////////
////                                                                                ////
////  adc_to_voltage jdavies@hep.ucl.ac.uk Jonathan Davies                          ////
////                                                                                ////
////                                                                                ////
////////////////////////////////////////////////////////////////////////////////////////

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
#include "TGraph.h"
#include "TGraphErrors.h"

TGraph* zeroMean(TGraph*);
Double_t getMean(TGraph*);
Double_t getMax(TGraph*);
Double_t getMin(TGraph*);
TGraph *getBlockGraph(TGraph*, Int_t);
TGraph* getHalfGraph(TGraph*, Int_t);
Int_t process_run(char*, char*, Double_t, Int_t, Int_t, TH1F*, TH1F*, TH1F*);
int adc_to_voltage(char*, char*);

using namespace std;
int main(int argc, char **argv)
{
  if(argc<3) {
    std::cerr << "Usage: " << argv[0] << " <runList> <outFile> \n";
    return 1;
  }
  return adc_to_voltage(argv[1], argv[2]);
}

int adc_to_voltage(char *runDescriptionFileName, char *outFileName){

  TFile *outFile = new TFile(outFileName, "RECREATE");\
  TGraphErrors *grMaxMin = 0;
  TCanvas *can_max_min = 0;
  Int_t channel=0, dda=0;
  Double_t peakToPeakVoltage=0, peakToPeakError=0;
  Double_t av_max_zm=0, av_min_zm=0;
  Double_t av_max_error=0, av_min_error=0;
  TTree *outTree = new TTree("voltage_to_adc", "voltage_to_adc");
  outTree->Branch("peakToPeakVoltage", &peakToPeakVoltage, "peakToPeakVoltage/D");
  outTree->Branch("peakToPeakError", &peakToPeakError, "peakToPeakError/D");
  outTree->Branch("av_max_zm", &av_max_zm, "av_max_zm/D");
  outTree->Branch("av_min_zm", &av_min_zm, "av_min_zm/D");
  outTree->Branch("av_max_error", &av_max_error, "av_max_error/D");
  outTree->Branch("av_min_error", &av_min_error, "av_min_error/D");


  std::fstream runDescriptionFile(runDescriptionFileName);
  char *runFileBaseName = new char[100];
  char *pedFileBaseName = new char[100];
  Int_t loop_number=0;
  TH1F* hist_max_zm;
  TH1F* hist_min_zm;
  TH1F* hist_mean;
  Int_t runNumber=0, pedNumber=0;
  Double_t x_av_max[30]={0};
  Double_t y_av_max[30]={0};
  Double_t xerr_av_max[30]={0};
  Double_t yerr_av_max[30]={0};
  Double_t x_av_min[30]={0};
  Double_t y_av_min[30]={0};
  Double_t xerr_av_min[30]={0};
  Double_t yerr_av_min[30]={0};
  Int_t loopNumber=0;

  runDescriptionFile >> runFileBaseName >> pedFileBaseName;
  while(runDescriptionFile >> runNumber >> pedNumber >> peakToPeakVoltage >> peakToPeakError >> channel >> dda){

    char runFileName[100], pedFileName[100];
    sprintf(runFileName, "%s/run%i/event%i.root", runFileBaseName, runNumber, runNumber);
    sprintf(pedFileName, "%s/run_000%i/pedestalValues.run000%i.dat", pedFileBaseName, pedNumber, pedNumber);

    printf("run %s ped %s\n", runFileName, pedFileName);
    hist_max_zm = new TH1F("hist_max_zm", "hist_max_zm", 2000, 1, 2000);
    hist_min_zm = new TH1F("hist_min_zm", "hist_min_zm", 2000, -2000, -1);
    hist_mean = new TH1F("hist_mean", "hist_mean", 2001, -1000, 1000);

    fprintf(stderr, "run %i ped %i vpp %f vpperr %f chan % i dda %i\n", runNumber, pedNumber, peakToPeakVoltage, peakToPeakError, channel, dda);

    process_run(runFileName, pedFileName, peakToPeakVoltage, dda, channel, hist_max_zm, hist_min_zm, hist_mean);
    av_max_zm = hist_max_zm->GetMean();
    av_min_zm = hist_min_zm->GetMean();
    av_max_error = hist_max_zm->GetMeanError();
    av_min_error = hist_min_zm->GetMeanError();
    
    y_av_max[loopNumber] = av_max_zm;
    y_av_min[loopNumber] = av_min_zm;
    yerr_av_max[loopNumber] = av_max_error;
    yerr_av_min[loopNumber] = av_min_error;
    
    x_av_max[loopNumber] = peakToPeakVoltage/2;
    x_av_min[loopNumber] = -1*(peakToPeakVoltage/2);
    xerr_av_max[loopNumber] = peakToPeakError;
    xerr_av_min[loopNumber] = peakToPeakError;

    outTree->Fill();

    delete hist_max_zm;
    delete hist_min_zm;
    delete hist_mean;
    
    loopNumber++;

  }
  runDescriptionFile.close();
  //make plots

  Double_t  *x_av_max_min = new Double_t[2*loopNumber];
  Double_t  *y_av_max_min = new Double_t[2*loopNumber];
  Double_t  *xerr_av_max_min = new Double_t[2*loopNumber];
  Double_t  *yerr_av_max_min = new Double_t[2*loopNumber];

  for(int i=0; i<loopNumber;i++){
    x_av_max_min[i] = x_av_min[i];
    x_av_max_min[i+loopNumber] = x_av_max[i];
    y_av_max_min[i] = y_av_min[i];
    y_av_max_min[i+loopNumber] = y_av_max[i];
    xerr_av_max_min[i] = xerr_av_min[i];
    xerr_av_max_min[i+loopNumber] = xerr_av_max[i];
    yerr_av_max_min[i] = yerr_av_min[i];
    yerr_av_max_min[i+loopNumber] = yerr_av_max[i];
    
    
  }

  grMaxMin = new TGraphErrors(2*loopNumber, x_av_max_min, y_av_max_min, xerr_av_max_min, yerr_av_max_min);
  grMaxMin->SetName("gr_adc_voltage");
  char title[200];
  sprintf(title, "adc vs voltage dda %i channel %i", dda, channel);
  grMaxMin->SetTitle(title);
  grMaxMin->GetXaxis()->SetTitle("Peak input voltage (mV)");
  grMaxMin->GetYaxis()->SetTitle("ADC (ped subbed)");
  grMaxMin->Fit("pol1");
  can_max_min = new TCanvas("can_max_min", "can_max_min");
  grMaxMin->SetMarkerStyle(26);
  grMaxMin->SetMarkerSize(0.8);
  grMaxMin->SetMarkerColor(kBlue);
  grMaxMin->Draw("AP");
  char plotName[200];
  sprintf(plotName, "%s.plot.eps", outFileName);
  can_max_min->SaveAs(plotName);
  sprintf(plotName, "%s.plot.pdf", outFileName);
  can_max_min->SaveAs(plotName);
  outFile->cd();
  //  grMaxMin->SetOptFit(1111);
  //   gSystem->SetOptFit(1111);
  //   gSystem->setOptStat(0);
  grMaxMin->Write();

  
  
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
Double_t getMean(TGraph* gr){
  Double_t *xVals=gr->GetX();
  Double_t *yVals=gr->GetY();
  Int_t maxN = gr->GetN();
  
  if(maxN<1) return 0;
  Double_t mean=0;
  for(int i=0;i<maxN; i++){
    mean+=yVals[i]/maxN;
  }
  return mean;
}

Double_t getMax(TGraph *gr){
  Double_t *xVals=gr->GetX();
  Double_t *yVals=gr->GetY();
  Int_t maxN = gr->GetN();

  if(maxN<1) return 0;

  Double_t max=0;
  for(int i=0;i<maxN;i++){
    if(yVals[i]>max) max=yVals[i];
  }

  return max;

}
Double_t getMin(TGraph *gr){
  Double_t *xVals=gr->GetX();
  Double_t *yVals=gr->GetY();
  Int_t maxN = gr->GetN();

  if(maxN<1) return 0;

  Double_t min=0;
  for(int i=0;i<maxN;i++){
    if(yVals[i]<min) min=yVals[i];
  }

  return min;

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

Int_t process_run(char* runName, char* pedName, Double_t peakToPeakVoltage, Int_t dda, Int_t channel, TH1F* hist_max_zm, TH1F* hist_min_zm, TH1F* hist_mean){

  Int_t chanIndex=channel+RFCHAN_PER_DDA*dda;
  char inName[180];
  sprintf(inName,"%s", runName);

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

  calib->setAtriPedFile(pedName, stationId);//FIXME
  UsefulAtriStationEvent *realEvPtr=0;
    for(int entry=10;entry<numEntries;entry++){ 
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    realEvPtr =  new UsefulAtriStationEvent(evPtr, AraCalType::kJustPed);
    TGraph *gr = realEvPtr->getGraphFromElecChan(chanIndex);

    Double_t temp_max_adc = getMax(gr);
    Double_t temp_min_adc = getMin(gr);
    Double_t temp_mean_adc = getMean(gr);

    TGraph *grZM = zeroMean(gr);
    
    Double_t temp_max_adc_zm = getMax(grZM);
    Double_t temp_min_adc_zm = getMin(grZM);
    Double_t temp_mean_adc_zm = getMean(grZM);
    hist_mean->Fill(temp_mean_adc_zm);
    hist_max_zm->Fill(temp_max_adc_zm);
    hist_min_zm->Fill(temp_min_adc_zm);

    delete gr;
    delete realEvPtr;
  }//entry  
  std::cerr << "\n";
 
  fp->Close();

  return 0;

}
