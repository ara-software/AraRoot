#include "AraWaveformGraph.h"
#include "AraCorrelationFactory.h"
#include "AraGeomTool.h"
#include "TButton.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1.h"
#include "TAxis.h"
#include <iostream>
using namespace std;

#include "FFTtools.h"


ClassImp(AraWaveformGraph);

AraWaveformGraph::AraWaveformGraph()

  : TGraph(),fElecChan(-1),fRFChan(-1),fNewCanvas(0)
    
{
  this->SetEditable(kFALSE);
}


AraWaveformGraph::AraWaveformGraph(int N, const Int_t *x, const Int_t *y)

  : TGraph(N,x,y),fElecChan(-1),fRFChan(-1),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

AraWaveformGraph::AraWaveformGraph(int N, const Float_t *x, const Float_t *y)

  : TGraph(N,x,y),fElecChan(-1),fRFChan(-1),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

AraWaveformGraph::AraWaveformGraph(int N, const Double_t *x, const Double_t *y)

  : TGraph(N,x,y),fElecChan(-1),fRFChan(-1),fNewCanvas(0)
{  
  this->SetEditable(kFALSE);
}

AraWaveformGraph::~AraWaveformGraph()
{
   //   std::cout << "~AraWaveformGraph" << std::endl;
}

void AraWaveformGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
  static int keyWasPressed=0;
   switch (event) {
   case kKeyPress:
     //     std::cout << "kKeyPress" << std::endl;
     keyWasPressed=1;
     break;
   case kButtonPress:
     //     cout << "kButtonPress" << endl;
     break;
     
   case kButton1Double:
     //     std::cout << "kButtonDoubleClick" << std::endl;
     //     new TCanvas();
     break;

   case kButton1Down:
     //     std::cout << "kButton1Down" << std::endl;
     if(!keyWasPressed) {
       if(!fNewCanvas) drawInNewCanvas();
       else this->TGraph::ExecuteEvent(event,px,py);
     }
     else {
       //       std::cout << "ctrl + click\n";
       AraCorrelationFactory::Instance()->addWaveformToCorrelation(this);
       keyWasPressed=0;
     }

     break;
          
   default:
       this->TGraph::ExecuteEvent(event,px,py);
       break;
   }
}

void AraWaveformGraph::setElecChan(int elecChan)
{
  fElecChan=elecChan;
  char graphName[180];
  sprintf(graphName,"grElec%d",elecChan);
  this->SetName(graphName);
  char graphTitle[180];
  sprintf(graphTitle,"Raw Electronics Channel %d",elecChan+1);
  this->SetTitle(graphTitle);
  
}

void AraWaveformGraph::setRFChan(int rfChan, int stationId)
{
  fRFChan=rfChan;
  char graphName[180];
  sprintf(graphName,"grRFChan%d",rfChan);
  this->SetName(graphName);
  char graphTitle[180];
  sprintf(graphTitle,"RF Channel %d (Ant %s)",rfChan+1,AraGeomTool::Instance()->fStationInfo[stationId].fAntInfo[rfChan].designator);
  this->SetTitle(graphTitle);
}
  
  
void AraWaveformGraph::drawInNewCanvas()
{
  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(1);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
  AraWaveformGraph *thisCopy = (AraWaveformGraph*)this->Clone();
  thisCopy->GetXaxis()->SetLabelSize(0.06);
  thisCopy->GetXaxis()->SetTitleSize(0.06);
  thisCopy->GetYaxis()->SetLabelSize(0.06);
  thisCopy->GetYaxis()->SetTitleSize(0.06);
  thisCopy->GetXaxis()->SetTitle("Time (ns)");
  thisCopy->GetYaxis()->SetTitle("Voltage (mV-ish)");


  thisCopy->SetTitle(this->GetTitle());
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);
  thisCopy->Draw("al");
  //  fNewCanvas=1;
  
}

//______________________________________________________________________________
void AraWaveformGraph::DrawFFT()
{
  //  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetLabelSize(0.1,"xy");
  gStyle->SetTitleSize(0.1,"xy");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetOptTitle(1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
   
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);

  TGraph *grFFT  = this->getFFT();
  grFFT->GetXaxis()->SetLabelSize(0.06);
  grFFT->GetXaxis()->SetTitleSize(0.06);
  grFFT->GetYaxis()->SetLabelSize(0.06);
  grFFT->GetYaxis()->SetTitleSize(0.06);
  grFFT->GetXaxis()->SetTitle("Frequency (MHz)");
  grFFT->GetYaxis()->SetTitle("dB (m maybe)");

  

  grFFT->SetTitle(this->GetTitle());
  grFFT->Draw("al");
  
  //   printf("AraWaveformGraph::DrawFFT: not yet implemented\n");
}

//______________________________________________________________________________
void AraWaveformGraph::DrawHilbert()
{
  //  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetLabelSize(0.1,"xy");
  gStyle->SetTitleSize(0.1,"xy");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetOptTitle(1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
   
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);

  TGraph *grHilbert  = this->getHilbert();
  grHilbert->GetXaxis()->SetLabelSize(0.06);
  grHilbert->GetXaxis()->SetTitleSize(0.06);
  grHilbert->GetYaxis()->SetLabelSize(0.06);
  grHilbert->GetYaxis()->SetTitleSize(0.06);
  grHilbert->GetXaxis()->SetTitle("Time (ns)");
  grHilbert->GetYaxis()->SetTitle("Voltage^2 (mv^2) ");
 
  grHilbert->SetTitle(this->GetTitle());
  grHilbert->Draw("al");
  
  //   printf("AraWaveformGraph::DrawFFT: not yet implemented\n");
}

TGraph *AraWaveformGraph::getFFT()
{
  Double_t newX[512],newY[512];
  TGraph *grInt = FFTtools::getInterpolatedGraph(this,0.5);
  Int_t numSamps=grInt->GetN();
  Double_t *xVals=grInt->GetX();
  Double_t *yVals=grInt->GetY();
  for(int i=0;i<512;i++) {
    if(i<numSamps) {
      newX[i]=xVals[i];
      newY[i]=yVals[i];
    }
    else {
      newX[i]=newX[i-1]+(1.);
      newY[i]=0;
    }      
  }
  TGraph *grNew = new TGraph(512,newX,newY);
  TGraph *grFFT = FFTtools::makePowerSpectrumMilliVoltsNanoSecondsdB(grNew);
  delete grNew;
  delete grInt;
  return grFFT;
}

TH1D *AraWaveformGraph::getFFTHisto() {
  TGraph *grFFT =getFFT();
  Double_t *xVals=grFFT->GetX();
  Double_t *yVals=grFFT->GetY();
  Int_t numPoints=grFFT->GetN();
  Double_t binWidth=xVals[1]-xVals[0];
  char histName[180];
  sprintf(histName,"%s_ffthist",this->GetName());
  TH1D *histFFT = new TH1D(histName,histName,256,0,1000);
  for(int i=0;i<numPoints;i++) {    
    histFFT->Fill(xVals[i],yVals[i]);
  }
  delete grFFT;
  return histFFT;
}




TGraph *AraWaveformGraph::getHilbert()
{
  TGraph *grInt = FFTtools::getInterpolatedGraph(this,1./2.6);
  TGraph *grHilbert = FFTtools::getHilbertEnvelope(grInt);
  delete grInt;
  return grHilbert;
}

void AraWaveformGraph::AddToCorrelation()
{
  AraCorrelationFactory::Instance()->addWaveformToCorrelation(this);
}
