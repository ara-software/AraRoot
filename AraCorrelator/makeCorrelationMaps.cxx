#include <iostream>
#include <libgen.h>

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TH2.h"

#include "RawIcrrStationEvent.h"  
#include "UsefulIcrrStationEvent.h"
#include "AraEventCorrelator.h"



int main(int argc, char **argv)
{
  if(argc<3) {
    std::cout << "Usage " << basename(argv[0]) << " <input file> <output file>" << std::endl;
    return -1;
  }

  TFile *fpIn = new TFile(argv[1],"OLD");
  if(!fpIn) {
    std::cerr << "Can not open " << argv[1] << "\n";
    return -1;
  }
  TTree *eventTree = (TTree*) fpIn->Get("eventTree");
  if(!eventTree) {
    std::cerr << "Can't get eventTree\n";
    return -1;
  }
  RawIcrrStationEvent *rawEvPtr=0;
  eventTree->SetBranchAddress("event",&rawEvPtr);
  
  TFile *fpOut = new TFile(argv[2],"RECREATE");
  if(!fpOut) {
    std::cout << "Can not open " << argv[2] << "\n";
    return -1;
  }
  
  AraEventCorrelator *theCorrelator = AraEventCorrelator::Instance();
  //theCorrelator->fDebugMode=1;

  char histName[180];
  for(int i=0;i<50;i++) {
    eventTree->GetEvent(i);
    fpOut->cd();
    UsefulIcrrStationEvent usefulEvent(rawEvPtr,AraCalType::kFirstCalib);
    TH2D *histMap = theCorrelator->getInterferometricMap(&usefulEvent,AraAntPol::kVertical);
    sprintf(histName,"vmap%d",i);
    histMap->SetName(histName);
    TH2D *histMap2 = theCorrelator->getInterferometricMap(&usefulEvent,AraAntPol::kHorizontal);
    sprintf(histName,"hmap%d",i);
    histMap2->SetName(histName);
    //histMap->Write();
    //histMap2->Write();
  }
  fpOut->Close();


}

