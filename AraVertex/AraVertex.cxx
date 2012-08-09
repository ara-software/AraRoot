//#include "iceProp.h"
#include "AraVertex.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "TVector.h"

AraVertex::AraVertex() {
  Tmin=-512;
  Tmax=0;
  Xmin=-2000; Xmax=2000;
  Ymin=-2000; Ymax=2000;
  Zmin=-2000; Zmax=100;
  Tstep=20;
  Xstep=400;
  Ystep=400;
  Zstep=20;
  ice = new iceProp(1.78,-0.427, 0.016);  
  RxInEarly.X=26; 
  RxInEarly.Y=15; 
  RxInEarly.Z=-30; 


  nHits=0;
}

void AraVertex::printHits() {
  printf ("===================================== \n "); 
 for (unsigned int i=0; i<RxIn.size(); i++) {
   printf ("%d. ch%d %f  (%f,%f,%f) \n",i,RxIn[i].Ch,RxIn[i].T, RxIn[i].X, RxIn[i].Y, RxIn[i].Z);
  }
  printf ("===================================== \n "); 
}

void AraVertex::printPairs() {
  printf ("===================================== \n "); 
 for (unsigned int i=0; i<RxPairIn.size(); i++) {
    printf ("%d.  %f  (%f,%f,%f) (%f,%f,%f)\n",i,RxPairIn[i].dT, 
	   RxPairIn[i].X1, RxPairIn[i].Y1, RxPairIn[i].Z1,RxPairIn[i].X2, RxPairIn[i].Y2, RxPairIn[i].Z2);
  }
  printf ("===================================== \n "); 
}



/*TRACKOUT AraVertex::GetTrackEngine() {
  TRACKOUT out;
  int N=ExIn.size();
  double Vx=0;  double Vy=0;   double Vz=0;

  
  for (int i=0; i<N; i++) {
    double R2=(RxIn[i].x1-RxIn[i].x2)**2 + (RxIn[i].y1-RxIn[i].y2)**2 + (RxIn[i].z1-RxIn[i].z2)**2  ;
    double dx=RxIn[i].x1-RxIn[i].x2;
    double dy=RxIn[i].y1-RxIn[i].y2;
    double dz=RxIn[i].z1-RxIn[i].z2;
    double dt= - RxIn[i].t;  // Defintion - take dt as t2-t1 and not t1-t2;
    Vx += dx / R2 * dt ; 
    Vy += dy / R2 * dt ; 
    Vz += dz / R2 * dt ;     
  }
  TVector3 v3(Vx,Vy,Vz);  
  out.R=(Double_t) v3.Mag();
  out.theta=(Double_t) v3.Theta();
  out.phi=(Double_t) v3.Phi();
  return out;
}
*/

void AraVertex::addPair(double_t dt, double_t x1, double_t y1, double_t z1, double x2, double y2, double z2){
  RxPairIn.push_back(inputPair(dt,x1,y1,z1,x2,y2,z2));
  
}

void AraVertex::addHit(double_t t0, double_t x0, double_t y0, double_t z0,  Int_t ch){
  if((t0<RxInEarly.T || RxInEarly.T==-999) && z0<-1 ) { RxInEarly=inputAnt(t0,x0,y0,z0,ch); printf("early=ch %d\n",ch);}
  RxIn.push_back(inputAnt(t0,x0,y0,z0,ch));
  nHits++;
}

 RECOOUT AraVertex::doPairFit() {
   //      AraVertex *lll = this;
      //RECOOUT ro; //=new RECOOUT();
      ROOT::Math::Minimizer*  eventrecoMinuit = ROOT::Math::Factory::CreateMinimizer("Minuit","Simplex");
      ROOT::Math::Functor f2(this,&AraVertex::CalcChiSquareDiff,3);
      Int_t IPrintLevel=-1;  
      eventrecoMinuit->SetPrintLevel(IPrintLevel); 
      eventrecoMinuit->SetMaxIterations(100000);
      eventrecoMinuit->SetMaxFunctionCalls(100000); 
      eventrecoMinuit->SetTolerance(0.001);  
      eventrecoMinuit->SetValidError(1);
      eventrecoMinuit->SetFunction(f2); //chi square routine
      //eventrecoMinuit->SetLimitedVariable(0,"t0",RxInEarly.T,Tstep,Tmin,Tmax);
      eventrecoMinuit->SetLimitedVariable(0,"x0",RxInEarly.X,Xstep,Xmin,Xmax);
      eventrecoMinuit->SetLimitedVariable(1,"y0",RxInEarly.Y,Ystep,Ymin,Ymax);
      eventrecoMinuit->SetLimitedVariable(2,"z0",RxInEarly.Z,Zstep,Zmin,Zmax);
      eventrecoMinuit->Minimize();
      const double *xOut = eventrecoMinuit->X();
    
      ro.X=(Double_t) xOut[0];
      ro.Y=(Double_t) xOut[1];
      ro.Z=(Double_t) xOut[2];
      const double *xErr = eventrecoMinuit->Errors();

      ro.dX=(Double_t) xErr[0];
      ro.dY=(Double_t)  xErr[1];
      ro.dZ=(Double_t) xErr[2];
      ro.Status=(Int_t) eventrecoMinuit->Status(); 
      ro.Edm=(Double_t) eventrecoMinuit->Edm();
      ro.chisq=(double) eventrecoMinuit->MinValue();
 
      
      ro.nhits=(Int_t) RxPairIn.size();
      TVector3 v3(xOut[0],xOut[1],xOut[2]);
      
      // printPairs();
      ro.R=(Double_t) v3.Mag();
      ro.theta=(Double_t) v3.Theta();
      ro.phi=(Double_t) v3.Phi();
      
      //      delete lll;
        delete eventrecoMinuit;
	//delete [] xErr;
	//delete []xOut;


	// Calculate Track Engine vector:
	int N=RxPairIn.size();
	double Vx=0;  double Vy=0;   double Vz=0; 
	for (int i=0; i<N; i++) {
	  double R2=pow((RxPairIn[i].X1-RxPairIn[i].X2),2) + pow((RxPairIn[i].Y1-RxPairIn[i].Y2),2) + pow((RxPairIn[i].Z1-RxPairIn[i].Z2),2)  ;
	  double dx=RxPairIn[i].X1-RxPairIn[i].X2;
	  double dy=RxPairIn[i].Y1-RxPairIn[i].Y2;
	  double dz=RxPairIn[i].Z1-RxPairIn[i].Z2;
	  double dt= - RxPairIn[i].dT;  // Defintion - take dt as t2-t1 and not t1-t2;
	  Vx += dx / R2 * dt ; 
	  Vy += dy / R2 * dt ; 
	  Vz += dz / R2 * dt ;     
	}
	TVector3 vtrack(Vx,Vy,Vz);  
	ro.trackR=(Double_t) vtrack.Mag() / N ;
	ro.trackTheta=(Double_t) vtrack.Theta();
	ro.trackPhi=(Double_t) vtrack.Phi();

	// cout<<"Track engine:"<<Vx<<","<<Vy<<","<<Vz<<"\t\t"<<ro.trackR<<"=R, threta,phi="<<ro.trackTheta<<" "<<ro.trackPhi<<endl;
     return ro;
 }


/*
 RECOOUT AraVertex::doFit() {
  
   AraVertex *lll = this;
   Int_t ierflg = 0;
   ROOT::Math::Minimizer*  eventrecoMinuit = ROOT::Math::Factory::CreateMinimizer("Minuit","Migrad");
   //   ROOT::Math::Minimizer*  eventrecoMinuit = ROOT::Math::Factory::CreateMinimizer("Minuit","Simplex");
   //   eventrecoMinuit = new TMinuit(4);
   //   ROOT::Math::Minimizer* eventrecoMinuit  = ROOT::Math::Factory::CreateMinimizer("GSLMultiMin","BFGS");
   ROOT::Math::Functor f2(lll,&AraVertex::CalcChiSquare,4);
   Int_t IPrintLevel=2;  
   eventrecoMinuit->SetPrintLevel(IPrintLevel); 
   eventrecoMinuit->SetMaxIterations(100000);
   eventrecoMinuit->SetMaxFunctionCalls(100000);
 
   eventrecoMinuit->SetTolerance(0.0000001);
   

   eventrecoMinuit->SetFunction(f2); //chi square routine
   //   Double_t arglist[2]={200,1}; //,10,10};	// UP = 200, 1 - is not used  
   //   eventrecoMinuit->mnexcm("SET ERR", arglist, 1, ierflg);	// Parameter UP for both SIMPLEX and MIGRAD is set up here to 200
   // RxInEarly.X=0.2;
   // RxInEarly.Y=-30;
   // RxInEarly.Z=-22;
   eventrecoMinuit->SetLimitedVariable(0,"t0",RxInEarly.T,Tstep,Tmin,Tmax);
   eventrecoMinuit->SetLimitedVariable(1,"x0",RxInEarly.X,Xstep,Zmin,Zmax);
   eventrecoMinuit->SetLimitedVariable(2,"y0",RxInEarly.Y,Ystep,Ymin,Ymax);
   eventrecoMinuit->SetLimitedVariable(3,"z0",RxInEarly.Z,Zstep,Zmin,Zmax);
   eventrecoMinuit->Minimize();
   const double *xOut = eventrecoMinuit->X();
   recoOut.FitPoint[0]=xOut[0];
   recoOut.FitPoint[1]=xOut[1];
   recoOut.FitPoint[2]=xOut[2];
   recoOut.FitPoint[3]=xOut[3];
   const double *xErr = eventrecoMinuit->Errors();
   recoOut.FitPerror[0]=xErr[0];
   recoOut.FitPerror[1]=xErr[1];
   recoOut.FitPerror[2]=xErr[2];
   recoOut.FitPerror[3]=xErr[3];
   printf ("%f %f \n ",xOut[1], xErr[1]);
   recoOut.Status=eventrecoMinuit->Status(); 
//        status = 1    : Covariance was made pos defined
//       status = 2    : Hesse is invalid
//       status = 3    : Edm is above max
//       status = 4    : Reached call limit
/       status = 5    : Any other failure
//
   recoOut.Edm=eventrecoMinuit->Edm();
   recoOut.chisq=eventrecoMinuit->MinValue();
   recoOut.nhits=RxIn.size();

  printf("==> reco res=%f %f %f %f chi=%f\n",xOut[0],xOut[1],xOut[2],xOut[3],recoOut.chisq);
   
   //   eventrecoMinuit->mnexcm("SIMPLEX", argsSimplexMigrad, 2, convergence[0]);	// SIMPLEX doesn't use UP parameter as soon as tolerance is specified
   //eventrecoMinuit->mnexcm("MIGRAD", argsSimplexMigrad, 2, convergence[1]);	// But MIGRAD somehow uses UP along with tolerance
   return recoOut;
 }
*/
double AraVertex::CalcChiSquare(const double *xx )
{


  double chisquare=0;
  //  printf("Number of hits = %d \n",RxIn.size() );
  for(unsigned int i=0; i<RxIn.size(); i++){ // Loop on Number of channels
    //float DistanceSq=  (xx[1]-RxIn[i][1])*(xx[1]-RxIn[i][1])+ (xx[2]-RxIn[i][2])*(xx[2]-RxIn[i][2]) + (xx[3]-RxIn[i][3])*(xx[3]-RxIn[i][3]) ; 
    double TransitTimens = ice->getT(xx[1],xx[2],xx[3],RxIn[i].X,RxIn[i].Y,RxIn[i].Z);
    double delta = xx[0]  +   TransitTimens  -   RxIn[i].T ; 
    // recoOut.Rxtresid[RxIn[i].Ch]=delta;
    chisquare =chisquare + delta*delta;
    printf("   channel %d: chisquare=%f : trying t=%f (%f %f %f) diff=%f, transitTime= %f peaktime=%f\n",RxIn[i].Ch,chisquare,xx[0],xx[1],xx[2],xx[3],delta,TransitTimens,  RxIn[i].T);
  }
  printf("chisquare: trying t=%f (%f %f %f) chi=%f \n",xx[0],xx[1],xx[2],xx[3],chisquare);
  return((chisquare));
}


double AraVertex::CalcChiSquareDiff(const double *xx )
{
  double chisquare=0;
  //  printf("Number of hits = %d \n",RxIn.size() );
  for(unsigned int i=0; i<RxPairIn.size(); i++){ // Loop on Number of channels
    double TransitTimens = ice->getDT(RxPairIn[i].X1,RxPairIn[i].Y1,RxPairIn[i].Z1,RxPairIn[i].X2,RxPairIn[i].Y2,RxPairIn[i].Z2,xx[0],xx[1],xx[2]);
    //printPair(i);
    
    double delta = TransitTimens  -   RxPairIn[i].dT ; 
    //    recoOut.Rxtresid[RxIn[i].Ch]=delta;
    chisquare =chisquare + delta*delta;
    //printf("  \t pair %d: chisquare=%f : trying  (%f %f %f) diff=%f, transitTime= %f peaktime=%f\n",i ,chisquare,xx[0],xx[1],xx[2],delta,TransitTimens,  RxPairIn[i].dT);
  }
  //    printf("==============>chisquare: trying  (%f %f %f) chi=%f \n",xx[0],xx[1],xx[2],chisquare);
  return((chisquare));
}



