#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <AraGeomTool.h>
#include "AraVertex.h"
#include "FFTtools.h"

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

L2::L2(int runNumber_,AraGeomTool *geometryInfo, int station) {
  araGeom=geometryInfo;
  Station=station;
  // Prepare tree:



  L2File = TFile::Open("A.root","Recreate");
  L2GeoTree= new TTree("L2GeoTree","Geometry");
  L2EventTree = new TTree("L2EventTree","Event tree");
  L2RunTree= new TTree("L2RunTree","Run header tree");
  L2RunTree->Branch("run",&runheader,"runNumber/I:startTime/i:startDOY/I:startHour/I:startMinute/I:startSecond/I:startDOW/I:endTime/i:endDOY/I:endHour/I:endMinute/I:endSecond/I:endDOW/I:duration/i:errorFlag/I:runType/I:NEvents/I:NRF/I:NPulser/I:NForced/I:NunClassified/I:Nall/I");  
  //  L2RunTree->Branch("geometry",);
  L2GeoTree->Branch("Geo",&antenna,"stationId/I:channelId/I:X/D:Y/D:Z/D:highPass/D:lowPass/D:cableDelay/D:pol/I:type/I:orient/I:noise/D");

  L2EventTree->Branch("trigger",&trigger,"RbClock/D:deadTime/D:triggerType/I:eventType/I:triggerPattern/I");
  L2EventTree->Branch("hk",&hk,"temperature[8]/D:RFPower[16]/D:sclGlobal/s:sclL1[12]/s:scl[24]/s");
  L2EventTree->Branch("header",&header,"unixTime/i:DOY/I:hour/I:minute/I:second/I:DOW/I:unixTimeUsec/i:eventNumber/i:gpsSubTime/I:calibStatus/s:priority/b:errorFlag/b");
  L2EventTree->Branch("wf",&wf,"mean[16]/D:rms[16]/D:inTrigPattern[16]/i");

  TString RecoBranch="X/D:Y/D:Z/D:R/D:theta/D:phi/D:dX/D:dY/D:dZ/D:nhits/I:chisqvtx/D:status/F:EDM/F";
  L2EventTree->Branch("RecoVMax",&recoVmax,RecoBranch);
  L2EventTree->Branch("RecoHMax",&recoHmax,RecoBranch);



  // Fill in run specific parameters
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

  memset(EventCounter, 0, 4*sizeof(int));
  //  EventCounter={0,0,0,0};

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
  
  // for (int station=0; station<ICRR_NO_STATIONS; station++) {
      for(int ant=0;ant<ANTS_PER_ICRR;ant++) {   
	antenna.stationId=Station;
	antenna.channelId=ant;
	antenna.Location[0]=araGeom->fStationInfo[Station].fAntInfo[ant].antLocation[0];
	antenna.Location[1]=araGeom->fStationInfo[Station].fAntInfo[ant].antLocation[1];
	antenna.Location[2]=araGeom->fStationInfo[Station].fAntInfo[ant].antLocation[2];
	antenna.HighPassFilter=araGeom->fStationInfo[Station].fAntInfo[ant].highPassFilterMhz;
	antenna.LowPassFilter=araGeom->fStationInfo[Station].fAntInfo[ant].lowPassFilterMhz;
	antenna.cableDelay=araGeom->fStationInfo[Station].fAntInfo[ant].cableDelay;
	antenna.antPol=araGeom->fStationInfo[Station].fAntInfo[ant].polType; 
	//	antenna.antDirection=araGeom->fStationInfo[station].fAntInfo[ant].antDir; 
	antenna.antType=araGeom->fStationInfo[Station].fAntInfo[ant].antType; 
	antenna.antOrient[0]=araGeom->fStationInfo[Station].fAntInfo[ant].antOrient[0];
	antenna.antOrient[1]=araGeom->fStationInfo[Station].fAntInfo[ant].antOrient[1];
	antenna.antOrient[2]=araGeom->fStationInfo[Station].fAntInfo[ant].antOrient[2];
	//jpd changed antOrient from Int_t to Double_t[3]
	antenna.averageNoiseFigure=araGeom->fStationInfo[Station].fAntInfo[ant].avgNoiseFigure;       	     L2GeoTree->Fill();

	if (antenna.antPol ==0 && antenna.Location[2]<-5 ) InIceV.push_back(ant);
	if (antenna.antPol ==1 && antenna.Location[2]<-5 ) InIceH.push_back(ant);
	if ((antenna.antPol ==1 || antenna.antPol ==0) && antenna.Location[2]<-5 ) InIceAll.push_back(ant);
      }
      printf("Number of vertical antennas=%d,  hor=%d both=%d \n",InIceV.size(), InIceH.size(),InIceAll.size());
      //}
  L2File->cd();
  L2GeoTree->Write();
  return(0);
}
void L2::Save() {

  endT=header.unixTime.epoch;
  printf("From %d to %d dt= %d \n",endT,startT,endT-startT);

  L2File->cd();
  // Int_t RunNumber;

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

  lastEvent=event->head.eventNumber;
  TCanvas *c1=new TCanvas("c1","c1",1000,600);
  trigger.TriggerType = (Int_t) event->trig.trigType;
  trigger.TriggerPattern = (Int_t) event->trig.trigPattern;
  trigger.RbClock = (Double_t) event->trig.getRubidiumTriggerTimeInSec();
  trigger.DeadTime = (Double_t) event->trig.getDeadtime();
  printf ("Rb clock is %f \n",trigger.RbClock);
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
  header.gpsSubTime=event->head.gpsSubTime;
  header.calibStatus=event->head.calibStatus;
  header.priority=event->head.priority;
  header.errorFlag=event->head.errorFlag;
  recoVmax=DoReconstruction(InIceV, 0) ;
  recoHmax=DoReconstruction(InIceH, 0) ;

  for (int ch=0; ch<16; ch++) {         
    TGraph *gWF = event->getGraphFromRFChan(ch);

    double fInterp=0.5 ; // Interpolation factor
    TGraph *gVt0;
    if (gWF->GetN() > 10) gVt0=FFTtools::getInterpolatedGraph(gWF,fInterp); //no gain
    else gVt0=gWF;
    wf.mean[ch]=gWF->GetMean(2);
    wf.rms[ch]=gWF->GetRMS(2);
    if (LastForcedRMS[ch]==0 || trigger.TriggerType==68) LastForcedRMS[ch]=wf.rms[ch];
    wf.isInTrigPattern[ch]=gWF->GetMean();    
    Double_t *xt=gVt0->GetX();
    Double_t *Vt=gVt0->GetY();
    Int_t nVt0=gVt0->GetN();

    double max=0; double tmax=0;
    double thisTime1=-999;
    double thisTime2=-999;


    double x=araGeom->fStationInfo[Station].fAntInfo[ch].antLocation[0];
    double y=araGeom->fStationInfo[Station].fAntInfo[ch].antLocation[1];
    double z=araGeom->fStationInfo[Station].fAntInfo[ch].antLocation[2];
    
    double delay=araGeom->fStationInfo[Station].fAntInfo[ch].cableDelay;
    printf ("Channel=%d, pol=%d \n",ch,(araGeom->fStationInfo[Station].fAntInfo[ch].polType));

    L2EventTree->FlushBaskets();
  }  
  


  //printf ("Values= %d %d %f%f \n",trigger.TriggerType, trigger.TriggerPattern, trigger.RbClock, trigger.DeadTime);
  //  printf ("power=%f temp=%f power=%f\n",event->hk.getRFPowerDiscone(2),  event->hk.getTemperature(0), event->hk.getRFPowerBatwing(2));

  L2EventTree->Fill();
  return(0);

}


RECOOUT  L2::DoReconstruction(vector<Int_t> chList, Int_t method) {
    double dt;
    AraVertex *Reco=new AraVertex();
    for (int i1=0; i1<chList.size(); i1++) {
      for (int i2=i1+1; i2<chList.size(); i2++) {
	int ch1=chList[i1];
	int ch2=chList[i2];

	dt=getTimeDiff(chList[i1],chList[i2],method);
	double x1=araGeom->fStationInfo[Station].fAntInfo[ch1].antLocation[0];
	double y1=araGeom->fStationInfo[Station].fAntInfo[ch1].antLocation[1];
	double z1=araGeom->fStationInfo[Station].fAntInfo[ch1].antLocation[2];
	double x2=araGeom->fStationInfo[Station].fAntInfo[ch2].antLocation[0];
	double y2=araGeom->fStationInfo[Station].fAntInfo[ch2].antLocation[1];
	double z2=araGeom->fStationInfo[Station].fAntInfo[ch2].antLocation[2];
	printf ("Adding Pair (%d %d) dt=%f [%f,%f,%f]  [%f,%f,%f] \n",ch1,ch2,dt,x1,y1,z1,x2,y2,z2);
	Reco->addPair(dt,x1,y1,z1,x2,y2,z2); 
      }
    }

    RECOOUT recout=Reco->doPairFit();
    return(recout);
    
 }


Double_t L2::getTimeDiff(int ch1, int ch2, int method) {
      if (method==0) { // Use maximum
	double max1=-999;
	double max2=-999;
	double tmax2,tmax1;
	TGraph *g1=event->getGraphFromRFChan(ch1);
	TGraph *g2=event->getGraphFromRFChan(ch2);
	Double_t *xt1=g1->GetX();
	Double_t *xt2=g2->GetX();
	Double_t *yv1=g1->GetY();
	Double_t *yv2=g2->GetY();
	for (int i=0; i<g1->GetN(); i++) {if (fabs(yv1[i])>max1 || max1==-999) {max1=fabs(yv1[i]); tmax1=xt1[i];}} 
	for (int i=0; i<g2->GetN(); i++) {if (fabs(yv2[i])>max2 || max2==-999) {max2=fabs(yv2[i]); tmax2=xt2[i];}}
	return(tmax1 - tmax2 );
	 
      }
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
