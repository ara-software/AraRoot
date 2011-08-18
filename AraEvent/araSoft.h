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
#define ARA_LOG_MESSAGE(LOG_LEVEL,...) if(printToScreen>LOG_LEVEL) fprintf(stderr, __VA_ARGS__); syslog(LOG_LEVEL,__VA_ARGS__);

///The one global variable for logging to screen
#ifndef ARA_ROOT
int printToScreen;
#endif
#endif


///Hardware descriptors
#define DDA_PER_ATRI 4
#define RFCHAN_PER_DDA 4
#define TDA_PER_ATRI 4


//PID Files  //will change to /var/run
#define ARAD_PID_FILE "/tmp/arad.pid"
#define ARA_ACQD_PID_FILE "/tmp/araAcqd.pid"

//CONFIG Files //will change to absoulte path maybe
#define ARAD_CONFIG_FILE "arad.config"
#define ARA_ACQD_CONFIG_FILE "araAcqd.config"

//SOCKETS
#define ATRI_CONTROL_SOCKET "/tmp/atri_control"
#define FX2_CONTROL_SOCKET "/tmp/fx2_control"
#define ACQD_RC_SOCKET "/tmp/acqd_rc"


#define ARA_RUN_NUMBER_FILE "/tmp/araRunNumber"
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
