/* configLoad.c - Functions for loading configuration data */

/* G.J.Crone, University College London */

/*
 * Current CVS Tag:
 * $Header: /work1/anitaCVS/flightSoft/common/configLib/configLoad.c,v 1.3 2005/06/15 16:22:00 rjn Exp $
 */
 
/* 
 * Modification History : DO NOT EDIT - MAINTAINED BY CVS 
 * $Log: configLoad.c,v $
 * Revision 1.3  2005/06/15 16:22:00  rjn
 * Fixed a lot of silly warnings
 *
 * Revision 1.2  2004/08/31 17:51:21  rjn
 * Stuck a load of syslog messages into the configLib stuff. Previosuly it used some strange wrapper around syslog, due to implementation issues on the operating system used by some parts of the MINOS daq.
 *
 * Revision 1.1  2004/08/31 15:48:09  rjn
 * Um... lots of additions. The biggest ones are switching from shared to static libraries, and the additon of configLib and kvpLib to read formatted config files. These two libraries were created by Gordon Crone (gjc@hep.ucl.ac.uk) and are in spirit released under the GPL license.
 * Also the first of the actually programs (Eventd) has been started. At the moment it just opens a socket to fakeAcqd. Other sockets and better handling of multiple sockets in socketLib are the next things on my to do list.
 *
 * Revision 1.10  2002/10/09 08:47:32  gjc
 * Downgrade "block not found" from error to warning.
 *
 * Revision 1.9  2002/07/17 15:57:44  gjc
 * Added missing fclose to configLoad (whoops!)
 *
 * Revision 1.8  2002/06/12 12:22:43  gjc
 * Changed loading message from info to debug on level 1
 *
 * Revision 1.7  2001/12/03 13:36:07  gjc
 * Removed duplicate lines from parseBlockList, presumambly left by wrong
 * edit of previous update conflict.
 *
 * Revision 1.6  2001/11/26 15:13:20  howcroft
 * some file format error fixed
 *
 * Revision 1.5  2001/11/19 15:17:35  howcroft
 * Removed comment in commet bug
 *
 * Revision 1.4  2001/11/08 11:39:55  howcroft
 * Fixed small bug in string termination in func:  parseBlockList
 *  
 *         blockName[nBlocks][length]='\0';
 *
 * Revision 1.3  2001/08/15 14:39:34  gjc
 * Fix silly bug:  forgot to switch off continuation flag!
 *
 * Revision 1.2  2001/07/27 12:51:07  gjc
 * Stricter checking in getTag.  New function configValidate. Extract
 * filespec expansion to separate file
 *
 * Revision 1.1  2001/07/24 13:36:44  gjc
 * First check in of new package
 *
 *
 *
 */ 

/*
DESCRIPTION
<Insert a description of the file/module here>
INCLUDE FILES: <place list of any relevant header files here>
*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "configLib/configLib.h"
#include "kvpLib/keyValuePair.h"

/* defines */

/* typedefs */

/* globals */   

/* locals */
static FILE* config ;
static char blockName[MAX_BLOCKS][BLOCKNAME_MAX] ;
static int nBlocks ;
static int inBlock ;

/* forward declarations */

static ConfigErrorCode getTag (char* thisBlock)
{
   int gotTag ;
   char* tPtr ;
   char buffer[RECLEN_MAX] ;
   int length ;

   gotTag = 0 ;
   while (!gotTag) {
      tPtr = fgets (buffer, sizeof (buffer), config) ;
      if (tPtr == NULL) {
         return (CONFIG_E_EOF) ;
      }

      if (buffer[0] == '<') {
         if (buffer[1] != '/') {
            /* block delimiter */
            tPtr = &buffer[1] ;
            length = 0 ;
            /* copy up to closing '>' */
            while ((*tPtr != '>') && (length < BLOCKNAME_MAX)) {
               thisBlock[length++] = *tPtr++ ;
            }
            /* terminate the string */
            thisBlock[length] = 0 ;
            gotTag = 1 ;
            inBlock = 1 ;
         }
         else {
            inBlock = 0 ;
         }
      } else if ((!inBlock) &&
                 !((buffer[0] == '/') && (buffer[1] == '/')) &&
                 (buffer[0] != '\n')) {
         return (CONFIG_E_UNNAMED) ;
      }
   }
   return (CONFIG_E_OK) ;
}

static int checkTag (
                     char* tag
                     )
{
   int block ;

   /* Compare with list of blocks we're interested in */
   for (block = 0 ; block < nBlocks ; block++) {
      if (strcmp (tag, blockName[block]) == 0) {
         return (block) ;
      }
   }
   return (-1) ;
}

static int readBlock (void)
{
   char buffer[RECLEN_MAX] ;
   char* tPtr ;
   int wantBlock ;
   int nitems ;
   //   char* errMessage ;
   int spaceLeft=0 ;
   int offset=0 ;
   int continuation ;
   int retCode ;

   continuation = 0 ;
   wantBlock = 1 ;
   while (wantBlock) {
      if (!continuation) {
         offset = 0 ;
         spaceLeft = sizeof (buffer) ;
      }
      tPtr = fgets (&buffer[offset], spaceLeft, config) ;
      if (tPtr == NULL)  {
         if (feof (config)) {
            syslog (LOG_ERR,"readBlock: End of file during read") ;
            retCode = CONFIG_E_EOF ;
         }
         else {
            syslog (LOG_ERR,"readBlock: Error %d reading config", errno) ;
            retCode = CONFIG_E_SYSTEM ;
         }
         return (retCode) ;
      }

      if (*tPtr == '<') {
         if (tPtr[1] == '/') {
            wantBlock = 0 ;
            inBlock = 0 ;
         }
         else {
            syslog (LOG_ERR,"Syntax error, nested blocks not supported") ;
            return (CONFIG_E_NESTING) ;
         }
      }
      else if ((tPtr[0] != 0) && !((tPtr[0] == '/') && (tPtr[1] == '/'))) {
         offset = strlen (buffer) - 1 ;
         /* Ignore newline if there is one */
         if (buffer[offset] == '\n') {
            buffer[offset] = 0 ;
            offset-- ;
         }
         /* Check for continuation mark */
         if (buffer[offset] == '\\') {
            continuation = 1 ;
         }
         else {
            nitems = kvpParse (buffer) ;
            if (nitems < 0) {
/*               errMessage = kvpErrorString (kvpError()) ;*/
               syslog (LOG_ERR,"configLoad: kvpParse error: %s, parsing <%s>",
                         kvpErrorString (kvpError()), buffer) ;
               return (CONFIG_E_KVP) ;
            }
            continuation = 0 ;
         }
      }
   }
   return (CONFIG_E_OK) ;
}


static int parseBlockList (char* blockList)
{
   int length ;
   int nBlocks ;
   int offset ;

   /* Split blockList into separate strings */
   length = 0 ;
   nBlocks = 0 ;
   for (offset = 0 ; offset < strlen (blockList) + 1 ; offset++) {
      if ((blockList[offset] != ',') && (blockList[offset] != 0)) {
         blockName[nBlocks][length] = blockList[offset] ;
         length++ ;
      }
      else {
         /*hack by clfh*/
         blockName[nBlocks][length]='\0';
         length = 0 ;
         nBlocks++ ;
      }
   }
   return (nBlocks) ;
}


/********************************************************************
*
* configLoad - Load configuration parameters from file
*
* <Insert longer description here>
*
* RETURNS: 0 => success, -1 => error
*
*/
ConfigErrorCode configLoad (
                            const char* fileName,   /* Name of config file */
                            const char* blockList   /* Blocks we're interested in */
                            )
{
   char thisBlock[BLOCKNAME_MAX] ;
   int foundBlock[MAX_BLOCKS] ;
   int wantBlock ;
   int blocksRead ;
   int block ;
   int status;
   //   ConfigErrorCode retVal ;
   char* fileSpec ;

   nBlocks = parseBlockList ((char*)blockList) ;

   blocksRead = 0 ;
   for (block = 0 ; block < nBlocks ; block++) {
      foundBlock[block] = 0 ;
   }

   fileSpec = configFileSpec ((char*)fileName) ;
   syslog (LOG_DEBUG,"configLoad Reading params from: %s", fileSpec) ;

   config = fopen (fileSpec, "r") ;
   if (config != NULL) {
      blocksRead = 0 ;
      do {
         /* Skip to block that we want */
         wantBlock = 0 ;
         inBlock = 0 ;
         status = 0 ;
         while (!wantBlock && (status == 0)) {
            status = getTag (thisBlock) ;
            if (status == CONFIG_E_OK) {
               block = checkTag (thisBlock) ;
               if (block >= 0) {
                  foundBlock[block] = 1 ;
                  wantBlock = 1 ;
               }
            }
            else if (status == CONFIG_E_EOF) {
               status = CONFIG_E_SECTION ;
               for (block = 0 ; block < nBlocks ; block++) {
                  if (!foundBlock[block]) {
                     syslog (LOG_WARNING,"<%s> block not found", blockName[block]) ;
                  }
               }
            }
         }

         if (wantBlock) {
            status = readBlock () ;
            blocksRead++ ;
         }

      } while ((blocksRead < nBlocks) && (status == CONFIG_E_OK));
      fclose (config) ;
   }
   else {
      if (errno == ENOENT) {
         syslog (LOG_ERR,"configLoad: %s not found", fileSpec) ;
         status = CONFIG_E_NOFILE ;
      }
      else {
         syslog (LOG_ERR,"configLoad: error %d opening %s", errno, fileSpec) ;
         status = CONFIG_E_SYSTEM ;
      }
   }

   return ConfigErrorCode(status) ;
}



ConfigErrorCode configLoadFullPath (
                            char* fileName,   /* Name of config file */
                            char* blockList   /* Blocks we're interested in */
                            )
{
   char thisBlock[BLOCKNAME_MAX] ;
   int foundBlock[MAX_BLOCKS] ;
   int wantBlock ;
   int blocksRead ;
   int block ;
   int status ;
//   char* fileSpec ;

   nBlocks = parseBlockList (blockList) ;

   blocksRead = 0 ;
   for (block = 0 ; block < nBlocks ; block++) {
      foundBlock[block] = 0 ;
   }

//   fileSpec = configFileSpec (fileName) ;
   syslog (LOG_DEBUG,"configLoad Reading params from: %s", fileName) ;

   config = fopen (fileName, "r") ;
   if (config != NULL) {
      blocksRead = 0 ;
      do {
         /* Skip to block that we want */
         wantBlock = 0 ;
         inBlock = 0 ;
         status = 0 ;
         while (!wantBlock && (status == 0)) {
            status = getTag (thisBlock) ;
            if (status == CONFIG_E_OK) {
               block = checkTag (thisBlock) ;
               if (block >= 0) {
                  foundBlock[block] = 1 ;
                  wantBlock = 1 ;
               }
            }
            else if (status == CONFIG_E_EOF) {
               status = CONFIG_E_SECTION ;
               for (block = 0 ; block < nBlocks ; block++) {
                  if (!foundBlock[block]) {
                     syslog (LOG_WARNING,"<%s> block not found", blockName[block]) ;
                  }
               }
            }
         }

         if (wantBlock) {
            status = readBlock () ;
            blocksRead++ ;
         }

      } while ((blocksRead < nBlocks) && (status == CONFIG_E_OK));
      fclose (config) ;
   }
   else {
      if (errno == ENOENT) {
         syslog (LOG_ERR,"configLoad: %s not found", fileName) ;
         status = CONFIG_E_NOFILE ;
      }
      else {
         syslog (LOG_ERR,"configLoad: error %d opening %s", errno, fileName) ;
         status = CONFIG_E_SYSTEM ;
      }
   }

   return ConfigErrorCode(status) ;
}



ConfigErrorCode configValidate (
                                char* fileName   /* Name of config file */
                                )
{
   char thisBlock[BLOCKNAME_MAX] ;
   int blocksRead ;
   int status ;
   char* fileSpec ;
   KvpErrorCode errCode ;


   blocksRead = 0 ;

   fileSpec = configFileSpec (fileName) ;

   config = fopen (fileSpec, "r") ;
   if (config != NULL) {
      blocksRead = 0 ;
      kvpReset () ;
      do {
        status = getTag (thisBlock) ;
        if (status == CONFIG_E_OK) {
           printf ("Validating block <%s>\n", thisBlock) ;
           status = readBlock () ;
           if (status == CONFIG_E_OK) {
              printf ("Block <%s> validated OK\n", thisBlock) ;
           }
           else if (status == CONFIG_E_KVP) {
              errCode = kvpError () ;
              printf ("Error %d (%s) from kvpParse\n",
                      errCode, kvpErrorString (errCode)) ;
           }
           kvpReset () ;
           blocksRead++ ;
        }
        else if (status == CONFIG_E_EOF) {
          printf ("Read %d blocks of configuration from %s\n",
                  blocksRead, fileSpec) ;
          return (CONFIG_E_OK) ;
        }
      } while (status == CONFIG_E_OK) ;
   }
   else {
      if (errno == ENOENT) {
         printf ("configValidate: %s not found", fileSpec) ;
         status = CONFIG_E_NOFILE ;
      }
      else {
         printf ("configValidate: error %d opening %s", errno, fileSpec) ;
         status = CONFIG_E_SYSTEM ;
      }
   }
   return ConfigErrorCode(status) ;
}



/********************************************************************
*
* readBlocks - Read block names from file
*
* <Insert longer description here>
*
* RETURNS: 0 => success, -1 => error
*
*/
ConfigErrorCode readBlocks(char *fileName,char blockList[MAX_BLOCKS][BLOCKNAME_MAX],int *numBlocks)
{
   int status ;
   char* fileSpec ;
   char thisBlock[BLOCKNAME_MAX] ;
   int blockNum=0;
   fileSpec = configFileSpec (fileName) ;
   config = fopen (fileSpec, "r") ;
   
   if (config != NULL) {
      do {
	  status=0;
	  status = getTag (thisBlock) ;
	  if (status == CONFIG_E_OK) {
	      strncpy(blockList[blockNum],thisBlock,BLOCKNAME_MAX);
	      blockNum++;
	  }
      } while ( status == CONFIG_E_OK);
      fclose (config) ;
   }
   else {
      if (errno == ENOENT) {
         syslog (LOG_ERR,"readBlocks: %s not found", fileSpec) ;
         status = CONFIG_E_NOFILE ;
      }
      else {
         syslog (LOG_ERR,"readBlocks: error %d opening %s", errno, fileSpec) ;
         status = CONFIG_E_SYSTEM ;
      }
   }
   *numBlocks=blockNum;
   return ConfigErrorCode(status) ;
}
