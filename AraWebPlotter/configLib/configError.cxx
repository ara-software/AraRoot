/* configError.c - Error reporting functions for config file handling */

/* G.J.Crone, University College London */

/*
 * Current CVS Tag:
 * $Header: /work1/anitaCVS/flightSoft/common/configLib/configError.c,v 1.1 2004/08/31 15:48:09 rjn Exp $
 */
 
/* 
 * Modification History : DO NOT EDIT - MAINTAINED BY CVS 
 * $Log: configError.c,v $
 * Revision 1.1  2004/08/31 15:48:09  rjn
 * Um... lots of additions. The biggest ones are switching from shared to static libraries, and the additon of configLib and kvpLib to read formatted config files. These two libraries were created by Gordon Crone (gjc@hep.ucl.ac.uk) and are in spirit released under the GPL license.
 * Also the first of the actually programs (Eventd) has been started. At the moment it just opens a socket to fakeAcqd. Other sockets and better handling of multiple sockets in socketLib are the next things on my to do list.
 *
 * Revision 1.2  2001/08/17 16:30:53  gjc
 * Return an error if a value is given outside the context of any section.
 *
 * Revision 1.1  2001/07/24 13:36:43  gjc
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

#include "configLib/configLib.h"
#include "kvpLib/keyValuePair.h"

/* defines */

/* typedefs */

/* globals */   

/* locals */

/* forward declarations */

const char* configErrorString (
                         ConfigErrorCode code
                         )
{
   char* string ;

   switch (code) {
   case CONFIG_E_OK :
      return "Success" ;
      break ;
   case CONFIG_E_NOFILE :
      return "File not found" ;
      break ;
   case CONFIG_E_NESTING :
      return "Nested sections not supported" ;
      break ;
   case CONFIG_E_EOF :
      return "End of file during parsing" ;
      break ;
   case CONFIG_E_SYSTEM :
      return "Error during system call" ;
      break ;
   case CONFIG_E_KVP :
      return "Error during kvpParse" ;
      break ;
   case CONFIG_E_SECTION :
      return "Requested section not found" ;
      break ;
   case CONFIG_E_UNNAMED :
      return "Unnamed sections not allowed" ;
      break ;
   default :
      return "Unknown error code" ;
   }

   return (string) ;
}


