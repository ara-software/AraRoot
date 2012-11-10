/*! \file araOneStructures.h
    \brief Header file for various ARA Constants
    
    The header file which specifies various ARA constants
    
    May 2011 rjn@hep.ucl.ac.uk

*/
/*
 *
 * Current CVS Tag:
 * $Header: $
 */

#ifndef ATRI_STRUCTURES_H
#define ATRI_STRUCTURES_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <stdint.h>
#endif

#include "araSoft.h"

#define ARA_SOFT_VERISON 3
#define ARA_SOFT_SUB_VERISON 0



//!  The ARA Data Structure Type Codes
/*!
  Each data structure has a type code and a version number. These are just hand-holdy things that are useful of keeping track of things down stream.
*/
enum {
  ARA_EVENT_TYPE=0x1, ///< Standard event data
  ARA_HEADER_TYPE=0x2, ///< Standard event header (if not part of event)
  ARA_EVENT_HK_TYPE=0x3, ///< Scalers, thresholds, etc.
  ARA_SENSOR_HK_TYPE=0x4, ///< Temperatures, Voltages, Currents, etc.
  ARA_SBC_HK_TYPE=0x5, ///< SBC related values disk space, etc.
  ARA_ICRR_EVENT_TYPE=0x06 ///< ICRR Event Type
} ;
typedef uint8_t AraDataStructureType_t;  ///< Ensure that it is just 8 bits

//!  The ARA Station Id
/*!
  Each data structure is going to have a one byte overhead saying where it came from
*/
enum {
  ARA_TESTBED=0x0, ///< Icrr TestBed probably won't chnage the testbed data yet
  ARA_STATION1=0x1, ///< Icrr Station1 data
  ARA_STATION1A=-1, ///< Placeholder for ATRI updated Station1
  ARA_STATION2=0x2, ///< ATRI Station2 data
  ARA_STATION3=0x3, ///< ATRI Station3 data
  ARA_STATION4=0x4, ///< ATRI Station4 data
  ARA_STATION5=0x5, ///< ATRI Station5 data
  ARA_STATION6=0x6, ///< ATRI Station6 data
  ARA_STATION7=0x7, ///< ATRI Station7 data
  ARA_STATION8=0x8, ///< ATRI Station8 data
  ARA_STATION9=0x9, ///< ATRI Station9 data

  ARA_STATION10=10, ///< ATRI Station10 data
  ARA_STATION11=11, ///< ATRI Station11 data
  ARA_STATION12=12, ///< ATRI Station12 data
  ARA_STATION13=13, ///< ATRI Station13 data
  ARA_STATION14=14, ///< ATRI Station14 data
  ARA_STATION15=15, ///< ATRI Station15 data
  ARA_STATION16=16, ///< ATRI Station16 data
  ARA_STATION17=17, ///< ATRI Station17 data
  ARA_STATION18=18, ///< ATRI Station18 data
  ARA_STATION19=19, ///< ATRI Station19 data

  ARA_STATION20=20, ///< ATRI Station20 data
  ARA_STATION21=21, ///< ATRI Station21 data
  ARA_STATION22=22, ///< ATRI Station22 data
  ARA_STATION23=23, ///< ATRI Station23 data
  ARA_STATION24=24, ///< ATRI Station24 data
  ARA_STATION25=25, ///< ATRI Station25 data
  ARA_STATION26=26, ///< ATRI Station26 data
  ARA_STATION27=27, ///< ATRI Station27 data
  ARA_STATION28=28, ///< ATRI Station28 data
  ARA_STATION29=29, ///< ATRI Station29 data

  ARA_STATION30=30, ///< ATRI Station30 data
  ARA_STATION31=31, ///< ATRI Station31 data
  ARA_STATION32=32, ///< ATRI Station32 data
  ARA_STATION33=33, ///< ATRI Station33 data
  ARA_STATION34=34, ///< ATRI Station34 data
  ARA_STATION35=35, ///< ATRI Station35 data
  ARA_STATION36=36, ///< ATRI Station36 data
  ARA_STATION37=37  ///< ATRI Station37 data

} ;
typedef uint8_t AraStationId_t;  ///< Ensure that it is just 8 bits

#define THIS_STATION ARA_STATION2
//SBC_DEFINE -- Each SBC should be running AraSoft with the correct stationId set as THIS_STATION


//!  Part of AraEvent library. The ATRI generic header data structure.
/*!
  This is the 8 byte header that prefaces all ARA one data written to disk. Contains information about which type and version a packet was, and a checksum to ensure the data is valid.
*/
typedef struct {
  AraDataStructureType_t typeId;
  AraStationId_t stationId;
  uint8_t verId;
  uint8_t subVerId;
  uint32_t numBytes;
  uint16_t checksum;
  uint16_t reserved;
  uint32_t alsoReserved;
} AtriGenericHeader_t;


//!  Part of AraEvent library. The ATRI housekeeping data structure. 
/*!
  WARNING -- This is the structure for araSoft version 2_7 and below

  This is the N-byte structure that contains information about the various housekeeping values associated with the event readout.
*/
typedef struct {
  AtriGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Time in microseconds (32-bits)
  uint32_t firmwareVersion; ///< Firmware version
  uint16_t wilkinsonCounter[DDA_PER_ATRI]; ///< Wilkinson counter one per DDA
  uint16_t wilkinsonDelay[DDA_PER_ATRI]; ///< Wilkinson delay?? one per DDA
  uint32_t ppsCounter; ///< Pulse per second counter
  uint32_t clockCounter; ///< Clock counter (which clock?)
  uint16_t l1Scaler[TDA_PER_ATRI][ANTS_PER_TDA]; ///< L1 scaler, am I correct in decoding this need to check mapping prescaled by 32
  uint16_t l1ScalerSurface[ANTS_PER_TDA]; ///< The Surface L1 scaler
  uint16_t l2Scaler[TDA_PER_ATRI][L2_PER_TDA]; ///< L2 scaler not prescaled
  uint16_t l3Scaler; ///< L3 scaler not prescaled
  uint16_t l3ScalerSurface; ///< L3 surface scaler not prescaled
  uint16_t l2ScalerAllTda12; ///< L2 for 3of8 in DTA 1&2
  uint16_t l2ScalerAllTda34; ///< L2 for 3of8 in DTA 3&4
  uint16_t vdlyDac[DDA_PER_ATRI]; ///< Value the vdly is set to
  uint16_t vadjDac[DDA_PER_ATRI]; ///< Value the vdly is set to
  uint16_t thresholdDac[TDA_PER_ATRI][ANTS_PER_TDA]; ///< Value the thresholds are set to
  uint16_t surfaceThresholdDac[ANTS_PER_TDA]; ///< The surface thresholds
  uint8_t deadTime[DDA_PER_ATRI]; ///< Dead time  8-bit measures of deadtime (multiply by 4096, divide by 1e6).
  uint8_t avgOccupancy[DDA_PER_ATRI]; ///< Average occupancy over last 16 milliseconds
  uint8_t maxOccupancy[DDA_PER_ATRI]; ///< Maximum occupancy in last second
  uint8_t reserved[DDA_PER_ATRI];
} AraEventHk2_7_t;



//!  Part of AraEvent library. The ATRI sensor housekeeping data structure.
/*!
  This is for araSoft version 3.0 and above
  This is the N-byte structure that contains information about the various housekeeping values associated with the event readout.
*/
typedef struct {
  AtriGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Time in microseconds (32-bits)
  uint32_t firmwareVersion; ///< Firmware version
  uint16_t wilkinsonCounter[DDA_PER_ATRI]; ///< Wilkinson counter one per DDA
  uint16_t wilkinsonDelay[DDA_PER_ATRI]; ///< Wilkinson delay?? one per DDA
  uint32_t ppsCounter; ///< Pulse per second counter
  uint32_t clockCounter; ///< Clock counter (which clock?)
  uint16_t l1Scaler[NUM_L1_SCALERS]; ///< L1 scaler, am I correct in decoding this need to check mapping prescaled by 32
  uint16_t l1ScalerSurface[ANTS_PER_TDA]; ///< The Surface L1 scaler
  uint16_t l2Scaler[NUM_L2_SCALERS]; ///< L2 scaler not prescaled
  uint16_t l3Scaler[NUM_L3_SCALERS]; ///< L3 scaler not prescaled
  uint16_t l4Scaler[NUM_L4_SCALERS]; ///< L4 scaler not prescaled
  uint16_t t1Scaler[NUM_T1_SCALERS]; ///< T1 scaler not prescaled
  uint16_t vdlyDac[DDA_PER_ATRI]; ///< Value the vdly is set to
  uint16_t vadjDac[DDA_PER_ATRI]; ///< Value the vdly is set to
  uint16_t thresholdDac[NUM_L1_SCALERS]; ///< Value the thresholds are set to
  uint16_t surfaceThresholdDac[ANTS_PER_TDA]; ///< The surface thresholds
  uint8_t deadTime[DDA_PER_ATRI]; ///< Dead time  8-bit measures of deadtime (multiply by 4096, divide by 1e6).
  uint8_t avgOccupancy[DDA_PER_ATRI]; ///< Average occupancy over last 16 milliseconds
  uint8_t maxOccupancy[DDA_PER_ATRI]; ///< Maximum occupancy in last second
  uint8_t reserved[DDA_PER_ATRI];
} AraEventHk_t;


//!  Part of AraEvent library. The ATRI sensor housekeeping data structure.
/*!
  This is the N-byte structure that contains information about the various housekeeping values not associated with the event readout.
*/
typedef struct {
  AtriGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Time in microseconds (32-bits)
  uint8_t atriVoltage; ///< ATRI Voltage (conversion??)
  uint8_t atriCurrent; ///< ATRI Current (conversion??)
  uint16_t ddaTemp[DDA_PER_ATRI]; ///< DDA Temperature conversion??
  uint16_t tdaTemp[TDA_PER_ATRI]; ///< TDA Temperature conversion??
  uint32_t ddaVoltageCurrent[DDA_PER_ATRI]; ///< 3 bytes only will work out better packing when I know what the numbers mean
  uint32_t tdaVoltageCurrent[DDA_PER_ATRI]; ///< 3 bytes only will work out better packing when I know what the numbers mean
} AraSensorHk_t;


//!  Part of AraEvent library. A simple structure used for development when getting the ATRI data format running.
/*!
  This is the N-byte structure that contains the event data in some format. This format will change when we have the full system working
*/
typedef struct {
  AtriGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Time in microseconds (32-bits)
  uint32_t eventNumber; ///< Event number
  uint32_t eventId[DDA_PER_ATRI]; ///< Event Id
  uint16_t blockId[DDA_PER_ATRI]; ///< Block Id
  uint16_t samples[DDA_PER_ATRI][512]; ///< Samples
} AraSimpleStationEvent_t;



#define EXTRA_SOFTWARE_HEADER_BYTES 32 ///I think

//!  Part of the AraEvent library. The ARA ATRI Station Event Header Format
/*!
  This is the N-byte structure that contains the event data in some format. This format will change when we have the full system working
*/
typedef struct {
  AtriGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Software event time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Software event time in microseconds (32-bits)
  uint32_t eventNumber; ///< Software event number
  uint32_t numBytes; ///<Bytes in station readout
  uint32_t timeStamp; ///< Timestamp
  uint32_t ppsNumber; ///< For matching up with thresholds etc.
  uint32_t eventId; ///< Event Id
  uint16_t versionNumber; ///<Event version number
  uint16_t numReadoutBlocks; ///< Number of readout blocks which follow header
  uint16_t triggerPattern[MAX_TRIG_BLOCKS]; ///< The trigger type block, might do something more clever with this
  uint16_t triggerInfo[MAX_TRIG_BLOCKS]; ///< The trigger type block, might do something more clever with this
  uint8_t triggerBlock[MAX_TRIG_BLOCKS]; ///< Which block (starting from 0) di trigger occur in?
} AraStationEventHeader_t;
  


//!  Part of AraEvent library. The ARA ATRI Station Event Block Header Format
/*!
  This is the 4-byte structure that contains the Ara Station Event Block Header
*/
typedef struct {
  uint16_t irsBlockNumber; ///< The IRS block number
  uint16_t channelMask; ///< Channel mask
  //  uint8_t channelMask; ///<Bit mask for the 8 available channels 0x8f by default
  //  uint8_t atriDdaNumber; 
} AraStationEventBlockHeader_t;


//!  Part of AraEvent library. The ARA ATRI Station Event Block Channel
/*!
  This is the 128-byte structure which contains the 64 samples in a single channel
*/
typedef struct {
  uint16_t samples[SAMPLES_PER_BLOCK]; ///< The IRS block readout
} AraStationEventBlockChannel_t;
  


#endif //ATRI_STRUCTURES_H
