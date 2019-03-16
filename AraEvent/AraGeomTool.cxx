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
#include <ctime>

//sqlite includes
#include <sqlite3.h>

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TVector3.h"

AraGeomTool * AraGeomTool::fgInstance=0;
Double_t AraGeomTool::nTopOfIce=1.48;
const Double_t fFtInm=0.3048;


// See Amy Connolly's Note https://www.phys.hawaii.edu/elog/ARA/91
// fGeoidA / fGeoidC are for the WGS84 GPS ellipsoid 
// These are all in meters
const Double_t fIceThicknessSP=2646.28;
const Double_t fGeoidA=6378137;
const Double_t fGeoidC=6356752.3;

// Removed the Utime variable and changed the order of arguments of AraStationInfo to have backwards compatibility after introducing default value in the constructor of AraStationInfo by UAL 02/19/2019


// 1 Astronomical Unit ~ mean distance from Earth to Sun (for sun position calculations);
const Double_t fAU=149597890000; //(in meters)

// Some locations in ARA global coordinates
Double_t fSouthPole2010_11[3]={27344.05*fFtInm,-3395.14*fFtInm,-35.63*fFtInm};
Double_t fSouthPole2011_12[3]={27322.88*fFtInm,-3369.89*fFtInm,-36.02*fFtInm};
Double_t fSouthPoleTelescope[3]={24774.18*fFtInm,-871.35*fFtInm,-30*fFtInm};  ///< Made up the u-number
Double_t fICL2011_12[4][3]={{24011.58*fFtInm,-1702.53*fFtInm,0},
							{24060.51*fFtInm,-1723.09*fFtInm,0},
							{24049.7*fFtInm,-1749.02*fFtInm,0},
							{24000.78*fFtInm,-1728.47*fFtInm,0}};

Double_t fWindTurbine2011_12[3][3]={{46166.49*fFtInm,52159.43*fFtInm,0},
									{22546.95*fFtInm,-2594.03*fFtInm,0},
									{18662.31*fFtInm,-3552.64*fFtInm,0}};



Double_t *AraGeomTool::getSouthPole2010_11()
{
	return fSouthPole2010_11;
}

Double_t *AraGeomTool::getSouthPole2011_12()
{
	return fSouthPole2011_12;
}

Double_t *AraGeomTool::getSouthPoleTelescope()
{
	return fSouthPoleTelescope;
}

Double_t *AraGeomTool::getICLCorner(int corner)  ///< Corner is 0,1,2 or 3
{
	return fICL2011_12[corner];
}

Double_t *AraGeomTool::getWindTurbine(int tbNumber) ///< TB number is 1, 2 or 3
{
	return fWindTurbine2011_12[tbNumber-1];
}

AraGeomTool::AraGeomTool() 
{
	 //Default Constructor

	//Read in the channel maps for the TestBed and Station1
	// readChannelMapDbIcrr(ARA_TESTBED);
	// readChannelMapDbIcrr(ARA_STATION1);
	// readChannelMapDbAtri(ARA_STATION1B);
	for(int i=0;i<ICRR_NO_STATIONS;i++) {
		fStationInfoICRR[i]=NULL;
		fArrayToStationRotationICRR[i]=NULL;
		fStationToArrayRotationICRR[i]=NULL;
	}
	for(int i=0;i<ATRI_NO_STATIONS;i++) {
		fStationInfoATRI[i]=0;
		fArrayToStationRotationATRI[i]=NULL;
		fStationToArrayRotationATRI[i]=NULL;
	}
	
	readAraArrayCoords();
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

AraGeomTool*  AraGeomTool::Instance()
{
	// static function
	if(fgInstance)
		return fgInstance;

	fgInstance = new AraGeomTool();
	return fgInstance;
}

int AraGeomTool::getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum, AraStationId_t stationId)
{
	
	// fprintf(stderr, "antPol %i antNum %i stationId %i\n", (int)antPol, antNum, (int)stationId);
	return getStationInfo(stationId)->getRFChanByPolAndAnt(antNum,antPol);
}


AraAntPol::AraAntPol_t AraGeomTool::getPolByRFChan(int rfChan, AraStationId_t stationId)
{
	// RJN Should add a check here for safety
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


// //FIXME //jpd this is most definitely a hack to make AraCanvasMaker work -> this will only
// //return the testbed lookup stuff not station1
// int AraGeomTool::getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum)
// {
//   return getRFChanByPolAndAnt(antPol,antNum,ARA_STATION1);
// }//FIXME -- this just loads the stuff for station1

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

// Removed the Utime argument in AraStationInfo by UAL 02/19/2019
// Added the ifile argument in getStationInfo by UAL 02/27/2019
AraStationInfo *AraGeomTool::getStationInfo(AraStationId_t stationId,Int_t DByear)
{
	Int_t unixtime=DByear;
		
	int calibIndex=getStationCalibIndex(stationId);
	if(isIcrrStation(stationId)) {
		if(!fStationInfoICRR[calibIndex]) {
			fStationInfoICRR[calibIndex] = new AraStationInfo(stationId,unixtime);
		}
		if(calibIndex>=0 && calibIndex<ICRR_NO_STATIONS)
			return fStationInfoICRR[calibIndex];
	}
	if(isAtriStation(stationId)) {
		if(!fStationInfoATRI[calibIndex]) {
			fStationInfoATRI[calibIndex] = new AraStationInfo(stationId,unixtime);
		}
		if(calibIndex>=0 && calibIndex<ATRI_NO_STATIONS)
			return fStationInfoATRI[calibIndex];
	}
	return NULL;
}

// LoadSQLDbAtri added by UAL 01/25/2019
// Changed the order of arguments of AraStationInfo to have backwards compatibility after introducing default value in the constructor of AraStationInfo by UAL 02/19/2019
AraStationInfo *AraGeomTool::LoadSQLDbAtri(Int_t unixtime, AraStationId_t stationId)
{
	int calibIndex=getStationCalibIndex(stationId);
	if(isAtriStation(stationId)) {
		if(!fStationInfoATRI[calibIndex]) {
			fStationInfoATRI[calibIndex] = new AraStationInfo(stationId,unixtime);
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


void AraGeomTool::readAraArrayCoords() {
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
	sprintf(fileName, "%s/AraArrayCoords.sqlite", calibDir);

	// open the database
	//  int rc = sqlite3_open_v2(fileName, &db, SQLITE_OPEN_READONLY, NULL);
	int rc = sqlite3_open(fileName, &db);;
	if(rc!=SQLITE_OK){
		printf("AraGeomTool::readAraArrayCoords() - Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	//All the station coordinate information is stored in the ARA table (one row per station)
	const char *query = "select * from ARA";

	// prepare an sql statment which will be used to obtain information from the data base
	//  rc=sqlite3_prepare_v2(db, query, strlen(query)+1, &stmt, NULL);
	rc=sqlite3_prepare(db, query, strlen(query)+1, &stmt, NULL);

	if(rc!=SQLITE_OK){
		printf("statement not prepared OK\n");
		//should close the data base and exit the function
		sqlite3_close(db);
		return;
	}

	AraStationId_t thisStationId;
	int calibIndex=0;
	int row=0;

	while(1){  //This loop is over station
		//printf("row number %i\n", row);
		rc=sqlite3_step(stmt);
		if(rc==SQLITE_DONE) break;
		int nColumns=sqlite3_column_count(stmt);

		row=sqlite3_column_int(stmt, 2)-1;//forcing the row to be correct
		//printf("row number %i\n", row);

		int column=0;
		
		const char* temp;    
		double tempVal;

		//primary key - stationId
		thisStationId=sqlite3_column_int(stmt,column);
		calibIndex=getStationCalibIndex(thisStationId);


		column++;  //station name
		temp = (const char*)sqlite3_column_text(stmt, column);
		//Ignore for now
		
		column++; //2: //electronics tupe
		temp = (const char*)sqlite3_column_text(stmt, column);
		//Ignore for now
		
		//Station Coords
		for(int i=0;i<3;i++) {
			column++;
			tempVal=sqlite3_column_double(stmt, column);
			if(isIcrrStation(thisStationId)) {
				fStationCoordsICRR[calibIndex][i]=tempVal;
			}
			else {
				fStationCoordsAtri[calibIndex][i]=tempVal;
			}
		}
		if(isIcrrStation(thisStationId)) {
			fStationVectorICRR[calibIndex].SetXYZ(fStationCoordsICRR[calibIndex][0],fStationCoordsICRR[calibIndex][1],fStationCoordsICRR[calibIndex][2]);
		}
		else {
			fStationVectorATRI[calibIndex].SetXYZ(fStationCoordsAtri[calibIndex][0],fStationCoordsAtri[calibIndex][1],fStationCoordsAtri[calibIndex][2]);
		}


		//Station Local Coordinates
		for(int i=0;i<3;i++) {
			for(int j=0;j<3;j++) {	
				column++;
				tempVal=sqlite3_column_double(stmt, column);
				if(isIcrrStation(thisStationId)) {
					fStationLocalCoordsICRR[calibIndex][i][j]=tempVal;
				}
				else {
					fStationLocalCoordsATRI[calibIndex][i][j]=tempVal;
				}
			}
		}

		if(isIcrrStation(thisStationId)) {
			fStationToArrayRotationICRR[calibIndex] = new TRotation();      
			TVector3 localx(fStationLocalCoordsICRR[calibIndex][0]);
			TVector3 globale(1,0,0);
			Double_t angleRotate=globale.Angle(localx);
			TVector3 axisRotate=globale.Cross(localx);
			fStationToArrayRotationICRR[calibIndex]->Rotate(angleRotate,axisRotate);
			fArrayToStationRotationICRR[calibIndex]=new TRotation(fStationToArrayRotationICRR[calibIndex]->Inverse());

		}
		else {
			fStationToArrayRotationATRI[calibIndex] = new TRotation();     

			//In the end this remarkably simple bit of code is all we need to define the matrix rotations necessary to switch
			// between array centric and station centric coordinates 
			// The basic idea is simply:
			// a) Find the angle between the array centric easting(x) and the station centric x
			// b) Find the vector that is perpendiculat to both of them using a cross product
			// c) Create a rotation matrix by rotating the identity matrix by this angle about this axis
			// d) Create the invere rotation to go the other way
			// e) Remember we also need to translate to actually convert between station and array and vice-versa
			TVector3 localx(fStationLocalCoordsATRI[calibIndex][0]);
			TVector3 globale(1,0,0);
			Double_t angleRotate=globale.Angle(localx);
			TVector3 axisRotate=globale.Cross(localx);
			fStationToArrayRotationATRI[calibIndex]->Rotate(angleRotate,axisRotate);

			fArrayToStationRotationATRI[calibIndex]=new TRotation(fStationToArrayRotationATRI[calibIndex]->Inverse());

			// fArrayToStationRotationATRI[calibIndex]->Dump();
		}
	} //while(1)

	// now need to destroy the sqls statement prepared earlier
	rc = sqlite3_finalize(stmt);
	if(rc!=SQLITE_OK) printf("error finlizing sql statement\n");
	//  printf("sqlite3_finalize(stmt) = %i\n", rc);

	// now close the connection to the database
	rc = sqlite3_close(db);
	if(rc!=SQLITE_OK) printf("error closing db\n");

}


TVector3 AraGeomTool::convertStationToArrayCoords(AraStationId_t stationId, TVector3 inputCoords) {
	TVector3 output=inputCoords;
	TRotation *mPtr = getStationToArrayRotation(stationId);
	output.Transform(*mPtr);
	output+=getStationVector(stationId);
	return TVector3(output); 
}

TVector3 AraGeomTool::convertArrayToStationCoords(AraStationId_t stationId, TVector3 inputCoords) {
	TVector3 output=inputCoords;
	//std::cout << "Station Vector: " << getStationVector(stationId).x() << "\t" << getStationVector(stationId).y() << "\t" << getStationVector(stationId).z() << "\n";
	output-=getStationVector(stationId);
	//  output.Print();
	TRotation *mPtr = getArrayToStationRotation(stationId);
	output.Transform(*mPtr);
	return TVector3(output); 
}


TRotation *AraGeomTool::getStationToArrayRotation(AraStationId_t stationId)
{
	int calibIndex=getStationCalibIndex(stationId);
	if(isIcrrStation(stationId)) {
		return fStationToArrayRotationICRR[calibIndex];
	}
	return fStationToArrayRotationATRI[calibIndex];
}

TRotation *AraGeomTool::getArrayToStationRotation(AraStationId_t stationId)
{
	int calibIndex=getStationCalibIndex(stationId);
	if(isIcrrStation(stationId)) {
		return fArrayToStationRotationICRR[calibIndex];
	}
	return fArrayToStationRotationATRI[calibIndex];
}


TVector3 &AraGeomTool::getStationVector(AraStationId_t stationId)
{
	int calibIndex=getStationCalibIndex(stationId);
	if(isIcrrStation(stationId)) {
		return fStationVectorICRR[calibIndex];
	}

	return fStationVectorATRI[calibIndex];

}



void AraGeomTool::convertStationToArrayCoords(AraStationId_t stationId, Double_t inputCoords[3], Double_t outputCoords[3] )
{
	TVector3 input(inputCoords);
	TVector3 output=convertStationToArrayCoords(stationId,input);
	outputCoords[0]=output.x();
	outputCoords[1]=output.y();
	outputCoords[2]=output.z();

}
void AraGeomTool::convertArrayToStationCoords(AraStationId_t stationId, Double_t inputCoords[3], Double_t outputCoords[3] )
{

	TVector3 input(inputCoords);
	TVector3 output=convertArrayToStationCoords(stationId,input);
	outputCoords[0]=output.x();
	outputCoords[1]=output.y();
	outputCoords[2]=output.z();

}

// JPD -- This section is for Sun Position Calculations

// Firstly functions to handle unixTime
Int_t AraGeomTool::getSecond(time_t unixTime){
	
	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_sec;

}

Int_t AraGeomTool::getMinute(time_t unixTime){

	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_min;

}

Int_t AraGeomTool::getHour(time_t unixTime){

	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_hour;

}

Int_t AraGeomTool::getDay(time_t unixTime){

	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_mday;

}

Int_t AraGeomTool::getDayOfYear(time_t unixTime){

	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_yday;

}

Int_t AraGeomTool::getMonth(time_t unixTime){

	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_mon+1;

}

Int_t AraGeomTool::getYear(time_t unixTime){

	struct tm *temp_time = gmtime(&unixTime);

	return temp_time->tm_year+1900;

}

time_t AraGeomTool::getUnixTime(Int_t year, Int_t month, Int_t day, Int_t hour, Int_t minute, Int_t second){

	time_t unixTime;
	
	struct tm *timeStruct;

	time(&unixTime);

	timeStruct = localtime(&unixTime);
	timeStruct->tm_year=year-1900;
	timeStruct->tm_mon=month-1;
	timeStruct->tm_mday=day;
	timeStruct->tm_hour=hour;
	timeStruct->tm_min=minute;
	timeStruct->tm_sec=second;

	unixTime = timegm(timeStruct);
	
	return unixTime;
	
}


// JPD Now some functions to get azimuth and elevation of sun from time and location
Double_t AraGeomTool::getSunAzimuthLongLat(Double_t longitude, Double_t latitude, Int_t year, Int_t month, Int_t day, Int_t hour, Int_t minute, Int_t second){
	
	AraSunPosTime time;
	time.iYear = year;
	time.iMonth = month;
	time.iDay = day;
	time.dHours = hour;
	time.dMinutes = minute;
	time.dSeconds = second;
	
	AraSunPosLocation location;
	location.dLongitude = longitude;
	location.dLatitude = latitude;

	AraSunPosSunCoordinates sunPosCoords;

	AraSunPos pos;

	pos.sunpos(time, location, &sunPosCoords);

	return sunPosCoords.dAzimuth+longitude;

}

Double_t AraGeomTool::getSunAzimuthLongLat(Double_t longitude, Double_t latitude, unsigned int unixTime){
	
	Int_t second = getSecond(unixTime);
	Int_t minute = getMinute(unixTime);
	Int_t hour = getHour(unixTime);
	Int_t day = getDay(unixTime);
	Int_t month = getMonth(unixTime);
	Int_t year = getYear(unixTime);

	return getSunAzimuthLongLat(longitude, latitude, year, month, day, hour, minute, second);

}

Double_t AraGeomTool::getSunZenithLongLat(Double_t longitude, Double_t latitude, Int_t year, Int_t month, Int_t day, Int_t hour, Int_t minute, Int_t second){
	
	AraSunPosTime time;
	time.iYear = year;
	time.iMonth = month;
	time.iDay = day;
	time.dHours = hour;
	time.dMinutes = minute;
	time.dSeconds = second;
	
	AraSunPosLocation location;
	location.dLongitude = longitude;
	location.dLatitude = latitude;

	AraSunPosSunCoordinates sunPosCoords;

	AraSunPos pos;

	pos.sunpos(time, location, &sunPosCoords);

	return sunPosCoords.dZenithAngle;

}

Double_t AraGeomTool::getSunZenithLongLat(Double_t longitude, Double_t latitude, unsigned int unixTime){
	
	Int_t second = getSecond(unixTime);
	Int_t minute = getMinute(unixTime);
	Int_t hour = getHour(unixTime);
	Int_t day = getDay(unixTime);
	Int_t month = getMonth(unixTime);
	Int_t year = getYear(unixTime);

	return getSunZenithLongLat(longitude, latitude, year, month, day, hour, minute, second);

}

TVector3 AraGeomTool::getSunPosition(AraStationId_t stationId, unsigned int unixTime){

	TVector3 sunPositionArrayCentric;
	TVector3 sunPositionStationCentric;
	
	TVector3 stationVector = getStationVector(stationId);;
	Int_t year = getYear(unixTime);
	
	Double_t longitude = getLongitudeFromArrayCoords(stationVector[1], stationVector[0], year);
	Double_t latitude = getGeographicLatitudeFromArrayCoords(stationVector[1], stationVector[0], year);

	Double_t azimuth = getSunAzimuthLongLat(longitude, latitude, unixTime);
	Double_t zenith = getSunZenithLongLat(longitude, latitude, unixTime);
	Double_t elevation = 90 - zenith;

	// Azimuth is measured as an angle from Northing in the getSunAzimuth function, but our coordinates are x = Easting, y = Northing, z = Uping
	// Hence we switch sine and cosine

	sunPositionArrayCentric[0] = fAU*TMath::Sin(TMath::DegToRad()*azimuth)*TMath::Cos(TMath::DegToRad()*elevation);
	sunPositionArrayCentric[1] = fAU*TMath::Cos(TMath::DegToRad()*azimuth)*TMath::Cos(TMath::DegToRad()*elevation);
	sunPositionArrayCentric[2] = fAU*TMath::Sin(TMath::DegToRad()*elevation);
	
	sunPositionStationCentric = convertArrayToStationCoords(stationId, sunPositionArrayCentric);
 
	
	return sunPositionStationCentric;
}

TVector3 AraGeomTool::getSunPosition(AraStationId_t stationId, Int_t year, Int_t month, Int_t day, Int_t hour, Int_t minute, Int_t second){

	time_t unixTime = getUnixTime(year, month, day, hour, minute, second);

	return getSunPosition(stationId, unixTime);
	
}

// JPD -- this part has tools for getting the longitude and latitude
Double_t AraGeomTool::getGeometricLatitudeFromArrayCoords(Double_t Northing, Double_t Easting, Int_t year){

	Double_t deltaNorthing = Northing;
	if(year==2011) deltaNorthing -= fSouthPole2010_11[1];
	else if(year==2012) deltaNorthing -= fSouthPole2011_12[1];
	else deltaNorthing -= fSouthPole2010_11[1];

	Double_t deltaEasting = Easting;
	if(year==2011) deltaEasting -= fSouthPole2010_11[0];
	else if(year==2012) deltaEasting -= fSouthPole2011_12[0];
	else deltaEasting -= fSouthPole2010_11[0];

	Double_t deltaR = TMath::Sqrt(TMath::Power(deltaNorthing,2)+TMath::Power(deltaEasting,2));
	Double_t tangent = (fGeoidC+fIceThicknessSP)/(deltaR);

	Double_t latitude = TMath::RadToDeg()*TMath::ATan(tangent);
	
	return -1*latitude;
}

Double_t AraGeomTool::getGeographicLatitudeFromArrayCoords(Double_t Northing, Double_t Easting, Int_t year){

	Double_t deltaNorthing = Northing;
	if(year==2011) deltaNorthing -= fSouthPole2010_11[1];
	else if(year==2012) deltaNorthing -= fSouthPole2011_12[1];
	else deltaNorthing -= fSouthPole2010_11[1];

	Double_t deltaEasting = Easting;
	if(year==2011) deltaEasting -= fSouthPole2010_11[0];
	else if(year==2012) deltaEasting -= fSouthPole2011_12[0];
	else deltaEasting -= fSouthPole2010_11[0];

	Double_t deltaR = TMath::Sqrt(TMath::Power(deltaNorthing,2)+TMath::Power(deltaEasting,2));
	Double_t tangent = (fGeoidC+fIceThicknessSP)/(deltaR)*(TMath::Power(fGeoidA,2))/(TMath::Power(fGeoidC,2));

	Double_t latitude = TMath::RadToDeg()*TMath::ATan(tangent);
	
	return -1*latitude;

}

Double_t AraGeomTool::getLongitudeFromArrayCoords(Double_t Northing, Double_t Easting, Int_t year){

	Double_t deltaNorthing = Northing;
	if(year==2011) deltaNorthing -= fSouthPole2010_11[1];
	else if(year==2012) deltaNorthing -= fSouthPole2011_12[1];
	else deltaNorthing -= fSouthPole2010_11[1];

	Double_t deltaEasting = Easting;
	if(year==2011) deltaEasting -= fSouthPole2010_11[0];
	else if(year==2012) deltaEasting -= fSouthPole2011_12[0];
	else deltaEasting -= fSouthPole2010_11[0];
	
	Double_t longitude = TMath::RadToDeg()*TMath::ATan2(deltaEasting, deltaNorthing);

	return longitude;

}

Double_t AraGeomTool::getNorthingFromLatLong(Double_t Latitude, Double_t Longitude){
	
	Double_t tanLatitude = TMath::Tan(TMath::DegToRad()*Latitude);
	Double_t cosLongitude = TMath::Cos(TMath::DegToRad()*Longitude);
	
	Double_t Northing = cosLongitude/tanLatitude*(fGeoidC+fIceThicknessSP)*(TMath::Power(fGeoidA,2))/(TMath::Power(fGeoidC,2));
	
	return Northing;

}

Double_t AraGeomTool::getEastingFromLatLong(Double_t Latitude, Double_t Longitude){
	
	Double_t tanLatitude = TMath::Tan(TMath::DegToRad()*Latitude);
	Double_t sinLongitude = TMath::Sin(TMath::DegToRad()*Longitude);
	
	Double_t Easting = sinLongitude/tanLatitude*(fGeoidC+fIceThicknessSP)*(TMath::Power(fGeoidA,2))/(TMath::Power(fGeoidC,2));
	
	return Easting;
}
