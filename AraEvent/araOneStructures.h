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

#ifndef ARAONE_STRUCTURES_H
#define ARAONE_STRUCTURES_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <stdint.h>
#endif
#include <araSoft.h>

#define ARA_SOFT_VERISON 0x1



//!  The ARA Data Structure Type Codes
/*!
  Each data structure has a type code and a version number. These are just hand-holdy things that are useful of keeping track of things down stream.
*/
enum {
  ARA_EVENT_TYPE=0x1, ///< Standard event data
  ARA_HEADER_TYPE=0x2, ///< Standard event header (if not part of event)
  ARA_EVENT_HK_TYPE=0x3, ///< Scalers, thresholds, etc.
  ARA_SENSOR_HK_TYPE=0x4, ///< Temperatures, Voltages, Currents, etc.
  ARA_SBC_HK_TYPE=0x5 ///< SBC related values disk space, etc.
} ;
typedef uint8_t AraDataStructureType_t;  ///< Ensure that it is just 8 bits

//!  The ARA Station Id
/*!
  Each data structure is going to have a one byte overhead saying where it came from
*/
enum {
  ARA_STATION_TESTBED=0x0, ///< TestBed1 probably won't chnage the testbed data yet
  ARA_STATION_ARA1=0x1 ///< ARA1 data
} ;
typedef uint8_t AraStationId_t;  ///< Ensure that it is just 8 bits


#define THIS_STATION ARA_STATION_ARA1



//!  The ARA Generic Header
/*!
  This is the 8 byte header that prefaces all ARA one data written to disk. Contains information about which type and version a packet was, and a checksum to ensure the data is valid.
*/
typedef struct {
  AraDataStructureType_t typeId;
  uint8_t verId;
  AraStationId_t stationId;
  uint8_t reserved;
  uint16_t numBytes;
  uint16_t checksum;
} AraGenericHeader_t;


//!  The ARA Event Housekeeping Structure
/*!
  This is the N-byte structure that contains information about the various housekeeping values associated with the event readout.
*/
typedef struct {
  AraGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Time in microseconds (32-bits)
  uint32_t firmwareVersion; ///< Firmware version
  uint16_t wilkinsonCounter[DDA_PER_ATRI]; ///< Wilkinson counter one per DDA
  uint16_t wilkinsonDelay[DDA_PER_ATRI]; ///< Wilkinson delay?? one per DDA
  uint32_t ppsCounter; ///< Pulse per second counter
  uint32_t clockCounter; ///< Clock counter (which clock?)
  uint16_t l1Scaler[DDA_PER_ATRI][RFCHAN_PER_DDA]; ///< L1 scaler, am I correct in decoding this need to check mapping
  uint16_t l2Scaler[DDA_PER_ATRI]; ///< L2 scaler
  uint16_t l3Scaler; ///< L3 scaler
  uint16_t triggerScaler; ///< Trigger scaler (what is this?)
} AraEventHk_t;


//!  The ARA Sensor Housekeeping Structure
/*!
  This is the N-byte structure that contains information about the various housekeeping values not associated with the event readout.
*/
typedef struct {
  AraGenericHeader_t gHdr; ///< The generic header 
  uint64_t unixTime; ///< Time in seconds (64-bits for future proofing)
  uint32_t unixTimeUs; ///< Time in microseconds (32-bits)
  uint8_t atriVoltage; ///< ATRI Voltage (conversion??)
  uint8_t atriCurrent; ///< ATRI Current (conversion??)
  uint16_t ddaTemmp[DDA_PER_ATRI]; ///< DDA Temperature conversion??
  uint16_t tdaTemp[TDA_PER_ATRI]; ///< TDA Temperature conversion??
  uint32_t ddaVoltageCurrent[DDA_PER_ATRI]; ///< 3 bytes only will work out better packing when I know what the numbers mean
  uint32_t tdaVoltageCurrent[DDA_PER_ATRI]; ///< 3 bytes only will work out better packing when I know what the numbers mean
} AraSensorHk_t;






#endif //ARAONE_STRUCTURES_H
