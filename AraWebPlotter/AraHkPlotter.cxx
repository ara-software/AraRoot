#include "AraHkPlotter.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TMultiGraph.h"
#include "TTimeStamp.h"
#include "TSystem.h"

AraHkPlotter::AraHkPlotter(char *plotDir, char *dataDir)  
{  
   fCurrentRun=0;
   fHistoFile=0;
   AraPlotUtils::setDefaultStyle();
   strncpy(fPlotDir,plotDir,180);
   strncpy(fDataDir,dataDir,180);
   sprintf(fTouchFile,"%s/lastHk",fPlotDir);
   //   std::cout << "AraHkPlotter " << fPlotDir << "\t" << fDataDir << "\n";
}

AraHkPlotter::~AraHkPlotter()
{
   saveFiles();
   fHistoFile->Close();   
}

void AraHkPlotter::saveFiles()
{
   
   if(fHistoFile) {
      fHistoFile->cd();
      for(int i=0;i<8;i++) {
	 if(fTempHistos[i]) fTempHistos[i]->Write(0,TObject::kWriteDelete);
	 if(fRfpDisconeHistos[i]) fRfpDisconeHistos[i]->Write(0,TObject::kWriteDelete);
	 if(fRfpBatwingHistos[i]) fRfpBatwingHistos[i]->Write(0,TObject::kWriteDelete);
	 if(fSclDisconeHistos[i]) fSclDisconeHistos[i]->Write(0,TObject::kWriteDelete);
	 if(fSclBatPlusHistos[i]) fSclBatPlusHistos[i]->Write(0,TObject::kWriteDelete);
	 if(fSclBatMinusHistos[i]) fSclBatMinusHistos[i]->Write(0,TObject::kWriteDelete);
      }

      
      for(int i=0;i<6;i++) {
	 for(int j=0;j<4;j++) {
	    if(fDacHistos[i][j]) fDacHistos[i][j]->Write(0,TObject::kWriteDelete);
	 }
      }


      for(int i=0;i<12;i++) {
	 if(fSclTrigL1Histos[i]) fSclTrigL1Histos[i]->Write(0,TObject::kWriteDelete);
      }

      if(fSclGlobalHisto) fSclGlobalHisto->Write(0,TObject::kWriteDelete);
   }
}

void AraHkPlotter::initialiseCurrentRunTimeHists()
{
   char filename[180];
   sprintf(filename,"%s/hkTimeHists%d.root",fDataDir,fCurrentRun);
   fHistoFile = new TFile(filename,"UPDATE"); 
   initialiseTimeHists();
}

void AraHkPlotter::initialiseTimeHists(Int_t binWidth)
{
   char name[180];
   char title[180];
   for(int i=0;i<8;i++) {
      sprintf(name,"tempHisto%d",i);
      sprintf(title,"Temperature %d",i+1);
      fTempHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fTempHistos[i])
	 fTempHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"rfpDisconeHisto%d",i);
      sprintf(title,"RF Power -- Discone %d",i+1);
      fRfpDisconeHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fRfpDisconeHistos[i])
	 fRfpDisconeHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"rfpBatwingHisto%d",i);
      sprintf(title,"RF Power -- Batwing %d",i+1);
      fRfpBatwingHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fRfpBatwingHistos[i])
	 fRfpBatwingHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"sclDisconeHisto%d",i);
      sprintf(title,"Scaler -- Discone %d",i+1);
      fSclDisconeHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fSclDisconeHistos[i])
	 fSclDisconeHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"sclBatPlusHisto%d",i);
      sprintf(title,"Scaler -- Bat+ %d",i+1);
      fSclBatPlusHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fSclBatPlusHistos[i])
	 fSclBatPlusHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
      sprintf(name,"sclBatMinusHisto%d",i);
      sprintf(title,"Scaler -- Bat- %d",i+1);
      fSclBatMinusHistos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fSclBatMinusHistos[i])
	 fSclBatMinusHistos[i] = new AraTimeHistoHandler(name,title,binWidth);
   }
  

   for(int i=0;i<6;i++) {
      for(int j=0;j<4;j++) {
	 sprintf(name,"dacHisto%d_%d",i,j);
	 sprintf(title,"DAC %c %d",AraHkData::getDacLetter(i),j+1);
	 fDacHistos[i][j] = (AraTimeHistoHandler*) fHistoFile->Get(name);
	 if(!fDacHistos[i][j])
	    fDacHistos[i][j]= new AraTimeHistoHandler(name,title,binWidth);
      }
   }

   for(int i=0;i<12;i++) {
      sprintf(name,"sclTrigL1Histo%d",i);
      sprintf(title,"Scaler L1 %d",i+1);
      fSclTrigL1Histos[i] = (AraTimeHistoHandler*) fHistoFile->Get(name);
      if(!fSclTrigL1Histos[i])
	 fSclTrigL1Histos[i]=new AraTimeHistoHandler(name,title,binWidth);
   }

   sprintf(name,"sclGlobalHisto");
   sprintf(title,"Scaler Global");
   fSclGlobalHisto = (AraTimeHistoHandler*) fHistoFile->Get(name);
   if(!fSclGlobalHisto)
      fSclGlobalHisto = new AraTimeHistoHandler(name,title,binWidth);

}

void AraHkPlotter::loadAllTimeHists()
{
   char filename[180];
   sprintf(filename,"%s/allHkTimeHists.root",fDataDir);
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
	       if(len >= 17 && strstr(entry,"hkTimeHists")) {
	      
		  TString fileName;
		  fileName+=fDataDir;
		  if( !fileName.EndsWith("/") ) fileName += "/";
		  fileName+=entry;
		  TFile *fpRun=TFile::Open(fileName.Data(),"OLD");
		  {
		     std::cerr << "*";
		     char name[180];
		     char title[180];
		     for(int i=0;i<8;i++) {
			sprintf(name,"tempHisto%d",i);
			sprintf(title,"Temperature %d",i+1);
			AraTimeHistoHandler *tempTempHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fTempHistos[i] && tempTempHistos)
			   fTempHistos[i]->addAraTimeHistoHandler(tempTempHistos);

			sprintf(name,"rfpDisconeHisto%d",i);
			sprintf(title,"RF Power -- Discone %d",i+1);
			AraTimeHistoHandler *tempRfpDisconeHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fRfpDisconeHistos[i] && tempRfpDisconeHistos)
			   fRfpDisconeHistos[i]->addAraTimeHistoHandler(tempRfpDisconeHistos);

			sprintf(name,"rfpBatwingHisto%d",i);
			sprintf(title,"RF Power -- Batwing %d",i+1);
			AraTimeHistoHandler *tempRfpBatwingHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fRfpBatwingHistos[i] && tempRfpBatwingHistos)
			   fRfpBatwingHistos[i]->addAraTimeHistoHandler(tempRfpBatwingHistos);

			sprintf(name,"sclDisconeHisto%d",i);
			sprintf(title,"Scaler -- Discone %d",i+1);
			AraTimeHistoHandler *tempSclDisconeHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fSclDisconeHistos[i] && tempSclDisconeHistos)
			   fSclDisconeHistos[i]->addAraTimeHistoHandler(tempSclDisconeHistos);
      
			sprintf(name,"sclBatPlusHisto%d",i);
			sprintf(title,"Scaler -- Bat+ %d",i+1);
			AraTimeHistoHandler *tempSclBatPlusHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fSclBatPlusHistos[i] && tempSclBatPlusHistos)
			   fSclBatPlusHistos[i]->addAraTimeHistoHandler(tempSclBatPlusHistos);
      
			sprintf(name,"sclBatMinusHisto%d",i);
			sprintf(title,"Scaler -- Bat- %d",i+1);
			AraTimeHistoHandler *tempSclBatMinusHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fSclBatMinusHistos[i] && tempSclBatMinusHistos )
			   fSclBatMinusHistos[i]->addAraTimeHistoHandler(tempSclBatMinusHistos);
		     }
  

		     for(int i=0;i<6;i++) {
			for(int j=0;j<4;j++) {
			   sprintf(name,"dacHisto%d_%d",i,j);
			   sprintf(title,"DAC %c %d",AraHkData::getDacLetter(i),j+1);
			   AraTimeHistoHandler *tempDacHistos = (AraTimeHistoHandler*) fpRun->Get(name);
			   if(fDacHistos[i][j] && tempDacHistos)
			      fDacHistos[i][j]->addAraTimeHistoHandler(tempDacHistos);
			}
		     }

		     for(int i=0;i<12;i++) {
			sprintf(name,"sclTrigL1Histo%d",i);
			sprintf(title,"Scaler L1 %d",i+1);
			AraTimeHistoHandler *tempSclTrigL1Histos = (AraTimeHistoHandler*) fpRun->Get(name);
			if(fSclTrigL1Histos[i] && tempSclTrigL1Histos)
			   fSclTrigL1Histos[i]->addAraTimeHistoHandler(tempSclTrigL1Histos);
		     }
    
		     sprintf(name,"sclGlobalHisto");
		     sprintf(title,"Scaler Global");
		     AraTimeHistoHandler *tempSclGlobalHisto = (AraTimeHistoHandler*) fpRun->Get(name);
		     if(fSclGlobalHisto && tempSclGlobalHisto)
			fSclGlobalHisto->addAraTimeHistoHandler(tempSclGlobalHisto);
		  }
		  countFiles++;
		  if(fpRun)
		     delete fpRun;
	       }
	    }
	 entry = gSystem->GetDirEntry(dirp);
      }  
   std::cerr << "\n";
   std::cout << "Found " << countFiles << " run hk time hist files\n";
  

}



void AraHkPlotter::addHk(Int_t runNumber,UInt_t unixTime, AraHkData *hkData)
{
   if(fCurrentRun!=runNumber) {
      fCurrentRun=runNumber;
      fEarliestTime=unixTime;
      fLatestTime=unixTime;
   }
   else {
      if(fEarliestTime>unixTime) 
	 fEarliestTime=unixTime;
      if(fLatestTime<unixTime) 
	 fLatestTime=unixTime;
   }
   if(!fHistoFile) initialiseCurrentRunTimeHists();
   fHistoFile->cd();
   AraPlotUtils::updateTouchFile(fTouchFile,unixTime);
   //Here we will need to add something to calibrate these numbers
   for(int i=0;i<8;i++) {
      fTempHistos[i]->addVariable(unixTime,hkData->getTemperature(i));
      fRfpDisconeHistos[i]->addVariable(unixTime,hkData->getRFPowerDiscone(i));
      fRfpBatwingHistos[i]->addVariable(unixTime,hkData->getRFPowerBatwing(i));
      fSclDisconeHistos[i]->addVariable(unixTime,hkData->sclDiscone[i]);
      fSclBatPlusHistos[i]->addVariable(unixTime,hkData->sclBatPlus[i]);
      fSclBatMinusHistos[i]->addVariable(unixTime,hkData->sclBatMinus[i]);
   }
  
   for(int i=0;i<6;i++) {
      for(int j=0;j<4;j++) {
	 fDacHistos[i][j]->addVariable(unixTime,hkData->dac[i][j]);
      }
   }
  
   for(int i=0;i<12;i++) {
      fSclTrigL1Histos[i]->addVariable(unixTime,hkData->sclTrigL1[i]);
   }
  
   fSclGlobalHisto->addVariable(unixTime,hkData->sclGlobal);

}

void AraHkPlotter::makePlots()
{
   fHistoFile->cd();
   char plotTitle[180];
   char canName[FILENAME_MAX];
   UInt_t lastTime=fTempHistos[0]->getLastTime();
   std::cout << "Last Time: " << lastTime << "\n";
   //Assume they are all the same
   TTimeStamp clockTime((time_t)lastTime,0);

   //First up lets do the temperature plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      //      if(timeInd==(int)AraPlotTime::kOneHour) continue;
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canTemp("canTemp","canTemp");
      TGraph *grTemp[8]={0};
      for(int i=0;i<8;i++) {
	 grTemp[i]= fTempHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Temperatures for %s (Last time: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canTemp,grTemp,8,plotTitle,"Time","Temp (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canTemp%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canTemp.Print(canName);
	 canTemp.Clear();
	 delete mg;
      }
   }


   //Next the RF Power Plots --Discones
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canRfpDiscone("canRfpDiscone","canRfpDiscone");
      TGraph *grRfpDiscone[8]={0};
      for(int i=0;i<8;i++) {
	 grRfpDiscone[i]= fRfpDisconeHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"RF Power (Discones) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRfpDiscone,grRfpDiscone,8,plotTitle,"Time","RF Power (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canRfpDiscone%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canRfpDiscone.Print(canName);
	 canRfpDiscone.Clear();
	 delete mg;
      }
   }


   //Next the RF Power Plots --Batwings
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canRfpBatwing("canRfpBatwing","canRfpBatwing");
      TGraph *grRfpBatwing[8]={0};
      for(int i=0;i<8;i++) {
	 grRfpBatwing[i]= fRfpBatwingHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"RF Power (Batwings) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRfpBatwing,grRfpBatwing,8,plotTitle,"Time","RF Power (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canRfpBatwing%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canRfpBatwing.Print(canName);
	 canRfpBatwing.Clear();
	 delete mg;
      }
   }



   //Next the Scaler Plots -- Discones
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canSclDiscone("canSclDiscone","canSclDiscone");
      TGraph *grSclDiscone[8]={0};
      for(int i=0;i<8;i++) {
	 grSclDiscone[i]= fSclDisconeHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Scalers (Discones) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclDiscone,grSclDiscone,8,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canSclDiscone%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canSclDiscone.Print(canName);
	 canSclDiscone.Clear();
	 delete mg;
      }
   }


   //Next the Scaler Plots -- Bat+
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canSclBatPlus("canSclBatPlus","canSclBatPlus");
      TGraph *grSclBatPlus[8]={0};
      for(int i=0;i<8;i++) {
	 grSclBatPlus[i]= fSclBatPlusHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Scalers (Bat+) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclBatPlus,grSclBatPlus,8,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canSclBatPlus%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canSclBatPlus.Print(canName);
	 canSclBatPlus.Clear();
	 delete mg;
      }
   }


   //Next the Scaler Plots -- Bat-
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canSclBatMinus("canSclBatMinus","canSclBatMinus");
      TGraph *grSclBatMinus[8]={0};
      for(int i=0;i<8;i++) {
	 grSclBatMinus[i]= fSclBatMinusHistos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Scalers (Bat-) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclBatMinus,grSclBatMinus,8,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canSclBatMinus%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canSclBatMinus.Print(canName);
	 canSclBatMinus.Clear();
	 delete mg;
      }
   }


   //Next the Scaler Plots -- Trig L1
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canSclTrigL1("canSclTrigL1","canSclTrigL1");
      TGraph *grSclTrigL1[12]={0};
      for(int i=0;i<12;i++) {
	 grSclTrigL1[i]= fSclTrigL1Histos[i]->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Scalers (L1) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclTrigL1,grSclTrigL1,12,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canSclTrigL1%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canSclTrigL1.Print(canName);
	 canSclTrigL1.Clear();
	 delete mg;
      }
   }

   //Next the Global Scaler, bit of overkill to make a multigraph for one thing, but hey ho there we go
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canSclGlobal("canSclGlobal","canSclGlobal");
      TGraph *grSclGlobal[1]={0};
      for(int i=0;i<1;i++) {
	 grSclGlobal[i]= fSclGlobalHisto->getTimeGraph(plotTime);
      }
      sprintf(plotTitle,"Scalers (Global) for %s (Last: %s)",AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclGlobal,grSclGlobal,1,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/hk/canSclGlobal%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canSclGlobal.Print(canName);
	 canSclGlobal.Clear();
	 delete mg;
      }
   }
  

   //Next the DAC Plots
   for(int timeInd = (int) AraPlotTime::kFullTime;
       timeInd<(int)AraPlotTime::kNoTime;timeInd++) {
      AraPlotTime::AraPlotTime_t plotTime=AraPlotTime::AraPlotTime_t(timeInd);
      TCanvas canDac("canDac","canDac",600,800);
      canDac.Divide(2,3);
      TGraph *grDac[6][4]={{0}};
      TMultiGraph *mg[6];
      Int_t doPlot=0;
      for(int i=0;i<6;i++) {
	 canDac.cd(i+1);
	 for(int j=0;j<4;j++) {
	    grDac[i][j]= fDacHistos[i][j]->getTimeGraph(plotTime);
	 }
	 sprintf(plotTitle,"DAC (%c) for %s (Last: %s)",AraHkData::getDacLetter(i),AraPlotTime::getTimeTitleString(plotTime),clockTime.AsString("sl"));      
	 //This is dodgy should think about whether to go home
	 mg[i] =AraPlotUtils::plotMultigraph((TCanvas*)gPad,grDac[i],4,plotTitle,"Time","Scaler (units)",1);
	 if(mg[i]) doPlot=1;
      }
    
      if(doPlot) {	
	 sprintf(canName,"%s/hk/canDac%s.png",fPlotDir,AraPlotTime::getTimeString(plotTime));
	 unlink(canName);
	 canDac.Print(canName);
	 canDac.Clear();
      }
      for(int i=0;i<6;i++) {
	 if(mg[i]) delete mg[i];
      }
   }



}

void AraHkPlotter::makeLatestRunPlots()
{
   fHistoFile->cd();
   char plotTitle[180];
   char canName[FILENAME_MAX];
   char dirName[FILENAME_MAX];
   sprintf(dirName,"%s/hk/archive/run%d",fPlotDir,fCurrentRun);
   gSystem->mkdir(dirName,kTRUE);
  
  

   //First up lets do the temperature plots
   {
    
      TCanvas canTemp("canTemp","canTemp");
      TGraph *grTemp[8]={0};
      for(int i=0;i<8;i++) {
	 grTemp[i]= fTempHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Temperatures for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canTemp,grTemp,8,plotTitle,"Time","Temp (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canTemp.png",dirName);
	 unlink(canName);
	 canTemp.Print(canName);
	 canTemp.Clear();
	 delete mg;
      }
   }


   //Next the RF Power Plots --Discones
   {
    
      TCanvas canRfpDiscone("canRfpDiscone","canRfpDiscone");
      TGraph *grRfpDiscone[8]={0};
      for(int i=0;i<8;i++) {
	 grRfpDiscone[i]= fRfpDisconeHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"RF Power (Discones) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRfpDiscone,grRfpDiscone,8,plotTitle,"Time","RF Power (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canRfpDiscone.png",dirName);
	 unlink(canName);
	 canRfpDiscone.Print(canName);
	 canRfpDiscone.Clear();
	 delete mg;
      }
   }


   //Next the RF Power Plots --Batwings
   {
    
      TCanvas canRfpBatwing("canRfpBatwing","canRfpBatwing");
      TGraph *grRfpBatwing[8]={0};
      for(int i=0;i<8;i++) {
	 grRfpBatwing[i]= fRfpBatwingHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"RF Power (Batwings) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canRfpBatwing,grRfpBatwing,8,plotTitle,"Time","RF Power (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canRfpBatwing.png",dirName);
	 unlink(canName);
	 canRfpBatwing.Print(canName);
	 canRfpBatwing.Clear();
	 delete mg;
      }
   }



   //Next the Scaler Plots -- Discones
   {
    
      TCanvas canSclDiscone("canSclDiscone","canSclDiscone");
      TGraph *grSclDiscone[8]={0};
      for(int i=0;i<8;i++) {
	 grSclDiscone[i]= fSclDisconeHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Scalers (Discones) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclDiscone,grSclDiscone,8,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canSclDiscone.png",dirName);
	 unlink(canName);
	 canSclDiscone.Print(canName);
	 canSclDiscone.Clear();
	 delete mg;
      }
   }


   //Next the Scaler Plots -- Bat+
   {
    
      TCanvas canSclBatPlus("canSclBatPlus","canSclBatPlus");
      TGraph *grSclBatPlus[8]={0};
      for(int i=0;i<8;i++) {
	 grSclBatPlus[i]= fSclBatPlusHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Scalers (Bat+) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclBatPlus,grSclBatPlus,8,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canSclBatPlus.png",dirName);
	 unlink(canName);
	 canSclBatPlus.Print(canName);
	 canSclBatPlus.Clear();
	 delete mg;
      }
   }


   //Next the Scaler Plots -- Bat-
   {
    
      TCanvas canSclBatMinus("canSclBatMinus","canSclBatMinus");
      TGraph *grSclBatMinus[8]={0};
      for(int i=0;i<8;i++) {
	 grSclBatMinus[i]= fSclBatMinusHistos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Scalers (Bat-) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclBatMinus,grSclBatMinus,8,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canSclBatMinus.png",dirName);
	 unlink(canName);
	 canSclBatMinus.Print(canName);
	 canSclBatMinus.Clear();
	 delete mg;
      }
   }


   //Next the Scaler Plots -- Trig L1
   {
    
      TCanvas canSclTrigL1("canSclTrigL1","canSclTrigL1");
      TGraph *grSclTrigL1[12]={0};
      for(int i=0;i<12;i++) {
	 grSclTrigL1[i]= fSclTrigL1Histos[i]->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Scalers (L1) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclTrigL1,grSclTrigL1,12,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canSclTrigL1.png",dirName);
	 unlink(canName);
	 canSclTrigL1.Print(canName);
	 canSclTrigL1.Clear();
	 delete mg;
      }
   }

   //Next the Global Scaler, bit of overkill to make a multigraph for one thing, but hey ho there we go
   {
    
      TCanvas canSclGlobal("canSclGlobal","canSclGlobal");
      TGraph *grSclGlobal[1]={0};
      for(int i=0;i<1;i++) {
	 grSclGlobal[i]= fSclGlobalHisto->getTimeGraph(fEarliestTime,fLatestTime);
      }
      sprintf(plotTitle,"Scalers (Global) for  Run %d",fCurrentRun);
    
      TMultiGraph *mg =AraPlotUtils::plotMultigraph(&canSclGlobal,grSclGlobal,1,plotTitle,"Time","Scaler (units)",1);
      if(mg) {	
	 sprintf(canName,"%s/canSclGlobal.png",dirName);
	 unlink(canName);
	 canSclGlobal.Print(canName);
	 canSclGlobal.Clear();
	 delete mg;
      }
   }
  

   //Next the DAC Plots
   {
    
      TCanvas canDac("canDac","canDac",600,800);
      canDac.Divide(2,3);
      TGraph *grDac[6][4]={{0}};
      TMultiGraph *mg[6];
      Int_t doPlot=0;
      for(int i=0;i<6;i++) {
	 canDac.cd(i+1);
	 for(int j=0;j<4;j++) {
	    grDac[i][j]= fDacHistos[i][j]->getTimeGraph(fEarliestTime,fLatestTime);
	 }
	 sprintf(plotTitle,"DAC (%c) for Run %d",AraHkData::getDacLetter(i),fCurrentRun);      
	 //This is dodgy should think about whether to go home
	 mg[i] =AraPlotUtils::plotMultigraph((TCanvas*)gPad,grDac[i],4,plotTitle,"Time","Scaler (units)",1);
	 if(mg[i]) doPlot=1;
      }
    
      if(doPlot) {	
	 sprintf(canName,"%s/canDac.png",dirName);
	 unlink(canName);
	 canDac.Print(canName);
	 canDac.Clear();
      }
      for(int i=0;i<6;i++) {
	 if(mg[i]) delete mg[i];
      }
   }



}
