////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//// generateDeltaTs
////    Simple piece of code to investigate the timing calibration for ARA
////    April 2013,  r.nichol@ucl.ac.uk 
////////////////////////////////////////////////////////////////////////////////

//Includes
#include <iostream>
#include <fstream>

//AraRoot Includes
#include "RawIcrrStationEvent.h"
#include "RawAtriStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "UsefulIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "AraGeomTool.h"
#include "AraEventCalibrator.h"
#include "FFTtools.h"

//ROOT Includes
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TTimeStamp.h"
#include "TSystem.h"

#include <map>

//#define DEBUG_FILE 1

RawIcrrStationEvent *rawIcrrEvPtr=0;
RawAtriStationEvent *rawAtriEvPtr=0;
RawAraStationEvent *rawEvPtr=0;
UsefulIcrrStationEvent *realIcrrEvPtr=0;
UsefulAtriStationEvent *realAtriEvPtr=0;
UsefulAraStationEvent *realEvPtr=0;

TGraph *correlateAndAverage(Int_t numGraphs, TGraph **grPtrPtr, Double_t *deltaTVals=0);


void usage(char **argv) 
{  
  std::cout << "Usage\n" << argv[0] << " <input file>\n";
  std::cout << "e.g.\n" << argv[0] << " http://www.hep.ucl.ac.uk/uhen/ara/monitor/root/run1841/event1841.root\n";  
}




int main(int argc, char **argv) {
  if(argc<2) {
    usage(argv);
    return -1;
  }


  TFile *fp = TFile::Open(argv[1]);
  if(!fp) {
    std::cerr << "Can't open file\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fp->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't find eventTree\n";
    return -1;
  }
   
  //Now check the electronics type of the station


  int isIcrrEvent=0;
  int isAtriEvent=0;

  Int_t runNumber;
  //Check an event in the run Tree and see if it is station1 or TestBed (stationId<2)
  eventTree->SetBranchAddress("event",&rawEvPtr);
  eventTree->SetBranchAddress("run",&runNumber);
  eventTree->GetEntry(0);
  int stationId=0;
  stationId=rawEvPtr->stationId;
    
  if((rawEvPtr->stationId)<2){
    isIcrrEvent=1;
    isAtriEvent=0;
  }
  else{
    isIcrrEvent=0;
    isAtriEvent=1; 
  }
  eventTree->ResetBranchAddresses();

  AraGeomTool *fGeomTool = AraGeomTool::Instance();
  AraStationInfo *fStationInfo = fGeomTool->getStationInfo(stationId);
  Int_t numVPols=fStationInfo->getNumAntennasByPol(AraAntPol::kVertical);
  Int_t numHPols=fStationInfo->getNumAntennasByPol(AraAntPol::kHorizontal);


  //Now set the appropriate branch addresses
  //The Icrr case
  if(isIcrrEvent){

    eventTree->SetBranchAddress("event", &rawIcrrEvPtr);
    rawEvPtr=rawIcrrEvPtr;
    std::cerr << "Set Branch address to Icrr\n";

  }
  //The Atri case
  else{

    eventTree->SetBranchAddress("event", &rawAtriEvPtr);
    rawEvPtr=rawAtriEvPtr;
    std::cerr << "Set Branch address to Atri\n";

  }

  //Now we set up out run list
  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;

  //jpd print to screen some info
  std::cerr << "isAtri " << isAtriEvent << " isIcrr " << isIcrrEvent << " number of entries is " <<  numEntries << std::endl;

  eventTree->GetEntry(0);

  //Pedestal fun
  if(argc>2) {
    std::ifstream PedList(argv[2]);    
    int pedRun,lastPedRun=-1;
    char pedFileName[FILENAME_MAX];
    char lastPedFileName[FILENAME_MAX];
    while(PedList >> pedRun >> pedFileName) {
      if(pedRun>runNumber) {
	//Take the last guy
	if(lastPedRun==-1) {
	  lastPedRun=pedRun;
	  strncpy(lastPedFileName,pedFileName,FILENAME_MAX);
	}	  
	break;
      }
      lastPedRun=pedRun;
      strncpy(lastPedFileName,pedFileName,FILENAME_MAX);
    }
    //Got the pedestal run
    AraEventCalibrator::Instance()->setAtriPedFile(lastPedFileName, stationId);    
  }


  //  numEntries=20;
  Double_t dt3[8];
  TFile *fpOut = new TFile("calPulserTimes.root","RECREATE");
  TTree *dtTree = new TTree("dtTree","Tree of deltaTs");
  dtTree->Branch("dt3",&dt3,"dt3[8]/D");
  

  for(Long64_t event=0;event<numEntries;event++) {
    if(event%starEvery==0) {
      std::cerr << "*";       
    }

    //This line gets the RawIcrr or RawAtri Event
    eventTree->GetEntry(event);

    //Here we create a useful event Either an Icrr or Atri event
    Int_t eventNumber=event;
    Int_t unixTime=1;

    Double_t triggerTime=0;

    if(isIcrrEvent){
      if(realIcrrEvPtr) delete realIcrrEvPtr;
      realIcrrEvPtr = new UsefulIcrrStationEvent(rawIcrrEvPtr, AraCalType::kLatestCalib);
      realEvPtr=realIcrrEvPtr;
      eventNumber=rawIcrrEvPtr->head.eventNumber;
      unixTime=rawIcrrEvPtr->head.unixTime;
      triggerTime=rawIcrrEvPtr->getRubidiumTriggerTimeInSec();
    }
    else if(isAtriEvent){
      if(realAtriEvPtr) delete realAtriEvPtr;
      realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kLatestCalib);
      realEvPtr=realAtriEvPtr;
      eventNumber=rawAtriEvPtr->eventNumber;
      unixTime=rawAtriEvPtr->unixTime;
      triggerTime=rawAtriEvPtr->timeStamp;
    }
    int isSoftTrig=0;
    if(rawAtriEvPtr->numReadoutBlocks<80) isSoftTrig=1;
    int isCalPulser=0;
    if(rawAtriEvPtr->isCalpulserEvent()) isCalPulser=1;
    if(!isCalPulser) continue;



    TGraph *grV[10]={0};
    TGraph *grH[10]={0};

    
    Double_t deltaTVs[10]={0};
    Double_t maxV[10]={0};
    Int_t indexV[10]={0};
    Int_t numPoints=0;
    for( int i=0; i<numVPols; ++i ) {
      int rfchan=fStationInfo->getRFChanByPolAndAnt(i,AraAntPol::kVertical);
      TGraph *grTemp=realAtriEvPtr->getGraphFromRFChan(rfchan);
      grV[i]=FFTtools::getInterpolatedGraph(grTemp,0.1);
      delete grTemp;
      Int_t numVals=grV[i]->GetN();
      if(numVals>numPoints) numPoints=numVals;
      Double_t maxValue=TMath::MaxElement(numVals,grV[i]->GetY());
      Double_t minValue=TMath::MinElement(numVals,grV[i]->GetY());
      maxV[i]=maxValue;
      if(TMath::Abs(minValue)>maxValue) maxV[i]=TMath::Abs(minValue);
    }
    TMath::Sort(numVPols,maxV,indexV);
    for(int ant=0;ant<numVPols;ant++) {
      //      std::cout << ant << "\t" << indexV[ant] << "\t" << maxV[ant] << "\n";
    }

    TGraph *grVOrdered[10];
    for(int ant=0;ant<numVPols;ant++) {
      grVOrdered[ant]=grV[indexV[ant]];
    }
    Double_t orderedDeltaTs[10];
    TGraph *grSum=correlateAndAverage(numVPols,grVOrdered,orderedDeltaTs);
    for( int i=0; i<numVPols; ++i ) {
      deltaTVs[indexV[i]]=orderedDeltaTs[i];      
      //      std::cout << eventNumber << "\t" << i << "\t" << indexV[i] << "\t" << orderedDeltaTs[i] << "\n";
      delete grV[i];
    }  

    //Arbitrary choice to set this as the reference antenna... well not quite arbitrary but could be any of teh other six
    Double_t deltaT0=deltaTVs[3];
    for( int i=0; i<numVPols; ++i ) {
      deltaTVs[i]-=deltaT0;
      dt3[i]=deltaTVs[i];
      //      std::cout << eventNumber << "\t" << i << "\t" << deltaTVs[i] << "\n";
    }
    dtTree->Fill();

  }
  dtTree->AutoSave();
  std::cerr << "\n";


}

TGraph *correlateAndAverage(Int_t numGraphs, TGraph **grPtrPtr, Double_t *deltaTVals)
{
#ifdef DEBUG_FILE
  char grName[180];
  TFile *fpDebug = new TFile("debug.root","RECREATE");
  for(int i=0;i<numGraphs;i++) {
    sprintf(grName,"gr%d",i);
    grPtrPtr[i]->SetName(grName);
    grPtrPtr[i]->Write();
  }
#endif
  //Assume they are all at same sampling rate
  if(numGraphs<2) return NULL;
  TGraph *grA = grPtrPtr[0];
  Int_t numPoints=grA->GetN();  
  Double_t *timeVals= grA->GetX();
  Double_t *safeTimeVals = new Double_t[numPoints];
  Double_t *sumVolts = new Double_t [numPoints];
  for(int i=0;i<numPoints;i++) 
    safeTimeVals[i]=timeVals[i];  
  
  int countWaves=1;
  if( deltaTVals) deltaTVals[0]=0;
  for(int graphNum=1;graphNum<numGraphs;graphNum++) {
    TGraph *grB = grPtrPtr[graphNum];
    if(grB->GetN()<numPoints)
      numPoints=grB->GetN();
    TGraph *grCorAB = FFTtools::getCorrelationGraph(grA,grB);
#ifdef DEBUG_FILE
    sprintf(grName,"grCor%d",graphNum);
    grCorAB->SetName(grName);
    grCorAB->Write();
#endif

    Int_t peakBin = FFTtools::getPeakBin(grCorAB);
    Double_t *corTVals=grCorAB->GetX();
    //    cout << peakBin << "\t" << grCorAB->GetN() << endl;
    Int_t offset=peakBin-(grCorAB->GetN()/2);
    if(deltaTVals) deltaTVals[graphNum]=corTVals[peakBin];
    //    cout << deltaTVals[peakBin] << "\t" << safeTimeVals[offset] << endl;
 
    Double_t *aVolts = grA->GetY();
    Double_t *bVolts = grB->GetY();

    for(int ind=0;ind<numPoints;ind++) {
      int aIndex=ind;
      int bIndex=ind-offset;
      
      if(bIndex>=0 && bIndex<numPoints) {
	sumVolts[ind]=(aVolts[aIndex]+bVolts[bIndex]);
      }
      else {
	sumVolts[ind]=aVolts[aIndex];
      }
    }
    

    TGraph *grComAB = new TGraph(numPoints,safeTimeVals,sumVolts);

    //    delete grB;
    delete grCorAB;
    if(graphNum>1)
      delete grA;
    grA=grComAB;
    countWaves++;

  }
  for(int i=0;i<numPoints;i++) {
    sumVolts[i]/=countWaves;
  }
  Double_t meanVal=TMath::Mean(numPoints,sumVolts);
  for(int i=0;i<numPoints;i++) {
    sumVolts[i]-=meanVal;
  }
  delete grA;
  TGraph *grRet = new TGraph(numPoints,safeTimeVals,sumVolts);
#ifdef DEBUG_FILE
  grRet->SetName("grCorSum");
  grRet->Write();
  fpDebug->Close();
#endif
  delete [] safeTimeVals;
  delete [] sumVolts;
  return grRet;

}
