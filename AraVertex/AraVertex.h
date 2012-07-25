#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include "UsefulIcrrStationEvent.h"
#include <TMinuit.h>
#include <TVector3.h>
#include "iceProp.h"

#include <iostream>
using namespace std;

#ifndef ARAVERTEX_H
#define ARAVERTEX_H


typedef struct  {
  Double_t FitPoint[3];
  Double_t R, theta,phi;
  Double_t FitPerror[3];
  Int_t nhits;
  Double_t chisqvtx;  
  //Float_t Rxtresid[16];
  //  Float_t deltaTime[16];
   Float_t convergence[2];
}RECOOUT; 


class AraVertex {
 public:
  AraVertex();
  ~AraVertex(){};


  struct inputAnt{
    Double_t T;
    Double_t X;
    Double_t Y;
    Double_t Z;
    Int_t Ch;
    inputAnt(Double_t t, Double_t x, Double_t y, Double_t z, Int_t ch) {
      T=t; X=x; Y=y; Z=z; Ch=ch;
    }
    inputAnt() {T=-999; X=-999; Y=-999; Z=-999;Ch=-1;}
  };

  struct inputPair{
    Double_t dT;
    Double_t X1,X2;
    Double_t Y1,Y2;
    Double_t Z1,Z2;
    inputPair(Double_t t, Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2) {
      dT=t; X1=x1; Y1=y1; Z1=z1;
      X2=x2; Y2=y2; Z2=z2; 
    }
    inputPair() {dT=-999; X1=-999; Y1=-999; Z1=-999; X2=-999; Y2=-999; Z2=-999;}
  };

  vector<inputAnt> RxIn; // Array of hits to be used in reco. each has [t,x,y,z];
  vector<inputPair> RxPairIn; // Array of hits to be used in reco. each has [t,x,y,z];
  

  
  Float_t Xmin, Xmax, Ymin, Ymax,Zmin,Zmax, Tmin,Tmax;
  Float_t Tstep,Xstep,Ystep,Zstep;
  void addHit(Double_t t0, Double_t x0, Double_t y0, Double_t z0, Int_t ch);
  void addPair(Double_t dt, Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);

  void printHits();
  void printPairs();
  RECOOUT doFit();
  RECOOUT doPairFit();
  void printPair(int i){printf ("\nusing to calculate transit time pair %d:(%f,%f,%f) (%f %f %f), dt=%f \n",i,RxPairIn[i].X1,RxPairIn[i].Y1,RxPairIn[i].Z1,RxPairIn[i].X2,RxPairIn[i].Y2,RxPairIn[i].Z2,RxPairIn[i].dT);};

 private:
  RECOOUT recoOut;
  iceProp *ice;
  double CalcChiSquare(const double *xx );
  double CalcChiSquareDiff(const double *xx );
  inputAnt RxInEarly;
  int nHits;
 // TMinuit *eventrecoMinuit;
};
#endif


