#include <iostream>

#include "TTree.h"
#include "TFile.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TStyle.h"

#include "UsefulAtriStationEvent.h"
#include "AraEventCorrelator.h"

int main(int argc, char **argv)
{

	gStyle->SetOptStat(0);
	if(argc<2) {
		std::cout << "Usage " << basename(argv[0]) << " <input file>" << std::endl;
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
	RawAtriStationEvent *rawEvPtr=0;
	eventTree->SetBranchAddress("event",&rawEvPtr);
	
	AraEventCorrelator *theCorrelator = AraEventCorrelator::Instance(8, 2); //8 ants per polarization, station 2

	char histName[180];
	for(int i=0;i<12;i++) {
		eventTree->GetEvent(i);
		if(!rawEvPtr->isCalpulserEvent()) continue;
		UsefulAtriStationEvent *usefulEvent = new UsefulAtriStationEvent(rawEvPtr, AraCalType::kLatestCalib);
		TH2D *histMap = theCorrelator->getInterferometricMap(usefulEvent,AraAntPol::kVertical,AraCorrelatorType::kPlaneWave);
		sprintf(histName,"vmap%d",i);
		histMap->SetName(histName);
		TH2D *histMap2 = theCorrelator->getInterferometricMap(usefulEvent,AraAntPol::kHorizontal,AraCorrelatorType::kPlaneWave);
		sprintf(histName,"hmap%d",i);
		histMap2->SetName(histName);

		TCanvas *c = new TCanvas("","",2*850,850);
		c->Divide(2,1);
		c->cd(1);
		histMap->Draw("colz");
		c->cd(2);
		histMap2->Draw("colz");
		c->SaveAs("test.png");

		delete c;
		delete histMap;
		delete histMap2;
		delete usefulEvent;

	}

}

