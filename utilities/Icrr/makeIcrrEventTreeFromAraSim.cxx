#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
 
using namespace std;

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"

#define HACK_FOR_ROOT

#include "araIcrrStructures.h"
#include "UsefulIcrrStationEvent.h"  

//Stuff from the sim event 
Double_t Volts[NUM_DIGITIZED_ICRR_CHANNELS][MAX_NUMBER_SAMPLES_LAB3];
Double_t Times[NUM_DIGITIZED_ICRR_CHANNELS][MAX_NUMBER_SAMPLES_LAB3];
Int_t NumPoints[NUM_DIGITIZED_ICRR_CHANNELS];
Double_t VoltsRF[RFCHANS_PER_ICRR][2*MAX_NUMBER_SAMPLES_LAB3];
Double_t TimesRF[RFCHANS_PER_ICRR][2*MAX_NUMBER_SAMPLES_LAB3];
Int_t NumPointsRF[RFCHANS_PER_ICRR];

Int_t NumRFChans;

TFile *outFp=0;
TTree *outTree=0;
UsefulIcrrStationEvent *outEvPtr=0;
Int_t run;

int processEvent(Int_t entry, char *outFileName, Int_t stationId);
int processRun(char *simFileName, char *outFileName, Int_t stationId);
int fillEvDataWithZeros();
int fillEvDataFromSim(Int_t eventNum);

int main(int argc, char **argv) {
  if(argc<4) {
    fprintf(stderr, "%s -  <simulationFileName> <outFileName> <stationId (Icrr==0 Station1==1)>\n", argv[0]);
    return -1;
  }

  char simFileName[FILENAME_MAX];
  char outFileName[FILENAME_MAX];
  Int_t stationId;

  sprintf(simFileName, "%s", argv[1]);
  sprintf(outFileName, "%s", argv[2]);
  stationId=atoi(argv[3]);

  processRun(simFileName, outFileName, stationId);

  return 0;
}

int processRun(char *simFileName, char *outFileName, Int_t stationId){

  fprintf(stderr, "simFileName %s\n", simFileName);
  fprintf(stderr, "outFileName %s\n", outFileName);

  TFile *simFp = TFile::Open(simFileName);
  if(!simFp){
    fprintf(stderr, "Error opening file %s\n", simFileName);
    return -1;    
  }

  TTree *simTree = (TTree*) simFp->Get("eventTree");
  if(!simTree){
    fprintf(stderr, "Error getting tree\n");
    return -1;    
  }

  UsefulIcrrStationEvent *simEvPtr=0;
  Int_t retVal = simTree->SetBranchAddress("event", &simEvPtr);
  
  Int_t numEntries = simTree->GetEntries();
  Int_t starEvery=numEntries/80;
  if(starEvery==0) starEvery=1;
  



  for(Int_t i=0;i<numEntries;i++){
    if(i%starEvery==0) fprintf(stderr, "*");
    simTree->GetEntry(i);
    
    //Zero everything
    for(Int_t chan=0;chan<NUM_DIGITIZED_ICRR_CHANNELS;chan++){
      for(Int_t samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++){
	Volts[chan][samp]=samp;//FIXME
	Times[chan][samp]=samp;//FIXME
      }
      NumPoints[chan]=MAX_NUMBER_SAMPLES_LAB3;//FIXME
    }
    for(Int_t rfChan=0;rfChan<RFCHANS_PER_ICRR;rfChan++){
      for(Int_t samp=0;samp<2*MAX_NUMBER_SAMPLES_LAB3;samp++){
	VoltsRF[rfChan][samp]=0;
	TimesRF[rfChan][samp]=0;
      }
      NumPointsRF[rfChan]=0;
    }
    NumRFChans=0;



    //Now fill with the real shit

    // fprintf(stderr, "1 - NumRFChans %i %i\n",NumRFChans, simEvPtr->numRFChans);//FIXME
    // fprintf(stderr, "2 - fNumPointsRF %i\n", simEvPtr->fNumPointsRF[0]);//FIXME
    NumRFChans=simEvPtr->numRFChans;
    for(Int_t rfChan=0;rfChan<NumRFChans;rfChan++){
      NumPointsRF[rfChan]=simEvPtr->fNumPointsRF[rfChan];
      for(Int_t samp=0;samp<NumPointsRF[rfChan];samp++){
	VoltsRF[rfChan][samp]=simEvPtr->fVoltsRF[rfChan][samp];
	TimesRF[rfChan][samp]=simEvPtr->fTimesRF[rfChan][samp];
      }
    }


    //Now put process the event
    processEvent(i, outFileName, stationId);

  }
  simTree->SetDirectory(0);

  fprintf(stderr, "\n");
  outTree->Write();
  outFp->Write();
  outFp->Close();

  return 0;
}


int processEvent(Int_t entry, char *outFileName, Int_t stationId){
  static int doneInit=0;
  if(!doneInit) {
    fprintf(stderr, "Creating file %s\n", outFileName);
    outFp = new TFile(outFileName, "RECREATE");
    outTree = new TTree("eventTree", "Tree of ARA Events - Steven Gerrard");
    outTree->SetDirectory(outFp);
    run=0;
    outTree->Branch("run",&run,"run/I");
    outTree->Branch("event","UsefulIcrrStationEvent",&outEvPtr);
    
    doneInit=1;
  }  
  if(outEvPtr) delete outEvPtr;
  outEvPtr = new UsefulIcrrStationEvent();
  

  fillEvDataWithZeros();
  fillEvDataFromSim(entry);

  outTree->Fill();

  return 0;
}


int fillEvDataWithZeros(){

  if(!outEvPtr) return -1;
  
    for(Int_t chan=0;chan<NUM_DIGITIZED_ICRR_CHANNELS;chan++){
      for(Int_t samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++){
	outEvPtr->fVolts[chan][samp]=samp;//FIXME
	outEvPtr->fTimes[chan][samp]=samp;//FIXME
      }
      outEvPtr->fNumPoints[chan]=MAX_NUMBER_SAMPLES_LAB3;//FIXME
    }
    for(Int_t rfChan=0;rfChan<RFCHANS_PER_ICRR;rfChan++){
      for(Int_t samp=0;samp<2*MAX_NUMBER_SAMPLES_LAB3;samp++){
	outEvPtr->fVoltsRF[rfChan][samp]=0;
	outEvPtr->fTimesRF[rfChan][samp]=0;
      }
      outEvPtr->fNumPointsRF[rfChan]=0;
    }
    outEvPtr->numRFChans=0;


    //    for(Int_t chan=0;chan<NUM_DIGITIZED_ICRR_CHANNELS;chan++){
    //      outEvPtr->chan[chan]=0;
    //    }
    //    outEvPtr->whichPeds=0;
    //    outEvPtr->head=0;
    //    outEvPtr->trig=0;
    //    outEvPtr->hk=0;
  return 0;
}

int fillEvDataFromSim(Int_t eventNum){
  if(!outEvPtr) return -1;

  outEvPtr->numRFChans=NumRFChans;
  for(Int_t rfChan=0;rfChan<NumRFChans;rfChan++){
    outEvPtr->fNumPointsRF[rfChan]=NumPointsRF[rfChan];
    for(Int_t samp=0;samp<NumPointsRF[rfChan];samp++){
      outEvPtr->fVoltsRF[rfChan][samp]=VoltsRF[rfChan][samp];
      outEvPtr->fTimesRF[rfChan][samp]=TimesRF[rfChan][samp];
    }
  }
  outEvPtr->head.eventNumber=eventNum;

  // fprintf(stderr, "3 - NumRFChans %i %i\n", NumRFChans, outEvPtr->numRFChans);//FIXME
  // fprintf(stderr, "4 - NumPoints %i %i\n", NumPointsRF[0], outEvPtr->fNumPointsRF[0]);//FIXME
  
  return 0;
}
