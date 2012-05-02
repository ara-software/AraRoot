/* configFileSpec.c - Functions for expanding config file spec */

/* G.J.Crone, University College London */

/*
 * Current CVS Tag:
 * $Header: /work1/anitaCVS/flightSoft/common/configLib/configFileSpec.c,v 1.3 2005/06/15 16:22:00 rjn Exp $
 */
 
/*
DESCRIPTION
<Insert a description of the file/module here>
INCLUDE FILES: <place list of any relevant header files here>
*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "configLib.h"

/* defines */

/* typedefs */

/* globals */   

/* locals */


char* configFileSpec (
                      char* fileName
                      )
{
   static char fileSpec[FILENAME_MAX] ;
   char* configPath = 0 ;
   //   char *tempPath=0;
   configPath = getenv ("ARA_WEBPLOTTER_CONFIG_DIR") ;
   if(configPath == NULL) {
//       configPath = "." ;
     strcpy(fileSpec,"/home/anita/flightSoft/config");
   }
   else {
     strcpy (fileSpec, configPath) ;
   }
   strcat (fileSpec, "/") ;
   strcat (fileSpec, fileName) ;

   return (fileSpec) ;
}

