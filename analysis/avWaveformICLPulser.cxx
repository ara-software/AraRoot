////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////  exampleLoop.cxx 
////      Just a very simple example that loops over RawAraEvent objects 
////      calibrating them to make a UsefulAraEvent
////
////    Feb 2011,  rjn@hep.ucl.ac.uk 
////////////////////////////////////////////////////////////////////////////////

//Includes
#include <iostream>

//AraRoot Includes
#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "AraEventCalibrator.h"
#include "FFTtools.h"


//Include FFTtools.h if you want to ask the correlation, etc. tools

//ROOT Includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"

RawAtriStationEvent *rawAtriEvPtr;
UsefulAtriStationEvent *realAtriEvPtr;

bool isICLPulserEvent(RawAtriStationEvent *evPtr);

int main(int argc, char **argv)
{

  if(argc<4) {
    std::cout << "Usage\n" << argv[0] << " <run file> <ped file> <out file>\n";
    std::cout << "e.g.\n" << argv[0] << " http://www.hep.ucl.ac.uk/uhen/ara/monitor/root/run1841/event1841.root\n";
    return 0;
  }
  
  char pedFileName[FILENAME_MAX];
  char outFileName[FILENAME_MAX];
  sprintf(pedFileName, "%s", argv[2]);
  sprintf(outFileName, "%s", argv[3]);
  
  printf("------------------------------------------------------------------------\n");
  printf("%s\n", argv[0]);
  printf("runFileName %s\n", argv[1]);
  printf("pedFileName %s\n", pedFileName);
  printf("outFileName %s\n", outFileName);
  printf("------------------------------------------------------------------------\n");
  TFile *fpOut = new TFile(outFileName, "RECREATE");
  TGraph *grAv[3][8]={{0}};
  TGraph *grAvFFT[3][8]={{0}};
  TCanvas *canAv[2]={0};

  const Int_t grSize=20000;
  const Int_t maxNumWaveforms=200;
  const Double_t intSample=(1./1.6)/32;
  Double_t grAvXVals[3][8][grSize]={{{0}}};
  Double_t grAvYVals[3][8][grSize]={{{0}}};
  Int_t numWaveforms[3][8]={{0}};
  Int_t chanMap[3][8]={{0,1,8,9,16,17,24,25},{3,4,10,11,18,19,26,27},{5,6,28,29,-1,-1}};
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
   
   eventTree->SetBranchAddress("event",&rawAtriEvPtr);
   Long64_t numEntries=eventTree->GetEntries();
   Long64_t starEvery=numEntries/80;
   Int_t stationId=0;
   eventTree->GetEntry(0);
   stationId = rawAtriEvPtr->stationId;
   if(starEvery==0) starEvery++;


   printf("stationId %i numEntries %li\n", stationId, (long int)numEntries);

   AraEventCalibrator *calibrator = AraEventCalibrator::Instance();
   calibrator->setAtriPedFile(pedFileName, stationId);

   for(Long64_t event=0;event<numEntries;event++) {
     if(event%starEvery==0) {
       std::cerr << "*";       
     }

     eventTree->GetEntry(event);
     
     //     if(rawAtriEvPtr->isCalpulserEvent()==0) continue;
     
     if(!isICLPulserEvent(rawAtriEvPtr)) continue;

     realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kFirstCalib);
     
     Int_t chan=0;
     Int_t ant=0;
     Int_t pol=0;
     for(pol=0;pol<2;pol++){
       for(ant=0;ant<8;ant++){
	 chan=chanMap[pol][ant];
	 //printf("pol %i ant %i chan %i\n", pol, ant, chan);
	 if(chan<0) continue;
	 if(numWaveforms[pol][ant]>=maxNumWaveforms) continue;
	 
	 TGraph *grChan = realAtriEvPtr->getGraphFromElecChan(chan);	 	 
	 TGraph *chanInt = FFTtools::getInterpolatedGraph(grChan,intSample); 
	 
	 //	 printf("grChan N %i chanInt N %i\n", grChan->GetN(), chanInt->GetN());



	 //second time etc
	 if(numWaveforms[pol][ant]==0){
	   Double_t *xVals = chanInt->GetX();
	   Double_t *yVals = chanInt->GetY();
	   Int_t numSamples = chanInt->GetN();
	   for(int samp=0;samp<grSize;samp++){
	     if(samp>numSamples) continue;
	     grAvYVals[pol][ant][samp]+=yVals[samp];
	     grAvXVals[pol][ant][samp]=xVals[samp];
	   }
	   numWaveforms[pol][ant]++;
	 }
	 else{
	   TGraph *grTemplate = new TGraph(grSize, grAvXVals[pol][ant], grAvYVals[pol][ant]);	 
	   TGraph *grCorr = FFTtools::getCorrelationGraph(chanInt, grTemplate);
	   Int_t peakBin =  FFTtools::getPeakBin(grCorr);
	   
	   Double_t *xVals = chanInt->GetX();
	   Double_t *yVals = chanInt->GetY();
	   Int_t numSamples = chanInt->GetN();
	   //	   printf("peakBin %i numSamples %d proposed %d\n", peakBin, grCorr->GetN()/2,  peakBin - grCorr->GetN()/2);
	   Int_t deltaBin = peakBin - grCorr->GetN()/2;
	   for(int samp=0;samp<grSize;samp++){
	     if(samp>numSamples) continue;
	     //	     grAvXVals[pol][ant][samp]=xVals[samp];

	     Int_t newSample = samp+deltaBin;
	     if(newSample<0 || newSample>grSize) continue;
	     grAvYVals[pol][ant][samp]+=yVals[newSample];
	     
	   }
	   numWaveforms[pol][ant]++;
	   delete grCorr;
	   delete grTemplate;

	 }
	 
	 delete grChan;
	 delete chanInt;
       }
     }
     delete realAtriEvPtr;
   
     if(maxNumWaveforms<=numWaveforms[0][0]) break;

   }
   


   Int_t chan=0;
   Int_t ant=0;
   Int_t pol=0;
   for(pol=0;pol<2;pol++){
     char canName[100];
     sprintf(canName, "can%sPol_%i_waveforms", pol ? "H": "V", numWaveforms[pol][0]);
     canAv[pol]=new TCanvas(canName, canName);
     canAv[pol]->Divide(4,2);
     for(ant=0;ant<8;ant++){
       Int_t row=0, column=0;
       row = ant%2;
       column = ant/2;
       canAv[pol]->cd(1+column+row*4);
       chan=chanMap[pol][ant];
       
       for(int samp=0;samp<grSize;samp++){
	 if(numWaveforms[pol][ant]==0) grAvYVals[pol][ant][samp]=0;
	 else grAvYVals[pol][ant][samp]/=numWaveforms[pol][ant];
       }
       grAv[pol][ant] = new TGraph(grSize, grAvXVals[pol][ant], grAvYVals[pol][ant]);
       char grName[100];
       sprintf(grName, "grAv_%sPol_ant%i", pol ? "H": "V", ant);
       grAv[pol][ant]->SetName(grName);
       grAv[pol][ant]->SetTitle(grName);
       grAv[pol][ant]->GetXaxis()->SetTitle("Time (ns)");
       grAv[pol][ant]->GetYaxis()->SetTitle("Voltage (mV)");
       fpOut->cd();
       grAv[pol][ant]->Draw("AL");
       grAv[pol][ant]->Write();

       grAvFFT[pol][ant] = FFTtools::makePowerSpectrumMilliVoltsNanoSecondsdB(grAv[pol][ant]);
       sprintf(grName, "grAvFFT_%sPol_ant%i", pol ? "H": "V", ant);
       grAvFFT[pol][ant]->SetName(grName);
       grAvFFT[pol][ant]->SetTitle(grName);
       grAvFFT[pol][ant]->GetXaxis()->SetTitle("Frequency (MHz)");
       grAvFFT[pol][ant]->GetYaxis()->SetTitle("Power ()");

       grAvFFT[pol][ant]->Write();
       //       printf("%sPol ant %i numWaveforms %i\n", pol ? "H" : "V", ant, numWaveforms[pol][ant]);

     }
     canAv[pol]->SetTitle(canName);
     canAv[pol]->Write();
     
   }
   fpOut->Write();

   std::cerr << "\n";

}




bool isICLPulserEvent(RawAtriStationEvent *evPtr){
  Int_t stationId = evPtr->stationId;
  UInt_t timeStamp = evPtr->timeStamp;
  
  if(stationId==3){
    if(evPtr->numReadoutBlocks!=80&&TMath::Abs(timeStamp-5e7)<1e6) return true;
  }
  else if(stationId==2){
    if(TMath::Abs(timeStamp-1.510e3)<5e1) return true;

  }
  else if(stationId==100){

    if(TMath::Abs(timeStamp-9.40e2)<5e1) return true;

  }
  

  return false;

}
