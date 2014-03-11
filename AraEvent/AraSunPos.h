// JPD -- copied from PSA
// This file is available in electronic form at http://www.psa.es/sdg/sunpos.htm

#ifndef ARASUNPOS_H
#define ARASUNPOS_H


// Declaration of some constants 

#define dEarthMeanRadius     6371.01	// In km
#define dAstronomicalUnit    149597890	// In km



struct AraSunPosTime
{
  int iYear;
  int iMonth;
  int iDay;
  double dHours;
  double dMinutes;
  double dSeconds;
};
   
struct AraSunPosLocation
{
  double dLongitude;
  double dLatitude;
};
   
struct AraSunPosSunCoordinates
{
  double dZenithAngle;
  double dAzimuth; //Angle from Prime Meridian i.e. Northing = cos(dAzimuth), Easting = sin(dAzimuth);
};
   
class AraSunPos
{
 public:

  AraSunPos();
  ~AraSunPos();
  static void sunpos(AraSunPosTime udtTime, AraSunPosLocation udtLocation, AraSunPosSunCoordinates *udtSunCoordinates);
  
};

#endif
