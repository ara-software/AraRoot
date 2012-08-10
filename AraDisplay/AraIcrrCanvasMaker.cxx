//////////////////////////////////////////////////////////////////////////////
/////  AraIcrrCanvasMaker.cxx        ARA Event Canvas make               /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ARA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "AraIcrrCanvasMaker.h"
#include "AraGeomTool.h"
#include "UsefulIcrrStationEvent.h"
#include "AraWaveformGraph.h"
#include "AraEventCorrelator.h"
#include "AraFFTGraph.h"
#include "araIcrrDefines.h"


#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TVector3.h"
#include "TROOT.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TText.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "TH2.h"
#include "TList.h"
#include "TFile.h"
#include "TObject.h"
#include "TTimeStamp.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TView3D.h"

#include "FFTtools.h"


AraIcrrCanvasMaker*  AraIcrrCanvasMaker::fgInstance = 0;
AraGeomTool *fIcrrACMGeomTool=0;




AraWaveformGraph *grIcrrElec[NUM_DIGITIZED_ICRR_CHANNELS]={0}; 
AraWaveformGraph *grIcrrElecFiltered[NUM_DIGITIZED_ICRR_CHANNELS]={0};
AraWaveformGraph *grIcrrElecHilbert[NUM_DIGITIZED_ICRR_CHANNELS]={0};
AraFFTGraph *grIcrrElecFFT[NUM_DIGITIZED_ICRR_CHANNELS]={0};
AraFFTGraph *grIcrrElecAveragedFFT[NUM_DIGITIZED_ICRR_CHANNELS]={0};

AraWaveformGraph *grIcrrRFChan[MAX_RFCHANS_PER_ICRR]={0};
AraWaveformGraph *grIcrrRFChanFiltered[MAX_RFCHANS_PER_ICRR]={0};
AraWaveformGraph *grIcrrRFChanHilbert[MAX_RFCHANS_PER_ICRR]={0};
AraFFTGraph *grIcrrRFChanFFT[MAX_RFCHANS_PER_ICRR]={0};
AraFFTGraph *grIcrrRFChanAveragedFFT[MAX_RFCHANS_PER_ICRR]={0};


TH1D *AraIcrrCanvasMaker::getFFTHisto(int ant)
{
  if(ant<0 || ant>=RFCHANS_PER_ICRR) return NULL;
  if(grIcrrRFChan[ant])
    return grIcrrRFChan[ant]->getFFTHisto();
    return NULL;

}

AraIcrrCanvasMaker::AraIcrrCanvasMaker(AraCalType::AraCalType_t calType)
{
  //Default constructor
  fIcrrACMGeomTool=AraGeomTool::Instance();
  fNumAntsInMap=4;
  fWebPlotterMode=0;
  fPassBandFilter=0;
  fNotchFilter=0;
  fLowPassEdge=200;
  fHighPassEdge=1200;
  fLowNotchEdge=235;
  fHighNotchEdge=500;
  fMinVoltLimit=-60;
  fMaxVoltLimit=60;
  fPhiMax=0;
  fMinClockVoltLimit=-200;
  fMaxClockVoltLimit=200;
  fAutoScale=1;
  fMinTimeLimit=0;
  fMaxTimeLimit=0;
  // if(AraCalType::hasCableDelays(calType)) {
  //   fMinTimeLimit=-200;
  //   fMaxTimeLimit=150;
  // }
  fMinPowerLimit=-60;
  fMaxPowerLimit=60;
  fMinFreqLimit=0;
  fMaxFreqLimit=1200;
  fWaveformOption=AraDisplayFormatOption::kWaveform;
  fRedoEventCanvas=0;
  //fRedoSurfCanvas=0;
  fLastWaveformFormat=AraDisplayFormatOption::kWaveform;
  fNewEvent=1;
  fCalType=calType;
  fCorType=AraCorrelatorType::kSphericalDist40;
  fgInstance=this;
  memset(grIcrrElec,0,sizeof(AraWaveformGraph*)*NUM_DIGITIZED_ICRR_CHANNELS);
  memset(grIcrrElecFiltered,0,sizeof(AraWaveformGraph*)*NUM_DIGITIZED_ICRR_CHANNELS);
  memset(grIcrrElecHilbert,0,sizeof(AraWaveformGraph*)*NUM_DIGITIZED_ICRR_CHANNELS);
  memset(grIcrrElecFFT,0,sizeof(AraFFTGraph*)*NUM_DIGITIZED_ICRR_CHANNELS);
  memset(grIcrrElecAveragedFFT,0,sizeof(AraFFTGraph*)*NUM_DIGITIZED_ICRR_CHANNELS);

  memset(grIcrrRFChan,0,sizeof(AraWaveformGraph*)*MAX_RFCHANS_PER_ICRR);
  memset(grIcrrRFChanFiltered,0,sizeof(AraWaveformGraph*)*MAX_RFCHANS_PER_ICRR);
  memset(grIcrrRFChanHilbert,0,sizeof(AraWaveformGraph*)*MAX_RFCHANS_PER_ICRR);
  memset(grIcrrRFChanFFT,0,sizeof(AraFFTGraph*)*MAX_RFCHANS_PER_ICRR);
  memset(grIcrrRFChanAveragedFFT,0,sizeof(AraFFTGraph*)*MAX_RFCHANS_PER_ICRR);  
  switch(fCalType) {
  case AraCalType::kNoCalib:
    fMaxVoltLimit=3000;
    fMinVoltLimit=1000;
  case AraCalType::kJustUnwrap:
    fMinTimeLimit=0;
    fMaxTimeLimit=260;
    break;
  default:
    break;
  }


}

AraIcrrCanvasMaker::~AraIcrrCanvasMaker()
{
   //Default destructor
}



//______________________________________________________________________________
AraIcrrCanvasMaker*  AraIcrrCanvasMaker::Instance()
{
   //static function
   return (fgInstance) ? (AraIcrrCanvasMaker*) fgInstance : new AraIcrrCanvasMaker();
}


TPad *AraIcrrCanvasMaker::getEventInfoCanvas(UsefulIcrrStationEvent *evPtr,  TPad *useCan, Int_t runNumber)
{
   static UInt_t lastEventNumber=0;
   static TPaveText *leftPave=0;
   static TPaveText *midLeftPave=0;
   static TPaveText *midRightPave=0;
   static TPaveText *rightPave=0;


   if(!fIcrrACMGeomTool)
      fIcrrACMGeomTool=AraGeomTool::Instance();
   char textLabel[180];
   TPad *topPad;
   if(!useCan) {
      topPad = new TPad("padEventInfo","padEventInfo",0.2,0.9,0.8,1);
      topPad->Draw();
   }
   else {
      topPad=useCan;
   } 
   if(1) {
     fNewEvent=1;
     topPad->Clear();
     topPad->SetTopMargin(0.05);
     topPad->Divide(4,1);
     topPad->cd(1);
     if(leftPave) delete leftPave;
     leftPave = new TPaveText(0,0.1,1,0.9);
     leftPave->SetName("leftPave");
     leftPave->SetBorderSize(0);
     leftPave->SetFillColor(0);
     leftPave->SetTextAlign(13);
     if(runNumber) {
       sprintf(textLabel,"Run: %d",runNumber);
       TText *runText = leftPave->AddText(textLabel);
       runText->SetTextColor(50);
     }
     if(evPtr->stationId==0) sprintf(textLabel,"TestBed Event: %d",evPtr->head.eventNumber);
     else if(evPtr->stationId==1) sprintf(textLabel,"Station1 Event: %d",evPtr->head.eventNumber);
     else sprintf(textLabel,"Event: %d",evPtr->head.eventNumber);

     TText *eventText = leftPave->AddText(textLabel);
     eventText->SetTextColor(50);
     leftPave->Draw();


     topPad->cd(2);
     gPad->SetRightMargin(0);
     gPad->SetLeftMargin(0);
     if(midLeftPave) delete midLeftPave;
     midLeftPave = new TPaveText(0,0.1,0.99,0.9);
     midLeftPave->SetName("midLeftPave");
     midLeftPave->SetBorderSize(0);
     midLeftPave->SetTextAlign(13);
     TTimeStamp trigTime((time_t)evPtr->head.unixTime,(Int_t)1000*evPtr->head.unixTimeUs);
     sprintf(textLabel,"Time: %s",trigTime.AsString("s"));
     TText *timeText = midLeftPave->AddText(textLabel);
     timeText->SetTextColor(1);
     sprintf(textLabel,"Sub: %f",(trigTime.GetNanoSec()/1e9));
     TText *timeText2 = midLeftPave->AddText(textLabel);
     timeText2->SetTextColor(1);
     midLeftPave->Draw();
     //     midLeftPave->Modified();
     gPad->Modified();
     gPad->Update();
     
     topPad->cd(3);
     if(midRightPave) delete midRightPave;
     midRightPave = new TPaveText(0,0.1,1,0.95);
     midRightPave->SetBorderSize(0);
     midRightPave->SetTextAlign(13);
     sprintf(textLabel,"PPS Num %d",
	     evPtr->trig.ppsNum);
     midRightPave->AddText(textLabel);
     sprintf(textLabel,"Trig Type %d%d%d",
	     evPtr->trig.isInTrigType(2),
	     evPtr->trig.isInTrigType(1),
	     evPtr->trig.isInTrigType(0));
     midRightPave->AddText(textLabel);
     sprintf(textLabel,"Pattern %d%d%d%d%d%d%d%d",
	     evPtr->trig.isInTrigPattern(7),
	     evPtr->trig.isInTrigPattern(6),
	     evPtr->trig.isInTrigPattern(5),
	     evPtr->trig.isInTrigPattern(4),
	     evPtr->trig.isInTrigPattern(3),
	     evPtr->trig.isInTrigPattern(2),
	     evPtr->trig.isInTrigPattern(1),
	     evPtr->trig.isInTrigPattern(0));
     midRightPave->AddText(textLabel);
     midRightPave->Draw();

     
     topPad->cd(4);
     if(rightPave) delete rightPave;
     rightPave = new TPaveText(0,0.1,1,0.95);
     rightPave->SetBorderSize(0);
     rightPave->SetTextAlign(13); 
     rightPave->Draw();
     topPad->Update();
     topPad->Modified();
               
     lastEventNumber=evPtr->head.eventNumber;
   }
      
   return topPad;
}


TPad *AraIcrrCanvasMaker::quickGetEventViewerCanvasForWebPlottter(UsefulIcrrStationEvent *evPtr,  TPad *useCan)
{
  TPad *retCan=0;
  fWebPlotterMode=1;
  //  static Int_t lastEventView=0;

  if(fAutoScale) {
    fMinVoltLimit=1e9;
    fMaxVoltLimit=-1e9;
    fMinClockVoltLimit=1e9;
    fMaxClockVoltLimit=-1e9;
  }



  for(int chan=0;chan<NUM_DIGITIZED_ICRR_CHANNELS;chan++) {
    if(grIcrrElec[chan]) delete grIcrrElec[chan];
    if(grIcrrElecFFT[chan]) delete grIcrrElecFFT[chan];
    if(grIcrrElecHilbert[chan]) delete grIcrrElecHilbert[chan];
    grIcrrElec[chan]=0;
    grIcrrElecFFT[chan]=0;
    grIcrrElecHilbert[chan]=0;
    //    if(grIcrrElecAveragedFFT[chan]) delete grIcrrElecAveragedFFT[chan];
    
    TGraph *grTemp = evPtr->getGraphFromElecChan(chan);
    grIcrrElec[chan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
    grIcrrElec[chan]->setElecChan(chan);
      //      std::cout << evPtr->head.eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grIcrrElec[chan]->GetRMS(2) << std::endl;
    
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grIcrrElec[chan]->getFFT();
      grIcrrElecFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grIcrrElecAveragedFFT[chan]) {
	grIcrrElecAveragedFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grIcrrElecAveragedFFT[chan]->AddFFT(grIcrrElecFFT[chan]);
      }
      delete grTempFFT;      
    }


    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      for(int i=0;i<numPoints;i++) {	
	if(yVals[i]<fMinVoltLimit)
	  fMinVoltLimit=yVals[i];
	if(yVals[i]>fMaxVoltLimit)
	  fMaxVoltLimit=yVals[i];	
      }      
    }

    delete grTemp;
  }

  

  for(int rfchan=0;rfchan<RFCHANS_PER_ICRR;rfchan++) {
    if(grIcrrRFChan[rfchan]) delete grIcrrRFChan[rfchan];
    if(grIcrrRFChanFFT[rfchan]) delete grIcrrRFChanFFT[rfchan];
    if(grIcrrRFChanHilbert[rfchan]) delete grIcrrRFChanHilbert[rfchan];
    grIcrrRFChan[rfchan]=0;
    grIcrrRFChanFFT[rfchan]=0;
    grIcrrRFChanHilbert[rfchan]=0;
    //    if(grIcrrRFChanAveragedFFT[chan]) delete grIcrrRFChanAveragedFFT[chan];
    //Need to work out how to do this
    TGraph *grTemp = evPtr->getGraphFromRFChan(rfchan);
    grIcrrRFChan[rfchan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
    grIcrrRFChan[rfchan]->setRFChan(rfchan, evPtr->stationId);
      //      std::cout << evPtr->head.eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grIcrrElec[chan]->GetRMS(2) << std::endl;
    
 
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grIcrrRFChan[rfchan]->getFFT();
      grIcrrRFChanFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grIcrrRFChanAveragedFFT[rfchan]) {
	grIcrrRFChanAveragedFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grIcrrRFChanAveragedFFT[rfchan]->AddFFT(grIcrrRFChanFFT[rfchan]);
      }
      delete grTempFFT;      
    }



    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      for(int i=0;i<numPoints;i++) {	
	if(yVals[i]<fMinVoltLimit)
	  fMinVoltLimit=yVals[i];
	if(yVals[i]>fMaxVoltLimit)
	  fMaxVoltLimit=yVals[i];	
      }      
    }
    delete grTemp;
  }


  if(fAutoScale) {
    if(fCalType!=AraCalType::kNoCalib && fCalType!=AraCalType::kJustUnwrap) {
      if(fMaxVoltLimit>-1*fMinVoltLimit) {
	fMinVoltLimit=-1*fMaxVoltLimit;
      }
      else {
	fMaxVoltLimit=-1*fMinVoltLimit;
      }
    }
  

   if(fMaxClockVoltLimit>-1*fMinClockVoltLimit) {
      fMinClockVoltLimit=-1*fMaxClockVoltLimit;
    }
    else {
      fMaxClockVoltLimit=-1*fMinClockVoltLimit;
    }
  }
    

  fRedoEventCanvas=0;

  retCan=AraIcrrCanvasMaker::getCanvasForWebPlotter(evPtr,useCan);

 

  return retCan;

}

TPad *AraIcrrCanvasMaker::getEventViewerCanvas(UsefulIcrrStationEvent *evPtr,
					   TPad *useCan)
{
  TPad *retCan=0;

  static UInt_t lastEventNumber=0;

  if(fAutoScale) {
    fMinVoltLimit=1e9;
    fMaxVoltLimit=-1e9;
    fMinClockVoltLimit=0;
    fMaxClockVoltLimit=0;
  }



  for(int chan=0;chan<NUM_DIGITIZED_ICRR_CHANNELS;chan++) {
    if(grIcrrElec[chan]) delete grIcrrElec[chan];
    if(grIcrrElecFFT[chan]) delete grIcrrElecFFT[chan];
    if(grIcrrElecHilbert[chan]) delete grIcrrElecHilbert[chan];
    grIcrrElec[chan]=0;
    grIcrrElecFFT[chan]=0;
    grIcrrElecHilbert[chan]=0;
    
    TGraph *grTemp = evPtr->getGraphFromElecChan(chan);
    grIcrrElec[chan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
    grIcrrElec[chan]->setElecChan(chan);


    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grIcrrElec[chan]->getFFT();
      grIcrrElecFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grIcrrElecAveragedFFT[chan]) {
	grIcrrElecAveragedFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grIcrrElecAveragedFFT[chan]->AddFFT(grIcrrElecFFT[chan]);
      }
      delete grTempFFT;      
    }


    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      if(chan%9==8) {
	//Clock channel
	for(int i=0;i<numPoints;i++) {	
	  if(yVals[i]<fMinClockVoltLimit)
	    fMinClockVoltLimit=yVals[i];
	  if(yVals[i]>fMaxClockVoltLimit)
	    fMaxClockVoltLimit=yVals[i];	
	}      
      }
      else{
	for(int i=0;i<numPoints;i++) {	
	  if(yVals[i]<fMinVoltLimit)
	    fMinVoltLimit=yVals[i];
	  if(yVals[i]>fMaxVoltLimit)
	    fMaxVoltLimit=yVals[i];	
	}      
      }
    }

    delete grTemp;
  }
  //  std::cout << "Limits\t" << fMinVoltLimit << "\t" << fMaxVoltLimit << "\n";


  for(int rfchan=0;rfchan<(evPtr->numRFChans);rfchan++) {
    if(grIcrrRFChan[rfchan]) delete grIcrrRFChan[rfchan];
    if(grIcrrRFChanFFT[rfchan]) delete grIcrrRFChanFFT[rfchan];
    if(grIcrrRFChanHilbert[rfchan]) delete grIcrrRFChanHilbert[rfchan];
    grIcrrRFChan[rfchan]=0;
    grIcrrRFChanFFT[rfchan]=0;
    grIcrrRFChanHilbert[rfchan]=0;
    //Need to work out how to do this
    TGraph *grTemp = evPtr->getGraphFromRFChan(rfchan);
    grIcrrRFChan[rfchan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());

    

    grIcrrRFChan[rfchan]->setRFChan(rfchan, evPtr->stationId);
      //      std::cout << evPtr->head.eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grIcrrElec[chan]->GetRMS(2) << std::endl;
    
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grIcrrRFChan[rfchan]->getFFT();
      grIcrrRFChanFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grIcrrRFChanAveragedFFT[rfchan]) {
	grIcrrRFChanAveragedFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grIcrrRFChanAveragedFFT[rfchan]->AddFFT(grIcrrRFChanFFT[rfchan]);
      }
      delete grTempFFT;      
    }
    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      for(int i=0;i<numPoints;i++) {	
	if(yVals[i]<fMinVoltLimit)
	  fMinVoltLimit=yVals[i];
	if(yVals[i]>fMaxVoltLimit)
	  fMaxVoltLimit=yVals[i];	
      }      
    }
    delete grTemp;
  }

  if(fAutoScale) {

    if(fCalType!=AraCalType::kNoCalib && fCalType!=AraCalType::kJustUnwrap) {
      if(fMaxVoltLimit>-1*fMinVoltLimit) {
	fMinVoltLimit=-1*fMaxVoltLimit;
      }
      else {
	fMaxVoltLimit=-1*fMinVoltLimit;
      }
      if(fMaxClockVoltLimit>-1*fMinClockVoltLimit) {
	fMinClockVoltLimit=-1*fMaxClockVoltLimit;
      }
      else {
	fMaxClockVoltLimit=-1*fMinClockVoltLimit;
      }
    }
  }
    
  //  std::cout << "Limits\t" << fMinVoltLimit << "\t" << fMaxVoltLimit << "\n";


  fRedoEventCanvas=0;
  fNewEvent=0;

  fRedoEventCanvas=0;
  if(fLastWaveformFormat!=fWaveformOption) fRedoEventCanvas=1;

  
  if(fCanvasLayout==AraDisplayCanvasLayoutOption::kElectronicsView) {
    retCan=AraIcrrCanvasMaker::getElectronicsCanvas(evPtr,useCan);
  }
  else if(fCanvasLayout==AraDisplayCanvasLayoutOption::kRFChanView) {
    retCan=AraIcrrCanvasMaker::getRFChannelCanvas(evPtr,useCan);
  }
  else if(fCanvasLayout==AraDisplayCanvasLayoutOption::kAntennaView) {
    retCan=AraIcrrCanvasMaker::getAntennaCanvas(evPtr,useCan);
  }
  else if(fCanvasLayout==AraDisplayCanvasLayoutOption::kIntMapView) {
    retCan=AraIcrrCanvasMaker::getIntMapCanvas(evPtr,useCan);
  }


  fLastWaveformFormat=fWaveformOption;
  fLastCanvasView=fCanvasLayout;

  return retCan;

}


TPad *AraIcrrCanvasMaker::getElectronicsCanvas(UsefulIcrrStationEvent *evPtr,TPad *useCan)
{
  //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 
  
  if(!fIcrrACMGeomTool)
    fIcrrACMGeomTool=AraGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canElec=0;
  TPad *plotPad=0;
  if(!useCan) {
    canElec = (TPad*) gROOT->FindObject("canElec");
    if(!canElec) {
      canElec = new TCanvas("canElec","canElec",1000,600);
    }
    canElec->Clear();
    canElec->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel," Event %d",evPtr->head.eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canElecMain","canElecMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupElecPadWithFrames(plotPad);



  int count=0;

  // A
  // 1 3 5 7 9 
  // 2 4 6 8 
  // B
  // 1 3 5 7 9 
  // 2 4 6 8 
  // C
  // 1 3 5 7 9 
  // 2 4 6 8 

  for(int row=0;row<6;row++) {    
    for(int column=0;column<5;column++) {
      plotPad->cd();
      int labChip=(row/2); //0, 1 or 2
      int channel=(row%2)+2*column;
      int chanIndex=channel+9*labChip;
      if(channel>8) continue;

      sprintf(padName,"elecChanPad%d",column+row*5);
      //      std::cout << chanIndex << "\t" << labChip << "\t" << channel << "\t" << padName << "\n";
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromElecPad(paddy1,chanIndex);



      if(fWaveformOption==AraDisplayFormatOption::kPowerSpectralDensity){
	if(!grIcrrElecFFT[chanIndex]) {
	  TGraph *grTemp=grIcrrElec[chanIndex]->getFFT();
	  grIcrrElecFFT[chanIndex]=new AraFFTGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	grIcrrElecFFT[chanIndex]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	grIcrrElecAveragedFFT[chanIndex]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(!grIcrrElecHilbert[chanIndex]) {
	  TGraph *grTemp=grIcrrElec[chanIndex]->getHilbert();
	  grIcrrElecHilbert[chanIndex]=new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	grIcrrElecHilbert[chanIndex]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform){

	grIcrrElec[chanIndex]->Draw("l");
	
	
	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      if(channel<8) {
		tempHist->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);
	      }
	      else {
		tempHist->GetYaxis()->SetRangeUser(fMinClockVoltLimit,fMaxClockVoltLimit);
	      }
	    }
	  }
	}
      }


      count++;
      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canElec;
  else
    return plotPad;
  

}


TPad *AraIcrrCanvasMaker::getCanvasForWebPlotter(UsefulIcrrStationEvent *evPtr,
					     TPad *useCan)
{
  //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fIcrrACMGeomTool)
    fIcrrACMGeomTool=AraGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canRFChan=0;
  TPad *plotPad=0;
  if(!useCan) {
    canRFChan = (TPad*) gROOT->FindObject("canRFChan");
    if(!canRFChan) {
      canRFChan = new TCanvas("canRFChan","canRFChan",1000,600);
    }
    canRFChan->Clear();
    canRFChan->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",evPtr->head.eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canRFChanMain","canRFChanMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  setupRFChanPadWithFrames(plotPad, evPtr->stationId);



  int count=0;


  //  1  2  3  4
  //  5  6  7  8
  //  9 10 11 12
  // 13 14 15 16

  for(int column=0;column<4;column++) {
    for(int row=0;row<4;row++) {
      plotPad->cd();
      int rfChan=column+4*row;

      sprintf(padName,"rfChanPad%d",column+4*row);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromRFPad(paddy1,rfChan);
      paddy1->cd();
     
      grIcrrRFChan[rfChan]->Draw("l");

      if(fAutoScale) {
	TList *listy = gPad->GetListOfPrimitives();
	for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);

	    }
	}
      }


      count++;
      paddy1->SetEditable(kFALSE);
    }
  }

  if(!useCan)
    return canRFChan;
  else
    return plotPad;
  

}

TPad *AraIcrrCanvasMaker::getRFChannelCanvas(UsefulIcrrStationEvent *evPtr,
					 TPad *useCan)
{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fIcrrACMGeomTool)
    fIcrrACMGeomTool=AraGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canRFChan=0;
  TPad *plotPad=0;
  if(!useCan) {
    canRFChan = (TPad*) gROOT->FindObject("canRFChan");
    if(!canRFChan) {
      canRFChan = new TCanvas("canRFChan","canRFChan",1000,600);
    }
    canRFChan->Clear();
    canRFChan->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",evPtr->head.eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canRFChanMain","canRFChanMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();

  //FIXME -- jpd - this is where we will set the fMaxTime and fMinTime

  TGraph *tempGraph=0;
  Double_t *tempX;
  
  for(int rfChan=0; rfChan<evPtr->getNumRFChannels();++rfChan){
    tempGraph=evPtr->getGraphFromRFChan(rfChan);
    int nEntries=tempGraph->GetN();
    tempX=tempGraph->GetX();
    if(tempX[0]<fMinTimeLimit) fMinTimeLimit=tempX[0];
    if(tempX[nEntries-1]>fMaxTimeLimit) fMaxTimeLimit=tempX[nEntries-1];
    //    fprintf(stderr, "getRFChannelCanvas() -- rfChan %i fMinTimeLimit %f tempX[0] %f fMaxTimeLimit %f tempX[N-1] %f\n", rfChan, fMinTimeLimit, tempX[0], fMaxTimeLimit, tempX[nEntries-1]);


    delete tempGraph;
  }

  //  fprintf(stderr, "\nfMinTimeLimit %f fMaxTimeLimit %f\n", fMinTimeLimit, fMaxTimeLimit);//DEBUG

  setupRFChanPadWithFrames(plotPad, evPtr->stationId);
  int maxColumns=0;
  int maxRows=0;

  if(evPtr->stationId==1){//FIXME --This should be something like if num antennas is 20 or something
    maxColumns=4;
    maxRows=5;
  }
  else {
    maxColumns=4;
    maxRows=4;
  }

  
  for(int column=0;column<maxColumns;column++) {
    for(int row=0;row<maxRows;row++) {
      plotPad->cd();
      int rfChan=column+4*row;
      
      sprintf(padName,"rfChanPad%d",column+4*row);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromRFPad(paddy1,rfChan);
      paddy1->cd();
      
      if(fWaveformOption==AraDisplayFormatOption::kPowerSpectralDensity){
	if(!grIcrrRFChanFFT[rfChan]) {
	  TGraph *grTemp=grIcrrRFChan[rfChan]->getFFT();
	  grIcrrRFChanFFT[rfChan]=new AraFFTGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	grIcrrRFChanFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	grIcrrRFChanAveragedFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(!grIcrrRFChanHilbert[rfChan])  {
	  TGraph *grTemp=grIcrrRFChan[rfChan]->getHilbert();
	  grIcrrRFChanHilbert[rfChan]=new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	grIcrrRFChanHilbert[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform) {
	grIcrrRFChan[rfChan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);
	    }
	  }
	}
      }

      paddy1->SetEditable(kFALSE);
    }
  }
  
  
  if(!useCan)
    return canRFChan;
  else 
    return plotPad;
  
}

//FIXME //jpd This needs to be tested and or fixed to comply with station1
TPad *AraIcrrCanvasMaker::getAntennaCanvas(UsefulIcrrStationEvent *evPtr,
				       TPad *useCan)
{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fIcrrACMGeomTool)
    fIcrrACMGeomTool=AraGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canRFChan=0;
  TPad *plotPad=0;
  if(!useCan) {
    canRFChan = (TPad*) gROOT->FindObject("canRFChan");
    if(!canRFChan) {
      canRFChan = new TCanvas("canRFChan","canRFChan",1000,600);
    }
    canRFChan->Clear();
    canRFChan->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",evPtr->head.eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canRFChanMain","canRFChanMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();

  //FIXME -- jpd - this is where we will set the fMaxTime and fMinTime

  TGraph *tempGraph=0;
  Double_t *tempX;
  
  for(int rfChan=0; rfChan<evPtr->getNumRFChannels();++rfChan){
    tempGraph=evPtr->getGraphFromRFChan(rfChan);
    int nEntries=tempGraph->GetN();
    tempX=tempGraph->GetX();
    if(tempX[0]<fMinTimeLimit) fMinTimeLimit=tempX[0];
    if(tempX[nEntries-1]>fMaxTimeLimit) fMaxTimeLimit=tempX[nEntries-1];
    //    fprintf(stderr, "getRFChannelCanvas() -- rfChan %i fMinTimeLimit %f tempX[0] %f fMaxTimeLimit %f tempX[N-1] %f\n", rfChan, fMinTimeLimit, tempX[0], fMaxTimeLimit, tempX[nEntries-1]);


    delete tempGraph;
  }

  //  fprintf(stderr, "\nfMinTimeLimit %f fMaxTimeLimit %f\n", fMinTimeLimit, fMaxTimeLimit);//DEBUG

  setupAntPadWithFrames(plotPad, evPtr->stationId);

  // TestBed
  // V V V V
  // V V S S
  // H H H H
  // H H H H
  // Station 1
  // V V V V
  // V V V V
  // H H H H
  // H H H H
  // S S S S


  AraAntPol::AraAntPol_t polMap[5][4]={{AraAntPol::kVertical}}; //Temp value
  int antPolNumMap[5][4]={{0}};
  
  if(evPtr->stationId==0){
    polMap[0][0]=AraAntPol::kVertical;
    polMap[0][1]=AraAntPol::kVertical;
    polMap[0][2]=AraAntPol::kVertical;
    polMap[0][3]=AraAntPol::kVertical;
    antPolNumMap[0][0]=0;
    antPolNumMap[0][1]=1;
    antPolNumMap[0][2]=2;
    antPolNumMap[0][3]=3;

    polMap[1][0]=AraAntPol::kVertical;
    polMap[1][1]=AraAntPol::kVertical;
    polMap[1][2]=AraAntPol::kSurface;
    polMap[1][3]=AraAntPol::kSurface;
    antPolNumMap[1][0]=4;
    antPolNumMap[1][1]=5;
    antPolNumMap[1][2]=0;
    antPolNumMap[1][3]=1;

    polMap[2][0]=AraAntPol::kHorizontal;
    polMap[2][1]=AraAntPol::kHorizontal;
    polMap[2][2]=AraAntPol::kHorizontal;
    polMap[2][3]=AraAntPol::kHorizontal;
    antPolNumMap[2][0]=0;
    antPolNumMap[2][1]=1;
    antPolNumMap[2][2]=2;
    antPolNumMap[2][3]=3;

    polMap[3][0]=AraAntPol::kHorizontal;
    polMap[3][1]=AraAntPol::kHorizontal;
    polMap[3][2]=AraAntPol::kHorizontal;
    polMap[3][3]=AraAntPol::kHorizontal;
    antPolNumMap[3][0]=4;
    antPolNumMap[3][1]=5;
    antPolNumMap[3][2]=6;
    antPolNumMap[3][3]=7;

  }

  if(evPtr->stationId==1){
 
    polMap[0][0]=AraAntPol::kVertical;
    polMap[0][1]=AraAntPol::kVertical;
    polMap[0][2]=AraAntPol::kVertical;
    polMap[0][3]=AraAntPol::kVertical;
    antPolNumMap[0][0]=0;
    antPolNumMap[0][1]=1;
    antPolNumMap[0][2]=2;
    antPolNumMap[0][3]=3;

    polMap[1][0]=AraAntPol::kVertical;
    polMap[1][1]=AraAntPol::kVertical;
    polMap[1][2]=AraAntPol::kVertical;
    polMap[1][3]=AraAntPol::kVertical;
    antPolNumMap[1][0]=4;
    antPolNumMap[1][1]=5;
    antPolNumMap[1][2]=6;
    antPolNumMap[1][3]=7;

    polMap[2][0]=AraAntPol::kHorizontal;
    polMap[2][1]=AraAntPol::kHorizontal;
    polMap[2][2]=AraAntPol::kHorizontal;
    polMap[2][3]=AraAntPol::kHorizontal;
    antPolNumMap[2][0]=0;
    antPolNumMap[2][1]=1;
    antPolNumMap[2][2]=2;
    antPolNumMap[2][3]=3;

    polMap[3][0]=AraAntPol::kHorizontal;
    polMap[3][1]=AraAntPol::kHorizontal;
    polMap[3][2]=AraAntPol::kHorizontal;
    polMap[3][3]=AraAntPol::kHorizontal;
    antPolNumMap[3][0]=4;
    antPolNumMap[3][1]=5;
    antPolNumMap[3][2]=6;
    antPolNumMap[3][3]=7;

    polMap[4][0]=AraAntPol::kSurface;
    polMap[4][1]=AraAntPol::kSurface;
    polMap[4][2]=AraAntPol::kSurface;
    polMap[4][3]=AraAntPol::kSurface;
    antPolNumMap[4][0]=0;
    antPolNumMap[4][1]=1;
    antPolNumMap[4][2]=2;
    antPolNumMap[4][3]=3;

  }

  int maxColumns=0;
  int maxRows=0;

  if(evPtr->stationId==1){//FIXME --This should be something like if num antennas is 20 or something
    maxColumns=4;
    maxRows=5;
  }
  else {
    maxColumns=4;
    maxRows=4;
  }

  

  //FIXME //polandant
  
  for(int row=0;row<maxRows;row++) {
    for(int column=0;column<maxColumns;column++) {
      plotPad->cd();
      int rfChan=fIcrrACMGeomTool->getRFChanByPolAndAnt(polMap[row][column],antPolNumMap[row][column], evPtr->stationId);
      //      std::cout << row << "\t" << column << "\t" << rfChan << "\n";
      
      sprintf(padName,"antPad%d", column + 4*row );
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromRFPad(paddy1,rfChan);
      paddy1->cd();
      
      if(fWaveformOption==AraDisplayFormatOption::kPowerSpectralDensity){
	if(!grIcrrRFChanFFT[rfChan]) {	  
	  TGraph *grTemp=grIcrrRFChan[rfChan]->getFFT();
	  grIcrrRFChanFFT[rfChan]=new AraFFTGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	grIcrrRFChanFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	grIcrrRFChanAveragedFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(!grIcrrRFChanHilbert[rfChan]) {	  
	  TGraph *grTemp=grIcrrRFChan[rfChan]->getHilbert();
	  grIcrrRFChanHilbert[rfChan]=new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	grIcrrRFChanHilbert[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform) {
	grIcrrRFChan[rfChan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimit,fMaxVoltLimit);
	    }
	  }
	}
      }

      paddy1->SetEditable(kFALSE);
    }
  }
  
  
  if(!useCan)
    return canRFChan;
  else 
    return plotPad;
  
}


TPad *AraIcrrCanvasMaker::getIntMapCanvas(UsefulIcrrStationEvent *evPtr,
				       TPad *useCan)
{
  static UInt_t lastEventNumber=0;
  static UInt_t lastUnixTime=0;
  static UInt_t lastUnixTimeUs=0;
  Int_t sameEvent=0;
  if(lastEventNumber==evPtr->head.eventNumber) {
    if(lastUnixTime==evPtr->head.unixTime) {
      if(lastUnixTimeUs==evPtr->head.unixTimeUs) {
	sameEvent=1;
      }
    }
  }
  lastEventNumber=evPtr->head.eventNumber;
  lastUnixTime=evPtr->head.unixTime;
  lastUnixTimeUs=evPtr->head.unixTimeUs;
  

   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fIcrrACMGeomTool)
    fIcrrACMGeomTool=AraGeomTool::Instance();
  char textLabel[180];
  char padName[180];
  TPad *canIntMap=0;
  TPad *plotPad=0;
  if(!useCan) {
    canIntMap = (TPad*) gROOT->FindObject("canIntMap");
    if(!canIntMap) {
      canIntMap = new TCanvas("canIntMap","canIntMap",1000,600);
    }
    canIntMap->Clear();
    canIntMap->SetTopMargin(0);
    TPaveText *leftPave = new TPaveText(0.05,0.92,0.95,0.98);
    leftPave->SetBorderSize(0);
    sprintf(textLabel,"Event %d",evPtr->head.eventNumber);
    TText *eventText = leftPave->AddText(textLabel);
    eventText->SetTextColor(50);
    leftPave->Draw();
    plotPad = new TPad("canIntMapMain","canIntMapMain",0,0,1,0.9);
    plotPad->Draw();
  }
  else {
    plotPad=useCan;
  }
  plotPad->cd();
  plotPad->Clear();
  AraEventCorrelator *araCorPtr = AraEventCorrelator::Instance(fNumAntsInMap, evPtr->stationId);
  static TH2D* histMapH=0;  
  static TH2D* histMapV=0;  
  plotPad->Divide(1,2);
  plotPad->cd(1);
  
  if(histMapV) {
    if(!sameEvent) {
      delete histMapV;
      histMapV=0;
    }
  }

  
  if(!histMapV)
    histMapV =araCorPtr->getInterferometricMap(evPtr,AraAntPol::kVertical,fCorType);
  histMapV->SetName("histMapV");
  histMapV->SetTitle("Vertical Polarisation");
  histMapV->SetXTitle("Azimuth (Degrees)");
  histMapV->SetYTitle("Elevation (Degrees)");
  histMapV->SetStats(0);
  //  histMapV->SetMaximum(1);
  //  histMapV->SetMinimum(-1);
  histMapV->Draw("colz");

  plotPad->cd(2);
  if(histMapH) {
    if(!sameEvent) {
      delete histMapH;
      histMapH=0;
    }
  }
  if(!histMapH)
    histMapH =araCorPtr->getInterferometricMap(evPtr,AraAntPol::kHorizontal,fCorType);
  histMapH->SetName("histMapH");
  histMapH->SetTitle("Hertical Polarisation");
  histMapH->SetXTitle("Azimuth (Degrees)");
  histMapH->SetYTitle("Elevation (Degrees)");
  histMapH->SetStats(0);
  //  histMapH->SetMaximum(1);
  //  histMapH->SetMinimum(-1);
  histMapH->Draw("colz");
  
  


  
  if(!useCan)
    return canIntMap;
  else 
    return plotPad;
  
}







void AraIcrrCanvasMaker::setupElecPadWithFrames(TPad *plotPad)
{
  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(fLastCanvasView!=AraDisplayCanvasLayoutOption::kElectronicsView) {
    plotPad->Clear();
  } 
  if(fRedoEventCanvas){
    plotPad->Clear();
  }

  fLastCanvasView=AraDisplayCanvasLayoutOption::kElectronicsView; 

  static int elecPadsDone=0;
  if(elecPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int i=0;i<30;i++) {
      sprintf(padName,"elecChanPad%d",i);
      TPad *paddy = (TPad*) plotPad->FindObject(padName);
      if(!paddy)
	errors++;
    }
    if(!errors)
      return;
  }

  elecPadsDone=1;
    

  Double_t left[5]={0.04,0.23,0.42,0.61,0.80};
  Double_t right[5]={0.23,0.42,0.61,0.80,0.99};
  Double_t top[6]={0.93,0.78,0.63,0.48,0.33,0.18};
  Double_t bottom[6]={0.78,0.63,0.48,0.33,0.18,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<5;column++) {
    sprintf(textLabel,"Chan %d/%d",1+(2*column),2+(2*column));
    if(column==4)
      texy.DrawTextNDC(right[column]-0.1,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.09,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  texy.DrawTextNDC(left[0]-0.01,bottom[0],"A");
  texy.DrawTextNDC(left[0]-0.01,bottom[2],"B");
  texy.DrawTextNDC(left[0]-0.01,bottom[4],"C");

  // A
  // 1 3 5 7 9 
  // 2 4 6 8 
  // B
  // 1 3 5 7 9 
  // 2 4 6 8 
  // C
  // 1 3 5 7 9 
  // 2 4 6 8 

  int count=0;




  for(int row=0;row<6;row++) {
    for(int column=0;column<5;column++) {
      plotPad->cd();
      //      if(row%2==1 && column==4) continue;
      sprintf(padName,"elecChanPad%d",column+5*row);
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==4)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==5)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();

      TH1F *framey=0;
      

      //      std::cout << "Limits\t" << fMinVoltLimit << "\t" << fMaxVoltLimit << "\n";
  

      if(fWaveformOption==AraDisplayFormatOption::kWaveform || fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) 
	framey = (TH1F*) paddy1->DrawFrame(0,fMinVoltLimit,250,fMaxVoltLimit);
	//	framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimit,fMaxTimeLimit,fMaxVoltLimit);
      else if(fWaveformOption==AraDisplayFormatOption::kFFT || fWaveformOption==AraDisplayFormatOption::kAveragedFFT)
	framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit); 

      framey->GetYaxis()->SetLabelSize(0.08);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
	 
      if(row==4) {
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }
      if(fWebPlotterMode && column!=0) {
	 framey->GetYaxis()->SetLabelSize(0);
	 framey->GetYaxis()->SetTitleSize(0);
	 framey->GetYaxis()->SetTitleOffset(0);
      }
      count++;
    }
  }

}



void AraIcrrCanvasMaker::setupRFChanPadWithFrames(TPad *plotPad, Int_t stationId)
{
  int maxColumns=0;
  int maxRows=0;
  int numRFChans=0;
  Double_t left[4]={0.04,0.27,0.50,0.73};
  Double_t right[4]={0.27,0.50,0.73,0.96};
  Double_t top[5]={0};
  Double_t bottom[5]={0};
  if(stationId==0){ //FIXME -- stationID
    maxColumns=4;
    maxRows=4;
    numRFChans=maxRows*maxColumns;

    top[0]=0.95;
    top[1]=0.72;
    top[2]=0.49;
    top[3]=0.26;
    bottom[0]=0.72;
    bottom[1]=0.49;
    bottom[2]=0.26;
    bottom[3]=0.03;
  }

  if(stationId==1){//FIXME -- Station ID
    maxColumns=4;
    maxRows=5;
    numRFChans=maxRows*maxColumns;

    top[0]=0.95;
    top[1]=0.77;
    top[2]=0.59;
    top[3]=0.41;
    top[4]=0.23;
    bottom[0]=0.77;
    bottom[1]=0.59;
    bottom[2]=0.41;
    bottom[3]=0.23;
    bottom[4]=0.05;
  }

  static int rfChanPadsDone=0;
  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(fLastCanvasView!=AraDisplayCanvasLayoutOption::kRFChanView) {
    plotPad->Clear();
  }

  if(fRedoEventCanvas && rfChanPadsDone){
    plotPad->Clear();
  }

  fLastCanvasView=AraDisplayCanvasLayoutOption::kRFChanView;

  if(rfChanPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int rfChan=0;rfChan<numRFChans;rfChan++) {
	sprintf(padName,"rfChanPad%d",rfChan);
	TPad *paddy = (TPad*) plotPad->FindObject(padName);
	if(!paddy)
	  errors++;
    }
    if(!errors)
      return;
  }
  
  
  rfChanPadsDone=1;
  
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<maxColumns;column++) {
    sprintf(textLabel,"%d/%d",1+column,5+column);
    if(column==3)
      texy.DrawTextNDC(right[column]-0.12,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.12,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  texy.DrawTextNDC(left[0]-0.01,bottom[0]+0.1,"1-4");
  texy.DrawTextNDC(left[0]-0.01,bottom[1]+0.1,"5-8");
  texy.DrawTextNDC(left[0]-0.01,bottom[2]+0.1,"9-12");
  texy.DrawTextNDC(left[0]-0.01,bottom[3]+0.1,"13-16");
  if(stationId==1) texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.1,"17-20");//FIXME -- station ID
  

 
  int count=0;

  //  1  2  3  4
  //  5  6  7  8
  //  9 10 11 12
  // 13 14 15 16

  for(int column=0;column<maxColumns;column++) {
    for(int row=0;row<maxRows;row++) {
      plotPad->cd();
      //      int rfChan=column+4*row;
      sprintf(padName,"rfChanPad%d",column+4*row);
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==3)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==3&&stationId==0) //FIXME -- stationId
	paddy1->SetBottomMargin(0.1);
      if(row==4&&stationId==1)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();
      TH1F *framey=0;
      if(fWaveformOption==AraDisplayFormatOption::kFFT || fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform || 
	      fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(row<3) { //FIXME -- maybe stationId case?
	  framey = (TH1F*) paddy1->DrawFrame((10*(int)fMinTimeLimit)/10 - 40, fMinVoltLimit, (10*(int)fMaxTimeLimit)/10 + 40,fMaxVoltLimit);
	}
	else{
	  framey = (TH1F*) paddy1->DrawFrame((10*(int)fMinTimeLimit)/10 - 40,fMinClockVoltLimit,(10*(int)fMaxTimeLimit)/10 + 40,fMaxClockVoltLimit);
	}
      }

      framey->GetYaxis()->SetLabelSize(0.1);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
      if(row==4&&stationId==1) { //FIXME -- station ID
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }
      if(row==3&&stationId==0) { //FIXME -- station ID
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }

      if(fWebPlotterMode && column!=0) {
	 framey->GetYaxis()->SetLabelSize(0);
	 framey->GetYaxis()->SetTitleSize(0);
	 framey->GetYaxis()->SetTitleOffset(0);
      }

      count++;
    }
  }
}



void AraIcrrCanvasMaker::setupAntPadWithFrames(TPad *plotPad, Int_t stationId)
{

  //First we must set up the pad sizes according to the station ID
  int maxColumns=0;
  int maxRows=0;
  int numRFChans=0;
  Double_t left[4]={0.04,0.27,0.50,0.73};
  Double_t right[4]={0.27,0.50,0.73,0.96};
  Double_t top[5]={0};
  Double_t bottom[5]={0};
  if(stationId==0){ //FIXME -- stationID
    maxColumns=4;
    maxRows=4;
    numRFChans=maxRows*maxColumns;

    top[0]=0.95;
    top[1]=0.72;
    top[2]=0.49;
    top[3]=0.26;
    bottom[0]=0.72;
    bottom[1]=0.49;
    bottom[2]=0.26;
    bottom[3]=0.03;
  }

  if(stationId==1){//FIXME -- Station ID
    maxColumns=4;
    maxRows=5;
    numRFChans=maxRows*maxColumns;

    top[0]=0.95;
    top[1]=0.77;
    top[2]=0.59;
    top[3]=0.41;
    top[4]=0.23;
    bottom[0]=0.77;
    bottom[1]=0.59;
    bottom[2]=0.41;
    bottom[3]=0.23;
    bottom[4]=0.05;
  }



  static int antPadsDone=0;
  char textLabel[180];
  char padName[180];
  plotPad->cd();
  if(fLastCanvasView!=AraDisplayCanvasLayoutOption::kAntennaView) {
    plotPad->Clear();
  }

  if(fRedoEventCanvas && antPadsDone){
    plotPad->Clear();
  }

  fLastCanvasView=AraDisplayCanvasLayoutOption::kAntennaView;

  if(antPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int rfChan=0; rfChan<numRFChans;rfChan++){
      sprintf(padName,"antPad%d", rfChan);
      TPad *paddy = (TPad*) plotPad->FindObject(padName);
      if(!paddy)
	errors++;
    }
    if(!errors)
      return;
  }
  
  
  antPadsDone=1;
  
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<maxColumns;column++) {
    sprintf(textLabel,"%d/%d",1+column,5+column);
    if(column==3)
      texy.DrawTextNDC(right[column]-0.12,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.12,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);

  if(stationId==0){
    texy.DrawTextNDC(left[0]-0.01,bottom[0]+0.1,"V");
    texy.DrawTextNDC(left[0]-0.01,bottom[1]+0.1,"V/S");
    texy.DrawTextNDC(left[0]-0.01,bottom[2]+0.1,"H");
    texy.DrawTextNDC(left[0]-0.01,bottom[3]+0.1,"H");
  }
  if(stationId==1){
    texy.DrawTextNDC(left[0]-0.01,bottom[0]+0.1,"V");
    texy.DrawTextNDC(left[0]-0.01,bottom[1]+0.1,"V");
    texy.DrawTextNDC(left[0]-0.01,bottom[2]+0.1,"H");
    texy.DrawTextNDC(left[0]-0.01,bottom[3]+0.1,"H");
    texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.1,"S");
  }
  //TestBed
  // V V V V
  // V V S S
  // H H H H
  // H H H H
  //Station1
  // V V V V
  // V V V V
  // H H H H
  // H H H H
  // S S S S 
 
  int count=0;
  
  for(int column=0;column<maxColumns;column++) {
    for(int row=0;row<maxRows;row++) {
      plotPad->cd();
      sprintf(padName,"antPad%d", column+4*row);
    
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==3)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==3&&stationId==0) //FIXME stationId
	paddy1->SetBottomMargin(0.1);
      if(row==4&&stationId==1)
	paddy1->SetBottomMargin(0.1);

      paddy1->Draw();
      paddy1->cd();
      TH1F *framey=0;
      if(fWaveformOption==AraDisplayFormatOption::kFFT || fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform || 
	      fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if((row<3&&stationId==0)||(row<4&&stationId==1)) { //FIXME
	  framey = (TH1F*) paddy1->DrawFrame((10*(int)fMinTimeLimit)/10 - 40,fMinVoltLimit,(10*(int)fMaxTimeLimit)/10 + 40,fMaxVoltLimit);
	}
	else{
	  framey = (TH1F*) paddy1->DrawFrame((10*(int)fMinTimeLimit)/10 - 40,fMinClockVoltLimit,(10*(int)fMaxTimeLimit)/10 + 40,fMaxClockVoltLimit);
	}
      }

      framey->GetYaxis()->SetLabelSize(0.1);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
      if(row==3&&stationId==0) {
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }

      if(row==4&&stationId==1) {
	framey->GetXaxis()->SetLabelSize(0.09);
	framey->GetXaxis()->SetTitleSize(0.09);
	framey->GetYaxis()->SetLabelSize(0.09);
	framey->GetYaxis()->SetTitleSize(0.09);
      }
      if(fWebPlotterMode && column!=0) {
	 framey->GetYaxis()->SetLabelSize(0);
	 framey->GetYaxis()->SetTitleSize(0);
	 framey->GetYaxis()->SetTitleOffset(0);
      }
      count++;
    }
  }
}



void AraIcrrCanvasMaker::deleteTGraphsFromElecPad(TPad *paddy,int chan)
{
  paddy->cd();
  if(fLastWaveformFormat==AraDisplayFormatOption::kWaveform) paddy->GetListOfPrimitives()->Remove(grIcrrElec[chan]);
  else if(fLastWaveformFormat==AraDisplayFormatOption::kFFT) paddy->GetListOfPrimitives()->Remove(grIcrrElecFFT[chan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kAveragedFFT) paddy->GetListOfPrimitives()->Remove(grIcrrElecAveragedFFT[chan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kHilbertEnvelope) paddy->GetListOfPrimitives()->Remove(grIcrrElecHilbert[chan]);  
  //  paddy->Update();
}


void AraIcrrCanvasMaker::deleteTGraphsFromRFPad(TPad *paddy,int rfchan)
{
  paddy->cd();
  if(fLastWaveformFormat==AraDisplayFormatOption::kWaveform) paddy->GetListOfPrimitives()->Remove(grIcrrRFChan[rfchan]);
  else if(fLastWaveformFormat==AraDisplayFormatOption::kFFT) paddy->GetListOfPrimitives()->Remove(grIcrrRFChanFFT[rfchan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kAveragedFFT) paddy->GetListOfPrimitives()->Remove(grIcrrRFChanAveragedFFT[rfchan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kHilbertEnvelope) paddy->GetListOfPrimitives()->Remove(grIcrrRFChanHilbert[rfchan]);  
  //  paddy->Update();
}


void AraIcrrCanvasMaker::resetAverage() 
{
  for(int chan=0;chan<NUM_DIGITIZED_ICRR_CHANNELS;chan++) {
    if(grIcrrElecAveragedFFT[chan]) {
      delete grIcrrElecAveragedFFT[chan];
      grIcrrElecAveragedFFT[chan]=0;
    }
  }
  for(int rfchan=0;rfchan<RFCHANS_PER_ICRR;rfchan++) {
    if(grIcrrRFChanAveragedFFT[rfchan]) {
      delete grIcrrRFChanAveragedFFT[rfchan];
      grIcrrRFChanAveragedFFT[rfchan]=0;
    }
  }
}



