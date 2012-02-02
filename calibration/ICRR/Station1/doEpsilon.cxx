//Written by Jonathan Davies - UCL
//doEpsilon.cxx used to calculate the wrap around widths for ARA experiment

#include <iostream>
#include <fstream>
#include <cmath>

//General Includes
#include <zlib.h>

//ROOT Includes
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TChain.h"

//AraRoot Includes -- using version 3.0
//#include "RawAraEvent.h"
#include "RawIcrrStationEvent.h"
//#include "AraRawRFChannel.h"
#include "AraRawIcrrRFChannel.h"
//#include "araDefines.h"
#include "araIcrrDefines.h"


Int_t fDebug=0;
int gotPedFile=0;
char pedFile[FILENAME_MAX];
float pedestalData[LAB3_PER_ICRR][CHANNELS_PER_LAB3][MAX_NUMBER_SAMPLES_LAB3];

int doEpsilon(char[FILENAME_MAX], char[FILENAME_MAX], char[FILENAME_MAX], char[FILENAME_MAX], int, int, int, int flag=0);
Int_t findLastZC(TGraph*, Double_t, Double_t*);
Int_t findFirstZC(TGraph*, Double_t, Double_t*);
void loadPedestals();

using namespace std;

int main(int argc, char *argv[]){
  int  minRun, maxRun, freq;

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

  doEpsilon(runDir, baseDir, temp, pedFile, freq, minRun, maxRun);
  
  return 0;
}




int doEpsilon(char runDir[FILENAME_MAX], char baseDir[FILENAME_MAX], char temp[FILENAME_MAX], char ped[FILENAME_MAX], Int_t frequency, int minRun, int maxRun, int flag){

  if(flag){
    fDebug=flag;
    printf("In debuggin mode level %i\n", fDebug);
  }

  //  sprintf(pedFile, "/Users/jdavies/ara/data/ara_station1_ICRR_calibration/data/peds/run_002263/peds_1324969832/peds_1324969832.905582.run002263.dat");
  strcpy(pedFile, ped);
  gotPedFile=1;

  //1.0 Define variables needed

  Int_t Chip=0, RCO=0, Sample=0;
  Double_t BinSize=0;
  Double_t period=1./(0.001*frequency);

  //1.1 Open the bin widths file and populate a time array

  Double_t calibTime[LAB3_PER_ICRR][2][MAX_NUMBER_SAMPLES_LAB3]={{{0}}};
  char calibName[FILENAME_MAX];
  sprintf(calibName, "%s/root/%s/BinWidths%iMHzRun%iTo%i.root", baseDir, temp, frequency, minRun, maxRun);

  if(fDebug)  printf("Opening Bin widths file\n%s\n", calibName);

  TFile *calibFile = TFile::Open(calibName);
  TTree *calibTree = (TTree*)calibFile->Get("binWidths");
  calibTree->SetBranchAddress("Chip", &Chip);
  calibTree->SetBranchAddress("Sample", &Sample);
  calibTree->SetBranchAddress("BinSize", &BinSize);
  calibTree->SetBranchAddress("RCO", &RCO); 

  for(int entry=0;entry<calibTree->GetEntries();++entry){
    calibTree->GetEntry(entry);
    calibTime[Chip][RCO][Sample]=BinSize;
  }
    
  calibFile->Close();

  if(fDebug){
    for(Chip=0;Chip<LAB3_PER_ICRR;++Chip){
      for(RCO=0;RCO<2;++RCO){
	for(Sample=0;Sample<MAX_NUMBER_SAMPLES_LAB3;++Sample){
	  printf("%i\t%i\t%i\t%f\n", Chip, RCO, Sample, calibTime[Chip][RCO][Sample]);
	}
      }
    }
  }
  
  //2. Open run files into a TChain
  
  TChain *eventTree = new TChain("eventTree");
  char runName[FILENAME_MAX];
  for(int run=minRun; run<=maxRun;run++){
    sprintf(runName, "%s/run%i/event%i.root", runDir, run, run);
    if(fDebug)  printf("Adding run file %s\n", runName);
    eventTree->Add(runName);
  }
  

  //  RawAraEvent *evPtr=0;
  // AraRawRFChannel chanOut;
  RawIcrrStationEvent *evPtr=0;
  AraRawIcrrRFChannel chanOut;

  eventTree->SetBranchAddress("event", &evPtr);

  //3. Open Pedestals
  loadPedestals();

  //4. Create an output Tree format and interesting variables
  TGraph *preWrapGraph[NUM_DIGITIZED_ICRR_CHANNELS];
  TGraph *postWrapGraph[NUM_DIGITIZED_ICRR_CHANNELS];
  Double_t preWrap[2][MAX_NUMBER_SAMPLES_LAB3];
  Double_t postWrap[2][MAX_NUMBER_SAMPLES_LAB3];
  Int_t Channel=0, firstHitBus=0, lastHitBus=0, wrapped=0, RCO1=0,RCO2=0, hitCut=0, noSamples=0, offsetCount=0, eventNo=0, entry=0, skipCount=0;
  Double_t deltaT=0, rawDeltaT=0, lastZC=0, firstZC=0, offset=0, cumalativeTime=0;

  char outName[FILENAME_MAX];
  if(fDebug)   sprintf(outName, "%s/root/%s/WrapAroundDebugRun%iTo%i.root", baseDir, temp, minRun, maxRun);

  else  sprintf(outName, "%s/root/%s/WrapAroundRun%iTo%i.root", baseDir, temp, minRun, maxRun);
  if(fDebug) printf("Creating outPut File and Tree %s\n", outName);
  
  
  TTree *wrapTree = new TTree("wrapTree", "wrapTree");

  wrapTree->Branch("Chip", &Chip, "Chip/I");
  wrapTree->Branch("RCO", &RCO, "RCO/I");
  wrapTree->Branch("Channel", &Channel, "Channel/I");
  wrapTree->Branch("firstHitBus", &firstHitBus, "firstHitBus/I");
  wrapTree->Branch("lastHitBus", &lastHitBus, "lastHitBus/I");
  wrapTree->Branch("wrapped", &wrapped, "wrapped/I");
  wrapTree->Branch("RCO1", &RCO1, "RCO1/I");
  wrapTree->Branch("entry", &entry, "entry/I");
  wrapTree->Branch("hitCut", &hitCut, "hitCut/I");
  wrapTree->Branch("noSamples", &noSamples, "noSamples/I");
  wrapTree->Branch("eventNo", &eventNo, "eventNo/I");
  wrapTree->Branch("skipCount", &skipCount, "skipCount/I");

  //  wrapTree->Branch("A", &A, "A/I");
  wrapTree->Branch("deltaT", &deltaT, "deltaT/D");
  wrapTree->Branch("rawDeltaT", &rawDeltaT, "rawDeltaT/D");

  wrapTree->Branch("firstZC", &firstZC, "firstZC/D");
  wrapTree->Branch("lastZC", &lastZC, "lastZC/D");
  //  wrapTree->Branch("A", &A, "A/D");

  //4.1 Create a bunch of histograms in which to store deltaT's
  TH1D *histDeltaT[LAB3_PER_ICRR][2];
  char histName[FILENAME_MAX];
  for(Chip=0;Chip<LAB3_PER_ICRR;Chip++){
    for(RCO=0;RCO<2;RCO++){
      sprintf(histName, "histDeltaTChip%iRCO%i", Chip, RCO);
      histDeltaT[Chip][RCO] = new TH1D(histName,histName, 1000, 2, 6);
    }
  }
  



  //5. Event loop
  int minEntry=0, maxEntry=eventTree->GetEntries(), minChan=0, maxChan=CHANNELS_PER_LAB3-1, starEvery=maxEntry/80;
  printf("No of entries in the run Tree is %i\n", maxEntry);
  if(fDebug){
    minEntry=46186;
    maxEntry=46187;
    minChan=0;
    //    maxChan=3;
  }
  for(entry=minEntry;entry<maxEntry;++entry){
    if(entry%starEvery==0) fprintf(stderr, "*");

    eventTree->GetEntry(entry);
    eventNo=evPtr->head.eventNumber;
    
    for(int logChan=minChan;logChan<NUM_DIGITIZED_ICRR_CHANNELS;++logChan){

      //5.1 Get all the necessary stuff from the event for that channel
      chanOut=evPtr->chan[logChan];
      //      Chip=evPtr->getLabChip(logChan);
      //      Chip=chanOut.getLabChip();
      Chip=logChan/9;
      Channel=logChan%9;
      if(Channel>maxChan) continue;
      firstHitBus=evPtr->getFirstHitBus(logChan);
      lastHitBus=evPtr->getLastHitBus(logChan);
      wrapped=evPtr->getWrappedHitBus(logChan);
      RCO2=evPtr->getRCO(logChan);
      RCO1=1-RCO2;


      if(fDebug){
	for(int i=0;i<80;i++) printf("-");
	printf("\nEvent %i Chip %i Channel %i RCO2%i\n", entry, Chip, Channel, RCO2);
      }


      //5.2 This is a cut made in first version of wrapAround.cxx
      if(firstHitBus<20||firstHitBus>236||lastHitBus<20||lastHitBus>236){
	//      	continue;
	hitCut=1;
      }
      else hitCut=0;

      //5.3 Calculate the offset
      offset=0;
      offsetCount=0;
      for(Sample=0; Sample<firstHitBus; Sample++){
      	offset += chanOut.data[Sample]-pedestalData[Chip][Channel][Sample];
      	offsetCount++;
      }
      for(Sample=lastHitBus+1; Sample <259; Sample++){
      	offset += chanOut.data[Sample]-pedestalData[Chip][Channel][Sample];
      	offsetCount++;	    
      }
      offset=offset/offsetCount;
 
      //5.4 Fill pre and post wrap arrays
      cumalativeTime=0;
      noSamples=0;
      for(Sample=lastHitBus+1;Sample<256;++Sample){
	preWrap[0][Sample-lastHitBus-1]=cumalativeTime;
	preWrap[1][Sample-lastHitBus-1]=(chanOut.data[Sample]-pedestalData[Chip][Channel][Sample])-offset;
	cumalativeTime+=calibTime[Chip][RCO1][Sample];
	++noSamples;
      }
      
      cumalativeTime-=calibTime[Chip][RCO1][255];

      preWrapGraph[logChan] = new TGraph(noSamples, preWrap[0], preWrap[1]);
      noSamples=0;
      
      for(Sample=0;Sample<firstHitBus;Sample++){
	postWrap[0][Sample]=cumalativeTime;
	postWrap[1][Sample]=(chanOut.data[Sample]-pedestalData[Chip][Channel][Sample])-offset;
	cumalativeTime+=calibTime[Chip][RCO2][Sample];	
	++noSamples;
      }

      postWrapGraph[logChan] = new TGraph(noSamples, postWrap[0], postWrap[1]);

      //5.5 Calculate wrapAround / deltaT

      firstZC=0;
      lastZC=0;

      if(findFirstZC(postWrapGraph[logChan], period, &firstZC)==-1)
	continue;
      if(findLastZC(preWrapGraph[logChan], period, &lastZC)==-1)
	continue;
      deltaT=(lastZC-firstZC)+period;
      rawDeltaT=deltaT;
      //      need to insert some more stringent requirement here
	
      if(deltaT<0) deltaT+=period;
      
      // deltaT=lastZC-firstZC;
      // rawDeltaT=deltaT+period;
      // while(TMath::Abs(deltaT+period)<TMath::Abs(deltaT))
      // 	deltaT+=period;
      // while(TMath::Abs(deltaT-period)<TMath::Abs(deltaT))
      // 	deltaT-=period;


      if(fDebug) printf("5.5 deltaT is %f rawDeltaT is %f\tfirstZC %f\tlastZC %f\n",deltaT,rawDeltaT, firstZC, lastZC);

      //5.6 Fill the Tree and or histograms

      skipCount=0;
      if(entry<10)
	histDeltaT[Chip][RCO1]->Fill(deltaT);
      else if(TMath::Abs(histDeltaT[Chip][RCO1]->GetMean()-deltaT)<.5)
	histDeltaT[Chip][RCO1]->Fill(deltaT);
      else skipCount=1;
      RCO=RCO1;
      wrapTree->Fill();  


      if(!fDebug){
	delete preWrapGraph[logChan];
	delete postWrapGraph[logChan];
      }
    }//logChan
  }//entry

  printf("\n");
  
  //6. Create a Canvas and other useful things before saving and closing the files
     if(fDebug) printf("6. Create a Canvas and other useful things before saving and closing the files\n");
  TFile *outFile = new TFile(outName, "RECREATE");  
  TCanvas *canvas[NUM_DIGITIZED_ICRR_CHANNELS];


  wrapTree->Write();
 
  if(fDebug){
    char canvasName[FILENAME_MAX];
    for(Int_t logChan=0;logChan<NUM_DIGITIZED_ICRR_CHANNELS;logChan++){
      Chip=logChan/9;
      Channel=logChan%9;
      sprintf(canvasName, "Event%iChip%iChannel%i", minEntry, Chip, Channel);
      canvas[logChan] = new TCanvas(canvasName,canvasName);
      canvas[logChan]->Divide(1,2);
      canvas[logChan]->cd(1);
      preWrapGraph[logChan]->SetTitle("preWrap");
      preWrapGraph[logChan]->GetXaxis()->SetTitle("Time (ns)");
      preWrapGraph[logChan]->GetXaxis()->SetLabelSize(0.04);
      preWrapGraph[logChan]->GetYaxis()->SetTitle("mV (ish)");
      preWrapGraph[logChan]->Draw("AL*");
      canvas[logChan]->cd(2);
      postWrapGraph[logChan]->SetTitle("postWrap");
      postWrapGraph[logChan]->GetXaxis()->SetTitle("Time (ns)");
      postWrapGraph[logChan]->GetXaxis()->SetLabelSize(0.04);
      postWrapGraph[logChan]->GetYaxis()->SetTitle("mV (ish)");
      postWrapGraph[logChan]->Draw("AL*");
    }
  }
  else{
    char canvasName[FILENAME_MAX];
    sprintf(canvasName, "histDeltaT");
    canvas[0] = new TCanvas(canvasName,canvasName);
    canvas[0]->Divide(3,2); 
    canvas[0]->cd(1);
    //    wrapTree->Draw("deltaT", "wrapped==0&&hitCut==0&&Channel<8&&Chip==0&&RCO==0");
    histDeltaT[0][0]->Draw();
    canvas[0]->cd(2);
    //    wrapTree->Draw("deltaT", "wrapped==0&&hitCut==0&&Channel<8&&Chip==1&&RCO==0");
    histDeltaT[1][0]->Draw();
    canvas[0]->cd(3);
    //    wrapTree->Draw("deltaT", "wrapped==0&&hitCut==0&&Channel<8&&Chip==2&&RCO==0");
    histDeltaT[2][0]->Draw();
    canvas[0]->cd(4);
    //    wrapTree->Draw("deltaT", "wrapped==0&&hitCut==0&&Channel<8&&Chip==0&&RCO==1");
    histDeltaT[0][1]->Draw();
    canvas[0]->cd(5);
    //    wrapTree->Draw("deltaT", "wrapped==0&&hitCut==0&&Channel<8&&Chip==1&&RCO==1");
    histDeltaT[1][1]->Draw();
    canvas[0]->cd(6);
    //    wrapTree->Draw("deltaT", "wrapped==0&&hitCut==0&&Channel<8&&Chip==2&&RCO==1");
    histDeltaT[2][1]->Draw();
    canvas[0]->Write();
    canvas[0]->Close();
    sprintf(canvasName, "rawDeltaT");
    canvas[1] = new TCanvas(canvasName,canvasName);
    canvas[1]->Divide(3,2); 
    canvas[1]->cd(1);
    wrapTree->Draw("rawDeltaT", "wrapped==0&&firstHitBus>20&&Channel<8&&Chip==0&&RCO==0");
    canvas[1]->cd(2);
    wrapTree->Draw("rawDeltaT", "wrapped==0&&firstHitBus>20&&Channel<8&&Chip==1&&RCO==0");
    canvas[1]->cd(3);
    wrapTree->Draw("rawDeltaT", "wrapped==0&&firstHitBus>20&&Channel<8&&Chip==2&&RCO==0");
    canvas[1]->cd(4);
    wrapTree->Draw("rawDeltaT", "wrapped==0&&firstHitBus>20&&Channel<8&&Chip==0&&RCO==1");
    canvas[1]->cd(5);
    wrapTree->Draw("rawDeltaT", "wrapped==0&&firstHitBus>20&&Channel<8&&Chip==1&&RCO==1");
    canvas[1]->cd(6);
    wrapTree->Draw("rawDeltaT", "wrapped==0&&firstHitBus>20&&Channel<8&&Chip==2&&RCO==1");
    canvas[1]->Write();
    canvas[1]->Close();
  }



  //7. Save everything then print the epsilon values

  for(Chip=0;Chip<LAB3_PER_ICRR;Chip++){
    for(RCO=0;RCO<2;RCO++){
      histDeltaT[Chip][RCO]->Write();
    }
  }
  Double_t epsilon=0;
  
  ofstream epsilonFile;
  
  if(fDebug)  cout << endl << "Epsilon values\n";

  ofstream textOut;
  char textOutName[FILENAME_MAX];
  sprintf(textOutName, "%s/calib/%s/epsilonFile.txt", baseDir, temp); 
  textOut.open(textOutName);
  for(Chip=0;Chip<LAB3_PER_ICRR;Chip++){
    for(RCO=0;RCO<2;RCO++){
      epsilon=-(histDeltaT[Chip][RCO]->GetMean())+calibTime[Chip][RCO][255]+calibTime[Chip][RCO][256]+calibTime[Chip][RCO][257]+calibTime[Chip][RCO][258];
      textOut << Chip << "\t" << RCO << "\t" << epsilon << endl;
      if(fDebug)      cout << Chip << "\t" << RCO << "\t" << histDeltaT[Chip][RCO]->GetMean() << "\t" << epsilon << endl;
    }
  }
  textOut.close();
  outFile->Write();
  //  outFile->Close();



  return 0;
}

// Int_t findLastZC(TGraph *graph, Double_t period, Double_t *lastZC){
//   Double_t lastZCEst=0, phase=0, phase0=0, *preWrap[2];
//   Int_t noZCs=0;

//   Int_t noSamples=graph->GetN();
//   preWrap[0]=graph->GetX();
//   preWrap[1]=graph->GetY();

//   if(fDebug>1)  printf("Finding first ZC\n");

//   for(Int_t Sample=noSamples-1; Sample>0; --Sample){
//     if(preWrap[1][Sample-1]<0&&preWrap[1][Sample]>0){
//       Double_t x1=preWrap[0][Sample-1];
//       Double_t x2=preWrap[0][Sample];
//       Double_t y1=preWrap[1][Sample-1];
//       Double_t y2=preWrap[1][Sample];
     
//       // time=(preWrap[1][Sample-1])*(preWrap[0][Sample-1]-preWrap[0][Sample])/(preWrap[1][Sample]-preWrap[1][Sample-1])+preWrap[0][Sample-1];

//       phase=y1*(x1-x2)/(y2-y1)+(x1);  

//       if(fDebug>1)      printf("zc no %i found at %f, relocated to %f\n", noZCs+1, phase, phase+noZCs*period);
//       lastZCEst+=phase+noZCs*period;
//       noZCs++;

//     }
//   }
  
//   if(fDebug>1)  printf("Average value is %f\n", lastZCEst/noZCs);
  
//   if(noZCs){
//     *lastZC=lastZCEst/noZCs;
//     return 0;
//   }
//   return -1;
// }

// Int_t findFirstZC(TGraph *graph, Double_t period, Double_t *firstZC){
//   Double_t firstZCEst=0, phase=0, lastPhase=0, *postWrap[2];
//   Int_t noZCs=0, noSamples=0;

//   noSamples=graph->GetN();
//   postWrap[0]=graph->GetX();
//   postWrap[1]=graph->GetY();

//   if(fDebug>1)  printf("Finding first ZC\n");

//   for(Int_t Sample=0; Sample<noSamples-1; ++Sample){
//     if(postWrap[1][Sample]<0&&postWrap[1][Sample+1]>0){
//       Double_t x1=postWrap[0][Sample];
//       Double_t x2=postWrap[0][Sample+1];
//       Double_t y1=postWrap[1][Sample];
//       Double_t y2=postWrap[1][Sample+1];

//       //    time=(postWrap[1][Sample])*(postWrap[0][Sample]-postWrap[0][Sample+1])/(postWrap[1][Sample+1]-postWrap[1][Sample])+postWrap[0][Sample];
     
//       phase=y1*(x1-x2)/(y2-y1)+(x1);
      
//       if(fDebug>1)      printf("zc no %i found at %f, relocated to %f phase-lastPhase %f\n", noZCs+1, phase, phase-noZCs*period, phase-lastPhase);

//       firstZCEst+=phase-noZCs*period;
//       noZCs++;
//       lastPhase=phase;
//     }
//   }
  
//   if(fDebug>1)  printf("Average value is %f\n", firstZCEst/noZCs);
  
//   if(noZCs){
//     *firstZC=firstZCEst/noZCs;
//     return 0;
//   }
//   return -1;
// }

void loadPedestals(){
  if(!gotPedFile) {
    char calibDir[FILENAME_MAX];
    char *calibEnv=getenv("ARA_CALIB_DIR");
    if(!calibEnv) {
      char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
      if(!utilEnv)
        sprintf(calibDir,"calib");
      else
        sprintf(calibDir,"%s/share/araCalib",utilEnv);
    }
    else {
      strncpy(calibDir,calibEnv,FILENAME_MAX);
    }  
    sprintf(pedFile,"%s/peds_1286989711.394723.dat",calibDir);
  }
  FullLabChipPedStruct_t peds;
  gzFile inPed = gzopen(pedFile,"r");
  if( !inPed ){
    fprintf(stderr,"Failed to open pedestal file %s.\n",pedFile);
    return;
  }

  int nRead = gzread(inPed,&peds,sizeof(FullLabChipPedStruct_t));
  if( nRead != sizeof(FullLabChipPedStruct_t)){
    int numErr;
    fprintf(stderr,"Error reading pedestal file %s; %s\n",pedFile,gzerror(inPed,&numErr));
    gzclose(inPed);
    return;
  }

  int chip,chan,samp;
  for(chip=0;chip<LAB3_PER_ICRR;++chip) {
    for(chan=0;chan<CHANNELS_PER_LAB3;++chan) {
      int chanIndex = chip*CHANNELS_PER_LAB3+chan;
      for(samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
	pedestalData[chip][chan][samp]=peds.chan[chanIndex].pedMean[samp];
      }
    }
  }
  gzclose(inPed);
}


//Modified versions of the findLast and findFirst ZC functions

Int_t findLastZC(TGraph *graph, Double_t period, Double_t *lastAvZC){
  Double_t *preWrap[2], thisZC=0, lastZC=0, meanZC=0;
  Int_t noZCs=0;
  Int_t noSamples=graph->GetN();
  preWrap[0]=graph->GetX();
  preWrap[1]=graph->GetY();

  if(fDebug>1)  printf("Finding last ZC\n");

  for(Int_t Sample=noSamples-1; Sample>0; --Sample){
    if(preWrap[1][Sample-1]<0&&preWrap[1][Sample]>0){
      Double_t x1=preWrap[0][Sample-1];
      Double_t x2=preWrap[0][Sample];
      Double_t y1=preWrap[1][Sample-1];
      Double_t y2=preWrap[1][Sample];
     
      thisZC=y1*(x1-x2)/(y2-y1)+(x1);  
      if(!noZCs) lastZC=thisZC;
      while(TMath::Abs((lastZC-thisZC)-period)<TMath::Abs(lastZC-thisZC)) thisZC+=period;
      meanZC+=thisZC;
      noZCs++;
    }
  }
  
  if(fDebug>1)  printf("Average value is %f\n", meanZC/noZCs);
  
  if(noZCs){
    *lastAvZC=meanZC/noZCs;
    return 0;
  }
  return -1;
}

Int_t findFirstZC(TGraph *graph, Double_t period, Double_t *firstAvZC){
  Double_t *postWrap[2], thisZC=0, meanZC=0, firstZC=0;
  Int_t noZCs=0;

  Int_t noSamples=graph->GetN();
  postWrap[0]=graph->GetX();
  postWrap[1]=graph->GetY();

  if(fDebug>1)  printf("Finding first ZC\n");

  for(Int_t Sample=0; Sample<noSamples-1; ++Sample){
    if(postWrap[1][Sample]<0&&postWrap[1][Sample+1]>0){
      Double_t x1=postWrap[0][Sample];
      Double_t x2=postWrap[0][Sample+1];
      Double_t y1=postWrap[1][Sample];
      Double_t y2=postWrap[1][Sample+1];

      thisZC=y1*(x1-x2)/(y2-y1)+(x1);

      if(!noZCs) firstZC=thisZC;

      while(TMath::Abs((firstZC-thisZC)+period)<TMath::Abs(firstZC-thisZC)) thisZC-=period;
      meanZC+=thisZC;
      noZCs++;

    }
  }
  
  if(fDebug>1)  printf("Average value is %f\n", meanZC/noZCs);
  
  if(noZCs){
    *firstAvZC=meanZC/noZCs;
    return 0;
  }
  return -1;
}
