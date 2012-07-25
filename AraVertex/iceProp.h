#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TF1.h>

#ifndef ICEPROP_H
#define ICEPROP_H


const double C_AIR= 0.29970551819;


class iceProp {
 public:
  iceProp(float A, float B, float C) { n_deep=A; n_c=C; n_shallow=A+B; setIceModelExp();}
  ~iceProp(){};

  //  A=1.78;   B=-0.427;   C=0.016; // units!
  Double_t n_deep, n_shallow, n_c;
  Double_t A,B,C;
  TF1 *iceN;
  
  void DrawIceModel() { iceN->Draw();}
  Double_t getDT(float R1x, float R1y, float R1z, float R2x, float R2y, float R2z, float Tx, float Ty, float Tz) { 
    Double_t t1=getT(R1x,R1y,R1z,Tx,Ty,Tz);
    Double_t t2=getT(R2x, R2y,R2z, Tx,Ty,Tz);
    //printf ("dt = %f - %f = %f [%f,%f,%f]\n",t1,t2,t1-t2,R2x,R2y,R2z);
    return (t1-t2);
  }; 
  Double_t getT(float x1,float y1, float z1, float x2, float y2, float z2) {return(getT(sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2)),z1,z2));}
  Double_t getT(float Rxyz, float z0, float z1) {
    //double Rxyz=sqrt(Rxy*Rxy+(z0-z1)*(z0-z1));
    if (z1>z0) {float temp=z1; z1=z0;z0=temp;}       // make sure z0 is above z1 so z1<z0
    if (z1==z0) {  // If Points are at the same depth, just propgate as straight line
      if (z0>0)  return(Rxyz/C_AIR);
      if (z0<=0) return(Rxyz/(C_AIR/iceN->Eval(z0)));
    }
    if (z1>=0 && z0>=0) return(Rxyz/C_AIR);
    if (z0<0 && z1<0) { // Both Zs are under the ice
      Double_t a=Rxyz/(z0-z1);
      return((A*(z0-z1)+B/C*(exp(C*z0)-exp(C*z1)))/C_AIR*sqrt(1+a*a));
    }
    if (z0>0 && z1<0) {
      Double_t a=Rxyz/(z0-z1);     
      Double_t Tice=(A*(z0-0)+B/C*(exp(C*z0)-exp(C*0)))/C_AIR*sqrt(1+a*a); // Check if this is correct. Rxy vs. Rxyz
      Double_t Tair=sqrt(z1*z1+(Rxyz-z0*a)*(Rxyz-z0*a))/C_AIR;
      return(Tair+Tice);// Need to check this calculation
    }
    return(-1);
  }

 private: 
  void setIceModelExp() {
    iceN=new TF1("iceN","[0]+[1]*exp(-x*[2])",0,-2000);
    iceN->SetParameter(0,n_deep);
    iceN->SetParameter(1,n_shallow-n_deep);
    iceN->SetParameter(2,n_c);
    A=n_deep;
    B=n_shallow-n_deep;
    C=n_c;
  }



};



#endif
