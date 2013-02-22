#include <iostream>
#include "AraEventCorrelator.h"
#include "AraGeomTool.h"
#include "AraAntennaInfo.h"
#include "AraStationInfo.h"
#include "UsefulIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "FFTtools.h"
#include "TH2D.h"
#include "TMath.h"

ClassImp(AraEventCorrelator);

AraEventCorrelator * AraEventCorrelator::fgInstance=0;


AraEventCorrelator::AraEventCorrelator(Int_t numAnts, Int_t stationId)
{
  //Default constructor
  fNumAnts=numAnts;
  fNumPairs=0;
  fStationId=stationId;
  for(int first=0;first<(fNumAnts-1);first++) {
    for(int second=first+1;second<fNumAnts;second++) {      
      fFirstAnt[fNumPairs]=first;
      fSecondAnt[fNumPairs]=second;
      fNumPairs++;
    }
  }


  //  fillDeltaTArrays();
  fillAntennaPositions();
  setupDeltaTInfinity();
  setupDeltaT40m();
}

AraEventCorrelator::~AraEventCorrelator()
{
  //Default destructor
}


//______________________________________________________________________________
AraEventCorrelator*  AraEventCorrelator::Instance(Int_t numAnts, Int_t stationId)
{
  //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraEventCorrelator(numAnts, stationId);
  return fgInstance;
}

void AraEventCorrelator::fillAntennaPositions()
{AraGeomTool *araGeom=AraGeomTool::Instance();
  for(int ant=0;ant<ANTS_PER_ICRR;ant++) {   
    int antPolNum=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->antPolNum; 
    std::cerr << ant << "\t" << antPolNum << "\t" << araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->polType << "\n";
    if(araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->polType==AraAntPol::kVertical) {
      if(antPolNum<7) {
	fRfChanVPol[antPolNum]=ant;
	fVPolPos[antPolNum][0]=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->getLocationXYZ()[0];
	fVPolPos[antPolNum][1]=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->getLocationXYZ()[1];
	fVPolPos[antPolNum][2]=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->getLocationXYZ()[2];
	fVPolRho[antPolNum]=TMath::Sqrt(fVPolPos[antPolNum][0]*fVPolPos[antPolNum][0]+
					fVPolPos[antPolNum][1]*fVPolPos[antPolNum][1]);
	fVPolPhi[antPolNum]=TMath::ATan2(fVPolPos[antPolNum][1],fVPolPos[antPolNum][0]);
      }
    }
    if(araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->polType==AraAntPol::kHorizontal) {
      if(antPolNum<7) {
	fRfChanHPol[antPolNum]=ant;
	fHPolPos[antPolNum][0]=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->getLocationXYZ()[0];
	fHPolPos[antPolNum][1]=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->getLocationXYZ()[1];
	fHPolPos[antPolNum][2]=araGeom->getStationInfo(fStationId)->getAntennaInfo(ant)->getLocationXYZ()[2];
	fHPolRho[antPolNum]=TMath::Sqrt(fHPolPos[antPolNum][0]*fHPolPos[antPolNum][0]+
					fHPolPos[antPolNum][1]*fHPolPos[antPolNum][1]);
	fHPolPhi[antPolNum]=TMath::ATan2(fHPolPos[antPolNum][1],fHPolPos[antPolNum][0]);
      }
    }
  }
  for(int i=0;i<7;i++) {
    std::cout << "V\t" << i << "\t" << fVPolPos[i][0] << "\t" << fVPolPos[i][1] << "\t" << fVPolPos[i][2] << "\n";
  }
  for(int i=0;i<7;i++) {
    std::cout << "H\t" << i << "\t" << fHPolPos[i][0] << "\t" << fHPolPos[i][1] << "\t" << fHPolPos[i][2] << "\n";
  }

 //Now fill the arrays with angles
  Double_t deltaPhi=360./NUM_PHI_BINS;
  Double_t deltaTheta=180./NUM_THETA_BINS;
  
  for(int i=0;i<NUM_PHI_BINS;i++) {
    fPhiWaveDeg[i]=-180+0.5*deltaPhi+deltaPhi*i;
    fPhiWave[i]=fPhiWaveDeg[i]*TMath::DegToRad();
  }
  for(int i=0;i<NUM_THETA_BINS;i++) {
    fThetaWaveDeg[i]=-90+0.5*deltaTheta+deltaTheta*i;
    fThetaWave[i]=fThetaWaveDeg[i]*TMath::DegToRad();
  }
}


void AraEventCorrelator::setupDeltaTInfinity() 
{
  
 
  for(int pair=0;pair<fNumPairs;pair++) {
    int ind1=0;
    int ind2=0;
    getPairIndices(pair,ind1,ind2);
    for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
      for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	fVPolDeltaTInfinity[pair][phiBin][thetaBin]=
	  calcDeltaTInfinity(fVPolPos[ind1],fVPolRho[ind1],fVPolPhi[ind1],
			     fVPolPos[ind2],fVPolRho[ind2],fVPolPhi[ind2],
			     fPhiWave[phiBin],fThetaWave[thetaBin]);
	fHPolDeltaTInfinity[pair][phiBin][thetaBin]=
	  calcDeltaTInfinity(fHPolPos[ind1],fHPolRho[ind1],fHPolPhi[ind1],
			     fHPolPos[ind2],fHPolRho[ind2],fHPolPhi[ind2],
			     fPhiWave[phiBin],fThetaWave[thetaBin]);	
      }
    }
  }
  
}


void AraEventCorrelator::setupDeltaT40m() 
{
  Double_t R=41.8;
 
  for(int pair=0;pair<fNumPairs;pair++) {
    int ind1=0;
    int ind2=0;
    getPairIndices(pair,ind1,ind2);
    //    std::cout << "Starting pair " << pair << "\t" << ind1 << "\t" << ind2 << "\n";
    for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
      for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	fVPolDeltaT40m[pair][phiBin][thetaBin]=
	  calcDeltaTR(fVPolPos[ind1],fVPolRho[ind1],fVPolPhi[ind1],
		      fVPolPos[ind2],fVPolRho[ind2],fVPolPhi[ind2],
		      fPhiWave[phiBin],fThetaWave[thetaBin],R);
	fHPolDeltaT40m[pair][phiBin][thetaBin]=
	  calcDeltaTR(fHPolPos[ind1],fHPolRho[ind1],fHPolPhi[ind1],
		      fHPolPos[ind2],fHPolRho[ind2],fHPolPhi[ind2],
		      fPhiWave[phiBin],fThetaWave[thetaBin],R);	
      }
    }
  }
  
}

Double_t AraEventCorrelator::calcDeltaTInfinity(Double_t ant1[3],Double_t rho1, Double_t phi1, Double_t ant2[3],Double_t rho2, Double_t phi2, Double_t phiWave, Double_t thetaWave)
{

  Double_t d1=TMath::Cos(thetaWave)*(ant1[2]*TMath::Tan(thetaWave)+rho1*TMath::Cos(phi1-phiWave));
  Double_t d2=TMath::Cos(thetaWave)*(ant2[2]*TMath::Tan(thetaWave)+rho2*TMath::Cos(phi2-phiWave));
  Double_t t1t2=(d2-d1)*AraGeomTool::nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;
  //   Double_t t2t1=(d1-d2)*AraGeomTool::nTopOfIce/TMath::C();
  //   t2t1*=1e9;
  //   return t2t1;
}

Double_t AraEventCorrelator::calcDeltaTR(Double_t ant1[3],Double_t rho1, Double_t phi1, Double_t ant2[3],Double_t rho2, Double_t phi2, Double_t phiWave, Double_t thetaWave, Double_t R)
{

  Double_t xs=R*TMath::Cos(thetaWave)*TMath::Cos(phiWave);
  Double_t ys=R*TMath::Cos(thetaWave)*TMath::Sin(phiWave);
  Double_t zs=R*TMath::Sin(thetaWave);

  //  if(phiWave*TMath::RadToDeg()>30 && phiWave*TMath::RadToDeg()<31) {
  //      if(thetaWave*TMath::RadToDeg()>-45 && thetaWave*TMath::RadToDeg()<-44) {
	//      std::cout << phiWave*TMath::RadToDeg() << "\t" << thetaWave*TMath::RadToDeg() << "\t" << R << "\t" << xs << "\t" << ys << "\t" << zs << "\n";    
  //      }
  //    }
  
  Double_t d1=TMath::Sqrt((xs-ant1[0])*(xs-ant1[0])+(ys-ant1[1])*(ys-ant1[1])+(zs-ant1[2])*(zs-ant1[2]));
  Double_t d2=TMath::Sqrt((xs-ant2[0])*(xs-ant2[0])+(ys-ant2[1])*(ys-ant2[1])+(zs-ant2[2])*(zs-ant2[2]));
    
    
  //    if(phiWave*TMath::RadToDeg()>30 && phiWave*TMath::RadToDeg()<31) {
  //	if(thetaWave*TMath::RadToDeg()>-45 && thetaWave*TMath::RadToDeg()<-44) {
  //	  std::cout << d1 << "\t" << d2 << "\t" << d1-d2 << "\n";	
  //	}
  //      }

     
  Double_t t1t2=(d1-d2)*AraGeomTool::nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;
}

void AraEventCorrelator::fillDeltaTArrays(AraCorrelatorType::AraCorrelatorType_t corType) {
  static AraCorrelatorType::AraCorrelatorType_t lastCorType=AraCorrelatorType::kNotACorType;
  if(lastCorType==corType) return;
  lastCorType=corType;

  switch (corType) {
  case AraCorrelatorType::kSphericalDist40:    
      for(int pair=0;pair<fNumPairs;pair++) {	
	for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
	  for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	    fVPolDeltaT[pair][phiBin][thetaBin]=fVPolDeltaT40m[pair][phiBin][thetaBin];
	    fHPolDeltaT[pair][phiBin][thetaBin]=fHPolDeltaT40m[pair][phiBin][thetaBin];
	  }
	}
      }
      break;
  case AraCorrelatorType::kPlaneWave:
  default:
      for(int pair=0;pair<fNumPairs;pair++) {	
	for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
	  for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	    fVPolDeltaT[pair][phiBin][thetaBin]=fVPolDeltaTInfinity[pair][phiBin][thetaBin];
	    fHPolDeltaT[pair][phiBin][thetaBin]=fHPolDeltaTInfinity[pair][phiBin][thetaBin];
	  }
	}
      }
      break;
  }
}
    


void AraEventCorrelator::getPairIndices(int pair, int &ant1, int &ant2)
{

  if(pair>=0 && pair<fNumPairs) {
    ant1=fFirstAnt[pair];
    ant2=fSecondAnt[pair];
  }    
}

TH2D *AraEventCorrelator::getInterferometricMap(UsefulIcrrStationEvent *evPtr, AraAntPol::AraAntPol_t polType,AraCorrelatorType::AraCorrelatorType_t corType)
{
  static int counter=0;
  fillDeltaTArrays(corType);
  //Now need to get correlations for the fNumPairs antenna pairs and then look up the correlation values of each one
  Double_t scale=1./fNumPairs;
  TH2D *histMap = new TH2D("histMap","histMap",NUM_PHI_BINS,-180,180,NUM_THETA_BINS,-90,90);
  TGraph *grRaw[fNumAnts];
  TGraph *grInt[fNumAnts];
  TGraph *grNorm[fNumAnts];
  TGraph *grCor[fNumPairs];
  for(int i=0;i<fNumAnts;i++) {
    grRaw[i]=0;
    grInt[i]=0;
    grNorm[i]=0;
  }
  for(int i=0;i<fNumPairs;i++) 
    grCor[i]=0;

  if(polType==AraAntPol::kVertical) {
    for(int ind=0;ind<fNumAnts;ind++) {
      //      std::cerr << ind << "\t" << fRfChanVPol[ind] << "\n";
      grRaw[ind]=evPtr->getGraphFromRFChan(fRfChanVPol[ind]);
      grInt[ind]=FFTtools::getInterpolatedGraph(grRaw[ind],0.5);
      grNorm[ind]=getNormalisedGraph(grInt[ind]);
    }
    //    std::cerr << "Got graphs and made int maps\n";

    for(int pair=0;pair<fNumPairs;pair++) {
      int ind1=0;
      int ind2=0;
      getPairIndices(pair,ind1,ind2);
      //      std::cerr << pair << "\t" << ind1 << "\t" << ind2 << "\n";

      grCor[pair]=FFTtools::getCorrelationGraph(grNorm[ind1],grNorm[ind2]);      
      for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
	for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	  //I think this is the correct equation to work out the bin number
	  //Could just use TH2::GetBin(binx,biny) but the below should be faster
	  Int_t globalBin=(phiBin+1)+(thetaBin+1)*(NUM_PHI_BINS+2);
	  Double_t dt=fVPolDeltaT[pair][phiBin][thetaBin];
	  //Double_t corVal=grCor[pair]->Eval(dt);
	  Double_t corVal=fastEvalForEvenSampling(grCor[pair],dt);
	  corVal*=scale;
	  Double_t binVal=histMap->GetBinContent(globalBin);
	  histMap->SetBinContent(globalBin,binVal+corVal);
	}
      }      
    }
  }
  else {
    for(int ind=0;ind<fNumAnts;ind++) {
      grRaw[ind]=evPtr->getGraphFromRFChan(fRfChanHPol[ind]);
      grInt[ind]=FFTtools::getInterpolatedGraph(grRaw[ind],0.5);
      grNorm[ind]=getNormalisedGraph(grInt[ind]);
    }

    for(int pair=0;pair<fNumPairs;pair++) {
      int ind1=0;
      int ind2=0;
      getPairIndices(pair,ind1,ind2);
      grCor[pair]=FFTtools::getCorrelationGraph(grNorm[ind1],grNorm[ind2]);      
      for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
	for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	  //I think this is the correct equation to work out the bin number
	  //Could just use TH2::GetBin(binx,biny) but the below should be faster
	  Int_t globalBin=(phiBin+1)+(thetaBin+1)*(NUM_PHI_BINS+2);
	  Double_t dt=fHPolDeltaT[pair][phiBin][thetaBin];
	  //	  Double_t corVal=grCor[pair]->Eval(dt);
	  Double_t corVal=fastEvalForEvenSampling(grCor[pair],dt);
	  corVal*=scale;
	  Double_t binVal=histMap->GetBinContent(globalBin);
	  histMap->SetBinContent(globalBin,binVal+corVal);
	}
      }      
    }
 }
  if(fDebugMode) {
    char histName[180];
    for(int i=0;i<fNumAnts;i++) {
      sprintf(histName,"grRaw%d_%d",i,counter);
      grRaw[i]->SetName(histName);
      grRaw[i]->SetTitle(histName);
      grRaw[i]->Write();
      sprintf(histName,"grInt%d_%d",i,counter);
      grInt[i]->SetName(histName);
      grInt[i]->SetTitle(histName);
      grInt[i]->Write();
      sprintf(histName,"grNorm%d_%d",i,counter);
      grNorm[i]->SetName(histName);
      grNorm[i]->SetTitle(histName);
      grNorm[i]->Write();
    }
    for(int i=0;i<fNumPairs;i++) {
      sprintf(histName,"grCor%d_%d",i,counter);
      grCor[i]->SetName(histName);
      grCor[i]->SetTitle(histName);
      grCor[i]->Write();
    }
    counter++;
  }
  for(int i=0;i<fNumAnts;i++) {
    if(grRaw[i]) delete grRaw[i];
    if(grInt[i]) delete grInt[i];
    if(grNorm[i]) delete grNorm[i];
  }
  for(int i=0;i<fNumPairs;i++) {
    if(grCor[i]) delete grCor[i];
  }
  //Moved the scaling to earlier for optimisation reasons
  //  histMap->Scale(scale);
  return histMap;
}

TH2D *AraEventCorrelator::getInterferometricMap(UsefulAtriStationEvent *evPtr, AraAntPol::AraAntPol_t polType,AraCorrelatorType::AraCorrelatorType_t corType)
{
  static int counter=0;
  fillDeltaTArrays(corType);
  //Now need to get correlations for the fNumPairs antenna pairs and then look up the correlation values of each one
  Double_t scale=1./fNumPairs;
  TH2D *histMap = new TH2D("histMap","histMap",NUM_PHI_BINS,-180,180,NUM_THETA_BINS,-90,90);
  TGraph *grRaw[fNumAnts];
  TGraph *grInt[fNumAnts];
  TGraph *grNorm[fNumAnts];
  TGraph *grCor[fNumPairs];
  for(int i=0;i<fNumAnts;i++) {
    grRaw[i]=0;
    grInt[i]=0;
    grNorm[i]=0;
  }
  for(int i=0;i<fNumPairs;i++) 
    grCor[i]=0;

  if(polType==AraAntPol::kVertical) {
    for(int ind=0;ind<fNumAnts;ind++) {
      //      std::cerr << ind << "\t" << fRfChanVPol[ind] << "\n";
      grRaw[ind]=evPtr->getGraphFromRFChan(fRfChanVPol[ind]);
      grInt[ind]=FFTtools::getInterpolatedGraph(grRaw[ind],0.5);
      grNorm[ind]=getNormalisedGraph(grInt[ind]);
    }
    //    std::cerr << "Got graphs and made int maps\n";

    for(int pair=0;pair<fNumPairs;pair++) {
      int ind1=0;
      int ind2=0;
      getPairIndices(pair,ind1,ind2);
      //      std::cerr << pair << "\t" << ind1 << "\t" << ind2 << "\n";

      grCor[pair]=FFTtools::getCorrelationGraph(grNorm[ind1],grNorm[ind2]);      
      for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
	for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	  //I think this is the correct equation to work out the bin number
	  //Could just use TH2::GetBin(binx,biny) but the below should be faster
	  Int_t globalBin=(phiBin+1)+(thetaBin+1)*(NUM_PHI_BINS+2);
	  Double_t dt=fVPolDeltaT[pair][phiBin][thetaBin];
	  //Double_t corVal=grCor[pair]->Eval(dt);
	  Double_t corVal=fastEvalForEvenSampling(grCor[pair],dt);
	  corVal*=scale;
	  Double_t binVal=histMap->GetBinContent(globalBin);
	  histMap->SetBinContent(globalBin,binVal+corVal);
	}
      }      
    }
  }
  else {
    for(int ind=0;ind<fNumAnts;ind++) {
      grRaw[ind]=evPtr->getGraphFromRFChan(fRfChanHPol[ind]);
      grInt[ind]=FFTtools::getInterpolatedGraph(grRaw[ind],0.5);
      grNorm[ind]=getNormalisedGraph(grInt[ind]);
    }

    for(int pair=0;pair<fNumPairs;pair++) {
      int ind1=0;
      int ind2=0;
      getPairIndices(pair,ind1,ind2);
      grCor[pair]=FFTtools::getCorrelationGraph(grNorm[ind1],grNorm[ind2]);      
      for(int phiBin=0;phiBin<NUM_PHI_BINS;phiBin++) {
	for(int thetaBin=0;thetaBin<NUM_THETA_BINS;thetaBin++) {
	  //I think this is the correct equation to work out the bin number
	  //Could just use TH2::GetBin(binx,biny) but the below should be faster
	  Int_t globalBin=(phiBin+1)+(thetaBin+1)*(NUM_PHI_BINS+2);
	  Double_t dt=fHPolDeltaT[pair][phiBin][thetaBin];
	  //	  Double_t corVal=grCor[pair]->Eval(dt);
	  Double_t corVal=fastEvalForEvenSampling(grCor[pair],dt);
	  corVal*=scale;
	  Double_t binVal=histMap->GetBinContent(globalBin);
	  histMap->SetBinContent(globalBin,binVal+corVal);
	}
      }      
    }
 }
  if(fDebugMode) {
    char histName[180];
    for(int i=0;i<fNumAnts;i++) {
      sprintf(histName,"grRaw%d_%d",i,counter);
      grRaw[i]->SetName(histName);
      grRaw[i]->SetTitle(histName);
      grRaw[i]->Write();
      sprintf(histName,"grInt%d_%d",i,counter);
      grInt[i]->SetName(histName);
      grInt[i]->SetTitle(histName);
      grInt[i]->Write();
      sprintf(histName,"grNorm%d_%d",i,counter);
      grNorm[i]->SetName(histName);
      grNorm[i]->SetTitle(histName);
      grNorm[i]->Write();
    }
    for(int i=0;i<fNumPairs;i++) {
      sprintf(histName,"grCor%d_%d",i,counter);
      grCor[i]->SetName(histName);
      grCor[i]->SetTitle(histName);
      grCor[i]->Write();
    }
    counter++;
  }
  for(int i=0;i<fNumAnts;i++) {
    if(grRaw[i]) delete grRaw[i];
    if(grInt[i]) delete grInt[i];
    if(grNorm[i]) delete grNorm[i];
  }
  for(int i=0;i<fNumPairs;i++) {
    if(grCor[i]) delete grCor[i];
  }
  //Moved the scaling to earlier for optimisation reasons
  //  histMap->Scale(scale);
  return histMap;
}

TGraph* AraEventCorrelator::getNormalisedGraph(TGraph *grIn)
{
  Double_t rms=grIn->GetRMS(2);
  Double_t mean=grIn->GetMean(2);
  Double_t *xVals = grIn->GetX();
  Double_t *yVals = grIn->GetY();
  Int_t numPoints = grIn->GetN();
  Double_t *newY = new Double_t [numPoints];
  for(int i=0;i<numPoints;i++) {
    newY[i]=(yVals[i]-mean)/rms;
  }
  TGraph *grOut = new TGraph(numPoints,xVals,newY);
  delete [] newY;
  return grOut;
}

Double_t AraEventCorrelator::fastEvalForEvenSampling(TGraph *grIn, Double_t xvalue)
{
  Int_t numPoints=grIn->GetN();
  if(numPoints<2) return 0;
  Double_t *xVals=grIn->GetX();
  Double_t *yVals=grIn->GetY();
  Double_t dx=xVals[1]-xVals[0];
  if(dx<=0) return 0;

  Int_t p0=Int_t((xvalue-xVals[0])/dx);
  if(p0<0) p0=0;
  if(p0>=numPoints) p0=numPoints-2;
  return FFTtools::simpleInterploate(xVals[p0],yVals[p0],xVals[p0+1],yVals[p0+1],xvalue);
}
