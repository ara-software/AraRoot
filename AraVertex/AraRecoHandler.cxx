#include "TMath.h"
#include "AraRecoHandler.h"

AraRecoHandler::AraRecoHandler() {
	// nothing right now
}

AraRecoHandler::~AraRecoHandler(){
	// nothing right now
}

//! identify hits using sliding V2 SNR definition, and fill delays with AraVertex tool
/*!
	\param araGeom an AraGeomTool tool
	\param Reco an AraVertex tool
	\param station what station are we reconstructing
	\param pol_select what polarization should we be trying to vertex
	\param excluded_channels vector of what channel numbers should be excluded from the reconstruction
	\param waveforms vector of interpolated waveforms (TGraph*)
	\param hitThreshold SNR threshold to be called a "hit"; default is 8.
	\return void
*/
void AraRecoHandler::identifyHitsPrepToVertex(
	AraGeomTool *araGeom, 
	AraVertex *Reco, 
	int station, 
	int pol_select, 
	vector<int> excluded_channels, 
	vector<TGraph*> waveforms, 
	double hitThreshold){

	// clear out AraVertex
	Reco->clear();

	int canUseChan[16]={0};
	double chanLocations[16][3];
	vector<int> polarizations;
	// work out what channels we're allowed to use
	for(int chan=0; chan<16; chan++){
		if(std::find(excluded_channels.begin(), excluded_channels.end(), chan) == excluded_channels.end()){
			canUseChan[chan]=1;
		}
		if(chan<8) polarizations.push_back(0);
		if(chan>7) polarizations.push_back(1);
		for(int coord=0; coord<3; coord++){
			chanLocations[chan][coord] = araGeom->getStationInfo(station)->getAntennaInfo(chan)->getLocationXYZ()[coord];
		}
	}

	// use a function to get the UW-based SNR defintion
	///////////////////////
	///////////////////////

	double interpV = 0.4;
	double interpH = 0.625;
	int nIntSamp_V = int(25./interpV);
	int nIntSamp_H = int(25./interpH);

	float chanSNRs[16];
	float hitTimes[16];
	AraRecoHandler::getChannelSlidingV2SNR_UW(waveforms, nIntSamp_V, nIntSamp_H, chanSNRs, hitTimes);
	// for(int i=0; i<16; i++){
	// 	printf("Chan %d, UW SNR is %.2f, hit time %.2f \n", i, chanSNRs[i], hitTimes[i]);
	// }

	// compute snrs and count hits
	///////////////////////
	///////////////////////

	// now, compute the SNRs and count hits
	int isChanHit[16]={0};
	int nHitsV=0;
	int nHitsH=0;
	for(int chan=0; chan<16; chan++){
		double SNR = chanSNRs[chan];
		if(canUseChan[chan]>0 && SNR>hitThreshold){
			isChanHit[chan]=1;
			// printf("Chan %2d, SNR %4.2f, max %4.2f, RMS %4.2f \n", chan, SNR, maxVolts[chan], chanRMSs[chan]);
			if(polarizations[chan]==0) nHitsV++;
			if(polarizations[chan]==1) nHitsH++;
		}
	}
	int nHitsTotal = nHitsV+nHitsH;

	// cout<<"Print SNRs"<<endl;
	// for(int i=0; i<16; i++){
	// 	printf("%.3f \n", chanSNRs[i]);
	// }


	// figure out which reconstruction mode to run in
	///////////////////////
	///////////////////////

	
	// -1 = "do nothing"
	// 0 = "in v only"
	// 1 = "in h only"
	// 2 = "in both"
	int howToReco=-1;

	if(nHitsTotal>=4){
		// 4 v hits and vpol request, we can do the reco just in v
		if(nHitsV>=4 && pol_select==0){
			howToReco=0;
		}
		// 4 h hits and hpol request, we can do the reco just in h
		else if(nHitsH>=4 && pol_select==1){
			howToReco=1;
		}
		// otherwise, we must need mixed polarization reconstruction
		else{
			howToReco=2;
		}
	}

	// store the channels that are to be used for reconstruction
	///////////////////////
	///////////////////////


	// now, assemble a vector f channels we can use for reconstruction
	vector<int> chansForReco;
	for(int chan=0; chan<16; chan++){
		if(howToReco==0){
			if(polarizations[chan]==0 && isChanHit[chan]>0){
				chansForReco.push_back(chan);
			}
		}
		else if(howToReco==1){
			if(polarizations[chan]==1 && isChanHit[chan]>0){
				chansForReco.push_back(chan);
			}
		}
		else if(howToReco==2){
			if(isChanHit[chan]>0){
				chansForReco.push_back(chan);
			}
		}
	}

	// add the time delay to the Reco object for that pair
	///////////////////////
	///////////////////////

	// now, add the pair and their time delays to the AraVertex object
	for(int i1=0; i1<chansForReco.size(); i1++){
		for(int i2=i1+1; i2<chansForReco.size(); i2++){
			int ch1 = chansForReco[i1];
			int ch2 = chansForReco[i2];
			double dt = hitTimes[ch1] - hitTimes[ch2];
			double x1 = chanLocations[ch1][0];
			double y1 = chanLocations[ch1][1];
			double z1 = chanLocations[ch1][2];
			double x2 = chanLocations[ch2][0];
			double y2 = chanLocations[ch2][1];
			double z2 = chanLocations[ch2][2];
			Reco->addPair(dt, x1, y1, z1, x2, y2, z2);
		}
	}
}

//! gets the maximum bin in a waveform
/*!
	\param gr interpolated waveform
	\return maxBin the maximum bin
*/
int AraRecoHandler::getMaxBin(TGraph *gr){
	double t, v, max;
	max = 0.;
	int maxBin = 0;
	for(int s=0; s<gr->GetN(); s++){
		gr->GetPoint(s, t, v);
		if( fabs(v) > max ){
			max = fabs(v);
			maxBin = s;
		}
	}
	return maxBin;
}

//! Sets the mean and sigma for a waveform in the region where the peak is *not*
/*!
	\param gr interpolated waveform
	\param stats an array to return the mean and sigma value in
	\return void
*/
void AraRecoHandler::setMeanAndSigmaInNoMax(TGraph *gr, double *stats){

	int bin = gr->GetN();
	int MaxBin = AraRecoHandler::getMaxBin(gr);
	int binCounter=0;

	double mean =0;
	double sigma=0;
	double t, v;

	if( MaxBin <= bin/4 ){

		for (int i=MaxBin+bin/4; i<bin; i++){
			gr->GetPoint(i, t, v);
			mean  += v;
			sigma += v * v;
			binCounter++;
		}
	}
	else if( MaxBin >= 3*bin/4 ){

		for (int i=0; i<MaxBin-bin/4; i++){
			gr->GetPoint(i, t, v);
			mean  += v;
			sigma += v * v;
			binCounter++;
		}
	}
	else{

		for (int i=0; i<MaxBin-bin/4; i++){
			gr->GetPoint(i, t, v);
			mean  += v;
			sigma += v * v;
			binCounter++;
		}

		for (int i=MaxBin+bin/4; i<bin; i++){
			gr->GetPoint(i, t, v);
			mean  += v;
			sigma += v * v;
			binCounter++;
		}
	}
	mean  = mean / (double)binCounter;
	sigma = TMath::Sqrt( ( sigma - ((double)binCounter * mean * mean )) / (double)(binCounter - 1) );
	stats[0] = mean;
	stats[1] = sigma;
}


//! Returns the sliding sqrt(V^2) envelope of interpolated waveform
/*!
	\param gr interpolated waveform
	\param nIntSamp the number of samples to be integrated over
	\return grV2Summed the sqrt(integrated V^2) waveform envelope
*/
TGraph* AraRecoHandler::getSqrtVoltageSquaredSummedWaveform(TGraph *gr, int nIntSamp){
	TGraph *grV2Summed = new TGraph();
	double t, v, t0;
	double sum;
	for(int p=0; p<gr->GetN()-nIntSamp; p++){
		sum=0.;
		for(int q=p; q<p+nIntSamp; q++){
			gr->GetPoint(q,t,v);
			sum+=(v*v);
			if(q==p) t0 = t;
		}
		sum /= (double)nIntSamp;
		grV2Summed->SetPoint(p, t0, sqrt(sum));
	}
	return grV2Summed;
}



//! Returns the sliding V^2 SNR peak time as the number of sigmas between the V^2 peak and mean for all channels. This is UW definition of SNR
/*!
	\param interpolatedWaveforms a vector of interpolated waveforms
	\param nIntSamp_V the number of samples to be integrated over for V channels
	\param nIntSamp_H the number of samples to be intergrated over for H channels
	\param snrArray the SNR values for all the channels
	\param hitTimeArray the hit times for all the channels
	\return void
*/
void AraRecoHandler::getChannelSlidingV2SNR_UW(vector<TGraph*> interpolatedWaveforms, int nIntSamp_V, int nIntSamp_H, float *snrArray, float *hitTimeArray){
	double sigma;
	double mean;
	double absPeak;
	double statsArray[2]={0};

	double t, v;
	int bin;

	TGraph *v2Gr = new TGraph();

	for ( int ch=0; ch<(int)interpolatedWaveforms.size(); ch++){

		absPeak = 0.;
		v2Gr = getSqrtVoltageSquaredSummedWaveform(interpolatedWaveforms[ch], (ch<8?nIntSamp_V:nIntSamp_H));

		bin   = v2Gr->GetN();
		AraRecoHandler::setMeanAndSigmaInNoMax(v2Gr, statsArray);

		mean  = statsArray[0];
		sigma = statsArray[1];
		double thishitTime=-9999.;

		for (int binCounter=0; binCounter<bin; binCounter++){
			v2Gr->GetPoint(binCounter, t, v);
			if( fabs(v-mean) > absPeak ){
				absPeak = fabs(v-mean);
				thishitTime = t;
			}
		}//end of binCounter

		if(sigma>0){
			snrArray[ch] = static_cast<float>(absPeak / sigma);
			hitTimeArray[ch] = thishitTime;
		}
		else{
			snrArray[ch] = 0.f;
			hitTimeArray[ch] = -9999.;
		}
		delete v2Gr;
	}//end of ch

}
