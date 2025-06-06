//////////////////////////////////////////////////////////////////////////////
/////  AraCanvasMaker.cxx        ARA Event Canvas make               /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ARA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "AraAtriCanvasMaker.h"
#include "AraGeomTool.h"
#include "UsefulAtriStationEvent.h"
#include "AraWaveformGraph.h"
#include "AraEventCorrelator.h"
#include "AraFFTGraph.h"
#include "araSoft.h"


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


#define VOLT_PADDING 20

AraAtriCanvasMaker*  AraAtriCanvasMaker::fgInstance = 0;
AraGeomTool *fACMGeomTool=0;





AraWaveformGraph *grElec[CHANNELS_PER_ATRI]={0}; 
AraWaveformGraph *grElecFiltered[CHANNELS_PER_ATRI]={0};
AraWaveformGraph *grElecHilbert[CHANNELS_PER_ATRI]={0};
AraFFTGraph *grElecFFT[CHANNELS_PER_ATRI]={0};
AraFFTGraph *grElecAveragedFFT[CHANNELS_PER_ATRI]={0};

AraWaveformGraph *grRFChan[CHANNELS_PER_ATRI]={0};
AraWaveformGraph *grRFChanFiltered[CHANNELS_PER_ATRI]={0};
AraWaveformGraph *grRFChanHilbert[CHANNELS_PER_ATRI]={0};
AraFFTGraph *grRFChanFFT[CHANNELS_PER_ATRI]={0};
AraFFTGraph *grRFChanAveragedFFT[CHANNELS_PER_ATRI]={0};

//Some globals for the antenna view


AraAntPol::AraAntPol_t polMap[ANT_ROWS][ANT_COLS]={{AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical},
						   {AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical},
						   {AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal},
						   {AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal},
						   {AraAntPol::kSurface,AraAntPol::kSurface,AraAntPol::kSurface,AraAntPol::kSurface}};
int antPolNumMap[ANT_ROWS][ANT_COLS]={{0,1,2,3},{4,5,6,7},{0,1,2,3},{4,5,6,7},{0,1,2,3}};
int rfChanMap[ANT_ROWS][ANT_COLS]={{0}};
int rfChanToAntRow[ANTS_PER_ATRI]={0};  ///< Which row is the antenna in?

void fillRfChanMap(AraStationId_t stationId) {
  for(int row=0;row<ANT_ROWS;row++) {
    for(int column=0;column<ANT_COLS;column++) {
      int rfChan=fACMGeomTool->getRFChanByPolAndAnt(polMap[row][column],antPolNumMap[row][column], stationId);
      rfChanMap[row][column]=rfChan;
      rfChanToAntRow[rfChan]=row;
    }
  }

}


TH1D *AraAtriCanvasMaker::getFFTHisto(int ant)
{
  if(ant<0 || ant>=CHANNELS_PER_ATRI) return NULL;
  if(grRFChan[ant])
    return grRFChan[ant]->getFFTHisto();
    return NULL;

}

AraAtriCanvasMaker::AraAtriCanvasMaker(AraCalType::AraCalType_t calType)
{
  //Default constructor
  fACMGeomTool=AraGeomTool::Instance();
  fLastStationId=ARA_STATION1B;
  fillRfChanMap(fLastStationId);
  fNumAntsInMap=7;//FIXME
  fWebPlotterMode=0;
  fPassBandFilter=0;
  fNotchFilter=0;
  fLowPassEdge=200;
  fHighPassEdge=1200;
  fLowNotchEdge=235;
  fHighNotchEdge=500;
  for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
    fMinVoltLimitElec[chan]=-200;
    fMaxVoltLimitElec[chan]=200;
  }
  for(int row=0;row<RF_ROWS;row++) {
    fMinVoltLimitRf[row]=-200;
    fMaxVoltLimitRf[row]=+200;
    fMinVoltLimitAnt[row]=-200;
    fMaxVoltLimitAnt[row]=+200;
  }
  fPhiMax=0;
  fMinClockVoltLimit=-200;
  fMaxClockVoltLimit=200;
  fAutoScale=1;
  fMinTimeLimit=0;
  fMaxTimeLimit=420;
  fThisMinTime=0;
  fThisMaxTime=100;
  if(AraCalType::hasCableDelays(calType)) {
    fMinTimeLimit=-100;
    fMaxTimeLimit=500;
  }
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
  memset(grElec,0,sizeof(AraWaveformGraph*)*CHANNELS_PER_ATRI);
  memset(grElecFiltered,0,sizeof(AraWaveformGraph*)*CHANNELS_PER_ATRI);
  memset(grElecHilbert,0,sizeof(AraWaveformGraph*)*CHANNELS_PER_ATRI);
  memset(grElecFFT,0,sizeof(AraFFTGraph*)*CHANNELS_PER_ATRI);
  memset(grElecAveragedFFT,0,sizeof(AraFFTGraph*)*CHANNELS_PER_ATRI);

  memset(grRFChan,0,sizeof(AraWaveformGraph*)*ANTS_PER_ATRI);
  memset(grRFChanFiltered,0,sizeof(AraWaveformGraph*)*ANTS_PER_ATRI);
  memset(grRFChanHilbert,0,sizeof(AraWaveformGraph*)*ANTS_PER_ATRI);
  memset(grRFChanFFT,0,sizeof(AraFFTGraph*)*ANTS_PER_ATRI);
  memset(grRFChanAveragedFFT,0,sizeof(AraFFTGraph*)*ANTS_PER_ATRI);  
  switch(fCalType) {
  case AraCalType::kNoCalib:
    for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
      fMaxVoltLimitElec[chan]=3000;
      fMinVoltLimitElec[chan]=1000;
    }

  for(int row=0;row<RF_ROWS;row++) {
    fMinVoltLimitRf[row]=1000;
    fMaxVoltLimitRf[row]=3000;
    fMinVoltLimitAnt[row]=1000;
    fMaxVoltLimitAnt[row]=3000;
  }
    break;
  case AraCalType::kJustUnwrap:
    fMinTimeLimit=0;
    fMaxTimeLimit=50;
    break;
  default:
    break;
  }


}

AraAtriCanvasMaker::~AraAtriCanvasMaker()
{
   //Default destructor
}



//______________________________________________________________________________
AraAtriCanvasMaker*  AraAtriCanvasMaker::Instance()
{
   //static function
   return (fgInstance) ? (AraAtriCanvasMaker*) fgInstance : new AraAtriCanvasMaker();
}


TPad *AraAtriCanvasMaker::getEventInfoCanvas(UsefulAtriStationEvent *evPtr,  TPad *useCan, Int_t runNumber)
{
   static UInt_t lastEventNumber=0;
   static TPaveText *leftPave=0;
   static TPaveText *midLeftPave=0;
   static TPaveText *midRightPave=0;
   static TPaveText *rightPave=0;


   if(!fACMGeomTool)
      fACMGeomTool=AraGeomTool::Instance();
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
     //     if(runNumber) {
     sprintf(textLabel,"%s",AraGeomTool::getStationName(evPtr->stationId));
     TText *stationIdText = leftPave->AddText(textLabel);
     stationIdText->SetTextColor(50);
     sprintf(textLabel,"Run: %d",runNumber);
     TText *runText = leftPave->AddText(textLabel);
     runText->SetTextColor(50);
     //    }
     sprintf(textLabel,"Event: %d",evPtr->eventNumber);
     TText *eventText = leftPave->AddText(textLabel);
     eventText->SetTextColor(50);
     sprintf(textLabel,"Event Id: %d",evPtr->eventId);
     TText *eventText2 = leftPave->AddText(textLabel);
     eventText2->SetTextColor(50);
     leftPave->Draw();


     topPad->cd(2);
     gPad->SetRightMargin(0);
     gPad->SetLeftMargin(0);
     if(midLeftPave) delete midLeftPave;
     midLeftPave = new TPaveText(0,0.1,0.99,0.9);
     midLeftPave->SetName("midLeftPave");
     midLeftPave->SetBorderSize(0);
     midLeftPave->SetTextAlign(13);
     TTimeStamp trigTime((time_t)evPtr->unixTime,(Int_t)1000*evPtr->unixTimeUs);
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
	     evPtr->ppsNumber);
     midRightPave->AddText(textLabel);
     sprintf(textLabel,"Trig Time %d ns",
	     evPtr->timeStamp*10);
     midRightPave->AddText(textLabel);
     // sprintf(textLabel,"Trig Type %d%d%d",
     // 	     evPtr->isInTrigType(2),
     // 	     evPtr->isInTrigType(1),
     // 	     evPtr->isInTrigType(0));
     // midRightPave->AddText(textLabel);
     // sprintf(textLabel,"Pattern %d%d%d%d%d%d%d%d",
     // 	     evPtr->isInTrigPattern(7),
     // 	     evPtr->isInTrigPattern(6),
     // 	     evPtr->isInTrigPattern(5),
     // 	     evPtr->isInTrigPattern(4),
     // 	     evPtr->isInTrigPattern(3),
     // 	     evPtr->isInTrigPattern(2),
     // 	     evPtr->isInTrigPattern(1),
     // 	     evPtr->isInTrigPattern(0));
     midRightPave->Draw();

     
     topPad->cd(4);
     if(rightPave) delete rightPave;
     rightPave = new TPaveText(0,0.1,1,0.95);
     rightPave->SetBorderSize(0);
     rightPave->SetTextAlign(13);
     sprintf(textLabel,"Num readout blocks %d",
	     evPtr->numReadoutBlocks);
     rightPave->AddText(textLabel); 
     sprintf(textLabel,"Blocks");
     rightPave->AddText(textLabel); 
     sprintf(textLabel,"");
     int count=0;
     for(int i=0;i<evPtr->numReadoutBlocks;i++) {
       if(evPtr->blockVec[i].getDda()==0) {
	 count++;
	 if(count%8==0) {
	   rightPave->AddText(textLabel);
	   sprintf(textLabel,"%d",evPtr->blockVec[i].getBlock());
	 }
	 else sprintf(textLabel,"%s%d ",textLabel,evPtr->blockVec[i].getBlock());

       }	 
       
     }
     rightPave->AddText(textLabel); 

     rightPave->Draw();
     topPad->Update();
     topPad->Modified();
               
     lastEventNumber=evPtr->eventNumber;
   }
      
   return topPad;
}


TPad *AraAtriCanvasMaker::quickGetEventViewerCanvasForWebPlottter(UsefulAtriStationEvent *evPtr,  TPad *useCan)
{
  TPad *retCan=0;
  fWebPlotterMode=1;
  //  static Int_t lastEventView=0;
  int foundTimeRange = 0;

  if(fAutoScale) {
    for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
      fMinVoltLimitElec[chan]=1e9;
      fMaxVoltLimitElec[chan]=-1e9;
    }
    fMinClockVoltLimit=1e9;
    fMaxClockVoltLimit=-1e9;
  }



  for(int chan=0;chan<CHANNELS_PER_ATRI;chan++) {
    if(grElec[chan]) delete grElec[chan];
    if(grElecFFT[chan]) delete grElecFFT[chan];
    if(grElecHilbert[chan]) delete grElecHilbert[chan];
    grElec[chan]=0;
    grElecFFT[chan]=0;
    grElecHilbert[chan]=0;
    //    if(grElecAveragedFFT[chan]) delete grElecAveragedFFT[chan];
    
    TGraph *grTemp = evPtr->getGraphFromElecChan(chan);
    if (!foundTimeRange && grTemp->GetN()) {
      foundTimeRange = 1;
      fThisMinTime=grTemp->GetX()[0];
      fThisMaxTime=grTemp->GetX()[grTemp->GetN()-1];
    }

    // If GetN() returns 0, it's just an empty graph.
    // Draw will throw a bunch of stupid errors ("illegal number of points")
    // but other than that it'll be fine. Maybe we'll put a check somewhere
    // on Draw to shut up the errors.
    if (grTemp->GetN()) {
      if(grTemp->GetX()[0]<fThisMinTime) fThisMinTime=grTemp->GetX()[0];
      if(grTemp->GetX()[grTemp->GetN()-1]>fThisMaxTime) fThisMaxTime=grTemp->GetX()[grTemp->GetN()-1];
    }

    grElec[chan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
    grElec[chan]->setElecChan(chan);
      //      std::cout << evPtr->eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grElec[chan]->GetRMS(2) << std::endl;
    
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grElec[chan]->getFFT();
      grElecFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grElecAveragedFFT[chan]) {
	grElecAveragedFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grElecAveragedFFT[chan]->AddFFT(grElecFFT[chan]);
      }
      delete grTempFFT;      
    }


    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      for(int i=0;i<numPoints;i++) {	
	if(yVals[i]<fMinVoltLimitElec[chan%RFCHAN_PER_DDA])
	  fMinVoltLimitElec[chan%RFCHAN_PER_DDA]=yVals[i];
	if(yVals[i]>fMaxVoltLimitElec[chan%RFCHAN_PER_DDA])
	  fMaxVoltLimitElec[chan%RFCHAN_PER_DDA]=yVals[i];	
      }      
    }

    delete grTemp;
  }

  foundTimeRange = 0;
  for(int rfchan=0;rfchan<ANTS_PER_ATRI;rfchan++) {
    if(grRFChan[rfchan]) delete grRFChan[rfchan];
    if(grRFChanFFT[rfchan]) delete grRFChanFFT[rfchan];
    if(grRFChanHilbert[rfchan]) delete grRFChanHilbert[rfchan];
    grRFChan[rfchan]=0;
    grRFChanFFT[rfchan]=0;
    grRFChanHilbert[rfchan]=0;
    //    if(grRFChanAveragedFFT[chan]) delete grRFChanAveragedFFT[chan];
    //Need to work out how to do this
    TGraph *grTemp = evPtr->getGraphFromRFChan(rfchan);
    
    if (!foundTimeRange && grTemp->GetN()) {
      foundTimeRange = 1;
      fThisMinTime=grTemp->GetX()[0];
      fThisMaxTime=grTemp->GetX()[grTemp->GetN()-1];
    }

    grRFChan[rfchan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
    grRFChan[rfchan]->setRFChan(rfchan, evPtr->stationId);
      //      std::cout << evPtr->eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grElec[chan]->GetRMS(2) << std::endl;
    
 
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grRFChan[rfchan]->getFFT();
      grRFChanFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grRFChanAveragedFFT[rfchan]) {
	grRFChanAveragedFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grRFChanAveragedFFT[rfchan]->AddFFT(grRFChanFFT[rfchan]);
      }
      delete grTempFFT;      
    }

    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      for(int i=0;i<numPoints;i++) {	
	if(yVals[i]<fMinVoltLimitElec[rfchan%RFCHAN_PER_DDA])
	  fMinVoltLimitElec[rfchan%RFCHAN_PER_DDA]=yVals[i];
	if(yVals[i]>fMaxVoltLimitElec[rfchan%RFCHAN_PER_DDA])
	  fMaxVoltLimitElec[rfchan%RFCHAN_PER_DDA]=yVals[i];	
      }      
    }
    delete grTemp;
  }


  if(fAutoScale) {
    if(fCalType!=AraCalType::kNoCalib && fCalType!=AraCalType::kJustUnwrap) {
      for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
	if(fMaxVoltLimitElec[chan]>-1*fMinVoltLimitElec[chan]) {
	  fMinVoltLimitElec[chan]=-1*fMaxVoltLimitElec[chan];
	}
	else {
	  fMaxVoltLimitElec[chan]=-1*fMinVoltLimitElec[chan];
	}
      }//rfchan
    }
  

   if(fMaxClockVoltLimit>-1*fMinClockVoltLimit) {
      fMinClockVoltLimit=-1*fMaxClockVoltLimit;
    }
    else {
      fMaxClockVoltLimit=-1*fMinClockVoltLimit;
    }
  }
    

  fRedoEventCanvas=0;

  retCan=AraAtriCanvasMaker::getCanvasForWebPlotter(evPtr,useCan);

 

  return retCan;

}

TPad *AraAtriCanvasMaker::getEventViewerCanvas(UsefulAtriStationEvent *evPtr,
					       TPad *useCan)
{
  TPad *retCan=0;

  static UInt_t lastEventNumber=0;

  int foundTimeRange = 0;

  if(fAutoScale) {
    for(int chan=0;chan<RFCHAN_PER_DDA;chan++){
      fMinVoltLimitElec[chan]=1e9;
      fMaxVoltLimitElec[chan]=-1e9;
    }
    for(int row=0;row<RF_ROWS;row++) {
      fMinVoltLimitRf[row]=1e9;
      fMaxVoltLimitRf[row]=-1e9;
      fMinVoltLimitAnt[row]=1e9;
      fMaxVoltLimitAnt[row]=-1e9;
    }
    fMinClockVoltLimit=0;
    fMaxClockVoltLimit=0;
  }



  for(int chan=0;chan<CHANNELS_PER_ATRI;chan++) {
    if(grElec[chan]) delete grElec[chan];
    if(grElecFFT[chan]) delete grElecFFT[chan];
    if(grElecHilbert[chan]) delete grElecHilbert[chan];
    grElec[chan]=0;
    grElecFFT[chan]=0;
    grElecHilbert[chan]=0;
    
    TGraph *grTemp = evPtr->getGraphFromElecChan(chan);
    if (grTemp->GetN() && !foundTimeRange) {
      fThisMinTime = grTemp->GetX()[0];
      fThisMaxTime = grTemp->GetX()[grTemp->GetN()-1];
      foundTimeRange = 1;
    }
    grElec[chan] = new AraWaveformGraph(grTemp->GetN(), grTemp->GetX(),grTemp->GetY());
    grElec[chan]->setElecChan(chan);
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grElec[chan]->getFFT();
      grElecFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grElecAveragedFFT[chan]) {
	grElecAveragedFFT[chan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grElecAveragedFFT[chan]->AddFFT(grElecFFT[chan]);
      }
      delete grTempFFT;      
    }


    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      if(chan%8==7) {
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
	  if(yVals[i]<fMinVoltLimitElec[chan%RFCHAN_PER_DDA])
	    fMinVoltLimitElec[chan%RFCHAN_PER_DDA]=yVals[i];
	  if(yVals[i]>fMaxVoltLimitElec[chan%RFCHAN_PER_DDA])
	    fMaxVoltLimitElec[chan%RFCHAN_PER_DDA]=yVals[i];	
	}      
      }
    }

    delete grTemp;
  }
  //  std::cout << "Limits\t" << fMinVoltLimitElec << "\t" << fMaxVoltLimitElec << "\n";
  foundTimeRange = 0;
  
  for(int rfchan=0;rfchan<ANTS_PER_ATRI;rfchan++) {
    if(grRFChan[rfchan]) delete grRFChan[rfchan];
    if(grRFChanFFT[rfchan]) delete grRFChanFFT[rfchan];
    if(grRFChanHilbert[rfchan]) delete grRFChanHilbert[rfchan];
    grRFChan[rfchan]=0;
    grRFChanFFT[rfchan]=0;
    grRFChanHilbert[rfchan]=0;
    //Need to work out how to do this
    TGraph *grTemp = evPtr->getGraphFromRFChan(rfchan);
    //    std::cout << "getEventViewerCanvas(): " << rfchan << "\t" << grTemp << "\n";
    if (grTemp->GetN() && !foundTimeRange) {
      fThisMinTime = grTemp->GetX()[0];
      fThisMaxTime = grTemp->GetX()[grTemp->GetN()-1];
      foundTimeRange = 1;
    }
    
    if (grTemp->GetN()) {
      if(grTemp->GetX()[0]<fThisMinTime) fThisMinTime=grTemp->GetX()[0];
      if(grTemp->GetX()[grTemp->GetN()-1]>fThisMaxTime) fThisMaxTime=grTemp->GetX()[grTemp->GetN()-1];
    }
    grRFChan[rfchan] = new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
    grRFChan[rfchan]->setRFChan(rfchan, evPtr->stationId);

      //      std::cout << evPtr->eventNumber << "\n";
      //      std::cout << surf << "\t" << chan << "\t" 
      //		<< grElec[chan]->GetRMS(2) << std::endl;
    
    if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT) {
      TGraph *grTempFFT = grRFChan[rfchan]->getFFT();
      grRFChanFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      if(!grRFChanAveragedFFT[rfchan]) {
	grRFChanAveragedFFT[rfchan]=new AraFFTGraph(grTempFFT->GetN(),
				      grTempFFT->GetX(),
				      grTempFFT->GetY());
      }
      else {
	grRFChanAveragedFFT[rfchan]->AddFFT(grRFChanFFT[rfchan]);
      }
      delete grTempFFT;      
    }
    if(fAutoScale) {
      Int_t numPoints=grTemp->GetN();
      Double_t *yVals=grTemp->GetY();
      	
      for(int i=0;i<numPoints;i++) {	
	//RF Channel View
	if(yVals[i]<fMinVoltLimitRf[rfchan/RF_COLS])
	  fMinVoltLimitRf[rfchan/RF_COLS]=yVals[i];
	if(yVals[i]>fMaxVoltLimitRf[rfchan/RF_COLS])
	  fMaxVoltLimitRf[rfchan/RF_COLS]=yVals[i];	
	//Antenna View
	if(yVals[i]<fMinVoltLimitAnt[rfChanToAntRow[rfchan]])
	  fMinVoltLimitAnt[rfChanToAntRow[rfchan]]=yVals[i];
	if(yVals[i]>fMaxVoltLimitAnt[rfChanToAntRow[rfchan]])
	  fMaxVoltLimitAnt[rfChanToAntRow[rfchan]]=yVals[i];	
      }      
    }
    delete grTemp;
  }

  if(fAutoScale) {

    if(fCalType!=AraCalType::kNoCalib && fCalType!=AraCalType::kJustUnwrap) {
      for(int chan=0;chan<RFCHAN_PER_DDA;chan++){

	if(fMaxVoltLimitElec[chan]>-1*fMinVoltLimitElec[chan]) {
	  fMinVoltLimitElec[chan]=-1*fMaxVoltLimitElec[chan];
	}
	else {
	  fMaxVoltLimitElec[chan]=-1*fMinVoltLimitElec[chan];
	}
      }
      if(fMaxClockVoltLimit>-1*fMinClockVoltLimit) {
	fMinClockVoltLimit=-1*fMaxClockVoltLimit;
      }
      else {
	fMaxClockVoltLimit=-1*fMinClockVoltLimit;
      }
    }
  }
    
  //  std::cout << "Limits\t" << fMinVoltLimitElec << "\t" << fMaxVoltLimitElec << "\n";


  fRedoEventCanvas=0;
  fNewEvent=0;

  fRedoEventCanvas=0;
  if(fLastWaveformFormat!=fWaveformOption) fRedoEventCanvas=1;

  
  if(fCanvasLayout==AraDisplayCanvasLayoutOption::kElectronicsView) {
    retCan=AraAtriCanvasMaker::getElectronicsCanvas(evPtr,useCan);
  }
  else if(fCanvasLayout==AraDisplayCanvasLayoutOption::kRFChanView) {
    retCan=AraAtriCanvasMaker::getRFChannelCanvas(evPtr,useCan);
  }
  else if(fCanvasLayout==AraDisplayCanvasLayoutOption::kAntennaView) {
    retCan=AraAtriCanvasMaker::getAntennaCanvas(evPtr,useCan);
  }
  else if(fCanvasLayout==AraDisplayCanvasLayoutOption::kIntMapView) {
    retCan=AraAtriCanvasMaker::getIntMapCanvas(evPtr,useCan);
  }


  fLastWaveformFormat=fWaveformOption;
  fLastCanvasView=fCanvasLayout;

  return retCan;

}


TPad *AraAtriCanvasMaker::getElectronicsCanvas(UsefulAtriStationEvent *evPtr,TPad *useCan)
{
  //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 
  
  if(!fACMGeomTool)
    fACMGeomTool=AraGeomTool::Instance();
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
    sprintf(textLabel," Event %d",evPtr->eventNumber);
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



  for(int row=0;row<RFCHAN_PER_DDA;row++) {    
    for(int column=0;column<DDA_PER_ATRI;column++) {
      plotPad->cd();
      int channel=row;
      int dda=column;
      int chanIndex=row+column*RFCHAN_PER_DDA;
      //if(channel>8) continue;

      sprintf(padName,"elecChanPad%d",row+column*RFCHAN_PER_DDA);
      //      std::cout << chanIndex << "\t" << labChip << "\t" << channel << "\t" << padName << "\n";
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromElecPad(paddy1,chanIndex);



      if(fWaveformOption==AraDisplayFormatOption::kPowerSpectralDensity){
	if(!grElecFFT[chanIndex]) {
	  TGraph *grTemp=grElec[chanIndex]->getFFT();
	  grElecFFT[chanIndex]=new AraFFTGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	if(grElecFFT[chanIndex]->GetN())
	  grElecFFT[chanIndex]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	if(	grElecAveragedFFT[chanIndex]->GetN())
	  grElecAveragedFFT[chanIndex]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(!grElecHilbert[chanIndex]) {
	  TGraph *grTemp=grElec[chanIndex]->getHilbert();
	  grElecHilbert[chanIndex]=new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	if(grElecHilbert[chanIndex]->GetN())
	  grElecHilbert[chanIndex]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform){
	if(grElec[chanIndex]->GetN())
	  grElec[chanIndex]->Draw("l");
		
	TList *listy = gPad->GetListOfPrimitives();
	for(int i=0;i<listy->GetSize();i++) {
	  TObject *fred = listy->At(i);
	  TH1F *tempHist = (TH1F*) fred;
	  if(tempHist->InheritsFrom("TH1")) {
	    tempHist->GetXaxis()->SetRangeUser(fThisMinTime,fThisMaxTime);
	    //	    std::cout << fThisMinTime << "\t" << fThisMaxTime << "\n";
	    if(fAutoScale) {
	      if(channel<7) {
		tempHist->GetYaxis()->SetRangeUser(fMinVoltLimitElec[chanIndex%RFCHAN_PER_DDA]-VOLT_PADDING,fMaxVoltLimitElec[chanIndex%RFCHAN_PER_DDA]+VOLT_PADDING);
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


TPad *AraAtriCanvasMaker::getCanvasForWebPlotter(UsefulAtriStationEvent *evPtr,
					     TPad *useCan)
{
  //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AraGeomTool::Instance();
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
    sprintf(textLabel,"Event %d",evPtr->eventNumber);
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
  setupRFChanPadWithFrames(plotPad);



  int count=0;


  //  1  2  3  4
  //  5  6  7  8
  //  9 10 11 12
  // 13 14 15 16
  // 17 18 19 20

  for(int column=0;column<RF_COLS;column++) {
    for(int row=0;row<RF_ROWS;row++) {
      plotPad->cd();
      int rfChan=column+RF_COLS*row;

      sprintf(padName,"rfChanPad%d",column+4*row);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromRFPad(paddy1,rfChan);
      paddy1->cd();
      
      grRFChan[rfChan]->Draw("l");

      if(fAutoScale) {
	TList *listy = gPad->GetListOfPrimitives();
	for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimitRf[row]-VOLT_PADDING,fMaxVoltLimitRf[row]+VOLT_PADDING);

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

TPad *AraAtriCanvasMaker::getRFChannelCanvas(UsefulAtriStationEvent *evPtr,
					 TPad *useCan)
{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AraGeomTool::Instance();
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
    sprintf(textLabel,"Event %d",evPtr->eventNumber);
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
  setupRFChanPadWithFrames(plotPad);


  
  for(int column=0;column<RF_COLS;column++) {
    for(int row=0;row<RF_ROWS;row++) {
      plotPad->cd();
      int rfChan=column+4*row;
      
      sprintf(padName,"rfChanPad%d",column+4*row);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromRFPad(paddy1,rfChan);
      paddy1->cd();
      
      if(fWaveformOption==AraDisplayFormatOption::kPowerSpectralDensity){
	if(!grRFChanFFT[rfChan]) {
	  TGraph *grTemp=grRFChan[rfChan]->getFFT();
	  grRFChanFFT[rfChan]=new AraFFTGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	if(grRFChanFFT[rfChan]->GetN())
	  grRFChanFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	if(grRFChanAveragedFFT[rfChan]->GetN())
	  grRFChanAveragedFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(!grRFChanHilbert[rfChan])  {
	  TGraph *grTemp=grRFChan[rfChan]->getHilbert();
	  grRFChanHilbert[rfChan]=new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	if(grRFChanHilbert[rfChan]->GetN())
	  grRFChanHilbert[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform) {

	grRFChan[rfChan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimitRf[row]-VOLT_PADDING,fMaxVoltLimitRf[row]+VOLT_PADDING);
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


TPad *AraAtriCanvasMaker::getAntennaCanvas(UsefulAtriStationEvent *evPtr,
					   TPad *useCan)
{
   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 
  if(!fACMGeomTool)
    fACMGeomTool=AraGeomTool::Instance();

  if(fLastStationId!=evPtr->stationId) {
    fLastStationId=evPtr->stationId;
    fillRfChanMap(evPtr->stationId);
  }

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
    sprintf(textLabel,"Event %d",evPtr->eventNumber);
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
  setupAntPadWithFrames(plotPad);


  for(int row=0;row<ANT_ROWS;row++) {
    for(int column=0;column<ANT_COLS;column++) {
      plotPad->cd();
      int rfChan=rfChanMap[row][column];
      //      std::cout << row << "\t" << column << "\t" << rfChan << "\n";
      sprintf(padName,"antPad%d_%d",column,row);
      TPad *paddy1 = (TPad*) plotPad->FindObject(padName);
      paddy1->SetEditable(kTRUE);
      deleteTGraphsFromRFPad(paddy1,rfChan);
      paddy1->cd();
      
      if(fWaveformOption==AraDisplayFormatOption::kPowerSpectralDensity){
	if(!grRFChanFFT[rfChan]) {	  
	  TGraph *grTemp=grRFChan[rfChan]->getFFT();
	  grRFChanFFT[rfChan]=new AraFFTGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	if(grRFChanFFT[rfChan]->GetN())
	  grRFChanFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	if(grRFChanAveragedFFT[rfChan]->GetN())
	  grRFChanAveragedFFT[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(!grRFChanHilbert[rfChan]) {	  
	  TGraph *grTemp=grRFChan[rfChan]->getHilbert();
	  grRFChanHilbert[rfChan]=new AraWaveformGraph(grTemp->GetN(),grTemp->GetX(),grTemp->GetY());
	  delete grTemp;
	}
	if(grRFChanHilbert[rfChan]->GetN())
	  grRFChanHilbert[rfChan]->Draw("l");
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform) {
	if(grRFChan[rfChan]->GetN())
	  grRFChan[rfChan]->Draw("l");

	if(fAutoScale) {
	  TList *listy = gPad->GetListOfPrimitives();
	  for(int i=0;i<listy->GetSize();i++) {
	    TObject *fred = listy->At(i);
	    TH1F *tempHist = (TH1F*) fred;
	    if(tempHist->InheritsFrom("TH1")) {
	      tempHist->GetYaxis()->SetRangeUser(fMinVoltLimitAnt[row]-VOLT_PADDING,fMaxVoltLimitAnt[row]+VOLT_PADDING);
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


TPad *AraAtriCanvasMaker::getIntMapCanvas(UsefulAtriStationEvent *evPtr,
				       TPad *useCan)
{
  static UInt_t lastEventNumber=0;
  static UInt_t lastUnixTime=0;
  static UInt_t lastUnixTimeUs=0;
  Int_t sameEvent=0;
  if(lastEventNumber==evPtr->eventNumber) {
    if(lastUnixTime==evPtr->unixTime) {
      if(lastUnixTimeUs==evPtr->unixTimeUs) {
	sameEvent=1;
      }
    }
  }
  lastEventNumber=evPtr->eventNumber;
  lastUnixTime=evPtr->unixTime;
  lastUnixTimeUs=evPtr->unixTimeUs;
  

   //  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(0); 

  if(!fACMGeomTool)
    fACMGeomTool=AraGeomTool::Instance();
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
    sprintf(textLabel,"Event %d",evPtr->eventNumber);
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

  
  //  if(!histMapV)
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

  //  if(!histMapH)
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







void AraAtriCanvasMaker::setupElecPadWithFrames(TPad *plotPad)
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


  int numRows=RFCHAN_PER_DDA;
  int numCols=DDA_PER_ATRI;

  static int elecPadsDone=0;
  if(elecPadsDone && !fRedoEventCanvas) {
    int errors=0;
    for(int i=0;i<numRows*numCols;i++) {
      sprintf(padName,"elecChanPad%d",i);
      TPad *paddy = (TPad*) plotPad->FindObject(padName);
      if(!paddy)
	errors++;
    }
    if(!errors)
      return;
  }

  elecPadsDone=1;
    

  Double_t left[DDA_PER_ATRI]={0.04,0.28,0.52,0.76};
  Double_t right[DDA_PER_ATRI]={0.28,0.52,0.76,0.99};
  Double_t top[RFCHAN_PER_DDA]={0.93,0.80,0.69,0.58,0.47,0.36,0.25,0.14};
  Double_t bottom[RFCHAN_PER_DDA]={0.80,0.69,0.58,0.47,0.36,0.25,0.14,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<numCols;column++) {
    sprintf(textLabel,"DDA %d",column+1);
    if(column==numCols-1)
      texy.DrawTextNDC(right[column]-0.1,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.09,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  //  texy.DrawTextNDC(left[0]-0.01,bottom[0],"A");
  //  texy.DrawTextNDC(left[0]-0.01,bottom[2],"B");
  //  texy.DrawTextNDC(left[0]-0.01,bottom[4],"C");

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




  for(int row=0;row<numRows;row++) {
    for(int column=0;column<numCols;column++) {
      plotPad->cd();
      //      if(row%2==1 && column==4) continue;
      sprintf(padName,"elecChanPad%d",row+column*RFCHAN_PER_DDA);
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==numCols-1)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==numRows-1)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();

      TH1F *framey=0;
      

      //      std::cout << "Limits\t" << fMinVoltLimitElec << "\t" << fMaxVoltLimitElec << "\n";
  

      if(fWaveformOption==AraDisplayFormatOption::kWaveform || fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) 
	//framey = (TH1F*) paddy1->DrawFrame(0,fMinVoltLimitElec,250,fMaxVoltLimitElec);
	framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimitElec[column],fMaxTimeLimit,fMaxVoltLimitElec[column]);
      else if(fWaveformOption==AraDisplayFormatOption::kFFT || fWaveformOption==AraDisplayFormatOption::kAveragedFFT)
	framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit); 

      framey->GetYaxis()->SetLabelSize(0.08);
      framey->GetYaxis()->SetTitleSize(0.1);
      framey->GetYaxis()->SetTitleOffset(0.5);
	 
      if(row==numRows-1) {
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



void AraAtriCanvasMaker::setupRFChanPadWithFrames(TPad *plotPad)
{
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
    for(int rfChan=0;rfChan<ANTS_PER_ATRI;rfChan++) {
	sprintf(padName,"rfChanPad%d",rfChan);
	TPad *paddy = (TPad*) plotPad->FindObject(padName);
	if(!paddy)
	  errors++;
    }
    if(!errors)
      return;
  }
  
  
  rfChanPadsDone=1;
  
  
  Double_t left[4]={0.04,0.27,0.50,0.73};
  Double_t right[4]={0.27,0.50,0.73,0.96};
  Double_t top[5]={0.95,0.77,0.59,0.41,0.23};
  Double_t bottom[5]={0.77,0.59,0.41,0.23,0.05};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<RF_COLS;column++) {
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
  texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.1,"17-20");

 
  int count=0;

  //  1  2  3  4
  //  5  6  7  8
  //  9 10 11 12
  // 13 14 15 16

  for(int column=0;column<RF_COLS;column++) {
    for(int row=0;row<RF_ROWS;row++) {
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
      if(row==4)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();
      TH1F *framey=0;
      if(fWaveformOption==AraDisplayFormatOption::kFFT || fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform || 
	      fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(row<3) {
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimitRf[row],fMaxTimeLimit,fMaxVoltLimitRf[row]);
	}
	else{
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinClockVoltLimit,fMaxTimeLimit,fMaxClockVoltLimit);
	}
      }

      framey->GetYaxis()->SetLabelSize(0.1);
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



void AraAtriCanvasMaker::setupAntPadWithFrames(TPad *plotPad)
{
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
    for(int column=0;column<ANT_COLS;column++) {
      for(int row=0;row<ANT_ROWS;row++) {
	sprintf(padName,"antPad%d_%d",column,row);
	TPad *paddy = (TPad*) plotPad->FindObject(padName);
	if(!paddy)
	  errors++;
      }
    }
    if(!errors)
      return;
  }
  
  
  antPadsDone=1;
  
  
  Double_t left[4]={0.04,0.27,0.50,0.73};
  Double_t right[4]={0.27,0.50,0.73,0.96};
  Double_t top[5]={0.95,0.77,0.59,0.41,0.23};
  Double_t bottom[5]={0.77,0.59,0.41,0.23,0.05};
  //  Double_t top[4]={0.95,0.72,0.49,0.26};
  //  Double_t bottom[4]={0.72,0.49,0.26,0.03};
  
  //Now add some labels around the plot
  TLatex texy;
  texy.SetTextSize(0.03); 
  texy.SetTextAlign(12);  
  for(int column=0;column<ANT_COLS;column++) {
    sprintf(textLabel,"%d",1+column);
    if(column==3)
      texy.DrawTextNDC(right[column]-0.12,0.97,textLabel);
    else
      texy.DrawTextNDC(right[column]-0.12,0.97,textLabel);
  }
  texy.SetTextAlign(21);  
  texy.SetTextAngle(90);
  texy.DrawTextNDC(left[0]-0.01,bottom[0]+0.1,"TV");
  texy.DrawTextNDC(left[0]-0.01,bottom[1]+0.1,"BV");
  texy.DrawTextNDC(left[0]-0.01,bottom[2]+0.1,"TH");
  texy.DrawTextNDC(left[0]-0.01,bottom[3]+0.1,"BH");
  texy.DrawTextNDC(left[0]-0.01,bottom[4]+0.1,"SA");

 
  int count=0;



  
  for(int column=0;column<ANT_COLS;column++) {
    for(int row=0;row<ANT_ROWS;row++) {
      plotPad->cd();
      sprintf(padName,"antPad%d_%d",column,row);
    
      TPad *paddy1 = new TPad(padName,padName,left[column],bottom[row],right[column],top[row]);   
      paddy1->SetTopMargin(0);
      paddy1->SetBottomMargin(0);
      paddy1->SetLeftMargin(0);
      paddy1->SetRightMargin(0);
      if(column==3)
	paddy1->SetRightMargin(0.01);
      if(column==0)
	paddy1->SetLeftMargin(0.1);
      if(row==4)
	paddy1->SetBottomMargin(0.1);
      paddy1->Draw();
      paddy1->cd();
      TH1F *framey=0;
      if(fWaveformOption==AraDisplayFormatOption::kFFT || fWaveformOption==AraDisplayFormatOption::kAveragedFFT){
	  framey = (TH1F*) paddy1->DrawFrame(fMinFreqLimit,fMinPowerLimit,fMaxFreqLimit,fMaxPowerLimit);
      }
      else if(fWaveformOption==AraDisplayFormatOption::kWaveform || 
	      fWaveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
	if(row<3) {
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinVoltLimitAnt[row],fMaxTimeLimit,fMaxVoltLimitAnt[row]);
	}
	else{
	  framey = (TH1F*) paddy1->DrawFrame(fMinTimeLimit,fMinClockVoltLimit,fMaxTimeLimit,fMaxClockVoltLimit);
	}
      }

      framey->GetYaxis()->SetLabelSize(0.1);
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



void AraAtriCanvasMaker::deleteTGraphsFromElecPad(TPad *paddy,int chan)
{
  paddy->cd();
  if(fLastWaveformFormat==AraDisplayFormatOption::kWaveform) paddy->GetListOfPrimitives()->Remove(grElec[chan]);
  else if(fLastWaveformFormat==AraDisplayFormatOption::kFFT) paddy->GetListOfPrimitives()->Remove(grElecFFT[chan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kAveragedFFT) paddy->GetListOfPrimitives()->Remove(grElecAveragedFFT[chan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kHilbertEnvelope) paddy->GetListOfPrimitives()->Remove(grElecHilbert[chan]);  
  //  paddy->Update();
}


void AraAtriCanvasMaker::deleteTGraphsFromRFPad(TPad *paddy,int rfchan)
{
  paddy->cd();
  if(fLastWaveformFormat==AraDisplayFormatOption::kWaveform) paddy->GetListOfPrimitives()->Remove(grRFChan[rfchan]);
  else if(fLastWaveformFormat==AraDisplayFormatOption::kFFT) paddy->GetListOfPrimitives()->Remove(grRFChanFFT[rfchan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kAveragedFFT) paddy->GetListOfPrimitives()->Remove(grRFChanAveragedFFT[rfchan]); 
  else if(fLastWaveformFormat==AraDisplayFormatOption::kHilbertEnvelope) paddy->GetListOfPrimitives()->Remove(grRFChanHilbert[rfchan]);  
  //  paddy->Update();
}


void AraAtriCanvasMaker::resetAverage() 
{
  for(int chan=0;chan<CHANNELS_PER_ATRI;chan++) {
    if(grElecAveragedFFT[chan]) {
      delete grElecAveragedFFT[chan];
      grElecAveragedFFT[chan]=0;
    }
  }
  for(int rfchan=0;rfchan<ANTS_PER_ATRI;rfchan++) {
    if(grRFChanAveragedFFT[rfchan]) {
      delete grRFChanAveragedFFT[rfchan];
      grRFChanAveragedFFT[rfchan]=0;
    }
  }
}



