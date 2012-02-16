//////////////////////////////////////////////////////////////////////////////
/////  AraEventCalibrator.h        Calibrator                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class for calibrating events                           /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraEventCalibrator.h"
#include "UsefulIcrrStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "AraGeomTool.h"
#include "TMath.h"
#include "TGraph.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <zlib.h>
#include <cstdlib>


Bool_t AraCalType::hasCableDelays(AraCalType::AraCalType_t calType)
{ 
  if(calType==kFirstCalibPlusCables || calType==kSecondCalibPlusCables)
    return kTRUE;
  return kFALSE;
}

Bool_t AraCalType::hasInterleaveCalib(AraCalType::AraCalType_t calType)
{ 
  if(calType==kFirstCalibPlusCables || calType==kSecondCalibPlusCables ||
     calType==kFirstCalib || calType==kSecondCalib)
    return kTRUE;
  return kFALSE;
}

Bool_t AraCalType::hasBinWidthCalib(AraCalType::AraCalType_t calType)
{ 
  if(calType>=kFirstCalib)
    return kTRUE;
  return kFALSE;
}


Bool_t AraCalType::hasClockAlignment(AraCalType::AraCalType_t calType)
{ 
  if(calType==kSecondCalibPlusCables ||
     calType==kSecondCalib)
    return kTRUE;
  return kFALSE;
}



Bool_t AraCalType::hasPedestalSubtraction(AraCalType::AraCalType_t calType)
{ 
  if(calType==kNoCalib) return kFALSE;
  return kTRUE;
}

Bool_t AraCalType::hasCommonMode(AraCalType::AraCalType_t calType)
{
  return kFALSE;
  if(calType==kNoCalib) return kFALSE;
  return kTRUE;
}



ClassImp(AraEventCalibrator);

AraEventCalibrator * AraEventCalibrator::fgInstance=0;


AraEventCalibrator::AraEventCalibrator() 
{
  for(int station=0;station<ICRR_NO_STATIONS;station++) gotPedFile[station]=0;
  fGotAtriPedFile=0;
  loadIcrrCalib();
  loadAtriCalib();
  //Default Constructor
}

AraEventCalibrator::~AraEventCalibrator() {
  //Default Destructor
}

//______________________________________________________________________________
AraEventCalibrator*  AraEventCalibrator::Instance()
{
  //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraEventCalibrator();
  return fgInstance;
}


void AraEventCalibrator::setPedFile(char fileName[], UInt_t stationId)
{
  strncpy(pedFile[stationId],fileName,FILENAME_MAX);
  fprintf(stdout, "AraEventCalibrator::setPedFile() setting ped %i to %s\n", stationId, pedFile[stationId]);
  gotPedFile[stationId]=1;
  loadIcrrPedestals();
}

void AraEventCalibrator::loadIcrrPedestals()
{
  //TestBed
  if(!gotPedFile[0]) {
    char *pedFileEnv = getenv( "ARA_PEDESTAL_FILE" );
    if ( pedFileEnv == NULL ) {
      char calibDir[FILENAME_MAX];
      char *calibEnv=getenv("ARA_CALIB_DIR");
      if(!calibEnv) {
	char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
	if(!utilEnv) {
	  sprintf(calibDir,"calib");
	  fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():TestBed: INFO - Pedestal file [from ./calib]");
	} else {
	  sprintf(calibDir,"%s/share/araCalib",utilEnv);
	  fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():TestBed: INFO - Pedestal file [from ARA_UTIL_INSTALL_DIR/share/calib]");
	}
      }
      else {
	strncpy(calibDir,calibEnv,FILENAME_MAX);
	fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():TestBed INFO - Pedestal file [from ARA_CALIB_DIR]");
      }
      sprintf(pedFile[0],"%s/ICRR/TestBed/peds_1294924296.869787.run001202.dat",calibDir);
      fprintf(stdout," = %s\n",pedFile[0]);
    } // end of IF-block for pedestal file not specified by environment variable
    else {
      strncpy(pedFile[0],pedFileEnv,FILENAME_MAX);
      fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():TestBed: INFO - Pedestal file [from ARA_PEDESTAL_FILE] = %s\n",pedFile[0]);
    } // end of IF-block for pedestal file specified by environment variable
  }

  //Station1
  if(!gotPedFile[1]) {
    char *pedFileEnv = getenv( "ARA_PEDESTAL_FILE" );
    if ( pedFileEnv == NULL ) {
      char calibDir[FILENAME_MAX];
      char *calibEnv=getenv("ARA_CALIB_DIR");
      if(!calibEnv) {
	char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
	if(!utilEnv) {
	  sprintf(calibDir,"calib");
	  fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():Station1: INFO - Pedestal file [from ./calib]");
	} else {
	  sprintf(calibDir,"%s/share/araCalib",utilEnv);
	  fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():Station1: INFO - Pedestal file [from ARA_UTIL_INSTALL_DIR/share/calib]");
	}
      }
      else {
	strncpy(calibDir,calibEnv,FILENAME_MAX);
	fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():Station1: INFO - Pedestal file [from ARA_CALIB_DIR]");
      }
      sprintf(pedFile[1],"%s/ICRR/Station1/peds_1326108401.602169.run003747.dat",calibDir);
      fprintf(stdout," = %s\n",pedFile[1]);
    } // end of IF-block for pedestal file not specified by environment variable
    else {
      strncpy(pedFile[1],pedFileEnv,FILENAME_MAX);
      fprintf(stdout,"AraEventCalibrator::loadIcrrPedestals():Station1: INFO - Pedestal file [from ARA_PEDESTAL_FILE] = %s\n",pedFile[1]);
    } // end of IF-block for pedestal file specified by environment variable
  }

  FullLabChipPedStruct_t peds;

  //TestBed
  gzFile inPedTB = gzopen(pedFile[0],"r");
  if( !inPedTB ){
    fprintf(stderr,"ERROR - Failed to open pedestal file for TestBed %s.\n",pedFile[0]);
    return;
  }

  int nRead = gzread(inPedTB,&peds,sizeof(FullLabChipPedStruct_t));
  if( nRead != sizeof(FullLabChipPedStruct_t)){
    int numErr;
    fprintf(stderr,"ERROR - Error reading pedestal file %s; %s\n",pedFile[0],gzerror(inPedTB,&numErr));
    gzclose(inPedTB);
    return;
  }

  int chip,chan,samp;
  for(chip=0;chip<LAB3_PER_ICRR;++chip) {
    for(chan=0;chan<CHANNELS_PER_LAB3;++chan) {
      int chanIndex = chip*CHANNELS_PER_LAB3+chan;
      for(samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
	pedestalData[0][chip][chan][samp]=peds.chan[chanIndex].pedMean[samp];
      }
    }
  }
  gzclose(inPedTB);
  gotPedFile[0]=1;

  //Station1
  gzFile inPedAra1 = gzopen(pedFile[1],"r");
  if( !inPedAra1 ){
    fprintf(stderr,"ERROR - Failed to open pedestal file for Station1 %s\n",pedFile[1]);
    return;
  }

  nRead = gzread(inPedAra1,&peds,sizeof(FullLabChipPedStruct_t));
  if( nRead != sizeof(FullLabChipPedStruct_t)){
    int numErr;
    fprintf(stderr,"ERROR - Error reading pedestal file %s; %s\n",pedFile[1],gzerror(inPedAra1,&numErr));
    gzclose(inPedAra1);
    return;
  }

  for(chip=0;chip<LAB3_PER_ICRR;++chip) {
    for(chan=0;chan<CHANNELS_PER_LAB3;++chan) {
      int chanIndex = chip*CHANNELS_PER_LAB3+chan;
      for(samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
	pedestalData[1][chip][chan][samp]=peds.chan[chanIndex].pedMean[samp];
      }
    }
  }
  gzclose(inPedAra1);
  gotPedFile[1]=1;

}

int AraEventCalibrator::doBinCalibration(UsefulIcrrStationEvent *theEvent, int chanIndex,int overrideRCO)
{
  int stationId=theEvent->stationId;

  int nChip=theEvent->chan[chanIndex].chanId/CHANNELS_PER_LAB3;
  int nChan=theEvent->chan[chanIndex].chanId%CHANNELS_PER_LAB3;
  int rco=overrideRCO;
  if(overrideRCO!=0 && overrideRCO!=1) 
    rco=theEvent->getRCO(chanIndex);
  int hbwrap = theEvent->chan[chanIndex].chipIdFlag&0x08;
  char hbextra = (theEvent->chan[chanIndex].chipIdFlag&0xf0)>>4;
  short hbstart=theEvent->chan[chanIndex].firstHitbus;
  short hbend=theEvent->chan[chanIndex].lastHitbus+hbextra;
    


  double calTime=0;
  for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp){
    rawadc[samp]=theEvent->chan[chanIndex].data[samp];
    if(theEvent->chan[chanIndex].data[samp]==0){
      calwv[samp]=0;
      pedsubadc[samp]=0;
    }else{
      pedsubadc[samp]=rawadc[samp]-pedestalData[stationId][nChip][nChan][samp];
      calwv[samp]=pedsubadc[samp]*ADCMV;
      if(calwv[samp]>SATURATION) calwv[samp]=SATURATION;
      if(calwv[samp]<-1*SATURATION) calwv[samp]=-1*SATURATION;
    }
  }
    

  if(nChan==0 || (overrideRCO==0 || overrideRCO==1)) {
    //Do the calibration for each chip
    calTime=0;
    int ir=0;
    if(hbwrap){ // Wrapped hitbus
      for(int samp=hbstart+1;samp<hbend;++samp) {
	tempTimeNums[ir++]=calTime;
	calTime+=binWidths[stationId][nChip][rco][samp];
      }
    }
    else{
      for(int samp=hbend+1;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
	tempTimeNums[ir++]=calTime;
	calTime+=binWidths[stationId][nChip][1-rco][samp];
      }
      //Now add epsilon
      calTime+=epsilonVals[stationId][nChip][rco];  //Need to check if this is rco or 1-rco
      for(int samp=0;samp<hbstart;++samp) {
	tempTimeNums[ir++]=calTime;
	calTime+=binWidths[stationId][nChip][rco][samp];
      }
    }
    for(int samp=ir;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
      tempTimeNums[samp]=calTime;
      calTime+=1;
    }    
    TMath::Sort(MAX_NUMBER_SAMPLES_LAB3,tempTimeNums,indexNums,kFALSE);
  }
  // ... and rotate
  int ir=0;
  int numValid=0;
  if(hbwrap){ // Wrapped hitbus
    for(int samp=hbstart+1;samp<hbend;++samp)
      v[ir++]=calwv[samp];
  }else{
    for(int samp=hbend+1;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) 
      v[ir++]=calwv[samp];
    for(int samp=0;samp<hbstart;++samp)
      v[ir++]=calwv[samp];
  }
  numValid=ir;
  // Fill in remaining bins with zeros
  for(int samp=ir;samp<MAX_NUMBER_SAMPLES_LAB3;++samp)
    v[samp]=0;
  
  //Now we just have to make sure the times are monotonically increasing
  for(int i=0;i<MAX_NUMBER_SAMPLES_LAB3;i++) {
    calTimeNums[i]=tempTimeNums[indexNums[i]];
    calVoltNums[i]=v[indexNums[i]];
    //      std::cout << i << "\t" << indexNums[i] << "\t" << calTimeNums[i] << "\t" << calVoltNums[i] << "\n";
  }
  return numValid;
}


void AraEventCalibrator::calibrateEvent(UsefulIcrrStationEvent *theEvent, AraCalType::AraCalType_t calType) 
{
  int stationId=theEvent->stationId;
  static int gotPeds=0;
  if(!gotPeds)  
    loadIcrrPedestals();
  gotPeds=1;
  if(AraCalType::hasBinWidthCalib(calType))
    theEvent->guessRCO(0); //Forces the calculation of the RCO phase from the clock


  // Calibrate waveforms
  for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp){
    sampNums[samp]=samp;
    timeNums[samp]=samp*NSPERSAMP;
  }
  
  for(int  chanIndex = 0; chanIndex < NUM_DIGITIZED_ICRR_CHANNELS; ++chanIndex ){
    int numValid=doBinCalibration(theEvent,chanIndex);



    //Now we stuff it back into the UsefulIcrrStationEvent object
         
    if(calType==AraCalType::kNoCalib) {
      theEvent->fNumPoints[chanIndex]=MAX_NUMBER_SAMPLES_LAB3;
      for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
	theEvent->fVolts[chanIndex][samp]=rawadc[samp];
	theEvent->fTimes[chanIndex][samp]=sampNums[samp];
      }
    }
    if(calType==AraCalType::kJustUnwrap || calType==AraCalType::kADC) {
      theEvent->fNumPoints[chanIndex]=numValid;
      for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
	theEvent->fTimes[chanIndex][samp]=sampNums[samp];
	if(samp<numValid) {
	  theEvent->fVolts[chanIndex][samp]=v[samp];
	}
	else {
	  theEvent->fVolts[chanIndex][samp]=0;
	}
      }
    }
    if(calType==AraCalType::kVoltageTime) {
      theEvent->fNumPoints[chanIndex]=numValid;
      for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
	theEvent->fTimes[chanIndex][samp]=timeNums[samp];
	if(samp<numValid) {
	  theEvent->fVolts[chanIndex][samp]=v[samp];
	}
	else {
	  theEvent->fVolts[chanIndex][samp]=0;
	}
      }
    }    
    if(calType==AraCalType::kJustPed) {
      theEvent->fNumPoints[chanIndex]=MAX_NUMBER_SAMPLES_LAB3;
      for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
	theEvent->fVolts[chanIndex][samp]=pedsubadc[samp];
	theEvent->fTimes[chanIndex][samp]=sampNums[samp];
      }
    }
    if(AraCalType::hasBinWidthCalib(calType)) {
      //Almost always want this
      theEvent->fNumPoints[chanIndex]=numValid;
      for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
	theEvent->fTimes[chanIndex][samp]=calTimeNums[samp];
	if(samp<numValid) {
	  theEvent->fVolts[chanIndex][samp]=calVoltNums[samp];
	}
	else {
	  theEvent->fVolts[chanIndex][samp]=0;
	}
      }
    }    
  }

  //Now we have done the initial bin-by-bin and epsilon calibrations
  //Next up is to do the clock alignment
  if(AraCalType::hasClockAlignment(calType)) {
    //All of the higher calibrations do some form of clock alignment
    calcClockAlignVals(theEvent,calType);    
    for(int  chanIndex = 0; chanIndex < NUM_DIGITIZED_ICRR_CHANNELS; ++chanIndex ){
      int nChip=theEvent->chan[chanIndex].chanId/CHANNELS_PER_LAB3;
      for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
	theEvent->fTimes[chanIndex][samp]+=clockAlignVals[stationId][nChip];
      }
    }
  }
  



  //For now we just have the one calibration type for interleaving
  AraGeomTool *tempGeom = AraGeomTool::Instance();
  for(int  rfchan = 0; rfchan < RFCHANS_PER_ICRR; ++rfchan ){
    memset(theEvent->fVoltsRF[rfchan],0,sizeof(Double_t)*2*MAX_NUMBER_SAMPLES_LAB3);
    memset(theEvent->fTimesRF[rfchan],0,sizeof(Double_t)*2*MAX_NUMBER_SAMPLES_LAB3);
    if(tempGeom->getNumLabChansForChan(rfchan)==2) {
      //      std::cout << chan << "\t"
      //		<< tempGeom->getFirstLabChanIndexForChan(rfchan) << "\t"
      //		<< tempGeom->getSecondLabChanIndexForChan(rfchan) << "\n";
      int ci1=tempGeom->getFirstLabChanIndexForChan(rfchan);
      int ci2=tempGeom->getSecondLabChanIndexForChan(rfchan);
      theEvent->fNumPointsRF[rfchan]=theEvent->fNumPoints[ci1]+theEvent->fNumPoints[ci2];
      //Need to zero mean, maybe should do this in each half seperately?
      //RJN hack 13/01/11
      double mean=0;
      for(int i=0;i<theEvent->fNumPoints[ci1];i++) {
	mean+=theEvent->fVolts[ci1][i];
      }
      for(int i=0;i<theEvent->fNumPoints[ci2];i++) {
	mean+=theEvent->fVolts[ci2][i];
      }
      mean/=theEvent->fNumPointsRF[rfchan];

      //Only the interleaved types have any special calib here
      if(AraCalType::hasInterleaveCalib(calType)) {
	int i1=0;
	int i2=0;
	for(int i=0;i<theEvent->fNumPointsRF[rfchan];i++) {
	  if(i1<theEvent->fNumPoints[ci1] && i2<theEvent->fNumPoints[ci2]) {
	    //Both in play
	    if(theEvent->fTimes[ci1][i1]<(theEvent->fTimes[ci2][i2]+interleaveVals[stationId][rfchan])) {
	      theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci1][i1];
	      theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci1][i1]-mean;
	      //	      std::cout << "A: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";

	      i1++;
	      continue;
	    }
	    else {
	      theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci2][i2]+interleaveVals[stationId][rfchan];
	      theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci2][i2]-mean;
	      //	      std::cout << "B: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
	      i2++;
	      continue;
	    }
	  }
	  else if(i1<theEvent->fNumPoints[ci1]) {
	    theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci1][i1];
	    theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci1][i1]-mean;
	    i1++;
	    //	      std::cout << "C: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
	    continue;
	  }
	  else if(i2<theEvent->fNumPoints[ci2]) {

	    theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci2][i2]+interleaveVals[stationId][rfchan];
	    theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci2][i2]-mean;
	    //	    std::cout << "D: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
	    i2++;
	    continue;
	  }
	}
      }
      
      else {	
	for(int i=0;i<theEvent->fNumPointsRF[rfchan];i++) {
	  if(i%2==0) {
	    //ci2 
	    theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci2][i/2];
	    theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci2][i/2];	  
	  }
	  else {
	    //ci1 
	    theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci1][i/2];
	    theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci1][i/2]+0.5*NSPERSAMP;	  
	  }
	  
	}
      }
      
    }
    else {
      //      std::cout << rfchan << "\t"
      //      << tempGeom->getFirstLabChanIndexForChan(rfchan) << "\n";
      int ci=tempGeom->getFirstLabChanIndexForChan(rfchan);
      theEvent->fNumPointsRF[rfchan]=theEvent->fNumPoints[ci];   

      //Need to zero mean the data
      double mean=0;
      for(int i=0;i<theEvent->fNumPoints[ci];i++) {
	mean+=theEvent->fVolts[ci][i];
      }
      mean/=theEvent->fNumPoints[ci];

      for(int i=0;i<theEvent->fNumPointsRF[rfchan];i++) {
	theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci][i]-mean;
	theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci][i];
	
      }
    }
   
    if(AraCalType::hasCableDelays(calType)) {
      Double_t delay=tempGeom->fAntInfo[rfchan].cableDelay;
      //      delay-=180; //Just an arbitrary offset
      for(int i=0;i<theEvent->fNumPointsRF[rfchan];i++) {
	theEvent->fTimesRF[rfchan][i]-=delay;
      }
    }
      
 
  }

  //  printf("calibrated the event\n");

}

void AraEventCalibrator::loadIcrrCalib()
{
  char calibFile[FILENAME_MAX];
  char calibDir[FILENAME_MAX];
  char *calibEnv=getenv("ARA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/araCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }  
  int chip,rco,chan;
 
  //Icrr
  //Bin Width Calib
  sprintf(calibFile,"%s/ICRR/TestBed/binWidths.txt",calibDir);
  std::ifstream BinFileTB(calibFile);
  double width;
  while(BinFileTB >> chip >> rco) {
    for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
      BinFileTB >> width;
      binWidths[0][chip][rco][samp]=width;
    }
  }
  //Epsilon Calib
  sprintf(calibFile,"%s/ICRR/TestBed/epsilonFile.txt",calibDir);
  std::ifstream EpsFileTB(calibFile);
  double epsilon;
  while(EpsFileTB >> chip >> rco >> epsilon) {
    epsilonVals[0][chip][rco]=epsilon;    
  }
  //Interleave Calib
  sprintf(calibFile,"%s/ICRR/TestBed/interleaveFile.txt",calibDir);
  std::ifstream IntFileTB(calibFile);
  double interleave;
  while(IntFileTB >> chip >> chan >> interleave) {
    interleaveVals[0][chan+4*chip]=interleave;    
  }

  //Station1
  //Bin Width Calib
  
  sprintf(calibFile,"%s/ICRR/Station1/binWidths.txt",calibDir);
  std::ifstream BinFileAra1(calibFile);
  while(BinFileAra1 >> chip >> rco) {
    for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;samp++) {
      BinFileAra1 >> width;
      binWidths[1][chip][rco][samp]=width;
    }
  }
  //Epsilon Calib
  sprintf(calibFile,"%s/ICRR/Station1/epsilonFile.txt",calibDir);
  std::ifstream EpsFileAra1(calibFile);
  while(EpsFileAra1 >> chip >> rco >> epsilon) {
    epsilonVals[1][chip][rco]=epsilon;    
  }
  //Interleave Calib
  sprintf(calibFile,"%s/ICRR/Station1/interleaveFile.txt",calibDir);
  std::ifstream IntFileAra1(calibFile);
  while(IntFileAra1 >> chip >> chan >> interleave) {
    interleaveVals[1][chan+4*chip]=interleave;    
  }
}



void AraEventCalibrator::calcClockAlignVals(UsefulIcrrStationEvent *theEvent, AraCalType::AraCalType_t calType)
{
  int stationId=theEvent->stationId;

  if(!AraCalType::hasClockAlignment(calType)) return;
  TGraph *grClock[LAB3_PER_ICRR]={0};
  Double_t lag[LAB3_PER_ICRR]={0};
  for(int chip=0;chip<LAB3_PER_ICRR;chip++) {
    clockAlignVals[stationId][chip]=0;    
    int chanIndex=ICRR1_CLOCK_CHANNEL+CHANNELS_PER_LAB3*chip; 
    grClock[chip]=theEvent->getGraphFromElecChan(chanIndex);
    lag[chip]=estimateClockLag(grClock[chip]);
    delete grClock[chip];

    if(chip>0) {
      //Then can actually do some alignment
      clockAlignVals[stationId][chip]=lag[0]-lag[chip];
      //The below fudge factors were "tuned" using pulser data 
      // to try and remove period ambiguities resulting from wrong cycle lag
      if(lag[chip]<8 && lag[0]>9) 
	clockAlignVals[stationId][chip]-=25;
      if(lag[chip]>9 && lag[0]<7) 
	clockAlignVals[stationId][chip]+=25;
      //      std::cout << "clockAlignVals[ " << chip << "] = " << clockAlignVals[chip] << "\n";
    }
    
  }  
}


Double_t AraEventCalibrator::estimateClockLag(TGraph *grClock)
{
  // This funciton estimates the clock lag (i.e. phase but expressed in terms of a deltaT between 0 and 1 period) by just using all the negative-positive zero crossing

  Double_t period=ICRR1_CLOCK_PERIOD;
  Double_t mean=grClock->GetMean(2);
  Int_t numPoints=grClock->GetN();
  if(numPoints<3) return 0;
  Double_t *tVals=grClock->GetX();
  Double_t *vVals=grClock->GetY();

  Double_t zc[1000]={0}; 
  Double_t rawZc[1000]={0}; 
  int countZC=0;
  for(int i=2;i<numPoints;i++) {
    if(vVals[i-1]<0 && vVals[i]>0) {
      Double_t x1=tVals[i-1];
      Double_t x2=tVals[i];
      Double_t y1=vVals[i-1]-mean;
      Double_t y2=vVals[i]-mean;      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      zc[countZC]=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      rawZc[countZC]=zc[countZC];
      countZC++;
      //      if(countZC==1)
      //      break;
    }
       
  }

  Double_t firstZC=zc[0];
  while(firstZC>period) firstZC-=period;
  Double_t meanZC=0;
  Double_t meanZC2=0;
  for(int i=0;i<countZC;i++) {
    while((zc[i])>period) zc[i]-=period;
    if(TMath::Abs((zc[i]-period)-firstZC)<TMath::Abs(zc[i]-firstZC))
      zc[i]-=period;
    if(TMath::Abs((zc[i]+period)-firstZC)<TMath::Abs(zc[i]-firstZC))
      zc[i]+=period;
    meanZC+=zc[i];
    meanZC2+=zc[i]*zc[i];
    //     std::cout << i << "\t" << zc[i] << "\t" << rawZc[i] << "\n";     
  }
  meanZC/=countZC;
  meanZC2/=countZC;
  //  Double_t rms=TMath::Sqrt(meanZC2-meanZC*meanZC);
  //  std::cout << meanZC << "\t" << rms << "\n";
  return meanZC;

}

void AraEventCalibrator::fillRCOGuessArray(UsefulIcrrStationEvent *theEvent, int rcoGuess[LAB3_PER_ICRR])
{
  for(int chip=0;chip<LAB3_PER_ICRR;chip++) {
    int chanIndex=ICRR1_CLOCK_CHANNEL+CHANNELS_PER_LAB3*chip;
    rcoGuess[chip]=theEvent->getRawRCO(chanIndex);
    
    Double_t period[2]={0};
    Double_t rms[2]={0};
    for(int rcoTest=0;rcoTest<2;rcoTest++) {
      int numValid=doBinCalibration(theEvent,chanIndex,rcoTest);
      period[rcoTest]=estimateClockPeriod(numValid,rms[rcoTest]);
    }
    
    Double_t periodTest=(TMath::Abs(period[0]-ICRR1_CLOCK_PERIOD)-TMath::Abs(period[1]-ICRR1_CLOCK_PERIOD));
    Double_t rmsTest=(rms[0]-rms[1]);
    if(periodTest<0) {
      rcoGuess[chip]=0;
      if(TMath::Abs(periodTest)<0.5 && rmsTest>0) 
	rcoGuess[chip]=1;
    }    
    else {
      rcoGuess[chip]=1;
      if(TMath::Abs(periodTest)<0.5 && rmsTest<0) {
	rcoGuess[chip]=0;	
      }
    }
    if(rms[rcoGuess[chip]]>4)  {
      rcoGuess[chip]=theEvent->getRawRCO(chanIndex);
    }

    //    std::cout << "AraEventCalibrator:\t" << period[0] << "\t" << period[1] << "\n";
    //    std::cout << "AraEventCalibrator:\t" << periodTest << "\t" << rmsTest << "\t" << rcoGuess[chip] << "\n";
  }
}



Double_t AraEventCalibrator::estimateClockPeriod(Int_t numPoints, Double_t &rms)
{
  // This funciton estimates the period by just using all the negative-positive zero crossing
  if(numPoints<3) return 0;
  Double_t mean=0;
  Double_t vVals[MAX_NUMBER_SAMPLES_LAB3]={0};
  for(int i=0;i<numPoints;i++) {
    mean+=calVoltNums[i];
  }
  mean/=numPoints;
  for(int i=0;i<numPoints;i++) {
    vVals[i]=calVoltNums[i]-mean;
  }


  Double_t zc[1000]={0};
  Double_t periods[1000]={0};
  int countZC=0;
  for(int i=2;i<numPoints;i++) {
    if(vVals[i-1]<0 && vVals[i]>0) {
      Double_t x1=calTimeNums[i-1];
      Double_t x2=calTimeNums[i];
      Double_t y1=vVals[i-1];
      Double_t y2=vVals[i];      
      //      std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
      Double_t zcTime=(((0-y1)/(y2-y1))*(x2-x1))+x1;
      if(countZC>0) {
	if((zcTime-zc[countZC-1])<10)
	  continue;
      }
      zc[countZC]=zcTime;
      countZC++;
      //      if(countZC==1)
      //      break;
    }    
  }

  if(countZC<2) return 0;
  
  
  int countPeriods=0;
  Double_t meanPeriod=0;
  Double_t meanPeriodSq=0;
  for(int i=1;i<countZC;i++) {
    periods[countPeriods]=zc[i]-zc[i-1];
    meanPeriod+=periods[countPeriods];
    meanPeriodSq+=periods[countPeriods]*periods[countPeriods];
    countPeriods++;
  }
  meanPeriod/=countPeriods;
  meanPeriodSq/=countPeriods;
  rms=TMath::Sqrt(meanPeriodSq-meanPeriod*meanPeriod);
  
  return meanPeriod;
}

void AraEventCalibrator::calibrateEvent(UsefulAtriStationEvent *theEvent, AraCalType::AraCalType_t calType) 
{
  //Really what we will do here is just kNoCalib
  static int gotPeds=0;
  if(!gotPeds)  {
    loadAtriPedestals();
    gotPeds=1;
  }

  //Step one is loop over the blocks

  std::vector<RawAtriStationBlock>::iterator blockIt;
  std::vector< std::vector<UShort_t> >::iterator vecVecIt;
  std::map< Int_t, std::vector <Double_t> >::iterator timeMapIt;
  std::map< Int_t, std::vector <Double_t> >::iterator voltMapIt;
  std::map< Int_t, std::vector <Double_t> >::iterator voltMapIt2;
  std::vector< UShort_t >::iterator shortIt;
  for(blockIt = theEvent->blockVec.begin(); 
      blockIt!=theEvent->blockVec.end();
      blockIt++) {
    //Step two is determine the channel Ids
    Int_t irsChan[8];
    Int_t numChans=0;
    for(Int_t bit=0;bit<8;bit++) {
      Int_t mask=(1<<bit);
      if((blockIt->channelMask)&mask) {
	irsChan[numChans]=bit;
	numChans++;
      }
    }
    //    std::cout << "Got numChans " << numChans << "\n";

    //Step three is loop over the channels within a block
    Int_t uptoChan=0;
    for(vecVecIt=blockIt->data.begin();
	vecVecIt!=blockIt->data.end();
	vecVecIt++) {
      Int_t chanId=irsChan[uptoChan] | ((blockIt->channelMask&0x300)>>5);
      Int_t chan=irsChan[uptoChan];
      Int_t dda=blockIt->getDda();
      Int_t block=blockIt->getBlock();
      Int_t capArray=blockIt->getCapArray();
      //      std::cout << "Got chanId " << chanId << "\t" << irsChan[uptoChan] << "\t" 
      //      		<< dda << "\t" << block << "\t" << RawAtriStationEvent::getPedIndex(dda,block,chan,0) << "\n";
      uptoChan++;

      //Step four is to check if we have already got this chanId

      timeMapIt=theEvent->fTimes.find(chanId);
      Double_t time=0;
      Int_t firstTime=1;
      if(timeMapIt==theEvent->fTimes.end()) {
	//First time round for this channel
	std::vector <Double_t> tempTimes;
	std::vector <Double_t> tempVolts;	
	//Now need to insert empty vector into map
	theEvent->fTimes.insert( std::pair< Int_t, std::vector <Double_t> >(chanId,tempTimes));
	theEvent->fVolts.insert( std::pair< Int_t, std::vector <Double_t> >(chanId,tempVolts));
	theEvent->fNumChannels++;
      }
      else {
	//Just get the last time
	time=timeMapIt->second.back();
	//	if(dda==1 &&chan==1)
	  //	  std::cout << "Last time " << time << "\t" << timeMapIt->second.size() << "\n";
	firstTime=0;
      }
      
      //Set the iterators to point to the correct channel in the map
      timeMapIt=theEvent->fTimes.find(chanId);
      voltMapIt=theEvent->fVolts.find(chanId);

      int samp=0;
      int index=0;
      //Now loop over the 64 samples
      Double_t tempTimes[SAMPLES_PER_BLOCK];
      Double_t tempVolts[SAMPLES_PER_BLOCK];
      Int_t voltIndex[SAMPLES_PER_BLOCK];

      //Here is the Epsilon calibration
      if(AraCalType::hasBinWidthCalib(calType)) {
	if(!firstTime) {
	  //Add on the time between blocks
	  time+=fAtriEpsilonTimes[dda][chan][capArray];
	  //	  if(dda==1 && chan==1)
	  //	    std::cout << "Block " << time << "\t" << fAtriEpsilonTimes[dda][chan][capArray] << "\n";

	}
      }

      for(shortIt=vecVecIt->begin();
	  shortIt!=vecVecIt->end();
	  shortIt++) {
	
	if(AraCalType::hasBinWidthCalib(calType)) {

	  //Now need to work out where to place the sample
	  voltIndex[samp]=fAtriSampleIndex[dda][chan][capArray][samp];

	  //Now get the time
	  tempTimes[samp]=time+fAtriSampleTimes[dda][chan][capArray][samp];

	  //	  if(dda==1 && chan==1) {
	  //	  //	    std::cout << dda << "\t" << chan << "\t" << capArray << "\t" << samp << "\t" << fAtriSampleTimes[dda][chan][capArray][samp] << "\n";
	  //	    std::cout << index << "\t" << tempTimes[samp] << "\t" << time << "\n";
	  //	  }
	  
	}
	else {
	  time+=1;	
	  tempTimes[samp]=time;
	  voltIndex[samp]=samp;
	}
	
	//Now the voltage part
	if(!AraCalType::hasPedestalSubtraction(calType))
	  tempVolts[samp]=(*shortIt); ///<Filling with ADC
	else {
	  tempVolts[samp]=(*shortIt)-(Int_t)fAtriPeds[RawAtriStationEvent::getPedIndex(dda,block,chan,samp)]; ///<Filling with ADC-Pedestal
	  //	  if(dda==3 && samp<2) {
	  //	    std::cout << (*shortIt)  << "\t" << (Int_t)fAtriPeds[RawAtriStationEvent::getPedIndex(dda,block,chan,samp)] << "\n";
	  //	  }
	}
	samp++;
      }

      for(samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
	timeMapIt->second.push_back(tempTimes[samp]); ///<Filling with time
	voltMapIt->second.push_back(tempVolts[voltIndex[samp]]); //Filling with volts
	
      }
      
	    
    }
  }
  
  if(hasCommonMode(calType)) {
    //Then we need to do a common mode correction
    //loop over dda
    //loop over chan 
    //loop over times and subtract one
    
    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
      for(Int_t chan=0;chan<5;chan++) {
  	Int_t chanId=chan+RFCHAN_PER_DDA*dda;
  	voltMapIt=theEvent->fVolts.find(chanId);
	Int_t chanId2=5+RFCHAN_PER_DDA*dda;
  	voltMapIt2=theEvent->fVolts.find(chanId2);
	
  	Int_t numPoints=(voltMapIt->second).size();
  	for(int samp=0;samp<numPoints;samp++) {
  	  voltMapIt->second[samp]-=voltMapIt2->second[samp];
  	}
      }
    }

  }


            
}



void AraEventCalibrator::setAtriPedFile(char *filename)
{
  strncpy(fAtriPedFile,filename,FILENAME_MAX);
  fGotAtriPedFile=1;
  loadAtriPedestals();
}

void AraEventCalibrator::loadAtriPedestals()
{  
  
  if(!fGotAtriPedFile) {
    char *pedFileEnv = getenv( "ARA_ONE_PEDESTAL_FILE" );
    if ( pedFileEnv == NULL ) {
      char calibDir[FILENAME_MAX];
      char *calibEnv=getenv("ARA_CALIB_DIR");
      if(!calibEnv) {
	char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
	if(!utilEnv) {
	  sprintf(calibDir,"calib");
	  fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ./calib]");
	} else {
	  sprintf(calibDir,"%s/share/araCalib",utilEnv);
	  fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ARA_UTIL_INSTALL_DIR/share/calib]");
	}
      }
      else {
	strncpy(calibDir,calibEnv,FILENAME_MAX);
	fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ARA_CALIB_DIR]");
      }
      sprintf(fAtriPedFile,"%s/ATRI/araAtriPedestals.txt",calibDir);
      fprintf(stdout," = %s\n",fAtriPedFile);
    } // end of IF-block for pedestal file not specified by environment variable
    else {
      strncpy(fAtriPedFile,pedFileEnv,FILENAME_MAX);
      fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ARA_ONE_PEDESTAL_FILE] = %s\n",fAtriPedFile);
    } // end of IF-block for pedestal file specified by environment variable
  }


  //Pedestal file
  std::ifstream PedFile(fAtriPedFile);
  Int_t dda,block,chan;
  UShort_t pedVal;
  
  fAtriPeds = new UShort_t [DDA_PER_ATRI*BLOCKS_PER_DDA*RFCHAN_PER_DDA*SAMPLES_PER_BLOCK];
  if(!fAtriPeds) {
    std::cerr << "Can not allocate memory for pedestal file\n";
    exit(0);
  }
      
  

  while(PedFile >> dda >> block >> chan) {
    for(int samp=0;samp<SAMPLES_PER_BLOCK;samp++) {
      PedFile >> pedVal;
      fAtriPeds[RawAtriStationEvent::getPedIndex(dda,block,chan,samp)]=pedVal;
    }
  }  

}


void AraEventCalibrator::loadAtriCalib()
{
  char calibFile[FILENAME_MAX];
  char calibDir[FILENAME_MAX];
  char *calibEnv=getenv("ARA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/araCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }  

  int dda,chan,sample,capArray;
  sprintf(calibFile,"%s/ATRI/araAtriSampleTiming.txt",calibDir);
  std::ifstream SampleFile(calibFile);
  for(dda=0;dda<DDA_PER_ATRI;dda++) {
    for(chan=0;chan<RFCHAN_PER_DDA;chan++) {
      for(capArray=0;capArray<2;capArray++) {
	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
	  fAtriSampleTimes[dda][chan][capArray][sample]=sample/3.2;
	  fAtriSampleIndex[dda][chan][capArray][sample]=sample;
	}
	fAtriEpsilonTimes[dda][chan][capArray]=1/3.2;
      }
    }
  }

  Double_t value;
  Int_t index;
  while(SampleFile >> dda >> chan >> capArray) {
    //    std::cout <<  dda << "\t" << chan << "\t" << capArray << "\t";
    for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
      SampleFile >> index;
      fAtriSampleIndex[dda][chan][capArray][sample]=index;
      //      std::cout << fAtriSampleIndex[dda][chan][ << " ";    
    }

    SampleFile >> dda >> chan >> capArray;
    for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
      SampleFile >> value;
      fAtriSampleTimes[dda][chan][capArray][sample]=value;
      //      std::cout << fAtriSampleTimes[dda][chan][capArray][sample] << " ";    
    }
    //    std::cout << "\n";
  }
}
