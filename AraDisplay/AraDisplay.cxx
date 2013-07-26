//////////////////////////////////////////////////////////////////////////////
/////  AraDisplay.cxx       Ara Display                                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The main Ara Display class displaying data                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
/*! \mainpage Ara ROOT v3.12 Software
 *
 * \section intro_sec Introduction
 *
 * Here is some Doxygen generated documentation for the ARA ROOT v3.12  software which is based on the ANITA software. 
 *
 * \section prereq_sec Prerequisites
 *
 * -# <A HREF="http://root.cern.ch">ROOT</A>
 * -# <A HREF="http://www.fftw.org/">FFTW 3 -- Fastest Fourier Transform in the West</a>
 * -# <A HREF="http://www.hep.ucl.ac.uk/uhen/anita/libRootFftwWrapper">libRootFftwWrapper -- a ROOT wrapper for FFTW 3</a>
 * -# <A HREF="http://www.gnu.org/software/gsl/">GNU Scientific Library -- library used by ROOT's MathMore library</A>
 * -# <A HREF="http://www.sqlite.org/">Sqlite3 -- Library for simple SQL database management and transactions </A>
 * -# <A HREF="http://www.cmake.org/">CMAKE -- cross-platform, open-source build system. Generates Makefiles for the user's system </A>

 * \section comp_sec Components
 *
 * -# ARA Event Library, ROOT classes for storing ARA data in ROOT files
 * -# ARA Display, the display program based on the Marvellous ANITA Graphical Interface and Classy Display (Magic Display).
 * -# ARA Web Plotter, the library and code which produces the dynamically updating webpages
 * -# Utilities, programs for converting raw ARA data into ROOT-ified versions
 * -# ARA Vertex - This is the vertex reconstuction code implemented in the production of L2 data
 *    NB this does not have Doxygen documentation
 *

 * \section bin_sec Executables
 *  - makeIcrrEventTree -- Program to convert the raw ARA TestBed / Station One data into a ROOT format
 *  - makeAtriEventTree -- Program to convert the raw ARA Atri electronics type station data into a ROOT format
 *  - AraWebRootFileMaker -- Program to make ROOT files for the webplotter
 *  - AraWebPlotter -- Program that reads these files and plots stuff
 *  - exampleLoop -- An example of analysis code that illustrates to the user how to load AraRoot files, loop through them, create "Useful" objects and perform some sort of analysis
 *  - exampleLoopL2 -- An example of analysis code that illustrates to the user how to process L0 or L1 (RawEvent or UsefulEvent) ROOT files and produce L2 files

 * \section script_sec Scripts
 * - runAraTestBedEventFileMaker.sh -- Script for calling makeAraEventTree to create TestBed root files
 * - runAraOneEventFileMaker.sh -- Script for calling makeAraEventTree to create Station One root files
 * - runAtriEventFileMaker.sh -- Script for calling makeAraEventTree to create ATRI station root files
 * - runAtriEventFileMakerForcedStationId.sh -- Script for calling makeAraEventTree to create ATRI station root files, but forcing a particular stationId into the event objects






 * \section install_sec Installation
 * The user must first specify the following two environmental variables:
 * <PRE>ARA_UTIL_INSTALL_DIR = {The install destination choosen by the user}</PRE>
 * <PRE>ARA_ROOT_DIR = {Location of the source code of AraRoot}</PRE>
 * Once these are set correctly the user needs to run
 * Do
 * <PRE>bash INSTALL.sh MODE </PRE> in the directory of the source code (i.e. in ARA_ROOT_DIR) - cmake will take care of the rest.
 *
 * MODE should be one of the following:
 * - 0 - re-build bins / libs that have been modified
 * - 1 - re-build all
 * - 99 - re-build debugging mode - this will produce more verbose output from the build process


 * \section rootfile_sec Making ROOT files
 * The makeAtriEventTree and makeIcrrEventTree executables are used to convert raw ARA data into a ROOT format. The easiest way to do this is through the runAraOneEventFileMaker.sh, runTestBedEventFileMaker.sh or runAtriEventFileMaker.sh scripts. The user must edit the following two variables in the scripts:
 * <PRE>RAW_BASE_DIR={base directory in which run_XXXXXX directories are kept}</PRE>
 * <PRE>ROOT_BASE_DIR={base directiry in which to write output root files}</PRE>
 * 



 *
 *\section display_sec Running AraDisplay
 * The easiest way to get AraDisplay running is to use the runAraDisplay.C macro (it is in AraDisplay/macros). You can either edit the macro to point to a ROOT event file (created as above) or you can pass the ROOT file name from the command line as in <PRE>root 'macros/runAraDisplay.C("/Users/rjn/ara/data/root/event_200MHz_DISC01.root")'</PRE> note the use of signal and double quotations so that the shell passes the argument to ROOT correctly.
 *\section installation_issues Installation Issues
 * This section will detail know issues with the installation of the software and is intended to provide a reference for anyone installing the software
 *
 * 

 * \section analysis_sec Adding Analysis Code to CMake Build List
 * Create your .cxx file in analysis and edit the analysis/CMakeLists.txt file. You will need to copy the lines that involved "exampleLoop", replacing it with the name of your executable

 * You should then be able to run INSTALL.sh 0 which will notice that there is something that it hasn't built yet and do the (hopefully) right thing and build it for you.

 * The executable will be created in two loactions:

 * ${ARA_ROOT_DIR}/build/analysis
 * and
 * ${ARA_UTIL_INSTALL_DIR}/bin

 *\section UW_Systems UW Systems

 *There is a common installation of AraRoot on the UW systems, an example analysis area and setup scripts for users
 *The installation files are in /home/jdavies/repositories/install/AraRoot/branches/3.12/
 *The setup scripts are in /home/jdavies/repositories/setupScripts
 *The example analysis area is in /home/jdavies/analysis/AraRoot/branches/3.12\n\n
 *
 *Steps to build your own analysis \n\n
 *-Copy the analysis area to your own user area
 *<PRE> cp /home/jdavies/analysis/AraRoot/branches/3.12 ~/analysis/AraRoot/branches/3.12</PRE>\n
 *-Copy accross the AraRoot 3.12 setup script
 *<PRE> cp /home/jdavies/repositories/setupScripts/AraRoot_v3.12.sh ~/setupScripts/</PRE>\n
 *-Edit the CMakeLists.txt file in ~/analysis/AraRoot/branches/3.12
 *-Ammend the lines building exampleLoop to your own analysis code name\n
 *<PRE> add_executable(exampleLoop exampleLoop.cxx)</PRE>
 *<PRE> target_link_libraries(exampleLoop ${ARAEVENT_LIBRARIES} ${ROOT_LIBRARIES} ${ZLIB_LIBRARIES})</PRE>\n
 *-Source the setup script
 *<PRE> source ~/setupScripts/AraRoot_v3.12.sh</PRE>\n
 *-Build the binary using the INSTALL.sh script
 *<PRE> bash INSTALL.sh < 0 for re-build or 1 for force rebuild></PRE>
 *-The binary will be in /analysis/AraRoot/branches/3.12/bin

 *\section trouble_sec Troubleshooting

 * There are a myriad of ways that one can run into problems with the software
 * Path problems -- the bane of poorly organised code. By far the easiest way to use the ARA root code is to just set the ARA_UTIL_INSTALL_DIR and have it point to the location you want to install all the packages. If things are set up correctly you will end up with 
 * - ARA_UTIL_INSTALL_DIR/include 
 * - ARA_UTIL_INSTALL_DIR/lib
 * - ARA_UTIL_INSTALL_DIR/bin
 * - ARA_UTIL_INSTALL_DIR/scripts 
 * - ARA_UTIL_INSTALL_DIR/share/araCalib  
 * all populated with essential headers, libraries and calibration constants. 

 * Most problems are fixed by running INSTALL.sh 1 to re-build AraRoot from scratch. 
 * If this doesn't fix things try removing everything from the ARA_UTIL_INSTALL_DIR, re-installating libRootFftwWrapper and then re-running INSTALL.sh with either 1 or 99.

 * If problems persist please contact <A HREF="/~jdavies/">Jonathan Davies</a>



 *
 *
 *
 *
 *
 *
 *
 */



//System includes
#include <fstream>
#include <iostream>

//Magic Display Includes
#include "AraDisplay.h"
#include "AraWaveformGraph.h"
#include "AraAtriCanvasMaker.h"
#include "AraIcrrCanvasMaker.h"
#include "AraControlPanel.h"
#include "AraGeomTool.h"

//Event Reader Includes
#include "UsefulIcrrStationEvent.h"
#include "RawIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "RawAtriStationEvent.h"

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
  fIcrrEventCanMaker=0;
  fAtriEventCanMaker=0;
  fCurrentRun=0;
  fApplyEventCut=0;
  fEventCutListEntry=-1;
  fEventTreeIndexEntry=-1;
  fEventEntry=0;
  fEventFile=0;
  fIcrrUsefulEventPtr=0;
  fIcrrRawEventPtr=0;
  fAtriUsefulEventPtr=0;
  fAtriRawEventPtr=0;
  fIcrrData=0;


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
 

  
  if(fIcrrData)
    fIcrrEventCanMaker=new AraIcrrCanvasMaker(this->fCalType);
  else
    fAtriEventCanMaker = new AraAtriCanvasMaker(this->fCalType);
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
  
  
  if(fIcrrData) {
    if(fIcrrUsefulEventPtr)
    delete fIcrrUsefulEventPtr;
    fIcrrUsefulEventPtr = new UsefulIcrrStationEvent(fIcrrRawEventPtr,fCalType);
  }
  else {
    if(fAtriUsefulEventPtr)
    delete fAtriUsefulEventPtr;
    fAtriUsefulEventPtr = new UsefulAtriStationEvent(fAtriRawEventPtr,fCalType);
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
    std::cerr << "Couldn't open: " << eventName << "\n";
    return -1;
  }
  //JPD - Use the RawStationEvent base class to interogate the tree
  //      and decide if this is a Icrr or Atri type station

  fEventTree->ResetBranchAddresses();
  fEventTree->SetBranchAddress("event",&fRawStationEventPtr);  
  fEventTree->GetEntry(1);
  //std::cout << "fRawStationEventPtr->stationId: " << (int)fRawStationEventPtr->stationId << "\n";
  fIcrrData=AraGeomTool::isIcrrStation(fRawStationEventPtr->stationId);

  fEventTree->ResetBranchAddresses();
  if(fIcrrData) 
    fEventTree->SetBranchAddress("event",&fIcrrRawEventPtr);  
  else
    fEventTree->SetBranchAddress("event",&fAtriRawEventPtr);  

  fEventTree->SetBranchAddress("run",&fCurrentRun);  
  fEventEntry=0;

  fEventTree->BuildIndex("event.head.eventNumber");
  fEventIndex = (TTreeIndex*) fEventTree->GetTreeIndex();

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
   if(fIcrrData) {
     fIcrrEventCanMaker->getEventInfoCanvas(fIcrrUsefulEventPtr,fAraEventInfoPad,fCurrentRun);   
     fIcrrEventCanMaker->setWaveformFormat(fWaveformFormat);
     fIcrrEventCanMaker->setCanvasLayout(fCanvasLayout);
     fIcrrEventCanMaker->getEventViewerCanvas(fIcrrUsefulEventPtr,fAraMainPad);      
   }
   else {
     fAtriEventCanMaker->getEventInfoCanvas(fAtriUsefulEventPtr,fAraEventInfoPad,fCurrentRun);   
     fAtriEventCanMaker->setWaveformFormat(fWaveformFormat);
     fAtriEventCanMaker->setCanvasLayout(fCanvasLayout);
     fAtriEventCanMaker->getEventViewerCanvas(fAtriUsefulEventPtr,fAraMainPad);      
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
      if(fIcrrData)
	fIcrrEventCanMaker->fNewEvent=1;
      else 
	fAtriEventCanMaker->fNewEvent=1;
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
    if(fIcrrData)
      fIcrrEventCanMaker->fNewEvent=1;
    else
      fAtriEventCanMaker->fNewEvent=1;
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
      if(fIcrrData)
	fIcrrEventCanMaker->fNewEvent=1;
      else
	fAtriEventCanMaker->fNewEvent=1;
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
    if(fIcrrData)
      fIcrrEventCanMaker->fNewEvent=1;
    else
      fAtriEventCanMaker->fNewEvent=1;
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
      if(fIcrrData)
	fIcrrEventCanMaker->fNewEvent=1;
      else
	fAtriEventCanMaker->fNewEvent=1;
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
    if(fIcrrData)
      fIcrrEventCanMaker->fNewEvent=1;
    else
      fAtriEventCanMaker->fNewEvent=1;
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
      if(fIcrrData)
	fIcrrEventCanMaker->fNewEvent=1;
      else
	fAtriEventCanMaker->fNewEvent=1;
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
   if(fIcrrData)
     fIcrrEventCanMaker->fNewEvent=1;
   else
     fAtriEventCanMaker->fNewEvent=1;
   if(retVal==0) {
     refreshEventDisplay(); 
   }  
   return retVal;  
  }
}


int AraDisplay::displayThisEvent(UInt_t eventNumber)
{
  cout << "displayThisEvent: " << eventNumber  <<endl;  
  
  if(!fIcrrEventCanMaker && !fAtriEventCanMaker) startEventDisplay();
  

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
   if(fIcrrData) {
     butReset = new TButton("Reset Avg","AraIcrrCanvasMaker::Instance()->resetAverage();",0.85,0.975,0.90,1);
   }
   else {
     butReset = new TButton("Reset Avg","AraAtriCanvasMaker::Instance()->resetAverage();",0.85,0.975,0.90,1);

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
   fElecViewButton->SetFillColor(kGray+3);
   fElecViewButton->Draw();
   fRFViewButton = new TButton("RF Chan#","AraDisplay::Instance()->setCanvasLayout(AraDisplayCanvasLayoutOption::kRFChanView); AraDisplay::Instance()->refreshEventDisplay();",0,0.95,0.1,0.975);
   fRFViewButton->SetTextSize(0.5);
   fRFViewButton->SetFillColor(kGray);
   fRFViewButton->Draw();
   fAntViewButton = new TButton("Antenna","AraDisplay::Instance()->setCanvasLayout(AraDisplayCanvasLayoutOption::kAntennaView); AraDisplay::Instance()->refreshEventDisplay();",0,0.925,0.1,0.95);
   fAntViewButton->SetTextSize(0.5);
   fAntViewButton->SetFillColor(kGray);
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
  if(fIcrrData) {
    if(fIcrrUsefulEventPtr) return fIcrrUsefulEventPtr->head.eventNumber; 
  }
  else {
    if(fAtriUsefulEventPtr) return fAtriUsefulEventPtr->eventNumber;
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

