/*! \file configLib.h
    \brief The configuration file hadling library
    
    Originally by G.J.Crone, University College London. Adapted for ANITA
    flight software use by R.J.Nichol, Ohio State University. Contains 
    routines for using kvpLib to parse human readable formatted config files.
    September 2004  rjn@mps.ohio-state.edu
*/
/*
 *
 * Current CVS Tag:
 * $Header: /work1/anitaCVS/flightSoft/common/configLib/configLib.h,v 1.3 2005/06/15 16:22:00 rjn Exp $
 */
 
/* 
 * Modification History : DO NOT EDIT - MAINTAINED BY CVS 
 * $Log: configLib.h,v $
 * Revision 1.3  2005/06/15 16:22:00  rjn
 * Fixed a lot of silly warnings
 *
 * Revision 1.2  2004/09/07 13:52:21  rjn
 * Added dignified multiple port handling capabilities to socketLib. Not fully functional... but getting there.
 *
 * Revision 1.1  2004/08/31 15:48:09  rjn
 * Um... lots of additions. The biggest ones are switching from shared to static libraries, and the additon of configLib and kvpLib to read formatted config files. These two libraries were created by Gordon Crone (gjc@hep.ucl.ac.uk) and are in spirit released under the GPL license.
 * Also the first of the actually programs (Eventd) has been started. At the moment it just opens a socket to fakeAcqd. Other sockets and better handling of multiple sockets in socketLib are the next things on my to do list.
 *
 * Revision 1.2  2001/07/27 12:56:36  gjc
 * Added CONFIG_E_UNNAMED error code and configValidate prototype.
 *
 * Revision 1.1  2001/07/24 13:36:43  gjc
 * First check in of new package
 *
 *
 */ 

#ifndef _config_H
#define _config_H
#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */
 
/* includes */
#include "time.h"

/* defines */
#define CONFIG_VAR "DAQ_CONFIG_DIR"
#define BLOCKNAME_MAX 64
#define MAX_BLOCKS 32
#define RECLEN_MAX 4096

/* typedefs */
   typedef enum {
      CONFIG_E_OK = 0,
      CONFIG_E_NOFILE = 0x100,
      CONFIG_E_NESTING,
      CONFIG_E_EOF,
      CONFIG_E_SYSTEM,
      CONFIG_E_KVP,
      CONFIG_E_SECTION,
      CONFIG_E_UNNAMED
   } ConfigErrorCode ;
/* global variable declarations*/   

/* function prototype declarations */
   char* configFileSpec (char* fileName) ;
   ConfigErrorCode configLoad (const char* fleName, const char* blockList) ;
   ConfigErrorCode configLoadFullPath (char* fleName, char* blockList) ;
   ConfigErrorCode configStore (char* fileName, char* blockName) ;
   const char* configErrorString (ConfigErrorCode code) ;
   ConfigErrorCode configValidate (char* fileName) ;

/* RJN additions */
    ConfigErrorCode configAppend (char* fileName, char* blockName) ;
    ConfigErrorCode readBlocks(char *fileName,char blockList[MAX_BLOCKS][BLOCKNAME_MAX],int *numBlocks);
    
    ConfigErrorCode configModifyInt(char *fileName,char *blockName,char *key,int value, time_t *rawTimePtr);
    ConfigErrorCode configModifyIntArray(char *fileName,char *blockName,char *key,int *values, int length, time_t *rawTimePtr);
    ConfigErrorCode configModifyUnsignedIntArray(char *fileName,char *blockName,char *key,unsigned int *values, int length, time_t *rawTimePtr);
    ConfigErrorCode configModifyFloatArray(char *fileName,char *blockName,char *key,float *values, int length, time_t *rawTimePtr);
    ConfigErrorCode configModifyUnsignedInt(char *fileName,char *blockName,char *key,unsigned long value, time_t *rawTimePtr);
    ConfigErrorCode configModifyFloat(char *fileName,char *blockName,char *key,float value, time_t *rawTimePtr);
    ConfigErrorCode configModifyString(char *fileName,char *blockName,char *key,char *value, time_t *rawTimePtr);

    ConfigErrorCode configReplace(char *oldFileName, char *newFileName, time_t *rawTimePtr);
    ConfigErrorCode configSwitch(char *configFile, char whichConfig, time_t *rawTimePtr);
    ConfigErrorCode configSwitchToLast(char *configFile, time_t *rawTimePtr);
#ifdef __cplusplus 
} 
#endif /* __cplusplus */ 

#endif /* _config_H */
