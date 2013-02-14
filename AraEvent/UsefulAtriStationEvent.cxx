//////////////////////////////////////////////////////////////////////////////
/////  UsefulAtriStationEvent.cxx        ARA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ARA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAtriStationEvent.h"
#include "AraEventCalibrator.h"
#include "FFTtools.h"
#include "AraGeomTool.h"
#include "TH1.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(UsefulAtriStationEvent);

AraEventCalibrator *fCalibrator;

UsefulAtriStationEvent::UsefulAtriStationEvent() 
{
   //Default Constructor
  fNumChannels=0;
  fCalibrator=0;
}

UsefulAtriStationEvent::~UsefulAtriStationEvent() {
   //Default Destructor
  fNumChannels=0;
  fCalibrator=0;
}

UsefulAtriStationEvent::UsefulAtriStationEvent(RawAtriStationEvent *rawEvent, AraCalType::AraCalType_t calType)
 :RawAtriStationEvent(*rawEvent)
{
  fCalibrator=AraEventCalibrator::Instance();
  fNumChannels=0;
  fCalibrator->calibrateEvent(this,calType);
  //  fprintf(stderr, "UsefulAtriStationEvent::UsefulAtriStationEvent() -- finished constructing event\n");  //DEBUG

}


TGraph *UsefulAtriStationEvent::getGraphFromElecChan(int chanId)
{
  std::map< Int_t, std::vector <Double_t> >::iterator timeMapIt;
  timeMapIt=fTimes.find(chanId);
  if(timeMapIt==fTimes.end()) {
    // This channel doesn't exist. We don't return a null pointer,
    // we return an empty graph.
    return new TGraph;
  }
  
  TGraph *gr = new TGraph(fTimes[chanId].size(),&(fTimes[chanId][0]),&(fVolts[chanId][0]));

  gr->Sort();
  
  return gr;
}

TGraph *UsefulAtriStationEvent::getGraphFromRFChan(int chan)
{

  //FIXME -- should probably fix this
  return getGraphFromElecChan(chan);
  return NULL;
}


TGraph *UsefulAtriStationEvent::getFFTForRFChan(int chan)
{

   //   static AraGeomTool *fGeomTool = AraGeomTool::Instance();
   TGraph *gr = getGraphFromRFChan(chan);
   if(!gr) return NULL;
   Double_t newX[512],newY[512];
   Double_t intSample=1;
   Int_t maxSamps=256;
   // if(fGeomTool->getNumLabChansForChan(chan)==2) {
   intSample=0.5;
   maxSamps=512;
   //   }


   TGraph *grInt = FFTtools::getInterpolatedGraph(gr,intSample);


   Int_t numSamps  = grInt->GetN();
   Double_t *xVals = grInt->GetX();
   Double_t *yVals = grInt->GetY();
   for(int i=0;i<maxSamps;i++) {
      if(i<numSamps) {
         newX[i]=xVals[i];
         newY[i]=yVals[i];
      }
      else {
         newX[i]=newX[i-1]+intSample;
         newY[i]=0;
      }
  }
   TGraph *grNew = new TGraph(maxSamps,newX,newY);
   TGraph *grFFT = FFTtools::makePowerSpectrumMilliVoltsNanoSecondsdB(grNew);
   delete gr;
   delete grNew;
   delete grInt;
   return grFFT;
}


TH1D *UsefulAtriStationEvent::getFFTHistForRFChan(int chan)
{

   Int_t numBins=256;
   Double_t minX=0.0;
   Double_t maxX=1000.0;
   minX = minX - ( (maxX-minX)/numBins/2.0 ); // adjust histogram edges so that the bin centers
   maxX = maxX + ( (maxX-minX)/numBins/2.0 ); // of the histograms are aligned with graph [add bdf]
   numBins++;
   char histName[180];
   sprintf(histName,"%s_ffthist",this->GetName());
   TH1D *histFFT = new TH1D(histName,histName,numBins,minX,maxX);
   if(fillFFTHistoForRFChan(chan,histFFT)==0)
      return histFFT;
   return NULL;

}

int UsefulAtriStationEvent::fillFFTHistoForRFChan(int chan, TH1D *histFFT)
{

   TGraph *grFFT =getFFTForRFChan(chan);
   if(!grFFT) return -1;
   Double_t *xVals=grFFT->GetX();
   Double_t *yVals=grFFT->GetY();
   Int_t numPoints=grFFT->GetN();
   for(int i=0;i<numPoints;i++) {
      histFFT->Fill(xVals[i],yVals[i]);
   }
   delete grFFT;
   return 0;

}

