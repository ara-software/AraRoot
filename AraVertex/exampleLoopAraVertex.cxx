////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////  exampleLoopAraVertex.cxx 
////      demonstrate how to use the AraVertex library
////
////    Apr 2020,  baclark@msu.edu 
////////////////////////////////////////////////////////////////////////////////

//Includes
#include <iostream>

//AraRoot Includes
#include "RawAtriStationEvent.h"
#include "UsefulAraStationEvent.h"
#include "UsefulIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "AraVertex.cxx" 
#include "AraRecoHandler.cxx"

//ROOT Includes
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"
#include "FFTtools.h"

RawIcrrStationEvent *rawIcrrEvPtr;
RawAtriStationEvent *rawAtriEvPtr;
RawAraStationEvent *rawEvPtr;
UsefulAtriStationEvent *realAtriEvPtr;

int main(int argc, char **argv)
{

  if(argc<3) {
    std::cout << "Usage\n" << argv[0] << " <station> <input file>\n";
    std::cout << "e.g.\n" << argv[0] << " 2 http://www.hep.ucl.ac.uk/uhen/ara/monitor/root/run1841/event1841.root\n";
    return 0;
  }

  int station = atoi(argv[1]);

  TFile *fp = TFile::Open(argv[2]);
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

  //Check an event in the run Tree and see if it is station1 or TestBed (stationId<2)
  eventTree->SetBranchAddress("event",&rawEvPtr);
  eventTree->GetEntry(0);

  if((rawEvPtr->stationId)<2){
    isIcrrEvent=1;
    isAtriEvent=0;
  }
  else{
    isIcrrEvent=0;
    isAtriEvent=1; 
  }
  eventTree->ResetBranchAddresses();
 
  //Now set the appropriate branch addresses
  //The Icrr case
  if(isIcrrEvent){
    eventTree->SetBranchAddress("event", &rawIcrrEvPtr);
    std::cerr << "Set Branch address to Icrr\n";
    std::cerr << "AraVertex/AraRecoHandler files currently not explicitly designed for ICRR events, quitting! \n";
    return -1;
  }
  //The Atri case
  else{
    eventTree->SetBranchAddress("event", &rawAtriEvPtr);
    std::cerr << "Set Branch address to Atri\n";
  }


  // compute the center of gravity (COG) of the station
  AraGeomTool *araGeom = AraGeomTool::Instance();
  double antenna_average[3]={0.};
  for(int i=0; i<16; i++){
    for(int ii=0; ii<3; ii++){
      antenna_average[ii]+=(araGeom->getStationInfo(station)->getAntennaInfo(i)->antLocation[ii]);
    }
  }
  for(int ii=0; ii<3; ii++){
    antenna_average[ii]/=16.;
  }
  // invoke a AraVertex tool and set the COG of the station
  AraVertex *Reco = new AraVertex();
  Reco -> SetCOG(antenna_average[0], antenna_average[1], antenna_average[2]);

  // also, invoke a RecoHandler tool
  // the point of the RecoHandler is to help with management of the AraVertex tool
  AraRecoHandler *RecoHandler = new AraRecoHandler();

  // define some parameters for interpolation
  double interpV = 0.4;
  double interpH = 0.625;
  int nIntSamp_V = int(25./interpV);
  int nIntSamp_H = int(25./interpH);

  // define what channels we would like *excluded* from the reconstruction
  // empty, for now
  vector<int> excluded_channels;

 
  //Now we set up out run list
  Long64_t numEntries=eventTree->GetEntries();
  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;

  numEntries=100;

  for(Long64_t event=0;event<numEntries;event++) {
    cout<<"Looking at eventy Number "<<event<<endl;
    if(event%starEvery==0) {
      std::cerr << "*";       
    }
    eventTree->GetEntry(event);
    UsefulAraStationEvent * realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kLatestCalib);
    vector<TGraph*> waveforms;
    for(int i=0; i<16; i++){
      TGraph *gr = realAtriEvPtr->getGraphFromRFChan(i);
      TGraph *grInt = FFTtools::getInterpolatedGraph(gr,i<8?interpV:interpH);
      waveforms.push_back(grInt);
      delete gr;
    }

    int polarization_of_interest = 0;

    // ask the reco handler to identify hits
    RecoHandler->identifyHitsPrepToVertex(araGeom, Reco, station, polarization_of_interest, 
                excluded_channels, waveforms,
                8.
                );

    // we can print out what pairs it found
    Reco->printPairs();

    // tell the AraVertex tool to actually run the vertexing algorithm
    RECOOUT recoVxcorSimple=Reco->doPairFitSpherical();

    // get theta and phi out at the end
    double theta = 90.-recoVxcorSimple.theta*TMath::RadToDeg();
    double phi = recoVxcorSimple.phi*TMath::RadToDeg();

    // print the result to scree for the user
    printf("Spherical algorithm theta, phi %.2f, %.2f \n", theta, phi);   

    // cleanup
    for(int i=0; i<waveforms.size(); i++) delete waveforms[i];
    delete realAtriEvPtr;
  }
}
   
  
  
 
