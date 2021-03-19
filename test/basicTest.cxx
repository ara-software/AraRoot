#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

/*
	Global variables to control our expectations for this test

*/
int numEntries_expected = 100; // number of events in the file
double max_mean = 1E-6; // required deviation from zero in the mean of a properly calibrated event
double max_diff_cal = 0.1; // required difference between means of waveforms when to different cal strategies are used

int main(int argc, char **argv){

	if(argc<2){
		std::cout<<"Usage requires input in the form: " << basename(argv[0]) << " <input data file>"<<std::endl;
		exit(-1);
	}

	// Check if the data file can be opened at all
	TFile *fpIn = new TFile(argv[1], "READ");
	if(fpIn->IsZombie()){
		printf("Cannot open ARA data file (%s). Test will fail.\n",argv[1]);
		exit(-1);
	}

	// Check if we can load the event tree
	fpIn->cd();
	TTree *eventTree = (TTree*) fpIn->Get("eventTree");
	if(!eventTree){
		printf("Cannot find eventTree in file (%s). Test will fail.\n",argv[1]);
		exit(-1);
	}

	// count the number of events (should be exactly 100)
	RawAtriStationEvent *rawEvent = 0;
	eventTree->SetBranchAddress("event", &rawEvent);
	int numEntries = eventTree->GetEntries();
	if(numEntries != numEntries_expected){
		printf("Number of events is %d (%d expected). Test will fail.",numEntries, numEntries_expected);
		exit(-1);
	}

	// make sure that when we get a fully calibrated event, the mean of every waveform is zero
	for(int event=0; event<numEntries; event++){
		eventTree->GetEntry(event);
		UsefulAtriStationEvent *usefulEvent = new UsefulAtriStationEvent(rawEvent, AraCalType::kLatestCalib);
		for(int ch=0; ch<16; ch++){
			TGraph *wave = usefulEvent->getGraphFromRFChan(ch);
			double mean = TMath::Mean(wave->GetN(), wave->GetY());
			delete wave;
			if(TMath::Abs(mean)>max_mean){
				printf("Event %d, Ch %d has a non zero mean (%e). Test will fail.", event, ch, mean);
				exit(-1);
			}
		}
		delete usefulEvent;
	}

	// make sure the two different calibration strategies return different results
	for(int event=0; event<numEntries; event++){
		eventTree->GetEntry(event);
		UsefulAtriStationEvent *usefulEvent_cal1 = new UsefulAtriStationEvent(rawEvent, AraCalType::kLatestCalib);
		UsefulAtriStationEvent *usefulEvent_cal2 = new UsefulAtriStationEvent(rawEvent, AraCalType::kNoCalib);
		for(int ch=0; ch<16; ch++){
			TGraph *wave_cal1 = usefulEvent_cal1->getGraphFromRFChan(ch);
			TGraph *wave_cal2 = usefulEvent_cal2->getGraphFromRFChan(ch);
			double mean_cal1 = TMath::Mean(wave_cal1->GetN(), wave_cal1->GetY());
			double mean_cal2 = TMath::Mean(wave_cal2->GetN(), wave_cal2->GetY());
			delete wave_cal1;
			delete wave_cal2;
			if(TMath::Abs(mean_cal2 - mean_cal1)<max_diff_cal){
				printf("Event %d, Ch %d: difference in waveform means between two cals is too small (%.2f vs %.2f). Test will fail. \n",
					event, ch, mean_cal1, mean_cal2);
				exit(-1);
			}
		}
		delete usefulEvent_cal1;
		delete usefulEvent_cal2;
	}


}

