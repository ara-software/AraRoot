//////////////////////////////////////////////////////////////////////////////
/////  AraStationInfo.h       ARA Station Information                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about an Ara Stations   /////
/////  Author: Jonathan Davies (jdavies@hep.ucl.ac.uk)                   /////
/////          & Ryan Nichol (rjn@hep.ucl.ac.uk)                         /////
//////////////////////////////////////////////////////////////////////////////

//Includes
#include <iostream>
#include <sqlite3.h>

//Ara Includes
#include "AraStationInfo.h"
#include "AraGeomTool.h"





ClassImp(AraStationInfo);

AraStationInfo::AraStationInfo()
  :fAntInfo(ANTS_PER_ATRI),fCalAntInfo(CAL_ANTS_PER_ATRI)
{
  numberRFChans=0;
  fNumberAntennas=0;
  fNumberCalAntennas=0;

}


AraStationInfo::AraStationInfo(AraStationId_t stationId)
  :fAntInfo(ANTS_PER_ATRI),fCalAntInfo(CAL_ANTS_PER_ATRI)
{
  fStationId=stationId;
  numberRFChans=0;
  fNumberAntennas=0;
  fNumberCalAntennas=0;
  if(AraGeomTool::isIcrrStation(fStationId)) {
    readChannelMapDbIcrr();
  }
  else {
    readChannelMapDbAtri_2();
    //    readChannelMapDbAtri();
  }
  readCalPulserDb();

}



AraStationInfo::~AraStationInfo()
{

}

Double_t AraStationInfo::getCableDelay(int rfChanNum) {
  if(rfChanNum>=0 && rfChanNum<numberRFChans) {
    return fAntInfo[rfChanNum].getCableDelay();
  }
  return 0;
}

AraCalAntennaInfo *AraStationInfo::getCalAntennaInfo(int calAntId) {
  if(calAntId>=0 && calAntId<=fNumberCalAntennas) 
    return &fCalAntInfo[calAntId];
  return NULL;
}

AraAntennaInfo *AraStationInfo::getAntennaInfoForTrigChan(int trigChan) {

   if(trigChan>=0 && trigChan<fTrigChanVec.size()) {
      return &fAntInfo[fTrigChanVec[trigChan]];
   }
   return NULL;
}


AraAntennaInfo *AraStationInfo::getAntennaInfo(int antNum) {
  if(antNum>=0 && antNum<numberRFChans) {
    return &fAntInfo[antNum];
  }
  return NULL;
}

AraAntennaInfo *AraStationInfo::getNewAntennaInfo(int antNum){
  //Assume this creates a new AraAntennaInfo; 
  fNumberAntennas++;
  numberRFChans++;
  //  std::cout << "getNewAntennaInfo(" << antNum << ") fNumberAntennas=" << fNumberAntennas << "\n";
  //Magic lines below, may remove at some point
  int temp=fAntInfo[antNum].chanNum;
  fAntInfo[antNum].chanNum=temp;
  fAntInfo[antNum].fStationId=fStationId;
  return &fAntInfo[antNum];
}


AraCalAntennaInfo *AraStationInfo::getNewCalAntennaInfo(int calAntId){
  //Assume this creates a new AraCalAntennaInfo; 
  fNumberCalAntennas++;
  //  std::cout << "getNewCalAntennaInfo(" << calAntId << ") fNumberCalAntennas=" << fNumberCalAntennas << "\n";
  //Magic lines below, may remove at some point
  //  int temp=fCalAntInfo[calAntId].calAntId;
  fCalAntInfo[calAntId].calAntId=calAntId;
  fCalAntInfo[calAntId].fStationId=fStationId;
  return &fCalAntInfo[calAntId];
}

void AraStationInfo::fillTrigChanVec() {
   fTrigChanVec.resize(fNumberAntennas);
   for(int ant=0;ant<fNumberAntennas;++ant){
      int trigChan=fAntInfo[ant].getTrigChan();
      if(trigChan<=fNumberAntennas) {
	 fTrigChanVec[trigChan]=ant;
      }
      else {
	 std::cerr << "Got trigger channel: " << trigChan << " but only have " << fNumberAntennas << " antennas";
      }
      
   }
}

void AraStationInfo::fillAntIndexVec() {
  //For now will use hard coded numbers
  fAntIndexVec[0].resize(20);
  fAntIndexVec[1].resize(20);
  fAntIndexVec[2].resize(20);

  Int_t countPols[3]={0};
  for(int ant=0;ant<fNumberAntennas;++ant){
    int polTypeInt=(int)fAntInfo[ant].polType;
    int antNum=fAntInfo[ant].antPolNum;
    int daqNum=fAntInfo[ant].daqChanNum;
    //    std::cerr << ant << "\t" << polTypeInt << "\t" << antNum << "\t" << daqNum << "\n";
    fAntIndexVec[polTypeInt][antNum]=ant;//daqNum;
    countPols[polTypeInt]++;
  }
  for(int pol=0;pol<3;pol++) {
    fAntIndexVec[pol].resize(countPols[pol]);
  }
}

Int_t AraStationInfo::getRFChanByPolAndAnt(Int_t antNum, AraAntPol::AraAntPol_t polType) {
  //RJN will add checking here at some point
  if(polType>=0 && polType<=2) {
    if(antNum>=0 && antNum<=fAntIndexVec[polType].size()) {
      return fAntIndexVec[polType][antNum];
    }
  }
  return -1;
}


Int_t AraStationInfo::getElecChanFromRFChan(Int_t rfChan) {
  //Should add error checking here
  if(rfChan >=fAntInfo.size()){
    fprintf(stderr, "%s : rfChan %i too high\n", __FUNCTION__, rfChan);
    return -1;
  }
  
  return fAntInfo[rfChan].daqChanNum;
}




void AraStationInfo::readChannelMapDbAtri(){
  sqlite3 *db;
  char *zErrMsg = 0;
  sqlite3_stmt *stmt;
  //  int calibIndex=getStationCalibIndex(fStationId);

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
    printf("AraStationInfo::readChannelMapDbAtri() - Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *query;

  //This is where we decide which table to access in the database
  if(fStationId==ARA_STATION1B) query = "select * from ARA01";
  else if(fStationId==ARA_STATION2) query = "select * from ARA02";
  else if(fStationId==ARA_STATION3) query = "select * from ARA03";
  // Added by Rishabh to test data from ARA04
  else if(fStationId==ARA_STATION4) query = "select * for ARA04";
  else if(fStationId==ARA_STATION5) query = "select * for ARA05";
  else if(fStationId==ARA_STATION6) query = "select * for ARA06";
  else{
    fprintf(stderr, "%s : fStationId %i is not ARA1-3\n", __FUNCTION__, fStationId);
    return;
  }

  //prepare an sql statment which will be used to obtain information from the data base
  rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

  if(rc!=SQLITE_OK){
    printf("AraStationInfo::readChannelMapDbAtri() : statement not prepared OK\n");
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
    //printf("row number %i\n", row);
    AraAntennaInfo *thisAntInfo=this->getNewAntennaInfo(row);

    for(int column=0;column<nColumns;column++){

      const char* temp;    

      switch(column){  
      case 0: //primary key - fStationId+labChip+channel

	break;
      case 1: //antDir

	temp = (const char*)sqlite3_column_text(stmt, column);

        if(strcmp (temp,"kReceiver")==0){
	  thisAntInfo->antDir=AraAntDir::kReceiver; 
	  //printf("fStationInfoATRI[%i].fAntInfo[%i].antDir %i\n", fStationId, row, thisAntInfo->antDir);
	}

	break;
      case 2: //chanNum

	thisAntInfo->chanNum=sqlite3_column_int(stmt, column);


	//printf("fStationInfoATRI[%i].fAntInfo[%i].chanNum %i\n", fStationId, row, thisAntInfo->chanNum);
	
	break;
      case 3: //daqChanType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kDisconeChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kDisconeChan;
        if(strcmp (temp,"kBatwingChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kBatwingChan;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].daqChanType %i\n", fStationId, row, thisAntInfo->daqChanType);

	break;
      case 4: //daqChanNum

	thisAntInfo->daqChanNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].daqChanNum %i\n", fStationId, row, thisAntInfo->daqChanNum);
	
	break;
      case 5: //highPassFilterMhz

	thisAntInfo->highPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].highPassFilterMhz %f\n", fStationId, row, thisAntInfo->highPassFilterMhz);

	break;
      case 6: //lowPassFilterMhz
	thisAntInfo->lowPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].lowPassFilterMhz %f\n", fStationId, row, thisAntInfo->lowPassFilterMhz);

	break;
      case 7: //daqTrigChan
	thisAntInfo->daqTrigChan=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].daqTrigChan %i\n", fStationId, row, thisAntInfo->daqTrigChan);

	break;
      case 8: //numLabChans

	thisAntInfo->numLabChans=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].numLabChans %i\n", fStationId, row, thisAntInfo->numLabChans);

	break;
      case 9: //labChip

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kA")==0) thisAntInfo->labChip=AraLabChip::kA;
        if(strcmp (temp,"kB")==0) thisAntInfo->labChip=AraLabChip::kB;
        if(strcmp (temp,"kC")==0) thisAntInfo->labChip=AraLabChip::kC;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].labChip %i\n", fStationId, row, thisAntInfo->labChip);

	break;
      case 10: //labChans[0]

	thisAntInfo->labChans[0]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoATRI[%i].fAntInfo[%i].labChans[0] %i\n", fStationId, row, thisAntInfo->labChans[0]);

	break;
      case 11: //labChans[1]

	thisAntInfo->labChans[1]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoATRI[%i].fAntInfo[%i].labChans[1] %i\n", fStationId, row, thisAntInfo->labChans[1]);

	break;
      case 12: //isDiplexed

	thisAntInfo->isDiplexed=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].isDiplexed %i\n", fStationId, row, thisAntInfo->isDiplexed);

	break;
      case 13: //diplexedChans[0]

	thisAntInfo->diplexedChans[0]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].diplexedChans[0] %i\n", fStationId, row, thisAntInfo->diplexedChans[0]);

	break;
      case 14: //diplexedChans[1]

	thisAntInfo->diplexedChans[1]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].diplexedChans[1] %i\n", fStationId, row, thisAntInfo->diplexedChans[1]);

	break;
      case 15: //preAmpNum

	thisAntInfo->preAmpNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].preAmpNum %i\n", fStationId, row, thisAntInfo->preAmpNum);

	break;
      case 16: //avgNoiseFigure

	thisAntInfo->avgNoiseFigure=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].avgNoiseFigure %f\n", fStationId, row, thisAntInfo->avgNoiseFigure);

	break;
      case 17: //rcvrNum

	thisAntInfo->rcvrNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].rcvrNum %i\n", fStationId, row, thisAntInfo->rcvrNum);

	break;
      case 18: //designator

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->designator, temp, 3);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].designator %s\n", fStationId, row, thisAntInfo->designator);

	break;
      case 19: //antPolNum

	thisAntInfo->antPolNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antPolNum %i\n", fStationId, row, thisAntInfo->antPolNum);

	break;
      case 20: //antType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kBicone")==0) thisAntInfo->antType=AraAntType::kBicone;
        if(strcmp (temp,"kBowtieSlot")==0) thisAntInfo->antType=AraAntType::kBowtieSlot;
        if(strcmp (temp,"kDiscone")==0) thisAntInfo->antType=AraAntType::kDiscone;
        if(strcmp (temp,"kBatwing")==0) thisAntInfo->antType=AraAntType::kBatwing;
        if(strcmp (temp,"kFatDipole")==0) thisAntInfo->antType=AraAntType::kFatDipole;
        if(strcmp (temp,"kQuadSlot")==0) thisAntInfo->antType=AraAntType::kQuadSlot;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].antType %i\n", fStationId, row, thisAntInfo->antType);

	break;
      case 21: //polType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kVertical")==0) thisAntInfo->polType=AraAntPol::kVertical;
        if(strcmp (temp,"kHorizontal")==0) thisAntInfo->polType=AraAntPol::kHorizontal;
        if(strcmp (temp,"kSurface")==0) thisAntInfo->polType=AraAntPol::kSurface;

	//printf("fStationInfoATRI[%i].fAntInfo[%i].AraAntPol %i\n", fStationId, row, thisAntInfo->polType);

	break;
      case 22: //locationName

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->locationName, temp, 4);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].locationName %s\n", fStationId, row, thisAntInfo->locationName);


	break;
      case 23: //antLocation[0]

	thisAntInfo->antLocation[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[0] %f\n", fStationId, row, thisAntInfo->antLocation[0]);

	break;
      case 24: //antLocation[1]

	thisAntInfo->antLocation[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[1] %f\n", fStationId, row, thisAntInfo->antLocation[1]);

	break;
      case 25: //antLocation[2]

	thisAntInfo->antLocation[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[2] %f\n", fStationId, row, thisAntInfo->antLocation[2]);

	break;
      case 26: //cableDelay

	thisAntInfo->cableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].cableDelay %f\n", fStationId, row, thisAntInfo->cableDelay);

	break;
      case 27: //debugHolePosition[0]

	thisAntInfo->debugHolePosition[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePosition[0] %f\n", fStationId, row, thisAntInfo->debugHolePosition[0]);

	break;
      case 28: //debugHolePosition[1]

	thisAntInfo->debugHolePosition[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePosition[1] %f\n", fStationId, row, thisAntInfo->debugHolePosition[1]);

	break;
      case 29: //debugHolePosition[2]

	thisAntInfo->debugHolePosition[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePosition[2] %f\n", fStationId, row, thisAntInfo->debugHolePosition[2]);

	break;
      case 30: //debugPreAmpDz

	thisAntInfo->debugPreAmpDz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugPreAmpDz %f\n", fStationId, row, thisAntInfo->debugPreAmpDz);

	break;
      case 31: //debugHolePositionZft

	thisAntInfo->debugHolePositionZft=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePositionZft %f\n", fStationId, row, thisAntInfo->debugHolePositionZft);

	break;
      case 32: //debugHolePositionZm

	thisAntInfo->debugHolePositionZm=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugHolePositionZm %f\n", fStationId, row, thisAntInfo->debugHolePositionZm);

	break;
      case 33: //debugTrueAsBuiltPosition[0]

	thisAntInfo->debugTrueAsBuiltPositon[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTrueAsBuiltPositon[0] %f\n", fStationId, row, thisAntInfo->debugTrueAsBuiltPositon[0]);

	break;
      case 34: //debugTrueAsBuiltPosition[1]

	thisAntInfo->debugTrueAsBuiltPositon[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTrueAsBuiltPositon[1] %f\n", fStationId, row, thisAntInfo->debugTrueAsBuiltPositon[1]);

	break;
      case 35: //debugTrueAsBuiltPosition[2]

	thisAntInfo->debugTrueAsBuiltPositon[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTrueAsBuiltPositon[2] %f\n", fStationId, row, thisAntInfo->debugTrueAsBuiltPositon[2]);

	break;
      case 36: //debugCableDelay2

	thisAntInfo->debugCableDelay2=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugCableDelay2 %f\n", fStationId, row, thisAntInfo->debugCableDelay2);

	break;
      case 37: //debugFeedPointDelay

	thisAntInfo->debugFeedPointDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugFeedPointDelay %f\n", fStationId, row, thisAntInfo->debugFeedPointDelay);

	break;
      case 38: //debugTotalCableDelay

	thisAntInfo->debugTotalCableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].debugTotalCableDelay %f\n", fStationId, row, thisAntInfo->debugTotalCableDelay);

	break;
      case 40: //antOrient[0]

	thisAntInfo->antOrient[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antOrient[0] %1.10f\n", fStationId, row, thisAntInfo->antOrient[0]);

	break;

      case 41: //antOrient[1]

	thisAntInfo->antOrient[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antOrient[1] %1.10f\n", fStationId, row, thisAntInfo->antOrient[1]);

	break;

      case 42: //antOrient[2]

	thisAntInfo->antOrient[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antOrient[2] %1.10f\n", fStationId, row, thisAntInfo->antOrient[2]);

	break;


      default:

	break;

      }//switch(column)

    }//column
    thisAntInfo->fillArrayCoords(); ///< Fill the array-centric coordintes

  }//while(1)

  //now need to destroy the sqls statement prepared earlier
  rc = sqlite3_finalize(stmt);
  if(rc!=SQLITE_OK) printf("error finlizing sql statement\n");
  //  printf("sqlite3_finalize(stmt) = %i\n", rc);

  //now close the connection to the database
  rc = sqlite3_close(db);
  if(rc!=SQLITE_OK) printf("error closing db\n");

  this->fillAntIndexVec();
  this->fillTrigChanVec();

}


//______________________________________________________________________________

void AraStationInfo::readChannelMapDbIcrr(){
  sqlite3 *db;
  char *zErrMsg = 0;
  sqlite3_stmt *stmt;
  //  int calibIndex=getStationCalibIndex(fStationId);

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
    printf("AraStationInfo::readChannelMapDbIcrr() - Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *query;

  //This is where we decide which table to access in the database
  if(fStationId==ARA_TESTBED) query = "select * from TestBed";
  if(fStationId==ARA_STATION1) query = "select * from Station1";

  //prepare an sql statment which will be used to obtain information from the data base
  //  rc=sqlite3_prepare_v2(db, query, strlen(query)+1, &stmt, NULL);
  rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

  if(rc!=SQLITE_OK){
    printf("AraStationInfo::readChannelMapDbIcrr() : statement not prepared OK\n");
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
     //printf("row number %i\n", row);
     
     AraAntennaInfo *thisAntInfo=this->getNewAntennaInfo(row);

    for(int column=0;column<nColumns;column++){

      const char* temp;    

      switch(column){  
      case 0: //primary key - fStationId+labChip+channel

	break;
      case 1: //antDir

	temp = (const char*)sqlite3_column_text(stmt, column);

        if(strcmp (temp,"kReceiver")==0){
	  thisAntInfo->antDir=AraAntDir::kReceiver; 
	  //printf("fStationInfoICRR[%i].fAntInfo[%i].antDir %i\n", fStationId, row, thisAntInfo->antDir);
	}

	break;
      case 2: //chanNum

	thisAntInfo->chanNum=sqlite3_column_int(stmt, column);


	//printf("fStationInfoICRR[%i].fAntInfo[%i].chanNum %i\n", fStationId, row, thisAntInfo->chanNum);
	
	break;
      case 3: //daqChanType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kDisconeChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kDisconeChan;
        if(strcmp (temp,"kBatwingChan")==0) thisAntInfo->daqChanType=AraDaqChanType::kBatwingChan;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].daqChanType %i\n", fStationId, row, thisAntInfo->daqChanType);

	break;
      case 4: //daqChanNum

	thisAntInfo->daqChanNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].daqChanNum %i\n", fStationId, row, thisAntInfo->daqChanNum);
	
	break;
      case 5: //highPassFilterMhz

	thisAntInfo->highPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].highPassFilterMhz %f\n", fStationId, row, thisAntInfo->highPassFilterMhz);

	break;
      case 6: //lowPassFilterMhz
	thisAntInfo->lowPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].lowPassFilterMhz %f\n", fStationId, row, thisAntInfo->lowPassFilterMhz);

	break;
      case 7: //daqTrigChan
	thisAntInfo->daqTrigChan=sqlite3_column_int(stmt, column);
	//RJN feature for now
	if(thisAntInfo->daqChanType==AraDaqChanType::kBatwingChan)
	  thisAntInfo->daqTrigChan+=8;
	//printf("fStationInfoICRR[%i].fAntInfo[%i].daqTrigChan %i\n", fStationId, row, thisAntInfo->daqTrigChan);

	break;
      case 8: //numLabChans

	thisAntInfo->numLabChans=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].numLabChans %i\n", fStationId, row, thisAntInfo->numLabChans);

	break;
      case 9: //labChip

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kA")==0) thisAntInfo->labChip=AraLabChip::kA;
        if(strcmp (temp,"kB")==0) thisAntInfo->labChip=AraLabChip::kB;
        if(strcmp (temp,"kC")==0) thisAntInfo->labChip=AraLabChip::kC;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].labChip %i\n", fStationId, row, thisAntInfo->labChip);

	break;
      case 10: //labChans[0]

	thisAntInfo->labChans[0]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoICRR[%i].fAntInfo[%i].labChans[0] %i\n", fStationId, row, thisAntInfo->labChans[0]);

	break;
      case 11: //labChans[1]

	thisAntInfo->labChans[1]=sqlite3_column_int(stmt, column)-1;
	//printf("fStationInfoICRR[%i].fAntInfo[%i].labChans[1] %i\n", fStationId, row, thisAntInfo->labChans[1]);

	break;
      case 12: //isDiplexed

	thisAntInfo->isDiplexed=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].isDiplexed %i\n", fStationId, row, thisAntInfo->isDiplexed);

	break;
      case 13: //diplexedChans[0]

	thisAntInfo->diplexedChans[0]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].diplexedChans[0] %i\n", fStationId, row, thisAntInfo->diplexedChans[0]);

	break;
      case 14: //diplexedChans[1]

	thisAntInfo->diplexedChans[1]=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].diplexedChans[1] %i\n", fStationId, row, thisAntInfo->diplexedChans[1]);

	break;
      case 15: //preAmpNum

	thisAntInfo->preAmpNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].preAmpNum %i\n", fStationId, row, thisAntInfo->preAmpNum);

	break;
      case 16: //avgNoiseFigure

	thisAntInfo->avgNoiseFigure=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].avgNoiseFigure %f\n", fStationId, row, thisAntInfo->avgNoiseFigure);

	break;
      case 17: //rcvrNum

	thisAntInfo->rcvrNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].rcvrNum %i\n", fStationId, row, thisAntInfo->rcvrNum);

	break;
      case 18: //designator

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->designator, temp, 3);
	strncpy(thisAntInfo->antName, temp, 3);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].designator %s\n", fStationId, row, thisAntInfo->designator);

	break;
      case 19: //antPolNum

	thisAntInfo->antPolNum=sqlite3_column_int(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antPolNum %i\n", fStationId, row, thisAntInfo->antPolNum);

	break;
      case 20: //antType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kBicone")==0) thisAntInfo->antType=AraAntType::kBicone;
        if(strcmp (temp,"kBowtieSlot")==0) thisAntInfo->antType=AraAntType::kBowtieSlot;
        if(strcmp (temp,"kDiscone")==0) thisAntInfo->antType=AraAntType::kDiscone;
        if(strcmp (temp,"kBatwing")==0) thisAntInfo->antType=AraAntType::kBatwing;
        if(strcmp (temp,"kFatDipole")==0) thisAntInfo->antType=AraAntType::kFatDipole;
        if(strcmp (temp,"kQuadSlot")==0) thisAntInfo->antType=AraAntType::kQuadSlot;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].antType %i\n", fStationId, row, thisAntInfo->antType);

	break;
      case 21: //polType

	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kVertical")==0) thisAntInfo->polType=AraAntPol::kVertical;
        if(strcmp (temp,"kHorizontal")==0) thisAntInfo->polType=AraAntPol::kHorizontal;
        if(strcmp (temp,"kSurface")==0) thisAntInfo->polType=AraAntPol::kSurface;

	//printf("fStationInfoICRR[%i].fAntInfo[%i].AraAntPol %i\n", fStationId, row, thisAntInfo->polType);

	break;
      case 22: //locationName

	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->locationName, temp, 4);
	strncpy(thisAntInfo->holeName, temp, 4);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].locationName %s\n", fStationId, row, thisAntInfo->locationName);


	break;
      case 23: //antLocation[0]

	thisAntInfo->antLocation[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antLocation[0] %f\n", fStationId, row, thisAntInfo->antLocation[0]);

	break;
      case 24: //antLocation[1]

	thisAntInfo->antLocation[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antLocation[1] %f\n", fStationId, row, thisAntInfo->antLocation[1]);

	break;
      case 25: //antLocation[2]

	thisAntInfo->antLocation[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antLocation[2] %f\n", fStationId, row, thisAntInfo->antLocation[2]);

	break;
      case 26: //cableDelay

	thisAntInfo->cableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].cableDelay %f\n", fStationId, row, thisAntInfo->cableDelay);

	break;
      case 27: //debugHolePosition[0]

	thisAntInfo->debugHolePosition[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePosition[0] %f\n", fStationId, row, thisAntInfo->debugHolePosition[0]);

	break;
      case 28: //debugHolePosition[1]

	thisAntInfo->debugHolePosition[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePosition[1] %f\n", fStationId, row, thisAntInfo->debugHolePosition[1]);

	break;
      case 29: //debugHolePosition[2]

	thisAntInfo->debugHolePosition[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePosition[2] %f\n", fStationId, row, thisAntInfo->debugHolePosition[2]);

	break;
      case 30: //debugPreAmpDz

	thisAntInfo->debugPreAmpDz=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugPreAmpDz %f\n", fStationId, row, thisAntInfo->debugPreAmpDz);

	break;
      case 31: //debugHolePositionZft

	thisAntInfo->debugHolePositionZft=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePositionZft %f\n", fStationId, row, thisAntInfo->debugHolePositionZft);

	break;
      case 32: //debugHolePositionZm

	thisAntInfo->debugHolePositionZm=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugHolePositionZm %f\n", fStationId, row, thisAntInfo->debugHolePositionZm);

	break;
      case 33: //debugTrueAsBuiltPosition[0]

	thisAntInfo->debugTrueAsBuiltPositon[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTrueAsBuiltPositon[0] %f\n", fStationId, row, thisAntInfo->debugTrueAsBuiltPositon[0]);

	break;
      case 34: //debugTrueAsBuiltPosition[1]

	thisAntInfo->debugTrueAsBuiltPositon[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTrueAsBuiltPositon[1] %f\n", fStationId, row, thisAntInfo->debugTrueAsBuiltPositon[1]);

	break;
      case 35: //debugTrueAsBuiltPosition[2]

	thisAntInfo->debugTrueAsBuiltPositon[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTrueAsBuiltPositon[2] %f\n", fStationId, row, thisAntInfo->debugTrueAsBuiltPositon[2]);

	break;
      case 36: //debugCableDelay2

	thisAntInfo->debugCableDelay2=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugCableDelay2 %f\n", fStationId, row, thisAntInfo->debugCableDelay2);

	break;
      case 37: //debugFeedPointDelay

	thisAntInfo->debugFeedPointDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugFeedPointDelay %f\n", fStationId, row, thisAntInfo->debugFeedPointDelay);

	break;
      case 38: //debugTotalCableDelay

	thisAntInfo->debugTotalCableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].debugTotalCableDelay %f\n", fStationId, row, thisAntInfo->debugTotalCableDelay);

	break;
      case 40: //antOrient[0]

	thisAntInfo->antOrient[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antOrient[0] %1.10f\n", fStationId, row, thisAntInfo->antOrient[0]);

	break;

      case 41: //antOrient[1]

	thisAntInfo->antOrient[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antOrient[1] %1.10f\n", fStationId, row, thisAntInfo->antOrient[1]);

	break;

      case 42: //antOrient[2]

	thisAntInfo->antOrient[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoICRR[%i].fAntInfo[%i].antOrient[2] %1.10f\n", fStationId, row, thisAntInfo->antOrient[2]);

	break;


      default:

	break;

      }//switch(column)

    }//column


  }//while(1)
  //now insert the no of rfchannels

  // if(fStationId==ARA_TESTBED)  {
  //   fStationInfoICRR[0].setNumRFChans(RFCHANS_TESTBED);
  //   fStationInfoICRR[0].setNumAnts(RFCHANS_TESTBED);
  // }
  // else if(fStationId==ARA_STATION1)  {
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

  this->fillAntIndexVec();
  this->fillTrigChanVec();

}


// ----------------------------------------------------------------------


void AraStationInfo::readCalPulserDb(){
  sqlite3 *db;
  char *zErrMsg = 0;
  sqlite3_stmt *stmt;

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
  sprintf(fileName, "%s/CalPulserInfo.sqlite", calibDir);

  //open the database
  //  int rc = sqlite3_open_v2(fileName, &db, SQLITE_OPEN_READONLY, NULL);
  int rc = sqlite3_open(fileName, &db);;
  if(rc!=SQLITE_OK){
    printf("AraStationInfo::readCalPulserDb() - Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *query;

  //This is where we decide which table to access in the database
  if(fStationId==ARA_TESTBED) query = "select * from TESTBED";
  else if(fStationId==ARA_STATION1B) query = "select * from ARA01";
  else if(fStationId==ARA_STATION2) query = "select * from ARA02";
  else if(fStationId==ARA_STATION3) query = "select * from ARA03";
  else if(fStationId==ARA_STATION4) query = "select * from ARA04";
  else if(fStationId==ARA_STATION5) query = "select * from ARA05";
  else if(fStationId==ARA_STATION6) query = "select * from ARA06";
  else{
    fprintf(stderr, "%s : Not loading calpulser information - fStationId %i is not ARA1-3\n", __FUNCTION__, fStationId);
    return;
  }
  //prepare an sql statment which will be used to obtain information from the data base
  //  rc=sqlite3_prepare_v2(db, query, strlen(query)+1, &stmt, NULL);
  rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

  if(rc!=SQLITE_OK){
    printf("AraStationInfo::readCalPulserDb() statement not prepared OK\n");
    //should close the data base and exit the function
    sqlite3_close(db);
    return;
  }

  int calAntId=0;
  int row=0;
  while(1){
    //printf("row number %i\n", row);
    rc=sqlite3_step(stmt);
    if(rc==SQLITE_DONE) break;
    int nColumns=sqlite3_column_count(stmt);

    row=sqlite3_column_int(stmt, 2)-1;//forcing the row to be correct
    //printf("row number %i\n", row);
    AraCalAntennaInfo *thisAntInfo=NULL;

    for(int column=0;column<nColumns;column++){

      const char* temp;    

      switch(column){  
      case 0: //primary key - fStationId+labChip+channel
	calAntId=sqlite3_column_int(stmt,column);
	thisAntInfo=this->getNewCalAntennaInfo(calAntId);
	break;
      case 1: //locationName
	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->locationName, temp, 4);       
	break;
      case 2: //antName
	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->antName, temp, 4);	
	break;
      case 3: //pulserName
	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->pulserName, temp, 4);       
	break;
      case 4: //antType
	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kBicone")==0) thisAntInfo->antType=AraAntType::kBicone;
        if(strcmp (temp,"kBowtieSlot")==0) thisAntInfo->antType=AraAntType::kBowtieSlot;
        if(strcmp (temp,"kDiscone")==0) thisAntInfo->antType=AraAntType::kDiscone;
        if(strcmp (temp,"kBatwing")==0) thisAntInfo->antType=AraAntType::kBatwing;
        if(strcmp (temp,"kFatDipole")==0) thisAntInfo->antType=AraAntType::kFatDipole;
        if(strcmp (temp,"kQuadSlot")==0) thisAntInfo->antType=AraAntType::kQuadSlot;
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antType %i\n", fStationId, row, thisAntInfo->antType);
	break;
      case 5: //polType
	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kVertical")==0) thisAntInfo->polType=AraAntPol::kVertical;
        if(strcmp (temp,"kHorizontal")==0) thisAntInfo->polType=AraAntPol::kHorizontal;
        if(strcmp (temp,"kSurface")==0) thisAntInfo->polType=AraAntPol::kSurface;
	//printf("fStationInfoATRI[%i].fAntInfo[%i].AraAntPol %i\n", fStationId, row, thisAntInfo->polType);
	break;
      case 6: //antLocation[0]

	thisAntInfo->antLocation[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[0] %f\n", fStationId, row, thisAntInfo->antLocation[0]);

	break;
      case 7: //antLocation[1]

	thisAntInfo->antLocation[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[1] %f\n", fStationId, row, thisAntInfo->antLocation[1]);

	break;
      case 8: //antLocation[2]

	thisAntInfo->antLocation[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocation[2] %f\n", fStationId, row, thisAntInfo->antLocation[2]);

	break;
      case 9: //cableDelay

	thisAntInfo->cableDelay=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].cableDelay %f\n", fStationId, row, thisAntInfo->cableDelay);

	break;


      case 10: //antLocationCalib[0]

	thisAntInfo->antLocationCalib[0]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocationCalib[0] %f\n", fStationId, row, thisAntInfo->antLocationCalib[0]);

	break;
      case 11: //antLocationCalib[1]

	thisAntInfo->antLocationCalib[1]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocationCalib[1] %f\n", fStationId, row, thisAntInfo->antLocationCalib[1]);

	break;
      case 12: //antLocationCalib[2]

	thisAntInfo->antLocationCalib[2]=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].antLocationCalib[2] %f\n", fStationId, row, thisAntInfo->antLocationCalib[2]);

	break;
      case 13: //cableDelayCalib

	thisAntInfo->cableDelayCalib=sqlite3_column_double(stmt, column);
	//printf("fStationInfoATRI[%i].fAntInfo[%i].cableDelay %f\n", fStationId, row, thisAntInfo->cableDelay);

	break;


      default:

	break;

      }//switch(column)

    }//column
    thisAntInfo->fillArrayCoords();
  }//while(1)

  //now need to destroy the sqls statement prepared earlier
  rc = sqlite3_finalize(stmt);
  if(rc!=SQLITE_OK) printf("error finlizing sql statement\n");
  //  printf("sqlite3_finalize(stmt) = %i\n", rc);

  //now close the connection to the database
  rc = sqlite3_close(db);
  if(rc!=SQLITE_OK) printf("error closing db\n");

}

// ----------------------------------------------------------------------


void AraStationInfo::readChannelMapDbAtri_2(){
  sqlite3 *db;
  char *zErrMsg = 0;
  sqlite3_stmt *stmt;

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
  int rc = sqlite3_open(fileName, &db);;
  if(rc!=SQLITE_OK){
    fprintf(stderr, "%s - Can't open database: %s\n", __FUNCTION__, sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *query;

  //This is where we decide which table to access in the database
  if(fStationId==ARA_STATION1B) query = "select * from ARA01";
  else if(fStationId==ARA_STATION2) query = "select * from ARA02";
  else if(fStationId==ARA_STATION3) query = "select * from ARA03";
  else if(fStationId==ARA_STATION4) query = "select * from ARA04";
  else if(fStationId==ARA_STATION5) query = "select * from ARA05";
  else if(fStationId==ARA_STATION6) query = "select * from ARA06";
  else{
    fprintf(stderr, "%s : fStationId %i is not ARA1-6\n", __FUNCTION__, fStationId);
    return;
  }
  
  rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

  if(rc!=SQLITE_OK){
    fprintf(stderr, "%s : statement not prepared OK\n", __FUNCTION__);
    sqlite3_close(db);
    return;
  }

  int antId=0;
  AraAntennaInfo *thisAntInfo=NULL;
  while(1){
    //printf("row number %i\n", row);
    rc=sqlite3_step(stmt);
    if(rc==SQLITE_DONE) break;
    int nColumns=sqlite3_column_count(stmt);

    for(int column=0;column<nColumns;column++){

      const char* temp;    

      switch(column){  
      case 0: //primary key
	antId=sqlite3_column_int(stmt,column);
	thisAntInfo=this->getNewAntennaInfo(antId);
	thisAntInfo->chanNum=antId;
	//	printf("\n\nthisAntInfo->chanNum=antId %i\n", thisAntInfo->chanNum=antId);
	break;
      case 1://holeName
	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->holeName, temp, 6);
	//	printf("thisAntInfo->holeName %s\n", thisAntInfo->holeName, temp);
	break;
      case 2://antName
	temp = (const char*)sqlite3_column_text(stmt, column);
	strncpy(thisAntInfo->antName, temp, 6);
	//	printf("thisAntInfo->holeName %s\n", thisAntInfo->holeName, temp);
	//	printf("thisAntInfo->antName %s\n", thisAntInfo->antName, temp);
	break;
      case 3://polType
	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kVertical")==0) thisAntInfo->polType=AraAntPol::kVertical;
        if(strcmp (temp,"kHorizontal")==0) thisAntInfo->polType=AraAntPol::kHorizontal;
        if(strcmp (temp,"kSurface")==0) thisAntInfo->polType=AraAntPol::kSurface;
	//printf("thisAntInfo->polType %i\n", thisAntInfo->polType);
	break;
      case 4://antPolNum
	thisAntInfo->antPolNum=sqlite3_column_int(stmt, column);
	//printf("thisAntInfo->antPolNum %i\n", thisAntInfo->antPolNum);
	break;
      case 5://daqChanNum
	thisAntInfo->daqChanNum=sqlite3_column_int(stmt, column);
	//printf("thisAntInfo->daqChanNum %i\n", thisAntInfo->daqChanNum);
	break;
      case 6://daqTrigChan
	thisAntInfo->daqTrigChan=sqlite3_column_int(stmt, column);
	//printf("thisAntInfo->daqTrigChan %i\n", thisAntInfo->daqTrigChan);
	break;
      case 7://foamId
	thisAntInfo->foamId=sqlite3_column_int(stmt, column);
	//printf("thisAntInfo->foamId %i\n", thisAntInfo->foamId);
	break;
      case 8://foamChanNum
	thisAntInfo->foamChanNum=sqlite3_column_int(stmt, column);
	//printf("thisAntInfo->foamChanNum %i\n", thisAntInfo->foamChanNum);
	break;
      case 9://antType
	temp = (const char*)sqlite3_column_text(stmt, column);
        if(strcmp (temp,"kBicone")==0) thisAntInfo->antType=AraAntType::kBicone;
        if(strcmp (temp,"kBowtieSlot")==0) thisAntInfo->antType=AraAntType::kBowtieSlot;
        if(strcmp (temp,"kDiscone")==0) thisAntInfo->antType=AraAntType::kDiscone;
        if(strcmp (temp,"kBatwing")==0) thisAntInfo->antType=AraAntType::kBatwing;
        if(strcmp (temp,"kFatDipole")==0) thisAntInfo->antType=AraAntType::kFatDipole;
        if(strcmp (temp,"kQuadSlot")==0) thisAntInfo->antType=AraAntType::kQuadSlot;
	//printf("thisAntInfo->antType %s\n", AraAntType::antTypeAsString(thisAntInfo->antType));
	break;
      case 10://antLocationX
	thisAntInfo->antLocation[0]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->antLocation[0] %f\n", thisAntInfo->antLocation[0]);
	break;
      case 11://antLocationY
	thisAntInfo->antLocation[1]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->antLocation[1] %f\n", thisAntInfo->antLocation[1]);
	break;
      case 12://antLocationZ
	thisAntInfo->antLocation[2]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->antLocation[2] %f\n", thisAntInfo->antLocation[2]);
	break;
      case 13://cableDelay
	thisAntInfo->cableDelay=sqlite3_column_double(stmt, column);
	break;
      case 14://calibAntLocationX
	thisAntInfo->calibAntLocation[0]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->calibAntLocation[0] %f\n", thisAntInfo->calibAntLocation[0]);
	break;
      case 15://calibAntLocationY
	thisAntInfo->calibAntLocation[1]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->calibAntLocation[1] %f\n", thisAntInfo->calibAntLocation[1]);
	break;
      case 16://calibAntLocationZ
	thisAntInfo->calibAntLocation[2]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->calibAntLocation[2] %f\n", thisAntInfo->calibAntLocation[2]);
	break;
      case 17://calibCableDelay
	thisAntInfo->calibCableDelay=sqlite3_column_double(stmt, column);
	break;
      case 18://antOrientX
	thisAntInfo->antOrient[0]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->antOrient[0] %f\n", thisAntInfo->antOrient[0]);
	break;
      case 19://antOrientY
	thisAntInfo->antOrient[1]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->antOrient[1] %f\n", thisAntInfo->antOrient[1]);
	break;
      case 20://antOrientZ
	thisAntInfo->antOrient[2]=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->antOrient[2] %f\n", thisAntInfo->antOrient[2]);
	break;
      case 21://highPassFilterMhz
	thisAntInfo->highPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->highPassFilterMhz %f\n", thisAntInfo->highPassFilterMhz);
	break;
      case 22://lowPassFilterMhz
	thisAntInfo->lowPassFilterMhz=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->lowPassFilterMhz %f\n", thisAntInfo->lowPassFilterMhz);
	break;
      case 23://avgNoiseFigure
	thisAntInfo->avgNoiseFigure=sqlite3_column_double(stmt, column);
	//printf("thisAntInfo->avgNoiseFigure %f\n", thisAntInfo->avgNoiseFigure);
	break;
      default:
	break;

      }//switch(column)

    }//column
    //RJN fixed string copying bug
    strncpy(thisAntInfo->designator, thisAntInfo->getRFChanName(),12);
     
    thisAntInfo->fillArrayCoords();
  
  }//while(1)

  rc = sqlite3_finalize(stmt);
  if(rc!=SQLITE_OK) fprintf(stderr, "%s : error finlizing sql statement\n", __FUNCTION__);
  rc = sqlite3_close(db);
  if(rc!=SQLITE_OK) fprintf(stderr, "%s : error closing db\n", __FUNCTION__);

  this->fillAntIndexVec();
  this->fillTrigChanVec();


}



//Labelling for TDA channels just for the ATRI stations
const char *AraStationInfo::getAtriSingleChannelLabel(Int_t tda, Int_t channel)
{
   AraAntennaInfo *antInfo = getAntennaInfoForTrigChan(getTrigChan(tda,channel));
   if(antInfo) 
      return antInfo->getRFChanName();
   return NULL;

}

const char *AraStationInfo::getAtriL2Label(Int_t index)
{
   switch(index) {
   case 0: return "S1/2 VPol";
   case 1: return "S3/4 VPol";
   case 2: return "S1/2 HPol";
   case 3: return "S3/4 HPol";
   default: return "Unknown";
   }

}

const char *AraStationInfo::getAtriL3Label(Int_t index)
{
   switch(index) {
   case 0: return "VPol";
   case 1: return "HPol";
   default: return "Unknown";
   } 

}

const char *AraStationInfo::getAtriL4Label(Int_t index)
{

   switch(index) {
   case 0: return "Deep";
   case 1: return "Surface";
   case 2: return "Software";
   case 3: return "Timed";
   default: return "Unknown";
   } 

}
