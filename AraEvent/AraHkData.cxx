//////////////////////////////////////////////////////////////////////////////
/////  AraHkData.cxx        Definition of the AraHkData            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that holds AraHkData                         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraHkData.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include <cstring>
ClassImp(AraHkData);

AraHkData::AraHkData() 
{
   //Default Constructor
}

AraHkData::~AraHkData() {
   //Default Destructor
}


AraHkData::AraHkData(AraHkDataStruct_t *theHk)
{
  //From TemperatureDataStruct_t
  memcpy(temp,theHk->temp.temp,8*sizeof(unsigned short)); ///< Temperatures
  
  //From RFPowerDataStruct_t   
  memcpy(rfpDiscone,theHk->rfPow.discone,8*sizeof(unsigned short)); ///< RF Power
  memcpy(rfpBatwing,theHk->rfPow.batwing,8*sizeof(unsigned short)); ///< RF Power
  
  //From DACDataStruct_t
  memcpy(dac,theHk->dac.dac,6*4*sizeof(unsigned short)); ///< DAC for what?

  //From SimpleScalerStruct_t
  memcpy(sclDiscone,theHk->scaler.discone,8*sizeof(unsigned short));
  memcpy(sclBatPlus,theHk->scaler.batPlus,8*sizeof(unsigned short));
  memcpy(sclBatMinus,theHk->scaler.batMinus,8*sizeof(unsigned short));
  memcpy(sclTrigL1,theHk->scaler.trigL1,12*sizeof(unsigned short));
  sclGlobal=theHk->scaler.global;


}

char AraHkData::getDacLetter(int index)
{
  if(index<0 || index>5) return '?';
  char alphabet[6]={'A','B','C','D','E','F'};
  return alphabet[index];

}



double AraHkData::getRFPowerDiscone(int discId)
{
   if(discId<0 || discId>7) return 0;
   Double_t ped=813; //Average ANITA-II value
   Double_t slope=0.0404; //Average ANITA-II value
   Double_t refTemp=393; //Average ANITA-II value
   
   Double_t DA=rfpDiscone[discId]-ped;
   Double_t kelvin=refTemp*TMath::Power(10,(slope*DA/10.));
   return kelvin;

}

double AraHkData::getRFPowerBatwing(int batId)
{
   if(batId<0 || batId>7) return 0;
   Double_t ped=813; //Average ANITA-II value
   Double_t slope=0.0404; //Average ANITA-II value
   Double_t refTemp=393; //Average ANITA-II value
   
   Double_t DA=rfpBatwing[batId]-ped;
   Double_t kelvin=refTemp*TMath::Power(10,(slope*DA/10.));
   return kelvin;

}

Double_t AraHkData::getTemperature(int tempId)
{
  if (tempId<0 || tempId>7)
    return -99.9;

  double deg_a  = (char)(temp[tempId]>>2);
         deg_a += (temp[tempId]&0x3) * 0.25;

  return deg_a;
}
