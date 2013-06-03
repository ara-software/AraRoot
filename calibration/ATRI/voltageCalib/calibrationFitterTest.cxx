#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TGraph.h"

#include "araSoft.h"


TFile *inFile;
TFile *outFile;
TF1* myTF1;
Double_t myFunction(Double_t *x, Double_t *par);
void calibrationFitter(char *inFileName, char *outFileName);
Double_t findUpperLimit(TGraph *gr);
Double_t findLowerLimit(TGraph *gr);

int main(int argc, char **argv)
{
  char inFileName[FILENAME_MAX];
  char outFileName[FILENAME_MAX];

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " <inFile> <outFile>  \n";
    return 1;
  }
  sprintf(inFileName, argv[1]);
  sprintf(outFileName, argv[1]);

  calibrationFitter(inFileName, outFileName);
  return 0;
}



void calibrationFitter(char *inFileName, char *outFileName){
  inFile = TFile::Open(inFileName);

  Int_t dda=0,chan=0,sample=0,block=0;
  Double_t p0=0,p1=0,p2=0,p3=0,p4=0,chiSq=0;

  TTree *fitTree = new TTree("fitTree", "Tree containing fit parameters for ADC to mV conversion");
  fitTree->Branch("dda", &dda, "dda/I");
  fitTree->Branch("chan", &chan, "chan/I");
  fitTree->Branch("sample", &sample, "sample/I");
  fitTree->Branch("block", &block, "block/I");

  fitTree->Branch("p0", &p0, "p0/D");
  fitTree->Branch("p1", &p1, "p1/D");
  fitTree->Branch("p2", &p2, "p2/D");
  fitTree->Branch("p3", &p3, "p3/D");
  fitTree->Branch("p4", &p4, "p4/D");

  fitTree->Branch("chiSq", &chiSq, "chiSq/D");

  myTF1 = new TF1("myTF1", myFunction,0,4096,5);
  myTF1->SetParLimits(0,0,4095);
  myTF1->SetParLimits(1,0.3,0.9);
  myTF1->SetParLimits(2,1e-5,1e-4);
  myTF1->SetParLimits(3,-1e-6,1e-6);
  myTF1->SetParLimits(4,-1e-8,+1e-8);

}


Double_t myFunction(Double_t *x, Double_t *par){

  Double_t ADC=x[0];
  Double_t mV=0;
  if(ADC<par[0]){
    mV = par[1]*(ADC-par[0])
      + par[2]*TMath::Power((ADC-par[0]),2);
  }
  else{
    mV = par[1]*(ADC-par[0]) 
      + par[2]*TMath::Power((ADC-par[0]),2)
      + par[3]*TMath::Power((ADC-par[0]),3)
      + par[4]*TMath::Power((ADC-par[0]),4);
  }

  return mV;

}
Double_t findUpperLimit(TGraph *gr){
  Double_t *yVals=gr->GetY();
  Double_t *xVals=gr->GetX();
  Int_t numVals=gr->GetN();
  Double_t maxADC=0;
  for(Int_t bin=0;bin<numVals;bin++){
    if(TMath::Abs(yVals[bin])<1e-3){
      if(xVals[bin]>maxADC) maxADC=xVals[bin];
    }
  }
  
  
  return maxADC;
  
}
Double_t findLowerLimit(TGraph *gr){
  Double_t *yVals=gr->GetY();
  Double_t *xVals=gr->GetX();
  Int_t numVals=gr->GetN();
  Double_t minADC=4095;
  for(Int_t bin=0;bin<numVals;bin++){
    if(TMath::Abs(yVals[bin])<1e-3){
      if(xVals[bin]<minADC) minADC=xVals[bin];
    }
  }
  return minADC;
  
}
