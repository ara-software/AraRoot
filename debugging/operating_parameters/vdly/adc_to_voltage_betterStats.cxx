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
#include "TH1.h"
#include "TLine.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"

TGraph* zeroMean(TGraph*);
Double_t getMean(TGraph*);
Double_t getMax(TGraph*);
Double_t getMin(TGraph*);
TGraph *getBlockGraph(TGraph*, Int_t);
TGraph* getHalfGraph(TGraph*, Int_t);
Int_t process_run(char*, char*, Int_t, Int_t, TH1D*);
int adc_to_voltage(char*, char*, Double_t);
Double_t getPercentageMean(TH1* hist, Double_t percentage, Int_t half);

using namespace std;
int main(int argc, char **argv)
{
  if(argc<4) {
    std::cerr << "Usage: " << argv[0] << " <runList> <outFile> <percentage>\n";
    return 1;
  }
  return adc_to_voltage(argv[1], argv[2], atof(argv[3]));
}

int adc_to_voltage(char *runDescriptionFileName, char *outFileName, Double_t percentage){

  Int_t channel=0, dda=0;
  Double_t peakToPeakVoltage=0, peakToPeakError=0;

  std::fstream runDescriptionFile(runDescriptionFileName);
  char *runFileBaseName = new char[100];
  char *pedFileBaseName = new char[100];
  Int_t loop_number=0;
  Int_t runNumber=0, pedNumber=0;
  Int_t loopNumber=0;
  char histName[100];
  char canName[100];
  Double_t posVoltage[20], posAdc[20];
  Double_t negVoltage[20], negAdc[20];

  TFile *outFile = new TFile(outFileName, "RECREATE");\

  runDescriptionFile >> runFileBaseName >> pedFileBaseName;
  while(runDescriptionFile >> runNumber >> pedNumber >> peakToPeakVoltage >> peakToPeakError >> channel >> dda){

    char runFileName[100], pedFileName[100];
    sprintf(runFileName, "%s/run%i/event%i.root", runFileBaseName, runNumber, runNumber);
    sprintf(pedFileName, "%s/run_000%i/pedestalValues.run000%i.dat", pedFileBaseName, pedNumber, pedNumber);
    printf("run %s ped %s\n", runFileName, pedFileName);

    fprintf(stderr, "run %i ped %i vpp %f vpperr %f chan % i dda %i\n", runNumber, pedNumber, peakToPeakVoltage, peakToPeakError, channel, dda);

    sprintf(histName, "histAdc%i", (int)peakToPeakVoltage);
    sprintf(canName, "canAdc%i", (int)peakToPeakVoltage);
    TH1D *hist = new TH1D(histName, histName, 4096, -2048, 2048);
    process_run(runFileName, pedFileName, dda, channel, hist);

    Double_t posMean = getPercentageMean(hist, percentage, +1);
    Double_t negMean = getPercentageMean(hist, percentage, -1);
    
    TCanvas *can = new TCanvas(canName, canName);
    can->cd();
    TLine *linePos = new TLine(posMean, 0, posMean, hist->GetMaximum());
    TLine *lineNeg = new TLine(negMean, 0, negMean, hist->GetMaximum());
    linePos->SetLineColor(kRed);
    lineNeg->SetLineColor(kRed);
    //    linePos->SetLineSize(2);
    //    lineNeg->SetLineSize(2);
    hist->GetXaxis()->SetRangeUser(-500,+500);
    hist->Draw();
    lineNeg->Draw("SAME");
    linePos->Draw("SAME");
    printf("p2p %f histo %s posMean %f negMean %f\n", peakToPeakVoltage, histName, posMean, negMean);
    posVoltage[loopNumber]=peakToPeakVoltage/2.;
    negVoltage[loopNumber]=-1*peakToPeakVoltage/2.;
    posAdc[loopNumber]=posMean;
    negAdc[loopNumber]=negMean;
    
    outFile->cd();
    can->Write();
    hist->Write();
    delete hist;
    delete can;
    delete linePos;
    delete lineNeg;
    
    loopNumber++;

  }
  runDescriptionFile.close();
  //make plots

  Double_t combinedAdc[40]={0};
  Double_t combinedVoltage[40]={0};
  for(int i=0;i<loopNumber;i++){
    combinedVoltage[i]=negVoltage[i];
    combinedVoltage[i+loopNumber]=posVoltage[i];
    combinedAdc[i]=negAdc[i];
    combinedAdc[i+loopNumber]=posAdc[i];
  }
  TGraph *grVoltAdc = new TGraph(2*loopNumber, combinedVoltage, combinedAdc);
  grVoltAdc->SetName("grVoltAdc");
  outFile->cd();
  grVoltAdc->Write();



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

Int_t process_run(char* runName, char* pedName, Int_t dda, Int_t channel, TH1D* histo_adc){

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
  if(numEntries > 1000) numEntries=1000; //FIXME -- 
  numEntries=500;//DEBUG
  Int_t starEvery = numEntries / 80;


  //Now set the pedfile
  Int_t stationId=0;
  eventTree->GetEntry(0);
  stationId= evPtr->stationId;
  //  std::cerr << "stationId " << stationId << "\n";
  AraEventCalibrator *calib = AraEventCalibrator::Instance();
  calib->setAtriPedFile(pedName, stationId);

  UsefulAtriStationEvent *realEvPtr=0;
    for(int entry=100;entry<numEntries;entry++){ 
    if(entry%starEvery==0) std::cerr <<"*";
    eventTree->GetEntry(entry);
    realEvPtr =  new UsefulAtriStationEvent(evPtr, AraCalType::kJustPed);
    TGraph *gr = realEvPtr->getGraphFromElecChan(chanIndex);
    TGraph *grZM = zeroMean(gr);
    Int_t numBlocks = grZM->GetN() / SAMPLES_PER_BLOCK;
    for(int block=0;block<numBlocks;block++){
      TGraph *grBlock = getBlockGraph(grZM, block);
      
      Int_t numSamples = grBlock->GetN();
      Double_t *yVals = grBlock->GetY();
      for(int sample=0;sample<numSamples;sample++){
	histo_adc->Fill(yVals[sample]);	
      }

      delete grBlock;
    }
    delete gr;
    delete grZM;
    delete realEvPtr;
  }//entry  
  std::cerr << "\n";
 
  fp->Close();

  return 0;

}


Double_t getPercentageMean(TH1* hist, Double_t percentage, Int_t half){

  Int_t numBins = hist->GetNbinsX(); //getsize returns noBins+overflow+underflow = noBins+2
  hist->GetXaxis()->SetRange(1,numBins);
  Int_t bin0 = hist->FindBin(0);
  Double_t mean=0;
  if(half==1){
    hist->GetXaxis()->SetRange(bin0, numBins);
    Int_t maxBin = hist->GetMaximumBin();
    Double_t totalEntries = hist->Integral(bin0,numBins);
    Double_t targetEntries = totalEntries*(percentage/100.);
    for(Int_t bin=numBins;bin>=bin0;--bin){
      Double_t integral = hist->Integral(bin,numBins);
      if(integral<targetEntries){
	mean+=hist->GetBinContent(bin)*hist->GetBinCenter(bin)/targetEntries;
      }
      else{
	mean+=(integral-targetEntries)*hist->GetBinCenter(bin)/targetEntries;
	break;
      }
    }
  }//positive half
  else if(half==-1){
    hist->GetXaxis()->SetRange(1, bin0);
    Int_t maxBin = hist->GetMaximumBin();
    Double_t totalEntries = hist->Integral(1,bin0);
    Double_t targetEntries = totalEntries*(percentage/100.);
    for(Int_t bin=1;bin<=bin0;bin++){
      Double_t integral = hist->Integral(1,bin);
      if(integral<targetEntries){
	mean+=hist->GetBinContent(bin)*hist->GetBinCenter(bin)/targetEntries;
      }
      else{
	mean+=(integral-targetEntries)*hist->GetBinCenter(bin)/targetEntries;
	break;
      }
    }
  }//negative half
  else{
  }

  hist->GetXaxis()->SetRange(1,numBins); //Return the axis to normal

  return mean;
}
