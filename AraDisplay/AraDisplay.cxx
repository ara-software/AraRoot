//////////////////////////////////////////////////////////////////////////////
/////  AraDisplay.cxx       Ara Display                                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The main Ara Display class displaying data                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
/*! \mainpage Ara ROOT based Test Bed Software
 *
 * \section intro_sec Introduction
 *
 * Here is some Doxygen generated documentation for the ARA ROOT based Test Bed software which is baed on the ANITA software. 
 *
 * \section prereq_sec Prerequisites
 *
 * -# <A HREF="http://root.cern.ch">ROOT</A>
 * -# <A HREF="http://www.fftw.org/">FFTW 3 -- Fastest Fourier Transform in the West</a>
 * -# <A HREF="http://www.hep.ucl.ac.uk/uhen/anita/libRootFftwWrapper">libRootFftwWrapper -- a ROOT wrapper for FFTW 3</a>
 * 
 * \section comp_sec Components
 *
 * -# ARA Event Library, ROOT classes for storing ARA data in ROOT files
 * -# ARA Display, the display program based on the Marvellous ANITA Graphical Interface and Classy Display (Magic Display).
 * -# ARA Web Plotter, the library and code which produces the dynamically updating webpages
 * -# Utilities, programs for converting raw ARA data into ROOT-ified versions
 *
 * \section install_sec Installation
 * -# Ensure all the prerequisites are installed and in LD_LIBRARY_PATH or system paths.
 * -# Set the environmental variable ARA_UTIL_INSTALL_DIR to point to the location you want the software installed in (note that lib, bin, etc. sub directories will be made)
 * -# Do <PRE>make</PRE><PRE>make install</PRE>
 *
 * \section rootfile_sec Making ROOT files
 * The makeAraEventTree executable is usedto convert raw ARA Test Bed data into a ROOT format. The easiest way to do this is through the runAraEventFileMaker.sh script. The usage is <PRE>runAraEventFileMaker.sh (input dir) (outputfile)</PRE> where input dir is a directory that contains the ev_XXXXXXXX sub directories.
 *
 *\section display_sec Running AraDisplay
 * The easiest way to get AraDisplay running is to use the runAraDisplay.C macro (it is in AraDisplay/macros). You can either edit the macro to point to a ROOT event file (created as above) or you can pass the ROOT file name from the command line as in <PRE>root 'macros/runAraDisplay.C("/Users/rjn/ara/data/root/event_200MHz_DISC01.root")'</PRE> note the use of signal and double quotations so that the shell passes the argument to ROOT correctly.
 */



//System includes
#include <fstream>
#include <iostream>

//Magic Display Includes
#include "AraDisplay.h"
#include "AraWaveformGraph.h"
#include "AraCanvasMaker.h"
#include "AraTBCanvasMaker.h"
#include "AraControlPanel.h"

//Event Reader Includes
#include "UsefulAraTestBedStationEvent.h"
#include "RawAraTestBedStationEvent.h"
#include "UsefulAraOneStationEvent.h"
#include "RawAraOneStationEvent.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeIndex.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include "TEventList.h"
#include <TGClient.h>

using namespace std;

AraDisplay*  AraDisplay::fgInstance = 0;
//Leave these as global variables for now



void AraDisplay::zeroPointers() 
{
  fTBEventCanMaker=0;
  fEventCanMaker=0;
  fCurrentRun=0;
  fApplyEventCut=0;
  fEventCutListEntry=-1;
  fEventTreeIndexEntry=-1;
  fEventEntry=0;
  fEventFile=0;
  fTBUsefulEventPtr=0;
  fTBRawEventPtr=0;
  fUsefulEventPtr=0;
  fRawEventPtr=0;
  fTBData=1;


  fAraCanvas=0;
  fAraEventInfoPad=0;
  fAraMainPad=0;
  

  fgInstance=this;
  fEventTree=0;
  fEventPlaySleepMs=0;
  
  fCanvasLayout=AraDisplayCanvasLayoutOption::kElectronicsView;
  fWaveformFormat=AraDisplayFormatOption::kWaveform;

}

AraDisplay::AraDisplay()
{
  //Default constructor
  zeroPointers();
}

AraDisplay::~AraDisplay()
{
   //Default destructor
}


AraDisplay::AraDisplay(char *baseDir, unsigned int time, AraCalType::AraCalType_t calType)
{
  //Offline constructor
  zeroPointers();
  cout << "AraDisplay::AraDisplay(" << baseDir << " , " << time
       << ")" << endl;
  fCurrentFileTime=time;
  strncpy(fCurrentBaseDir,baseDir,179);
  fCalType=calType;
  
}

AraDisplay::AraDisplay(char *eventFile, AraCalType::AraCalType_t calType)
{
  //Offline constructor
  zeroPointers();
  cout << "AraDisplay::AraDisplay(" << eventFile
       << ")" << endl;
  fCurrentFileTime=-1;
  //  strncpy(fCurrentBaseDir,gSystem->DirName(eventFile),FILENAME_MAX);
  fCalType=calType;
  loadEventTree(eventFile);
  
}


//______________________________________________________________________________
AraDisplay*  AraDisplay::Instance()
{
   //static function
   return (fgInstance) ? (AraDisplay*) fgInstance : new AraDisplay();
}

void AraDisplay::startControlPanel() 
{
  fControlPanel=AraControlPanel::Instance();
  //  
  //  fControlPanel=new AraControlPanel(gClient->GetRoot(), 400, 200);
  //  fControlPanel->DrawControlPanel();
  //  fControlPanel->Draw();
}


void AraDisplay::startEventDisplay()
{
 
  if(fTBData)
    fTBEventCanMaker=new AraTBCanvasMaker(this->fCalType);
  else
    fEventCanMaker = new AraCanvasMaker(this->fCalType);
  int retVal=this->getEventEntry();
  if(retVal==0)
      this->refreshEventDisplay();   
}

int AraDisplay::getEventEntry()
{
  if(!fEventTree) {
    if(loadEventTree()<0) {
      std::cout << "Couldn't open event file\n";
      return -1;
    }
  }
  if(fEventEntry<fEventTree->GetEntries())
    fEventTree->GetEntry(fEventEntry);
  else {
    std::cout << "No more entries in event tree " << fEventEntry << "\t" << fEventTree->GetEntries() << endl;
    return -1;
  }
  
  
  if(fTBData) {
    if(fTBUsefulEventPtr)
    delete fTBUsefulEventPtr;
    fTBUsefulEventPtr = new UsefulAraTestBedStationEvent(fTBRawEventPtr,fCalType);
  }
  else {
    if(fUsefulEventPtr)
    delete fUsefulEventPtr;
    fUsefulEventPtr = new UsefulAraOneStationEvent(fRawEventPtr,fCalType);
  }
  
  //Need to make configurable at some point
  //This will also need to be modifed to make realEvent accessible outside here
   return 0;
}


void AraDisplay::closeCurrentFile()
{
  
  if(fEventFile)
    fEventFile->Close();

  fEventFile=0;
  fEventTree=0;
}


int AraDisplay::loadEventTree(char *eventFile)
{     
  //Try and open file or chain
  if(!fEventTree) 
    fEventTree = new TChain("eventTree");
  char eventName[FILENAME_MAX];
  if(!eventFile) {
    //Step one try calEventFile
    sprintf(eventName,"%s/event_%d.root",fCurrentBaseDir,fCurrentFileTime);   
    std::cout << eventName << "\n";
    fEventTree->Add(eventName);
    //    std::cout << fEventTree->GetEntries() << "\n";
    if(fEventTree->GetEntries()>0) {
      for(int extra=1;extra<100;extra++) {
	sprintf(eventName,"%s/event_%d_%d.root",fCurrentBaseDir,fCurrentFileTime,extra); 
	TFile *fpTest = TFile::Open(eventName);
	if(!fpTest) 
	  break;
	else {
	  delete fpTest;
	  fEventTree->Add(eventName);
	}
      }
    }
  }
  else {
    fEventTree->Add(eventFile);
    strncpy(eventName,eventFile,FILENAME_MAX);
  }
  
  if(fEventTree->GetEntries()<1) {
    cout << "Couldn't open: " << eventName << "\n";
    return -1;
  }
  //  std::cout << "Here\n";
  if(fTBData) 
    fEventTree->SetBranchAddress("event",&fTBRawEventPtr);  
  else
    fEventTree->SetBranchAddress("event",&fRawEventPtr);  
  fEventTree->SetBranchAddress("run",&fCurrentRun);  
  fEventEntry=0;
  return 0;
}

void AraDisplay::refreshEventDisplay()
{
   if(!fAraCanvas) {
      fAraCanvas = new TCanvas("canAra","canAra",1200,800);
      fAraCanvas->cd();
      drawEventButtons();
   }
   if(!fAraMainPad) {
      fAraCanvas->cd();
      fAraMainPad= new TPad("canAraMain","canAraMain",0,0,1,0.9);
      fAraMainPad->Draw();
      fAraCanvas->Update();
   }
   if(!fAraEventInfoPad) {
      fAraCanvas->cd();
      fAraEventInfoPad= new TPad("canAraEventInfo","canAraEventInfo",0.2,0.91,0.8,0.99);
      fAraEventInfoPad->Draw();
      fAraCanvas->Update();
   }  
   //   fAraMainPad->Clear();

           
   //This will need to change  
   if(fTBData) {
     fTBEventCanMaker->getEventInfoCanvas(fTBUsefulEventPtr,fAraEventInfoPad,fCurrentRun);   
     fTBEventCanMaker->setWaveformFormat(fWaveformFormat);
     fTBEventCanMaker->setCanvasLayout(fCanvasLayout);
     fTBEventCanMaker->getEventViewerCanvas(fTBUsefulEventPtr,fAraMainPad);      
   }
   else {
     fEventCanMaker->getEventInfoCanvas(fUsefulEventPtr,fAraEventInfoPad,fCurrentRun);   
     fEventCanMaker->setWaveformFormat(fWaveformFormat);
     fEventCanMaker->setCanvasLayout(fCanvasLayout);
     fEventCanMaker->getEventViewerCanvas(fUsefulEventPtr,fAraMainPad);      
   }
   fAraCanvas->Update();
}

void AraDisplay::applyCut(char *cutString)
{
  if(cutString==0)
    fApplyEventCut=0;
  
  if(!fEventTree) {
    if(loadEventTree()<0) {
      std::cout << "Couldn't open event file\n";
      return;
    }
  }

  TCanvas tempCan;
  tempCan.cd();
  fEventTree->Draw(">>elist1",cutString);
  fCutEventList = (TEventList*)gDirectory->Get("elist1");
  fApplyEventCut=1;
  fCutEventList->Print();
 

}

int AraDisplay::displayNextEvent()
{
  //  static Int_t fEventTreeIndexEntry=-1;
  //  static Int_t listNumber=-1;
  if(fApplyEventCut==1) {
    fEventCutListEntry++;
    if(fEventCutListEntry<fCutEventList->GetN()) {
      fEventEntry=fCutEventList->GetEntry(fEventCutListEntry);  
      int retVal=getEventEntry();
      if(fTBData)
	fTBEventCanMaker->fNewEvent=1;
      else 
	fEventCanMaker->fNewEvent=1;
      if(retVal==0) {
	refreshEventDisplay(); 
      }
      return retVal;
    }
    else {
      fEventCutListEntry=fCutEventList->GetN()-1;
      return -1;
    }      
  }
  else {
    fEventEntry++;
    int retVal=getEventEntry();
    if(fTBData)
      fTBEventCanMaker->fNewEvent=1;
    else
      fEventCanMaker->fNewEvent=1;
    if(retVal==0) {
      refreshEventDisplay(); 
    }
    else {
      fEventEntry--;
    }
    return retVal;  
  }
}


int AraDisplay::displayFirstEvent()
{

  if(fApplyEventCut==1) {    
    fEventCutListEntry=0;
    if(fEventCutListEntry<fCutEventList->GetN()) {
      fEventEntry=fCutEventList->GetEntry(fEventCutListEntry);  
      int retVal=getEventEntry();
      if(fTBData)
	fTBEventCanMaker->fNewEvent=1;
      else
	fEventCanMaker->fNewEvent=1;
      if(retVal==0) {
	refreshEventDisplay(); 
      }
      return retVal;
    }
    else {
      fEventCutListEntry=0;
      return -1;
    }      
  }
  else  {    
    fEventEntry=0;
    int retVal=getEventEntry();
    if(fTBData)
      fTBEventCanMaker->fNewEvent=1;
    else
      fEventCanMaker->fNewEvent=1;
    if(retVal==0) {
      refreshEventDisplay(); 
    }
    else fEventEntry--;
    return retVal;  
  }
  return -1;
}


int AraDisplay::displayLastEvent()
{
  //  fEventTree->Refresh();
  
  Long64_t eventEnts=fEventTree->GetEntries();

  if(fApplyEventCut==1) {    
    fEventCutListEntry=fCutEventList->GetN()-1;
    if(fEventCutListEntry<fCutEventList->GetN() && fEventCutListEntry>=0) {
      fEventEntry=fCutEventList->GetEntry(fEventCutListEntry);  
      int retVal=getEventEntry();
      if(fTBData)
	fTBEventCanMaker->fNewEvent=1;
      else
	fEventCanMaker->fNewEvent=1;
      if(retVal==0) {
	refreshEventDisplay(); 
      }
      return retVal;
    }
    else {
      fEventCutListEntry=0;
      return -1;
    }      
  }
  else  {    
    fEventEntry=eventEnts-1;
    int retVal=getEventEntry();
    if(fTBData)
      fTBEventCanMaker->fNewEvent=1;
    else
      fEventCanMaker->fNewEvent=1;
    if(retVal==0) {
      refreshEventDisplay(); 
    }
    else fEventEntry--;
    return retVal;  
  }
  return -1;
}


int AraDisplay::displayPreviousEvent()
{
  //  static Int_t fEventTreeIndexEntry=-1;
  //  static Int_t fEventCutListEntry=-1;

  if(fApplyEventCut==1) {
    //    std::cout << fApplyEventCut << "\t" << fEventCutListEntry << "\t" << fCutEventList->GetN() << "\n";
    fEventCutListEntry--;
    if(fEventCutListEntry>=0 && fEventCutListEntry<fCutEventList->GetN()) {
      fEventEntry=fCutEventList->GetEntry(fEventCutListEntry);  
      int retVal=getEventEntry();
      if(fTBData)
	fTBEventCanMaker->fNewEvent=1;
      else
	fEventCanMaker->fNewEvent=1;
      if(retVal==0) {
	refreshEventDisplay(); 
      }
      else {
	fEventCutListEntry++;
      }
      return retVal;
    }
    else {
      fEventCutListEntry++;
      return -1;
    }      
  }
  else  {
    if(fEventEntry>0)
      fEventEntry--;
    else 
      return -1;
   int retVal=getEventEntry();
   if(fTBData)
     fTBEventCanMaker->fNewEvent=1;
   else
     fEventCanMaker->fNewEvent=1;
   if(retVal==0) {
     refreshEventDisplay(); 
   }  
   return retVal;  
  }
}


int AraDisplay::displayThisEvent(UInt_t eventNumber)
{
  cout << "displayThisEvent: " << eventNumber  <<endl;  
  
  if(!fTBEventCanMaker && !fEventCanMaker) startEventDisplay();
  

  if(eventNumber==0) {
    fEventEntry=0;
  }
  else {
    fEventEntry=fEventTree->GetEntryNumberWithIndex(eventNumber);
    if(fEventEntry<0) 
      return -1;      
  }
  cout << "fEventEntry: " << fEventEntry << endl;
  int retVal=getEventEntry();
  if(retVal==0) 
    refreshEventDisplay(); 
  else {
    cout << "retVal: " << retVal << endl;
  }
  return retVal;  
}

void AraDisplay::drawEventButtons() {
   TButton *butNext = new TButton("Next ","AraDisplay::Instance()->displayNextEvent();",0.95,0.975,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   TButton *butPrev = new TButton("Prev.","AraDisplay::Instance()->displayPreviousEvent();",0.95,0.95,1,0.975);
   butPrev->SetTextSize(0.5);
   butPrev->SetFillColor(kBlue-10);
   butPrev->Draw();
   TButton *butFirst = new TButton("First ","AraDisplay::Instance()->displayFirstEvent();",0.95,0.925,1,0.95);
   butFirst->SetTextSize(0.5);
   butFirst->SetFillColor(kOrange+10);
   butFirst->Draw();
   TButton *butLast = new TButton("Last.","AraDisplay::Instance()->displayLastEvent();",0.95,0.90,1,0.925);
   butLast->SetTextSize(0.5);
   butLast->SetFillColor(kViolet-10);
   butLast->Draw();
   

   TButton *butPlay = new TButton("Play","AraDisplay::Instance()->startEventPlaying();",0.9,0.97,0.95,1);
   butPlay->SetTextSize(0.5);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();
   TButton *butPlayRev = new TButton("Rev ","AraDisplay::Instance()->startEventPlayingReverse();",0.9,0.94,0.95,0.97);
   butPlayRev->SetTextSize(0.5);
   butPlayRev->SetFillColor(kBlue-10);
   butPlayRev->Draw();
   TButton *butStop = new TButton("Stop","AraDisplay::Instance()->stopEventPlaying();",0.90,0.90,0.95,0.94);
   butStop->SetTextSize(0.5);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();
   TButton *butReset; 
   if(fTBData) {
     butReset = new TButton("Reset Avg","AraTBCanvasMaker::Instance()->resetAverage();",0.85,0.975,0.90,1);
   }
   else {
     butReset = new TButton("Reset Avg","AraCanvasMaker::Instance()->resetAverage();",0.85,0.975,0.90,1);

   }
   butReset->SetTextSize(0.5);
   butReset->SetFillColor(kViolet-10);
   butReset->Draw();;
   TButton *butGoto = new TButton("Go to Event","AraDisplay::Instance()->startControlPanel();",0.85,0.95,0.9,0.975);
   butGoto->SetTextSize(0.5);
   butGoto->SetFillColor(kOrange);
   butGoto->Draw();
   //   fTimeEntryButton= new TButton("Event#","AraDisplay::Instance()->toggleTimeEventOrdering();",0.85,0.925,0.9,0.95);
   //   fTimeEntryButton->SetTextSize(0.5);
   //   fTimeEntryButton->SetFillColor(kGray);
   //   fTimeEntryButton->Draw();


   fElecViewButton = new TButton("Elec. View","AraDisplay::Instance()->setCanvasLayout(AraDisplayCanvasLayoutOption::kElectronicsView); AraDisplay::Instance()->refreshEventDisplay();",0,0.975,0.1,1);
   fElecViewButton->SetTextSize(0.5);
   fElecViewButton->SetFillColor(kGray);
   fElecViewButton->Draw();
   fRFViewButton = new TButton("RF Chan#","AraDisplay::Instance()->setCanvasLayout(AraDisplayCanvasLayoutOption::kRFChanView); AraDisplay::Instance()->refreshEventDisplay();",0,0.95,0.1,0.975);
   fRFViewButton->SetTextSize(0.5);
   fRFViewButton->SetFillColor(kGray);
   fRFViewButton->Draw();
   fAntViewButton = new TButton("Antenna","AraDisplay::Instance()->setCanvasLayout(AraDisplayCanvasLayoutOption::kAntennaView); AraDisplay::Instance()->refreshEventDisplay();",0,0.925,0.1,0.95);
   fAntViewButton->SetTextSize(0.5);
   fAntViewButton->SetFillColor(kGray+3);
   fAntViewButton->Draw();
   fMapViewButton = new TButton("Int. Map","AraDisplay::Instance()->setCanvasLayout(AraDisplayCanvasLayoutOption::kIntMapView); AraDisplay::Instance()->refreshEventDisplay();",0,0.90,0.1,0.925);
   fMapViewButton->SetTextSize(0.5);
   fMapViewButton->SetFillColor(kGray);
   fMapViewButton->Draw();

   //NEW BUTTONS
   fWaveformButton = new TButton("Waveform","AraDisplay::Instance()->setWaveformFormat(AraDisplayFormatOption::kWaveform); AraDisplay::Instance()->refreshEventDisplay();",0.1,0.975,0.2,1);
   fWaveformButton->SetTextSize(0.4);
   fWaveformButton->SetFillColor(kGray+3);
   fWaveformButton->Draw();
   fPowerButton = new TButton("FFT","AraDisplay::Instance()->setWaveformFormat(AraDisplayFormatOption::kFFT); AraDisplay::Instance()->refreshEventDisplay();",0.1,0.95,0.2,0.975);
   fPowerButton->SetTextSize(0.4);
   fPowerButton->SetFillColor(kGray);
   fPowerButton->Draw();
   fHilbertButton = new TButton("Hilbert","AraDisplay::Instance()->setWaveformFormat(AraDisplayFormatOption::kHilbertEnvelope); AraDisplay::Instance()->refreshEventDisplay();",0.1,0.925,0.2,0.95);
   fHilbertButton->SetTextSize(0.4);
   fHilbertButton->SetFillColor(kGray);
   fHilbertButton->Draw();
   fAverageFFTButton = new TButton("Average FFT","AraDisplay::Instance()->setWaveformFormat(AraDisplayFormatOption::kAveragedFFT); AraDisplay::Instance()->refreshEventDisplay();",0.1,0.9,0.2,0.925);
   fAverageFFTButton->SetTextSize(0.4);
   fAverageFFTButton->SetFillColor(kGray);
   fAverageFFTButton->Draw();

   
}

void AraDisplay::setCanvasLayout(AraDisplayCanvasLayoutOption::AraDisplayCanvasLayoutOption_t option)
{
   fCanvasLayout=option;
   switch(option) {
   case AraDisplayCanvasLayoutOption::kElectronicsView:
      fElecViewButton->SetFillColor(kGray+3);
      fRFViewButton->SetFillColor(kGray);
      fAntViewButton->SetFillColor(kGray);
      fMapViewButton->SetFillColor(kGray);
      break;
   case AraDisplayCanvasLayoutOption::kAntennaView:
      fElecViewButton->SetFillColor(kGray);
      fRFViewButton->SetFillColor(kGray);
      fAntViewButton->SetFillColor(kGray+3);
      fMapViewButton->SetFillColor(kGray);
      break;
   case AraDisplayCanvasLayoutOption::kRFChanView:
      fRFViewButton->SetFillColor(kGray+3);
      fElecViewButton->SetFillColor(kGray);
      fAntViewButton->SetFillColor(kGray);
      fMapViewButton->SetFillColor(kGray);
      break;
   case AraDisplayCanvasLayoutOption::kIntMapView:
      fRFViewButton->SetFillColor(kGray);
      fElecViewButton->SetFillColor(kGray);
      fAntViewButton->SetFillColor(kGray);
      fMapViewButton->SetFillColor(kGray+3);
      break;
   default:
      break;
   }   
   fElecViewButton->Modified();
   fRFViewButton->Modified();  
   fAntViewButton->Modified(); 
   fMapViewButton->Modified(); 
}



void AraDisplay::setWaveformFormat(AraDisplayFormatOption::AraDisplayFormatOption_t waveformOption)
{
  fWaveformFormat=waveformOption;
  if(waveformOption==AraDisplayFormatOption::kWaveform) {
      //Turn on fft view off     
      fWaveformButton->SetFillColor(kGray+3);
      fPowerButton->SetFillColor(kGray);
      fHilbertButton->SetFillColor(kGray);
      fAverageFFTButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
  }
  else if(waveformOption==AraDisplayFormatOption::kFFT) {
      //Turn fft view on
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray+3);
      fHilbertButton->SetFillColor(kGray);
      fAverageFFTButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
   }
  else if(waveformOption==AraDisplayFormatOption::kHilbertEnvelope) {
      //Turn fft view on
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray);
      fHilbertButton->SetFillColor(kGray+3);
      fAverageFFTButton->SetFillColor(kGray);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
   }
  else if(waveformOption==AraDisplayFormatOption::kAveragedFFT) {
      //Turn fft view on
      fWaveformButton->SetFillColor(kGray);
      fPowerButton->SetFillColor(kGray);
      fHilbertButton->SetFillColor(kGray);
      fAverageFFTButton->SetFillColor(kGray+3);
      fWaveformButton->Modified();
      fPowerButton->Modified();
      fHilbertButton->Modified();
      fAverageFFTButton->Modified();
   }
      
}


UInt_t AraDisplay::getCurrentEvent()
{
  if(fTBData) {
    if(fTBUsefulEventPtr) return fTBUsefulEventPtr->head.eventNumber; 
  }
  else {
    if(fUsefulEventPtr) return fUsefulEventPtr->eventNumber;
  }
  return 0;
}


void AraDisplay::startEventPlaying()
{

  fInEventPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInEventPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayNextEvent()==0);
  if(fInEventPlayMode) {
    Long64_t Entries=fEventTree->GetEntries();
    if(fEventTree->GetEntries()!=Entries) {
      std::cout << Entries <<  "\n";
      startEventPlaying();
    }
  }    
}



void AraDisplay::startEventPlayingReverse()
{
  fInEventPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInEventPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayPreviousEvent()==0);
}

void AraDisplay::stopEventPlaying() 
{
  fInEventPlayMode=0;
}

