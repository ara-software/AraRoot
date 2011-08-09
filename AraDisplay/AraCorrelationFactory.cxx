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
    //    if(fGrWave[0]->fChan!=8) {
      //      sprintf(graphTitle,"Correlation Ant %d %c -%d %c",fGrWave[0]->fAnt,
      //	      AnitaPol::polAsChar(fGrWave[0]->fPol),fGrWave[1]->fAnt,
      //	      AnitaPol::polAsChar(fGrWave[1]->fPol));
    //    }
    //    else {
      //      sprintf(graphTitle,"Correlation SURF %d Chan %d - SURF %d Chan %d",
      //	      fGrWave[0]->fSurf+1,fGrWave[0]->fChan+1,
      //	      fGrWave[1]->fSurf+1,fGrWave[1]->fChan+1);
    //    }    
    new TCanvas();
    grCor->SetTitle(graphTitle);
    grCor->Draw("al");
    fGrWave[0]=0;
    fGrWave[1]=0;
  }

}
