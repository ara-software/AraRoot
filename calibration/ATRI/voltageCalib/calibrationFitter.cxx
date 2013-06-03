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
void calibrationFitter(char *inFileName, char *outFileName, Int_t thisBlock);
Double_t findUpperLimit(TGraph *gr);
Double_t findLowerLimit(TGraph *gr);

int main(int argc, char **argv)
{
  char inFileName[FILENAME_MAX];
  char outFileName[FILENAME_MAX];
  Int_t thisBlock=0;
  if(argc<4){
    std::cerr << "Usage: " << argv[0] << " <inFile> <outFile> <block>\n";
    return 1;
  }
  sprintf(inFileName, argv[1]);
  sprintf(outFileName, argv[2]);
  thisBlock=atoi(argv[3]);
  calibrationFitter(inFileName, outFileName, thisBlock);
  return 0;
}



void calibrationFitter(char *inFileName, char *outFileName, Int_t thisBlock){
  inFile = TFile::Open(inFileName);
  outFile = new TFile(outFileName, "RECREATE");

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
  myTF1->SetParLimits(2,1e-6,1e-4);
  myTF1->SetParLimits(3,-1e-6,1e-6);
  myTF1->SetParLimits(4,-1e-8,+1e-8);
  
  block=thisBlock;

  char grName[FILENAME_MAX];
  TGraph *grTemp;
  Int_t entry=0;
  Int_t starEvery=DDA_PER_ATRI*RFCHAN_PER_DDA*SAMPLES_PER_BLOCK/80;
  for(dda=0;dda<DDA_PER_ATRI;dda++){
    for(chan=0;chan<RFCHAN_PER_DDA;chan++){
      for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	entry++;
	if(entry%starEvery==0) std::cerr << "*";
	

	sprintf(grName, "grADCVolt_%i_%i_%i_%i", dda, chan,sample,block);
	grTemp = (TGraph*) inFile->Get(grName);

	//Prepare fitting function
	Double_t lowerLimit = findLowerLimit(grTemp);
	Double_t upperLimit = findUpperLimit(grTemp);
	lowerLimit-=500;
	upperLimit+=500;
	
	Double_t spPar0=(upperLimit+lowerLimit)/2.;
	Double_t spPar1=7e-1;
	Double_t spPar2=5e-5;
	Double_t spPar3=-9e-7;
	Double_t spPar4=7e-10;
	
	myTF1->SetParLimits(0,lowerLimit, upperLimit);
	myTF1->SetParameters(spPar0,spPar1,spPar2,spPar3,spPar4);

	//Fit
	grTemp->Fit("myTF1", "QB");
	p0=myTF1->GetParameter(0);
	p1=myTF1->GetParameter(1);
	p2=myTF1->GetParameter(2);
	p3=myTF1->GetParameter(3);
	p4=myTF1->GetParameter(4);
	chiSq=myTF1->GetChisquare();
	fitTree->Fill();
      }
    }
  }
  
  std::cerr << "\n";
  
  
  outFile->Write();
  outFile->Close();
  inFile->Close();
  
  std::cerr << "Done!\n";


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
