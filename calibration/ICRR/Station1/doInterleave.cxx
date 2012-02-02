//doInterleave by Jonathan Davies UCL

//Includes
#include <iostream>
#include <fstream>
#include <cmath>

//AraRoot Includes
//#include "UsefulAraEvent.h"
//#include "RawAraEvent.h"
//#include "AraEventCalibrator.h"

//AraRoot Includes -- using version 3.0
#include "RawIcrrStationEvent.h"
#include "UsefulIcrrStationEvent.h"
#include "AraEventCalibrator.h"

//ROOT Includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TChain.h"

#include <zlib.h>

int doInterleave(char[FILENAME_MAX], char[FILENAME_MAX], char[FILENAME_MAX], char[FILENAME_MAX], int, int, int);
double findFirstZC(double*, double*, int, int, int*);

using namespace std;

int main(int argc, char *argv[]){
  int freq, minRun, maxRun;
  char runDir[200], baseDir[200], temp[200], pedFile[200];
  sprintf(runDir, "/Users/jdavies/ara/data/ara_station1_ICRR_calibration/root/Station1Test");
  sprintf(baseDir, "/Users/jdavies/ara/calibration/ara_station1_ICRR/testing/testing");
  sprintf(pedFile, "/Users/jdavies/ara/data/ara_station1_ICRR_calibration/data/peds/run_003747/peds_1326108401/peds_1326108401.602169.run003747.dat");

  // if(argc<5){
  //   printf("Correct usage : ./doBinWidths <frequency> <minRun> <maxRun> <Minus40 / Minus20 / Minus5 / RoomTemp>\n");
  //   return -1;
  // }
  // freq=atoi(argv[1]);
  // minRun=atoi(argv[2]);
  // maxRun=atoi(argv[3]);
  // strcpy(temp, argv[4]);

  //jd to comment out
  freq=175;
  minRun=3763;
  maxRun=3769;
  sprintf(temp, "Minus20");



  printf("freq %i minRun %i maxRun %i temp %s\n", freq, minRun, maxRun, temp);

  doInterleave(runDir, baseDir, temp, pedFile, freq, minRun,maxRun);
  
  return 0;
}


int doInterleave(char runDir[FILENAME_MAX], char baseDir[FILENAME_MAX], char temp[FILENAME_MAX], char ped[FILENAME_MAX], int frequency, int minRun, int maxRun)
{

  //1. Open run files & create instance of event calibrator

  // printf("Processing run files\n");
  // printf("-----------------------------------------------\n");
  // printf("Frequency %i MHz run %i to run %i\n", frequency, minRun, maxRun);

  //  RawAraEvent *evPtr=0;
  RawIcrrStationEvent *evPtr=0;

  TChain *eventTree = new TChain("eventTree");
  char runName[FILENAME_MAX];
  for(int run=minRun; run<=maxRun;run++){
    sprintf(runName, "%s/run%i/event%i.root", runDir, run, run);
    eventTree->Add(runName);
  }

  eventTree->SetBranchAddress("event", &evPtr);

  AraEventCalibrator *calibEvent =  AraEventCalibrator::Instance();

  //  calibEvent->setPedFile("/Users/jdavies/ara/data/ara_station1_ICRR_calibration/data/peds/run_002263/peds_1324969832/peds_1324969832.905582.run002263.dat");
  calibEvent->setPedFile(ped, 1);

  //1.1 Create a tree of interleave times - deltaT

  TTree *interleaveTree = new TTree("interleaveTree", "interleaveTree");
  Int_t chip=0,pair=0, event=0, noZC1=0, noZC2=0, firstSample1=0, firstSample2=0;
  Double_t deltaT=0, rawDeltaT=0;
  interleaveTree->Branch("chip", &chip, "chip/I");
  interleaveTree->Branch("pair", &pair, "pair/I");
  interleaveTree->Branch("event", &event, "event/I");
  interleaveTree->Branch("noZC1", &noZC1, "noZC1/I");
  interleaveTree->Branch("noZC2", &noZC2, "noZC2/I");
  interleaveTree->Branch("firstSample1", &firstSample1, "firstSample1/I");
  interleaveTree->Branch("firstSample2", &firstSample2, "firstSample2/I");
  interleaveTree->Branch("deltaT", &deltaT, "deltaT/D");
  interleaveTree->Branch("rawDeltaT", &rawDeltaT, "rawDeltaT/D");

  //1.2 Create some histograms to populate with interleave times

  TH1D *histInterleave[2][4];
  char histName[FILENAME_MAX];
  for(chip=0;chip<2;chip++){
    for(pair=0;pair<4;pair++){
      sprintf(histName, "histInterleaveChip%iPair%i", chip, pair);
      histInterleave[chip][pair] = new TH1D(histName, histName, 1000, -1, 0);
    }
  }
 

  //2. Run through the event tree and get events

  Long64_t numEntries=eventTree->GetEntries();
  //  if(numEntries>45000) numEntries=45000;
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;

  printf("No of entries is %i\n", numEntries);
  
  for(event=0;event<numEntries;event++) {
    if(event%starEvery==0)  fprintf(stderr, "*");

 //printf("*%i", event);//std::cerr << "*";       
    eventTree->GetEntry(event);
    //2.1 skip any events where the rap around happens in the first / last 20 samples
    //2.2 Interleaved channels are consecutive channels on a chip = 4 pairs
    //    UsefulAraEvent *realEvent = new UsefulAraEvent(evPtr, AraCalType::kFirstCalib);
    UsefulIcrrStationEvent *realEvent = new UsefulIcrrStationEvent(evPtr, AraCalType::kFirstCalib);
    for(chip=0; chip<2; chip++){
       for(pair=0;pair<4; pair++){
	 firstSample1=evPtr->getEarliestSample(2*pair+chip*9);
	 firstSample2=evPtr->getEarliestSample(2*pair+chip*9+1);
	 if(firstSample1<20||firstSample2<20) continue;
	 TGraph *chan1 = realEvent->getGraphFromElecChan(2*pair+chip*9);
	 TGraph *chan2 = realEvent->getGraphFromElecChan(2*pair+chip*9+1);  

	 //zero-mean the waveform
	 double *tempx1=chan1->GetX();
	 double *tempy1=chan1->GetY();
	 double *tempx2=chan2->GetX();
	 double *tempy2=chan2->GetY();
	 
	 //2.3 Zero-mean the waveforms
	 int n1 =chan1->GetN();
	 int n2 =chan2->GetN();
	 double offset=0;
	 for(int i=0; i<n1; i++){
	   offset+=tempy1[i]/n1;
	 }
	 for(int i=0; i<n1; i++){
	   tempy1[i]=tempy1[i]-offset;
	 }
	 offset=0;
	 for(int i=0; i<n2; i++){
	   offset+=tempy2[i]/n2;
	 }
	 for(int i=0; i<n2; i++){
	   tempy2[i]=tempy2[i]-offset;
	 }
	 
	 //chan1Calib and chan2Calib are the calibrated waveforms
	 //2.4 find the phase of each

	 deltaT=findFirstZC(tempx1,tempy1, n1, frequency, &noZC1);
	 deltaT-=findFirstZC(tempx2,tempy2, n2, frequency, &noZC2);
	 rawDeltaT=deltaT;

	 if(noZC1-noZC2!=0) continue;
	 while(deltaT<-1./(frequency*0.001)) deltaT+=1./(frequency*0.001);
	 while(deltaT>1./(frequency*0.001))  deltaT-=1./(frequency*0.001);
	 while(deltaT>0) deltaT-=1./(frequency*0.001); 

	 interleaveTree->Fill();
	 histInterleave[chip][pair]->Fill(deltaT);

       }//pair
    }//chip
    //    if(realEvent) delete realEvent;
    delete realEvent;

  }//event

  //  delete calibEvent;

  //  printf("Completed the event loop/n");

  
  //3.1 Write the output to calib file

  ofstream textOut;
  char calibName[FILENAME_MAX];
  sprintf(calibName, "%s/calib/%s/interleaveFile.txt", baseDir, temp); 
  textOut.open(calibName);

  //  printf("Opened the calib file\n");
  
  for(chip=0;chip<2;chip++){
    for(pair=0;pair<4;pair++){
      textOut << chip << "\t" << pair << "\t" << histInterleave[chip][pair]->GetMean() << endl;      
    }
  }

  textOut.close();

  //3.2 Create a root file for the output

  char outName[FILENAME_MAX];
  sprintf(outName, "%s/root/%s/Interleave%iMHzTun%iTo%i.root", baseDir, temp, frequency, minRun, maxRun);
  TFile *outFile= new TFile(outName, "RECREATE");

  //  printf("Created the output root file\n");


  interleaveTree->Write();
  for(chip=0;chip<2;chip++){
    for(pair=0;pair<4;pair++){
      histInterleave[chip][pair]->Write();
      if(histInterleave[chip][pair]) delete histInterleave[chip][pair];
    }
  }

  outFile->Write();
  outFile->Close();

  return 0;
}

Double_t findFirstZC(double *fX, double *fY, int noSamples, int frequency, int *returnZC){
  Int_t noZC=0;
  Double_t firstZC=0, phase=0;

  for(int sample=0;sample<noSamples-1;sample++){
    if(fY[sample]<0&&fY[sample+1]>0){
      phase=(fY[sample])*(fX[sample]-fX[sample+1])/(fY[sample+1]-fY[sample])+fX[sample]-frequency*noZC;
      firstZC+=phase;
      noZC++;
    }
  }
  *returnZC=noZC;
  return firstZC/noZC;
}

