//////////////////////////////////////////////////////////////////////////////
/////  AraGeomTool.h       ARA Geometry tool                             /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class working out what is where                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAGEOMTOOL_H
#define ARAGEOMTOOL_H

//Includes
#include <TObject.h>
#include <TMath.h>
#include <TRotation.h>
#include <TVector3.h>
#include "araIcrrStructures.h"
#include "araIcrrDefines.h"
#include "araAtriStructures.h"
#include "AraAntennaInfo.h"
#include "AraStationInfo.h"
#include "AraSunPos.h"

#include "time.h"

//! Part of AraEvent library. Loads and stores information about each station's geometry as well as information about the antennae (filters, positions, channels etc...).
/*!
  The Ara geometry and numbering tool
  \ingroup rootclasses
*/
class AraGeomTool
{
	public:
		AraGeomTool(); ///< Default constructor
		~AraGeomTool(); ///< Destructor
		
		// These functions are really AraStationInfo functions, might remove them at some point
		int getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum, AraStationId_t stationId);
		AraAntPol::AraAntPol_t getPolByRFChan(int rfChan, AraStationId_t stationId);
		Int_t getAntNumByRFChan(int rfChan, AraStationId_t stationId);
		Int_t getElecChanFromRFChan(int rfChan, AraStationId_t stationId) {return getStationInfo(stationId)->getElecChanFromRFChan(rfChan);}

		// These functions are all to do with calculating delta-t and reconstruciton
		Double_t calcDeltaTInfinity(Double_t ant1[3], Double_t ant2[3],Double_t phiWave, Double_t thetaWave);
		Double_t calcDeltaTR(Double_t ant1[3], Double_t ant2[3], Double_t phiWave, Double_t thetaWave,Double_t R);   
		Double_t calcDeltaTInfinity(Int_t rfChan1, Int_t rfChan2,Double_t phiWave, Double_t thetaWave, AraStationId_t stationId);
		Double_t calcDeltaTR(Int_t rfChan1, Int_t rfChan2, Double_t phiWave, Double_t thetaWave,Double_t R, AraStationId_t stationId);
		Double_t calcDeltaTSimple(Double_t ant1[3], Double_t ant2[3], Double_t source[3]);

		// LoadSQLDbAtri added by UAL 01/25/2019
		// Corrected unixtime argument from Double_t to Int_t by UAL 02/27/2019
		AraStationInfo *LoadSQLDbAtri(Int_t unixtime, AraStationId_t stationId);

		// Added ifile argument by UAL 02/27/2019
		AraStationInfo *getStationInfo(AraStationId_t stationId,Int_t DByear=0); ///< Would like to make this const but for now this is fine

		// Utility functions to do with the different stations
		static bool isIcrrStation(AraStationId_t stationId); ///< Returns TRUE if the station is an ICRR station and false otherwise
		static bool isAtriStation(AraStationId_t stationId); ///< Returns TRUE if the station is an ATRIA station and false otherwise
		static Int_t getStationCalibIndex(AraStationId_t stationId); ///< Used by the calibrator. This function returns the calibration and pedestal index for a stationId (TESTBED==0, STATION1==1, STATION1A==0, STATION2==1, STATION3==2...)
		static Int_t getStationNumber(AraStationId_t stationId);
		static void printStationName(AraStationId_t stationId); ///< Print to stdout the station Name
		static const char *getStationName(AraStationId_t stationId); ///< Return char* with the station Name from the stationId
		static AraStationId_t getAtriStationId(int stationNumber); ///< Simple utility function


		// Station and array geometry functions
		void convertStationToArrayCoords(AraStationId_t stationId, Double_t inputCoords[3], Double_t outputCoords[3] );  ///< Converts from station-centric to array-centric coordinates
		void convertArrayToStationCoords(AraStationId_t stationId, Double_t inputCoords[3], Double_t outputCoords[3] );  ///< Converts from array-centric to station-centric coordinates
		TVector3 convertStationToArrayCoords(AraStationId_t stationId, TVector3 inputCoords); ///< Converts from station-centric to array-centric coordinates
		TVector3 convertArrayToStationCoords(AraStationId_t stationId, TVector3 inputCoords); ///< Converts from array-centric to station-centric coordinates
		TRotation *getStationToArrayRotation(AraStationId_t stationId);  ///< The rotation matrix used in converting from station-centric to array-centric coordinates
		TRotation *getArrayToStationRotation(AraStationId_t stationId); ///< The rotation matrix used in converting from array-centric to station-centric coordinates
		TVector3 &getStationVector(AraStationId_t stationId);  ///< The lcoordinates of the origin of the station-centric system in array-centric coordinates


		// Non-station locations in array centric corrdinates
		Double_t *getSouthPole2010_11();
		Double_t *getSouthPole2011_12();
		Double_t *getSouthPoleTelescope();
		Double_t *getICLCorner(int corner); ///< Corner is 0,1,2 or 3
		Double_t *getWindTurbine(int tbNumber); ///< TB number is 1, 2 or 3


		// Instance generator
		static AraGeomTool*  Instance();
		
		// Some variables to do with ice properties
		static Double_t nTopOfIce;
		
		// JPD -- This section is for Sun Position Calculations
		
		// Firstly functions to handle unixTime
		static Int_t getSecond(time_t unixTime);
		static Int_t getMinute(time_t unixTime);
		static Int_t getHour(time_t unixTime);
		static Int_t getDay(time_t unixTime);
		static Int_t getDayOfYear(time_t unixTime);
		static Int_t getMonth(time_t unixTime);
		static Int_t getYear(time_t unixTime);
		static time_t getUnixTime(Int_t year, Int_t month, Int_t day, Int_t hour=0, Int_t minute=0, Int_t second=0);

		// Now some functions to get azimuth and elevation of sun from time and location

		// Azimuth is angle from Northing. Ara Global Coordinates have x = Easting, y = Northing, z = Uping
		static Double_t getSunAzimuthLongLat(Double_t longitude, Double_t latitude, Int_t year, Int_t month, Int_t day, Int_t hour=0, Int_t minute=0, Int_t second=0);
		static Double_t getSunAzimuthLongLat(Double_t longitude, Double_t latitude, unsigned int unixTime);
		
		// Should be elevation
		static Double_t getSunZenithLongLat(Double_t longitude, Double_t latitude, Int_t year, Int_t month, Int_t day, Int_t hour=0, Int_t minute=0, Int_t second=0);
		static Double_t getSunZenithLongLat(Double_t longitude, Double_t latitude, unsigned int unixTime);


		TVector3 getSunPosition(AraStationId_t stationId, unsigned int unixTime);
		TVector3 getSunPosition(AraStationId_t stationId, Int_t year, Int_t month, Int_t day, Int_t hour=0, Int_t minute=0, Int_t second=0);
		
		// JPD -- geometry tools to get longitude and latitude
		static Double_t getLongitudeFromArrayCoords(Double_t Northing, Double_t Easting, Int_t year=2011);
		static Double_t getGeometricLatitudeFromArrayCoords(Double_t Northing, Double_t Easting, Int_t year=2011);
		static Double_t getGeographicLatitudeFromArrayCoords(Double_t Northing, Double_t Easting, Int_t year=2011);

		// Maybe these will be useful one day
		static Double_t getNorthingFromLatLong(Double_t Latitude, Double_t Longitude);
		static Double_t getEastingFromLatLong(Double_t Latitude, Double_t Longitude);

	protected:
		static AraGeomTool *fgInstance;  
		// protect against multiple instances

	private:

		AraStationInfo *fStationInfoICRR[ICRR_NO_STATIONS]; ///< Station info contains the antenna info and station information
		AraStationInfo *fStationInfoATRI[ATRI_NO_STATIONS]; ///< Station info contains the antenna info and station information
		
		//Here are the ARA station coordinates
		Double_t fStationCoordsICRR[ICRR_NO_STATIONS][3];  ///<Station coordinates in Northing, Easting, Elevation
		Double_t fStationCoordsAtri[ATRI_NO_STATIONS][3];  ///<Station coordinates in Northing, Easting, Elevation
		Double_t fStationLocalCoordsICRR[ICRR_NO_STATIONS][3][3]; ///< Station x, y, z directions in array coords
		Double_t fStationLocalCoordsATRI[ATRI_NO_STATIONS][3][3]; ///< Station x, y, z directions in array coords


		TRotation *fArrayToStationRotationICRR[ICRR_NO_STATIONS]; ///< Really are rotation matrices
		TRotation *fArrayToStationRotationATRI[ATRI_NO_STATIONS]; ///< Really are rotation matrices
		TRotation *fStationToArrayRotationICRR[ICRR_NO_STATIONS]; ///< Really are rotation matrices
		TRotation *fStationToArrayRotationATRI[ATRI_NO_STATIONS];///< Really are rotation matrices

		TVector3 fStationVectorATRI[ATRI_NO_STATIONS]; ///< The vector position of the station origin in array coordinates
		TVector3 fStationVectorICRR[ICRR_NO_STATIONS]; ///< The vector position of the station origin in array coordinates

		void readAraArrayCoords();  ///< Reads the sqlite database with the array-centric and station-centric coordinate definitions

};


#endif //ARAGEOMTOOL_H
