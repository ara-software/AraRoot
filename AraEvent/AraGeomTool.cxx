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
#include <sqlite3.h>
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
    readStationInfoICRR[i]=0;
  for(int i=0;i<ATRI_NO_STATIONS;i++) 
    readStationInfoATRI[i]=0;
  

}

AraGeomTool::~AraGeomTool() {
   //Default Destructor
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

//______________________________________________________________________________

void AraGeomTool::readChannelMapDbIcrr(AraStationId_t stationId){
  sqlite3 *db;
  char *zErrMsg = 0;
  sqlite3_stmt *stmt;
  int calibIndex=getStationCalibIndex(stationId);

  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ARA_CALIB_DIR");
  if(!calibEnv) {
     char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
     if(!utilEnv)
        sprintf(calibDir,"calib");
     else
        sprintf(calibDir,"%s/share/araCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }  
  sprintf(fileName, "%s/AntennaInfo.sqlite", calibDir);

  //open the database
  //  int rc = sqlite3_open_v2(fileName, &db, SQLITE_OPEN_READONLY, NULL);
  int rc = sqlite3_open(fileName, &db);;
  if(rc!=SQLITE_OK){
    printf("AraGeomTool::readChannelMapDbIcrr(AraStationId_t stationId) - Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *query;

  //This is where we decide which table to access in the database
  if(stationId==ARA_TESTBED) query = "select * from TestBed";
  if(stationId==ARA_STATION1) query = "select * from Station1";

  //prepare an sql statment which will be used to obtain information from the data base
  //  rc=sqlite3_prepare_v2(db, query, strlen(query)+1, &stmt, NULL);
  rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

  if(rc!=SQLITE_OK){
    printf("statement not prepared OK\n");
    //should close the data base and exit the function
    sqlite3_close(db);
    return;
  }
  int row=0;
  while(1){
    //printf("row number %i\n", row);
    rc=sqlite3_step(stmt);
    if(rc==SQLITE_DONE) break;
    int nColumns=sqlite3_column_count(stmt);
   
     row=sqlite3_column_int(stmt, 2)-1;//forcing the row to be correct
     printf("row number %i\n", row);
     
     AraAntennaInfo *thisAntInfo=fStationInfoICRR[calibIndex].getNewAntennaInfo(row);

    for(int column=0;column<nColumns;column++){

      const char* temp;    

      switch(column){  
      case 0: //primary key - stationId+labChip+channel

	break;
      case 1: //antDir

	temp = (const char*)sqlite3_column_text(stmt, column);

        if(strcmp (temp,"kReceiver")==0){
	  thisAntInfo->antDir=AraAntDir::kReceiver; 
	  //printf("fStationInfoICRR[%i].fAntInfo[%i].antDir %i\n", stationId, row, thisAntInfo->antDir);
	}

	break;
      case 2: //chanNum

	thisAntInfo->chanNum=sqlite3_column_int(stmt, column);


	//printf("fStationInfoICRR[%i].fAntInfo[%i].chanNum %i\n", stationId, row, thisAntInfo->chanNum);
	
	break;
      case 3: //daqChanType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kDisconeChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kDisconeChan;
        if(strcmp (temp,"kBatwingChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kBatwingChan;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].daqChanType %i\n", stationId, row, thisAntInfo->daqChanType);

	break;
      case 4: //daqChanNum

	thisAntInfo->daqChanNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].daqChanNum %i\n", stationId, row, thisAntInfo->daqChanNum);
	
	break;
      case 5: //highPassFilterMhz

	thisAntInfo->highPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].highPassFilterMhz %f\n", stationId, row, thisAntInfo->highPassFilterMhz);

	break;
      case 6: //lowPassFilterMhz
	thisAntInfo->lowPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].lowPassFilterMhz %f\n", stationId, row, thisAntInfo->lowPassFilterMhz);

	break;
      case 7: //daqTrigChan
	thisAntInfo->daqTrigChan=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].daqTrigChan %i\n", stationId, row, thisAntInfo->daqTrigChan);

	break;
      case 8: //numLabChans

	thisAntInfo->numLabChans=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].numLabChans %i\n", stationId, row, thisAntInfo->numLabChans);

	break;
      case 9: //labChip

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kA")==0) thisAntInfo->labChip=AraLabChip::kA;
        if(strcmp (temp,"kB")==0) thisAntInfo->labChip=AraLabChip::kB;
        if(strcmp (temp,"kC")==0) thisAntInfo->labChip=AraLabChip::kC;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].labChip %i\n", stationId, row, thisAntInfo->labChip);

	break;
      case 10: //labChans[0]

	thisAntInfo->labChans[0]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoICRR[%i].fAntInfo[%i].labChans[0] %i\n", stationId, row, thisAntInfo->labChans[0]);

	break;
      case 11: //labChans[1]

	thisAntInfo->labChans[1]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoICRR[%i].fAntInfo[%i].labChans[1] %i\n", stationId, row, thisAntInfo->labChans[1]);

	break;
      case 12: //isDiplexed

	thisAntInfo->isDiplexed=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].isDiplexed %i\n", stationId, row, thisAntInfo->isDiplexed);

	break;
      case 13: //diplexedChans[0]

	thisAntInfo->diplexedChans[0]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].diplexedChans[0] %i\n", stationId, row, thisAntInfo->diplexedChans[0]);

	break;
      case 14: //diplexedChans[1]

	thisAntInfo->diplexedChans[1]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].diplexedChans[1] %i\n", stationId, row, thisAntInfo->diplexedChans[1]);

	break;
      case 15: //preAmpNum

	thisAntInfo->preAmpNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].preAmpNum %i\n", stationId, row, thisAntInfo->preAmpNum);

	break;
      case 16: //avgNoiseFigure

	thisAntInfo->avgNoiseFigure=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].avgNoiseFigure %f\n", stationId, row, thisAntInfo->avgNoiseFigure);

	break;
      case 17: //rcvrNum

	thisAntInfo->rcvrNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].rcvrNum %i\n", stationId, row, thisAntInfo->rcvrNum);

	break;
      case 18: //designator

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->designator, temp, 3);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].designator %s\n", stationId, row, thisAntInfo->designator);

	break;
      case 19: //antPolNum

	thisAntInfo->antPolNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antPolNum %i\n", stationId, row, thisAntInfo->antPolNum);

	break;
      case 20: //antType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kBicone")==0) thisAntInfo->antType=AraAntType::kBicone;
        if(strcmp (temp,"kBowtieSlot")==0) thisAntInfo->antType=AraAntType::kBowtieSlot;
        if(strcmp (temp,"kDiscone")==0) thisAntInfo->antType=AraAntType::kDiscone;
        if(strcmp (temp,"kBatwing")==0) thisAntInfo->antType=AraAntType::kBatwing;
        if(strcmp (temp,"kFatDipole")==0) thisAntInfo->antType=AraAntType::kFatDipole;
        if(strcmp (temp,"kQuadSlot")==0) thisAntInfo->antType=AraAntType::kQuadSlot;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].antType %i\n", stationId, row, thisAntInfo->antType);

	break;
      case 21: //polType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kVertical")==0) thisAntInfo->polType=AraAntPol::kVertical;
        if(strcmp (temp,"kHorizontal")==0) thisAntInfo->polType=AraAntPol::kHorizontal;
        if(strcmp (temp,"kSurface")==0) thisAntInfo->polType=AraAntPol::kSurface;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].AraAntPol %i\n", stationId, row, thisAntInfo->polType);

	break;
      case 22: //locationName

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->locationName, temp, 4);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].locationName %s\n", stationId, row, thisAntInfo->locationName);


	break;
      case 23: //antLocation[0]

	thisAntInfo->antLocation[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antLocation[0] %f\n", stationId, row, thisAntInfo->antLocation[0]);

	break;
      case 24: //antLocation[1]

	thisAntInfo->antLocation[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antLocation[1] %f\n", stationId, row, thisAntInfo->antLocation[1]);

	break;
      case 25: //antLocation[2]

	thisAntInfo->antLocation[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antLocation[2] %f\n", stationId, row, thisAntInfo->antLocation[2]);

	break;
      case 26: //cableDelay

	thisAntInfo->cableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].cableDelay %f\n", stationId, row, thisAntInfo->cableDelay);

	break;
      case 27: //debugHolePosition[0]

	thisAntInfo->debugHolePosition[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePosition[0] %f\n", stationId, row, thisAntInfo->debugHolePosition[0]);

	break;
      case 28: //debugHolePosition[1]

	thisAntInfo->debugHolePosition[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePosition[1] %f\n", stationId, row, thisAntInfo->debugHolePosition[1]);

	break;
      case 29: //debugHolePosition[2]

	thisAntInfo->debugHolePosition[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePosition[2] %f\n", stationId, row, thisAntInfo->debugHolePosition[2]);

	break;
      case 30: //debugPreAmpDz

	thisAntInfo->debugPreAmpDz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugPreAmpDz %f\n", stationId, row, thisAntInfo->debugPreAmpDz);

	break;
      case 31: //debugHolePositionZft

	thisAntInfo->debugHolePositionZft=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePositionZft %f\n", stationId, row, thisAntInfo->debugHolePositionZft);

	break;
      case 32: //debugHolePositionZm

	thisAntInfo->debugHolePositionZm=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePositionZm %f\n", stationId, row, thisAntInfo->debugHolePositionZm);

	break;
      case 33: //debugTrueAsBuiltPosition[0]

	thisAntInfo->debugTrueAsBuiltPositon[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTrueAsBuiltPositon[0] %f\n", stationId, row, thisAntInfo->debugTrueAsBuiltPositon[0]);

	break;
      case 34: //debugTrueAsBuiltPosition[1]

	thisAntInfo->debugTrueAsBuiltPositon[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTrueAsBuiltPositon[1] %f\n", stationId, row, thisAntInfo->debugTrueAsBuiltPositon[1]);

	break;
      case 35: //debugTrueAsBuiltPosition[2]

	thisAntInfo->debugTrueAsBuiltPositon[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTrueAsBuiltPositon[2] %f\n", stationId, row, thisAntInfo->debugTrueAsBuiltPositon[2]);

	break;
      case 36: //debugCableDelay2

	thisAntInfo->debugCableDelay2=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugCableDelay2 %f\n", stationId, row, thisAntInfo->debugCableDelay2);

	break;
      case 37: //debugFeedPointDelay

	thisAntInfo->debugFeedPointDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugFeedPointDelay %f\n", stationId, row, thisAntInfo->debugFeedPointDelay);

	break;
      case 38: //debugTotalCableDelay

	thisAntInfo->debugTotalCableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTotalCableDelay %f\n", stationId, row, thisAntInfo->debugTotalCableDelay);

	break;
      case 40: //antOrient[0]

	thisAntInfo->antOrient[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antOrient[0] %1.10f\n", stationId, row, thisAntInfo->antOrient[0]);

	break;

      case 41: //antOrient[1]

	thisAntInfo->antOrient[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antOrient[1] %1.10f\n", stationId, row, thisAntInfo->antOrient[1]);

	break;

      case 42: //antOrient[2]

	thisAntInfo->antOrient[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antOrient[2] %1.10f\n", stationId, row, thisAntInfo->antOrient[2]);

	break;


      default:

	break;

      }//switch(column)

    }//column


  }//while(1)
  //now insert the no of rfchannels

  // if(stationId==ARA_TESTBED)  {
  //   fStationInfoICRR[0].setNumRFChans(RFCHANS_TESTBED);
  //   fStationInfoICRR[0].setNumAnts(RFCHANS_TESTBED);
  // }
  // else if(stationId==ARA_STATION1)  {
  //   fStationInfoICRR[1].setNumRFChans(RFCHANS_STATION1);
  //   fStationInfoICRR[1].setNumAnts(RFCHANS_STATION1);
  // }

  //now need to destroy the sqls statement prepared earlier
  rc = sqlite3_finalize(stmt);
  if(rc!=SQLITE_OK) printf("error finlizing sql statement\n");
  //  printf("sqlite3_finalize(stmt) = %i\n", rc);

  //now close the connection to the database
  rc = sqlite3_close(db);
  if(rc!=SQLITE_OK) printf("error closing db\n");

  fStationInfoICRR[calibIndex].fillAntIndexVec();

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
    if(!readStationInfoICRR[calibIndex]) {
      //fprintf(stderr, "readChannelMapDbIcrr\n");
      readChannelMapDbIcrr(stationId);
      readStationInfoICRR[calibIndex]=1;
    }
    if(calibIndex>=0 && calibIndex<ICRR_NO_STATIONS)
      return &fStationInfoICRR[calibIndex];
  }
  if(isAtriStation(stationId)) {
    if(!readStationInfoATRI[calibIndex]) {
      //fprintf(stderr, "readChannelMapDbAtri\n");
      readChannelMapDbAtri(stationId);
      readStationInfoATRI[calibIndex]=1;
    }
    if(calibIndex>=0 && calibIndex<ATRI_NO_STATIONS)
      return &fStationInfoATRI[calibIndex];
  }
  return NULL;
}


void AraGeomTool::readChannelMapDbAtri(AraStationId_t stationId){
  sqlite3 *db;
  char *zErrMsg = 0;
  sqlite3_stmt *stmt;
  int calibIndex=getStationCalibIndex(stationId);

  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ARA_CALIB_DIR");
  if(!calibEnv) {
     char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
     if(!utilEnv)
        sprintf(calibDir,"calib");
     else
        sprintf(calibDir,"%s/share/araCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }  
  sprintf(fileName, "%s/AntennaInfo.sqlite", calibDir);

  //open the database
  //  int rc = sqlite3_open_v2(fileName, &db, SQLITE_OPEN_READONLY, NULL);
  int rc = sqlite3_open(fileName, &db);;
  if(rc!=SQLITE_OK){
    printf("AraGeomTool::readChannelMapDbAtri(AraStationId_t stationId) - Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *query;

  //This is where we decide which table to access in the database
  if(stationId==ARA_STATION1B) query = "select * from ARA01";
  else if(stationId==ARA_STATION2) query = "select * from ARA02";
  else if(stationId==ARA_STATION3) query = "select * from ARA03";
  else{
    fprintf(stderr, "%s : stationId %i is not ARA1-3\n", __FUNCTION__, stationId);
    return;
  }
  //prepare an sql statment which will be used to obtain information from the data base
  //  rc=sqlite3_prepare_v2(db, query, strlen(query)+1, &stmt, NULL);
  rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

  if(rc!=SQLITE_OK){
    printf("statement not prepared OK\n");
    //should close the data base and exit the function
    sqlite3_close(db);
    return;
  }

  //Insert hard coded number of antennas for now
  //  fStationInfoATRI[calibIndex].setNumAnts(ANTS_PER_ATRI);
  //  fStationInfoATRI[calibIndex].setNumRFChans(ANTS_PER_ATRI);  ///< Not sure wht value this should be



  int row=0;
  while(1){
    //printf("row number %i\n", row);
    rc=sqlite3_step(stmt);
    if(rc==SQLITE_DONE) break;
    int nColumns=sqlite3_column_count(stmt);

    row=sqlite3_column_int(stmt, 2)-1;//forcing the row to be correct
    //printf("row number %i\n", row);
    AraAntennaInfo *thisAntInfo=fStationInfoATRI[calibIndex].getNewAntennaInfo(row);

    for(int column=0;column<nColumns;column++){

      const char* temp;    

      switch(column){  
      case 0: //primary key - stationId+labChip+channel

	break;
      case 1: //antDir

	temp = (const char*)sqlite3_column_text(stmt, column);

        if(strcmp (temp,"kReceiver")==0){
	  thisAntInfo->antDir=AraAntDir::kReceiver; 
	  //printf("fStationInfoATRI[%i].fAntInfo[%i].antDir %i\n", stationId, row, thisAntInfo->antDir);
	}

	break;
      case 2: //chanNum

	thisAntInfo->chanNum=sqlite3_column_int(stmt, column);


	//printf("fStationInfoATRI[%i].fAntInfo[%i].chanNum %i\n", stationId, row, thisAntInfo->chanNum);
	
	break;
      case 3: //daqChanType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kDisconeChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kDisconeChan;
        if(strcmp (temp,"kBatwingChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kBatwingChan;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].daqChanType %i\n", stationId, row, thisAntInfo->daqChanType);

	break;
      case 4: //daqChanNum

	thisAntInfo->daqChanNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].daqChanNum %i\n", stationId, row, thisAntInfo->daqChanNum);
	
	break;
      case 5: //highPassFilterMhz

	thisAntInfo->highPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].highPassFilterMhz %f\n", stationId, row, thisAntInfo->highPassFilterMhz);

	break;
      case 6: //lowPassFilterMhz
	thisAntInfo->lowPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].lowPassFilterMhz %f\n", stationId, row, thisAntInfo->lowPassFilterMhz);

	break;
      case 7: //daqTrigChan
	thisAntInfo->daqTrigChan=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].daqTrigChan %i\n", stationId, row, thisAntInfo->daqTrigChan);

	break;
      case 8: //numLabChans

	thisAntInfo->numLabChans=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].numLabChans %i\n", stationId, row, thisAntInfo->numLabChans);

	break;
      case 9: //labChip

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kA")==0) thisAntInfo->labChip=AraLabChip::kA;
        if(strcmp (temp,"kB")==0) thisAntInfo->labChip=AraLabChip::kB;
        if(strcmp (temp,"kC")==0) thisAntInfo->labChip=AraLabChip::kC;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].labChip %i\n", stationId, row, thisAntInfo->labChip);

	break;
      case 10: //labChans[0]

	thisAntInfo->labChans[0]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoATRI[%i].fAntInfo[%i].labChans[0] %i\n", stationId, row, thisAntInfo->labChans[0]);

	break;
      case 11: //labChans[1]

	thisAntInfo->labChans[1]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoATRI[%i].fAntInfo[%i].labChans[1] %i\n", stationId, row, thisAntInfo->labChans[1]);

	break;
      case 12: //isDiplexed

	thisAntInfo->isDiplexed=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].isDiplexed %i\n", stationId, row, thisAntInfo->isDiplexed);

	break;
      case 13: //diplexedChans[0]

	thisAntInfo->diplexedChans[0]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].diplexedChans[0] %i\n", stationId, row, thisAntInfo->diplexedChans[0]);

	break;
      case 14: //diplexedChans[1]

	thisAntInfo->diplexedChans[1]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].diplexedChans[1] %i\n", stationId, row, thisAntInfo->diplexedChans[1]);

	break;
      case 15: //preAmpNum

	thisAntInfo->preAmpNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].preAmpNum %i\n", stationId, row, thisAntInfo->preAmpNum);

	break;
      case 16: //avgNoiseFigure

	thisAntInfo->avgNoiseFigure=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].avgNoiseFigure %f\n", stationId, row, thisAntInfo->avgNoiseFigure);

	break;
      case 17: //rcvrNum

	thisAntInfo->rcvrNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].rcvrNum %i\n", stationId, row, thisAntInfo->rcvrNum);

	break;
      case 18: //designator

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->designator, temp, 3);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].designator %s\n", stationId, row, thisAntInfo->designator);

	break;
      case 19: //antPolNum

	thisAntInfo->antPolNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antPolNum %i\n", stationId, row, thisAntInfo->antPolNum);

	break;
      case 20: //antType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kBicone")==0) thisAntInfo->antType=AraAntType::kBicone;
        if(strcmp (temp,"kBowtieSlot")==0) thisAntInfo->antType=AraAntType::kBowtieSlot;
        if(strcmp (temp,"kDiscone")==0) thisAntInfo->antType=AraAntType::kDiscone;
        if(strcmp (temp,"kBatwing")==0) thisAntInfo->antType=AraAntType::kBatwing;
        if(strcmp (temp,"kFatDipole")==0) thisAntInfo->antType=AraAntType::kFatDipole;
        if(strcmp (temp,"kQuadSlot")==0) thisAntInfo->antType=AraAntType::kQuadSlot;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].antType %i\n", stationId, row, thisAntInfo->antType);

	break;
      case 21: //polType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kVertical")==0) thisAntInfo->polType=AraAntPol::kVertical;
        if(strcmp (temp,"kHorizontal")==0) thisAntInfo->polType=AraAntPol::kHorizontal;
        if(strcmp (temp,"kSurface")==0) thisAntInfo->polType=AraAntPol::kSurface;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].AraAntPol %i\n", stationId, row, thisAntInfo->polType);

	break;
      case 22: //locationName

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->locationName, temp, 4);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].locationName %s\n", stationId, row, thisAntInfo->locationName);


	break;
      case 23: //antLocation[0]

	thisAntInfo->antLocation[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[0] %f\n", stationId, row, thisAntInfo->antLocation[0]);

	break;
      case 24: //antLocation[1]

	thisAntInfo->antLocation[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[1] %f\n", stationId, row, thisAntInfo->antLocation[1]);

	break;
      case 25: //antLocation[2]

	thisAntInfo->antLocation[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[2] %f\n", stationId, row, thisAntInfo->antLocation[2]);

	break;
      case 26: //cableDelay

	thisAntInfo->cableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].cableDelay %f\n", stationId, row, thisAntInfo->cableDelay);

	break;
      case 27: //debugHolePosition[0]

	thisAntInfo->debugHolePosition[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePosition[0] %f\n", stationId, row, thisAntInfo->debugHolePosition[0]);

	break;
      case 28: //debugHolePosition[1]

	thisAntInfo->debugHolePosition[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePosition[1] %f\n", stationId, row, thisAntInfo->debugHolePosition[1]);

	break;
      case 29: //debugHolePosition[2]

	thisAntInfo->debugHolePosition[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePosition[2] %f\n", stationId, row, thisAntInfo->debugHolePosition[2]);

	break;
      case 30: //debugPreAmpDz

	thisAntInfo->debugPreAmpDz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugPreAmpDz %f\n", stationId, row, thisAntInfo->debugPreAmpDz);

	break;
      case 31: //debugHolePositionZft

	thisAntInfo->debugHolePositionZft=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePositionZft %f\n", stationId, row, thisAntInfo->debugHolePositionZft);

	break;
      case 32: //debugHolePositionZm

	thisAntInfo->debugHolePositionZm=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePositionZm %f\n", stationId, row, thisAntInfo->debugHolePositionZm);

	break;
      case 33: //debugTrueAsBuiltPosition[0]

	thisAntInfo->debugTrueAsBuiltPositon[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTrueAsBuiltPositon[0] %f\n", stationId, row, thisAntInfo->debugTrueAsBuiltPositon[0]);

	break;
      case 34: //debugTrueAsBuiltPosition[1]

	thisAntInfo->debugTrueAsBuiltPositon[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTrueAsBuiltPositon[1] %f\n", stationId, row, thisAntInfo->debugTrueAsBuiltPositon[1]);

	break;
      case 35: //debugTrueAsBuiltPosition[2]

	thisAntInfo->debugTrueAsBuiltPositon[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTrueAsBuiltPositon[2] %f\n", stationId, row, thisAntInfo->debugTrueAsBuiltPositon[2]);

	break;
      case 36: //debugCableDelay2

	thisAntInfo->debugCableDelay2=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugCableDelay2 %f\n", stationId, row, thisAntInfo->debugCableDelay2);

	break;
      case 37: //debugFeedPointDelay

	thisAntInfo->debugFeedPointDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugFeedPointDelay %f\n", stationId, row, thisAntInfo->debugFeedPointDelay);

	break;
      case 38: //debugTotalCableDelay

	thisAntInfo->debugTotalCableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTotalCableDelay %f\n", stationId, row, thisAntInfo->debugTotalCableDelay);

	break;
      case 40: //antOrient[0]

	thisAntInfo->antOrient[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antOrient[0] %1.10f\n", stationId, row, thisAntInfo->antOrient[0]);

	break;

      case 41: //antOrient[1]

	thisAntInfo->antOrient[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antOrient[1] %1.10f\n", stationId, row, thisAntInfo->antOrient[1]);

	break;

      case 42: //antOrient[2]

	thisAntInfo->antOrient[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antOrient[2] %1.10f\n", stationId, row, thisAntInfo->antOrient[2]);

	break;


      default:

	break;

      }//switch(column)

    }//column
  }//while(1)
  //now insert the no of rfchannels

  //now need to destroy the sqls statement prepared earlier
  rc = sqlite3_finalize(stmt);
  if(rc!=SQLITE_OK) printf("error finlizing sql statement\n");
  //  printf("sqlite3_finalize(stmt) = %i\n", rc);

  //now close the connection to the database
  rc = sqlite3_close(db);
  if(rc!=SQLITE_OK) printf("error closing db\n");

  fStationInfoATRI[calibIndex].fillAntIndexVec();

}


AraStationId_t AraGeomTool::getAtriStationId(int stationNumber) {
  ///< Simple utility function
  if(stationNumber==1) return ARA_STATION1B;
  return (AraStationId_t)stationNumber;
}
