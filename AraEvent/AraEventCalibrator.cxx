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
#include "araSoft.h"
#include "TMath.h"
#include "TGraph.h"
#include "FFTtools.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <zlib.h>
#include <cstdlib>
#include <sstream>
#include <numeric>

/*!
    Returns if a calibration type should or should not trim the first block of a waveform., 27-09-2021 -MK-
    The first block trimming is triggerred above kVoltageTime mode. 
    So that, analyzers still can choose to see 1st block at least in ADC WF
    If analyzers want to see 1st block with voltage value, uncommand out the return kFALSE;
*/
/*!
    \param calType A calibration mode listed in the EAraCalType
    \return boolean True: perform the first block trimming, False: skip it
*/
Bool_t AraCalType::hasTrimFirstBlock(AraCalType::AraCalType_t calType)
{
    //return kFALSE; ///< Just in case, analyzers want to see 1st block on kLatestCalib
    if(calType==kOnlyPed
        || calType==kOnlyADC) return kFALSE;
    if(calType<kVoltageTime) return kFALSE;
    return kTRUE;
}

//! Returns if a calibration type should or should not invert the RF channels = 0,4,8 in A3, 27-09-2021 -MK-
/*!
    \param calType A calibration mode listed in the EAraCalType
    \return boolean True: perform the inversion for only RF channels = 0,4,8 in A3, False: skip it
*/
Bool_t AraCalType::hasInvertA3Chans(AraCalType::AraCalType_t calType)
{
    if(calType>kLatestCalibWithOutZeroMean) return kFALSE;  
    if(calType<kVoltageTime) return kFALSE;
    return kTRUE;
}

//! Zeroing ADC WF by subtracting mean based on inputted calibration type, 27-09-2021 -MK-
/*!
    \param calType A calibration mode listed in the EAraCalType
    \return boolean True: perform the ApplyMeanZero for ADC WF, False: skip it
*/
Bool_t AraCalType::hasADCZeroMean(AraCalType::AraCalType_t calType)
{
    if(calType>kLatestCalib14to20_Bug) return kFALSE;
    if(calType<kVoltageTime) return kFALSE;
    return kTRUE;    
}

//! Zeroing voltage WF by subtracting mean based on inputted calibration type, 27-09-2021 -MK-
/*!
    \param calType A calibration mode listed in the EAraCalType
    \return boolean True: perform the ApplyMeanZero for voltage WF, False: skip it
*/
Bool_t AraCalType::hasVoltZeroMean(AraCalType::AraCalType_t calType)
{
    if(calType>kLatestCalib14to20_Bug) return kFALSE;
    if(calType<kVoltageTime) return kFALSE;
    return kTRUE;
}

//added, 12-Feb 2014 -THM-
Bool_t AraCalType::hasVoltCal(AraCalType::AraCalType_t calType)
{
    if(calType>kLatestCalibWithOutZeroMean) return kFALSE;
    if(calType==kLatestCalib14to20_Bug)return kFALSE;
    //return kFALSE; //RJN hackcd .. un-hacked KAH 09152020
    if(calType<kVoltageTime) return kFALSE; ///< Need to be only kFALSE before voltage calibration
    return kTRUE;
}

Bool_t AraCalType::hasCableDelays(AraCalType::AraCalType_t calType)
{ 
    if(calType==kFirstCalibPlusCables
        || calType==kSecondCalibPlusCables
        || calType==kSecondCalibPlusCablesUnDiplexed
        || calType==kLatestCalib14to20_Bug
        || calType==kLatestCalibWithOutZeroMean
        || calType==kOnlyPedWithOut1stBlockAndBadSamples
        || calType==kOnlyADCWithOut1stBlockAndBadSamples
        || calType==kJustPedWithOut1stBlockAndBadSamples){

        return kTRUE;
    }
    return kFALSE;
}

Bool_t AraCalType::hasInterleaveCalib(AraCalType::AraCalType_t calType)
{ 
    if(calType==kFirstCalibPlusCables
        || calType==kSecondCalibPlusCables
        || calType==kFirstCalib
        || calType==kSecondCalib
        || calType==kSecondCalibPlusCablesUnDiplexed
        || calType==kLatestCalib14to20_Bug){

        return kTRUE;
    }
    return kFALSE;
}

Bool_t AraCalType::hasBinWidthCalib(AraCalType::AraCalType_t calType)
{
    if(calType==kOnlyPed
        || calType==kOnlyPedWithOut1stBlock
        || calType==kOnlyADC
        || calType==kOnlyADCWithOut1stBlock
        || calType==kJustPedWithOut1stBlock) return kFALSE;
    if(calType>=kFirstCalib)
        return kTRUE;
    return kFALSE;
}

Bool_t AraCalType::hasClockAlignment(AraCalType::AraCalType_t calType)
{ 
    if(calType==kSecondCalibPlusCables
        || calType==kSecondCalib
        || calType==kSecondCalibPlusCablesUnDiplexed

        || calType==kLatestCalib14to20_Bug){

        return kTRUE;
    }
    return kFALSE;
}

Bool_t AraCalType::hasPedestalSubtraction(AraCalType::AraCalType_t calType)
{ 
    if(calType==kJustPedWithOut1stBlock
        || calType==kJustPedWithOut1stBlockAndBadSamples) return kTRUE;
    if(calType>kOnlyPedWithOut1stBlockAndBadSamples) return kFALSE; ///< Get the raw ADC WF without bad samples and pedestal subtraction, 19-12-2021 -MK-
    if(calType==kNoCalib) return kFALSE;
    return kTRUE;
}

Bool_t AraCalType::hasCommonMode(AraCalType::AraCalType_t calType)
{
    return kFALSE;
    if(calType==kNoCalib) return kFALSE;
    return kTRUE;
}

Bool_t AraCalType::hasUnDiplexing(AraCalType::AraCalType_t calType)
{
    if(calType==kSecondCalibPlusCablesUnDiplexed 
        || calType==kLatestCalib14to20_Bug) return kTRUE;

    return kFALSE;
}



ClassImp(AraEventCalibrator);

AraEventCalibrator * AraEventCalibrator::fgInstance=0;


AraEventCalibrator::AraEventCalibrator() 
{

    // Loop through and set the got ped / calib flags to zero. This assumes stationId's first n elements
    // are for ICRR stations and the remaining N-n are ATRI
    memset(fGotAtriPedFile,0,sizeof(Int_t)*ATRI_NO_STATIONS);
    memset(fGotAtriCalibFile,0,sizeof(Int_t)*ATRI_NO_STATIONS);
    memset(gotIcrrPedFile,0,sizeof(Int_t)*ICRR_NO_STATIONS);
    memset(gotIcrrCalibFile,0,sizeof(Int_t)*ICRR_NO_STATIONS);
    fAtriPeds=0;


}

AraEventCalibrator::~AraEventCalibrator() {
    // Default Destructor
}

AraEventCalibrator*  AraEventCalibrator::Instance()
{
    
    // printf("AraEventCalibrator::Instance() creating an instance of the calibrator\n");
    // static function
    if(fgInstance)
        return fgInstance;

    fgInstance = new AraEventCalibrator();

    return fgInstance;
}


void AraEventCalibrator::setPedFile(char fileName[], AraStationId_t stationId)
{

    strncpy(IcrrPedFile[stationId],fileName,FILENAME_MAX);
    if(stationId==ARA_TESTBED){
        // fprintf(stdout, "AraEventCalibrator::setPedFile() setting TestBed IcrrPedFile to %s\n",  IcrrPedFile[stationId]);
        gotIcrrPedFile[0]=1; //Protects from loading the default pedestal File
    }

    else if(stationId==ARA_STATION1){
        // fprintf(stdout, "AraEventCalibrator::setPedFile() setting Station1 IcrrPedFile to %s\n", IcrrPedFile[stationId]);
        gotIcrrPedFile[1]=1; //Protects from loading the default pedestal File
    }
    else {
        fprintf(stderr, "AraEventCalibrator::setPedFile() -- not ICRR station, stationId %d\n", stationId);
        return;
    }
    loadIcrrPedestals(stationId);
}

void AraEventCalibrator::loadIcrrPedestals(AraStationId_t stationId)
{

    if(stationId==ARA_TESTBED&&gotIcrrPedFile[0]==0){
        // populate the IcrrPedFile[0] variable
        char *pedFileEnv = getenv( "ARA_PEDESTAL_FILE" );
        if ( pedFileEnv == NULL ) {
            char calibDir[FILENAME_MAX];
            char *calibEnv=getenv("ARA_CALIB_DIR");
            if(!calibEnv) {
                char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
                if(!utilEnv) {
                    sprintf(calibDir,"calib");
                } else {
                    sprintf(calibDir,"%s/share/araCalib",utilEnv);
                }
            }
            else {
                strncpy(calibDir,calibEnv,FILENAME_MAX);
            }
            sprintf(IcrrPedFile[0],"%s/ICRR/TestBed/peds_1294924296.869787.run001202.dat",calibDir);
        } // end of IF-block for pedestal file not specified by environment variable
        else {
            strncpy(IcrrPedFile[0],pedFileEnv,FILENAME_MAX);
        } // end of IF-block for pedestal file specified by environment variable
    }
    if(stationId==ARA_STATION1&&gotIcrrPedFile[1]==0){
        // populate the IcrrPedFile[1] variable
        char *pedFileEnv = getenv( "ARA_PEDESTAL_FILE" );
        if ( pedFileEnv == NULL ) {
            char calibDir[FILENAME_MAX];
            char *calibEnv=getenv("ARA_CALIB_DIR");
            if(!calibEnv) {
                char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
                if(!utilEnv) {
                    sprintf(calibDir,"calib");
                } else {
                    sprintf(calibDir,"%s/share/araCalib",utilEnv);
                }
            }
            else {
                strncpy(calibDir,calibEnv,FILENAME_MAX);
            }
            sprintf(IcrrPedFile[1],"%s/ICRR/Station1/peds_1326108401.602169.run003747.dat",calibDir);
        } // end of IF-block for pedestal file not specified by environment variable
        else {
            strncpy(IcrrPedFile[1],pedFileEnv,FILENAME_MAX);
        } // end of IF-block for pedestal file specified by environment variable
    }
    // Finished populating the IcrrPedFile variables

    FullLabChipPedStruct_t peds;

    // Now we load in the pedestals
    if(stationId==ARA_TESTBED){
        gzFile inPedTB = gzopen(IcrrPedFile[0],"r");
        fprintf(stdout, "%s : loading IcrrPedFile TestBed %s\n", __FUNCTION__, IcrrPedFile[0]);//DEBUG

        if( !inPedTB ){
            fprintf(stderr,"ERROR - Failed to open pedestal file for TestBed %s.\n",IcrrPedFile[0]);
            return;
        }
        int nRead = gzread(inPedTB,&peds,sizeof(FullLabChipPedStruct_t));
        if( nRead != sizeof(FullLabChipPedStruct_t)){
            int numErr;
            fprintf(stderr,"ERROR - Error reading pedestal file %s; %s\n",IcrrPedFile[0],gzerror(inPedTB,&numErr));
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
        
        gotIcrrPedFile[0]=1;
        gotIcrrPedFile[1]=0;
        // last thing is to set the gotIcrrPedFile flags appropriately
    }
    if(stationId==ARA_STATION1){
        gzFile inPedAra1 = gzopen(IcrrPedFile[1],"r");
        fprintf(stdout, "%s : loading IcrrPedFile Station1 %s\n", __FUNCTION__, IcrrPedFile[1]);//DEBUG
        if( !inPedAra1 ){
            fprintf(stderr,"ERROR - Failed to open pedestal file for Station1 %s\n",IcrrPedFile[1]);
            return;
        }
        
        int nRead = gzread(inPedAra1,&peds,sizeof(FullLabChipPedStruct_t));
        if( nRead != sizeof(FullLabChipPedStruct_t)){
            int numErr;
            fprintf(stderr,"ERROR - Error reading pedestal file %s; %s\n",IcrrPedFile[1],gzerror(inPedAra1,&numErr));
            gzclose(inPedAra1);
            return;
        }
        
        int chip, chan, samp;
        for(chip=0;chip<LAB3_PER_ICRR;++chip) {
            for(chan=0;chan<CHANNELS_PER_LAB3;++chan) {
                int chanIndex = chip*CHANNELS_PER_LAB3+chan;
                for(samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp) {
                    pedestalData[1][chip][chan][samp]=peds.chan[chanIndex].pedMean[samp];
                }
            }
        }
        gzclose(inPedAra1);
        gotIcrrPedFile[0]=0;
        gotIcrrPedFile[1]=1;
        // last thing is to set the gotIcrrPedFile flags appropriately
    }
    // Finished loading pedestals
}

int AraEventCalibrator::doBinCalibration(UsefulIcrrStationEvent *theEvent, int chanIndex,int overrideRCO)
{
    AraStationId_t stationId=theEvent->stationId;

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
            if(calwv[samp]>SATURATION)
                calwv[samp]=SATURATION;
            if(calwv[samp]<-1*SATURATION)
                calwv[samp]=-1*SATURATION;
        }
    }
        
    if(nChan==0 || (overrideRCO==0 || overrideRCO==1)) {
        // Do the calibration for each chip
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
            // Now add epsilon
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
    
    // Now we just have to make sure the times are monotonically increasing
    for(int i=0;i<MAX_NUMBER_SAMPLES_LAB3;i++) {
        calTimeNums[i]=tempTimeNums[indexNums[i]];
        calVoltNums[i]=v[indexNums[i]];
        // std::cout << i << "\t" << indexNums[i] << "\t" << calTimeNums[i] << "\t" << calVoltNums[i] << "\n";
    }
    return numValid;
}


void AraEventCalibrator::calibrateEvent(UsefulIcrrStationEvent *theEvent, AraCalType::AraCalType_t calType) 
{

    AraStationId_t stationId=theEvent->stationId;
    if(gotIcrrPedFile[stationId]==0){
        loadIcrrPedestals(stationId); //This will only load the pedestals once
    }
    
    if(gotIcrrCalibFile[stationId]==0){
        loadIcrrCalib(stationId); //This will only load the calib values once
    }

    if(AraCalType::hasBinWidthCalib(calType))
        theEvent->guessRCO(0); //Forces the calculation of the RCO phase from the clock
    // FIXME -- should this be a zero or do we want the actual channel number?


    // set the number of rfchans in the event

    if(stationId==ARA_TESTBED)
        theEvent->numRFChans=RFCHANS_TESTBED;
    else
        theEvent->numRFChans=RFCHANS_STATION1;


    for(int samp=0;samp<MAX_NUMBER_SAMPLES_LAB3;++samp){
        sampNums[samp]=samp;
        timeNums[samp]=samp*NSPERSAMP_ICRR;
    }
    
    for(int  chanIndex = 0; chanIndex < NUM_DIGITIZED_ICRR_CHANNELS; ++chanIndex ){
        int numValid=doBinCalibration(theEvent,chanIndex);

        // Now we stuff it back into the UsefulIcrrStationEvent object
                 
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

    // Now we have done the initial bin-by-bin and epsilon calibrations
    // Next up is to do the clock alignment
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
    
    // For now we just have the one calibration type for interleaving
    AraGeomTool *tempGeom = AraGeomTool::Instance();

    for(int  rfchan = 0; rfchan < (theEvent->numRFChans); ++rfchan ){
        // printf("AraEventCalibrator::calibrateEvent() rfchan %i stationId %i\n", rfchan, stationId);
        
        memset(theEvent->fVoltsRF[rfchan],0,sizeof(Double_t)*2*MAX_NUMBER_SAMPLES_LAB3);
        memset(theEvent->fTimesRF[rfchan],0,sizeof(Double_t)*2*MAX_NUMBER_SAMPLES_LAB3);


        if(tempGeom->getStationInfo(stationId)->getNumLabChansForChan(rfchan)==2) {
            // printf("interleaved channel\n");
            // std::cout << rfchan << "\t"
            //  << tempGeom->getStationInfo(stationId)->getFirstLabChanIndexForChan(rfchan) << "\t"
            // << tempGeom->getStationInfo(stationId)->getSecondLabChanIndexForChan(rfchan) << "\n";

            // printf("rfchan %i stationId %i labChip %i FirstLabChan %i SecondLabChan %i numLabChans %i labChans[0] %i labChans[1] %i\n", rfchan, tempGeom->getStationInfo(stationId)->getLabChipForChan(rfchan), tempGeom->getStationInfo(stationId)->getFirstLabChanForChan(rfchan), tempGeom->getStationInfo(stationId)->getSecondLabChanForChan(rfchan),tempGeom->getStationInfo(stationId)->getNumLabChansForChan(rfchan), tempGeom->getStationInfo(stationId)->getFirstLabChanIndexForChan(rfchan), tempGeom->getStationInfo(stationId)->getSecondLabChanIndexForChan(rfchan));

            int ci1=tempGeom->getStationInfo(stationId)->getFirstLabChanIndexForChan(rfchan);
            int ci2=tempGeom->getStationInfo(stationId)->getSecondLabChanIndexForChan(rfchan);
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
                        // Both in play
                        if(theEvent->fTimes[ci1][i1]<(theEvent->fTimes[ci2][i2]+interleaveVals[stationId][rfchan])) {
                            theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci1][i1];
                            theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci1][i1]-mean;
                            // std::cout << "A: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
                            i1++;
                            continue;
                        }
                        else {
                            theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci2][i2]+interleaveVals[stationId][rfchan];
                            theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci2][i2]-mean;
                            // std::cout << "B: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
                            i2++;
                            continue;
                        }
                    }
                    else if(i1<theEvent->fNumPoints[ci1]) {
                        theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci1][i1];
                        theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci1][i1]-mean;
                        i1++;
                        // std::cout << "C: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
                        continue;
                    }
                    else if(i2<theEvent->fNumPoints[ci2]) {
                        theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci2][i2]+interleaveVals[stationId][rfchan];
                        theEvent->fVoltsRF[rfchan][i]=theEvent->fVolts[ci2][i2]-mean;
                        // std::cout << "D: " << i << "\t" << theEvent->fTimesRF[rfchan][i] << "\n";
                        i2++;
                        continue;
                    }
                }
            }//interleaved
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
                        theEvent->fTimesRF[rfchan][i]=theEvent->fTimes[ci1][i/2]+0.5*NSPERSAMP_ICRR;      
                    }
                }
            }  
        }//numLabChans==2
        
        //Perform undiplexing of channels (ARA1 has surface antennae diplexed in to HPOL channels)
        else if(tempGeom->getStationInfo(stationId)->isDiplexed(rfchan)==0||!(AraCalType::hasUnDiplexing(calType))){
            // printf("non-interleaved non-diplexed channel\n");
            // std::cout << rfchan << "\t"
            // << tempGeom->getStationInfo(stationId)->getFirstLabChanIndexForChan(rfchan) << "\n";
            int ci=tempGeom->getStationInfo(stationId)->getFirstLabChanIndexForChan(rfchan);
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
        }//numLabChannels!=2&&isDiplexed==0
        
        else if(tempGeom->getStationInfo(stationId)->isDiplexed(rfchan)==1&&(AraCalType::hasUnDiplexing(calType))){
            // printf("non-interleaved diplexed channel\n");
            // now need to work out how to do a diplexed channel
            // find lab channel from which to derived rfchan
            int ci=tempGeom->getStationInfo(stationId)->getFirstLabChanIndexForChan(rfchan);
            int noPoints=theEvent->fNumPoints[ci];
            Double_t lowPassFreq=tempGeom->getStationInfo(stationId)->getLowPassFilter(rfchan);
            Double_t highPassFreq=tempGeom->getStationInfo(stationId)->getHighPassFilter(rfchan);

            TGraph *gUnfiltered = new TGraph(noPoints, theEvent->fTimes[ci], theEvent->fVolts[ci]);
    
            TGraph *gFiltered = FFTtools::simplePassBandFilter(gUnfiltered, highPassFreq, lowPassFreq);
            Double_t *filteredT = gFiltered->GetX();
            Double_t *filteredV = gFiltered->GetY();


            for(int i=0;i<noPoints;i++){
                theEvent->fTimesRF[rfchan][i]=filteredT[i];
                theEvent->fVoltsRF[rfchan][i]=filteredV[i];
            }
            theEvent->fNumPointsRF[rfchan]=noPoints;

            delete gFiltered;
            delete gUnfiltered;

        }//isDiplexed==1
     
        if(AraCalType::hasCableDelays(calType)) {
            Double_t delay=tempGeom->getStationInfo(stationId)->getCableDelay(rfchan);
            //      delay-=180; //Just an arbitrary offset
            for(int i=0;i<theEvent->fNumPointsRF[rfchan];i++) {
                theEvent->fTimesRF[rfchan][i]-=delay;
            }
        }
    }
}

void AraEventCalibrator::loadIcrrCalib(AraStationId_t stationId)
{
    char calibDir[FILENAME_MAX];
    char binWidthFileName[FILENAME_MAX];
    char epsilonFileName[FILENAME_MAX];
    char interleaveFileName[FILENAME_MAX];
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
    // Populate the binWidthFileName, epsilonFileName and interleaveFileName variables
    if(stationId==ARA_TESTBED&&gotIcrrCalibFile[0]==0){
        sprintf(binWidthFileName,"%s/ICRR/TestBed/binWidths.txt",calibDir);
        sprintf(epsilonFileName,"%s/ICRR/TestBed/epsilonFile.txt",calibDir);
        sprintf(interleaveFileName,"%s/ICRR/TestBed/interleaveFile.txt",calibDir);
    }
    if(stationId==ARA_STATION1&&gotIcrrCalibFile[1]==0){
        sprintf(binWidthFileName,"%s/ICRR/Station1/binWidths.txt",calibDir);
        sprintf(epsilonFileName,"%s/ICRR/Station1/epsilonFile.txt",calibDir);
        sprintf(interleaveFileName,"%s/ICRR/Station1/interleaveFile.txt",calibDir);
    }
    // load the calibration files

    std::ifstream binWidthFile(binWidthFileName);
    std::ifstream epsilonFile(epsilonFileName);
    std::ifstream interleaveFile(interleaveFileName);
    int chip, rco, chan;
    double width, epsilon, interleave;
    while(binWidthFile >> chip >> rco){
        for(int samp=0; samp<MAX_NUMBER_SAMPLES_LAB3;++samp){
            binWidthFile >> width;
            binWidths[stationId][chip][rco][samp]=width;
        }
    }
    binWidthFile.close();
    while(epsilonFile >> chip >> rco >> epsilon){
        epsilonVals[stationId][chip][rco]=epsilon;
    }
    epsilonFile.close();
    while(interleaveFile >> chip >> chan >> interleave){
        interleaveVals[stationId][chan+4*chip]=interleave;
    }
    interleaveFile.close();
    
    //Set the gotIcrrCalibFile flags
    if(stationId==ARA_TESTBED){
        gotIcrrCalibFile[0]=1;
        gotIcrrCalibFile[1]=0;
    }
    if(stationId==ARA_STATION1){
        gotIcrrCalibFile[0]=0;
        gotIcrrCalibFile[1]=1;
    }
}


void AraEventCalibrator::calcClockAlignVals(UsefulIcrrStationEvent *theEvent, AraCalType::AraCalType_t calType)
{
    AraStationId_t stationId=theEvent->stationId;

    if(!AraCalType::hasClockAlignment(calType)) return;
    TGraph *grClock[LAB3_PER_ICRR]={0};
    Double_t lag[LAB3_PER_ICRR]={0};
    for(int chip=0;chip<LAB3_PER_ICRR;chip++) {
        clockAlignVals[stationId][chip]=0;    
        int chanIndex=ICRR1_CLOCK_CHANNEL+CHANNELS_PER_LAB3*chip; 
        grClock[chip]=theEvent->getGraphFromElecChan(chanIndex);
        lag[chip]=estimateClockLag(grClock[chip]);
        clockLagVals[stationId][chip]=lag[chip];
        delete grClock[chip];

        if(chip>0) {
            // Then can actually do some alignment
            clockAlignVals[stationId][chip]=lag[0]-lag[chip];
            // The below fudge factors were "tuned" using pulser data 
            // to try and remove period ambiguities resulting from wrong cycle lag
            // jpd -- updated the fudge factors to remove 1e-3 failures in 
            if(lag[chip]<8.5 && lag[0]>8.5) 
                clockAlignVals[stationId][chip]-=25;
            if(lag[chip]>8.5 && lag[0]<8.5) 
                clockAlignVals[stationId][chip]+=25;
            // std::cout << "clockAlignVals[ " << chip << "] = " << clockAlignVals[chip] << "\n";
        }
    }
}


Double_t AraEventCalibrator::estimateClockLag(TGraph *grClock)
{
    // This funciton estimates the clock lag (i.e. phase but expressed in terms of a deltaT between 0 and 1 period) by just using all the negative-positive zero crossing

    Double_t period=ICRR1_CLOCK_PERIOD;
    Double_t mean=grClock->GetMean(2);
    Int_t numPoints=grClock->GetN();
    if(numPoints<3)
        return 0;
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
            // std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
            zc[countZC]=(((0-y1)/(y2-y1))*(x2-x1))+x1;
            rawZc[countZC]=zc[countZC];
            countZC++;
            // if(countZC==1)
            // break;
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
            // std::cout << i << "\t" << zc[i] << "\t" << rawZc[i] << "\n";     
    }
    meanZC/=countZC;
    meanZC2/=countZC;
    // Double_t rms=TMath::Sqrt(meanZC2-meanZC*meanZC);
    // std::cout << meanZC << "\t" << rms << "\n";
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
            if(TMath::Abs(periodTest)<0.5 && rmsTest>0){
                rcoGuess[chip]=1;
            }
        }    
        else {
            rcoGuess[chip]=1;
            if(TMath::Abs(periodTest)<0.5 && rmsTest<0) {
                rcoGuess[chip]=0;   
            }
        }
        if(rms[rcoGuess[chip]]>4){
            rcoGuess[chip]=theEvent->getRawRCO(chanIndex);
        }
        // std::cout << "AraEventCalibrator:\t" << period[0] << "\t" << period[1] << "\n";
        // std::cout << "AraEventCalibrator:\t" << periodTest << "\t" << rmsTest << "\t" << rcoGuess[chip] << "\n";
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
            // std::cout << i << "\t" << y2 << "\t" << y1 << "\t" << (y2-y1) << "\n";
            Double_t zcTime=(((0-y1)/(y2-y1))*(x2-x1))+x1;
            if(countZC>0) {
                if((zcTime-zc[countZC-1])<10)
                    continue;
            }
            zc[countZC]=zcTime;
            countZC++;
            // if(countZC==1)
            // break;
        }    
    }

    if(countZC<2)
        return 0;
    
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
    // fprintf(stderr, "begin calibrating event\n");//FIXME

    //! Modulates calibration step. MK added 27-09-2021
    //! 1st step. Configures basic information (station id, unixtime) and arrays (time,volt, sample index, etc.)
    AraStationId_t thisStationId = theEvent->stationId;
    Double_t unixtime = theEvent->unixTime; ///< The unixtime line was added by UAL 01/26/2019.
    std::map< Int_t, std::vector <Double_t> >::iterator timeMapIt;
    std::map< Int_t, std::vector <Double_t> >::iterator voltMapIt;
    std::vector<std::vector<int> > *sampleList = new std::vector<std::vector<int> >(CHANNELS_PER_ATRI, std::vector<int>(0)); ///< pointer for WF sample numbers
    std::vector<std::vector<int> > *capArrayList = new std::vector<std::vector<int> >(DDA_PER_ATRI, std::vector<int>(0)); ///< pointer for 'block number' modulo 2
    Bool_t hasTrimFirstBlk = false;
    Bool_t hasTimingCalib = false; 

    //! 2nd step. Loads Tables (Pedestal, Conversion factor, Sample timing) 
    Int_t calibIndex = AraGeomTool::getStationCalibIndex(thisStationId);
    //! RJN debug
    // std::cout << "Station Id fun: " << (int)thisStationId << "\t" << calibIndex << "\n";
    //! Only load the pedestals / calib if they are not already loaded
    if(fGotAtriCalibFile[calibIndex]==0){
        loadAtriCalib(thisStationId, unixtime); ///< Adds unix time to select the new timing table for A3 2019 data. MK added 08-02-2022
    }
    if(fGotAtriPedFile[calibIndex]==0){
        loadAtriPedestals(thisStationId);
    }

    //! 3rd step. Converts DAQ data format to Electronic channel format
    UnpackDAQFormatToElecChanFormat(theEvent, voltMapIt, timeMapIt, sampleList, capArrayList);

    /*! 
	4th step. Common mode
	This function is subtracting DDA ch5 value from DDA ch1~4
        But the purpose of this function is unknown. And the return of the hasCommonMode (calType) is always kFalse
	Since it will only work when the number of samples between DDA ch 1~4 and ch 5 is the same,
	It is placed before TrimFirstBlock() and TimingCalibrationAndBadSampleReomval()
    */
    if(hasCommonMode(calType)) {
        CommonMode(theEvent, voltMapIt);
    }

    /*!
        From here, each function is independent. So, You can call the functiions at any step
        As a default, prioritizing the functions that reduce the number of samples
    */

    //! 5th step. Erase first block that currupted by trigger
    //! Apply conditioner function here
    if(hasTrimFirstBlock(calType)) {
        hasTrimFirstBlk = TrimFirstBlock(theEvent, voltMapIt, timeMapIt, sampleList, capArrayList, hasTimingCalib);
    }

    //! 6th step. Timing calibration and bad sample removal
    //! This step calibrates the time of each sample and only selecting the samples that have good performance
    if(hasBinWidthCalib(calType)){ 
        hasTimingCalib = TimingCalibrationAndBadSampleReomval(theEvent, voltMapIt, timeMapIt, sampleList, capArrayList, hasTrimFirstBlk);    
    }
    
    //! 7th step. Pedestal subtraction
    if(hasPedestalSubtraction(calType)) {
        PedestalSubtraction(theEvent, voltMapIt, sampleList, calType);
    }
    
    /*!
        8th step. Zeroing ADC WF by subtracting mean
        If 1st block is still in the WF, exclude the samples in the 1st block from mean calculation

        Previous updates 
        RJN change 13-Feb-2013
        Now do zero mean,   
        THM added 12-Feb-2014
        After zeroMean (and only then!!) do voltage calibration,
        KAH added 09-15-2020
        first, need to do voltage calibration. if you zero-mean the block beforehand, the ADC values get screwed up and the resulting waveform looks bad.
        each sample needs to be individually calibrated. This requires knowing the dda, the channel, the block and the sample%64. 
        The below code loops over each of these.
        the previous version of the code did the zero mean before the votlage calibration. If there are issues with A2 or A3 calibration, try switching the order back.

        Latest updates
        MK added 27-09-2021
        Adds back just for A2 and A3. Dealing with outlier events presented in this talk:
        https://aradocs.wipac.wisc.edu/cgi-bin/DocDB/ShowDocument?docid=2464 (slide No. 6~7)
        Some of the WFs have ADC values that are far from the expected ADC values from thermal noise
        Even though we use the repeder pedestal, these WFs are not nicely centering in zero
        The best solution we can have for now is applying ApplyZeroMean() before the conversion
        In the future, we might need to apply some sort of filtering method to exclude these outlier events
        
        I'm not sure A5 has this kind of outlier event. so, I preserve original condition that not applying 'Zero meaning before conversion' just for A5
        In the future, we might need to check whether A5 also has outlier event or not
            
        Latest update :: 6th Nov 2022, by Paramita Dasgupta(PDG)
        In A4, applied calibration to each sample and did nt do zero-mean before voltage calibration ( similar to ARA5. So, adding  && thisStationId != 4 to the following condition so that we "DO NOT" zero mean before Voltage calibration. In the future, we might need to check if A4 also has outlier event or not, but for now, we are not zero-meaning before v calib.
    */
    if(hasADCZeroMean(calType) && thisStationId != 5 && thisStationId != 4) {
        ApplyZeroMean(theEvent, voltMapIt, capArrayList, hasTrimFirstBlk, hasTimingCalib);
    }

    //! 9th step. Voltage calibration
    if(hasVoltCal(calType)) {
        VoltageCalibration(theEvent, voltMapIt, sampleList, thisStationId);
    }
   
    /*! 
        10th step. Zeroing voltage WF by subtracting mean
        If 1st block is still in the WF, exclude the samples in the 1st block from mean calculation
        
        Even though WF is centering in zero before ADC conversion, sometimes mean of voltage WF has an offset from zero
        Example is in this talk: https://aradocs.wipac.wisc.edu/cgi-bin/DocDB/ShowDocument?docid=2464 (slide No.9)
        It is mainly haeepning in A3 data. 2nd ApplyZeroMean() is required for this offset
        In the future, we might need to perform recalibration to get a better conversion factor
    */
    if(hasVoltZeroMean(calType)) {
        ApplyZeroMean(theEvent, voltMapIt, capArrayList, hasTrimFirstBlk, hasTimingCalib);
    }

    //! 11th step. Inverts only RF channels = 0,4,8 in A3
    //! Apply conditioner function here
    if (hasInvertA3Chans(calType) && thisStationId ==3) {
        InvertA3Chans(theEvent, voltMapIt, thisStationId);
    }

    //! 12th step. Remove knwon cable delay
    //! jpd change 25-03-13
    //! now subtract off the cable delays
    if(hasCableDelays(calType)){
        ApplyCableDelay(theEvent, timeMapIt, unixtime, thisStationId);
    }
    delete sampleList, capArrayList; ///< delete the pointer

    // fprintf(stderr, "AraEventCalibrator::CalibrateEvent() -- finished calibrating event\n");//DEBUG                        
}

//! Converts DAQ data format to Electronic channel format
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \param timeMapIt the iterator referring to the time elements in the 2d map container
    \param sampleList the pointer of WF sample numbers
    \param capArrayList the pointer of 'block number' modulo 2
    \return void
*/
void AraEventCalibrator::UnpackDAQFormatToElecChanFormat(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, std::vector<std::vector<int> > *sampleList, std::vector<std::vector<int> > *capArrayList)
{

    int samples_per_block = SAMPLES_PER_BLOCK;

    std::vector<RawAtriStationBlock>::iterator blockIt;
    std::vector< std::vector<UShort_t> >::iterator vecVecIt;
    std::vector< UShort_t >::iterator shortIt;

    //! Step one is loop over the blocks 
    for(blockIt = theEvent->blockVec.begin();
            blockIt!=theEvent->blockVec.end();
            blockIt++) {
        //! Step two is determine the channel Ids
        Int_t irsChan[8];
        Int_t numChans=0;
        for(Int_t bit=0;bit<8;bit++) {
            Int_t mask=(1<<bit);
            if((blockIt->channelMask)&mask) {
                irsChan[numChans]=bit;
                numChans++;
            }
        }
        // std::cout << "Got numChans " << numChans << "\n";
        Int_t dda=blockIt->getDda();
        Int_t block=blockIt->getBlock();  ///< This is a number between 0 and 511 and is the storage block
        Int_t capArray=blockIt->getCapArray();
        capArrayList->at(dda).push_back(capArray);

        //! Step three is loop over the channels within a block
        Int_t uptoChan=0;
        for(vecVecIt=blockIt->data.begin();
            vecVecIt!=blockIt->data.end();
            vecVecIt++) {
            Int_t chanId=irsChan[uptoChan] | ((blockIt->channelMask&0x300)>>5);
            Int_t chan=irsChan[uptoChan];

            // std::cout << "Got chanId " << chanId << "\t" << irsChan[uptoChan] << "\t";
            // << dda << "\t" << block << "\t" << RawAtriStationEvent::getPedIndex(dda,block,chan,0) << "\n";
            uptoChan++;

            //! Step four is to check if we have already got this chanId
            timeMapIt=theEvent->fTimes.find(chanId);
            Double_t time=0;
            Int_t firstTime=1;
            if(timeMapIt==theEvent->fTimes.end()) {
                //! First time round for this channel
                std::vector <Double_t> tempTimes;
                std::vector <Double_t> tempVolts;
                //! Now need to insert empty vector into map
                theEvent->fTimes.insert( std::pair< Int_t, std::vector <Double_t> >(chanId,tempTimes));
                theEvent->fVolts.insert( std::pair< Int_t, std::vector <Double_t> >(chanId,tempVolts));
                theEvent->fNumChannels++;
            }
            else {
                //! Just get the last time
                time=timeMapIt->second.back();
                // if(dda==1 &&chan==1)
                // std::cout << "Last time " << time << "\t" << timeMapIt->second.size() << "\n";
                firstTime=0;
            }
   
            //! Set the iterators to point to the correct channel in the map
            timeMapIt=theEvent->fTimes.find(chanId);
            voltMapIt=theEvent->fVolts.find(chanId);

            int samp=0;
            //! Now loop over the 64 samples
            for(shortIt=vecVecIt->begin();
                shortIt!=vecVecIt->end();
                shortIt++){

                time+=NSPERSAMP_ATRI;
                timeMapIt->second.push_back(time); ///< Filling with time
                voltMapIt->second.push_back((*shortIt)); ///< Filling with volts
                sampleList->at(chanId).push_back(block * samples_per_block + samp); ///< Filling with sample number. It is needed for pedestal subtraction and voltage calibration
                samp++;

            }
        }
    }
}

/*! 
    Timing calibration and bad sample removal
    This step calibrates the time of each sample and only selecting the samples that have good performance
    fAtriSampleIndex and fAtriSampleTimes stores the sample numbers and times that have good performance
    Based on the sample numbers in the tables, this step will remove bad samples from WF
    The sampleList that contains the sample number of WF will be updated by the good performance sample numbers 
    This sampleList will be used for calling the right pedestal subtraction samples and the conversion factor samples 
*/
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \param timeMapIt the iterator referring to the time elements in the 2d map container
    \param sampleList the pointer of WF sample numbers
    \param capArrayList the pointer of 'block number' modulo 2
    \param hasTrimFirstBlk boolean statement that whether first block trimming is already performed or not
    \return boolean true or false
*/
Bool_t AraEventCalibrator::TimingCalibrationAndBadSampleReomval(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, std::vector<std::vector<int> > *sampleList, std::vector<std::vector<int> > *capArrayList, Bool_t hasTrimFirstBlk)
{
 
    int capArrayNumber = 0;
    int samples_per_block = SAMPLES_PER_BLOCK;

    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda; ///< make electronic channel number
            voltMapIt=theEvent->fVolts.find(chanId);
            timeMapIt=theEvent->fTimes.find(chanId);
            if(voltMapIt!=theEvent->fVolts.end()) {
                Int_t numPoints=(voltMapIt->second).size();
               
                //! copy the voltage and sample index in temperary array and clear them before filling with calibrated values
                std::vector<double> tempVolts;
                tempVolts = voltMapIt->second;
                voltMapIt->second.clear();
                timeMapIt->second.clear();
                std::vector<int> tempSamps;
                tempSamps = sampleList->at(chanId);
                sampleList->at(chanId).clear();
           
                int numBlock = int(numPoints/samples_per_block); 
                for(int blk=0; blk<numBlock; blk++){
                    capArrayNumber = capArrayList->at(dda)[blk];
 
                    //! array that stores trimmed values based on the sample indexes in the fAtriSampleIndex table
                    int voltIndex[samples_per_block];
                    Double_t tempTimes[samples_per_block];

                    for (int samp=0; samp<samples_per_block; samp++){
                        //! Select index and time of well calibrated samples from the tables
                        voltIndex[samp] = fAtriSampleIndex[dda][chan][capArrayNumber][samp] + blk * samples_per_block;
                        tempTimes[samp] = (blk + hasTrimFirstBlk) * 20.0 + fAtriSampleTimes[dda][chan][capArrayNumber][samp] - 20.0*capArrayNumber; ///< hasTrimFirstBlk will take into account whether first block is trimmed or not
                    }   
         
                    Int_t numSamples=fAtriNumSamples[dda][chan][capArrayNumber]; ///< number of samples in each block after timing calibration
                    for (int trim=0; trim<numSamples; trim++){
                        timeMapIt->second.push_back(tempTimes[trim]); ///< Filling with time
                        voltMapIt->second.push_back(tempVolts[voltIndex[trim]]); ///< Filling with volt
                        sampleList->at(chanId).push_back(tempSamps[voltIndex[trim]]); ///< Filling with sample index
                    }
                }
            } 
        }
    }
    return true;
}

//! Pedestal subtraction
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \param sampleList the pointer of WF sample numbers
*/
void AraEventCalibrator::PedestalSubtraction(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::vector<std::vector<int> > *sampleList, AraCalType::AraCalType_t calType)
{

    int sampleIndex, sampleNumber, blockIndex = 0; ///< capacitor sample index, block sample index, capacitor block index
    int samples_per_block = SAMPLES_PER_BLOCK;

    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda; ///< make electronic channel number
            voltMapIt=theEvent->fVolts.find(chanId);
            if(voltMapIt!=theEvent->fVolts.end()) {
                Int_t numPoints=(voltMapIt->second).size();
                if(calType==AraCalType::kOnlyPed
                    || calType==AraCalType::kOnlyPedWithOut1stBlock
                    || calType==AraCalType::kOnlyPedWithOut1stBlockAndBadSamples)
                { voltMapIt->second.clear(); } ///< clear voltMapIt iterator before filling with pedestal values

                for(int samp=0;samp<numPoints;samp++) {
                    sampleIndex = sampleList->at(chanId)[samp];
                    sampleNumber = sampleIndex%samples_per_block;
                    blockIndex = int(sampleIndex/samples_per_block);
                    //! Filling with the pedestal values for the corresponding raw WF, 19-12-2021 -MK-
                    if(calType==AraCalType::kOnlyPed 
                        || calType==AraCalType::kOnlyPedWithOut1stBlock 
                        || calType==AraCalType::kOnlyPedWithOut1stBlockAndBadSamples) 
                    { voltMapIt->second.push_back((Int_t)fAtriPeds[RawAtriStationEvent::getPedIndex(dda,blockIndex,chan,sampleNumber)]);
                    //! Filling with ADC-Pedestal. Iunputted pedestal will be stored in fAtriPeds table 
                    } else { voltMapIt->second[samp] -= (Int_t)fAtriPeds[RawAtriStationEvent::getPedIndex(dda,blockIndex,chan,sampleNumber)]; }
                }
            }
        }
    }
}

//! Voltage calibration. Converts ADC to voltage sample by sample
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \param stationId id of the station
    \return void
*/
void AraEventCalibrator::VoltageCalibration(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::vector<std::vector<int> > *sampleList, AraStationId_t thisStationId)
{
    int blockIndex = 0;///< This is now filled in the code below- KAH
    int sampleIndex, sampleNumber = 0; ///< removed this and instead use "samp" variable below. re-use this for the trimmed sample index -MK-
    int samples_per_block = SAMPLES_PER_BLOCK;

    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda;
            voltMapIt=theEvent->fVolts.find(chanId);
            if(voltMapIt!=theEvent->fVolts.end()) {
                Int_t numPoints=(voltMapIt->second).size();
                for(int samp=0;samp<numPoints;samp++) {
                    sampleIndex = sampleList->at(chanId)[samp]; ///< capacitor sample index
                    sampleNumber = sampleIndex%samples_per_block; ///< block sample index
                    blockIndex = int(sampleIndex/samples_per_block); ///< capacitor block index
                    //! Move to conversion function
                    //! Apply conversion parameter on each sample
                    voltMapIt->second[samp] = convertADCtoMilliVolts( voltMapIt->second[samp], dda, blockIndex, chan, sampleNumber, thisStationId);
                }
            }
        }
    }
}

//! Inverts only RF channels = 0,4,8 in A3
//! Apply Brian's conditioner inside of calibration
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \param stationId id of the station
    \return void
*/
void AraEventCalibrator::InvertA3Chans(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, AraStationId_t thisStationId)
{
    
    std::vector<Int_t> list_to_invert;
    list_to_invert.push_back(0);
    list_to_invert.push_back(4);
    list_to_invert.push_back(8);

    //! loop over that list and invert them (multiply by -1)
    for(int i=0; i<list_to_invert.size(); i++){
        //! get the elec chan
        Int_t rf_chan = list_to_invert[i];
        Int_t elec_chan = AraGeomTool::Instance()->getElecChanFromRFChan(rf_chan, thisStationId);
 
        voltMapIt=theEvent->fVolts.find(elec_chan);
        if(voltMapIt!=theEvent->fVolts.end()) {
            Int_t numPoints=(voltMapIt->second).size();
           
            //! perform inversion on every sample
            for(int samp=0;samp<numPoints;samp++) {
                voltMapIt->second[samp]*=-1.;
            }
        }
    }
}

//! Remove knwon cable delay
/*!
    \param theEvent the useful atri event pointer
    \param timeMapIt the iterator referring to the time elements in the 2d map container
    \param stationId id of the station
    \return void
*/
void AraEventCalibrator::ApplyCableDelay(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, Double_t unixtime, AraStationId_t thisStationId)
{
    
    for(int rfChan=0;rfChan<ANTS_PER_ATRI;rfChan++){
        AraGeomTool* tempGeom = AraGeomTool::Instance();
        tempGeom->LoadSQLDbAtri(unixtime,thisStationId); ///< LoadSQLDbAtri() added by UAL 01/25/2019
        Double_t delay=tempGeom->getStationInfo(thisStationId)->getCableDelay(rfChan);
        int chanId = tempGeom->getElecChanFromRFChan(rfChan, thisStationId);
        timeMapIt=theEvent->fTimes.find(chanId);
        if(timeMapIt!=theEvent->fTimes.end()) {
            Int_t numPoints = (timeMapIt->second).size();
            for(int samp=0;samp<numPoints;samp++){
                timeMapIt->second[samp]-=delay;
            }
        }
    }
}

//! Erase first block that currupted by trigger
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \param timeMapIt the iterator referring to the time elements in the 2d map container
    \param sampleList the pointer of WF sample number
    \param capArrayList the pointer of  'block number' modulo 2
    \param hasTimingCalib boolean statement that whether TimingCalibrationAndBadSampleReomval is already performed or not
    \return boolean true or false
*/
Bool_t AraEventCalibrator::TrimFirstBlock(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::map< Int_t, std::vector <Double_t> >::iterator &timeMapIt, std::vector<std::vector<int> > *sampleList, std::vector<std::vector<int> > *capArrayList, Bool_t hasTimingCalib)
{

    int first_block_len = 0;
    int first_capNumber = 0;

    //! check if number of total samples are smaller than number of samples in the first block
    bool hasTooFewBlocks=false;
    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        first_capNumber = capArrayList->at(dda)[0];
        for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda;
            voltMapIt=theEvent->fVolts.find(chanId);
            if(voltMapIt!=theEvent->fVolts.end()) {
                Int_t numPoints=(voltMapIt->second).size();
                if (hasTimingCalib){
                    first_block_len = fAtriNumSamples[dda][chan][first_capNumber]; ///< use the exact number of samples in the first if timing calibration has already happened
                } else {
                    first_block_len = SAMPLES_PER_BLOCK;
                }
                if (numPoints < first_block_len){
                    hasTooFewBlocks=true;
                    break;
                }
            }
        }
    }
    /*!
        returning '!hasTooFewBlocks' for let TimingCalibrationAndBadSampleReomval() know the block is trimmed or not.
        fAtriSampleTimes table doesn't know block is trimmed unless this return
    */
    if(hasTooFewBlocks) return !hasTooFewBlocks;

    //! if number of samples in the WF is bigger than first block, perform first block trimming
    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        first_capNumber = capArrayList->at(dda)[0];
        capArrayList->at(dda).erase(capArrayList->at(dda).begin(),capArrayList->at(dda).begin()+1); ///< delete the first block number. This trimming is needed for TimingCalibrationAndBadSampleReomval()
        for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda;
            voltMapIt=theEvent->fVolts.find(chanId);
            timeMapIt=theEvent->fTimes.find(chanId);
            if(voltMapIt!=theEvent->fVolts.end()) {
                if (hasTimingCalib){
                    first_block_len = fAtriNumSamples[dda][chan][first_capNumber]; ///< use the exact number of samples in the first if timing calibration has already happened
                } else {
                    first_block_len = SAMPLES_PER_BLOCK;
                }
                voltMapIt->second.erase(voltMapIt->second.begin(),voltMapIt->second.begin()+first_block_len); ///< delete the times in the first block
                timeMapIt->second.erase(timeMapIt->second.begin(),timeMapIt->second.begin()+first_block_len); ///< delete the ADC (or volt) in the first block
                sampleList->at(chanId).erase(sampleList->at(chanId).begin(),sampleList->at(chanId).begin()+first_block_len); ///< delete the samples in the first block
            }
        }
    } return !hasTooFewBlocks;
}

/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \return void
*/
void AraEventCalibrator::CommonMode(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt)
{
    /*!
        Then we need to do a common mode correction
        loop over dda
        loop over chan
        loop over times and subtract one
    */
    std::map< Int_t, std::vector <Double_t> >::iterator voltMapIt2;

    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        for(Int_t chan=0;chan<5;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda;
            voltMapIt= theEvent->fVolts.find(chanId);
            Int_t chanId2=5+RFCHAN_PER_DDA*dda;
            voltMapIt2= theEvent->fVolts.find(chanId2);
            Int_t numPoints=(voltMapIt->second).size();
            for(int samp=0;samp<numPoints;samp++) {
                voltMapIt->second[samp]-= voltMapIt2->second[samp];
            }
        }
    }
}

/*! 
    Make the mean of the voltage samples
    Apply Brian's conditioner inside of calibration
    If 1st block is still in the WF, exclude the samples in the 1st block from mean calculation
*/
/*!
    \param theEvent the useful atri event pointer
    \param voltMapIt the iterator referring to the voltage elements in the 2d map container
    \return void
*/
void AraEventCalibrator::ApplyZeroMean(UsefulAtriStationEvent *theEvent, std::map< Int_t, std::vector <Double_t> >::iterator &voltMapIt, std::vector<std::vector<int> > *capArrayList, Bool_t hasTrimFirstBlk, Bool_t hasTimingCalib)
{
    int first_block_len = 0;
    int numPoints_for_mean = 0;
    int first_capNumber = 0;
    int samples_per_block = SAMPLES_PER_BLOCK;

    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        first_capNumber = capArrayList->at(dda)[0];
        for(Int_t chan=0;chan<RFCHAN_PER_DDA;chan++) {
            Int_t chanId=chan+RFCHAN_PER_DDA*dda;
            voltMapIt=theEvent->fVolts.find(chanId);
            if(voltMapIt!=theEvent->fVolts.end()) {
                Int_t numPoints=(voltMapIt->second).size();
                if (!hasTrimFirstBlk) {
                    if (hasTimingCalib) {
                        first_block_len = fAtriNumSamples[dda][chan][first_capNumber]; ///< use the exact number of samples in the first if timing calibration has already happened
                    } else { 
                        first_block_len = samples_per_block;
                    }
                    numPoints_for_mean = numPoints - first_block_len;
                } else {
                    numPoints_for_mean = numPoints;
                    first_block_len = 0;
                }
                //! compute the mean, and let C++ help by doing the addition for us
                //! If 1st block is still in the WF, exclude the samples in the 1st block from mean calculation
                Double_t mean = std::accumulate((voltMapIt->second).begin() + first_block_len, (voltMapIt->second).end(), 0.0);
                mean /= numPoints_for_mean;
                for(int samp=0;samp<numPoints;samp++) {
                    voltMapIt->second[samp]-=mean;
                }
            }
        }
    }
}

void AraEventCalibrator::setAtriPedFile(char *filename, AraStationId_t stationId)
{
    Int_t calibIndex = AraGeomTool::getStationCalibIndex(stationId);
    strncpy(fAtriPedFile[calibIndex],filename,FILENAME_MAX);
    fGotAtriPedFile[calibIndex]=1; //Protects us from loading the default pedfile
    loadAtriPedestals(stationId);
}

void AraEventCalibrator::loadAtriPedestals(AraStationId_t stationId)
{  
    Int_t calibIndex = AraGeomTool::getStationCalibIndex(stationId);
    Int_t stationNumber = AraGeomTool::getStationNumber(stationId);
    if(calibIndex==-1){
        fprintf(stderr, "AraEventCalibrator::loadAtriPedestals -- ERROR Unknown stationId %i\n", stationId);
        exit(0);
    }
    if(fGotAtriPedFile[calibIndex]==1){
        if(!fileExists(fAtriPedFile[calibIndex])){
            fGotAtriPedFile[calibIndex]=0;
            fprintf(stderr, "%s -- pedFile does not exist!\n", __FUNCTION__);
        }
        else if(numberOfPedestalValsInFile(fAtriPedFile[calibIndex]) != RFCHAN_PER_DDA*DDA_PER_ATRI*BLOCKS_PER_DDA*SAMPLES_PER_BLOCK){
            fGotAtriPedFile[calibIndex]=0;
            fprintf(stderr, "%s -- pedFile has too few values!\n", __FUNCTION__);
        }
    }

    if(fGotAtriPedFile[calibIndex]==0){
        char *pedFileEnv = getenv( "ARA_ATRI_PEDESTAL_FILE" );
        if ( pedFileEnv == NULL ) {
            char calibDir[FILENAME_MAX];
            char *calibEnv=getenv("ARA_CALIB_DIR");
            if(!calibEnv) {
                char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
                if(!utilEnv) {
                    sprintf(calibDir,"calib");
                    // fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ./calib]");
                } else {
                    sprintf(calibDir,"%s/share/araCalib",utilEnv);
                    // fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ARA_UTIL_INSTALL_DIR/share/calib]");
                }
            }
            else {
                strncpy(calibDir,calibEnv,FILENAME_MAX);
                // fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ARA_CALIB_DIR]");
            }
            sprintf(fAtriPedFile[calibIndex],"%s/ATRI/araAtriStation%iPedestals.txt",calibDir, stationId);
            // fprintf(stdout," = %s\n",fAtriPedFile[calibIndex]);
        } // end of IF-block for pedestal file not specified by environment variable
        else {
            strncpy(fAtriPedFile[calibIndex],pedFileEnv,FILENAME_MAX);
            // fprintf(stdout,"AraEventCalibrator::loadAtriPedestals(): INFO - Pedestal file [from ARA_ONE_PEDESTAL_FILE] = %s\n",fAtriPedFile[calibIndex]);
        } // end of IF-block for pedestal file specified by environment variable
    }
    
    // Pedestal file
    
    fprintf(stdout, "%s : Loading fAtriPedFile - %s\n", __FUNCTION__, fAtriPedFile[calibIndex]);
    
    // If there are pedestals in memory already then we need to delete them and replace with this station's
    if(fAtriPeds)
        delete fAtriPeds;

    fAtriPeds = new UShort_t [DDA_PER_ATRI*BLOCKS_PER_DDA*RFCHAN_PER_DDA*SAMPLES_PER_BLOCK];
    if(!fAtriPeds) {
        std::cerr << "Can not allocate memory for pedestal file\n";
        exit(0);
    }

    // now, we open and load the pedestal files
    gzFile inPed = gzopen(fAtriPedFile[calibIndex], "r");
    std::vector<char> buffer(6000000);
    int nRead = gzread(inPed, &buffer[0], sizeof(char)*6000000);

    // we then put the buffer (which are characters at this point)
    // into a string object that we can manipulate
    std::string string_buffer(buffer.begin(), buffer.end()); // convert the vector of chars to a string    
    std::stringstream ss(string_buffer);

    // to do this, we need "buffer" variables
    std::string dda_buf;
    std::string block_buf;
    std::string chan_buf;
    std::string ped_buf;
   
    while ( ss >> dda_buf >> block_buf >> chan_buf){
        // we cast the dda, block, and channels into integers
        int dda = std::stoi(dda_buf);
        int block = std::stoi(block_buf);
        int chan = std::stoi(chan_buf);

        // the pedestal values are cast as shorts
        for(int samp=0; samp < SAMPLES_PER_BLOCK; samp++){
            ss >> ped_buf;
            short pedVal = short(std::stoi(ped_buf));
            fAtriPeds[RawAtriStationEvent::getPedIndex(dda,block,chan,samp)]=pedVal;
        }
    }
    gzclose(inPed);
    
    // Now we set the gotPedFile flags to indicate which station we have in memory
    
    // for(AraStationId_t station=0;station<ICRR_NO_STATIONS+ATRI_NO_STATIONS;station++){
    //   if(AraGeomTool::isAtriStation(station)){
    //     calibIndex=AraGeomTool::getStationCalibIndex(station);
    //     if(station==stationId) fGotAtriPedFile[calibIndex]=1;
    //     else fGotAtriPedFile[calibIndex]=0;
    //   }
    // }

    // RJN change 13-Feb-2013
    memset(fGotAtriPedFile,ATRI_NO_STATIONS*sizeof(Int_t),0);
    calibIndex = AraGeomTool::getStationCalibIndex(stationId);
    fGotAtriPedFile[calibIndex]=1;
}


void AraEventCalibrator::loadAtriCalib(AraStationId_t stationId, Double_t unixtime)
{
    Int_t calibIndex = AraGeomTool::getStationCalibIndex(stationId);
    // std::cout << "Loading calibration info for station: " << (int)stationId << "\t" << calibIndex << "\n";
    if(calibIndex==-1){
        fprintf(stderr, "AraEventCalibrator::loadAtriCalib -- ERROR Unknown stationId %i\n", stationId);
        return;
    }

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


    // Reading the reference VadjValues from file (only for ARA02). -THM-
    int VadjRef[DDA_PER_ATRI]={17562, 17765, 17834, 17526};//These default values are from ARA02. Not to be applied to any other station.-THM-
    int currentVadj[DDA_PER_ATRI]={17795, 17725, 17602, 17676};//These default values are from ARA02. Not to be applied to any other station.-THM-
    if(stationId==2){
        char VadjRefFile[200];
        sprintf(VadjRefFile,"%s/ATRI/araAtriStation%iVadjRef.txt", calibDir, stationId );
        fprintf(stdout, "AraEventCalibrator::loadAtriCalib(): INFO - Vadj reference file = %s\n", VadjRefFile);//DEBUG
        std::ifstream VadjFile(VadjRefFile);
        if(VadjFile.is_open()) {
            while(VadjFile >> dda){
                VadjFile >> VadjRef[dda];
            }
            VadjFile.close();
        }
        else {
            std::cerr << "Can not open: " << VadjRefFile << "\n";
            abort();
        }
    } //end modification -THM-

    // sprintf(calibFile,"%s/ATRI/araAtriStation%iSampleTiming.txt",calibDir, stationId);
    /*!    
        MK added 08-02-2022
        WFs that recorded from A3 string 4 show ADC counts duplication from December 2018
        It looks like every even group of 16 samples is duplicated (overwriting) to an odd group of 16 samples
        In order to remove duplicate samples, a new timing table for the 2019 data set are implemented 
        It will exclude the samples that contain duplicated ADC
        Related talk: https://aradocs.wipac.wisc.edu/cgi-bin/DocDB/ShowDocument?docid=2535
    
        MK added 10-09-2022
        It looks like duplication is disappeared from 2019-2020 pole season
        The new timing table will be only used between Run12866 and Run16481
    */
    if (stationId==3 && unixtime > 1544125405 && unixtime < 1576210568){ ///< use new timing table from A3 Run12866 (2018/12/21) until Run16481 (2019/12/13)
        sprintf(calibFile,"%s/ATRI/araAtriStation%iSampleTimingNew_2019DataSet.txt",calibDir, stationId);
    }
    else {
        sprintf(calibFile,"%s/ATRI/araAtriStation%iSampleTimingNew.txt",calibDir, stationId); // opening the new calibration file -THM-
    }
    fprintf(stdout, "AraEventCalibrator::loadAtriCalib(): INFO - Calibration file = %s\n", calibFile);//DEBUG

    std::ifstream SampleFile(calibFile);
    if(!SampleFile.is_open()) {
        std::cerr << "Can not open: " << calibFile << "\n";
        abort();
    }
    //Initialises to some default values
    for(dda=0;dda<DDA_PER_ATRI;dda++) {
        for(chan=0;chan<RFCHAN_PER_DDA;chan++) {
            for(capArray=0;capArray<2;capArray++) {
                for(sample=0;sample<SAMPLES_PER_BLOCK;sample++) {
                    fAtriSampleTimes[dda][chan][capArray][sample]=sample/3.2;
                    fAtriSampleIndex[dda][chan][capArray][sample]=-1;
                }
                fAtriEpsilonTimes[dda][chan][capArray]=1/3.2;
            }
        }
    }

    Double_t value;
    Int_t index;
    while(SampleFile >> dda >> chan >> capArray){
        SampleFile >> fAtriNumSamples[dda][chan][capArray];
        // std::cerr <<  dda << "\t" << chan << "\t" << capArray << "\t" << fAtriNumSamples[dda][chan][capArray] << "\t";
        for(sample=0;sample<fAtriNumSamples[dda][chan][capArray];sample++) {
            SampleFile >> index;
            fAtriSampleIndex[dda][chan][capArray][sample]=index;
            // std::cerr << fAtriSampleIndex[dda][chan][capArray][sample] << " ";    
        }
        // std::cerr << "\n";
        SampleFile >> dda >> chan >> capArray >> fAtriNumSamples[dda][chan][capArray];
        // std::cerr <<  dda << "\t" << chan << "\t" << capArray << "\t" << fAtriNumSamples[dda][chan][capArray] << "\t";
        for(sample=0;sample<fAtriNumSamples[dda][chan][capArray];sample++) {
            SampleFile >> value;
            // Now the sample times are read and corrected compared to the TSA reference for a change of Vadj. Note that this should be only done for ARA02 so far. 
            // Only for ARA02 we have data about the dependency of the sampling speed on Vadj which look reliable. Maybe we can fix this with future measurements.
            // Current Vadj somehow needs to get here from the eventHk-file. Not sure what the best way is. -THM- 
            if(stationId==2){
                fAtriSampleTimes[dda][chan][capArray][sample]=value * 1.0/(0.9978 - 0.0002*(VadjRef[dda] - currentVadj[dda]));
            }
            else{
                fAtriSampleTimes[dda][chan][capArray][sample]=value;
            } //end modification -THM-
            // std::cerr << fAtriSampleTimes[dda][chan][capArray][sample] << " ";    
        }
        // std::cerr << "\n";
    }
    SampleFile.close();

    // RJN -- Add call to check sample timing
    checkAtriSampleTiming();

    // Read the ADC to volts conversion factors for the range between -400 and 400 ADC counts. -THM-
    int blockNumber;
    double conv;
    char adcToVoltageConvFile[200];
    sprintf(adcToVoltageConvFile,"%s/ATRI/araAtriStation%iadcToVoltsConv.txt", calibDir, stationId );
    fprintf(stdout, "AraEventCalibrator::loadAtriCalib(): INFO - Voltage-calibration file = %s\n", adcToVoltageConvFile);//DEBUG
    std::ifstream ADCConvFile(adcToVoltageConvFile);
    if(ADCConvFile.is_open()) {
         while(ADCConvFile >> dda >> chan >> blockNumber){
            for(sample=0;sample<64;sample++){
                for(int cv=0;cv<9;cv++){
                    ADCConvFile >> conv;
                    fAtriSampleADCVoltsConversion[dda][chan][blockNumber][sample][cv] = conv;
                }
            }
        }
        ADCConvFile.close();
        // end modification -THM-
    }
    else {
        std::cerr << "Can not open: " << adcToVoltageConvFile << "\n";
        abort();
    }

    //Read the ADC to volts conversion factors for the range below -400 and above 400 ADC counts. -THM-
    char highAdcToVoltageConvFile[200];
    sprintf(highAdcToVoltageConvFile,"%s/ATRI/araAtriStation%ihighAdcToVoltsConv.txt", calibDir, stationId );
    fprintf(stdout, "AraEventCalibrator::loadAtriCalib(): INFO - high voltage-calibration file = %s\n", highAdcToVoltageConvFile);//DEBUG
    std::ifstream highADCConvFile(highAdcToVoltageConvFile);
    if(highADCConvFile.is_open()) {
        while(highADCConvFile >> dda >> chan >> blockNumber){
            for(sample=0;sample<64;sample++){
                for(int cv=0;cv<5;cv++){
                    highADCConvFile >> conv;
                    fAtriSampleHighADCVoltsConversion[dda][chan][blockNumber][sample][cv] = conv;
                }
            }
        }
        highADCConvFile.close();
    }
    else {
        std::cerr << "Can not open: " << highAdcToVoltageConvFile << "\n";
        abort();
    }
    // end modification -THM-

    char epsilonFileName[100];
    sprintf(epsilonFileName,"%s/ATRI/araAtriStation%iEpsilon.txt",calibDir, stationId);
    // fprintf(stdout, "AraEventCalibrator::loadAtriCalib(): INFO - Epsilon file = %s\n", epsilonFileName);//DEBUG
    std::ifstream epsilonFile(epsilonFileName);
    if(epsilonFile.is_open()) {
         while(epsilonFile >> dda >> chan >> capArray){
            epsilonFile >> value;
            fAtriEpsilonTimes[dda][chan][capArray]=value;
            // printf("%s : dda %i channel %i capArray %f\n", __FUNCTION__, dda, chan, value);
         }
        epsilonFile.close();
    }
    else {
        std::cerr << "Can not open: " << epsilonFileName << "\n";
        abort();
    }

//   {
//      double time=0;
//      double lastTime=0;
//      int index=0;
//      for(int block=0;block<3;block++) {
//  int thisDda=0;
//  int thisChan=6;
//  int thisCapArray=block%2;
//  if(block!=0) time+=fAtriEpsilonTimes[thisDda][thisChan][thisCapArray];
//  for (int samp=0;samp<fAtriNumSamples[thisDda][thisChan][thisCapArray];samp++) {
//     Double_t sampTime=time+fAtriSampleTimes[thisDda][thisChan][thisCapArray][samp];
//     std::cout << index << "\t" << sampTime << "\t" << sampTime-lastTime << "\n";
//     index++;
//     lastTime=sampTime;
//  }
//  time=lastTime;
//      }
//   }   

    
    //Now we set the gotCalibFile flags to indicate which station we have in memory
    
//   for(AraStationId_t station=0;station<ICRR_NO_STATIONS+ATRI_NO_STATIONS;station++){
//     if(AraGeomTool::isAtriStation(station)){
//       calibIndex=AraGeomTool::getStationCalibIndex(station);
//       if(station==stationId) fGotAtriCalibFile[calibIndex]=1;
//       else fGotAtriCalibFile[calibIndex]=0;
//     }
//   }
    //  fprintf(stderr, "finished loading calib\n");//FIXME



    // RJN change 13-Feb-2013
    memset(fGotAtriCalibFile,ATRI_NO_STATIONS*sizeof(Int_t),0);
    calibIndex = AraGeomTool::getStationCalibIndex(stationId);
    fGotAtriCalibFile[calibIndex]=1;
}

void AraEventCalibrator::checkAtriSampleTiming() {
    for(int dda=0;dda<DDA_PER_ATRI;dda++) {
        for(int chan=0;chan<RFCHAN_PER_DDA;chan++) {
            int madeChange=0;
            do {
                madeChange=0;
                // Need to check if times follow each other or not
                if(fAtriSampleTimes[dda][chan][0][fAtriNumSamples[dda][chan][0]-1]>
                    fAtriSampleTimes[dda][chan][1][0]) {
                    // Need to trim one sample off cap array 0
                    // std::cerr << "Oops calibration issue: dda: " << dda << ", chan: " << chan << ", capArray: " << 0 << ", sample " << fAtriNumSamples[dda][chan][0]-1 << ":" << fAtriSampleTimes[dda][chan][0][fAtriNumSamples[dda][chan][0]-1] << " is after capArray: 1 sample 0: " <<  fAtriSampleTimes[dda][chan][1][0] << "\n";
                    // std::cerr << "Removing one sample from cap array 0\n";
                    fAtriNumSamples[dda][chan][0]--;
                    madeChange=1;
                }
                if(fAtriSampleTimes[dda][chan][1][fAtriNumSamples[dda][chan][1]-1]>
                    (40+fAtriSampleTimes[dda][chan][0][0])) {
                    // Need to trim one sample off cap array 1
                    // std::cerr << "Oops calibration issue: dda: " << dda << ", chan: " << chan << ", capArray: " << 1 << ", sample " << fAtriNumSamples[dda][chan][1]-1 << ": " << fAtriSampleTimes[dda][chan][1][fAtriNumSamples[dda][chan][1]-1] << " is after capArray: 0 sample 0: " <<  40+fAtriSampleTimes[dda][chan][0][0] << "\n";
                    // std::cerr << "Removing one sample from cap array 1\n";
                    fAtriNumSamples[dda][chan][1]--;
                    madeChange=1;
                }
            } while(madeChange);
        }
    }
}


Bool_t AraEventCalibrator::fileExists(char *fileName){
    std::ifstream theFile(fileName);  
    if(!theFile){
        theFile.close();
        return kFALSE;
    }
    else{
        theFile.close();
        return kTRUE;
    }
}


Int_t AraEventCalibrator::numberOfPedestalValsInFile(char *fileName){
    Int_t numPedVals=0;
    gzFile inPed = gzopen(fileName, "r");
    if(inPed){
        std::vector<char> buffer(6000000);
        int nRead = gzread(inPed, &buffer[0], sizeof(char)*6000000);
        std::string string_buffer(buffer.begin(), buffer.end()); // convert the vector of chars to a string
        std::stringstream ss(string_buffer); // and then convert the string to a stringstream

        std::string dummy;
        while( ss >> dummy >> dummy >> dummy){
            for(int samp=0; samp < SAMPLES_PER_BLOCK; samp++){
                ss >> dummy;
                numPedVals++;
            }
        }
    }
    gzclose(inPed);
    return numPedVals;
}

/*! 
    Apply conversion parameter on each ADC sample
    Currently, the way to treat for the loaded conversion table is optimized for just A2/3 and A5 
    And default treatment for the loaded conversion table is following A2/3 optimization
    In the future, If the conversion table for A1/4 has a different number of parameters or need different treatment, It need to be updated 
    Update 6th Non 2022, PDG ( A4 indeed used diff. calibration method. I used linear fit between ADC to voltage. So, p2,p3 are 0.0 for all samples in A4. p0 and p1 are slope and intercept respectively.
    Retaining same conversation table format as A5, only p0=p1=0.0 are set.
*/
/*!
    \param adcCountsIn ADC value from the WF sample 
    \param dda corresponding dda board number of adcCountsIn
    \param block corresponding black number of adcCountsIn
    \param chan corresponding dda channel number of adcCountsIn
    \param sample corresponding sample number of adcCountsIn
    \param stationId id of the station
    \return void
*/
Double_t AraEventCalibrator::convertADCtoMilliVolts(Double_t adcCountsIn, int dda, int block, int chan, int sample, AraStationId_t stationId) ///< -THM-, -MK- imports the station id to optimize conversion for each station
{
    
    Double_t volts = 0.0;
    int samples_per_block = SAMPLES_PER_BLOCK;
    int blocks_per_dda = BLOCKS_PER_DDA;
    int samples_per_dda = SAMPLES_PER_DDA;

    /*!
       There is an offset induced in the pedestal numbers, due to asymmetry of the chip. 
       From calibration files this offset with the given noise will be around 11 ADC counts.
       If it's not station 5, subtract an offset of 11. (THM)
    */
    double adc_offset; 
    int high_adc_limit; ///< Define the high ADC limit here (MK)
    if (stationId != 5 && stationId != 4){
        high_adc_limit = 400;
        adc_offset = -11.0;
    } else if (stationId == 5) {
        high_adc_limit = 500;
        adc_offset = 0.0;
    } else if (stationId == 4) {
        high_adc_limit = 800; // only for ARA 4
        adc_offset = 0.0; // only for ARA 4 
    }
    //! Define neighboring sample or block offset. Based on Thomas's thesis p.69
    int neighboring_index = 2;

    /*! 
        Each conversion parameter in fAtriSampleADCVoltsConversion: pos_fit_x, pos_fit_x^2, pos_fit_x^3, neg_fit_x, neg_fit_x^2, neg_fit_x^3, fit_const, zeroval, chi2
        Each conversion parameter in fAtriSampleHighADCVoltsConversion: pos_fit_const, pos_fit_x, neg_fit_const, neg_fit_x
    */
    //std::cout << chan << ", "<< block<<", " << sample << std::endl;
    //! Only apply calibration on calibrated channels (RF channels)!
    if( (dda==0 && chan<6)||(dda==1 && chan<4)||(dda==2 && chan<4)||(dda==3 && chan<6) ){

        /*! 
            Check if the fit worked out well parameter[8] is the Chi^2/NDF of the fit. Normally it is very good if <1.0.
            For A2/3, If Chi^2/NDF is > 1.0, the conversion factor of the same sample number in a neighboring block, provided it has a better Chi^2/NDF value, will be used. -- Thomas's thesis p.69
            For A5 and A4, the conversion factor of the neighboring sample, provided it has a better Chi^2/NDF value, will be used.
         update by PDG 6th Nov 2022, adding A4 condition here for sample to be considered */ 
        if (stationId != 5 && stationId != 4){
            while(fAtriSampleADCVoltsConversion[dda][chan][block][sample][8]>1.0) block = (block - neighboring_index + blocks_per_dda)%blocks_per_dda;
        } else if (stationId == 5 || stationId != 4) {
            if (sample%2==0 && chan>0) sample=(sample+1)%samples_per_dda; ///< Dumping even samples
            while(fAtriSampleADCVoltsConversion[dda][chan][block][sample][8]>1.0) sample = (sample - neighboring_index + samples_per_dda)%samples_per_dda;
        }

        //! Offset needs to be subtracted
        double adcCounts = adcCountsIn + adc_offset;

        //! Start ADC to voltage conversion
        if(TMath::Abs(adcCounts)<high_adc_limit){
            //! conversion factors for higher ADC values have strong errors, therefore we need the alternative calibration (see below)
            //! RJN chnaged the below to remove calls to pow for code optimisation
            double modAdcCounts=adcCounts-fAtriSampleADCVoltsConversion[dda][chan][block][sample][7];
            //double modAdcCounts=adcCounts;//-fAtriSampleADCVoltsConversion[dda][chan][block][sample][7];

            Double_t fit_const; ///< Define the fit_const here (MK)
            double adc_zero_def; ///< Define which value will be used to choose a positive or negative conversion
            if (stationId != 5 && stationId != 4) {
                fit_const = fAtriSampleADCVoltsConversion[dda][chan][block][sample][6];
                adc_zero_def = adcCounts;
            } else {
                fit_const = 0.0; // fit const for A5 and A4 (? *** )
                adc_zero_def = modAdcCounts;
            }

            if(adc_zero_def>0){
                //! positive and negative values need different calibration constants
                volts = fit_const
                    +modAdcCounts*fAtriSampleADCVoltsConversion[dda][chan][block][sample][0] 
                    +modAdcCounts*modAdcCounts*fAtriSampleADCVoltsConversion[dda][chan][block][sample][1] 
                    +modAdcCounts*modAdcCounts*modAdcCounts*fAtriSampleADCVoltsConversion[dda][chan][block][sample][2]; 
            }
            else{
                volts = fit_const      
                    +modAdcCounts*fAtriSampleADCVoltsConversion[dda][chan][block][sample][3] 
                    +modAdcCounts*modAdcCounts*fAtriSampleADCVoltsConversion[dda][chan][block][sample][4]
                    +modAdcCounts*modAdcCounts*modAdcCounts*fAtriSampleADCVoltsConversion[dda][chan][block][sample][5];
            }

            /*!
                For A5, since there is no high ADC calibration data, use ADC count
                If ADC count between -500 ~ 500 is converted to over 800 mV, this condition decides to use just ADC value instead of the conversion result
                It seems ADC values between -400 ~ 400 are not converted to over 800 millivolts on A2/3
                Related talk: https://aradocs.wipac.wisc.edu/cgi-bin/DocDB/ShowDocument?docid=2464 (slide 16 ~17)
                I leave this condition just for A5 -MK-
            */
            if (stationId == 5 && volts > 800) volts=modAdcCounts;
            else if (stationId == 4 && volts > 800) volts=modAdcCounts;
            
        }
        else {
            if (stationId != 5 && stationId != 4){
                //! here is the alternative calibration (used only for A2 and A3) if the ADC count exceeds 400
                if(adcCounts>0) {
                    volts = fAtriSampleHighADCVoltsConversion[dda][chan][block][sample][0] 
                    + adcCounts*fAtriSampleHighADCVoltsConversion[dda][chan][block][sample][1];
                }
                else{
                    volts = fAtriSampleHighADCVoltsConversion[dda][chan][block][sample][2] 
                    + adcCounts*fAtriSampleHighADCVoltsConversion[dda][chan][block][sample][3];
                }
            } else {
                //! For A5, since there is no high ADC calibration data, use ADC count for conervison result in case A5 encount high ADC count
                // Similarly for A4, since no high ADC calib data is available, we use ADC count for the corresponding voltage result for high ADC count
                volts = adcCountsIn + adc_offset;
            }
        }
        
    }
    else{
        volts = adcCountsIn + adc_offset;
    }
    return volts;
}
