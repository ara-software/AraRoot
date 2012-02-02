#include <iostream>
#include <fstream>

//Event Reader Includes
#include "UsefulAraTestBedStationEvent.h"
#include "RawAraTestBedStationEvent.h"
#include "araTestbedDefines.h"

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
#include <TGClient.h>

#include <zlib.h>

void loadPedestals();   
int gotPedFile=0;
char pedFile[FILENAME_MAX];
float pedestalData[LAB3_PER_TESTBED][CHANNELS_PER_LAB3][MAX_NUMBER_SAMPLES_LAB3];


int main(int argc, char *argv)
{
  //  TFile *fp = new TFile("/Users/rjn/ara/data/root/event_frozen_200MHz.root");
  TFile *fp = new TFile("/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/sine_wave_data/root/event200MHz_317mV.root");
  if(!fp) {
    std::cerr << "Can't open file\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fp->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't find eventTree\n";
    return -1;
  }
  RawAraTestBedStationEvent *evPtr=0;
  eventTree->SetBranchAddress("event",&evPtr);
  //  strcpy(pedFile,"/Users/rjn/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
  strcpy(pedFile,"/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291303459/peds_1291303459.323022.dat");
  gotPedFile=1;
  loadPedestals();

  Double_t frequency=0.2; //Ghz
  Double_t ampmv=317;

  char histName[180];
  sprintf(histName,"histFile_%3.0fMHz_%3.0fmV.root",1000*frequency,ampmv);
  TFile *histFile = new TFile(histName,"RECREATE");
  TH1F *histFirstSamp[NUM_DIGITIZED_TESTBED_CHANNELS][2];
  TH1F *histLastSamp[NUM_DIGITIZED_TESTBED_CHANNELS][2];
  TH1F *histZC[NUM_DIGITIZED_TESTBED_CHANNELS][2];
  TH1F *histNorm[NUM_DIGITIZED_TESTBED_CHANNELS][2];
  TH1F *histBinWidth[NUM_DIGITIZED_TESTBED_CHANNELS][2];
  TH1F *histBinWidthErr[NUM_DIGITIZED_TESTBED_CHANNELS][2];
  for(int chan=0;chan<NUM_DIGITIZED_TESTBED_CHANNELS;chan++) {
    for(int rco=0;rco<2;rco++) {
      sprintf(histName,"histFirstSamp_%d_%d",chan,rco);
      histFirstSamp[chan][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histLastSamp_%d_%d",chan,rco);
      histLastSamp[chan][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histZC_%d_%d",chan,rco);
      histZC[chan][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histNorm_%d_%d",chan,rco);
      histNorm[chan][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histBinWidth_%d_%d",chan,rco);
      histBinWidth[chan][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histBinWidthErr_%d_%d",chan,rco);
      histBinWidthErr[chan][rco] = new TH1F(histName,histName,260,-0.5,259.5);
    }
  }

  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;
  for(Long64_t i=0;i<numEntries;i++) {
    if(i%starEvery==0) std::cerr << "*";
    eventTree->GetEntry(i);
    
    for(int chan=0;chan<NUM_DIGITIZED_TESTBED_CHANNELS;chan++) {
      int chip=chan/9;
      int realChan=chan%9;
      if(realChan==8) continue;
      int rco=evPtr->getRCO(chan);
      int firstSamp=evPtr->getEarliestSample(chan);
      int lastSamp=evPtr->getLatestSample(chan);
      //This cut is to exclude those events with misdetermined RCO phases
      if(firstSamp<20 || firstSamp>250) continue;
      histFirstSamp[chan][rco]->Fill(firstSamp);
      histLastSamp[chan][rco]->Fill(lastSamp);
      double data[MAX_NUMBER_SAMPLES_LAB3];

      for(int samp=0;samp<260;samp++) {
	data[samp]=evPtr->chan[chan].data[samp]-pedestalData[chip][realChan][samp];
      }
      //Zero mean the waveform
      double mean=TMath::Mean(260,data);
      //      std::cout << mean << "\t" << data[0] << "\n";
      for(int samp=0;samp<260;samp++)
	data[samp]-=mean;

      if(firstSamp<lastSamp) {
        //continue;	
	//One RCO phase
	for(int i=firstSamp;i<lastSamp-1;i++) {
	  double val1=data[i];
	  double val2=data[i+1];
	  histNorm[chan][rco]->Fill(i);
	  if(val1<0 && val2>0) {
	    histZC[chan][rco]->Fill(i);
	    histBinWidth[chan][rco]->Fill(i);
	  }
	  else if(val1>0 && val2<0) {
	    histZC[chan][rco]->Fill(i);
	    histBinWidth[chan][rco]->Fill(i);
	  }
	  else if(val1==0 || val2==0) {
	    histZC[chan][rco]->Fill(i,0.5);
	    histBinWidth[chan][rco]->Fill(i,0.5);
	  }
	  
	}
      }
      else {
	//continue;
	//Two RCO phases
	if(firstSamp<258) {
	  for(int i=firstSamp;i<259;i++) {
	    double val1=data[i];
	    double val2=data[i+1];
	    //	    if(i==258)
	    //	      std::cout << val1 << "\t" << val2 << "\n";
	    histNorm[chan][1-rco]->Fill(i);
	    if(val1<0 && val2>0) {
	      histZC[chan][1-rco]->Fill(i);
	      histBinWidth[chan][1-rco]->Fill(i);
	    }
	    else if(val1>0 && val2<0) {
	      histZC[chan][1-rco]->Fill(i);
	      histBinWidth[chan][1-rco]->Fill(i);
	    }
	    else if(val1==0 || val2==0) {
	      histZC[chan][1-rco]->Fill(i,0.5);
	      histBinWidth[chan][1-rco]->Fill(i,0.5);
	    }	
	    
	  }
	}
	if(lastSamp>2) {
	  for(int i=0;i<lastSamp-1;i++) {
	    double val1=data[i];
	    double val2=data[i+1];
	    histNorm[chan][rco]->Fill(i);
	    if(val1<0 && val2>0) {
	      histZC[chan][rco]->Fill(i);
	      histBinWidth[chan][rco]->Fill(i);
	    }
	    else if(val1>0 && val2<0) {
	      histZC[chan][rco]->Fill(i);
	      histBinWidth[chan][rco]->Fill(i);
	    }
	    else if(val1==0 || val2==0) {
	      histZC[chan][rco]->Fill(i,0.5);
	      histBinWidth[chan][rco]->Fill(i,0.5);
	    }
	    
	  }
	}	    
      }
    }	
  }
  
  Double_t avgNorm=0;
  Int_t numNorm=0;
  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {
      for(int chan=0;chan<8;chan++) {
	int chanIndex = chip*CHANNELS_PER_LAB3+chan;
	if(chip==2 && chan==7) continue;
	for(int bin=1;bin<260;bin++) {
	  numNorm++;
	  avgNorm+=histNorm[chanIndex][rco]->GetBinContent(bin);
	}
      }
    }
  }
  avgNorm/=numNorm;

  TH1F *histBinWidthChip[3][2];
  TH1F *histNormChip[3][2];
  TH1F *histBinWidthChipErr[3][2];
  char outName[FILENAME_MAX];
  sprintf(outName,"binWidths_%3.0fMhz_%3.0fmV.txt",frequency*1000,ampmv);

  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {

      //Make the chip-by-chip hists
      sprintf(histName,"histNormChip_%d_%d",chip,rco);
      histNormChip[chip][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histBinWidthChip_%d_%d",chip,rco);
      histBinWidthChip[chip][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      sprintf(histName,"histBinWidthChipErr_%d_%d",chip,rco);
      histBinWidthChipErr[chip][rco] = new TH1F(histName,histName,260,-0.5,259.5);
      
      for(int chan=0;chan<8;chan++) {
	if(chip==2 && chan==7) continue;
	
	int chanIndex=chan+chip*9;   
	//Fill chip-by-chip hists
	histBinWidthChip[chip][rco]->Add(histBinWidth[chanIndex][rco]);
	histNormChip[chip][rco]->Add(histNorm[chanIndex][rco]); 
	
	//Scale chan-by-chan hists
	for(int bin=1;bin<260;bin++) {
	  double value=histBinWidth[chanIndex][rco]->GetBinContent(bin);
	  if(value>0) {
	    histBinWidthErr[chanIndex][rco]->SetBinContent(bin,TMath::Sqrt(value));
	  }
	}
	histBinWidth[chanIndex][rco]->Divide(histNorm[chanIndex][rco]);
	histBinWidth[chanIndex][rco]->Scale(0.5/frequency);
	histBinWidthErr[chanIndex][rco]->Divide(histNorm[chanIndex][rco]);
	//histBinWidthErr[chanIndex][rco]->Scale(1./avgNorm);
	histBinWidthErr[chanIndex][rco]->Scale(0.5/frequency);
      }
    }
  }


  Double_t avgChipNorm[3]={0};
  Int_t numChipNorm[3]={0};
  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {
      for(int bin=1;bin<260;bin++) {
	numChipNorm[chip]++;
	avgChipNorm[chip]+=histNormChip[chip][rco]->GetBinContent(bin);
      }
    }
    avgChipNorm[chip]/=numChipNorm[chip];
  }



  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {
      //Scale chip-by-chip hists
      for(int bin=1;bin<260;bin++) {
	double value=histBinWidthChip[chip][rco]->GetBinContent(bin);
	if(value>0) {
	  histBinWidthChipErr[chip][rco]->SetBinContent(bin,TMath::Sqrt(value));
	}
      }
      histBinWidthChip[chip][rco]->Divide(histNormChip[chip][rco]);
      // histBinWidthChip[chip][rco]->Scale(1./avgChipNorm[chip]);
      histBinWidthChip[chip][rco]->Scale(0.5/frequency);
      histBinWidthChipErr[chip][rco]->Divide(histNormChip[chip][rco]);
      //histBinWidthChipErr[chip][rco]->Scale(1./avgChipNorm[chip]);
      histBinWidthChipErr[chip][rco]->Scale(0.5/frequency);
    }
  }
      
      
  std::ofstream OutFile(outName);
  for(int chip=0;chip<3;chip++) {
    for(int rco=0;rco<2;rco++) {
      OutFile << chip << "\t" << rco << "\t";
    
      for(int samp=0;samp<260;samp++) {
	int bin=samp+1;
	if(bin<260) {
	  OutFile << histBinWidthChip[chip][rco]->GetBinContent(bin) << " ";
	}
	else {
	  OutFile << "0 ";
	}
      }
      OutFile << "\n";
    }
  }


  histFile->Write();

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
  for(chip=0;chip<LAB3_PER_TESTBED;++chip) {
    for(chan=0;chan<CHANNELS_PER_LAB3;++chan) {
      int chanIndex = chip*CHANNELS_PER_LAB3+chan;
      for(samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
	pedestalData[chip][chan][samp]=peds.chan[chanIndex].pedMean[samp];
      }
    }
  }
  gzclose(inPed);
}
