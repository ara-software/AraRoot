//////////////////////////////////////////////////////////////////////////////
/////  AraCorrelationFactory.cxx        ANITA Event Canvas make               /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for ANITA-II            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "AraDisplayConventions.h"
#include "AraCorrelationFactory.h"

//#include "AraConventions.h"
#include "AraGeomTool.h"
#include "AraWaveformGraph.h"
#include "FFTtools.h"

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
#include "AraWaveformGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1.h"
#include "THStack.h"
#include "TList.h"
#include "TObject.h"
#include "TTimeStamp.h"



AraCorrelationFactory*  AraCorrelationFactory::fgInstance = 0;


AraCorrelationFactory::AraCorrelationFactory()
{
  //Default constructor
  fDeltaT=1./2.6;  ///Default sampling speed
  fgInstance=this;
  fGrWave[0]=NULL;
  fGrWave[1]=NULL;
  
}

AraCorrelationFactory::~AraCorrelationFactory()
{
  //Default destructor
}



//______________________________________________________________________________
AraCorrelationFactory*  AraCorrelationFactory::Instance()
{
  //static function
  return (fgInstance) ? (AraCorrelationFactory*) fgInstance : new AraCorrelationFactory();
}

void AraCorrelationFactory::addWaveformToCorrelation(AraWaveformGraph *inputWave)
{
  char graphTitle[180];
  if(fGrWave[0]==NULL)
    fGrWave[0]=inputWave;
  else {
    fGrWave[1]=inputWave;
    //Now we can correlate
    TGraph *grCor = FFTtools::getInterpolatedCorrelationGraph(fGrWave[0],fGrWave[1],fDeltaT);
    if(((fGrWave[0]->fElecChan)<0)&&((fGrWave[1]->fElecChan)<0)){
      sprintf(graphTitle, "Correlation Ant %d - %d", fGrWave[0]->fRFChan, fGrWave[1]->fRFChan);      
    }
    else if(((fGrWave[0]->fRFChan)<0)&&((fGrWave[1]->fRFChan)<0)){
      sprintf(graphTitle, "Correlation Elec Chan %d - %d", fGrWave[0]->fElecChan, fGrWave[1]->fElecChan);      
    }
      
    new TCanvas();
    gStyle->SetOptTitle(1);//jpd fix the title
    grCor->SetTitle(graphTitle);
    grCor->Draw("al");
    fGrWave[0]=0;
    fGrWave[1]=0;
  }

}
