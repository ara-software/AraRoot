//Written by Jonathan Davies - UCL
//doBinWidths.cxx - Calculate time between samples in the LABCHIPs used on ARA station 1

#include <iostream>
#include <fstream>
#include <cmath>
#include "zlib.h"


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

int gotPedFile=0;
char pedFile[FILENAME_MAX];
float pedestalData[LAB3_PER_ICRR][CHANNELS_PER_LAB3][MAX_NUMBER_SAMPLES_LAB3];

int doBinWidths(char[FILENAME_MAX], char[FILENAME_MAX], char[FILENAME_MAX], char[FILENAME_MAX], int, int, int);
void loadPedestals();  

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

  doBinWidths(runDir, baseDir, temp, pedFile, freq, minRun,maxRun);
  
  return 0;
}

int doBinWidths(char runName[FILENAME_MAX], char baseDir[FILENAME_MAX], char temp[FILENAME_MAX], char ped[FILENAME_MAX], int frequency, int minRun, int maxRun){

  TChain *eventTree = new TChain("eventTree");
  
  for(int run=minRun; run<=maxRun;run++){
    char name[200];
    sprintf(name, "%s/run%i/event%i.root", runName, run, run);
    eventTree->Add(name);
  }

  //  RawAraEvent *event=0;
  RawIcrrStationEvent *event=0;

  //  AraRawRFChannel chanOut; 
  AraRawIcrrRFChannel chanOut;

  Double_t zeroCrossing[LAB3_PER_ICRR][2][MAX_NUMBER_SAMPLES_LAB3] = {{{0}}};
  Double_t noSineWaves[LAB3_PER_ICRR][2][MAX_NUMBER_SAMPLES_LAB3]={{{0}}};
  Double_t dataOut[NUM_DIGITIZED_CHANNELS][MAX_NUMBER_SAMPLES_LAB3] = {{0}};
  Double_t BinWidths[LAB3_PER_ICRR][2][MAX_NUMBER_SAMPLES_LAB3]={{{0}}};
  Double_t BinSize=0, zc=0, sc=0, offset=0, error=0;
  Int_t firstHitBus=0, lastHitBus=0, wrapped=0, Chip=0, RCO = 0, RCO2=0, Sample = 0, Channel=0, earliestSample=0, offsetcount=0;
  Double_t period=1./(.001*frequency);
  eventTree->SetBranchAddress("event", &event);

  
  //strcpy(pedFile,"/Users/jdavies/ara/data/ara_station1_ICRR_calibration/data/peds/run_002263/peds_1324969832/peds_1324969832.905582.run002263.dat");
  strcpy(pedFile, ped);
  gotPedFile=1;
  loadPedestals();
  
  Int_t maxEntries = eventTree->GetEntries();
  Int_t starEvery = maxEntries/80;
  printf("No of entries in the run Tree is %i\n", maxEntries);

  cout << maxEntries << endl;
  
  for(int entry = 0; entry < maxEntries; entry ++){
    if(entry%starEvery==0) fprintf(stderr, "*");

    eventTree->GetEntry(entry);
    
    for(int i=0;i<NUM_DIGITIZED_CHANNELS;i++){
      Chip = i/9;
      Channel = i%9;
      if(Channel==8) continue; //skip the clock channel
      chanOut = event->chan[i];
      firstHitBus = event->getFirstHitBus(i);
      lastHitBus = event->getLastHitBus(i);
      wrapped = event->getWrappedHitBus(i);
      earliestSample = event->getEarliestSample(i);
      RCO = event->getRCO(i); 
      RCO2 = 1-RCO;
      
      //offset used to zero-mean sine-wave data
      
      offset=0;
      offsetcount=0;
      
      if(earliestSample<20){
	continue;
      }
      
      //zero-meaning the waveform
      
      if(wrapped==0){
	
	for(Sample=0; Sample<firstHitBus; Sample++){
	  offset += chanOut.data[Sample]-pedestalData[Chip][Channel][Sample];
	  offsetcount++;
	}
	
	for(Sample=lastHitBus+1; Sample <259; Sample++){
	  offset += chanOut.data[Sample]-pedestalData[Chip][Channel][Sample];
	  offsetcount++;	    
	}
      }
      
      if(wrapped==1){
	
	for(Sample=firstHitBus+1; Sample<lastHitBus; Sample++){
	  offset += chanOut.data[Sample]-pedestalData[Chip][Channel][Sample];
	  offsetcount++;
	}
      }
      offset=offset/offsetcount;
        
      if(wrapped==0){
	for(Sample =0; Sample<firstHitBus-1;Sample++){
	  dataOut[i][Sample]= chanOut.data[Sample]-pedestalData[Chip][Channel][Sample]-offset;
	  dataOut[i][Sample+1]= chanOut.data[Sample+1]-pedestalData[Chip][Channel][Sample+1]-offset;
	  noSineWaves[Chip][RCO][Sample]+=1;
	  if(dataOut[i][Sample]>0&&dataOut[i][Sample+1]<0){
	    zeroCrossing[Chip][RCO][Sample]+=1;
	  }
	  if(dataOut[i][Sample]<0&&dataOut[i][Sample+1]>0){
	    zeroCrossing[Chip][RCO][Sample]+=1;
	  }
	  if(dataOut[i][Sample]==0||dataOut[i][Sample+1]==0){
	    zeroCrossing[Chip][RCO][Sample]+=0.5;    
	  }
	}
	
	
	for(Sample=lastHitBus+1; Sample<259;Sample++){
	  dataOut[i][Sample]= chanOut.data[Sample]-pedestalData[Chip][Channel][Sample]-offset;
	  dataOut[i][Sample+1]= chanOut.data[Sample+1]-pedestalData[Chip][Channel][Sample+1]-offset;
	  noSineWaves[Chip][RCO2][Sample]+=1;
	  if(dataOut[i][Sample]>0&&dataOut[i][Sample+1]<0){
	    zeroCrossing[Chip][RCO2][Sample]+=1;
	  }
	  if(dataOut[i][Sample]<0&&dataOut[i][Sample+1]>0){
	    zeroCrossing[Chip][RCO2][Sample]+=1;
	  }
	  if(dataOut[i][Sample]==0||dataOut[i][Sample+1]==0){
	    zeroCrossing[Chip][RCO2][Sample]+=0.5;    
	  }
	}
      }//wrapped==0
      
      if(wrapped==1){
	for(Sample=firstHitBus+1; Sample<lastHitBus-1;Sample++){
	  dataOut[i][Sample]= chanOut.data[Sample]-pedestalData[Chip][Channel][Sample]-offset;
	  dataOut[i][Sample+1]= chanOut.data[Sample+1]-pedestalData[Chip][Channel][Sample+1]-offset;
	  noSineWaves[Chip][RCO][Sample]+=1;
	  if(dataOut[i][Sample]>0&&dataOut[i][Sample+1]<0){
	    zeroCrossing[Chip][RCO][Sample]+=1;
	  }
	  if(dataOut[i][Sample]<0&&dataOut[i][Sample+1]>0){
	    zeroCrossing[Chip][RCO][Sample]+=1;
	  }
	  if(dataOut[i][Sample]==0||dataOut[i][Sample+1]==0){
	    zeroCrossing[Chip][RCO][Sample]+=0.5;    
	  }
	}
      }//wrapped==1
    }//logical Channel
  }//entry   
  
  printf("\n");

  char rootName[FILENAME_MAX];
  sprintf(rootName, "%s/root/%s/BinWidths%iMHzRun%iTo%i.root",baseDir, temp,frequency, minRun,maxRun);
  
  TFile *outFile = new TFile(rootName, "RECREATE");
  TTree *outTree = new TTree("binWidths", "binWidths");
  
  outTree->Branch("Chip", &Chip, "Chip/I");
  outTree->Branch("Sample", &Sample, "Sample/I");
  outTree->Branch("BinSize", &BinSize, "BinSize/D");
  outTree->Branch("error", &error, "error/D");
  outTree->Branch("RCO", &RCO, "RCO/I");
  outTree->Branch("zc", &zc, "zc/D");
  outTree->Branch("sc", &sc, "sc/D");
  
  for(Chip=0; Chip<3;Chip++){
    for(RCO=0; RCO<2;RCO++){
      for(Sample=0; Sample<MAX_NUMBER_SAMPLES_LAB3; Sample++){	      
	zc=zeroCrossing[Chip][RCO][Sample];
	sc=noSineWaves[Chip][RCO][Sample];
	BinSize = (1.0/(frequency*0.002))*(zc/sc);  
	error = (1.0/(frequency*0.002))*(sqrt(zc)/sc);	    
	outTree->Fill();
	BinWidths[Chip][RCO][Sample]=BinSize;
      }
    }
  }


  TCanvas *canvas = new TCanvas("ChipRCO", "ChipRCO");
  char name[100];
  canvas->Divide(3,2);
  canvas->cd(1);
  outTree->Draw("BinSize","Chip==0&&RCO==0&&Sample<256");
  canvas->cd(2);
  outTree->Draw("BinSize","Chip==1&&RCO==0&&Sample<256");
  canvas->cd(3);
  outTree->Draw("BinSize","Chip==2&&RCO==0&&Sample<256");
  canvas->cd(4);
  outTree->Draw("BinSize","Chip==0&&RCO==1&&Sample<256");
  canvas->cd(5);
  outTree->Draw("BinSize","Chip==1&&RCO==1&&Sample<256");
  canvas->cd(6);
  outTree->Draw("BinSize","Chip==2&&RCO==1&&Sample<256");

  canvas->Write();
  canvas->Close();
  
  //Now to produce binWdiths.txt file for AraRoot

  ofstream textOut;
  char calibName[FILENAME_MAX];
  sprintf(calibName, "%s/calib/%s/binWidths.txt", baseDir,temp);
  textOut.open(calibName);

  for(Chip=0; Chip<3;Chip++){
    for(RCO=0; RCO<2;RCO++){
      textOut << Chip << "\t" <<  RCO << "\t" ;
      for(Sample=0; Sample<MAX_NUMBER_SAMPLES_LAB3-1; Sample++){	      
	textOut << BinWidths[Chip][RCO][Sample] << " ";
      }
      Sample=259;
      BinSize=0;
      textOut << BinSize << endl;
    }
  }
  
  textOut.close();
  
  outFile->Write();
  outFile->Close();
  
  
  return 0;
}


void loadPedestals()
{
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




