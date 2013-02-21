//////////////////////////////////////////////////////////////////////////////
/////  AraGeomTool.h       ARA Geometry tool                             /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class working out what is where                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
/////          & Jonathan Davies (jdavies@hep.ucl.ac.uk)                 /////
//////////////////////////////////////////////////////////////////////////////

#include "AraGeomTool.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <zlib.h>
#include <cstdlib>


//sqlite includes
#include <sqlite3.h>

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"

AraGeomTool * AraGeomTool::fgInstance=0;
Double_t AraGeomTool::nTopOfIce=1.48;

AraGeomTool::AraGeomTool() 
{
   //Default Constructor

  //Read in the channel maps for the TestBed and Station1
  // readChannelMapDbIcrr(ARA_TESTBED);
  // readChannelMapDbIcrr(ARA_STATION1);
  // readChannelMapDbAtri(ARA_STATION1B);
  for(int i=0;i<ICRR_NO_STATIONS;i++) 
    fStationInfoICRR[i]=NULL;
  for(int i=0;i<ATRI_NO_STATIONS;i++) 
    fStationInfoATRI[i]=0;
  

}

AraGeomTool::~AraGeomTool() {
   //Default Destructor
  for(int i=0;i<ICRR_NO_STATIONS;i++)  {
    if(fStationInfoICRR[i]) delete fStationInfoICRR[i];
  }
  for(int i=0;i<ATRI_NO_STATIONS;i++) {
    if(fStationInfoATRI[i]) delete fStationInfoATRI[i];
  }
}

//______________________________________________________________________________
AraGeomTool*  AraGeomTool::Instance()
{
  //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraGeomTool();
  return fgInstance;
}

int AraGeomTool::getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum, AraStationId_t stationId)
{
  
  //  fprintf(stderr, "antPol %i antNum %i stationId %i\n", (int)antPol, antNum, (int)stationId);
  return getStationInfo(stationId)->getRFChanByPolAndAnt(antNum,antPol);
}


AraAntPol::AraAntPol_t AraGeomTool::getPolByRFChan(int rfChan, AraStationId_t stationId)
{
  //RJN Should add a check here for safety
  if(getStationInfo(stationId)) {
    return getStationInfo(stationId)->getAntennaInfo(rfChan)->polType;
  }
   return AraAntPol::kNotAPol;
}

Int_t AraGeomTool::getAntNumByRFChan(int rfChan, AraStationId_t stationId)
{
  //RJN Should add a check here for safety
  if(getStationInfo(stationId))
    return getStationInfo(stationId)->getAntennaInfo(rfChan)->antPolNum;
  return -1;
}


//FIXME //jpd this is most definitely a hack to make AraCanvasMaker work -> this will only
//return the testbed lookup stuff not station1
int AraGeomTool::getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum)
{
  return getRFChanByPolAndAnt(antPol,antNum,ARA_STATION1);
}//FIXME -- this just loads the stuff for station1

Double_t AraGeomTool::calcDeltaTSimple(Double_t ant1[3], Double_t ant2[3], Double_t source[3]) {
  Double_t d1=TMath::Sqrt(TMath::Power(ant1[0]-source[0],2)+TMath::Power(ant1[1]-source[1],2)+TMath::Power(ant1[2]-source[2],2));
  Double_t d2=TMath::Sqrt(TMath::Power(ant2[0]-source[0],2)+TMath::Power(ant2[1]-source[1],2)+TMath::Power(ant2[2]-source[2],2));
  Double_t t1t2=(d2-d1)*nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;
}


Double_t AraGeomTool::calcDeltaTInfinity(Double_t ant1[3], Double_t ant2[3],Double_t phiWave, Double_t thetaWave)
{
  //Calc some cylindrical coordinates
  Double_t rho1=TMath::Sqrt(ant1[0]*ant1[0]+ant1[1]*ant1[1]);
  Double_t phi1=TMath::ATan2(ant1[1],ant1[0]);
  Double_t rho2=TMath::Sqrt(ant2[0]*ant2[0]+ant2[1]*ant2[1]);
  Double_t phi2=TMath::ATan2(ant2[1],ant2[0]);
  Double_t d1=TMath::Cos(thetaWave)*(ant1[2]*TMath::Tan(thetaWave)+rho1*TMath::Cos(phi1-phiWave));
  Double_t d2=TMath::Cos(thetaWave)*(ant2[2]*TMath::Tan(thetaWave)+rho2*TMath::Cos(phi2-phiWave));
  Double_t t1t2=(d2-d1)*nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;

}

//jd
Double_t AraGeomTool::calcDeltaTInfinity(Int_t chan1, Int_t chan2,Double_t phiWave, Double_t thetaWave, AraStationId_t stationId)
{
  if(chan1<0 || chan1>=TOTAL_ANTS_PER_ICRR)
    return 0;
  if(chan2<0 || chan2>=TOTAL_ANTS_PER_ICRR)
    return 0;
  return calcDeltaTInfinity(getStationInfo(stationId)->fAntInfo[chan1].antLocation,getStationInfo(stationId)->fAntInfo[chan2].antLocation,phiWave,thetaWave);            
}

Double_t AraGeomTool::calcDeltaTR(Double_t ant1[3], Double_t ant2[3], Double_t phiWave, Double_t thetaWave,Double_t R)
{

  Double_t xs=R*TMath::Cos(thetaWave)*TMath::Cos(phiWave);
  Double_t ys=R*TMath::Cos(thetaWave)*TMath::Sin(phiWave);
  Double_t zs=R*TMath::Sin(thetaWave);

  
  Double_t d1=TMath::Sqrt((xs-ant1[0])*(xs-ant1[0])+(ys-ant1[1])*(ys-ant1[1])+(zs-ant1[2])*(zs-ant1[2]));
  Double_t d2=TMath::Sqrt((xs-ant2[0])*(xs-ant2[0])+(ys-ant2[1])*(ys-ant2[1])+(zs-ant2[2])*(zs-ant2[2]));
         
  Double_t t1t2=(d1-d2)*nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;

}

//jd
Double_t AraGeomTool::calcDeltaTR(Int_t chan1, Int_t chan2, Double_t phiWave, Double_t thetaWave,Double_t R, AraStationId_t stationId)
{
  if(chan1<0 || chan1>=TOTAL_ANTS_PER_ICRR)
    return 0;
  if(chan2<0 || chan2>=TOTAL_ANTS_PER_ICRR)
    return 0;
  return calcDeltaTR(getStationInfo(stationId)->fAntInfo[chan1].antLocation,getStationInfo(stationId)->fAntInfo[chan2].antLocation,phiWave,thetaWave,R);   

}



bool AraGeomTool::isIcrrStation(AraStationId_t stationId){ 
   //RJN changed
  if(stationId==ARA_TESTBED||stationId==ARA_STATION1) return true; 
  else return false;
  
}

bool AraGeomTool::isAtriStation(AraStationId_t stationId){

  if(isIcrrStation(stationId)) return false;
  else return true;

}

Int_t AraGeomTool::getStationNumber(AraStationId_t stationId){
   if(ARA_TESTBED) 
      return 0;
   if(ARA_STATION1B || ARA_STATION1) 
      return 1;
   return (int)stationId;
}


Int_t AraGeomTool::getStationCalibIndex(AraStationId_t stationId){
  
  switch(stationId){
    
  case ARA_TESTBED:
     return 0;
     break;
  case ARA_STATION1:
     return 1;
     break;
  case ARA_STATION1B:
     return 0;
     break;
  case ARA_STATION2:
     return 1;
     break;
  case ARA_STATION3:
     return 2;
    break;
  case ARA_STATION4:
    return 3;
    break;
  case ARA_STATION5:
    return 4;
    break;
  case ARA_STATION6:
    return 5;
    break;
  case ARA_STATION7:
    return 6;
    break;
  case ARA_STATION8:
    return 7;
    break;
  case ARA_STATION9:
    return 8;
    break;
  case ARA_STATION10:
    return 9;
    break;
  case ARA_STATION11:
    return 10;
    break;
  case ARA_STATION12:
    return 11;
    break;
  case ARA_STATION13:
    return 12;
    break;
  case ARA_STATION14:
    return 13;
    break;
  case ARA_STATION15:
    return 14;
    break;
  case ARA_STATION16:
    return 15;
    break;
  case ARA_STATION17:
    return 16;
    break;
  case ARA_STATION18:
    return 17;
    break;
  case ARA_STATION19:
    return 18;
    break;
  case ARA_STATION20:
    return 19;
    break;
  case ARA_STATION21:
    return 20;
    break;
  case ARA_STATION22:
    return 21;
    break;
  case ARA_STATION23:
    return 22;
    break;
  case ARA_STATION24:
    return 23;
    break;
  case ARA_STATION25:
    return 24;
    break;
  case ARA_STATION26:
    return 25;
    break;
  case ARA_STATION27:
    return 26;
    break;
  case ARA_STATION28:
    return 27;
    break;
  case ARA_STATION29:
    return 28;
    break;
  case ARA_STATION30:
    return 29;
    break;
  case ARA_STATION31:
    return 30;
    break;
  case ARA_STATION32:
    return 31;
    break;
  case ARA_STATION33:
    return 32;
    break;
  case ARA_STATION34:
    return 33;
    break;
  case ARA_STATION35:
    return 34;
    break;
  case ARA_STATION36:
    return 35;
    break;
  case ARA_STATION37:
    return 36;
    break;
  default:
    fprintf(stderr, "AraGeomTool::getStationCalibIndex -- Error - Unknown stationId %i\n", stationId);
    return -1;
  }
}




void AraGeomTool::printStationName(AraStationId_t stationId){

  std::cout << getStationName(stationId) << "\n";
}

const char* AraGeomTool::getStationName(AraStationId_t stationId){

  switch(stationId){

  case ARA_TESTBED:
    return "TESTBED";
    break;
  case ARA_STATION1:
    return "STATION1";
    break;
  case ARA_STATION1B:
    return "STATION1B";
    break;
  case ARA_STATION2:
    return "STATION2";
    break;
  case ARA_STATION3:
    return "STATION3";
    break;
  case ARA_STATION4:
    return "STATION4";
    break;
  case ARA_STATION5:
    return "STATION5";
    break;
  case ARA_STATION6:
    return "STATION6";
    break;
  case ARA_STATION7:
    return "STATION7";
    break;
  case ARA_STATION8:
    return "STATION8";
    break;
  case ARA_STATION9:
    return "STATION9";
    break;
  case ARA_STATION10:
    return "STATION10";
    break;
  case ARA_STATION11:
    return "STATION11";
    break;
  case ARA_STATION12:
    return "STATION12";
    break;
  case ARA_STATION13:
    return "STATION13";
    break;
  case ARA_STATION14:
    return "STATION14";
    break;
  case ARA_STATION15:
    return "STATION15";
    break;
  case ARA_STATION16:
    return "STATION16";
    break;
  case ARA_STATION17:
    return "STATION17";
    break;
  case ARA_STATION18:
    return "STATION18";
    break;
  case ARA_STATION19:
    return "STATION19";
    break;
  case ARA_STATION20:
    return "STATION20";
    break;
  case ARA_STATION21:
    return "STATION21";
    break;
  case ARA_STATION22:
    return "STATION22";
    break;
  case ARA_STATION23:
    return "STATION23";
    break;
  case ARA_STATION24:
    return "STATION24";
    break;
  case ARA_STATION25:
    return "STATION25";
    break;
  case ARA_STATION26:
    return "STATION26";
    break;
  case ARA_STATION27:
    return "STATION27";
    break;
  case ARA_STATION28:
    return "STATION28";
    break;
  case ARA_STATION29:
    return "STATION29";
    break;
  case ARA_STATION30:
    return "STATION30";
    break;
  case ARA_STATION31:
    return "STATION31";
    break;
  case ARA_STATION32:
    return "STATION32";
    break;
  case ARA_STATION33:
    return "STATION33";
    break;
  case ARA_STATION34:
    return "STATION34";
    break;
  case ARA_STATION35:
    return "STATION35";
    break;
  case ARA_STATION36:
    return "STATION36";
    break;
  case ARA_STATION37:
    return "STATION37";
    break;
  default:
    return "Unkown StationId";
    break;
  }

}


AraStationInfo *AraGeomTool::getStationInfo(AraStationId_t stationId)
{
  int calibIndex=getStationCalibIndex(stationId);
  if(isIcrrStation(stationId)) {
    if(!fStationInfoICRR[calibIndex]) {
      fStationInfoICRR[calibIndex] = new AraStationInfo(stationId);
    }
    if(calibIndex>=0 && calibIndex<ICRR_NO_STATIONS)
      return fStationInfoICRR[calibIndex];
  }
  if(isAtriStation(stationId)) {
    if(!fStationInfoATRI[calibIndex]) {
      fStationInfoATRI[calibIndex] = new AraStationInfo(stationId);
    }
    if(calibIndex>=0 && calibIndex<ATRI_NO_STATIONS)
      return fStationInfoATRI[calibIndex];
  }
  return NULL;
}





AraStationId_t AraGeomTool::getAtriStationId(int stationNumber) {
  ///< Simple utility function
  if(stationNumber==1) return ARA_STATION1B;
  return (AraStationId_t)stationNumber;
}
