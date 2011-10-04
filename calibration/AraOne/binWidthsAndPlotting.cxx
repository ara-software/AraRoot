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
int firstCalibTry(int run,Double_t freq, Int_t dda, Int_t chan);
void plotEvent(Int_t run, Int_t event, Int_t dda, Int_t chan, Int_t block);  
void nextEvent();
void previousEvent();

//Global variables usd by the plotting functions
Double_t newTimeVals[2][2][SAMPLES_PER_BLOCK/2];
Int_t lastRun=0, lastEvent=0, lastDda=0, lastChan=0, lastBlock=0;
//Global canvas and graphs
TGraph *gr = NULL;
TCanvas *can = NULL;
TGraph *grHalf[2]={0};
TGraph *grHalfRaw[2]={0};


int main(int argc, char **argv)
{
  if(argc<5) {
    std::cerr << "Usage: " << argv[0] << " <run> <freq in GHz> <dda> <chan>\n";
    return 1;
  }
  return firstCalibTry(atoi(argv[1]),atof(argv[2]),atoi(argv[3]),atoi(argv[4]));
}

int firstCalibTry(int run,Double_t frequency,Int_t dda, Int_t chan)
{
  Int_t chanIndex=chan+RFCHAN_PER_DDA*dda;

  char inName[180];
  sprintf(inName,"/Users/jdavies/ara/data/hawaii2011/root/run%d/event%d.root",run,run);

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
  sprintf(histName,"sineOut_%3.0fMHz_run%d_dda%d_chan%d.root",1000*frequency,run,dda,chan);
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

  for(Long64_t i=40;i<numEntries;i++) {
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);


    //For now will assume all the ddas have the same block
    Int_t capArray=evPtr->blockVec[dda].getCapArray();

    gr = realEvent.getGraphFromElecChan(chanIndex);
    Double_t *rawT=gr->GetX();
    Double_t *rawV=gr->GetY();

    Int_t numSamples=gr->GetN();
    Int_t numBlocks=numSamples/64;


    //    histMean->Fill(mean);

    for(int block=0;block<numBlocks;block++) {
      Int_t thisCapArray=capArray;
      Double_t mean[2]={0};//gr->GetMean(2);
      if(block%2) thisCapArray=1-capArray;
      
      //jpd picking out the correct block and splitting into two halves
      //128 samples = 2 caparrays
      //64 samples (block) split into two halfs -- equivalent to splitting into the odds and evens
  
      for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	tVals[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*block];
	mean[samp%2]+=rawV[samp+SAMPLES_PER_BLOCK*block];
      }
      mean[0]/=SAMPLES_PER_BLOCK/2;
      mean[1]/=SAMPLES_PER_BLOCK/2;
      

      for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	tVals[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*block];
	vVals[samp%2][samp/2]=rawV[samp+SAMPLES_PER_BLOCK*block]-mean[samp%2];
      }

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

      //jpd previous section just counts total ZCs in the two halves
      //jpd require that the number in the two halves is ~equal
      if(TMath::Abs(countZc[0]-countZc[1])<2) {
	entryVec.push_back(i);
	//      std::cerr << i << "\t" <<countZc[0] << "\t" << countZc[1] << "\t" << mean << "\n";
	
	numEvents[thisCapArray]++;
	for(int half=0;half<2;half++) {
	  for(int samp=0;samp<(SAMPLES_PER_BLOCK/2)-1;samp++) {
	    Double_t val1=vVals[half][samp];
	    Double_t val2=vVals[half][samp+1];
	    if(val1<0 && val2>0) {
	      histZC[thisCapArray][half]->Fill(samp);
	      histBinWidth[thisCapArray][half]->Fill(samp);
	    }
	    else if(val1>0 && val2<0) {
	      histZC[thisCapArray][half]->Fill(samp);
	      histBinWidth[thisCapArray][half]->Fill(samp);
	    }
	    else if(val1==0 || val2==0) {
	      histZC[thisCapArray][half]->Fill(samp,0.5);
	      histBinWidth[thisCapArray][half]->Fill(samp,0.5);
	    }	
	  }
	}     
      }
      delete grHalf[0];
      grHalf[0] = NULL;
      delete grHalf[1];
      grHalf[1] = NULL;
    }

    delete gr;
    gr=NULL;
  }
  
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      histBinWidth[capArray][half]->Scale(1./numEvents[capArray]);
      histBinWidth[capArray][half]->Scale(0.5/frequency);
    }
  }
  //jpd at this stage we have produced a set of calibration constants contained in histBinWidth[capArray][half]


  //Step two is to work out the interleave factor

  std::cerr << "\n";

  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      Double_t time=0;
      for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
	newTimeVals[capArray][half][samp]=time;
	time+=histBinWidth[capArray][half]->GetBinContent(samp+1);
      }
      std::cout << "Mean : " << capArray << "\t" << half << "\t" << time/32. << "\n";
    }
  }

  //jpd print em all to screen
  for(int capArray=0;capArray<2;capArray++) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
      printf("%i\t%i\t%0.3f\t%0.3f\t%0.3f\t%0.3f\n", capArray, samp, newTimeVals[capArray][0][samp], newTimeVals[capArray][1][samp], newTimeVals[capArray][0][samp]-newTimeVals[capArray][0][samp-1] , newTimeVals[capArray][1][samp]-newTimeVals[capArray][1][samp-1]);
    }
  }



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

void plotEvent(Int_t run, Int_t event, Int_t dda, Int_t chan, Int_t block){
  Int_t chanIndex=chan+RFCHAN_PER_DDA*dda;
  lastRun=run;
  lastEvent=event;
  lastDda=dda;
  lastChan=chan;
  lastBlock=block;
  //delete all the previous canvas / graphs
  if(gr)
    delete gr;
  if(grHalf[0])
    delete grHalf[0];
  if(grHalf[1])
    delete grHalf[1];
  if(grHalfRaw[0])
    delete grHalfRaw[0];
  if(grHalfRaw[1])
    delete grHalfRaw[1];
  if(can)
    delete can;

  char inName[180];
  sprintf(inName,"/Users/jdavies/ara/data/hawaii2011/root/run%d/event%d.root",run,run);

  TFile *fp = new TFile(inName);
  
  TTree *eventTree = (TTree*) fp->Get("eventTree");

  RawAraOneStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr); 

  eventTree->GetEntry(event);
  UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);
  
  //For now will assume all the ddas have the same block
  Int_t capArray=evPtr->blockVec[0].getCapArray();
  Int_t thisCapArray=0;  
  
  gr = realEvent.getGraphFromElecChan(chanIndex);
  Double_t *rawT=gr->GetX();
  Double_t *rawV=gr->GetY();
  Double_t mean[2]={0};
  Int_t numSamples=gr->GetN();
  
  // if(numSamples>6*SAMPLES_PER_BLOCK){
  //   cout << "Too many samples to plot!" << endl;
  //   return;
  // }
  Int_t numBlocks=numSamples/64;
  Double_t tVals[2][32];
  Double_t tValsRaw[2][32];
  Double_t vVals[2][32];
  
  printf("Analysing run %i event %i dda %i channel %i\n", run, event, dda, chan);
  printf("%i samples %i blocks\n", numSamples, numBlocks);
  
  can = new TCanvas();//"can","can",600,600);
  can->Divide(1,5);
  can->cd(1);
  gr->SetMarkerStyle(22);
  gr->Draw("alp");
  gr->GetXaxis()->SetRangeUser(block*SAMPLES_PER_BLOCK+1, (block+2)*SAMPLES_PER_BLOCK-1);
  gr->GetXaxis()->SetLabelSize(0.04);
  
  for(int thisBlock=block;thisBlock<block+2;thisBlock++) {
    mean[0]=0;
    mean[1]=0;
    thisCapArray=capArray;
    if(thisBlock%2) thisCapArray=1-capArray;

    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tVals[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*thisBlock];
      tValsRaw[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*thisBlock];
      mean[samp%2]+=rawV[samp+SAMPLES_PER_BLOCK*thisBlock];
    }
    mean[0]/=SAMPLES_PER_BLOCK/2;
    mean[1]/=SAMPLES_PER_BLOCK/2;
    
    
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tVals[samp%2][samp/2]=newTimeVals[thisCapArray][samp%2][samp/2];
      vVals[samp%2][samp/2]=rawV[samp+SAMPLES_PER_BLOCK*thisBlock]-mean[samp%2];
    }
    
    char graphName[180];
    for(int half=0;half<2;half++) {
      grHalf[half] = new TGraph(SAMPLES_PER_BLOCK/2,tVals[half],vVals[half]);
      grHalfRaw[half] = new TGraph(SAMPLES_PER_BLOCK/2,tValsRaw[half],vVals[half]);      
      
      sprintf(graphName, "thisBlock %i half %i", thisBlock+1, half+1);
  
      grHalf[half]->SetNameTitle(graphName, graphName);
      sprintf(graphName, "thisBlock %i half %i - Raw",thisBlock+1, half+1);
  
      grHalfRaw[half]->SetNameTitle(graphName, graphName);
      
    }
    can->cd((thisBlock-block)*2+2);
    grHalf[0]->SetMarkerStyle(22);
    grHalf[0]->Draw("alp"); //even samples
     //can->cd(3);
     grHalf[1]->SetLineColor(kGreen+2);
     grHalf[1]->SetMarkerColor(kGreen+2);
     grHalf[1]->SetMarkerStyle(22);
     grHalf[1]->Draw("lp"); //odd samples
    
    
     can->cd((thisBlock-block)*2+3);
    grHalfRaw[0]->SetLineColor(1);
    grHalfRaw[0]->SetMarkerColor(1);
    grHalfRaw[0]->SetMarkerStyle(22);
    grHalfRaw[0]->Draw("alp");
    //can->cd(3);
    grHalfRaw[1]->SetLineColor(kGreen+2);
    grHalfRaw[1]->SetMarkerColor(kGreen+2);
    grHalfRaw[1]->SetMarkerStyle(22);
    grHalfRaw[1]->Draw("lp");

    
    grHalfRaw[0]->GetXaxis()->SetLabelSize(0.04);
    grHalfRaw[1]->GetXaxis()->SetLabelSize(0.04);
        

  }

  delete fp;

}


void nextEvent(){
  

  plotEvent(lastRun, lastEvent+1, lastDda, lastChan, lastBlock);

}
void previousEvent(){

  plotEvent(lastRun, lastEvent-1, lastDda, lastChan, lastBlock);

}
/*
TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
TCanvas *can = new TCanvas();//"can","can",600,600);
TGraph *grHalf[2]={0};
TGraph *grHalfRaw[2]={0};
 
*/
