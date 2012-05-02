/* configStore.c - <short description of this file/module> */

/* G.J.Crone, University College London */

/*
 * Current CVS Tag:
 * $Header: /work1/anitaCVS/flightSoft/common/configLib/configStore.c,v 1.4 2005/06/15 16:22:00 rjn Exp $
 */
 
/* 
 * Modification History : DO NOT EDIT - MAINTAINED BY CVS 
 * $Log: configStore.c,v $
 * Revision 1.4  2005/06/15 16:22:00  rjn
 * Fixed a lot of silly warnings
 *
 * Revision 1.3  2004/10/13 20:27:37  rjn
 * Added Mike's GPS code to the repository. And fixed a couple of minor bugs.
 *
 * Revision 1.2  2004/08/31 17:51:21  rjn
 * Stuck a load of syslog messages into the configLib stuff. Previosuly it used some strange wrapper around syslog, due to implementation issues on the operating system used by some parts of the MINOS daq.
 *
 * Revision 1.1  2004/08/31 15:48:09  rjn
 * Um... lots of additions. The biggest ones are switching from shared to static libraries, and the additon of configLib and kvpLib to read formatted config files. These two libraries were created by Gordon Crone (gjc@hep.ucl.ac.uk) and are in spirit released under the GPL license.
 * Also the first of the actually programs (Eventd) has been started. At the moment it just opens a socket to fakeAcqd. Other sockets and better handling of multiple sockets in socketLib are the next things on my to do list.
 *
 * Revision 1.1  2001/07/24 13:36:44  gjc
 * First check in of new package
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
#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "configLib/configLib.h"
#include "kvpLib/keyValuePair.h"

/* defines */

/* typedefs */

/* globals */   

/* locals */

/* forward declarations */


 int copyFile2(const char *theFile, const char *newFile) 
 { 
     //Only works on relative small files (will write a better version) 
     static int errorCounter=0;
     char *buffer;
     //Open the input file
     FILE *fin = fopen(theFile,"r");
     unsigned long numBytes;
     if(!fin) {
	 if(errorCounter<100) {
	     syslog(LOG_ERR,"Error reading file %s -- Error %s\n",
		   theFile,strerror(errno));
	     fprintf(stderr,"Error reading file %s-- Error %s\n",
		     theFile,strerror(errno));
	     errorCounter++;
	 }
	 return -1;
     }
     
     fseek(fin,0,SEEK_END);
     (numBytes)=ftell(fin);
     fseek(fin,SEEK_SET,0);
     buffer=(char*)malloc((numBytes));
     fread(buffer,1,(numBytes),fin);
     fclose(fin);
     
     //Open the output file 
     FILE *fout = fopen(newFile,"w"); 
     if(!fout) { 
 	if(errorCounter<100) { 
 	    syslog(LOG_ERR,"Couldn't open %s for copy of %s -- Error %s\n", 
 		   newFile,theFile,strerror(errno)); 
 	    fprintf(stderr,"Couldn't open %s for copy of %s -- Error %s\n", 
 		    newFile,theFile,strerror(errno)); 
 	    errorCounter++; 
 	} 
 	return -1; 
     } 
     fwrite(buffer,1,numBytes,fout); 
     fclose(fout); 
     free(buffer); 
     return 0; 
 } 



ConfigErrorCode configSwitchToLast(char *configFile, time_t *rawTimePtr) {    
    
//    int retVal=0;
    //   char* errMessage ;
    char configPath[FILENAME_MAX] ;
    struct passwd *myPwent ;
    int myUid ;
    struct stat fileStat ;
    char oldFileSpec[FILENAME_MAX] ;
    char lastFileSpec[FILENAME_MAX];
    char archiveFileSpec[FILENAME_MAX] ;
    char newFileSpec[FILENAME_MAX] ;

    char *tempPath =getenv ("ARA_WEBPLOTTER_CONFIG_DIR") ;
    if (tempPath == NULL) {
	/* Environment variable not set so use current dir if there's
	   already a config file here */
      if (stat (configFile, &fileStat) == 0) {
	strcpy(configPath,"./");
      }
      else {
	/* Finally default to home directory */
	myUid = getuid () ;
	myPwent = getpwuid (myUid) ;
	strcpy(configPath,myPwent->pw_dir) ;
      }
    }
    else {
      strcpy(configPath,tempPath);
    }


     time ( rawTimePtr );

     strcpy (oldFileSpec, configPath) ;
     strcat (oldFileSpec, "/") ;
     strcat (oldFileSpec, configFile) ;

     strcpy (lastFileSpec, configPath) ;
     strcat (lastFileSpec, "/previous/") ;
     strcat (lastFileSpec, configFile) ;

     sprintf(archiveFileSpec,"%s/archive/%s.%ld",configPath,configFile,*rawTimePtr);
     sprintf(newFileSpec,"%s/%s.temp",configPath,configFile);


    if(stat (lastFileSpec, &fileStat) != 0) {
	syslog(LOG_ERR,"configSwitch couldn't find: %s",lastFileSpec);
	return CONFIG_E_NOFILE;
    }
    if(stat (oldFileSpec, &fileStat) != 0) {
	syslog(LOG_ERR,"configSwitch couldn't find: %s",oldFileSpec);
	return CONFIG_E_NOFILE;
    }

    // mv lastFileSpec newFileSpec
    // cp oldFileSpec lastFileSpec
    // mv oldFileSpec archiveFileSpec
    // mv newFileSpec oldFileSpec
    copyFile2(lastFileSpec,newFileSpec);
    copyFile2(oldFileSpec,lastFileSpec);
    copyFile2(oldFileSpec,archiveFileSpec);
    copyFile2(newFileSpec,oldFileSpec);
    unlink(newFileSpec);

    syslog(LOG_INFO,"configSwitch arcvhived: %s", archiveFileSpec);   
    return CONFIG_E_OK;

 }



 ConfigErrorCode configSwitch(char *configFile, char whichConfig, time_t *rawTimePtr) {    

 //    int retVal=0;
     //   char* errMessage ;

    char configPath[FILENAME_MAX] ;
    struct passwd *myPwent ;
    int myUid ;
    struct stat fileStat ;
    char oldFileSpec[FILENAME_MAX] ;
    char lastFileSpec[FILENAME_MAX];
    char archiveFileSpec[FILENAME_MAX] ;
    char newFileSpec[FILENAME_MAX] ;

    char *tempPath =getenv ("ARA_WEBPLOTTER_CONFIG_DIR") ;
    if (tempPath == NULL) {
	/* Environment variable not set so use current dir if there's
	   already a config file here */
      if (stat (configFile, &fileStat) == 0) {
	strcpy(configPath,"./");
      }
      else {
	/* Finally default to home directory */
	myUid = getuid () ;
	myPwent = getpwuid (myUid) ;
	strcpy(configPath,myPwent->pw_dir) ;
      }
    }
    else {
      strcpy(configPath,tempPath);
    }



    time ( rawTimePtr );

    strcpy (oldFileSpec, configPath) ;
    strcat (oldFileSpec, "/") ;
    strcat (oldFileSpec, configFile) ;

    strcpy (lastFileSpec, configPath) ;
    strcat (lastFileSpec, "/previous/") ;
    strcat (lastFileSpec, configFile) ;

    sprintf(archiveFileSpec,"%s/archive/%s.%ld",configPath,configFile,*rawTimePtr);
    if(whichConfig>0) 
	sprintf(newFileSpec,"%s/defaults/%s.%d",configPath,configFile,whichConfig);
    else
	sprintf(newFileSpec,"%s/defaults/%s",configPath,configFile);


    if(stat (newFileSpec, &fileStat) != 0) {
	syslog(LOG_ERR,"configSwitch couldn't find: %s",newFileSpec);
	return CONFIG_E_NOFILE;
    }
    if(stat (oldFileSpec, &fileStat) != 0) {
	syslog(LOG_ERR,"configSwitch couldn't find: %s",oldFileSpec);
	return CONFIG_E_NOFILE;
    }

    // cp oldFileSpec lastFileSpec
    // mv oldFileSpec archiveFileSpec
    // cp newFileSpec oldFileSpec
    copyFile2(oldFileSpec,lastFileSpec);
    rename(oldFileSpec,archiveFileSpec);
    copyFile2(newFileSpec,oldFileSpec);

    syslog(LOG_INFO,"configSwitch arcvhived: %s", archiveFileSpec);   
    return CONFIG_E_OK;

 }



 /********************************************************************
 *
 * configReplace - Copies one file to another and archives replaced file
 *
 * <Insert longer description here>
 *
 * RETURNS: 0 => success,  -1 => failure
 *
 */
 ConfigErrorCode configReplace(char *oldFileName, char *newFileName, time_t *rawTimePtr)
 {
 /*     printf("configReplace %s %s\n",oldFileName,newFileName); */
 //    int retVal=0;
    //   char* errMessage ;


    char configPath[FILENAME_MAX] ;
    struct passwd *myPwent ;
    int myUid ;
    struct stat fileStat ;
    char oldFileSpec[FILENAME_MAX] ;
    char lastFileSpec[FILENAME_MAX];
    char archiveFileSpec[FILENAME_MAX] ;
    char newFileSpec[FILENAME_MAX] ;

    char *tempPath =getenv ("ARA_WEBPLOTTER_CONFIG_DIR") ;
    if (tempPath == NULL) {
	/* Environment variable not set so use current dir if there's
	   already a config file here */
      if (stat (oldFileName, &fileStat) == 0) {
	strcpy(configPath,"./");
      }
      else {
	/* Finally default to home directory */
	myUid = getuid () ;
	myPwent = getpwuid (myUid) ;
	strcpy(configPath,myPwent->pw_dir) ;
      }
    }
    else {
      strcpy(configPath,tempPath);
    }


    time ( rawTimePtr );

    strcpy (oldFileSpec, configPath) ;
    strcat (oldFileSpec, "/") ;
    strcat (oldFileSpec, oldFileName) ;

    strcpy (lastFileSpec, configPath) ;
    strcat (lastFileSpec, "/previous/") ;
    strcat (lastFileSpec, oldFileName) ;


    if(rawTimePtr!=NULL)
       sprintf(archiveFileSpec,"%s/archive/%s.%ld",configPath,oldFileName,*rawTimePtr);


    strcpy (newFileSpec, configPath) ;
    strcat (newFileSpec, "/") ;
    strcat (newFileSpec, newFileName) ;



    if(stat (newFileSpec, &fileStat) != 0) {
	syslog(LOG_ERR,"configReplace couldn't find: %s",newFileSpec);
	return CONFIG_E_NOFILE;
    }
    if(stat (oldFileSpec, &fileStat) != 0) {
	syslog(LOG_ERR,"configReplace couldn't find: %s",oldFileSpec);
	return CONFIG_E_NOFILE;
    }

    // cp oldFileSpec lastFileSpec
    // mv oldFileSpec archiveFileSpec
    // mv newFileSpec oldFileSpec
    if(rawTimePtr) {
	copyFile2(oldFileSpec,lastFileSpec);
	rename(oldFileSpec,archiveFileSpec);
	rename(newFileSpec,oldFileSpec);
    }
    else {
	copyFile2(newFileSpec,oldFileSpec);
	unlink(newFileSpec);
    }

 //   unlink(newFileSpec);


 /*    sprintf(mvCommand,"cp %s %s\n",oldFileSpec,lastFileSpec); */
 /*    retVal=system(mvCommand); */
 /*    if(retVal<0) { */
 /*        syslog(LOG_ERR,"Problem storing config: %s %s ",archiveFileSpec,strerror(errno)); */
 /*        return CONFIG_E_SYSTEM; */
 /*    } */
 /*    sprintf(mvCommand,"mv %s %s\n",oldFileSpec,archiveFileSpec); */
 /*    retVal=system(mvCommand); */
 /*    if(retVal<0) { */
 /*        syslog(LOG_ERR,"Problem archiving config: %s %s ",archiveFileSpec,strerror(errno)); */
 /*        return CONFIG_E_SYSTEM; */
 /*    } */
 /*    sprintf(mvCommand,"mv %s %s\n",newFileSpec,oldFileSpec); */
 /*    retVal=system(mvCommand); */
 /*    if(retVal<0) { */
 /*        syslog(LOG_ERR,"Problem replacing config: %s %s ",archiveFileSpec,strerror(errno)); */
 /*        return CONFIG_E_SYSTEM; */
 /*    } */
    syslog(LOG_INFO,"configReplace arcvhived: %s", archiveFileSpec);   
    return CONFIG_E_OK;
 }

 /********************************************************************
 *
 * storeConfig - Write current contents of kvp buffer to file
 *
 * <Insert longer description here>
 *
 * RETURNS: 0 => success,  -1 => failure
 *
 */
 ConfigErrorCode configStore (
			      char* fileName,
			      char* blockName
			      )
 {
    int config ;
    //   char* errMessage ;
    char configPath[FILENAME_MAX]; ;
    struct passwd *myPwent ;
    int myUid ;
    struct stat fileStat ;
    char fileSpec[FILENAME_MAX] ;
    int status ;
    char tag[BLOCKNAME_MAX+3] ;

    char *tempPath = getenv ("ARA_WEBPLOTTER_CONFIG_DIR") ;
    if (tempPath == NULL) {
       /* Environment variable not set so use current dir if there's
	  already a config file here */
       if (stat (fileName, &fileStat) == 0) {
	 strcpy(configPath,"./") ;
       }
       else {
	  /* Finally default to home directory */
	  myUid = getuid () ;
	  myPwent = getpwuid (myUid) ;
	  strcpy(configPath,myPwent->pw_dir);
       }
    }
    else {
      strcpy(configPath,tempPath);
    }

    strcpy (fileSpec, configPath) ;
    strcat (fileSpec, "/") ;
    strcat (fileSpec, fileName) ;
    syslog(LOG_INFO,"storeConfig writing params to: %s", fileSpec) ;

    config = creat (fileSpec, 0664) ;
    if (config == -1) {
       syslog (LOG_ERR,"storeConfig: error %d creating %s", errno, fileSpec) ;
       return (CONFIG_E_SYSTEM) ;
    }
    else {
       snprintf (tag, BLOCKNAME_MAX+2, "<%s>\n", blockName) ;
       write (config, tag, strlen (tag)) ;
       status = kvpWrite (config) ;
       if (status == -1) {
	   /*errMessage = kvpErrorString (kvpError()) ;*/
	  syslog (LOG_ERR,"storeConfig: kvpWrite failed %s",kvpErrorString (kvpError()) ) ;
	  close(config);
	  return (CONFIG_E_KVP) ;
       }
       snprintf (tag, BLOCKNAME_MAX+3, "\n</%s>\n", blockName) ;
       write (config, tag, strlen (tag)) ;
       close(config);
       return (CONFIG_E_OK) ;
    }
 }

 ConfigErrorCode configAppend (
			      char* fileName,
			      char* blockName
			      )
 {
    int config ;
    //   char* errMessage ;
    char configPath[FILENAME_MAX] ;
    struct passwd *myPwent ;
    int myUid ;
    struct stat fileStat ;
    char fileSpec[FILENAME_MAX] ;
    int status ;
    char tag[BLOCKNAME_MAX+3] ;

    char *tempPath = getenv ("ARA_WEBPLOTTER_CONFIG_DIR") ;
    if (tempPath == NULL) {
       /* Environment variable not set so use current dir if there's
	  already a config file here */
       if (stat (fileName, &fileStat) == 0) {
	 strcpy(configPath,"./" );
       }
       else {
	  /* Finally default to home directory */
	  myUid = getuid () ;
	  myPwent = getpwuid (myUid) ;
	  strcpy(configPath,myPwent->pw_dir );
       }
    }
    else {
      strcpy(configPath,tempPath);
    }

    strcpy (fileSpec, configPath) ;
    strcat (fileSpec, "/") ;
    strcat (fileSpec, fileName) ;
    syslog(LOG_INFO,"configAppend writing params to: %s", fileSpec) ;

    config = open (fileSpec,O_CREAT|O_WRONLY|O_APPEND,0664) ;
    if (config == -1) {
       syslog (LOG_ERR,"configAppend: error %d creating %s", errno, fileSpec) ;
       return (CONFIG_E_SYSTEM) ;
    }
    else {
       snprintf (tag, BLOCKNAME_MAX+2, "\n\n<%s>\n", blockName) ;
       write (config, tag, strlen (tag)) ;
       status = kvpWrite (config) ;
       if (status == -1) {
	   /*errMessage = kvpErrorString (kvpError()) ;*/
	  syslog (LOG_ERR,"configAppend: kvpWrite failed %s",kvpErrorString (kvpError()) ) ;
	  close(config);
	  return (CONFIG_E_KVP) ;
       }
       snprintf (tag, BLOCKNAME_MAX+3, "\n</%s>\n", blockName) ;
       write (config, tag, strlen (tag)) ;
       close(config);
       return (CONFIG_E_OK) ;
    }
 }



 ConfigErrorCode configModifyInt(char *fileName,char *blockName,char *key,int value, time_t *rawTimePtr)
 /* Will tidy it up so that 
 Does exactly what it says on the tin */
 {
     /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
 //    char* eString;
     char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
     int numBlocks=0,blockNum;
     char tempFile[FILENAME_MAX];
     sprintf(tempFile,"%s.new",fileName);
     /* Load Config */
     kvpReset () ;
     readBlocks(fileName,blockList,&numBlocks);
     for(blockNum=0;blockNum<numBlocks;blockNum++) {
	 kvpReset () ;
	 status = configLoad (fileName,blockList[blockNum]) ;
	 if(status == CONFIG_E_OK) {	
	     if (strcmp (blockName, blockList[blockNum]) == 0) {
 //		printf("Here\n");
		 kvpStatus=kvpUpdateInt(key,value);
		 if(kvpStatus!=KVP_E_OK) {
		     printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		 }
	     }
 //	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	     if(blockNum==0)
		 status = configStore(tempFile,blockList[blockNum]);
	     else
		 status = configAppend(tempFile,blockList[blockNum]);
	     if(status != CONFIG_E_OK) {
		 printf("Bugger\n");
	     }
	 }
     }
     return configReplace(fileName,tempFile,rawTimePtr);
 }


 ConfigErrorCode configModifyIntArray(char *fileName,char *blockName,char *key,int *values, int length, time_t *rawTimePtr)
 /* Will tidy it up so that 
 Does exactly what it says on the tin */
 {
     /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
 //    char* eString;
     char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
     int numBlocks=0,blockNum;
     char tempFile[FILENAME_MAX];
     sprintf(tempFile,"%s.new",fileName);
     /* Load Config */
     kvpReset () ;
     readBlocks(fileName,blockList,&numBlocks);
     for(blockNum=0;blockNum<numBlocks;blockNum++) {
	 kvpReset () ;
	 status = configLoad (fileName,blockList[blockNum]) ;
	 if(status == CONFIG_E_OK) {	
	     if (strcmp (blockName, blockList[blockNum]) == 0) {
 //		printf("Here\n");
		 kvpStatus=kvpUpdateIntArray(key,values,length);
		 if(kvpStatus!=KVP_E_OK) {
		     printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		 }
	     }
 //	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	     if(blockNum==0)
		 status = configStore(tempFile,blockList[blockNum]);
	     else
		 status = configAppend(tempFile,blockList[blockNum]);
	     if(status != CONFIG_E_OK) {
		 printf("Bugger\n");
	     }
	 }
     }
     return configReplace(fileName,tempFile,rawTimePtr);
 }

 ConfigErrorCode configModifyUnsignedIntArray(char *fileName,char *blockName,char *key,unsigned int *values, int length, time_t *rawTimePtr)
 {
     /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
 //    char* eString;
     char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
     int numBlocks=0,blockNum;
     char tempFile[FILENAME_MAX];
     sprintf(tempFile,"%s.new",fileName);
     /* Load Config */
     kvpReset () ;
     readBlocks(fileName,blockList,&numBlocks);
     for(blockNum=0;blockNum<numBlocks;blockNum++) {
	 kvpReset () ;
	 status = configLoad (fileName,blockList[blockNum]) ;
	 if(status == CONFIG_E_OK) {	
	     if (strcmp (blockName, blockList[blockNum]) == 0) {
 //		printf("Here\n");
		 kvpStatus=kvpUpdateUnsignedIntArray(key,values,length);
		 if(kvpStatus!=KVP_E_OK) {
		     printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		 }
	     }
 //	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	     if(blockNum==0)
		 status = configStore(tempFile,blockList[blockNum]);
	     else
		 status = configAppend(tempFile,blockList[blockNum]);
	     if(status != CONFIG_E_OK) {
		 printf("Bugger\n");
	     }
	 }
     }
     return configReplace(fileName,tempFile,rawTimePtr);
 }

 ConfigErrorCode configModifyFloatArray(char *fileName,char *blockName,char *key,float *values, int length, time_t *rawTimePtr) 
 {
    /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
 //    char* eString;
     char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
     int numBlocks=0,blockNum;
     char tempFile[FILENAME_MAX];
     sprintf(tempFile,"%s.new",fileName);
     /* Load Config */
     kvpReset () ;
     readBlocks(fileName,blockList,&numBlocks);
     for(blockNum=0;blockNum<numBlocks;blockNum++) {
	 kvpReset () ;
	 status = configLoad (fileName,blockList[blockNum]) ;
	 if(status == CONFIG_E_OK) {	
	     if (strcmp (blockName, blockList[blockNum]) == 0) {
 //		printf("Here\n");
		 kvpStatus=kvpUpdateFloatArray(key,values,length);
		 if(kvpStatus!=KVP_E_OK) {
		     printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		 }
	     }
 //	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	     if(blockNum==0)
		 status = configStore(tempFile,blockList[blockNum]);
	     else
		 status = configAppend(tempFile,blockList[blockNum]);
	     if(status != CONFIG_E_OK) {
		 printf("Bugger\n");
	     }
	 }
     }
     return configReplace(fileName,tempFile,rawTimePtr);

 }



 ConfigErrorCode configModifyUnsignedInt(char *fileName,char *blockName,char *key,unsigned long value, time_t *rawTimePtr)
 /* Will tidy it up so that 
 Does exactly what it says on the tin */
 {
     /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
 //    char* eString;
     char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
     int numBlocks=0,blockNum;
     char tempFile[FILENAME_MAX];
     sprintf(tempFile,"%s.new",fileName);
     /* Load Config */
     kvpReset () ;
     readBlocks(fileName,blockList,&numBlocks);
     for(blockNum=0;blockNum<numBlocks;blockNum++) {
	 kvpReset () ;
	 status = configLoad (fileName,blockList[blockNum]) ;
	 if(status == CONFIG_E_OK) {	
	     if (strcmp (blockName, blockList[blockNum]) == 0) {
 //		printf("Here\n");
		 kvpStatus=kvpUpdateUnsignedInt(key,value);
		 if(kvpStatus!=KVP_E_OK) {
		     printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		 }
	     }
 //	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	     if(blockNum==0)
		 status = configStore(tempFile,blockList[blockNum]);
	     else
		 status = configAppend(tempFile,blockList[blockNum]);
	     if(status != CONFIG_E_OK) {
		 printf("Bugger\n");
	     }
	 }
     }
     return configReplace(fileName,tempFile,rawTimePtr);
 }



 ConfigErrorCode configModifyFloat(char *fileName,char *blockName,char *key,float value, time_t *rawTimePtr)
 /* Will tidy it up so that 
 Does exactly what it says on the tin */
 {
     /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
 //    char* eString;
     char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
     int numBlocks=0,blockNum;
     char tempFile[FILENAME_MAX];
     sprintf(tempFile,"%s.new",fileName);
     /* Load Config */
     kvpReset () ;
     readBlocks(fileName,blockList,&numBlocks);
     for(blockNum=0;blockNum<numBlocks;blockNum++) {
	 kvpReset () ;
	 status = configLoad (fileName,blockList[blockNum]) ;
	 if(status == CONFIG_E_OK) {	
	     if (strcmp (blockName, blockList[blockNum]) == 0) {
 //		printf("Here\n");
		 kvpStatus=kvpUpdateFloat(key,value);
		 if(kvpStatus!=KVP_E_OK) {
		     printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		 }
	     }
 //	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	     if(blockNum==0)
		 status = configStore(tempFile,blockList[blockNum]);
	     else
		 status = configAppend(tempFile,blockList[blockNum]);
	     if(status != CONFIG_E_OK) {
		 printf("Bugger\n");
	     }
	 }
     }
     return configReplace(fileName,tempFile,rawTimePtr);
 }

 ConfigErrorCode configModifyString(char *fileName,char *blockName,char *key,char *value, time_t *rawTimePtr)
 /* Will tidy it up so that 
 Does exactly what it says on the tin */
 {
     /* Config file thingies */
     int status=0;
     KvpErrorCode kvpStatus=KVP_E_OK;
//    char* eString;
    char blockList[MAX_BLOCKS][BLOCKNAME_MAX];
    int numBlocks=0,blockNum;
    char tempFile[FILENAME_MAX];
    sprintf(tempFile,"%s.new",fileName);
    /* Load Config */
    kvpReset () ;
    readBlocks(fileName,blockList,&numBlocks);
    for(blockNum=0;blockNum<numBlocks;blockNum++) {
	kvpReset () ;
	status = configLoad (fileName,blockList[blockNum]) ;
	if(status == CONFIG_E_OK) {	
	    if (strcmp (blockName, blockList[blockNum]) == 0) {
//		printf("Here\n");
		kvpStatus=kvpUpdateString(key,value);
		if(kvpStatus!=KVP_E_OK) {
		    printf("%d\t%s\n",kvpStatus,kvpErrorString(kvpStatus));
		}
	    }
//	    printf("%s\t%s\n",blockName,blockList[blockNum]);
	    if(blockNum==0)
		status = configStore(tempFile,blockList[blockNum]);
	    else
		status = configAppend(tempFile,blockList[blockNum]);
	    if(status != CONFIG_E_OK) {
		printf("Bugger\n");
	    }
	}
    }
    return configReplace(fileName,tempFile,rawTimePtr);

}
