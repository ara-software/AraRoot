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
Double_t estimateFirstZC(TGraph *grIn, Double_t freq);
Double_t estimateLastZC(TGraph *grIn, Double_t freq);
int firstCalibTry(int run,Double_t freq, Int_t dda, Int_t chan);
void plotEvent(Int_t event, Int_t block);  
void nextEvent();
void previousEvent();

Double_t newTimeValsUnsorted[2][2][SAMPLES_PER_BLOCK/2];
Double_t newTimeVals[2][2][SAMPLES_PER_BLOCK/2];
Double_t newTimeValsEpsilon[2][2][SAMPLES_PER_BLOCK/2];
Double_t newEpsilon[2];
Int_t lastRun=0, lastEvent=0, lastDda=0, lastChan=0, lastBlock=0;

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
  lastRun=run;
  lastDda=dda;
  lastChan=chan;
  //1. calculate the bin to bin widths for the odd and even samples in the two halves
  //this is only going to be done for one channel on one dda - wrapped into chanIndex
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
  char histName[180];
  sprintf(histName,"sineOut_%3.0fMHz_run%d_dda%d_chan%d_new.root",1000*frequency,run,dda,chan);
  TFile *histFile = new TFile(histName,"RECREATE");

  Long64_t numEntries=eventTree->GetEntries();

  cout << "Number of entries in file is " << numEntries << endl;

  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;

  Double_t tVals[2][32];
  Double_t vVals[2][32];

  Int_t numEvents[2]={0}; //used to scale the bin widths
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

  TH1F *histMean = new TH1F("histMean","histMean",1000,-0.1,0.1);

  std::vector <Long64_t> entryVec;

  for(Long64_t i=0;i<numEntries;i++) {
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);

    //all the ddas block zero will be the same block
    //the capArray will toggle as we move through the event
    Int_t capArray=evPtr->blockVec[0].getCapArray();

    TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
    Double_t *rawT=gr->GetX();
    Double_t *rawV=gr->GetY();

    Int_t numSamples=gr->GetN();
    Int_t numBlocks=numSamples/64;

    for(int block=0;block<numBlocks;block++) {
      Int_t thisCapArray=capArray;
      Double_t mean[2]={0};//gr->GetMean(2);
      if(block%2) thisCapArray=1-capArray;
      
      //jpd picking out the correct block and splitting into two halves
      //128 samples = 2 caparrays
      //64 samples (block) split into two halfs -- equivalent to splitting into the odds and evens
  
      for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	tVals[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*block];
	mean[samp%2]+=rawV[samp+SAMPLES_PER_BLOCK*block]*2/(SAMPLES_PER_BLOCK);
      }

      //zero-mean the waveforms
      for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	vVals[samp%2][samp/2]=rawV[samp+SAMPLES_PER_BLOCK*block]-mean[samp%2];
      }

      TGraph *grHalf[2]={0};
      for(int half=0;half<2;half++) {
	grHalf[half] = new TGraph(SAMPLES_PER_BLOCK/2,tVals[half],vVals[half]);
	histMean->Fill(grHalf[half]->GetMean(2));
      }

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
      delete grHalf[1];
    }

    delete gr;
  }
  
  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      histBinWidth[capArray][half]->Scale(1./numEvents[capArray]);
      histBinWidth[capArray][half]->Scale(0.5/frequency);
    }
  }
  std::cerr << "\n";

  for(int capArray=0;capArray<2;capArray++) {
    for(int half=0;half<2;half++) {
      Double_t time=0;
      for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
	newTimeVals[capArray][half][samp]=time;
	time+=histBinWidth[capArray][half]->GetBinContent(samp+1);
      }
      std::cout << "Mean : " << capArray << " " << half << " " << time/32. << "\n";
    }
  }

  cout << "-----------------Done bin widths-----------------" << endl;
  cout << "cap     samp Time even Time odd" << endl;

  //jpd print em all to screen
  for(int capArray=0;capArray<2;capArray++) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
      printf("%i\t%i\t%0.3f\t%0.3f\n", capArray, samp, newTimeVals[capArray][0][samp], newTimeVals[capArray][1][samp]);
    }
  }

  cout << "Doing Interleave timing" << endl;

  //2. now calculate the interleave times

  TTree *lagTree = new TTree("lagTree","lagTree");
  Double_t lag1,lag0,deltaLagg;
  Int_t block, thisCapArray;
  lagTree->Branch("block",&block,"block/I");
  lagTree->Branch("capArray",&thisCapArray,"capArray/I");
  lagTree->Branch("lag1",&lag1,"lag1/D");
  lagTree->Branch("lag0",&lag0,"lag0/D");
  lagTree->Branch("deltaLag",&deltaLagg,"deltaLag/D");

  TH1F *histLag[2];
  for(int capArray=0;capArray<2;capArray++) {
    sprintf(histName,"histLag%d",capArray);
    histLag[capArray] = new TH1F(histName,histName,10000,-5,5);
  }

  std::vector<Long64_t>::iterator entryIt;

  for(Long64_t i=0;i<numEntries;i++) {
  //  for(entryIt=entryVec.begin();entryIt!=entryVec.end();entryIt++) {
  //  for(int i =0; i<numEntries; i++){
  //for(int i=100;i<101;i++){
    //    Long64_t i=*entryIt;
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    if(evPtr->blockVec[0].getBlock()==0) continue;
    UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);

    //For now will assume all the ddas have the same block
    Int_t capArray=evPtr->blockVec[0].getCapArray();
   
    TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
    Double_t *rawV=gr->GetY();
    Int_t numSamples=gr->GetN();
    Int_t numBlocks=numSamples/64;

    for(block=0;block<numBlocks;block++) {
      Double_t mean[2]={0};
 
      thisCapArray=capArray;
      if(block%2) thisCapArray=1-capArray;
     
      for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	mean[samp%2]+=rawV[samp+SAMPLES_PER_BLOCK*block];
      }
      mean[0]/=SAMPLES_PER_BLOCK/2;
      mean[1]/=SAMPLES_PER_BLOCK/2;
      
      //zero-mean the waveforms
      for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	tVals[samp%2][samp/2]=newTimeVals[thisCapArray][samp%2][samp/2];
	vVals[samp%2][samp/2]=rawV[samp+SAMPLES_PER_BLOCK*block]-mean[samp%2];
      }
      
      TGraph *grHalf[2]={0};
      for(int half=0;half<2;half++) {
	grHalf[half] = new TGraph(SAMPLES_PER_BLOCK/2,tVals[half],vVals[half]);
      }

      lag0=estimateLag(grHalf[0],frequency);
      lag1=estimateLag(grHalf[1],frequency);
           
      deltaLagg=lag0-lag1;
      while(TMath::Abs(deltaLagg-1.0/frequency)<TMath::Abs(deltaLagg))
	deltaLagg-=1./frequency;
      while(TMath::Abs(deltaLagg+1.0/frequency)<TMath::Abs(deltaLagg))
	deltaLagg+=1./frequency;
      

      //Arbitrary to make sample 1 after sample zero
      // if(deltaLagg<0) 
      // 	deltaLagg+=1./frequency;

      lagTree->Fill();

      histLag[thisCapArray]->Fill(deltaLagg);
      
      // delete grHalf[0];
      //      delete grHalf[1];

      //     printf("lag0 %0.3f lag1 %0.3f deltaLag %0.3f deltaLag\n", lag0, lag1, deltaLagg);

      // TCanvas *can = new TCanvas();//"can","can",600,600);
      // can->Divide(1,2);
      // can->cd(1);				
      // grHalf[0]->GetXaxis()->SetLabelSize(0.06);
      // grHalf[0]->Draw("alp");
      // can->cd(2);
      // grHalf[1]->GetXaxis()->SetLabelSize(0.06);
      // grHalf[1]->Draw("alp");

      
      delete grHalf[0];
      delete grHalf[1];


    }
    delete gr;

  }
  std::cerr << "\n";
  lagTree->AutoSave();

  // Double_t deltaLag[2]={histLag[0]->GetMean(),histLag[1]->GetMean()};

  Double_t timeVals[2][SAMPLES_PER_BLOCK];
  Int_t indexVals[2][SAMPLES_PER_BLOCK];


  for(int capArray=0;capArray<2;capArray++) { 
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      if(samp%2==0)
	timeVals[capArray][samp]=newTimeVals[capArray][samp%2][samp/2];
      else 
	{
	  timeVals[capArray][samp]=newTimeVals[capArray][samp%2][samp/2]+histLag[capArray]->GetMean();
	  newTimeVals[capArray][samp%2][samp/2]+=histLag[capArray]->GetMean();
	}
    }

    //sorts timeVals[capArray] in ascending order, recording the indices in indexVals
    TMath::Sort(SAMPLES_PER_BLOCK,timeVals[capArray],indexVals[capArray],kFALSE);
  }

  cout << endl << "-------------Done Interleaving-------------" << endl  << "cap       samp Time even Time odd" << endl;

  //jpd print em all to screen
  for(int capArray=0;capArray<2;capArray++) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
      printf("%i\t%i\t%0.3f\t%0.3f\n", capArray, samp, newTimeVals[capArray][0][samp], newTimeVals[capArray][1][samp]);
    }
  }
  
  cout << endl << "estimated interleave capArray 0 " << histLag[0]->GetMean() << " \t capArray 1 " << histLag[1]->GetMean()<< endl;

  // cout << "--------------------------Time Ordered-----------------------" << endl;

  // for(int capArray=0;capArray<2;capArray++){
  //   for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++){
  //     printf("%i\t%i\t%i\t%0.3f\n", capArray, samp, indexVals[capArray][samp],timeVals[capArray][samp]);
  //   }
  // }

  
  
  cout << "Doing epsilon Calibration" << endl;
  
  //3. Do epsilon calibration

  Double_t epsilon[2] = {0};
  TH1F *histEpsilon[2];
  
  sprintf(histName,"histEpsilonCapArray%i",0);
  histEpsilon[0] = new TH1F(histName,histName, 1000, -5, 5);
  sprintf(histName,"histEpsilonCapArray%i",1);
  histEpsilon[1] = new TH1F(histName,histName, 1000, -5, 5);
  
  for(int i=0; i<numEntries;i++){
    //for(int i=100; i<101; i++){
  
    eventTree->GetEntry(i);
    if(evPtr->blockVec[0].getBlock()==0) continue;
    UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);
    
    //For now will assume all the ddas have the same  block
    Int_t capArray=evPtr->blockVec[0].getCapArray();

    TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
    Double_t *rawV=gr->GetY();
    Int_t numSamples=gr->GetN();
    Int_t numBlocks=numSamples/64;

    for(int pairBlocks=0; pairBlocks<2;pairBlocks++){
      if(pairBlocks!=0)
	capArray=1-capArray;
      //take into account that the capArray toggles between blocks
      Double_t mean[2][2]={{0}};
      for(block=0; block<2; block++){
	for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	  mean[block][samp%2]+=rawV[samp+SAMPLES_PER_BLOCK*(block+pairBlocks)];
	}
	mean[block][0]/=SAMPLES_PER_BLOCK/2;
	mean[block][1]/=SAMPLES_PER_BLOCK/2;
      }
      //now split into two graphs
    
      Double_t tValsHalf[2][64];
      Double_t vValsHalf[2][64];
      Double_t vValsHalfOrdered[2][64];

      //populate vValsHalf and tValsHalf with the zero-meaned waveforms for pair
      //of blocks
      
      for(int samp=0; samp<SAMPLES_PER_BLOCK; samp++){
	vValsHalf[0][samp]=rawV[samp+SAMPLES_PER_BLOCK*pairBlocks]-mean[0][samp%2];
	vValsHalf[1][samp]=rawV[samp+SAMPLES_PER_BLOCK*(1+pairBlocks)]-mean[1][samp%2];
	tValsHalf[0][samp]=timeVals[capArray][indexVals[capArray][samp]];
	tValsHalf[1][samp]=timeVals[1-capArray][indexVals[1-capArray][samp]]+timeVals[capArray][SAMPLES_PER_BLOCK-1];
      }

      //now have the time ordered tValsHalf but the non-time ordered vValsHalf

      for(int half=0;half<2;half++){
	for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++){
	  int thisCapArray = capArray;
	  if(half==1) thisCapArray=1-capArray;
	  vValsHalfOrdered[half][samp]=vValsHalf[half][indexVals[thisCapArray][samp]];

	}
      }

      //print to screen to double check
      // for(int half=0;half<2;half++){
      // 	for(int samp=0; samp<SAMPLES_PER_BLOCK;samp++){
      // 	  printf("%i\t%i\t%i\t%0.3f\t%0.3f\t%0.3f\n", half, capArray, samp, tValsHalf[half][samp], vValsHalfOrdered[half][samp], vValsHalf[half][samp]);
      // 	}
      // }
      
      TGraph *grHalf[2]={0};
      for(int half=0;half<2;half++) {
	grHalf[half] = new TGraph(SAMPLES_PER_BLOCK,tValsHalf[half],vValsHalfOrdered[half]);
      }
      //now estimate the epsilon value

       Double_t lastZC = estimateLastZC(grHalf[0], frequency);

       Double_t firstZC = estimateFirstZC(grHalf[1], frequency);

       if((vValsHalfOrdered[0][SAMPLES_PER_BLOCK-1]<0&&vValsHalfOrdered[1][0]>0)||(vValsHalfOrdered[0][SAMPLES_PER_BLOCK-1]==0&&vValsHalfOrdered[1][0]==0))
	 epsilon[capArray]=lastZC-firstZC+2./frequency;
       else
	 epsilon[capArray]=lastZC-firstZC+1./frequency;
       while(epsilon[capArray]<0)
	 epsilon[capArray]+=1./frequency;
       while(epsilon[capArray]>1./frequency)
	 epsilon[capArray]-=1./frequency;
       
       histEpsilon[capArray]->Fill(epsilon[capArray]);
        delete grHalf[0];
        delete grHalf[1];


       // Double_t vValsHalfnHalf[4][SAMPLES_PER_BLOCK/2];
       // Double_t tValsHalfnHalf[4][SAMPLES_PER_BLOCK/2];
       // for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++){
       // 	 vValsHalfnHalf[samp%2][samp/2]=vValsHalfOrdered[0][samp];
       // 	 vValsHalfnHalf[samp%2+2][samp/2]=vValsHalfOrdered[1][samp];
       // 	 tValsHalfnHalf[samp%2][samp/2]=tValsHalf[0][samp];
       // 	 tValsHalfnHalf[samp%2+2][samp/2]=tValsHalf[1][samp];

       // }
       
       // TGraph *grHalfnHalf[4]={0};
       // for(int half=0;half<4;half++){
       // 	 grHalfnHalf[half]=new TGraph(SAMPLES_PER_BLOCK/2, tValsHalfnHalf[half], vValsHalfnHalf[half]);
       // }


       // TCanvas *can = new TCanvas();//"can","can",600,600);
       // can->Divide(2,2);
       // can->cd(1);				
       // grHalf[0]->GetXaxis()->SetLabelSize(0.06);
       // grHalf[0]->SetTitle("Half 1 - odd and even samples interleaved");
       // grHalf[0]->Draw("alp");
       // can->cd(3);
       // grHalfnHalf[0]->SetLineColor(1);
       // grHalfnHalf[0]->SetMarkerColor(1);
       // grHalfnHalf[1]->SetLineColor(kGreen+2);
       // grHalfnHalf[1]->SetMarkerColor(kGreen+2);
       // grHalfnHalf[0]->GetXaxis()->SetLabelSize(0.06);
       // grHalfnHalf[1]->GetXaxis()->SetLabelSize(0.06);
       // grHalfnHalf[1]->SetTitle("Half 1 - odd and even split out");
       // grHalfnHalf[0]->Draw("alp");
       // grHalfnHalf[1]->Draw("lp");
       // can->cd(2);
       // grHalf[1]->SetLineColor(1);
       // grHalf[1]->SetMarkerColor(1);       
       // grHalf[1]->GetXaxis()->SetLabelSize(0.06);
       // grHalf[1]->SetTitle("Half 2 - odd and even samples interleaved");
       // grHalf[1]->Draw("alp");
       // can->cd(4);
       // grHalfnHalf[2]->SetLineColor(1);
       // grHalfnHalf[2]->SetMarkerColor(1);
       // grHalfnHalf[3]->SetLineColor(kGreen+2);
       // grHalfnHalf[3]->SetMarkerColor(kGreen+2);
       // grHalfnHalf[2]->GetXaxis()->SetLabelSize(0.06);
       // grHalfnHalf[3]->GetXaxis()->SetLabelSize(0.06);
       // grHalfnHalf[2]->SetTitle("Half 2 - odd and even split out");
       // grHalfnHalf[2]->Draw("alp");
       // grHalfnHalf[3]->Draw("lp");
       
       // cout << "FirstZC " << firstZC << "LastZC "<< lastZC << "Epsilon " << epsilon[capArray] << endl;
       
    }  
    delete gr;
  }

  //3.A printing epsilon values to file

  char outName[180];
  sprintf(outName,"sampleTiming_run%d_dda%d_chan%d.txt",run,dda,chan);

  std::ofstream OutFile(outName);
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

  sprintf(outName,"epsilon_run%d_dda%d_chan%d.txt",run,dda,chan);

  std::ofstream OutFile2(outName);
  for(int capArray=0; capArray<2; capArray++){
    OutFile2 << dda << "\t" << chan << "\t" << capArray << "\t";   
    OutFile2 << histEpsilon[capArray]->GetMean(1) << "\n";
  }

  OutFile2 << "\n";
    
  OutFile2.close();

  //3.B Filling the newEpsilon[capArray] array

  for(int capArray=0;capArray<2;capArray++){
    newEpsilon[capArray]=histEpsilon[capArray]->GetMean(1);
  }
  

  cout << endl << "--------------Done epsilon values-------------" << endl << "cap    samp Time even Time odd" << endl;

  for(int half=0;half<2;half++){
    for(int samp=0; samp<SAMPLES_PER_BLOCK/2;samp++){
      newTimeValsEpsilon[1][half][samp]+=newTimeVals[1][half][samp]+(newTimeVals[0][1][31]+histEpsilon[0]->GetMean(1));
      newTimeValsEpsilon[0][half][samp]=newTimeVals[0][half][samp];
    }
  }
  //jpd print em all to screen
  for(int capArray=0;capArray<2;capArray++) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK/2;samp++) {
      printf("%i\t%i\t%0.3f\t%0.3f\n", capArray, samp, newTimeValsEpsilon[capArray][0][samp], newTimeValsEpsilon[capArray][1][samp]);
    }
  }

  cout << "Estimate epsilon 0 to 1 is " << histEpsilon[0]->GetMean(1) << " 1 to 0 is " << histEpsilon[1]->GetMean(1) << endl;
  
  //jpd end of epsilon calculation



  histFile->Write();
  for(int capArray=0; capArray<2;capArray++){
    if(histLag[capArray])
      delete histLag[capArray];
    if(histEpsilon[capArray])
      delete histEpsilon[capArray];
    for(int half=0; half<2; half++){
      if(histZC[capArray][half])
	delete histZC[capArray][half];
      if(histBinWidth[capArray][half])
	delete histBinWidth[capArray][half];
    }
  }
  if(histMean)
    delete histMean;
  if(lagTree)
    delete lagTree;
  if(fp)
    delete fp;
  if(histFile)
    delete histFile;
      
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

Double_t estimateFirstZC(TGraph *grIn, Double_t freq)
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
  //  while(firstZC>period) firstZC-=period;
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

Double_t estimateLastZC(TGraph *grIn, Double_t freq)
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
  for(int i=numPoints-1;i>1;i--) {
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

  Double_t lastZC=zc[0];
  while(lastZC<(tVals[numPoints-1]-period)) lastZC+=period;
  Double_t meanZC=0;
  for(int i=0;i<countZC;i++) {
     while((lastZC-zc[i])>period) zc[i]+=period;
     // if(TMath::Abs((zc[i]-period)-firstZC)<TMath::Abs(zc[i]-firstZC))
     //   zc[i]-=period;]
     if((zc[i]+period-lastZC)<(lastZC-zc[i]))
       zc[i]+=period;
    
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


void plotEvent(Int_t event, Int_t block){
  Int_t chanIndex=lastChan+RFCHAN_PER_DDA*lastDda;
  lastEvent=event;
  lastBlock=block;

  char inName[180];
  sprintf(inName,"/Users/jdavies/ara/data/hawaii2011/root/run%d/event%d.root",lastRun,lastRun);

  TFile *fp = new TFile(inName);
  
  TTree *eventTree = (TTree*) fp->Get("eventTree");

  RawAraOneStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr); 

  eventTree->GetEntry(event);
  UsefulAraOneStationEvent realEvent(evPtr,AraCalType::kVoltageTime);
  
  //For now will assume all the ddas have the same block
  Int_t capArray=evPtr->blockVec[0].getCapArray();
  Int_t thisCapArray=0;  
  
  TGraph *gr = realEvent.getGraphFromElecChan(chanIndex);
  Double_t *rawT=gr->GetX();
  Double_t *rawV=gr->GetY();
  Double_t rawSubArrayT[128];
  Double_t rawSubArrayV[128];
  Double_t calibratedSubArrayT[128];
  Double_t calibratedSubArrayV[128];

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
  
  printf("Analysing run %i event %i dda %i channel %i\n", lastRun, event, lastDda, lastChan);
  printf("%i samples %i blocks\n", numSamples, numBlocks);
  
  TCanvas *can = new TCanvas();//"can","can",600,600);
  can->Divide(1,3);
  can->cd(1);

  //jpd cut out the relevant part of the graph
  
  for(int samp=0; samp<SAMPLES_PER_BLOCK*2;samp++){
    rawSubArrayT[samp]=rawT[samp+SAMPLES_PER_BLOCK*block];
    rawSubArrayV[samp]=rawV[samp+SAMPLES_PER_BLOCK*block];
  }

  //should probably zero mean it

  //now want to calibrate the graph properly
  //firstly populate timeVals with the newTimeVals
  //then re-order timeVals

  Int_t indexVals[2][SAMPLES_PER_BLOCK];
  Double_t timeVals[2][SAMPLES_PER_BLOCK];
  Int_t lastSampleFirstBlock;

  for(int cap=0; cap<2;cap++){
    for(int samp=0; samp<SAMPLES_PER_BLOCK;samp++){
      timeVals[cap][samp]=newTimeVals[cap][samp%2][samp/2];
    }
    TMath::Sort(SAMPLES_PER_BLOCK, timeVals[cap], indexVals[cap],kFALSE);    
  }

  //check that the sorting has worked properly
  //seems to be working fine!
  cout << "-------------sorted the time values--------------" <<endl;

  for(int cap=0;cap<2;cap++){
    for(int samp=0; samp<SAMPLES_PER_BLOCK;samp++){
      printf("%i\t%i\t%i\t%0.3f\n", cap, samp, indexVals[cap][samp], timeVals[cap][samp]);
    }
  }
  
  cout << "-----------------end of-------------------" << endl;



  //find last sample in the first block and the capArray of this block

  if(block%2) thisCapArray=1-capArray;
  lastSampleFirstBlock=indexVals[thisCapArray][SAMPLES_PER_BLOCK-1];

  cout << "first capArray is " << thisCapArray << " last sample is " << lastSampleFirstBlock << " " << timeVals[thisCapArray][lastSampleFirstBlock] << endl;

  //fill the calibratedSubArray and re-order the voltages
  for(int samp=0; samp<SAMPLES_PER_BLOCK;samp++){
    
    calibratedSubArrayT[samp]=timeVals[thisCapArray][samp];
    calibratedSubArrayV[samp]=rawSubArrayV[indexVals[thisCapArray][samp]];

    calibratedSubArrayT[samp+SAMPLES_PER_BLOCK]=timeVals[1-thisCapArray][samp]+timeVals[thisCapArray][lastSampleFirstBlock]+newEpsilon[thisCapArray];
    calibratedSubArrayV[samp+SAMPLES_PER_BLOCK]=rawSubArrayV[indexVals[1-thisCapArray][samp]];

  }


  //check that the sorting has worked properly
  //seems to be working fine!
  cout << "-------------sorted the time values--------------" <<endl;

  for(int cap=0;cap<2;cap++){
    for(int samp=0; samp<SAMPLES_PER_BLOCK;samp++){
      printf("%i\t%i\t%i\t%0.3f\n", cap, samp, indexVals[cap][samp], calibratedSubArrayT[samp+cap*SAMPLES_PER_BLOCK]);
    }
  }
  
  cout << "-----------------end of-------------------" << endl;




  //jpd and form grSubArray from the wanted part of the graph

  TGraph *grSubArray = new TGraph(SAMPLES_PER_BLOCK*2, rawSubArrayT, rawSubArrayV);
  TGraph *grCalSubArray = new TGraph(SAMPLES_PER_BLOCK*2, calibratedSubArrayT, calibratedSubArrayV);

  // grSubArray->GetXaxis()->SetLabelSize(0.06);
  // grSubArray->SetMarkerStyle(22);
  // grSubArray->Draw("alp");

  grCalSubArray->GetXaxis()->SetLabelSize(0.06);
  grCalSubArray->SetMarkerStyle(22);
  grCalSubArray->Draw("alp");

  //gr->Draw("alp");
  
  for(int thisBlock=block;thisBlock<block+2;thisBlock++) {
    mean[0]=0;
    mean[1]=0;
    thisCapArray=capArray;
    if(thisBlock%2) thisCapArray=1-capArray;
    else thisCapArray = capArray; // possible issue here with the wrong capArray!

    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tVals[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*thisBlock];
      tValsRaw[samp%2][samp/2]=rawT[samp+SAMPLES_PER_BLOCK*thisBlock];
      mean[samp%2]+=rawV[samp+SAMPLES_PER_BLOCK*thisBlock];
    }
    mean[0]/=SAMPLES_PER_BLOCK/2;
    mean[1]/=SAMPLES_PER_BLOCK/2;
    
    
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      tVals[samp%2][samp/2]=newTimeVals[thisCapArray][samp%2][samp/2];
      vVals[samp%2][samp/2]=rawV[samp+SAMPLES_PER_BLOCK*thisBlock];//-mean[samp%2];
    }
    
    TGraph *grHalf[2]={0};
    TGraph *grHalfRaw[2]={0};
    char graphName[180];
    for(int half=0;half<2;half++) {
      grHalf[half] = new TGraph(SAMPLES_PER_BLOCK/2,tVals[half],vVals[half]);
      grHalfRaw[half] = new TGraph(SAMPLES_PER_BLOCK/2,tValsRaw[half],vVals[half]);      
      
      sprintf(graphName, "block %i half %i", thisBlock+1, half+1);
      grHalf[half]->SetNameTitle(graphName, graphName);
      sprintf(graphName, "block %i half %i - Raw",thisBlock+1, half+1);
      grHalfRaw[half]->SetNameTitle(graphName, graphName);
      
    }
    
    can->cd(thisBlock-block+2);
    grHalfRaw[0]->SetLineColor(1);
    grHalfRaw[0]->SetMarkerColor(1);
    grHalfRaw[0]->GetXaxis()->SetLabelSize(0.06);
    grHalfRaw[0]->SetMarkerStyle(22);
    grHalfRaw[0]->Draw("alp");
    //can->cd(3);
    grHalfRaw[1]->SetLineColor(kGreen+2);
    grHalfRaw[1]->SetMarkerColor(kGreen+2);
    grHalfRaw[1]->GetXaxis()->SetLabelSize(0.06);
    grHalfRaw[1]->SetMarkerStyle(22);
    grHalfRaw[1]->Draw("lp");
        

  }

}


void nextEvent(){
  

  plotEvent(lastEvent+1, lastBlock);

}
void previousEvent(){

  plotEvent(lastEvent-1, lastBlock);

}
