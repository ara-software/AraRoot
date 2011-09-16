#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraOneStationEvent.h"
#include "RawAraOneStationEvent.h"
#include "araSoft.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include "TEventList.h"
#include "TMath.h"
#include "TCanvas.h"
#include <TGClient.h>


Double_t estimateLag(TGraph *grIn, Double_t freq);
int firstCalibTry(int run,Double_t freq);

int main(int argc, char **argv)
{
  if(argc<3) {
    std::cerr << "Need to give a run number and a frequency (in GHz)\n";
    return 1;
  }
  return firstCalibTry(atoi(argv[1]),atof(argv[2]));
}

int firstCalibTry(int run,Double_t frequency)
{
  char inName[180];
  sprintf(inName,"/Users/rjn/ara/data/ohio2011/root/run%d/event%d.root",run,run);

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
  RawAraOneStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);

  Double_t ampmv=5;

  char histName[180];
  sprintf(histName,"sineOut_%3.0fMHz_run%d.root",1000*frequency,run);
  TFile *histFile = new TFile(histName,"RECREATE");

  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;

  Double_t tVals[2][32];
  Double_t vVals[2][32];


  //Step one is to determine the 2-times 2x32 individual timing constants for the capicators

  Int_t numEvents[2]={0};  //Array is the capArray
  TH1F *histZC[2][2];
  TH1F *histBinWidth[2][2]; 
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      sprintf(histName,"histZC%d_%d",capArray,half);
      histZC[capArray][half] = new TH1F(histName,histName,SAMPLES_PER_BLOCK/2,-0.5,(SAMPLES_PER_BLOCK/2)-0.5);
      sprintf(histName,"histBinWidth%d_%d",capArray,half);
      histBinWidth[capArray][half] = new TH1F(histName,histName,SAMPLES_PER_BLOCK/2,-0.5,(SAMPLES_PER_BLOCK/2)-0.5);
    }
  }

  TH1F *histMean = new TH1F("histMean","histMean",1000,-500,500);

  std::vector <Long64_t> entryVec;

  for(Long64_t i=10;i<numEntries;i++) {
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    if(evPtr->blockVec[0].getBlock()==0) continue;
    UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);


    //For now will assume all the ddas have the same block
    Int_t capArray=evPtr->blockVec[0].getCapArray();

    TGraph *gr = realEvent.getGraphFromElecChan(16);
    Double_t *rawT=gr->GetX();
    Double_t *rawV=gr->GetY();
    Double_t mean=gr->GetMean(2);

    //    histMean->Fill(mean);

    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tVals[samp%2][samp/2]=rawT[samp];
      vVals[samp%2][samp/2]=rawV[samp]-mean;
    }

    TGraph *grHalf[2]={0};
    for(int half=0;half<2;half++) {
      grHalf[half] = new TGraph(SAMPLES_PER_BLOCK/2,tVals[half],vVals[half]);
    }

    // TCanvas *can = new TCanvas("can","can",600,600);
    // can->Divide(1,3);
    // can->cd(1);
    // gr->Draw("alp");
    // can->cd(2);
    // grHalf[0]->Draw("alp");
    // can->cd(3);
    // grHalf[1]->Draw("alp");
    // return 1;

    Double_t countZc[2]={0};
    for(int half=0;half<2;half++) {
      for(int samp=0;samp<(SAMPLES_PER_BLOCK/2)-1;samp++) {
	Double_t val1=vVals[half][samp];
	Double_t val2=vVals[half][samp+1];
	if(val1<0 && val2>0) {
	  countZc[half]++;
	}
	else if(val1>0 && val2<0) {
	  countZc[half]++;
	}
	else if(val1==0 || val2==0) {
	  countZc[half]+=0.5;
	}	
      }
    }
    if(TMath::Abs(countZc[0]-countZc[1])<2) {
      entryVec.push_back(i);
      //      std::cerr << i << "\t" <<countZc[0] << "\t" << countZc[1] << "\t" << mean << "\n";
      
      numEvents[capArray]++;
      for(int half=0;half<2;half++) {
	for(int samp=0;samp<(SAMPLES_PER_BLOCK/2)-1;samp++) {
	  Double_t val1=vVals[half][samp];
	  Double_t val2=vVals[half][samp+1];
	  if(val1<0 && val2>0) {
	    histZC[capArray][half]->Fill(samp);
	    histBinWidth[capArray][half]->Fill(samp);
	  }
	  else if(val1>0 && val2<0) {
	    histZC[capArray][half]->Fill(samp);
	    histBinWidth[capArray][half]->Fill(samp);
	  }
	  else if(val1==0 || val2==0) {
	    histZC[capArray][half]->Fill(samp,0.5);
	    histBinWidth[capArray][half]->Fill(samp,0.5);
	  }	
	}
      }     
    }

    
    delete gr;
    delete grHalf[0];
    delete grHalf[1];

  }
  
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      histBinWidth[capArray][half]->Scale(1./numEvents[capArray]);
      histBinWidth[capArray][half]->Scale(0.5/frequency);
    }
  }

  //Step two is to work out the interleave factor

  std::cerr << "\n";
  Double_t newTimeVals[2][2][SAMPLES_PER_BLOCK/2];
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      Double_t time=0;
      for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
	newTimeVals[capArray][half][samp]=time;
	time+=histBinWidth[capArray][half]->GetBinContent(samp+1);
      }
      std::cout << "Mean : " << capArray << "\t" << half << "\t" << time/64. << "\n";
    }
  }

  std::cout << "Desired rate is " << 25*128 << " MHz\t" << (1000./(25*128)) << "ns\n";

  // for(int capArray=0;capArray<2;capArray++) {
  //   int totalZc[2]={0};
  //   for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
  //     totalZc[0]+=histZC[capArray][0]->GetBinContent(samp+1);
  //     totalZc[1]+=histZC[capArray][1]->GetBinContent(samp+1);
  //     std::cout << capArray << "\t" << samp << "\t" 
  // 		<< histZC[capArray][0]->GetBinContent(samp+1)
  // 		<< "\t" <<  histZC[capArray][1]->GetBinContent(samp+1) << "\t"
  // 		<< totalZc[0] << "\t" << totalZc[1] << "\n";

      
  //   }
  // }

  for(int capArray=0;capArray<2;capArray++) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
      std::cout << capArray << "\t" << samp << "\t" << newTimeVals[capArray][0][samp]
		<< "\t" << newTimeVals[capArray][1][samp] << "\n";
    }
  }



  TH1F *histLag[2];
  for(int capArray=0;capArray<2;capArray++) {
    sprintf(histName,"histLag%d",capArray);
    histLag[capArray] = new TH1F(histName,histName,1000,-0.5,0.5);
  }

  std::vector<Long64_t>::iterator entryIt;

  //  for(Long64_t i=0;i<numEntries;i++) {
  for(entryIt=entryVec.begin();entryIt!=entryVec.end();entryIt++) {
    Long64_t i=*entryIt;
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    if(evPtr->blockVec[0].getBlock()==0) continue;
    UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);

    //For now will assume all the ddas have the same block
    Int_t capArray=evPtr->blockVec[0].getCapArray();
    

    TGraph *gr = realEvent.getGraphFromElecChan(16);
    //    Double_t *rawT=gr->GetX();
    Double_t *rawV=gr->GetY();
    
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tVals[samp%2][samp/2]=newTimeVals[capArray][samp%2][samp/2];
      vVals[samp%2][samp/2]=rawV[samp];
    }

    TGraph *grHalf[2]={0};
    for(int half=0;half<2;half++) {
      grHalf[half] = new TGraph(SAMPLES_PER_BLOCK/2,tVals[half],vVals[half]);
    }

    // TCanvas *can = new TCanvas();//"can","can",600,600);
    // can->Divide(1,2);
    // can->cd(1);
    // gr->Draw("alp");
    // can->cd(2);
    // grHalf[0]->Draw("alp");
    // //    can->cd(3);
    // grHalf[1]->SetLineColor(kGreen+2);
    // grHalf[1]->SetMarkerColor(kGreen+2);
    // grHalf[1]->Draw("lp");
    Double_t lag0=estimateLag(grHalf[0],frequency);
    Double_t lag1=estimateLag(grHalf[1],frequency);

    Double_t deltaLag=lag0-lag1;
    while(TMath::Abs(deltaLag-1.0/frequency)<TMath::Abs(deltaLag))
      deltaLag-=1./frequency;

    histLag[capArray]->Fill(deltaLag);
   
    delete gr;
    delete grHalf[0];
    delete grHalf[1];


  }
  std::cerr << "\n";

  Int_t dda=0;
  Int_t chan=3;

  Double_t deltaLag[2]={histLag[0]->GetMean(),histLag[1]->GetMean()};

  Double_t timeVals[2][SAMPLES_PER_BLOCK];
  Int_t indexVals[2][SAMPLES_PER_BLOCK];


  for(int capArray=0;capArray<2;capArray++) { 
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      if(samp%2==0)
	timeVals[capArray][samp]=newTimeVals[capArray][samp%2][samp/2];
      else 
	timeVals[capArray][samp]=newTimeVals[capArray][samp%2][samp/2]+deltaLag[capArray];
    }
    TMath::Sort(SAMPLES_PER_BLOCK,timeVals[capArray],indexVals[capArray],kFALSE);
  }
  
  
  

  std::ofstream OutFile("sampleTiming.txt");
  for(int capArray=0;capArray<2;capArray++) {
    OutFile << dda << "\t" << chan << "\t" << capArray << "\t";   
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      if(samp%2==0)
	OutFile << indexVals[capArray][samp] << " ";
      else
	OutFile << indexVals[capArray][samp]  << " ";
    }
    OutFile << "\n";
    OutFile << dda << "\t" << chan << "\t" << capArray << "\t";   
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      if(samp%2==0)
	OutFile << timeVals[capArray][indexVals[capArray][samp]] << " ";
      else
	OutFile << timeVals[capArray][indexVals[capArray][samp]]  << " ";
    }
    OutFile << "\n";
  }
  OutFile.close();



  histFile->Write();

      
  return 0;
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


