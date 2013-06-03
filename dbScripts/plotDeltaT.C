#include "AraStationInfo.h"
#include "AraCalAntennaInfo.h"
#include "AraAntennaInfo.h"
#include "TMath.h"
#include "araSoft.h"

void fillDeltaArray(Int_t stationId, Double_t antLocations[][3], Double_t calLocations[][3], Double_t deltaTArray[][CAL_ANTS_PER_ATRI], Double_t deltaRArray[][CAL_ANTS_PER_ATRI]);
Double_t getDeltaR(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);
Double_t getDeltaT(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2);
Double_t getDeltaR(Double_t *locA, Double_t *locB);
Double_t getDeltaT(Double_t *locA, Double_t *locB);
Double_t getDeltaT(AraCalAntennaInfo *calAnt,AraAntennaInfo *ant1, AraAntennaInfo *ant2);

Int_t getClosestAnt(Int_t calAnt, Double_t deltaRArray[][CAL_ANTS_PER_ATRI]);
AraAntennaInfo* getClosestAnt(AraCalAntennaInfo *calAnt);

void plotDeltaT(Int_t stationId){


  Double_t antLoc[ANTS_PER_ATRI][3]={{0}};
  Double_t calLoc[CAL_ANTS_PER_ATRI][3]={{0}};
  Double_t deltaR[ANTS_PER_ATRI][CAL_ANTS_PER_ATRI]={{0}};
  Double_t deltaT[ANTS_PER_ATRI][CAL_ANTS_PER_ATRI]={{0}};

  fillDeltaArray(stationId, antLoc, calLoc, deltaT, deltaR);

  AraStationInfo *tempInfo = new AraStationInfo(stationId);
  for(int cal=0;cal<CAL_ANTS_PER_ATRI;cal++){
    AraCalAntennaInfo *calInfo = tempInfo->getCalAntennaInfo(cal);
    printf("calAnt %i %s\n", cal, AraAntPol::antPolAsString(calInfo->polType));
    AraAntennaInfo *antInfo = getClosestAnt(calInfo);
    
    Double_t *calPos = calInfo->getLocationXYZ();
    Double_t *antPos =  antInfo->getLocationXYZ();
    Double_t minDeltaR = getDeltaR(calPos,antPos);
    Double_t minDeltaT = getDeltaT(calPos,antPos);
    
    // printf("calAnt %i %s - \tclosest ant chanNum %i %s    \tantPolNum %i daqChanNum %i\nminDeltaR %f minDeltaT %f\n\n\n",
    // 	   cal, AraAntPol::antPolAsString(calInfo->polType),
    // 	   antInfo->chanNum, AraAntPol::antPolAsString(antInfo->polType), antInfo->antPolNum, antInfo->daqChanNum,
    // 	   minDeltaR, minDeltaT);
    delete antInfo;

  }
  


  


}

void fillDeltaArray(Int_t stationId, Double_t antLocations[][3], Double_t calLocations[][3], Double_t deltaTArray[][CAL_ANTS_PER_ATRI], Double_t deltaRArray[][CAL_ANTS_PER_ATRI]){
  AraStationInfo *stationInfo = new AraStationInfo(stationId);
  //printf("made stationInfo %i\n", stationInfo);
  for(int i=0;i<ANTS_PER_ATRI;i++){
    Double_t *location = stationInfo->getAntennaInfo(i)->getLocationXYZ();
    antLocations[i][0] = location[0];
    antLocations[i][1] = location[1];    
    antLocations[i][2] = location[2];
    //printf("done antLocations\n");
  }
  for(int i=0;i<CAL_ANTS_PER_ATRI;i++){
    Double_t *antLoc = stationInfo->getCalAntennaInfo(i)->getLocationXYZ();
    calLocations[i][0] = antLoc[0];
    calLocations[i][1] = antLoc[1];    
    calLocations[i][2] = antLoc[2];
  }
  for(int ant=0;ant<ANTS_PER_ATRI;ant++){
    for(int cal=0;cal<CAL_ANTS_PER_ATRI;cal++){
      deltaTArray[ant][cal]=getDeltaT(antLocations[ant][0],antLocations[ant][1],antLocations[ant][2],calLocations[cal][0],calLocations[cal][1],calLocations[cal][2]);
      deltaRArray[ant][cal]=getDeltaR(antLocations[ant][0],antLocations[ant][1],antLocations[ant][2],calLocations[cal][0],calLocations[cal][1],calLocations[cal][2]);
       //printf("ant %i cal %i antLoc %f %f %f calLoc %f %f %f deltaR %f deltaT %f\n", ant, cal, 
       //antLocations[ant][0],antLocations[ant][1],antLocations[ant][2],
       //calLocations[ant][0],calLocations[ant][1],calLocations[ant][2],
       //deltaRArray[ant][cal], deltaTArray[ant][cal]);
     }
   }

   delete stationInfo;

 }


 Double_t getDeltaR(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2){
   Double_t deltaR = TMath::Sqrt( TMath::Power(x1-x2,2) +TMath::Power(y1-y2,2) +TMath::Power(z1-z2,2) );
   return deltaR;
 }

 Double_t getDeltaT(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2){
   Double_t deltaR = getDeltaR(x1,y1,z1,x2,y2,z2);
   Double_t n_ice = 1.75;
   Double_t deltaT = deltaR*n_ice/TMath::C();
   deltaT*=1e9;
   return deltaT;
 }

Double_t getDeltaR(Double_t *locA, Double_t *locB){
  Double_t deltaR = TMath::Sqrt( TMath::Power(locA[0]-locB[0],2) +TMath::Power(locA[1]-locB[1],2) +TMath::Power(locA[2]-locB[2],2) );
  return deltaR;
}

Double_t getDeltaT(Double_t *locA, Double_t *locB){
  Double_t deltaR = getDeltaR(locA, locB);
   Double_t n_ice = 1.75;
   Double_t deltaT = deltaR*n_ice/TMath::C();
   deltaT*=1e9;
   return deltaT;
}

Double_t getDeltaT(AraCalAntennaInfo *calAnt,AraAntennaInfo *ant1, AraAntennaInfo *ant2){
  Double_t tAnt1=getDeltaT( ant1->getLocationXYZ(), calAnt->getLocationXYZ());
  Double_t tAnt2=getDeltaT( ant2->getLocationXYZ(), calAnt->getLocationXYZ());
  Double_t deltaTAnt12=tAnt2-tAnt1;
  
  return deltaTAnt12;
    
}

Int_t getClosestAnt(Int_t calAnt, Double_t deltaRArray[][CAL_ANTS_PER_ATRI]){
   Double_t minR=1e9;
   Int_t minAnt=-1;
   for(int ant=0;ant<ANTS_PER_ATRI;ant++){
     if(deltaRArray[ant][calAnt] < minR){
       minR = deltaRArray[ant][calAnt];
       minAnt=ant;
     }
   }
   return minAnt;
}

AraAntennaInfo* getClosestAnt(AraCalAntennaInfo *calAnt){
  AraAntPol::AraAntPol_t calPol = calAnt->polType;
  Int_t thisStationId = calAnt->fStationId;
  AraStationInfo *tempStationInfo = new AraStationInfo(thisStationId);
  Int_t numAntennas =  tempStationInfo->getNumAntennasByPol(calPol);
  
  // printf("calPol %i %s stationId %i numAntennas %i\n",
  // 	 calPol, AraAntPol::antPolAsString(calPol), thisStationId, numAntennas);

  Double_t minR=1e9;
  Double_t minT=1e9;
  Int_t minAntNum=-1;
  
  AraAntennaInfo *ant0Info = tempStationInfo->getAntennaInfo(0, calPol);

  for(int antNum=0; antNum<numAntennas/2;antNum++){//Just an ordering thingy -- get Top then Bottom for string 1, then string 2...
    AraAntennaInfo *tempAntInfo = tempStationInfo->getAntennaInfo(antNum, calPol);
    Double_t tempR = getDeltaR( tempAntInfo->getLocationXYZ(), calAnt->getLocationXYZ());
    Double_t tempT = getDeltaT( tempAntInfo->getLocationXYZ(), calAnt->getLocationXYZ());
    Double_t deltaT = getDeltaT(calAnt, ant0Info, tempAntInfo);
    printf("%s deltaT %f - deltaT from %s %f\n",
	   tempAntInfo->designator, tempT, ant0Info->designator,deltaT);



    if(tempR < minR){
      minR = tempR;
      minT = tempT;
      minAntNum = antNum;
    }
    // tempAntInfo = tempStationInfo->getAntennaInfo(antNum+4, calPol);
    // tempR = getDeltaR( tempAntInfo->getLocationXYZ(), calAnt->getLocationXYZ());
    // tempT = getDeltaT( tempAntInfo->getLocationXYZ(), calAnt->getLocationXYZ());
    // printf("%s chanNum %i daqChanNum %i deltaR %f deltaT %f\n",
    // 	   tempAntInfo->designator, tempAntInfo->chanNum, tempAntInfo->daqChanNum, tempR, tempT);
    
    // if(tempR < minR){
    //   minR = tempR;
    //   minT = tempT;
    //   minAntNum = antNum;
    // }
  }
  
  AraAntennaInfo *tempAntInfo = tempStationInfo->getAntennaInfo(minAntNum, calPol);
  AraAntennaInfo *theAntInfo = (AraAntennaInfo*)(tempAntInfo->Clone());

  delete tempStationInfo;
  
  return theAntInfo;
  
}

