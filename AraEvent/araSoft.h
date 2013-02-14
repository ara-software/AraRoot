/*! \file araSoft.h
    \brief Header file for various ARA Constants
    
    The header file which specifies various ARA constants
    
    May 2011 rjn@hep.ucl.ac.uk

*/
/*
 *
 * Current CVS Tag:
 * $Header: $
 */

#ifndef ARA_SOFT_H
#define ARA_SOFT_H


#ifndef __MAKECINT__
#include <stdio.h>
#include <syslog.h>


#define ARA_LOG LOG_LOCAL4

//#define ARA_LOG_MESSAGE(LOG_LEVEL,...) if(printToScreen>LOG_LEVEL) fprintf(stderr, __VA_ARGS__); syslog(LOG_LEVEL,__VA_ARGS__);

///The one global variable for logging to screen
#ifndef ARA_ROOT
//int printToScreen;
#endif
#endif


///Hardware descriptors
#define ANTS_PER_ATRI 20
#define DDA_PER_ATRI 4
#define BLOCKS_PER_DDA 512
#define RFCHAN_PER_DDA 8
#define TDA_PER_ATRI 4
#define ANTS_PER_TDA 4 
#define L2_PER_TDA 4 //Wrong?!
#define THRESHOLDS_PER_ATRI TDA_PER_ATRI*ANTS_PER_TDA
#define SAMPLES_PER_BLOCK 64
#define MAX_TRIG_BLOCKS 4
#define CHANNELS_PER_ATRI DDA_PER_ATRI*RFCHAN_PER_DDA
#define RFCHANS_PER_ATRI 20


//As per email from P.Alison 9th Nov 2012
#define NUM_L1_SCALERS 32
#define NUM_L2_SCALERS 32
#define NUM_L3_SCALERS 16
#define NUM_L4_SCALERS 8
#define NUM_T1_SCALERS 8


#define FIRST_BLOCK_FRAME_START 0x65  //'e'
#define FIRST_BLOCK_FRAME_OTHER 0x45  //'E'
#define MIDDLE_BLOCK_FRAME_START 0x62 // 'b'
#define MIDDLE_BLOCK_FRAME_OTHER 0x42 // 'B'
#define LAST_BLOCK_FRAME_START 0x66 // 'f'
#define LAST_BLOCK_FRAME_OTHER 0x46 // 'F'
#define ONLY_BLOCK_FRAME_START 0x6f  //'o'
#define ONLY_BLOCK_FRAME_OTHER 0x4f  //'O'

#define ATRI_NO_STATIONS 37 ///<Future proofing such that there can be multiple ATRI stations

//PID Files  //will change to /var/run
#define ARAD_PID_FILE "/tmp/arad.pid"
#define ARA_ACQD_PID_FILE "/tmp/araAcqd.pid"

//CONFIG Files //will change to absoulte path maybe
#define ARAD_CONFIG_FILE "arad.config"
#define ARA_ACQD_CONFIG_FILE "ARAAcqd.config"

//SOCKETS
#define ATRI_CONTROL_SOCKET "/tmp/atri_control"
#define FX2_CONTROL_SOCKET "/tmp/fx2_control"
#define ACQD_RC_SOCKET "/tmp/acqd_rc"


#define ARA_RUN_NUMBER_FILE "/home/ara/araRunNumber"
#define DAQ_EVENT_DIR "event/"
#define DAQ_SENSOR_HK_DIR    "sensorHk/"
#define DAQ_EVENT_HK_DIR "eventHk/"
#define DAQ_PED_DIR   "peds/"

#define EVENT_FILE_HEAD "ev"
#define SENSOR_HK_FILE_HEAD    "sensorHk"
#define EVENT_HK_FILE_HEAD    "eventHk"
#define PED_FILE_HEAD   "peds"
#define DAQ_RUNLOG_DIR  "logs"


#endif //ARA_SOFT_H
