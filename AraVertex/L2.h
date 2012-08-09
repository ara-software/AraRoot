#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include "L2Structure.h"
#include "UsefulIcrrStationEvent.h"

#ifndef L2_H
#define L2_H



class L2 {
 public:
  L2(int runNumber,AraGeomTool *geometryInfo);
  ~L2();
  int FillHeader(double x,double y);
  int FillEvent(UsefulIcrrStationEvent *realIcrrEvPtr);
  int FillGeoTree();
  void Save();
  void AutoSave();
 private: 
  vector<Int_t> InIceV;
  vector<Int_t> InIceH;
  vector<Int_t> InIceAll;
  vector<Int_t> InIceTrig;

  AraVertex *Reco;
  RECOOUT  DoReconstruction(vector<Int_t> chList, Int_t dtMethod);
  Double_t getTimeDiff(int ch1, int ch2, int method);
  TGraph * getCorrelationGraph(int ch1, int ch2);
  Double_t getCorreMax(TGraph *grCorI) ;
  Double_t fillFFTHistoForRFChanL2(int chan, TH1D *histFFT);

  UsefulIcrrStationEvent * event;
  TTree * L2EventTree;
  TTree * L2RunTree;
  TTree * L2GeoTree;
  TFile * L2File;
  TIME timeader;
  RUNHEADER *runheader;
  TRIGGER trigger;
  HK hk;
  WF wf;
  HEADER header;
  ANTENNA antenna;
  

  RECOOUT recoVmax;
  RECOOUT recoHmax;
  RECOOUT recoVxcor;
  RECOOUT recoHxcor;
  RECOOUT recoAllmax;
  RECOOUT recoAllxcor;
  // RECOOUT recoTrigmax;
  //RECOOUT recoTrigxcor;


  /*  RECOOUT recoVoutSig1;
  RECOOUT recoHoutSig1;
  RECOOUT recoVoutSig2;
  RECOOUT recoHoutSig2;
  RECOOUT recoVoutMax;
  RECOOUT recoHoutMax;
  */
  int runNumber;
  AraGeomTool * araGeom;
  int Station;

  unsigned int Adt; 
  Int_t CountAdt;
  unsigned int t0CountAdt;

  unsigned int Bdt; 
  Int_t CountBdt;
  unsigned int t0CountBdt;

  unsigned int startT;
  unsigned int endT;
  int isFirstEvent;
  double LastForcedRMS[16];

  int firstEvent;
  int lastEvent;
  int EventCounter[4];
  float sigReco1;
  float sigReco2;

  /*
  double RX_X[16], RX_Y[16], RX_Z[16], RX_T[16], RX_T1[16], RX_T2[16], RX_T3[16], RX_T4[16];
  Double_t  RX_DELAY[16];
  int  RX_POL[16];
  */
  //ClassDef(L2,1); 
};

/*

class L2Event : public TObject {
 public:
  L2Event();
  ~L2Event();
 private:  
  Int_t EventNumber;
  Int_t TriggerType;
  Int_t EventType;  
  Int_t NumberOfEvents;
  double Temperature;
  Double_t RubidiumTriggerTime;
  Double_t DeadTime;
  Float_t AverageTriggerRate;

  
}

class L2Channel : public TObject {
  double TotalPower;
  

}

class L2Run : public TObject {
 private:
  Int_t RunNumber;
  ULong64_t RunEndTime;
  ULong64_t RunStartTime;
  Int_t ErrorFlag;
  Int_t RunType;
  Int_t NumberOfEvents;
  Int_t NumberOfRFEvents;
  Int_t NumberOfPulserEvents;
  Int_t NumberOfForcedEvents;
 public:

}




Class L2Reco : public TObject {
 private:
  float X;
  float Y;
  float Z;
  float dx;
  float dy;
  float dz;
  float chi2;
 public:
  void SetVertex(float x,float y,float z) {X=x; Y=y; Z=z;}
  void setVertexError(float x, float y, float z) {dx=x; dy=Y; dz=Z;}
  void setchi2(float c2) {chi2=c2;}
}
*/
#endif //L2_H
