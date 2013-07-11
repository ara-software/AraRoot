////////////////////////////////////////////////////////////////////////
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
#include "TF1.h"
#include "TTree.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TGraph.h"
#include "TMultiGraph.h"


//Standard Includes
#include <iostream>
#include <fstream>


#define MAX_ADC 4096

//Prototype Functions
Int_t getADCmVConversion(char *baseName, Int_t runLow, Int_t runHigh, Int_t thisBlock, Int_t thisDda);
TGraph *getZeroSubGraph(TGraph *gr, Int_t factor);
TGraph *addGraphs(TGraph *grOne, TGraph *grTwo);

int main(int argc, char **argv)
{
  Int_t runLow, runHigh, block, dda;
  char baseName[FILENAME_MAX];

  if(argc<6) {
    std::cerr << "Usage: " << argv[0] << " <baseDir> <runLow> <runHigh> <block> <dda> \n";
    return 1;
  }
  strncpy(baseName, argv[1],FILENAME_MAX);
  runLow=atoi(argv[2]);
  runHigh=atoi(argv[3]);
  block=atoi(argv[4]);
  dda=atoi(argv[5]);
  getADCmVConversion(baseName, runLow, runHigh, block, dda);

}






Int_t getADCmVConversion(char *baseName, Int_t runLow, Int_t runHigh, Int_t thisBlock, Int_t thisDda){

  TChain *chain = new TChain("maxPosNegTree");
  char runName[FILENAME_MAX];
  for(int run=runLow; run<=runHigh;run++){
    sprintf(runName, "%s/root/run%i/adcSampleBlock.root", baseName, run);
    chain->Add(runName);
  }
  char outFileName[FILENAME_MAX];
  sprintf(outFileName, "%s/root/voltageCalibThursday/ADCmVConversionTest_block%idda%i.root", baseName, thisBlock, thisDda);
  
  TFile *outFile = new TFile(outFileName, "RECREATE");

  Int_t dda=0, chan=0, sample=0, block=0, posNeg=0, ndf=0;
  Double_t maxPosValue=0, maxNegValue=0, voltage=0;
  Double_t p0=0, p1=0, p2=0, p3=0, p4=0, chiSq=0;
  

  
  char exp[100];
  char cut[100];
  char grName[100];
  TGraph *grTemp;
  TCanvas *tempCan;

  TF1* funcPos = new TF1("myPosPol3", "pol3");
  TF1* funcNeg = new TF1("myNegPol1", "pol1");
  funcPos->SetParLimits(1, 0,2);
  funcPos->SetParLimits(2, -1,+1);
  funcPos->SetParLimits(3, -1,+1);
  funcNeg->SetParLimits(1, 0,+2);
  funcPos->FixParameter(0,0);
  funcNeg->FixParameter(0,0);
	



  TTree *fitTree = new TTree("fitTree", "Tree of fit parameters");
  fitTree->Branch("dda", &dda, "dda/I");
  fitTree->Branch("chan", &chan, "chan/I");
  fitTree->Branch("sample", &sample, "sample/I");
  fitTree->Branch("block", &block, "block/I");
  fitTree->Branch("ndf", &ndf, "ndf/I");
  fitTree->Branch("posNeg", &posNeg, "posNeg/I");

  fitTree->Branch("p0", &p0, "p0/D");
  fitTree->Branch("p1", &p1, "p1/D");
  fitTree->Branch("p2", &p2, "p2/D");
  fitTree->Branch("p3", &p3, "p3/D");
  fitTree->Branch("chiSq", &chiSq, "chiSq/D");



  block=thisBlock;
  dda=thisDda;
  //  for(dda=0;dda<1;dda++){
    for(chan=0;chan<1;chan++){
      //      fprintf(stderr, "dda %i chan %i ", dda, chan);
      for(sample=40;sample<45;sample++){

	if((dda==1 || dda==2)&&(chan>3)) continue;
	//Reset starting point of parameters
	funcPos->SetParameter(1, 1.0);
	funcPos->SetParameter(2, -0.001);
	funcPos->SetParameter(3, 0);
	funcNeg->SetParameter(1, 0.55);

	//	fprintf(stderr, "%i ", sample);
	sprintf(exp, "voltage/2:maxPosValue");
	sprintf(cut, "dda==%i&&chan==%i&&block==%i&&sample==%i", dda, chan, block, sample);
	sprintf(grName, "grPos_%i_%i_%i_%i", dda, chan, block, sample);
	tempCan = new TCanvas();
	chain->Draw(exp, cut);
	grTemp = (TGraph*)tempCan->GetPrimitive("Graph");
	//	TGraph *grPosTemp = (TGraph*) grTemp->Clone();
	TGraph *grPosTemp = getZeroSubGraph(grTemp, +1);

	grPosTemp->SetName(grName);
	grPosTemp->Fit("myPosPol3", "QBW");
	p0=funcPos->GetParameter(0);
	p1=funcPos->GetParameter(1);
	p2=funcPos->GetParameter(2);
	p3=funcPos->GetParameter(3);
	chiSq =funcPos->GetChisquare();
	ndf = funcPos->GetNDF();
	p4=0;
	posNeg=+1;
	printf("%i %i %i %i %f %f\n", dda, chan, block, sample, p1, p2);
	fitTree->Fill();
	delete tempCan;
	
	
	tempCan = new TCanvas();
	sprintf(exp, "-1*voltage/2:maxNegValue");
	sprintf(grName, "grNeg_%i_%i_%i_%i", dda, chan, block, sample);	  
	chain->Draw(exp, cut);
	grTemp = (TGraph*)tempCan->GetPrimitive("Graph");
	//	TGraph *grNegTemp = (TGraph*) grTemp->Clone();
	TGraph *grNegTemp = getZeroSubGraph(grTemp, -1);

	grNegTemp->SetName(grName);
	grNegTemp->Fit("myNegPol1", "QBW");
	p0=funcNeg->GetParameter(0);
	p1=funcNeg->GetParameter(1);
	chiSq=funcNeg->GetChisquare();
	p2=0;
	p3=0;
	p4=0;
	// p0=grNegTemp->GetFunction("myNegPol1")->GetParameter(0);
	// p1=grNegTemp->GetFunction("myNegPol1")->GetParameter(1);
	// chiSq = grNegTemp->GetFunction("myNegPol1")->GetChisquare();

	posNeg=-1;
	printf("%i %i %i %i %f %f\n", dda, chan, block, sample, p1, p2);
	fitTree->Fill();
	delete tempCan;
	

	outFile->cd();
	grPosTemp->Write();
	grNegTemp->Write();
	//	grPosNeg->Write();

	//	delete grPosNeg;
	delete grNegTemp;	
	delete grPosTemp;


      }
      fprintf(stderr, "\n");
    }
    //  }

  outFile->cd();
  fitTree->Write();

  fprintf(stderr, "Done!\n");

  delete funcPos;
  delete funcNeg;


  return 0;

}



TGraph *getZeroSubGraph(TGraph *gr, Int_t factor){
  Int_t numEntries = gr->GetN();
  Double_t *yVals=gr->GetY();
  Double_t *xVals=gr->GetX();
  
  if(numEntries<=0) return NULL;
  Double_t *xValsNew = new Double_t[numEntries];
  Double_t xValZero=0;

  for(int entry=0;entry<numEntries;entry++){
    if(yVals[entry]==0){
      xValZero=xVals[entry];
      break;
    }
  }
  for(int entry=0;entry<numEntries;entry++){
    Double_t newX = TMath::Sqrt( TMath::Power(xVals[entry],2) - TMath::Power(xValZero,2) );
    xValsNew[entry] = factor*newX;
  }

  TGraph *grNew = new TGraph(numEntries, xValsNew, yVals);


  delete [] xValsNew;

  return grNew;

}

TGraph *addGraphs(TGraph *grOne, TGraph *grTwo){
  Int_t numEntriesOne = grOne->GetN();
  Int_t numEntriesTwo = grTwo->GetN();
  Double_t *yVals1 = grOne->GetY();
  Double_t *xVals1 = grOne->GetX();
  Double_t *yVals2 = grTwo->GetY();
  Double_t *xVals2 = grTwo->GetX();

  Double_t *yValsNew = new Double_t[numEntriesOne+numEntriesTwo];
  Double_t *xValsNew = new Double_t[numEntriesOne+numEntriesTwo];

  for(int entry = 0;entry<numEntriesOne;entry++){
    xValsNew[entry] = xVals1[entry];
    yValsNew[entry] = yVals1[entry];
  }
  for(int entry = 0;entry<numEntriesTwo;entry++){
    xValsNew[entry+numEntriesOne] = xVals2[entry];
    yValsNew[entry+numEntriesOne] = yVals2[entry];
  }

  TGraph *grNew = new TGraph(numEntriesOne+numEntriesTwo, xValsNew, yValsNew);

  delete [] yValsNew;
  delete [] xValsNew;

  return grNew;
}
