//////////////////////////////////////////////////////////////////////////////
/////  UsefulAtriStationEvent.cxx        ARA header reading class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ARA headers and produces   ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAtriStationEvent.h"
#include "AraEventCalibrator.h"
#include "AraEventConditioner.h"
#include "FFTtools.h"
#include "AraGeomTool.h"
#include "TH1.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(UsefulAtriStationEvent);

AraEventCalibrator *fCalibrator;
AraEventConditioner *fConditioner;

UsefulAtriStationEvent::UsefulAtriStationEvent() 
{
  //Default Constructor
  fNumChannels=0;
  fCalibrator=0;
  fConditioner=0;
  fIsConditioned=0;
}

UsefulAtriStationEvent::~UsefulAtriStationEvent() {
   //Default Destructor
  fNumChannels=0;
  fCalibrator=0;
  fConditioner=0;
  fIsConditioned=0;
}

UsefulAtriStationEvent::UsefulAtriStationEvent(RawAtriStationEvent *rawEvent, AraCalType::AraCalType_t calType)
 :RawAtriStationEvent(*rawEvent)
{
  fCalibrator=AraEventCalibrator::Instance();
  fNumChannels=0;
  fCalibrator->calibrateEvent(this,calType);
  fIsConditioned=0;
  fConditioner=AraEventConditioner::Instance();
  fConditioner->conditionEvent(this);
}


TGraph *UsefulAtriStationEvent::getGraphFromElecChan(int chanId)
{
  std::map< Int_t, std::vector <Double_t> >::iterator timeMapIt;
  timeMapIt=fTimes.find(chanId);
  if(timeMapIt==fTimes.end()) {
    // This channel doesn't exist. We don't return a null pointer,
    // we return an empty graph. 
    // RJN should fix this as it is a silly idea
    return new TGraph;
  }
  
  TGraph *gr = new TGraph(fTimes[chanId].size(),&(fTimes[chanId][0]),&(fVolts[chanId][0]));

  //Why do we need to sort the array. Shouldn't this be done in AraEventCalibrator??
  //FIXME -- jpd - this is my dumb idea
//   double lastTime=-1e9;
//   int countNegative=0;
//   for(int i=0;i<fTimes[chanId].size();i++) {
//      if(fTimes[chanId][i]<lastTime) {
// 	std::cerr << "Ooops: " << chanId << "\t" << i << "\t" << lastTime << "\t" << fTimes[chanId][i] << "\n";
// 	countNegative++;
//      }
//      lastTime=fTimes[chanId][i];
//   }

//   if(countNegative>0) {
//      std::cerr << "Back in time on chan Id: " << chanId << "\t" << countNegative << "\n";
//      gr->Sort();
//   }
  if(fTimes[chanId].size()==0) {
     std::cerr << "Oh no there aren't any points\n";
  }

  return gr;
}

TGraph *UsefulAtriStationEvent::getGraphFromRFChan(int chan)
{ 
  Int_t elecChan = AraGeomTool::Instance()->getElecChanFromRFChan(chan,stationId);
  if(elecChan < 0){
    return NULL;
  }
  
  TGraph *grRet = getGraphFromElecChan(elecChan);
  TGraph *grOut = trimGraph(grRet, 20.); //trim off 20 ns from the *front* (remove the first block essentially)
  delete grRet;
  return grOut;
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



Int_t UsefulAtriStationEvent::getNumRFChannels() 
{
  return AraGeomTool::Instance()->getStationInfo(stationId)->getNumRFChans();

}

/*
Trim first 20ns from the waveform
inputs: graph to be trimmed, amount to trim from the front of the graph
returns: pointer to new trimmed graph
*/
TGraph *UsefulAtriStationEvent::trimGraph(TGraph *grIn, double trim_value){

  //load the old X and Y arrays
  double *oldX = grIn->GetX();
  double *oldY = grIn->GetY();
  
  //record the first sample
  double first_time = oldX[0];

  //create holders for the trimmed X and Y arrays
  std::vector<double> newX;
  std::vector<double> newY;

  for(int samp=0; samp<grIn->GetN(); samp++){ //loop over samples in the old waveform
    if(oldX[samp]>first_time+trim_value){ //if the time of the sample is > the trim amount, keep it
      newX.push_back(oldX[samp]); //record the x value
      newY.push_back(oldY[samp]); //record the y value
    }
  }

  TGraph *grOut = new TGraph(newX.size(), &newX[0], &newY[0]);
  return grOut;
}