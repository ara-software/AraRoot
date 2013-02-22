#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <AraGeomTool.h>
#include "AraVertex.h"
#include "FFTtools.h"
#include <cmath>
//#include <cmath.h>
//using namespace std;
#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TArrow.h"

#include "TGraph.h"
#include "TCanvas.h"
#include "L2.h"
#include "araIcrrDefines.h"

//ClassImp(L2);
TGraph * getNormalisedGraph(TGraph *grIn);

L2::L2(int runNumber_,AraGeomTool *geometryInfo) {
  araGeom=geometryInfo;
  Reco=new AraVertex(); 
  // Prepare tree:
  printf ("here \n");


  L2File = TFile::Open(Form("L2_%d.root",runNumber_),"Recreate");
  L2GeoTree= new TTree("L2GeoTree","Geometry");
  L2EventTree = new TTree("L2EventTree","Event tree");
  L2RunTree= new TTree("L2RunTree","Run header tree");
  L2RunTree->Branch("run",&runheader,"runNumber/I:stationId/I:startTime/i:startDOY/I:startHour/I:startMinute/I:startSecond/I:startDOW/I:endTime/i:endDOY/I:endHour/I:endMinute/I:endSecond/I:endDOW/I:duration/i:errorFlag/I:runType/I:NEvents/I:NRF/I:NPulser/I:NForced/I:NunClassified/I:Nall/I");  
  //  L2RunTree->Branch("geometry",);
  //L2GeoTree->Branch("Geo",&antenna,"stationId/I:channelId/I:X/D:Y/D:Z/D:highPass/D:lowPass/D:cableDelay/D:pol/I:type/I:orient/I:noise/D");
  L2GeoTree->Branch("Geo",&antenna,"stationId/I:channelId/I:X/D:Y/D:Z/D:highPass/D:lowPass/D:cableDelay/D:pol/I:type/I:orient[3]/D:noise/D");

  L2EventTree->Branch("trigger",&trigger,"RbClock/D:deadTime/D:triggerType/I:eventType/I:triggerPattern/I");
  L2EventTree->Branch("hk",&hk,"temperature[8]/D:RFPower[16]/D:sclGlobal/s:sclL1[12]/s:scl[24]/s");
  L2EventTree->Branch("header",&header,"runNumber/i:stationId/i:unixTime/i:DOY/I:hour/I:minute/I:second/I:DOW/I:unixTimeUsec/i:eventNumber/i:gpsSubTime/I:calibStatus/s:priority/b:errorFlag/b");
  L2EventTree->Branch("wf",&wf,Form("mean[16]/D:rms[16]/D:v2[16]/D:power[16]/D:maxV[16]/D:freqMax[16]/F:freqMaxRatio[16]/F:powerBin[16][%d]/F:inTrigPattern[16]/i:trackR/D:trackPhi/D:trackTheta/D",N_POWER_BINS));

  TString RecoBranch="X/D:Y/D:Z/D:R/D:theta/D:phi/D:dX/D:dY/D:dZ/D:chisq/D:trackR/D:trackPhi/D:trackTheta/D:EDM/D:nhits/I:status/I:dt[32]/D";
  L2EventTree->Branch("RecoVMax",&recoVmax,RecoBranch);
  L2EventTree->Branch("RecoHMax",&recoHmax,RecoBranch);
  // L2EventTree->Branch("RecoAllMax",&recoAllmax,RecoBranch);
  //  L2EventTree->Branch("RecoTrigmax",&recoTrigmax,RecoBranch);

  L2EventTree->Branch("RecoVxcor",&recoVxcor,RecoBranch);
  L2EventTree->Branch("RecoHxcor",&recoHxcor,RecoBranch);
  //  L2EventTree->Branch("RecoAllxcor",&recoAllxcor,RecoBranch);
  L2EventTree->Branch("RecoVxcorTrack",&recoVxcorSimple,RecoBranch);
  L2EventTree->Branch("RecoHxcorTrack",&recoHxcorSimple,RecoBranch);

  

  //  L2EventTree->Branch("RecoTrigxcor",&recoTrigxcor,RecoBranch);




  // Fill in run specific parameters
  //runheader=new RUNHEADER();

  runheader.RunNumber=runNumber_;
 

  // init variables for event counting:
  isFirstEvent=1;
  CountBdt=0;
  CountAdt=0;
  startT=0;
  endT=0;
  Adt=60;
  Bdt=600;
  for (int i=0; i<16; i++) LastForcedRMS[i]=0;
//  EventCounter={0,0,0,0};
  memset(EventCounter, 0, 4*sizeof(int));

  sigReco1=3;
  sigReco2=4;
  //L2EventTree->Branch("events",&stam,"stam1/I:stam2/I");


  firstEvent=0;
  lastEvent=0;
} 

L2::~L2() {
  //Save();
}

int L2::FillGeoTree() {
  
   for (int station=0; station<ICRR_NO_STATIONS; station++) {
      for(int ant=0;ant<ANTS_PER_ICRR;ant++) {   
	antenna.stationId=station;
	antenna.channelId=ant;
	antenna.Location[0]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->getLocationXYZ()[0];
	antenna.Location[1]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->getLocationXYZ()[1];
	antenna.Location[2]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->getLocationXYZ()[2];
	antenna.HighPassFilter=araGeom->getStationInfo(station)->getAntennaInfo(ant)->highPassFilterMhz;
	antenna.LowPassFilter=araGeom->getStationInfo(station)->getAntennaInfo(ant)->lowPassFilterMhz;
	antenna.cableDelay=araGeom->getStationInfo(station)->getAntennaInfo(ant)->cableDelay;
	antenna.antPol=araGeom->getStationInfo(station)->getAntennaInfo(ant)->polType; 
	//	antenna.antDirection=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antDir; 
	antenna.antType=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antType; 
	//antenna.antOrient=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient;

	// antenna.antOrient[0]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient;
	// antenna.antOrient[1]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient;
	// antenna.antOrient[2]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient;
	
	antenna.antOrient[0]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient[0];
	antenna.antOrient[1]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient[1];
	antenna.antOrient[2]=araGeom->getStationInfo(station)->getAntennaInfo(ant)->antOrient[2]; 
	antenna.averageNoiseFigure=araGeom->getStationInfo(station)->getAntennaInfo(ant)->avgNoiseFigure;       	     L2GeoTree->Fill();
	


	//	if (antenna.antPol ==0 && antenna.Location[2]<-5 ) InIceV.push_back(ant);
	//if (antenna.antPol ==1 && antenna.Location[2]<-5 ) InIceH.push_back(ant);
	//if ((antenna.antPol ==1 || antenna.antPol ==0) && antenna.Location[2]<-5 ) InIceAll.push_back(ant);
      }
      //      printf("Number of vertical antennas=%d,  hor=%d both=%d \n",InIceV.size(), InIceH.size(),InIceAll.size());
   }
  L2File->cd();
  L2GeoTree->Write();
  return(0);
}

void L2::AutoSave() {
  L2EventTree->AutoSave(); 

}
void L2::Save() {
  
  endT=header.unixTime.epoch;
    printf("From %d to %d dt= %d \n",endT,startT,endT-startT);

  L2File->cd();
  // Int_t RunNumber;
  runheader.stationId=header.stationId;
  runheader.RunStartTime=TIMESTAMP(startT);
  runheader.RunEndTime=TIMESTAMP(endT);
  runheader.RunDuration=endT-startT;
  runheader.ErrorFlag=0;
  runheader.RunType=0;
  runheader.NumberOfEvents=EventCounter[0]+EventCounter[1]+EventCounter[2]+EventCounter[3];
  runheader.NumberOfRFEvents=EventCounter[1];
  runheader.NumberOfPulserEvents=EventCounter[3];
  runheader.NumberOfForcedEvents=EventCounter[2];
  runheader.NumberOfUnknownEvents=EventCounter[0];
  runheader.Nall=lastEvent-firstEvent;
  printf("Time: %d \n", runheader.RunStartTime.epoch);
  double one=1;
  double three=3;

  //  L2EventTree->GetUserInfo()->Add(runheader);
  //  L2EventTree->GetUserInfo()->Add(three);

  L2RunTree->Fill();
  
  L2RunTree->Write(); 
  printf("writing 1 \n");
  L2EventTree->Write();




  L2File->Close();


}



int L2::FillEvent(UsefulIcrrStationEvent *event0) {

  event=event0;
  if (isFirstEvent) {
    isFirstEvent=0;
    startT=event->head.unixTime;
    firstEvent=event->head.eventNumber;
  }

  
  if (event->getStationId()==0x00) {Station=0;} //cout<<"testbed \n";
  if (event->getStationId()==0x01) {Station=1;}; //cout<<"ara1 \n";


  InIceAll.clear();
  InIceH.clear();
  InIceV.clear();
  InIceTrig.clear();


  double Station_COG_X=0;
  double Station_COG_Y=0;
  double Station_COG_Z=0;

  
  for(int ant=0;ant<ANTS_PER_ICRR;ant++) {   
    double z=araGeom->getStationInfo(Station)->getAntennaInfo(ant)->getLocationXYZ()[2];
    double p=araGeom->getStationInfo(Station)->getAntennaInfo(ant)->polType; 
    if (p ==0 && z < -5 && ant<8) {InIceV.push_back(ant);}
    if (p ==1 && z < -5 && ant<8) {InIceH.push_back(ant);}
    //   if (p ==0 && z < -5) {InIceV.push_back(ant);}
    //if (p ==1 && z < -5) {InIceH.push_back(ant);}
    if ((p ==1 || p ==0) && z<-5 ) {
      InIceAll.push_back(ant);
      Station_COG_X+=araGeom->getStationInfo(Station)->getAntennaInfo(ant)->getLocationXYZ()[0];
      Station_COG_Y+=araGeom->getStationInfo(Station)->getAntennaInfo(ant)->getLocationXYZ()[1];
      Station_COG_Z+=araGeom->getStationInfo(Station)->getAntennaInfo(ant)->getLocationXYZ()[2];      
    }
    // cout<<"ch:"<<ant<<"  trig="<<(event->trig.isInTrigPattern(ant))<<endl;
    if (z<-5 && (event->trig.isInTrigPattern(ant)==1)   ) {InIceTrig.push_back(ant); cout<<"In!\n";} 
  }
  Station_COG_X =  Station_COG_X / InIceAll.size();
  Station_COG_Y =  Station_COG_Y / InIceAll.size();
  Station_COG_Z =  Station_COG_Z / InIceAll.size();
  Reco->SetCOG(Station_COG_X,Station_COG_Y,Station_COG_Z);
  // printf (" COG for this station : %f %f %f \n", Station_COG_X, Station_COG_Y, Station_COG_Z);

  //      printf("Number of vertical antennas=%d,  hor=%d both=%d \n",InIceV.size(), InIceH.size(),InIceAll.size());
  lastEvent=event->head.eventNumber;

//  TCanvas *c1=new TCanvas("c1","c1",1000,600);
  trigger.TriggerType = (Int_t) event->trig.trigType;
  trigger.TriggerPattern = (Int_t) event->trig.trigPattern;
  trigger.RbClock = (Double_t) event->trig.getRubidiumTriggerTimeInSec();
  trigger.DeadTime = (Double_t) event->trig.getDeadtime();
//  printf ("Rb clock is %f \n",trigger.RbClock);
  trigger.EventType=0; // unknown
  if (trigger.TriggerType==68) trigger.EventType=2;  // Forced
  else if (fabs(trigger.RbClock-20.7e-6)<1e-7) trigger.EventType=3; // Pulser
  else if (trigger.TriggerType==1) trigger.EventType=1;  // RF
  EventCounter[trigger.EventType]++;
  if (trigger.EventType==3) printf (" Pulser Pulser \n");
  for (int ch=0; ch<8; ch++) hk.temperature[ch]=event->hk.getTemperature(ch);
  for (int ch=0; ch<8; ch++) hk.RFPower[ch]=event->hk.getRFPowerBatwing(ch);
  for (int ch=0; ch<8; ch++) hk.RFPower[ch+8]=event->hk.getRFPowerDiscone(ch);
  hk.sclGlobal=hk.sclGlobal;
  for (int i=0; i<12; i++) hk.sclL1[i]=event->hk.sclTrigL1[i];
  for (int ch=0; ch<8; ch++) {hk.scl[ch]=event->hk.sclBatMinus[ch]; hk.scl[ch+8]=event->hk.sclBatPlus[ch]; hk.scl[ch+16]=event->hk.sclDiscone[ch];}
  header.unixTime=TIMESTAMP(event->head.unixTime);
  header.unixTimeusec=event->head.unixTimeUs;
  header.eventNumber=event->head.eventNumber;
  //cout<<"Number="<<header.eventNumber<<" "<<event->head.eventNumber<<endl;
  header.gpsSubTime=event->head.gpsSubTime;
  header.calibStatus=event->head.calibStatus;
  header.priority=event->head.priority;
  header.errorFlag=event->head.errorFlag;
  header.RunNumber= runheader.RunNumber;
  header.stationId=Station;
  //cout<<"reco 1:"<<endl;
  //if (trigger.EventType==3){cout<<"reco\n";


  //  if (minMethod==1)  return(Reco->doPairFitSpherical());    
  //if (minMethod==0)  return(Reco->doPairFit());    
  
  FilldTPairs(InIceV,1);  recoVxcor=Reco->doPairFit();
  FilldTPairs(InIceH,1);  recoHxcor=Reco->doPairFit();
  FilldTPairs(InIceV,0);  recoVmax=Reco->doPairFit();
  FilldTPairs(InIceH,0);  recoHmax=Reco->doPairFit();
  FilldTPairs(InIceV,1);  recoVxcorSimple=Reco->doPairFitSpherical();
  FilldTPairs(InIceH,1);  recoHxcorSimple=Reco->doPairFitSpherical();
  FilldTPairs(InIceAll,0); 
  TVector3 tv=Reco->getVtrack(); 
  // printf ("track %f %f %f \n",tv.Mag(), tv.Theta(),tv.Phi());
  wf.trackR=tv.Mag();
  wf.trackPhi=tv.Phi();
  wf.trackTheta=tv.Theta();
  /*  recoVmax=DoReconstruction(InIceV, 0,0) ;


  recoVxcor=Reco->FilldTPairs(InIceV,1);  
  recoVxcor=Reco->doPairFit();
DoReconstruction(InIceV, 1,0) ;
  recoVmax=DoReconstruction(InIceV, 0,0) ;
  //  recoVmax=DoReconstruction(InIceV, 0,1) ;
  recoHmax=DoReconstruction(InIceH, 0,0) ; //}
  //cout<<"reco 2:"<<endl;
  //  if (trigger.EventType==3){cout<<"reco\n";
  recoHxcor=DoReconstruction(InIceH, 1,0) ;//}
  //cout<<"reco 3:"<<endl;
  // recoAllmax=DoReconstruction(InIceAll, 0,0) ;
  // recoTrigmax=DoReconstruction(InIceTrig,0);
 recoVxcorSimple=DoReconstruction(InIceV, 1,1) ; //}
 recoHxcorSimple=DoReconstruction(InIceH, 1,1) ; //}

  */
 // recoAllxcor=DoReconstruction(InIceAll, 1,0) ;
  //recoTrigxcor=DoReconstruction(InIceTrig,1);
  TH1D *histFFTPower = new TH1D("histFFTPower","histFFTPower",N_POWER_BINS, FREQ_POWER_MIN - ((FREQ_POWER_MAX - FREQ_POWER_MIN)/N_POWER_BINS/2.), FREQ_POWER_MAX + ((FREQ_POWER_MAX - FREQ_POWER_MIN)/N_POWER_BINS/2.));
  TH1D *histFFTPowerLow = new TH1D("histFFTPowerLow","histFFTPowerLow",N_POWER_BINS_L, FREQ_POWER_MIN_L - ((FREQ_POWER_MAX_L - FREQ_POWER_MIN_L)/N_POWER_BINS_L/2.), FREQ_POWER_MAX_L + ((FREQ_POWER_MAX_L - FREQ_POWER_MIN_L)/N_POWER_BINS_L/2.));

  for (int ch=0; ch<16; ch++) {         
    TGraph *gWF = event->getGraphFromRFChan(ch);
    // Fill histogram with total power in freq bins for the lower frequencies.
    double totPower=fillFFTHistoForRFChanL2(ch, histFFTPower);
    double totPowerLow=fillFFTHistoForRFChanL2(ch, histFFTPowerLow);
  
    wf.power[ch]=totPower;
    wf.freqMax[ch]=(Float_t) histFFTPower->GetBinCenter(histFFTPower->GetMaximumBin());
    //    wf.freqMaxVal[ch]=(Float_t) histFFTPower->GetBinContent(histFFTPower->GetMaximumBin()) / totPower;
    wf.freqMaxVal[ch]=(Float_t) pow(10,histFFTPower->GetBinContent(histFFTPower->GetMaximumBin())/10.) / pow(10,totPower/10.);


    for (int b=1; b<N_POWER_BINS_L+1; b++){ wf.powerBin[ch][b-1]=(Float_t) histFFTPowerLow->GetBinContent(b);}
    //    cout<<ch<<" Max at: "<<histFFTPower->GetMaximumBin()<<"  val="<< histFFTPower->GetBinContent(histFFTPower->GetMaximumBin())<<endl;


    double fInterp=0.5 ; // Interpolation factor
    TGraph *gVt0;
    if (gWF->GetN() > 10) gVt0=FFTtools::getInterpolatedGraph(gWF,fInterp); 
    else 
      gVt0=gWF;
    wf.v2[ch]=FFTtools::integrateVoltageSquared(gWF,-1,-1);
    //cout<<"Power in="<<wf.v2[ch]<<"\t"<< wf.power[ch]<<endl;
    wf.maxV[ch]=0;double *wfV=gWF->GetY();
    for (int iy=0; iy<gWF->GetN(); iy++) {if (fabs(wfV[iy])>wf.maxV[ch]) wf.maxV[ch] = fabs(wfV[iy]);}
    wf.mean[ch]=gWF->GetMean(2);
    wf.rms[ch]=gWF->GetRMS(2);
    if (LastForcedRMS[ch]==0 || trigger.TriggerType==68) LastForcedRMS[ch]=wf.rms[ch];
    wf.isInTrigPattern[ch]=event->trig.isInTrigPattern(ch);    


//    printf ("Channel=%d, pol=%d \n",ch,(araGeom->getStationInfo(Station)->fAntInfo[ch].polType));

    delete gVt0;
    delete gWF;
  }
  delete histFFTPowerLow;
  delete histFFTPower;
  

  //printf ("Values= %d %d %f%f \n",trigger.TriggerType, trigger.TriggerPattern, trigger.RbClock, trigger.DeadTime);
  //  printf ("power=%f temp=%f power=%f\n",event->hk.getRFPowerDiscone(2),  event->hk.getTemperature(0), event->hk.getRFPowerBatwing(2));
  L2EventTree->Fill();
  return(0);
  
}


/*
RECOOUT  L2::DoReconstruction(vector<Int_t> chList, Int_t method) {
  RECOOUT a;
  AraVertex *Reco=new AraVertex();
  delete  Reco;
  //  Reco->Delete();
  return (a);
}
*/
/*
RECOOUT  L2::DoReconstruction(vector<Int_t> chList, Int_t method, Int_t minMethod) {
  // Memory leak is here
  double dt;
  Reco->clear();
  //Reco=new AraVertex();   // <=== memoory leak is here
  for (int i1=0; i1<(int) chList.size(); i1++) {
    for (int i2=i1+1; i2<(int) chList.size(); i2++) {
      int ch1=chList[i1];
      int ch2=chList[i2];
      dt=0;
      //cout<<"get diff \n";
      dt=getTimeDiff(chList[i1],chList[i2],method);
      double x1=araGeom->getStationInfo(Station)->getAntennaInfo(ch1)->getLocationXYZ()[0];
      double y1=araGeom->getStationInfo(Station)->getAntennaInfo(ch1)->getLocationXYZ()[1];
      double z1=araGeom->getStationInfo(Station)->getAntennaInfo(ch1)->getLocationXYZ()[2];
      double x2=araGeom->getStationInfo(Station)->getAntennaInfo(ch2)->getLocationXYZ()[0];
      double y2=araGeom->getStationInfo(Station)->getAntennaInfo(ch2)->getLocationXYZ()[1];
      double z2=araGeom->getStationInfo(Station)->getAntennaInfo(ch2)->getLocationXYZ()[2];
      //	printf ("%d.%d Adding Pair (%d %d) dt=%f [%f,%f,%f]  [%f,%f,%f] \n",i1,i2,ch1,ch2,dt,x1,y1,z1,x2,y2,z2);
      if (dt!=-999 ) 	Reco->addPair(dt,x1,y1,z1,x2,y2,z2);   // leak not here
    }
  }
  //  cout<<"Pair fit \n";
  //  RECOOUT recout=Reco->doPairFit(); // leak not here (?)
  //delete Reco;
  TVector3 t=Reco->getVtrack();
  
  //  printf ("trackV:: \t %f \t %f \t %f %d \n",t.Mag(),t.Theta(), t.Phi(),minMethod);

  if (minMethod==1)  return(Reco->doPairFitSpherical());    
  if (minMethod==0)  return(Reco->doPairFit());    

}
*/
void  L2::FilldTPairs(vector<Int_t> chList, Int_t method) {
  // Memory leak is here
  double dt;
  Reco->clear();
  //Reco=new AraVertex();   // <=== memoory leak is here
  for (int i1=0; i1<(int) chList.size(); i1++) {
    for (int i2=i1+1; i2<(int) chList.size(); i2++) {
      int ch1=chList[i1];
      int ch2=chList[i2];
      dt=0;
      //cout<<"get diff \n";
      dt=getTimeDiff(chList[i1],chList[i2],method);
      double x1=araGeom->getStationInfo(Station)->getAntennaInfo(ch1)->getLocationXYZ()[0];
      double y1=araGeom->getStationInfo(Station)->getAntennaInfo(ch1)->getLocationXYZ()[1];
      double z1=araGeom->getStationInfo(Station)->getAntennaInfo(ch1)->getLocationXYZ()[2];
      double x2=araGeom->getStationInfo(Station)->getAntennaInfo(ch2)->getLocationXYZ()[0];
      double y2=araGeom->getStationInfo(Station)->getAntennaInfo(ch2)->getLocationXYZ()[1];
      double z2=araGeom->getStationInfo(Station)->getAntennaInfo(ch2)->getLocationXYZ()[2];
      //	printf ("%d.%d Adding Pair (%d %d) dt=%f [%f,%f,%f]  [%f,%f,%f] \n",i1,i2,ch1,ch2,dt,x1,y1,z1,x2,y2,z2);
      if (dt!=-999 ) 	Reco->addPair(dt,x1,y1,z1,x2,y2,z2);   // leak not here
    }
  }
  TVector3 t=Reco->getVtrack();
  return;
}


  //  cout<<"Pair fit \n";
  //  RECOOUT recout=Reco->doPairFit(); // leak not here (?)
  //delete Reco;
  
  //  printf ("trackV:: \t %f \t %f \t %f %d \n",t.Mag(),t.Theta(), t.Phi(),minMethod);

  //  if (minMethod==1)  return(Reco->doPairFitSpherical());    
  //if (minMethod==0)  return(Reco->doPairFit());    
//}



Double_t L2::getTimeDiff(int ch1, int ch2, int method) {
  // double Xdelays[16]={0,0,1.996,1.208,1.182,0,0.14,0,-3.239,0,-1.289,0,0,0,0,0};
  double offset=0;
  //  if (Station==0) offset=Xdelays[ch1]-Xdelays[ch2];
  //  cout<<"getTimeDiff\n";
  double fInterp=0.5 ;
  TGraph *g10=event->getGraphFromRFChan(ch1);
  TGraph *g20=event->getGraphFromRFChan(ch2);
  if (g10->GetN()<5 || g20->GetN()<5) return -999;
   TGraph *g1=FFTtools::getInterpolatedGraph(g10,fInterp);
  TGraph *g2=FFTtools::getInterpolatedGraph(g20,fInterp);
  //  TGraph *g1=event->getGraphFromRFChan(ch1);
  //TGraph *g2=event->getGraphFromRFChan(ch2);
  //TGraph *g1=g10;
  //TGraph *g2=g20;

  if (g1->GetN()<5 || g2->GetN()<5) return -999;
  Double_t *xt1=g1->GetX();
  Double_t *xt2=g2->GetX();
  Double_t *yv1=g1->GetY();
  Double_t *yv2=g2->GetY();
  
  if (method==1) { //xcor
    double fInterp=0.5 ; // Interpolation factor
    TGraph *grNorm1= getNormalisedGraph(FFTtools::getInterpolatedGraph(event->getGraphFromRFChan(ch1),fInterp));
    TGraph *grNorm2= getNormalisedGraph(FFTtools::getInterpolatedGraph(event->getGraphFromRFChan(ch2),fInterp)); 
    // TGraph *grNorm1= getNormalisedGraph((event->getGraphFromRFChan(ch1)));
    //TGraph *grNorm2= getNormalisedGraph((event->getGraphFromRFChan(ch2))); 
    double dt=getCorreMax(FFTtools::getCorrelationGraph(grNorm1,grNorm2));
    delete g1;
    delete g2;
    delete g10;
    delete g20;
    delete grNorm1;
    delete grNorm2;
    //cout<<"dt in method1="<<dt<<endl;
    //delete  xt1;
    //delete [] xt2;
    //delete [] yv1;
    //delete [] yv2;
    
    return (dt-offset);
  }

  if (method==0) { // Use maximum point in the wf as the time estimation
    double max1=-999;
    double max2=-999;
    double tmax2=0;
    double tmax1=0;
    for (int i=0; i<g1->GetN(); i++) {if (fabs(yv1[i])>max1 || max1==-999) {max1=fabs(yv1[i]); tmax1=xt1[i];}} 
    for (int i=0; i<g2->GetN(); i++) {if (fabs(yv2[i])>max2 || max2==-999) {max2=fabs(yv2[i]); tmax2=xt2[i];}}
   
    delete g1;
    delete g2;
    delete g10;
    delete g20;
    //  delete [] xt1;
    //delete [] xt2;
    //delete [] yv1;
    //delete [] yv2;
   

    // cout<<"dt in method0="<<tmax1-tmax2<<endl;
    return(tmax1 - tmax2-offset);	 
  }
  
  return(0);
}

Double_t L2::getCorreMax(TGraph *grCorI) {

  Double_t *yVals1=grCorI->GetY();   	
  Double_t *xVals1=grCorI->GetX();	       
  Double_t max=0, imax=0;
  Int_t binmax=0;
  for (Int_t pair2=0; pair2<grCorI->GetN(); pair2++) { if (yVals1[pair2]>max || max==0) {max=yVals1[pair2]; imax=xVals1[pair2]; binmax=pair2; }}

 
 Double_t weighted=(yVals1[binmax+1]*xVals1[binmax+1]+yVals1[binmax]*xVals1[binmax]+yVals1[binmax-1]*xVals1[binmax-1])/(yVals1[binmax+1]+yVals1[binmax]+yVals1[binmax-1]);
  imax=weighted;

  delete [] yVals1;
  delete [] xVals1;
  return(imax);
}

/*TGraph * L2::getCorrelationGraph(int ch1, int ch2) {
  cout<<"In get CorrelationGraph \n";
      double fInterp=0.5 ; // Interpolation factor
      TGraph *grNorm1= getNormalisedGraph(FFTtools::getInterpolatedGraph(event->getGraphFromRFChan(ch1),fInterp));
      TGraph *grNorm2= getNormalisedGraph(FFTtools::getInterpolatedGraph(event->getGraphFromRFChan(ch2),fInterp));
      cout<<"Doiung Xcor \n";
      TGraph *grCorI=FFTtools::getCorrelationGraph(grNorm1, grNorm2);     
      cout<<"Done \n";
     delete grNorm1; delete grNorm2; 

 return(grCorI);
 }*/

double L2::fillFFTHistoForRFChanL2(int chan, TH1D *histFFT) 
{
  double tot=0;
  for (int b=0; b<histFFT->GetNbinsX()+1; b++) histFFT->SetBinContent(b,0);
  
   TGraph *grFFT =event->getFFTForRFChan(chan);
   if(!grFFT) return -1;
   Double_t *xVals=grFFT->GetX();
   Double_t *yVals=grFFT->GetY();
   Int_t numPoints=grFFT->GetN();
   
   for(int i=0;i<numPoints;i++) {    
     Int_t bin=histFFT->FindBin(xVals[i]);
     Double_t currentVal=histFFT->GetBinContent(bin);
     Double_t newVal=10.*log10( pow(10,(currentVal/10.))+pow(10,(yVals[i]/10.)));
     tot=tot+pow(10,(yVals[i]/10.));
     histFFT->SetBinContent(bin, newVal);
  
   }
   tot=10.*log10(tot);
   //   delete [] xVals;
   //delete [] xVals;

   delete grFFT;
  
   return tot;

}


TGraph * getNormalisedGraph(TGraph *grIn)
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
  delete[] newY;
  delete [] xVals;
  delete [] yVals;

  return grOut;

}

/*int main(int argc, char **argv) {

  L2 * l2=new L2(123);

  l2->FillHeader(1,3);
  l2->FillHeader(3,5);
  

  l2->Save();
  printf ("bye \n");
  return(1);  
  }
*/
