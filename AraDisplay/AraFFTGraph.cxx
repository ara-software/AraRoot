#include "AraFFTGraph.h"
#include "TButton.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"
#include <iostream>
using namespace std;

#include "FFTtools.h"


ClassImp(AraFFTGraph);

AraFFTGraph::AraFFTGraph()

  : TGraph(),fNewCanvas(0),fNumInAverage(1)
    
{
  this->SetEditable(kFALSE);
  
}


AraFFTGraph::AraFFTGraph(int N, const Int_t *x, const Int_t *y)

  : TGraph(N,x,y),fNewCanvas(0),fNumInAverage(1)
{
  this->SetEditable(kFALSE);
}

AraFFTGraph::AraFFTGraph(int N, const Float_t *x, const Float_t *y)

  : TGraph(N,x,y),fNewCanvas(0),fNumInAverage(1)
{
  this->SetEditable(kFALSE);
}

AraFFTGraph::AraFFTGraph(int N, const Double_t *x, const Double_t *y)

  : TGraph(N,x,y),fNewCanvas(0),fNumInAverage(1)
{  
  this->SetEditable(kFALSE);
}

AraFFTGraph::~AraFFTGraph()
{
   //   std::cout << "~AraFFTGraph" << std::endl;
}

void AraFFTGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   switch (event) {
   case kButtonPress:
     //     cout << "kButtonPress" << endl;
     break;
     
   case kButtonDoubleClick:
     //     std::cout << "kButtonDoubleClick" << std::endl;
     new TCanvas();
     break;

   case kButton1Down:
     //     std::cout << "kButton1Down" << std::endl;
     if(!fNewCanvas) drawInNewCanvas();
     else this->TGraph::ExecuteEvent(event,px,py);
     break;
          
   default:
       this->TGraph::ExecuteEvent(event,px,py);
       break;
   }
}

void AraFFTGraph::drawInNewCanvas()
{
  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(1);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
  AraFFTGraph *thisCopy = (AraFFTGraph*)this->Clone();
  thisCopy->GetXaxis()->SetLabelSize(0.06);
  thisCopy->GetXaxis()->SetTitleSize(0.06);
  thisCopy->GetYaxis()->SetLabelSize(0.06);
  thisCopy->GetYaxis()->SetTitleSize(0.06);
  thisCopy->GetXaxis()->SetTitle("Frequency (MHz)");
  thisCopy->GetYaxis()->SetTitle("dB (m maybe)");
  //  sprintf(graphTitle,"Ant %d%c (%s Ring --  Phi %d -- SURF %d -- Chan %d)",
//	  fAnt+1,AnitaPol::polAsChar(fPol),AnitaRing::ringAsString(fRing),
//	  fPhi+1,fSurf+1,fChan+1);
  thisCopy->SetTitle(graphTitle);
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);
  thisCopy->Draw("al");
  //  fNewCanvas=1;
}

Int_t AraFFTGraph::AddFFT(AraFFTGraph *otherGraph)
{
  if(otherGraph->GetN()!=this->GetN()) {
    std::cerr << "Trying to add AraFFTGraph with different number of points " << otherGraph->GetN() << " instead of " << this->GetN() << "\n";
    return -1;
  }
  Double_t *newY=otherGraph->GetY();
  for(int bin=0;bin<fNpoints;bin++) {
    fY[bin]=(fY[bin]*fNumInAverage + newY[bin])/Double_t(fNumInAverage+1);
  }
  fNumInAverage++;   
  return fNumInAverage;
}

