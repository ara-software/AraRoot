#include "AraHistoHandler.h"
#include <ctime>
#include <iostream>

#define TIME_BIN_SIZE 60

ClassImp(AraHistoHandler);

AraHistoHandler::~AraHistoHandler()
{

}

AraHistoHandler::AraHistoHandler(TFile *fpHist,  const char *name, const char *title )
{
  strncpy(fName,name,179);
  strncpy(fTitle,title,179);
  //Default constructor
  fHistoFile=fpHist;
  char testString[180];
  if(fHistoFile->IsOpen()) {
    fDirectory = (TDirectory*) fHistoFile->Get(name);
    if(fDirectory) {
       fDirectory->cd();
      //Now try and read in all the histos
      for(int i=0 ; i!=fDirectory->GetListOfKeys()->GetEntries() ; i++) {
	 TH1D *hist = (TH1D*)fDirectory->Get(fDirectory->GetListOfKeys()->At(i)->GetName());	
	UInt_t keyValue=0;
	sprintf(testString,"%s_%%u",fName);
	sscanf(hist->GetName(),testString,&keyValue);
	Int_t count=(Int_t)hist->GetBinContent(0);
	//	std::cerr << i << "\t" << fDirectory->GetListOfKeys()->At(i) << "\t" << fDirectory->GetListOfKeys()->At(i)->GetName() << "\t" << keyValue << "\t" << hist->GetBinContent(0) << "\n";
	//	hist->SetBinContent(0,0);

	theCountMap[keyValue]=count;
	hasChangedMap[keyValue]=0;
	theHistoMap[keyValue]=hist;
      }
    }
    else {
      fDirectory=fHistoFile->mkdir(name);
    }
  }    
}

void AraHistoHandler::addFile(TFile *fpNext)
{
  //This is going to be in theory for a read only mode of operation
  char testString[180];
  if(fpNext->IsOpen()) {
    TDirectory *tempDir = (TDirectory*) fpNext->Get(fName);
    if(tempDir) {
      tempDir->cd();
      //Now try and read in all the histos
      for(int i=0 ; i!=tempDir->GetListOfKeys()->GetEntries() ; i++) {
	TH1D *hist = (TH1D*)tempDir->Get(tempDir->GetListOfKeys()->At(i)->GetName());	
	UInt_t keyValue=0;
	sprintf(testString,"%s_%%u",fName);
	sscanf(hist->GetName(),testString,&keyValue);
	Int_t count=(Int_t)hist->GetBinContent(0);

	//Now we need to check if we have this key or not

	histoMap::iterator it=theHistoMap.find(keyValue);
	countMap::iterator countIt=theCountMap.find(keyValue);
	if(it==theHistoMap.end()) {
	  //Then we don't have this key
	  //     std::cerr << "Making " << histName << "\n";
	   fDirectory->cd();
	   TH1D *firstHist = (TH1D*) hist->Clone(testString);
	   theHistoMap[keyValue]=firstHist;
	   theCountMap[keyValue]=count;
	   hasChangedMap[keyValue]=1;
	   tempDir->cd();
	}
	else {
	  countIt->second+=count;
	  it->second->Add(hist);	  
	}
      }
    }
  }
}

void AraHistoHandler::Write()
{
  fDirectory->cd();
  for(histoMap::iterator it=theHistoMap.begin();it!=theHistoMap.end();it++) {
    UInt_t keyValue=it->first;
    //Check to see if we have actually chnaged anything
    if(hasChangedMap[keyValue]) {
      Int_t count=theCountMap[keyValue];
      //    std::cout << "Count for " << fName << "\t" << keyValue << "\t" << count << "\t" << (it->second)->GetName() << "\n" ;
      (it->second)->SetBinContent(0,count); //Dodgy use of underflow bin
      //    std::cout << (it->second)->GetBinContent(0) << "\n";
      
      (it->second)->Write(0,TObject::kWriteDelete);
      hasChangedMap[keyValue]=0;
    }
    
  }
 
}


void AraHistoHandler::addHisto(UInt_t unixTime, TH1D *histo)
{
  //  std::cout << unixTime << "\t" << histo << "\n";
  char histName[180];
  UInt_t keyValue=unixTime/TIME_BIN_SIZE;  
  sprintf(histName,"%s_%u",this->GetName(),keyValue);
  histoMap::iterator it=theHistoMap.find(keyValue);
  countMap::iterator countIt=theCountMap.find(keyValue);
  if(it==theHistoMap.end()) {
    //This is the first entry
    //     std::cerr << "Making " << histName << "\n";
    TH1D *firstHist = (TH1D*) histo->Clone(histName);
    theHistoMap[keyValue]=firstHist;
    theCountMap[keyValue]=1;
    hasChangedMap[keyValue]=1;
    
  }
  else {
    countIt->second+=1;
    it->second->Add(histo);
    hasChangedMap[keyValue]=1;
    //    std::cerr << "Pre\n";
    //    std::cerr << "Adding to " <<  histName << "\t" << theCountMap[keyValue] << "\n";
    
  }
}

TH1D *AraHistoHandler::getTimeHisto(UInt_t firstTime, UInt_t lastTime)
{
  if(theHistoMap.size()==0) return NULL;  
  histoMap::iterator firstIt=theHistoMap.lower_bound(firstTime/TIME_BIN_SIZE);
  if(firstIt==theHistoMap.end()) return NULL;
  histoMap::iterator lastIt=theHistoMap.upper_bound(lastTime/TIME_BIN_SIZE); 
  if(lastIt==theHistoMap.end()) lastIt--; //Decrement if we have already reached the end

  Int_t numInAverage=0;
  TH1D *outputHisto=NULL;
  char histName[180];
  sprintf(histName,"%s_temp",this->GetName());
  
  lastIt++; //To include lastIt
  for(histoMap::iterator it=firstIt;it!=lastIt;it++) {
    //    std::cout << (it->first) << "\t" << (it->second).second << "\n";
    UInt_t keyValue=it->first;
    TH1D *hist=(it->second);
    numInAverage+=theCountMap[keyValue];
    if(!outputHisto) {
      outputHisto = (TH1D*) hist->Clone(histName);
    }
    else {
      outputHisto->Add(hist);
    }
  }

  Double_t scaleFactor=1./numInAverage;
  outputHisto->Scale(scaleFactor);

  return outputHisto;

}

TH1D *AraHistoHandler::getTimeHisto(AraPlotTime::AraPlotTime_t plotTime)
{

  if(theHistoMap.size()==0) return NULL;  
  UInt_t lastTime=getLastTime();
 // std::cout << this->GetName() << "\t" << plotTime << "\t" << getStartTime(lastTime,plotTime) << "\t" << lastTime << "\n";
  return getTimeHisto(AraPlotTime::getStartTime(lastTime,plotTime),lastTime);
}


TH1D *AraHistoHandler::getCurrentTimeHisto(AraPlotTime::AraPlotTime_t plotTime)
{

  if(theHistoMap.size()==0) return NULL;  
  UInt_t lastTime=time(NULL);
  return getTimeHisto(AraPlotTime::getStartTime(lastTime,plotTime),lastTime);
}

UInt_t AraHistoHandler::getLastTime()
{
   if(theHistoMap.size()==0) return 0;  
   histoMap::iterator lastIt=theHistoMap.end(); //One past the end of the amp
   lastIt--;//end of map  
   return TIME_BIN_SIZE*(lastIt->first);

}



TH2D *AraHistoHandler::getTimeColourHisto(UInt_t firstTime, UInt_t lastTime,Int_t numPoints)
{

  if(theHistoMap.size()==0) return NULL;  
  histoMap::iterator firstIt=theHistoMap.lower_bound(firstTime/TIME_BIN_SIZE);
  if(firstIt==theHistoMap.end()) return NULL;
  histoMap::iterator lastIt=theHistoMap.upper_bound(lastTime/TIME_BIN_SIZE); 
  if(lastIt==theHistoMap.end()) lastIt--; //Decrement if we have already reached the end
  
  UInt_t firstKey=firstIt->first;
  UInt_t lastKey=lastIt->first;
  UInt_t stepSize=(lastKey-firstKey)/(numPoints);
  //  if(stepSize==0) 
  stepSize++;

  Int_t safetyFactor=10;
  TH1D **sliceHistos = new TH1D*[numPoints+safetyFactor];
  Double_t *numEnts = new Double_t[numPoints+safetyFactor];
  for(int i=0;i<numPoints+safetyFactor;i++) {
    sliceHistos[i]=NULL;
    numEnts[i]=0;
  }
  
  
  char histName[180];
  
  lastIt++; //To include lastIt
  Int_t yBins=0;
  Double_t minY=0;
  Double_t maxY=0;
  for(histoMap::iterator it=firstIt;it!=lastIt;it++) {
    //    std::cout << (it->first) << "\t" << (it->second).second << "\n";
    UInt_t keyValue=it->first;
    Int_t bin=(keyValue-firstKey)/stepSize;    
    TH1D *hist=(it->second);
    numEnts[bin]+=theCountMap[keyValue];
    if(!sliceHistos[bin]) {
      sprintf(histName,"%s_temp_%d",this->GetName(),bin);
      sliceHistos[bin] = (TH1D*) hist->Clone(histName);
      if(!yBins) {
	yBins=sliceHistos[bin]->GetNbinsX();
	minY=sliceHistos[bin]->GetBinLowEdge(1);
	maxY=sliceHistos[bin]->GetBinLowEdge(yBins)+sliceHistos[bin]->GetBinWidth(yBins);
      }
    }
    else {
      sliceHistos[bin]->Add(hist);
    }
  }
  Double_t minTime=(firstKey-0.5)*TIME_BIN_SIZE;
  Double_t maxTime=(lastKey+0.5)*TIME_BIN_SIZE;
  sprintf(histName,"%s_output2d",this->GetName());
  TH2D *outputHisto= new TH2D(histName,histName,numPoints,minTime,maxTime,yBins,minY,maxY);
  for(int i=0;i<numPoints;i++) {
    Double_t time=(firstKey+(i*stepSize))*TIME_BIN_SIZE;
    if(sliceHistos[i]) {
      Double_t scaleFactor=1./numEnts[i];
      sliceHistos[i]->Scale(scaleFactor);
      for(int biny=1;biny<sliceHistos[i]->GetNbinsX();biny++) {
	Double_t yVal=sliceHistos[i]->GetBinCenter(biny);
	Double_t zVal=sliceHistos[i]->GetBinContent(biny);
	outputHisto->Fill(time,yVal,zVal);
      }
      delete sliceHistos[i];
    }
  }

  return outputHisto;
  

}


TH2D *AraHistoHandler::getTimeColourHisto(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints)
{
  if(theHistoMap.size()==0) return NULL;  
  UInt_t lastTime=getLastTime();
 // std::cout << this->GetName() << "\t" << plotTime << "\t" << getStartTime(lastTime,plotTime) << "\t" << lastTime << "\n";
  return getTimeColourHisto(AraPlotTime::getStartTime(lastTime,plotTime),lastTime,numPoints);

}

TH2D *AraHistoHandler::getCurrentTimeColourHisto(AraPlotTime::AraPlotTime_t plotTime, Int_t numPoints)
{

  if(theHistoMap.size()==0) return NULL;  
  UInt_t lastTime=time(NULL);
  return getTimeColourHisto(AraPlotTime::getStartTime(lastTime,plotTime),lastTime,numPoints);
}
