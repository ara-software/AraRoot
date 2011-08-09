/*! \file araDefinest.h
    \brief The standard ARA include file.
    
    It will end up including all those definitions that are needed 
    all over the shop. Hopefully there won't be too many of these floating 
    around.

    
    June 2010  ped@m2-consult.com , 
    Nov 2010 rjn@hep.ucl.ac.uk
    Copyright: M2 Consulting, 2010, sod off is it Copyright M2 Consulting I wrote most of this for ANITA
*/


#ifndef ARA_DEFINES_H
#define ARA_DEFINES_H

// Hardware stuff
#define RUBIDIUM_FREQUENCY 280000000 //280MHz
#define TESTBED1_CLOCK_PERIOD 25 //ns
#define TESTBED1_CLOCK_CHANNEL 8 // counting from 0
#define CHANNELS_PER_CHIP 9
#define MAX_NUMBER_SAMPLES 260
#define ACTIVE_CHIPS 3
#define NUM_DIGITIZED_CHANNELS (ACTIVE_CHIPS*CHANNELS_PER_CHIP)
#define EFFECTIVE_SAMPLES 256
#define ADC_MAX 4096

#define NUM_PRIORITIES 10
#define NUM_PROCESSES 8

//New stuff rjn added
#define ANTS_PER_STATION 16
#define RFCHANS_PER_STATION ANTS_PER_STATION
#define TRANS_PER_STATION 6
#define TOTAL_ANTS ANTS_PER_STATION+TRANS_PER_STATION

#endif // ARA_DEFINES_H
