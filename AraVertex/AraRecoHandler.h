#include "TGraph.h"
#include "AraGeomTool.h"
#include "AraVertex.h"

#include <iostream>
using namespace std;

#ifndef ARARECOHANDLER_H
#define ARARECOHANDLER_H

class AraRecoHandler {
	public:
		AraRecoHandler();
		~AraRecoHandler();

		// some functions for dealing with SNR calculation
		int getMaxBin(TGraph *gr);
		void setMeanAndSigmaInNoMax(TGraph *gr, double *stats);
		TGraph* getSqrtVoltageSquaredSummedWaveform(TGraph *gr, int nIntSamp);
		void getChannelSlidingV2SNR_UW(vector<TGraph*> interpolatedWaveforms, int nIntSamp_V, int nIntSamp_H, float *snrArray, float *hitTimeArray);
		
		// a function for hit finding and preparing to vertex
		void identifyHitsPrepToVertex(AraGeomTool *araGeom, AraVertex *Reco, int station, int pol_select, vector<int> excluded_channels, vector<TGraph*> waveforms, double hitThreshold=8.);
};
#endif