#include "AraEventPlotter.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TMultiGraph.h"
#include "TTimeStamp.h"
#include "TSystem.h"
#include "TPaveText.h"

#include "FFTtools.h"
#include "AraCanvasMaker.h"
#include "AraGeomTool.h"

AraCanvasMaker *gEventCanvasMaker=0;

AraEventPlotter::AraEventPlotter(char *plotDir, char *dataDir)
{
   fCurrentRun=0;
   histTrigPat=0;
   fftHist=0;
   histUnixTimeUs=0;
   fHistoFile=0;
   AraPlotUtils::setDefaultStyle();
   strncpy(fPlotDir,plotDir,180);
   strncpy(fDataDir,dataDir,180);
   sprintf(fTouchFile,"%s/lastEvent",fPlotDir);
   //  initialiseCurrentRunTimeHists();

}

AraEventPlotter::~AraEventPlotter()
{
   std::cerr << "AraEventPlotter::~AraEventPlotter()\n";
   //  saveFiles();
   //  for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
   //    if(fAverageFFTHisto[ant]) {
   //      delete fAverageFFTHisto[ant];
   //    }
   //  }
   //  fHistoFile->Close();
}

void AraEventPlotter::saveFiles()
{
   if(fHistoFile) {
      fHistoFile->cd();  
      if(fEventNumberHisto) fEventNumberHisto->Write(0,TObject::kWriteDelete);
      if(fEventRateHisto) fEventRateHisto->Write(0,TObject::kWriteDelete);
      if(fPriorityHisto) fPriorityHisto->Write(0,TObject::kWriteDelete);
      if(fPpsNumHisto) fPpsNumHisto->Write(0,TObject::kWriteDelete);
      for(int i=0;i<16;i++) {
	 if(fTrigPatternHisto[i]) fTrigPatternHisto[i]->Write(0,TObject::kWriteDelete);
      }
      for(int i=0;i<8;i++) {
	 if(fCalibStatusBitHistos[i]) fCalibStatusBitHistos[i]->Write(0,TObject::kWriteDelete);
	 if(fErrorFlagBitHistos[i]) fErrorFlagBitHistos[i]->Write(0,TObject::kWriteDelete);
	 //The trig time histo handlers
	 if(fTrigTypeBitHistos[i]) fTrigTypeBitHistos[i]->Write(0,TObject::kWriteDelete);
      }
      if(fDeadTimeHisto) fDeadTimeHisto->Write(0,TObject::kWriteDelete);
      for(int i=0;i<3;i++) {
	 if(fRoVddHisto[i]) fRoVddHisto[i]->Write(0,TObject::kWriteDelete);
	 if(fRcoCountHisto[i]) fRcoCountHisto[i]->Write(0,TObject::kWriteDelete);
      }

      for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
	 if(fWaveformRMSHisto[ant]) fWaveformRMSHisto[ant]->Write(0,TObject::kWriteDelete);
	 if(fWaveformSNRHisto[ant]) fWaveformSNRHisto[ant]->Write(0,TObject::kWriteDelete);
      }

      for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
	 if(fAverageFFTHisto[ant]) fAverageFFTHisto[ant]->Write();

      }
      if(fAverageTriggerPattern) fAverageTriggerPattern->Write();
      if(fAverageUnixTimeUs) fAverageUnixTimeUs->Write();
   }
}

void AraEventPlotter::initialiseCurrentRunTimeHists()
{  
   char filename[180];
   sprintf(filename,"%s/eventTimeHists%d.root",fDataDir,fCurrentRun);
   fHistoFile = new TFile(filename,"RECREATE"); //Might switch this to RECREATE at some point
   initialiseTimeHists();
}

void AraEventPlotter::initialiseTimeHists(Int_t binWidth)
{
   char name[180];
   char title[180];
   sprintf(name,"eventNumberHisto");
   sprintf(title,"Event Number");
   fEventNumberHisto = (AraTimeHistoHandler*) fHistoFile->Get(name);
   if(!fEventNumberHisto)
      fEventNumberHisto = new AraTimeHistoHandler(name,title,binWidth);
   sprintf(name,"eventRateHisto");
   sprintf(title,"Event Rate");
   fEventRateHisto = (AraTimeHistoHandler*) fHistoFile->Get(name);
   if(!fEventRateHisto)
      fEventRateHisto = new AraTimeHistoHandler(name,title,binWidth);
   sprintf(name,"priorityHisto");
   sprintf(title,"Priority");
   fPriorityHisto = (AraTimeHistoHandler*) fHistoFile->Get(name);
   if(!fPriorityHisto)
      fPriorityHisto = new AraTimeHistoHandler(name,title,binWidth);
   sprintf(name,"ppsNumHisto");
   sprintf(title,"Pulse Pers Second Number");
   fPpsNumHisto = (AraTimeHistoHandler*) fHistoFile->Get(name);
   if(!fPpsNumHisto)
      fPpsNumHisto = new AraTimeHistoHandler(name,title,binWidth);
   for(int bit=0;bit<16;bit++) {
      sprintf(name,"trigPatternHisto%d",bit);
      sprintf(title,"Trigger Pattern (Bit %d)",bit);  
      fTrigPatternHisto[bit] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fTrigPatternHisto[bit])
	 fTrigPatternHisto[bit] = new AraTimeHistoHandler(name,title,binWidth);
   }
  
   for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
      sprintf(name,"waveformRMSHisto%d",ant);
      sprintf(title,"Waveform RMS (Ant %d)",ant);  
      fWaveformRMSHisto[ant] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fWaveformRMSHisto[ant])
	 fWaveformRMSHisto[ant] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"waveformSNRHisto%d",ant);
      sprintf(title,"Waveform SNR (Ant %d)",ant);  
      fWaveformSNRHisto[ant] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fWaveformSNRHisto[ant])
	 fWaveformSNRHisto[ant] = new AraTimeHistoHandler(name,title,binWidth);
   }
  

   for(int i=0;i<8;i++) {
      sprintf(name,"calibStatusBitHisto%d",i);
      sprintf(title,"Calib Status Bit %d",i);
      fCalibStatusBitHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fCalibStatusBitHistos[i])
	 fCalibStatusBitHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"errorFlagBitHisto%d",i);
      sprintf(title,"Error Flag Bit %d",i);
      fErrorFlagBitHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fErrorFlagBitHistos[i])
	 fErrorFlagBitHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"trigTypeBitHisto%d",i);
      sprintf(title,"Trig Type Bit %d",i);
      fTrigTypeBitHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fTrigTypeBitHistos[i])
	 fTrigTypeBitHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
   }

   for(int i=0;i<3;i++) {
      sprintf(name,"roVddBitHisto%d",i);
      sprintf(title,"RO VDD %d",i);
      fRoVddHisto[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fRoVddHisto[i])
	 fRoVddHisto[i] = new AraTimeHistoHandler(name,title,binWidth);

      sprintf(name,"rcoCountHisto%d",i);
      sprintf(title,"RCO Count %d",i);
      fRcoCountHisto[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fRcoCountHisto[i])
	 fRcoCountHisto[i] = new AraTimeHistoHandler(name,title,binWidth);
   }

   sprintf(name,"deadTimeHisto");
   sprintf(title,"Dead Time");
   fDeadTimeHisto = (AraTimeHistoHandler*) fHistoFile->Get(name);
   if(!fDeadTimeHisto)
      fDeadTimeHisto = new AraTimeHistoHandler(name,title,binWidth);
  

   for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
      sprintf(name,"averageFFTHisto%d",ant);
      sprintf(title,"Average FFT Chan %d",ant+1);
      fAverageFFTHisto[ant] = new AraHistoHandler(fHistoFile,name,title);
   }

   sprintf(name,"averageTrigPattern");
   sprintf(title,"Average Trigger Pattern");
   fAverageTriggerPattern = new AraHistoHandler(fHistoFile,name,title);

   sprintf(name,"averageUnixTimeUs");
   sprintf(title,"Unix Time Us");
   fAverageUnixTimeUs = new AraHistoHandler(fHistoFile,name,title);

}
     

void AraEventPlotter::loadAllTimeHists()
{  
   char filename[180];
   sprintf(filename,"%s/allEventTimeHists.root",fDataDir);
   fHistoFile = new TFile(filename,"RECREATE"); //Might switch this to RECREATE at some point
   initialiseTimeHists(600);
   int countFiles=0;
   //Now need to loop over all eventTimeHists run files in the directory
   void* dirp = gSystem->OpenDirectory(fDataDir);
   const char *entry = gSystem->GetDirEntry(dirp);
   while(entry != 0)
      {
	 int len = strlen(entry);
	 if(len >= 5 && strcmp(&entry[len - 5], ".root") == 0)
	    {
	       if(len >= 20 && strstr(entry,"eventTimeHists")) {
	      
		  TString fileName;
		  fileName+=fDataDir;
		  if( !fileName.EndsWith("/") ) fileName += "/";
		  fileName+=entry;
		  TFile *fpRun=TFile::Open(fileName.Data(),"OLD");

		  {
		     std::cerr << "*";
		     char name[180];
		     char title[180];
		     sprintf(name,"eventNumberHisto");
		     sprintf(title,"Event Number");
		     AraTimeHistoHandler *tempEventNumberHisto = (AraTimeHistoHandler*) fpRun->Get(name);
		     if(fEventNumberHisto && tempEventNumberHisto)
			fEventNumberHisto->addAraTimeHistoHandler(tempEventNumberHisto);

		     sprintf(name,"eventRateHisto");
		     sprintf(title,"Event Rate");
		     AraTimeHistoHandler *tempEventRateHisto = (AraTimeHistoHandler*) fpRun->Get(name);
		     if(fEventRateHisto && tempEventRateHisto)
			fEventRateHisto->addAraTimeHistoHandler(tempEventRateHisto);

		     sprintf(name,"priorityHisto");
		     sprintf(title,"Priority");
		     AraTimeHistoHandler *tempPriorityHisto = (AraTimeHistoHandler*) fpRun->Get(name);
		     if(fPriorityHisto && tempPriorityHisto)
			fPriorityHisto->addAraTimeHistoHandler(tempPriorityHisto);
    
		     sprintf(name,"ppsNumHisto");
		     sprintf(title,"Pulse Pers Second Number");
		     AraTimeHistoHandler *tempPpsNumHisto = (AraTimeHistoHandler*) fpRun->Get(name);
		     if(fPpsNumHisto && tempPpsNumHisto)
			fPpsNumHisto->addAraTimeHistoHandler(tempPpsNumHisto);
    
		     for(int bit=0;bit<16;bit++) {
			sprintf(name,"trigPatternHisto%d",bit);
			sprintf(title,"Trigger Pattern (Bit %d)",bit);  
			AraTimeHistoHandler *tempTrigPatternHisto = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fTrigPatternHisto[bit] && tempTrigPatternHisto)
			   fTrigPatternHisto[bit]->addAraTimeHistoHandler(tempTrigPatternHisto);
		     }
  
		     for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
			sprintf(name,"waveformRMSHisto%d",ant);
			sprintf(title,"Waveform RMS (Ant %d)",ant);  
			AraTimeHistoHandler *tempWaveformRMSHisto = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fWaveformRMSHisto[ant] && tempWaveformRMSHisto)
			   fWaveformRMSHisto[ant]->addAraTimeHistoHandler(tempWaveformRMSHisto);
      
			sprintf(name,"waveformSNRHisto%d",ant);
			sprintf(title,"Waveform SNR (Ant %d)",ant);  
			AraTimeHistoHandler *tempWaveformSNRHisto = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fWaveformSNRHisto[ant] && tempWaveformSNRHisto)
			   fWaveformSNRHisto[ant]->addAraTimeHistoHandler(tempWaveformSNRHisto);
		     }
    

		     for(int i=0;i<8;i++) {
			sprintf(name,"calibStatusBitHisto%d",i);
			sprintf(title,"Calib Status Bit %d",i);
			AraTimeHistoHandler *tempCalibStatusBitHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fCalibStatusBitHistos[i] && tempCalibStatusBitHistos)
			   fCalibStatusBitHistos[i]->addAraTimeHistoHandler(tempCalibStatusBitHistos);
      
			sprintf(name,"errorFlagBitHisto%d",i);
			sprintf(title,"Error Flag Bit %d",i);
			AraTimeHistoHandler *tempErrorFlagBitHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fErrorFlagBitHistos[i] && tempErrorFlagBitHistos)
			   fErrorFlagBitHistos[i]->addAraTimeHistoHandler(tempErrorFlagBitHistos);
      
			sprintf(name,"trigTypeBitHisto%d",i);
			sprintf(title,"Trig Type Bit %d",i);
			AraTimeHistoHandler *tempTrigTypeBitHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fTrigTypeBitHistos[i] && tempTrigTypeBitHistos)
			   fTrigTypeBitHistos[i]->addAraTimeHistoHandler(tempTrigTypeBitHistos);
		     }
    
		     for(int i=0;i<3;i++) {
			sprintf(name,"roVddBitHisto%d",i);
			sprintf(title,"RO VDD %d",i);
			AraTimeHistoHandler *tempRoVddHisto = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fRoVddHisto[i] && tempRoVddHisto)
			   fRoVddHisto[i]->addAraTimeHistoHandler(tempRoVddHisto);
      
			sprintf(name,"rcoCountHisto%d",i);
			sprintf(title,"RCO Count %d",i);
			AraTimeHistoHandler *tempRcoCountHisto = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fRcoCountHisto[i] && tempRcoCountHisto)
			   fRcoCountHisto[i]->addAraTimeHistoHandler(tempRcoCountHisto);
		     }
    
		     sprintf(name,"deadTimeHisto");
		     sprintf(title,"Dead Time");
		     AraTimeHistoHandler *tempDeadTimeHisto = (AraTimeHistoHandler*) fpRun->Get(name);
		     if(fDeadTimeHisto && tempDeadTimeHisto)
			fDeadTimeHisto->addAraTimeHistoHandler(tempDeadTimeHisto);
  

		     for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
			fAverageFFTHisto[ant]->addFile(fpRun);
		     }
    
    
		     fAverageTriggerPattern->addFile(fpRun);    
		     fAverageUnixTimeUs->addFile(fpRun);
		  }

		  countFiles++;
		  if(fpRun)
		    delete fpRun;
	       }
	    }
	 entry = gSystem->GetDirEntry(dirp);
      }  
   
   std::cerr << "\n";
   
   std::cout << "Found " << countFiles << " run event time hist files\n";
    
  
}




void AraEventPlotter::addEvent(Int_t runNumber,RawAraTestBedStationEvent *rawEvent)
{  
   static Int_t lastPpsNum=0;
   static Double_t tthen=0;
   static UInt_t lastEventNumber=rawEvent->head.eventNumber;
   if(fCurrentRun!=runNumber) {
      fCurrentRun=runNumber;
      fEarliestTime=rawEvent->head.unixTime;
      fLatestTime=rawEvent->head.unixTime;
   }
   else {
      if(fEarliestTime>rawEvent->head.unixTime) 
	 fEarliestTime=rawEvent->head.unixTime;
      if(fLatestTime<rawEvent->head.unixTime) 
	 fLatestTime=rawEvent->head.unixTime;
   }
   if(!fHistoFile) initialiseCurrentRunTimeHists();
   fHistoFile->cd();

   Double_t deltaT=0;

   Double_t tnow=Double_t(rawEvent->head.unixTime)+1e-6*Double_t(rawEvent->head.unixTimeUs);
   if(rawEvent->head.eventNumber>lastEventNumber) {
      Double_t dn=rawEvent->head.eventNumber-lastEventNumber;
      //    Double_tthen=Double_t(lastUnixTime)+1e-6*Double_t(lastUnixTimeUs);
      if(tnow>tthen && (tnow-tthen)<20) {
	 deltaT=(tnow-tthen)/dn;
      }
   }
   lastEventNumber=rawEvent->head.eventNumber;
   tthen=tnow;
  
  
      

    
   AraPlotUtils::updateTouchFile(fTouchFile,rawEvent->head.unixTime);
      
   fEventNumberHisto->addVariable(rawEvent->head.unixTime,rawEvent->head.eventNumber);
   if(deltaT>0)
      fEventRateHisto->addVariable(rawEvent->head.unixTime,deltaT);
  
   fPriorityHisto->addVariable(rawEvent->head.unixTime,rawEvent->head.priority);
   fPpsNumHisto->addVariable(rawEvent->head.unixTime,rawEvent->trig.ppsNum);
   for(int bit=0;bit<16;bit++) {
      fTrigPatternHisto[bit]->addVariable(rawEvent->head.unixTime,rawEvent->isInTrigPattern(bit));
   }
   for(int i=0;i<8;i++) {
      fCalibStatusBitHistos[i]->addVariable(rawEvent->head.unixTime,(((rawEvent->head.calibStatus)&(1<<i))>>i));
      fErrorFlagBitHistos[i]->addVariable(rawEvent->head.unixTime,(((rawEvent->head.errorFlag)&(1<<i))>>i));
      fTrigTypeBitHistos[i]->addVariable(rawEvent->head.unixTime,(((rawEvent->trig.trigType)&(1<<i))>>i));
   }
   if(rawEvent->trig.ppsNum!=lastPpsNum) 
      fDeadTimeHisto->addVariable(rawEvent->head.unixTime,rawEvent->trig.getDeadtime());
   lastPpsNum=rawEvent->trig.ppsNum;
   for(int i=0;i<3;i++) {
      fRcoCountHisto[i]->addVariable(rawEvent->head.unixTime,rawEvent->trig.rcoCount[i]);
      fRoVddHisto[i]->addVariable(rawEvent->head.unixTime,rawEvent->trig.rovdd[i]);
   }

   if(!histTrigPat) histTrigPat = new TH1D("temphistTrigPat","temphistTrigPat",16,-0.5,15.5);
   histTrigPat->Reset();
   for(int i=0;i<16;i++) {
      histTrigPat->Fill(i,rawEvent->isInTrigPattern(i));
   }
   fAverageTriggerPattern->addHisto(rawEvent->head.unixTime,histTrigPat);

   if(!histUnixTimeUs) histUnixTimeUs = new TH1D("temphistUnixTimeUs","temphistUnixTimeUs",200,0,1);
   histUnixTimeUs->Reset();
   histUnixTimeUs->Fill(rawEvent->head.unixTimeUs/1e6); 
   fAverageUnixTimeUs->addHisto(rawEvent->head.unixTime,histUnixTimeUs);



  
   //return;
   static TFile *fpEvent=0;
   if(!fpEvent) {  
      char histName[180];
      sprintf(histName,"%s/histEvent.root",fDataDir);
      fpEvent = new TFile(histName,"RECREATE");
   }
   else {
      fpEvent->cd();
   }
   //Now we deal with the event quantities
   UsefulAraTestBedStationEvent *usefulEventPtr = new UsefulAraTestBedStationEvent(rawEvent,AraCalType::kFirstCalib);
   //First up plot the event
   if(fEventPlotFlag) plotEvent(runNumber,usefulEventPtr);

   fHistoFile->cd();
   //Next we can make average FFT stuff
   for(int ant=0;ant<ANTS_PER_TESTBED;ant++) {
      int good=0;
      if(!fftHist) {
	 fftHist = usefulEventPtr->getFFTHistForRFChan(ant);//gEventCanvasMaker->getFFTHisto(ant);
	 if(fftHist) good=1;
      }
      else {
	 fftHist->Reset();
	 if(usefulEventPtr->fillFFTHistoForRFChan(ant,fftHist)==0)
	    good=1;
      }
      
      if(good) {
	 fAverageFFTHisto[ant]->addHisto(rawEvent->head.unixTime,fftHist);
      }
      TGraph *grChan = usefulEventPtr->getGraphFromRFChan(ant);
      if(grChan) {
	 Double_t rms=grChan->GetRMS(2);
	 fWaveformRMSHisto[ant]->addVariable(rawEvent->head.unixTime,rms);
	 Double_t peak=FFTtools::getPeakVal(grChan); //Actually just the maximum positive value
	 Double_t snr=0;
	 if(rms>0) {
	    snr=peak/rms;
	 }
	 fWaveformSNRHisto[ant]->addVariable(rawEvent->head.unixTime,snr);
	 delete grChan;
      }
   }
   fHistoFile->cd();
   delete usefulEventPtr;
}

void AraEventPlotter::makePlots()
{
   fHistoFile->cd();
   char plotTitle[180];
   char canName[FILENAME_MAX];
   UInt_t lastTime=fEventNumberHisto->getLastTime();
   std::cout << "Last Event Time: " << lastTime << "\n";
   //Assume they are all the same
   TTimeStamp clockTime((time_t)lastTime,0);
   //Event Number plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canEventNumber("canEventNumber","canEventNumber");     
      sprintf(plotTitle,"Event Number for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
      TGraph *grEventNumber[1]={0};
      for(int i=0;i<1;i++) {
	 grEventNumber[i]= fEventNumberHisto->getTimeGraph(plotTime);
      }
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canEventNumber,grEventNumber,1,plotTitle,"Time","Event Number",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canEventNumber%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canEventNumber.Print(canName);
	 canEventNumber.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canEventNumber\n";
   //Event Rate plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canEventRate("canEventRate","canEventRate");     
      sprintf(plotTitle,"Event Rate for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
      TGraph *grEventRate[1]={0};
      for(int i=0;i<1;i++) {
	 TGraph *grTemp = fEventRateHisto->getTimeGraph(plotTime);
	 if(grTemp) {
	    if(grTemp->GetN()>1) {
	       Double_t *yVals=grTemp->GetY();
	       Double_t *xVals=grTemp->GetX();
	       Double_t *newVals = new Double_t[grTemp->GetN()];
	       for(int point =0;point<grTemp->GetN();point++) {
		  if(yVals[point]>0)
		     newVals[point]=1./yVals[point];
		  else
		     newVals[point]=0;
	       }
	       grEventRate[i]= new TGraph(grTemp->GetN(),xVals,newVals);
	       delete [] newVals;	  
	    }
	    delete grTemp;
	 }
      }
      if(grEventRate[0]) {
	 TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canEventRate,grEventRate,1,plotTitle,"Time","Event Rate",1);
	 if(mg) {	
	    sprintf(canName,"%s/headers/canEventRate%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	    unlink(canName);
	    canEventRate.Print(canName);
	    canEventRate.Clear();
	    delete mg;
	 }
      }
   }
   std::cerr << "Done canEventRate\n";

   //Priority plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canPriority("canPriority","canPriority");
      TGraph *grPriority[1]={0};
      for(int i=0;i<1;i++) {
	 grPriority[i]= fPriorityHisto->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Priority for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canPriority,grPriority,1,plotTitle,"Time","Priority",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canPriority%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canPriority.Print(canName);
	 canPriority.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canPriority\n";
   //PpsNum plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canPpsNum("canPpsNum","canPpsNum");
      TGraph *grPpsNum[1]={0};
      for(int i=0;i<1;i++) {
	 grPpsNum[i]= fPpsNumHisto->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"PpsNum for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canPpsNum,grPpsNum,1,plotTitle,"Time","PpsNum",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canPpsNum%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canPpsNum.Print(canName);
	 canPpsNum.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canPpsNum\n";
   //TrigPattern plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canTrigPatternTime("canTrigPatternTime","canTrigPatternTime");
      TGraph *grTrigPattern[16]={0};
      for(int i=0;i<16;i++) {
	 grTrigPattern[i]= fTrigPatternHisto[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Trigger Pattern for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canTrigPatternTime,grTrigPattern,16,plotTitle,"Time","Trigger Pattern",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canTrigPatternTime%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canTrigPatternTime.Print(canName);
	 canTrigPatternTime.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canTrigPatternTime\n";
   AraGeomTool *fGeomTool=AraGeomTool::Instance();

   AraAntPol::AraAntPol_t polMap[16]={AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kSurface,AraAntPol::kSurface,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal};
   int antPolNumMap[16]={0,1,2,3,4,5,0,1,0,1,2,3,4,5,6,7};

   //Waveform RMS plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canWaveformRMSTime("canWaveformRMSTime","canWaveformRMSTime",600,600);
   
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(plotTitle,"Waveform RMS for %s  (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("s"));      
      titlePave.SetBorderSize(0);     
      titlePave.AddText(plotTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(2,2);
    
      TGraph *grWaveformRMS[16]={0};
      for(int i=0;i<16;i++) {
	 int chan=fGeomTool->getRFChanByPolAndAnt(polMap[i],antPolNumMap[i]);
	 grWaveformRMS[i]= fWaveformRMSHisto[chan]->getTimeGraph(plotTime);
      }
    
      char labels[4][5]={"V1-4","H1-4","VS-8","HS-8"};
      TMultiGraph *mg[4]={0};   
      for(int pad=1;pad<=4;pad++) {
	 subPad.cd(pad);
	 sprintf(plotTitle,"RMS for %s",labels[pad-1]);
      
	 mg[pad-1]=AraPlotUtils::plotMultigraph(&canWaveformRMSTime,&(grWaveformRMS[4*(pad-1)]),4,plotTitle,"Time","Waveform RMS",1);
      }
      sprintf(canName,"%s/headers/canWaveformRMSTime%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
      unlink(canName);
      canWaveformRMSTime.Print(canName);
      canWaveformRMSTime.Clear();
      for(int i=0;i<4;i++) {
	 if(mg[i]) delete mg[i];
      }
   }
   std::cerr << "Done canWaveformRMSTime\n";
    


   //Waveform SNR plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canWaveformSNRTime("canWaveformSNRTime","canWaveformSNRTime",600,600);
   
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(plotTitle,"Waveform SNR for %s  (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("s"));      
      titlePave.SetBorderSize(0);     
      titlePave.AddText(plotTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(2,2);
    
      TGraph *grWaveformSNR[16]={0};
      for(int i=0;i<16;i++) {
	 int chan=fGeomTool->getRFChanByPolAndAnt(polMap[i],antPolNumMap[i]);
	 grWaveformSNR[i]= fWaveformSNRHisto[chan]->getTimeGraph(plotTime);
      }
    
      char labels[4][5]={"V1-4","H1-4","VS-8","HS-8"};
      TMultiGraph *mg[4]={0};   
      for(int pad=1;pad<=4;pad++) {
	 subPad.cd(pad);
	 sprintf(plotTitle,"SNR for %s",labels[pad-1]);
      
	 mg[pad-1]=AraPlotUtils::plotMultigraph(&canWaveformSNRTime,&(grWaveformSNR[4*(pad-1)]),4,plotTitle,"Time","Waveform SNR",1);
      }
      sprintf(canName,"%s/headers/canWaveformSNRTime%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
      unlink(canName);
      canWaveformSNRTime.Print(canName);
      canWaveformSNRTime.Clear();
      for(int i=0;i<4;i++) {
	 if(mg[i]) delete mg[i];
      }
   }
   std::cerr << "Done canWaveformSNRTime\n";
    


   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canTrigPattern("canTrigPattern","canTrigPattern");
      TH1D *histTrigPattern = fAverageTriggerPattern->getTimeHisto(plotTime);
     
      sprintf(plotTitle,"Trigger Pattern for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
     
      histTrigPattern->SetTitle(plotTitle);
      histTrigPattern->SetXTitle("Trigger Bit");
      histTrigPattern->SetYTitle("Event Fraction");
      histTrigPattern->SetFillColor(AraPlotUtils::getNiceColour(0));
      histTrigPattern->SetLineColor(AraPlotUtils::getNiceColour(0));
      histTrigPattern->SetFillStyle(2002);
      histTrigPattern->SetStats(0);
      histTrigPattern->Draw(); 
      sprintf(canName,"%s/headers/canTrigPattern%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
      unlink(canName);
      canTrigPattern.Print(canName);
      canTrigPattern.Clear();
      delete histTrigPattern;
     
   }   
   std::cerr << "Done canTrigPattern\n";


   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canUnixTimeUs("canUnixTimeUs","canUnixTimeUs");
      TH1D *histUnixTimeUs2 = fAverageUnixTimeUs->getTimeHisto(plotTime);
     
      sprintf(plotTitle,"Unix Time (#mu s) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
     
      histUnixTimeUs2->SetTitle(plotTitle);
      histUnixTimeUs2->SetXTitle("Time s");
      histUnixTimeUs2->SetYTitle("Event Fraction");
      histUnixTimeUs2->SetFillColor(AraPlotUtils::getNiceColour(1));
      histUnixTimeUs2->SetLineColor(AraPlotUtils::getNiceColour(1));
      histUnixTimeUs2->SetFillStyle(2002);
      histUnixTimeUs2->SetStats(0);
      histUnixTimeUs2->Draw(); 
      sprintf(canName,"%s/headers/canUnixTimeUs%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
      unlink(canName);
      canUnixTimeUs.Print(canName);
      canUnixTimeUs.Clear();
      delete histUnixTimeUs2;
     
   }   
   std::cerr << "Done canUnixTimeUs\n";


   //CalibStatusBit plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canCalibStatusBit("canCalibStatusBit","canCalibStatusBit");
      TGraph *grCalibStatusBit[8]={0};
      for(int i=0;i<8;i++) {
	 grCalibStatusBit[i]= fCalibStatusBitHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Calib Status Bit for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canCalibStatusBit,grCalibStatusBit,8,plotTitle,"Time","Calib Status Bit",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canCalibStatusBit%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canCalibStatusBit.Print(canName);
	 canCalibStatusBit.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canCalibStatusBit\n";

   //ErrorFlagBit plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canErrorFlag("canErrorFlag","canErrorFlag");
      TGraph *grErrorFlagBit[8]={0};
      for(int i=0;i<8;i++) {
	 grErrorFlagBit[i]= fErrorFlagBitHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Error Flag Bit for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canErrorFlag,grErrorFlagBit,8,plotTitle,"Time","Error Flag Bit",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canErrorFlag%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canErrorFlag.Print(canName);
	 canErrorFlag.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canErrorFlag\n";

   //TrigTypeBit plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canTrigTypeBit("canTrigTypeBit","canTrigTypeBit");
      TGraph *grTrigTypeBit[8]={0};
      for(int i=0;i<8;i++) {
	 grTrigTypeBit[i]= fTrigTypeBitHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Trig Type Bit for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canTrigTypeBit,grTrigTypeBit,8,plotTitle,"Time","Trig Type Bit",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canTrigTypeBit%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canTrigTypeBit.Print(canName);
	 canTrigTypeBit.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canTrigTypeBit\n";
   

   //DeadTime plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canDeadTime("canDeadTime","canDeadTime");
      TGraph *grDeadTime[1]={0};
      grDeadTime[0]= fDeadTimeHisto->getTimeGraph(plotTime);
    
      sprintf(plotTitle,"Dead Time for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canDeadTime,grDeadTime,1,plotTitle,"Time","Dead Time (s)",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canDeadTime%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canDeadTime.Print(canName);
	 canDeadTime.Clear();
	 delete mg;
      }
   }
   std::cerr << "Done canDeadTime\n";

   //RoVdd plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canRoVdd("canRoVdd","canRoVdd");
      TGraph *grRoVdd[3]={0};
      for(int i=0;i<3;i++) {
	 grRoVdd[i]= fRoVddHisto[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"ROVDD for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRoVdd,grRoVdd,3,plotTitle,"Time","ROVDD",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canRoVdd%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canRoVdd.Print(canName);
	 canRoVdd.Clear();
	 delete mg;
      }
   } 
   std::cerr << "Done canRoVdd\n";

   //RcoCount plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canRcoCount("canRcoCount","canRcoCount");
      TGraph *grRcoCount[3]={0};
      for(int i=0;i<3;i++) {
	 grRcoCount[i]= fRcoCountHisto[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"RCO Count for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRcoCount,grRcoCount,3,plotTitle,"Time","RCO Count",1);
      if(mg) {	
	 sprintf(canName,"%s/headers/canRcoCount%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canRcoCount.Print(canName);
	 canRcoCount.Clear();
	 delete mg;
      }
   } 
   std::cerr << "Done canRcoCount\n";


   //Average FFT plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      char histTitle[180];
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canAverageFFT("canAverageFFT","canAverageFFT",800,800);      
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(histTitle,"Average Power Spectral Density Estimators for %s  (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("s"));      
      titlePave.SetBorderSize(0);     
      titlePave.AddText(histTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(4,4);
      TH1D *histFFT[ANTS_PER_TESTBED]={0};
      Double_t minVal=1e9;
      Double_t maxVal=-1e9;
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 //      std::cerr << i << fAverageFFTHisto[i] << "\n";
	 histFFT[i]= fAverageFFTHisto[i]->getTimeHisto(plotTime);
	 if(histFFT[i]) {
	    subPad.cd(i+1);
	    sprintf(histTitle,"Average PSD Chan %d",i+1);
	    histFFT[i]->SetTitle(histTitle);
	    histFFT[i]->SetLineColor(AraPlotUtils::getNiceColour(i));
	    histFFT[i]->Draw();
	    if(histFFT[i]->GetXaxis()) {
	       histFFT[i]->GetXaxis()->SetTitle("Frequency (MHz)");
	       //	  histFFT[i]->GetXaxis()->SetRangeUser(0,500);
	    }
	    if(histFFT[i]->GetYaxis()) {
	       histFFT[i]->GetYaxis()->SetTitle("Power (dB?)");
	    }
	    histFFT[i]->SetStats(0);
	    if(histFFT[i]->GetMinimum()<minVal) minVal=histFFT[i]->GetMinimum();
	    if(histFFT[i]->GetMaximum()>maxVal) maxVal=histFFT[i]->GetMaximum();

	    //	std::cout << histFFT[i]->GetMean() << "\t" << histFFT[i]->GetEntries() << "\n";
	 }
      }
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFT[i]) {
	    subPad.cd(i+1);
	    if(histFFT[i]->GetYaxis()) {
	       histFFT[i]->GetYaxis()->SetRangeUser(minVal-5,maxVal+5);
	    }
	 }
      }
	  
      
    
      sprintf(canName,"%s/headers/canAverageFFT%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
      unlink(canName);
      canAverageFFT.Print(canName);
      canAverageFFT.Clear();
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFT[i]) delete histFFT[i];
      } 
   }

   std::cerr << "Done canAverageFFT\n";


   //Average FFT plots
   Int_t numPointsArray[6]={60,60,12,24,24,48};
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      char histTitle[180];
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canAverageFFTTime("canAverageFFTTime","canAverageFFTTime",800,800);      
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(histTitle,"Average Power Spectral Density Estimators for %s  (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("s"));      
      titlePave.SetBorderSize(0);     
      titlePave.AddText(histTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(4,4);
      TH2D *histFFTTime[ANTS_PER_TESTBED]={0};
      Double_t minVal=1e9;
      Double_t maxVal=-1e9;
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 //      std::cerr << i << fAverageFFTHisto[i] << "\n";
	 histFFTTime[i]= fAverageFFTHisto[i]->getTimeColourHisto(plotTime,numPointsArray[timeInd]);
	 if(histFFTTime[i]) {
	    subPad.cd(i+1);
	    sprintf(histTitle,"Average PSD Chan %d",i+1);
	    histFFTTime[i]->SetTitle(histTitle);
	    histFFTTime[i]->SetLineColor(AraPlotUtils::getNiceColour(i));
	    histFFTTime[i]->Draw("colz");
	    if(histFFTTime[i]->GetYaxis()) {
	       histFFTTime[i]->GetXaxis()->SetTitle("Frequency (MHz)");
	       //	  histFFTTime[i]->GetXaxis()->SetRangeUser(0,500);
	    }
	    if(histFFTTime[i]->GetXaxis()) {
	       histFFTTime[i]->GetXaxis()->SetTitle("Time");
	       histFFTTime[i]->GetXaxis()->SetTimeDisplay(1);
	    }
	    histFFTTime[i]->SetStats(0);
	    if(histFFTTime[i]->GetMinimum()<minVal) minVal=histFFTTime[i]->GetMinimum();
	    if(histFFTTime[i]->GetMaximum()>maxVal) maxVal=histFFTTime[i]->GetMaximum();

	    //	std::cout << histFFTTime[i]->GetMean() << "\t" << histFFTTime[i]->GetEntries() << "\n";
	 }
      }
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFTTime[i]) {
	    subPad.cd(i+1);
	    if(histFFTTime[i]->GetZaxis()) {
	   
	       histFFTTime[i]->GetZaxis()->SetRangeUser(5,maxVal+5);
	    }
	 }
      }
	  
      
    
      sprintf(canName,"%s/headers/canAverageFFTTime%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
      unlink(canName);
      canAverageFFTTime.Print(canName);
      canAverageFFTTime.Clear();
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFTTime[i]) delete histFFTTime[i];
      } 
   }

}




void AraEventPlotter::plotEvent(Int_t runNumber,UsefulAraTestBedStationEvent *usefulEvent)
{

   gEventCanvasMaker=AraCanvasMaker::Instance();
   char gifName[180];
   if(!fAraDisplayCanvas) {
      fAraDisplayCanvas = new TCanvas("canAraDisplay","canAraDisplay",1200,800);
      fAraDisplayCanvas->cd();
   }
   if(!fAraDisplayMainPad) {
      fAraDisplayCanvas->cd();
      fAraDisplayMainPad= new TPad("canAraDisplayMain","canAraDisplayMain",0,0,1,0.9);
      fAraDisplayMainPad->Draw();
      fAraDisplayCanvas->Update();
   }
   if(!fAraDisplayEventInfoPad) {
      fAraDisplayCanvas->cd();
      fAraDisplayEventInfoPad= new TPad("canAraDisplayEventInfo","canAraDisplayEventInfo",0.2,0.91,0.8,0.99);
      fAraDisplayEventInfoPad->Draw();
      fAraDisplayCanvas->Update();
   } 

   gEventCanvasMaker->getEventInfoCanvas(usefulEvent,fAraDisplayEventInfoPad);
   //  gEventCanvasMaker->fMinVoltLimit=-100;
   //  gEventCanvasMaker->fMaxVoltLimit=100;
   gEventCanvasMaker->quickGetEventViewerCanvasForWebPlottter(usefulEvent,fAraDisplayMainPad);


   if(fEventPlotFlag) {
      char dirName[FILENAME_MAX];
      sprintf(dirName,"%s/events/all/run%d",fPlotDir,runNumber);
      gSystem->mkdir(dirName,kTRUE);
      sprintf(gifName,"%s/event_%u_%u.%u.gif",dirName,usefulEvent->head.eventNumber,usefulEvent->head.unixTime,usefulEvent->head.unixTimeUs);
      unlink(gifName);
      fAraDisplayCanvas->Print(gifName);

      char lastName[FILENAME_MAX];
      sprintf(lastName,"%s/events/lastEvent.gif",fPlotDir);
      unlink(lastName);
      link(gifName,lastName);
   }
}



void AraEventPlotter::makeLatestRunPlots()
{
   fHistoFile->cd();
   char plotTitle[180];
   char canName[FILENAME_MAX];
   char dirName[FILENAME_MAX];
   sprintf(dirName,"%s/headers/archive/run%d",fPlotDir,fCurrentRun);
   gSystem->mkdir(dirName,kTRUE);
  
   UInt_t lastTime=fEventNumberHisto->getLastTime();
   std::cout << "Last Event Time: " << lastTime << "\n";
   //Assume they are all the same
   TTimeStamp clockTime((time_t)lastTime,0);
   //Event Number plots 
   {
      TCanvas canEventNumber("canEventNumber","canEventNumber");     
      sprintf(plotTitle,"Event Number for Run %d",fCurrentRun);
      TGraph *grEventNumber[1]={0};
      for(int i=0;i<1;i++) {
	 grEventNumber[i]= fEventNumberHisto->getTimeGraph(fEarliestTime,fLatestTime);
      }
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canEventNumber,grEventNumber,1,plotTitle,"Time","Event Number",1);
      if(mg) {	
	 sprintf(canName,"%s/canEventNumber.png",dirName);
	 unlink(canName);
	 canEventNumber.Print(canName);
	 canEventNumber.Clear();
	 delete mg;
      }
   }
   //Event Rate plots
   {
      TCanvas canEventRate("canEventRate","canEventRate");     
      sprintf(plotTitle,"Event Rate for Run %d",fCurrentRun);
      TGraph *grEventRate[1]={0};
      for(int i=0;i<1;i++) {
	 TGraph *grTemp = fEventRateHisto->getTimeGraph(fEarliestTime,fLatestTime);
      

	 if(grTemp) {
	    if(grTemp->GetN()>1) {
	       Double_t *yVals=grTemp->GetY();
	       Double_t *xVals=grTemp->GetX();
	       Double_t *newVals = new Double_t[grTemp->GetN()];
	       for(int point =0;point<grTemp->GetN();point++) {
		  if(yVals[point]>0)
		     newVals[point]=1./yVals[point];
		  else
		     newVals[point]=0;
	       }
	       grEventRate[i]= new TGraph(grTemp->GetN(),xVals,newVals);
	       delete [] newVals;	  
	    }
	    delete grTemp;
	 }      
      }
      if(grEventRate[0]) {
	 TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canEventRate,grEventRate,1,plotTitle,"Time","Event Rate",1);
	 if(mg) {	
	    sprintf(canName,"%s/canEventRate.png",dirName);
	    unlink(canName);
	    canEventRate.Print(canName);
	    canEventRate.Clear();
	    delete mg;
	 }
      }
   }
   //Priority plots
   {
      TCanvas canPriority("canPriority","canPriority");
      TGraph *grPriority[1]={0};
      for(int i=0;i<1;i++) {
	 grPriority[i]= fPriorityHisto->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Priority for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canPriority,grPriority,1,plotTitle,"Time","Priority",1);
      if(mg) {	
	 sprintf(canName,"%s/canPriority.png",dirName);
	 unlink(canName);
	 canPriority.Print(canName);
	 canPriority.Clear();
	 delete mg;
      }
   }
   //PpsNum plots
   {
      TCanvas canPpsNum("canPpsNum","canPpsNum");
      TGraph *grPpsNum[1]={0};
      for(int i=0;i<1;i++) {
	 grPpsNum[i]= fPpsNumHisto->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"PpsNum for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canPpsNum,grPpsNum,1,plotTitle,"Time","PpsNum",1);
      if(mg) {	
	 sprintf(canName,"%s/canPpsNum.png",dirName);
	 unlink(canName);
	 canPpsNum.Print(canName);
	 canPpsNum.Clear();
	 delete mg;
      }
   }
   //TrigPattern plots
   {
      TCanvas canTrigPatternTime("canTrigPatternTime","canTrigPatternTime");
      TGraph *grTrigPattern[16]={0};
      for(int i=0;i<16;i++) {
	 grTrigPattern[i]= fTrigPatternHisto[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Trigger Pattern for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canTrigPatternTime,grTrigPattern,16,plotTitle,"Time","Trigger Pattern",1);
      if(mg) {	
	 sprintf(canName,"%s/canTrigPatternTime.png",dirName);
	 unlink(canName);
	 canTrigPatternTime.Print(canName);
	 canTrigPatternTime.Clear();
	 delete mg;
      }
   }

   //TrigPattern plots
   {
      TCanvas canTrigPattern("canTrigPattern","canTrigPattern");
      TH1D *histTrigPattern = fAverageTriggerPattern->getTimeHisto(fEarliestTime,fLatestTime);
      sprintf(plotTitle,"Trigger Pattern for Run %d",fCurrentRun);
      histTrigPattern->SetTitle(plotTitle);
      histTrigPattern->SetXTitle("Trigger Bit");
      histTrigPattern->SetYTitle("Event Fraction");
      histTrigPattern->SetFillColor(AraPlotUtils::getNiceColour(0));
      histTrigPattern->SetLineColor(AraPlotUtils::getNiceColour(0));
      histTrigPattern->SetFillStyle(2002);
      histTrigPattern->SetStats(0);
      histTrigPattern->Draw();    
      sprintf(canName,"%s/canTrigPattern.png",dirName);
      unlink(canName);
      canTrigPattern.Print(canName);
      canTrigPattern.Clear();
      delete histTrigPattern;
   }

   //UnixTimeUs plots
   {
      TCanvas canUnixTimeUs("canUnixTimeUs","canUnixTimeUs");
      TH1D *histUnixTimeUs2 = fAverageUnixTimeUs->getTimeHisto(fEarliestTime,fLatestTime);
      sprintf(plotTitle,"Unix Time (#mu s) for Run %d",fCurrentRun);
      histUnixTimeUs2->SetTitle(plotTitle);
      histUnixTimeUs2->SetXTitle("Unix Time (s)");
      histUnixTimeUs2->SetYTitle("Event Fraction");
      histUnixTimeUs2->SetFillColor(AraPlotUtils::getNiceColour(0));
      histUnixTimeUs2->SetLineColor(AraPlotUtils::getNiceColour(0));
      histUnixTimeUs2->SetFillStyle(2002);
      histUnixTimeUs2->SetStats(0);
      histUnixTimeUs2->Draw();    
      sprintf(canName,"%s/canUnixTimeUs.png",dirName);
      unlink(canName);
      canUnixTimeUs.Print(canName);
      canUnixTimeUs.Clear();
      delete histUnixTimeUs2;
   }
    


   AraGeomTool *fGeomTool=AraGeomTool::Instance();
   AraAntPol::AraAntPol_t polMap[16]={AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kVertical,AraAntPol::kSurface,AraAntPol::kSurface,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal,AraAntPol::kHorizontal};
   int antPolNumMap[16]={0,1,2,3,4,5,0,1,0,1,2,3,4,5,6,7};
   //Waveform RMS plots
   {
      TCanvas canWaveformRMSTime("canWaveformRMSTime","canWaveformRMSTime",600,600);
   
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(plotTitle,"Waveform RMS for Run %d",fCurrentRun);    
      titlePave.SetBorderSize(0);     
      titlePave.AddText(plotTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(2,2);
    
      TGraph *grWaveformRMS[16]={0};
      for(int i=0;i<16;i++) {
	 int chan=fGeomTool->getRFChanByPolAndAnt(polMap[i],antPolNumMap[i]);
	 grWaveformRMS[i]= fWaveformRMSHisto[chan]->getTimeGraph(fEarliestTime,fLatestTime);
      }
    
      char labels[4][5]={"V1-4","V/S","H1-4","H5-8"};
      TMultiGraph *mg[4]={0};   
      for(int pad=1;pad<=4;pad++) {
	 subPad.cd(pad);
	 sprintf(plotTitle,"RMS for %s",labels[pad-1]);
      
	 mg[pad-1]=AraPlotUtils::plotMultigraph(&canWaveformRMSTime,&(grWaveformRMS[4*(pad-1)]),4,plotTitle,"Time","Waveform RMS",1);
      }
      sprintf(canName,"%s/canWaveformRMSTime.png",dirName);
      unlink(canName);
      canWaveformRMSTime.Print(canName);
      canWaveformRMSTime.Clear();
      for(int i=0;i<4;i++) {
	 if(mg[i]) delete mg[i];
      }
   }
   std::cerr << "Done canWaveformRMSTime\n";

   //Waveform SNR plots
   {
      TCanvas canWaveformSNRTime("canWaveformSNRTime","canWaveformSNRTime",600,600);
   
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(plotTitle,"Waveform SNR for Run %d",fCurrentRun);    
      titlePave.SetBorderSize(0);     
      titlePave.AddText(plotTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(2,2);
    
      TGraph *grWaveformSNR[16]={0};
      for(int i=0;i<16;i++) {
	 int chan=fGeomTool->getRFChanByPolAndAnt(polMap[i],antPolNumMap[i]);
	 grWaveformSNR[i]= fWaveformSNRHisto[chan]->getTimeGraph(fEarliestTime,fLatestTime);
      }
    
      char labels[4][5]={"V1-4","H1-4","VS-8","HS-8"};
      TMultiGraph *mg[4]={0};   
      for(int pad=1;pad<=4;pad++) {
	 subPad.cd(pad);
	 sprintf(plotTitle,"SNR for %s",labels[pad-1]);
      
	 mg[pad-1]=AraPlotUtils::plotMultigraph(&canWaveformSNRTime,&(grWaveformSNR[4*(pad-1)]),4,plotTitle,"Time","Waveform SNR",1);
      }
      sprintf(canName,"%s/canWaveformSNRTime.png",dirName);
      unlink(canName);
      canWaveformSNRTime.Print(canName);
      canWaveformSNRTime.Clear();
      for(int i=0;i<4;i++) {
	 if(mg[i]) delete mg[i];
      }
   }
   std::cerr << "Done canWaveformSNRTime\n";


   //CalibStatusBit plots
   {
      TCanvas canCalibStatusBit("canCalibStatusBit","canCalibStatusBit");
      TGraph *grCalibStatusBit[8]={0};
      for(int i=0;i<8;i++) {
	 grCalibStatusBit[i]= fCalibStatusBitHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Calib Status Bit for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canCalibStatusBit,grCalibStatusBit,8,plotTitle,"Time","Calib Status Bit",1);
      if(mg) {	
	 sprintf(canName,"%s/canCalibStatusBit.png",dirName);
	 unlink(canName);
	 canCalibStatusBit.Print(canName);
	 canCalibStatusBit.Clear();
	 delete mg;
      }
   }

   //ErrorFlagBit plots
   {
      TCanvas canErrorFlag("canErrorFlag","canErrorFlag");
      TGraph *grErrorFlagBit[8]={0};
      for(int i=0;i<8;i++) {
	 grErrorFlagBit[i]= fErrorFlagBitHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Error Flag Bit for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canErrorFlag,grErrorFlagBit,8,plotTitle,"Time","Error Flag Bit",1);
      if(mg) {	
	 sprintf(canName,"%s/canErrorFlag.png",dirName);
	 unlink(canName);
	 canErrorFlag.Print(canName);
	 canErrorFlag.Clear();
	 delete mg;
      }
   }

   //TrigTypeBit plots
   {
      TCanvas canTrigTypeBit("canTrigTypeBit","canTrigTypeBit");
      TGraph *grTrigTypeBit[8]={0};
      for(int i=0;i<8;i++) {
	 grTrigTypeBit[i]= fTrigTypeBitHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Trig Type Bit for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canTrigTypeBit,grTrigTypeBit,8,plotTitle,"Time","Trig Type Bit",1);
      if(mg) {	
	 sprintf(canName,"%s/canTrigTypeBit.png",dirName);
	 unlink(canName);
	 canTrigTypeBit.Print(canName);
	 canTrigTypeBit.Clear();
	 delete mg;
      }
   }
   

   //DeadTime plots
   {
      TCanvas canDeadTime("canDeadTime","canDeadTime");
      TGraph *grDeadTime[1]={0};
      grDeadTime[0]= fDeadTimeHisto->getTimeGraph(fEarliestTime,fLatestTime);
    
      sprintf(plotTitle,"Dead Time for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canDeadTime,grDeadTime,1,plotTitle,"Time","Dead Time (s)",1);
      if(mg) {	
	 sprintf(canName,"%s/canDeadTime.png",dirName);
	 unlink(canName);
	 canDeadTime.Print(canName);
	 canDeadTime.Clear();
	 delete mg;
      }
   }
   //RoVdd plots
   {
      TCanvas canRoVdd("canRoVdd","canRoVdd");
      TGraph *grRoVdd[3]={0};
      for(int i=0;i<3;i++) {
	 grRoVdd[i]= fRoVddHisto[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"ROVDD for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRoVdd,grRoVdd,3,plotTitle,"Time","ROVDD",1);
      if(mg) {	
	 sprintf(canName,"%s/canRoVdd.png",dirName);
	 unlink(canName);
	 canRoVdd.Print(canName);
	 canRoVdd.Clear();
	 delete mg;
      }
   } 

   //RcoCount plots
   {
      TCanvas canRcoCount("canRcoCount","canRcoCount");
      TGraph *grRcoCount[3]={0};
      for(int i=0;i<3;i++) {
	 grRcoCount[i]= fRcoCountHisto[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"RCO Count for Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRcoCount,grRcoCount,3,plotTitle,"Time","RCO Count",1);
      if(mg) {	
	 sprintf(canName,"%s/canRcoCount.png",dirName);
	 unlink(canName);
	 canRcoCount.Print(canName);
	 canRcoCount.Clear();
	 delete mg;
      }
   } 


   //Average FFT plots
   {
      char histTitle[180];
      TCanvas canAverageFFT("canAverageFFT","canAverageFFT",800,800);      
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(histTitle,"Average Power Spectral Density Estimators for Run %d",fCurrentRun);      
      titlePave.SetBorderSize(0);     
      titlePave.AddText(histTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(4,4);
      TH1D *histFFT[ANTS_PER_TESTBED]={0};
      Double_t minVal=1e9;
      Double_t maxVal=-1e9;
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 //      std::cerr << i << fAverageFFTHisto[i] << "\n";
	 histFFT[i]= fAverageFFTHisto[i]->getTimeHisto(fEarliestTime,fLatestTime);
	 if(histFFT[i]) {
	    subPad.cd(i+1);
	    sprintf(histTitle,"Average PSD Chan %d",i+1);
	    histFFT[i]->SetTitle(histTitle);
	    histFFT[i]->SetLineColor(AraPlotUtils::getNiceColour(i));
	    histFFT[i]->Draw();
	    if(histFFT[i]->GetXaxis()) {
	       histFFT[i]->GetXaxis()->SetTitle("Frequency (MHz)");
	       //	  histFFT[i]->GetXaxis()->SetRangeUser(0,500);
	    }
	    if(histFFT[i]->GetYaxis()) {
	       histFFT[i]->GetYaxis()->SetTitle("Power (dB?)");
	    }
	    histFFT[i]->SetStats(0);
	    if(histFFT[i]->GetMinimum()<minVal) minVal=histFFT[i]->GetMinimum();
	    if(histFFT[i]->GetMaximum()>maxVal) maxVal=histFFT[i]->GetMaximum();

	    //	std::cout << histFFT[i]->GetMean() << "\t" << histFFT[i]->GetEntries() << "\n";
	 }
      }
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFT[i]) {
	    subPad.cd(i+1);
	    if(histFFT[i]->GetYaxis()) {
	       histFFT[i]->GetYaxis()->SetRangeUser(minVal-5,maxVal+5);
	    }
	 }
      }	        
    
      sprintf(canName,"%s/canAverageFFT.png",dirName);
      unlink(canName);
      canAverageFFT.Print(canName);
      canAverageFFT.Clear();
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFT[i]) delete histFFT[i];
      } 
   }


   {
      char histTitle[180];
      TCanvas canAverageFFTTime("canAverageFFTTime","canAverageFFTTime",800,800);      
      TPaveText titlePave(0.02,0.95,0.98,0.98);      
      sprintf(histTitle,"Average Power Spectral Density Estimators for Run %d",fCurrentRun);
      titlePave.SetBorderSize(0);     
      titlePave.AddText(histTitle);      
      titlePave.Draw();  
      TPad subPad("subPad","subPad",0,0,1,0.95);	         
      subPad.Draw();      
      subPad.SetLogy(0);         
      subPad.Divide(4,4);
      TH2D *histFFTTime[ANTS_PER_TESTBED]={0};
      Double_t minVal=1e9;
      Double_t maxVal=-1e9;
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 //      std::cerr << i << fAverageFFTHisto[i] << "\n";
	 histFFTTime[i]= fAverageFFTHisto[i]->getTimeColourHisto(fEarliestTime,fLatestTime,12);
	 if(histFFTTime[i]) {
	    subPad.cd(i+1);
	    sprintf(histTitle,"Average PSD Chan %d",i+1);
	    histFFTTime[i]->SetTitle(histTitle);
	    histFFTTime[i]->SetLineColor(AraPlotUtils::getNiceColour(i));
	    histFFTTime[i]->Draw("colz");
	    if(histFFTTime[i]->GetYaxis()) {
	       histFFTTime[i]->GetYaxis()->SetTitle("Frequency (MHz)");
	    }
	    if(histFFTTime[i]->GetZaxis()) {
	       histFFTTime[i]->GetZaxis()->SetTitle("dB(?)");
	    }
	    if(histFFTTime[i]->GetXaxis()) {
	       histFFTTime[i]->GetXaxis()->SetTitle("Time");
	       histFFTTime[i]->GetXaxis()->SetTimeDisplay(1);
	    }
	    histFFTTime[i]->SetStats(0);
	    if(histFFTTime[i]->GetMinimum()<minVal) minVal=histFFTTime[i]->GetMinimum();
	    if(histFFTTime[i]->GetMaximum()>maxVal) maxVal=histFFTTime[i]->GetMaximum();

	    //	std::cout << histFFTTime[i]->GetMean() << "\t" << histFFTTime[i]->GetEntries() << "\n";
	 }
      }
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFTTime[i]) {
	    subPad.cd(i+1);
	    if(histFFTTime[i]->GetZaxis()) {
	       histFFTTime[i]->GetZaxis()->SetRangeUser(5,maxVal+5);
	    }
	 }
      }
	  
      
    
      sprintf(canName,"%s/canAverageFFTTime.png",dirName);
      unlink(canName);
      canAverageFFTTime.Print(canName);
      canAverageFFTTime.Clear();
      for(int i=0;i<ANTS_PER_TESTBED;i++) {
	 if(histFFTTime[i]) delete histFFTTime[i];
      } 
   }

}
