/* keyValuePair.h - <short description of this file/module> */

/* G.J.Crone, University College London */

/*
 * Current CVS Tag:
 * $Header: /work1/anitaCVS/flightSoft/common/kvpLib/keyValuePair.h,v 1.1 2004/08/31 15:48:09 rjn Exp $
 */
 
/* 
 * Modification History : DO NOT EDIT - MAINTAINED BY CVS 
 * $Log: keyValuePair.h,v $
 * Revision 1.1  2004/08/31 15:48:09  rjn
 * Um... lots of additions. The biggest ones are switching from shared to static libraries, and the additon of configLib and kvpLib to read formatted config files. These two libraries were created by Gordon Crone (gjc@hep.ucl.ac.uk) and are in spirit released under the GPL license.
 * Also the first of the actually programs (Eventd) has been started. At the moment it just opens a socket to fakeAcqd. Other sockets and better handling of multiple sockets in socketLib are the next things on my to do list.
 *
 * Revision 1.11  2004/03/04 16:15:56  gjc
 * Grow internal buffer (by allocating larger buffer and copying data) as
 * necessary.
 *
 * Revision 1.10  2004/03/04 11:50:20  gjc
 * Added new function kvpQueryLength to return the number of elements in
 * an array.
 *
 * Revision 1.9  2004/01/23 17:40:24  gjc
 * double size of buffer (KVP_BUFFER_SIZE).
 *
 * Revision 1.8  2003/09/04 21:06:16  gjc
 * Use malloc instead of static array.  Move BUFFER_SIZE in keyValuePair.c to KVP_BUFFER_SIZE in keyValuePair.h
 *
 * Revision 1.7  2003/07/31 16:53:01  gjc
 * Invalidate old key and skip it from kvpFormat when overwriting in kvpParse
 *
 * Revision 1.6  2003/02/27 12:34:36  gjc
 * Robert Hatcher's mods to avoid warnings with gcc 3.2
 *
 * Revision 1.1  2003/02/25 21:18:40  rhatcher
 * key-value pair string en/decoding that the DAQ group uses.  Comes through
 * in the RawRunConfigBlock (which holds the "run prepare" string).
 * small modifications to "official" version to eliminate warning messages.
 *
 * Revision 1.5  2003/02/13 15:41:18  gjc
 * Increased MAXKEYS and check for exceding it in kvpParse
 *
 * Revision 1.4  2001/07/24 17:19:55  gjc
 * Added new function kvpGetStringArray
 *
 * Revision 1.3  2001/05/16 11:52:09  gjc
 * Added new function kvpUpdateInt needed for run number kludge.
 * Removed reference to syslog and added new error code KVP_E_TERMINATOR
 * for missing semi-colon.
 *
 * Revision 1.2  2001/04/11 16:27:05  gjc
 * Stricter checking of numeric values in kvpParse.  Fixes bug with
 * inconsistent element count for hex values.
 * No longer start again at beginnning of internal buffers every time
 * kvpParse is a called.  Must call kvpReset before kvpParse if that's
 * what you want.
 *
 * Revision 1.1  2001/03/26 18:09:21  gjc
 * initial check in
 *
 *
 */ 

#ifndef _keyValuePair_H
#define _keyValuePair_H
#ifdef __cplusplus 
extern "C" { 
#endif /* __cplusplus */
 
/* includes */

/* defines */
#define MAXKEYS 1000
#define KVP_MIN_BUFFER_SIZE 20480

#define MAX_INT_LENGTH 12
#define MAX_FLOAT_LENGTH 14

/* typedefs */
typedef enum {
   KVP_E_OK = 0,
   KVP_E_DUPLICATE,
   KVP_E_NOTFOUND,
   KVP_E_SYSTEM,
   KVP_E_BADKEY,
   KVP_E_BADVALUE,
   KVP_E_TOOMANY,
   KVP_E_TOOLONG,
   KVP_E_BADTYPE,
   KVP_E_BADINPUT,
   KVP_E_WRONGCOUNT,
   KVP_E_TRUNCATE,
   KVP_E_ILLNUMBER,
   KVP_E_TERMINATOR
} KvpErrorCode ;

   typedef enum {
      KT_STRING,
      KT_FLOAT,
      KT_INT,
      KT_UINT,
      KT_INVALID
   } KeyType ;
/* global variable declarations*/   

/* function prototype declarations */
   void kvpInit (int bufferSize) ;
   int kvpParse (const char* record) ;

   void kvpReset (void) ;
   KvpErrorCode kvpGetEntry (int entry, char** key, KeyType* type,
                             int*size, char** value) ;

   char* kvpGetString (const char* key) ;
    
    int kvpGetInt (const char* key, int defaultValue) ;
    int kvpGetDec (const char* key, int defaultValue) ;
    int kvpGetHex (const char* key, int defaultValue) ;
    unsigned int kvpGetUnsignedInt (const char* key,unsigned int defaultValue);


   float kvpGetFloat (const char* key, float defaultValue) ;
   double kvpGetDouble (const char* key, double defaultValue) ;

   int kvpQueryLength (const char* key) ;
   void kvpDumpInternals (void) ;

   KvpErrorCode kvpSetString (const char* key, const char* value) ;
   KvpErrorCode kvpSetInt (const char* key, int value) ;
   KvpErrorCode kvpSetFloat (const char* key, float value) ;

   KvpErrorCode kvpSetIntArray (const char* key, const int* values,
                                int count) ;
   KvpErrorCode kvpSetFloatArray (const char* key, const float* values,
                                  int count) ;

   KvpErrorCode kvpGetIntArray (const char* key,
                                int* values, int* entries) ;
   KvpErrorCode kvpGetFloatArray (const char* key,
                                  float* values, int* entries) ;

   KvpErrorCode kvpGetStringArray (const char* key, char* values,
                                   int  maxlen, int* entries) ;

    KvpErrorCode kvpUpdateInt (const char* key, int value) ;
    KvpErrorCode kvpUpdateIntArray (const char* key, int *values,
				    int count) ;
    KvpErrorCode kvpUpdateUnsignedInt (const char* key, unsigned int value) ;

    KvpErrorCode kvpUpdateUnsignedIntArray (const char* key, unsigned int *values,
					    int count) ;
    KvpErrorCode kvpUpdateFloatArray (const char* key, float *values,
				      int count) ;
    
    KvpErrorCode kvpUpdateFloat (const char* key, float value) ;
    KvpErrorCode kvpUpdateString (const char* key,const char *value);

   int kvpWrite (int stream) ;
   int kvpFormat (char* buffer, int length) ;

   KvpErrorCode kvpError (void) ;
   void kvpPrintError (KvpErrorCode code) ;
   const char* kvpErrorString (KvpErrorCode code) ;
#ifdef __cplusplus 
} 
#endif /* __cplusplus */ 

#endif /* _keyValuePair_H */
