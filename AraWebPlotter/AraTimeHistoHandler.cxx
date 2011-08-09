#include "AraTimeHistoHandler.h"
#include <ctime>
#include <iostream>

ClassImp(AraTimeHistoHandler);

AraTimeHistoHandler::AraTimeHistoHandler()
{
   fBinWidthInSeconds=60;
}

AraTimeHistoHandler::AraTimeHistoHandler(  const char *name, const char *title , Int_t binWidth ):TNamed(name,title)
{
  //Default constructor
   fBinWidthInSeconds=binWidth;
}

void AraTimeHistoHandler::addAraTimeHistoHandler(AraTimeHistoHandler *other)
{
  for(variableMap::iterator otherIt=other->theMap.begin();
      otherIt!=other->theMap.end();
      otherIt++) {
     UInt_t keyValue=(otherIt->first*other->fBinWidthInSeconds)/fBinWidthInSeconds;
     variableMap::iterator it=theMap.find(keyValue);
     variableMap::iterator itSq=theMapSq.find(keyValue);
     variableMap::iterator otherSqIt=other->theMapSq.find(otherIt->first);
     if(otherSqIt==other->theMapSq.end()) {
	std::cerr << "Consistency problem got variable but not variable squared\n";
	continue;
     }
     if(it==theMap.end()) {
	theMap[keyValue]=otherIt->second;
	theMapSq[keyValue]=otherSqIt->second;      
     }
     else {
	if(itSq==theMapSq.end()) {
	   std::cerr << "Consistency problem got variable but not variable squared\n";
	   continue;
	}	
	(it->second).first+=(otherIt->second).first; //This is the count
	(it->second).second+=(otherIt->second).second; //This is the variable
	(itSq->second).first+=(otherSqIt->second).first; //This is the count
	(itSq->second).second+=(otherSqIt->second).second; //This is the variable
     }
  }
  
}


void AraTimeHistoHandler::addVariable(UInt_t unixTime, Double_t variable)
{
  //  std::cout << unixTime << "\t" << variable << "\n";
  UInt_t keyValue=unixTime/fBinWidthInSeconds;  
  variableMap::iterator it=theMap.find(keyValue);
  if(it==theMap.end()) {
    //This is the first entry
    variablePair first(1,variable);
    theMap[keyValue]=first;
    variablePair firstSq(1,variable*variable);
    theMapSq[keyValue]=firstSq;
  }
  else {
    theMap[keyValue].first++;
    theMap[keyValue].second+=variable;
    theMapSq[keyValue].first++;
    theMapSq[keyValue].second+=variable*variable;
  }
}

TGraph *AraTimeHistoHandler::getTimeGraph(UInt_t firstTime, UInt_t lastTime, Int_t numPoints)
{
  if(theMap.size()==0) return NULL;  
  variableMap::iterator firstIt=theMap.lower_bound(firstTime/fBinWidthInSeconds);
  if(firstIt==theMap.end()) return NULL;
  variableMap::iterator lastIt=theMap.upper_bound(lastTime/fBinWidthInSeconds); 
  if(lastIt==theMap.end()) lastIt--; //Decrement if we have already reached the end

  UInt_t firstKey=firstIt->first;
  UInt_t lastKey=lastIt->first;
  UInt_t stepSize=(lastKey-firstKey)/(numPoints);
  //  if(stepSize==0) 
  stepSize++;
  //  std::cout << "Keys: " << firstKey  << "\t" << lastKey << "\t" << stepSize << "\n";
  Int_t safetyFactor=10;
  Double_t *times = new Double_t[numPoints+safetyFactor];
  Double_t *values = new Double_t[numPoints+safetyFactor];
  Double_t *numEnts = new Double_t[numPoints+safetyFactor];
  for(int i=0;i<numPoints+safetyFactor;i++) {
    values[i]=0;
    numEnts[i]=0;
  }
  lastIt++; //To include lastIt
  for(variableMap::iterator it=firstIt;it!=lastIt;it++) {
    //    std::cout << (it->first) << "\t" << (it->second).second << "\n";
    Int_t bin=(it->first-firstKey)/stepSize;
    //    std::cout << "bin: " << bin << "\n";
    values[bin]+=(it->second).second;
    numEnts[bin]+=(it->second).first;    
  }

  int counter=0;
  for(int i=0;i<numPoints;i++) {
    if(numEnts[i]>0) {
      times[counter]=fBinWidthInSeconds*(firstKey+i*stepSize);
      values[i]/=numEnts[i];
      values[counter]=values[i];
      counter++;
    }
  }
  //for(int i=0;i<counter;i++) {
  //  std::cerr << "\t" << i << "\t" << numPoints << "\t" << times[i] << "\t" << values[i] << "\n";
 // }

  TGraph *gr=NULL;
  if(counter>1)       
    gr = new TGraph(counter,times,values);
  //  delete [] times;
  //  delete [] values;
  //  delete [] numEnts;
  return gr;

}

TGraph *AraTimeHistoHandler::getTimeGraph(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints)
{

  if(theMap.size()==0) return NULL;  
  UInt_t lastTime=getLastTime();
  //  std::cout << this->GetName() << "\t" << plotTime << "\t" << getStartTime(lastTime,plotTime) << "\t" << lastTime << "\n";
  return getTimeGraph(AraPlotTime::getStartTime(lastTime,plotTime),lastTime,numPoints);
}


TGraph *AraTimeHistoHandler::getCurrentTimeGraph(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints)
{

  if(theMap.size()==0) return NULL;  
  UInt_t lastTime=time(NULL);
  return getTimeGraph(AraPlotTime::getStartTime(lastTime,plotTime),lastTime,numPoints);
}

UInt_t AraTimeHistoHandler::getLastTime()
{
   if(theMap.size()==0) return 0;  
   variableMap::iterator lastIt=theMap.end(); //One past the end of the amp
   lastIt--;//end of map  
   return fBinWidthInSeconds*(lastIt->first);

}
