/* 
   Utility functions, mostly file I/O and file handling

   M2 Consulting, July 2010
   ped@m2-consult.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <libgen.h>

#define _GNU_SOURCE
#include <string.h>

#include "araSoft.h"
#include "fileWriterUtil.h"

void initWriter(ARAWriterStruct_t* writer, 
                int runNumber,
                int compression, 
		int maxFiles, int maxEvents, const char* filePrefix,
		const char* topDir, const char* linkDir ){
//ARA_LOG_MESSAGE(LOG_INFO,"utilLib::initWriter() - runNumber = %d",runNumber);
  writer->currentRunNumber = runNumber;
  writer->currentFilePtr = 0;
  writer->maxEvents = maxEvents;
  writer->maxFiles = maxFiles;
  writer->writeCount = maxEvents+1; // This triggers creation of new file 
  writer->fileCount = maxFiles+1;   // This triggers creation of new sub dir
  writer->dirCount = 0;
  sprintf(writer->compressionLevel,"w%d",compression);
  strcpy(writer->filePrefix,filePrefix);
  strcpy(writer->currentDirName,topDir);
  writer->linkDir = linkDir;
}

void closeWriter(ARAWriterStruct_t* writer){
  if( writer->currentFilePtr ){
    gzflush(writer->currentFilePtr,Z_FINISH);
    gzclose(writer->currentFilePtr);
    if( writer->linkDir )
      makeLink(writer->currentFileName,writer->linkDir);
    // This forces new file if writer is used again
    writer->writeCount = writer->maxEvents;
    writer->currentFilePtr = 0;
  }
}

int newWriterFile(ARAWriterStruct_t* writer){
  struct timeval timeStruct;
  if( writer->currentFilePtr ){
    gzclose(writer->currentFilePtr);
    if( writer->linkDir)
      makeLink(writer->currentFileName,writer->linkDir);
  }
  if(writer->fileCount >= writer->maxFiles ){
    int retVal = newWriterSubDir(writer);
    if( retVal ){
      fprintf(stderr,"Failed to make new sub directory needed for new file");
      return retVal;
    }
    writer->fileCount = 0;
  }
//ARA_LOG_MESSAGE(LOG_INFO,"Hello 1 -- %s",writer->filePrefix);
  gettimeofday(&timeStruct,NULL);
  (writer->startTime).tv_sec  = timeStruct.tv_sec;
  (writer->startTime).tv_usec = timeStruct.tv_usec;
//ARA_LOG_MESSAGE(LOG_INFO,"Hello 2 -- %s",writer->filePrefix);
  sprintf(writer->currentFileName,"%s/%s_%u.%06u.run%6.6d.dat",
	  writer->currentSubDirName,writer->filePrefix,
	  (unsigned int) writer->startTime.tv_sec,(unsigned int) writer->startTime.tv_usec,
	  writer->currentRunNumber
	  );
//ARA_LOG_MESSAGE(LOG_INFO,"Hello 3 -- %s",writer->filePrefix);
  writer->currentFilePtr = gzopen(writer->currentFileName,writer->compressionLevel);
  if( !writer->currentFilePtr ){
    fprintf(stderr,"Failed to open file %s:\t%s",writer->currentFileName,strerror(errno));
    return errno;
  }
  writer->fileCount++;
//ARA_LOG_MESSAGE(LOG_INFO,"Hello 4 -- %s",writer->filePrefix);
  return 0;
}

int newWriterSubDir(ARAWriterStruct_t* writer){
  struct timeval timeStruct;
  gettimeofday(&timeStruct,NULL);
  sprintf(writer->currentSubDirName,"%s/%s_%u",
	  writer->currentDirName,
	  writer->filePrefix,
	  (unsigned int)timeStruct.tv_sec);
  return makeDirectories(writer->currentSubDirName);
}

int writeBuffer(ARAWriterStruct_t* writer, char* buffer, int len, int *new_file_flag ){
  int retVal = 0;
  *new_file_flag = 0;   /* = false; clear new_file_flag */
  if( writer->writeCount >= writer->maxEvents ){
//  ARA_LOG_MESSAGE(LOG_INFO,"writeBuffer() - new output file started");
    retVal = newWriterFile(writer);
    if( retVal )
      return retVal;
    writer->writeCount = 0; 
    *new_file_flag = 1; /* = true; set new_file_flag    */
  }
  retVal = gzwrite(writer->currentFilePtr,buffer,len);
  if(retVal<=0){
    int error;
    const char* errStr = gzerror(writer->currentFilePtr,&error);
    fprintf(stderr,"Error writing to file %s;\t%s (%d)",
	   writer->currentFileName,errStr,error);

  }
  writer->writeCount++;

  if(retVal<len)
    fprintf(stderr,"Incomplete buffer %d in file %s; %d of %d bytes written",
	   writer->writeCount,writer->currentFileName,retVal,len);

  return retVal;
}

int removeFile(const char *theFile)
{
  static int errorCounter=0;
  int retVal=unlink(theFile);
  if(retVal!=0) {
    if(errorCounter<100) {
      fprintf(stderr,"Error (%d of 100) removing %s:\t%s",errorCounter,theFile,strerror(errno));
      errorCounter++;
    }
  }
  return retVal;
}

int is_dir(const char *path)
{
  struct stat s;
  if (stat(path, &s))
    return 0;
 
  return S_ISDIR(s.st_mode) ? 1 : 0;
}


int makeDirectories(const char *theTmpDir) 
{
  static int errorCounter = 0;

  char copyDir[FILENAME_MAX];
  char newDir[FILENAME_MAX];
  char *subDir;
  int retVal;
    
  strncpy(copyDir,theTmpDir,FILENAME_MAX);

  strcpy(newDir,"");

  subDir = strtok(copyDir,"/");
  while(subDir != NULL) {
    sprintf(newDir,"%s/%s",newDir,subDir);
    retVal=is_dir(newDir);
    if(!retVal) {	
      retVal=mkdir(newDir,0777);
      if(retVal<0){
	if(errorCounter<100){
	  errorCounter++;
	  fprintf(stderr,"Error (%d of 100) making directory %s:\t%s",errorCounter,newDir,strerror(errno));
	}
	break;
      }
    }
    subDir = strtok(NULL,"/");
  }
  return retVal;
}

int makeLink(const char *theFile, const char *theLinkDir)
{
  static int errorCounter=0;
  char *justFile=basename((char*)theFile);
  char newFile[FILENAME_MAX];
  sprintf(newFile,"%s/%s",theLinkDir,justFile);
  //    printf("Linking %s to %s\n",theFile,newFile);
  int retVal=symlink(theFile,newFile);
  if(retVal!=0) {
    if(errorCounter<100) {
      errorCounter++;
      fprintf(stderr,"Error (%d of 100) linking %s to %s:\t%s",errorCounter,theFile,newFile,strerror(errno));
    }
  }
  return retVal;
}

int moveFile(const char *theFile, const char *theDir)
{
  static int errorCounter=0;
  int retVal;
  char *justFile=basename((char *)theFile);
  char newFile[FILENAME_MAX];
  sprintf(newFile,"%s/%s",theDir,justFile);
  retVal=rename(theFile,newFile);
  if(!retVal && errorCounter<100){
    errorCounter++;
    fprintf(stderr,"Error (%d of 100) moving file %s to %s:\t%s",errorCounter,theFile,newFile,strerror(errno));
  }
  return retVal;    
}

int checkPidFile(const char *fileName)
{
  int retVal=0;
  FILE *fpPid ;
  char procDir[FILENAME_MAX];
  
  //    pid_t thePid;
  int theNaughtyPid;
  if (!(fpPid=fopen(fileName, "r"))) {
    return 0;
  }else {
    //There shouldn't be a pidFile here
    retVal=fscanf(fpPid,"%d", &theNaughtyPid) ;
    fclose(fpPid);
    if(retVal>0) {  
      //Now check to see the process is running
      retVal=sprintf(procDir,"/proc/%d",theNaughtyPid);
      retVal=access(procDir,F_OK);
      if(retVal==0) {
	//The process is running
	return theNaughtyPid;
      } else {
	fprintf(stderr,"%s exists but PID %d is not running, will delete the file and start new process",fileName,theNaughtyPid);
	retVal=unlink(fileName);
	if(retVal<0) {
	  fprintf(stderr,"Error trying to unlink %s -- %s",fileName,strerror(errno));
	}
	return 0;
      }
    }
  }
  return -1;
}

int writePidFile(const char *fileName)
{
  FILE *fpPid ;
  char theDir[FILENAME_MAX];
  char tempFileName[FILENAME_MAX];
  strncpy(tempFileName,fileName,FILENAME_MAX-1);
  strncpy(theDir,dirname(tempFileName),FILENAME_MAX-1);
  makeDirectories(theDir);
  if(!(fpPid=fopen(fileName, "w"))) {
    fprintf(stderr,"Failed to open a file for PID, %s\n", fileName);
    return 1;
  }
  fprintf(fpPid,"%d\n", getpid()) ;
  fclose(fpPid) ;
  return 0;
}




/** Time handling */
void setNextTime(struct timeval* nextTime, const struct timeval* lastTime, 
                 const struct timeval* timeInterval)
{
  nextTime->tv_sec = lastTime->tv_sec + timeInterval->tv_sec;
  time_t tempusec = lastTime->tv_usec + timeInterval->tv_usec;
  if( tempusec > 1000000 ){
    nextTime->tv_sec += 1;
    tempusec -= 1000000;
  }
  nextTime->tv_usec = tempusec;
}

void setTimeInterval(struct timeval* timeInterval, double step)
{
  timeInterval->tv_sec = (time_t)step;
  timeInterval->tv_usec = (suseconds_t)((step-timeInterval->tv_sec)*1e6);
}

void getFileStartTime( ARAWriterStruct_t* writer , unsigned int *second , unsigned int *usec )
{

  *second = (unsigned int) (writer->startTime).tv_sec;
  *usec   = (unsigned int) (writer->startTime).tv_usec;

  return;

} // end of getter function to return the start time for the current run file

uint32_t getRunNumber() {
  int retVal=0;
  uint32_t runNumber=0;
 
  FILE *pFile;    
  pFile = fopen (ARA_RUN_NUMBER_FILE, "r");
  if(!pFile) {
    //    ARA_LOG_MESSAGE (LOG_ERR,"Couldn't open %s",ARA_RUN_NUMBER_FILE);
    return -1;
  }

  retVal=fscanf(pFile,"%u",&runNumber);
  if(retVal<0) {
    fprintf(stderr,"fscanff: %s ---  %s\n",strerror(errno),
            ARA_RUN_NUMBER_FILE);
  }
  fclose (pFile);
  return runNumber;
    
}


void fillGenericHeader(void *thePtr, AraDataStructureType_t type, unsigned int numBytes)
{
  unsigned int intBytes=(numBytes-sizeof(AtriGenericHeader_t))/4;
  AtriGenericHeader_t *gHdr= (AtriGenericHeader_t*)thePtr;
  unsigned int *dataPtr=(unsigned int*) (thePtr+sizeof(AtriGenericHeader_t));
  gHdr->typeId=type;
  gHdr->verId=ARA_SOFT_VERISON;
  gHdr->subVerId=ARA_SOFT_SUB_VERISON;
  gHdr->stationId=THIS_STATION;
  gHdr->reserved=0xfefe; ///For now
  gHdr->numBytes=numBytes;
  gHdr->checksum=simpleIntCrc(dataPtr,intBytes);
 
}


#define PKT_E_CHECKSUM 0x1
#define PKT_E_CODE 0x2
#define PKT_E_FEBYTE 0x4
#define PKT_E_SIZE 0x8


int checkPacket(void *thePtr)
//0 means no errors
//1 means checksum mismatch
//2 means unknown packet code
//8 means packet size mismatch
{

  int retVal=0,dataSize=0;
  AtriGenericHeader_t *gHdr= (AtriGenericHeader_t*)thePtr;
  unsigned int intBytes=(gHdr->numBytes-sizeof(AtriGenericHeader_t))/4;
  unsigned int *dataPtr=(unsigned int*) (thePtr+sizeof(AtriGenericHeader_t)); 
  unsigned int checksum=0;
  
  //Not sure why the <4000 was here
  //  if(intBytes<4000)
  checksum=simpleIntCrc(dataPtr,intBytes);
  if(checksum!=gHdr->checksum) {
    printf("Checksum Mismatch (possibly %s (%#x)) (%u bytes) %u -- %u \n",
	   araDataTypeAsString(gHdr->typeId),gHdr->typeId,intBytes,checksum,gHdr->checksum);     
    retVal+=PKT_E_CHECKSUM;             
  }

  switch(gHdr->typeId) {
  case ARA_EVENT_TYPE: dataSize=0; break;
  case ARA_HEADER_TYPE: dataSize=0; break;
  case ARA_EVENT_HK_TYPE: dataSize=sizeof(AraEventHk_t); break;
  case ARA_SENSOR_HK_TYPE: dataSize=sizeof(AraSensorHk_t); break;
  case ARA_SBC_HK_TYPE: dataSize=0; break;
  default: dataSize=0; retVal+=PKT_E_CODE; break;
  }

  if(dataSize!=gHdr->numBytes) {
    retVal+=PKT_E_SIZE;
  }
  return retVal;

}

unsigned int simpleIntCrc(unsigned int *p, unsigned int n)
{
  unsigned int sum = 0;
  unsigned int i;
  for (i=0L; i<n; i++) {
    //
    sum += *p++;
  }
  //    printf("%u %u\n",*p,sum);
  return ((0xffffffff - sum) + 1);

}

const char *araDataTypeAsString(AraDataStructureType_t type)
{

  switch(type) {
  case ARA_EVENT_TYPE: return "AraStationEvent_t";
  case ARA_HEADER_TYPE: return "AraStationHeader_t";
  case ARA_EVENT_HK_TYPE: return "AraEventHk_t";
  case ARA_SENSOR_HK_TYPE: return "AraSensorHk_t";
  case ARA_SBC_HK_TYPE: return "AraSbcHk_t";
  default: return "Unknown";
  }
  
}


unsigned int grayToBinary(unsigned int gray)
{
       gray ^= (gray >> 16);
       gray ^= (gray >> 8);
       gray ^= (gray >> 4);
       gray ^= (gray >> 2);
       gray ^= (gray >> 1);
       return(gray);
}

int copyFile(const char *theFile, const char *theDir)
{
  static int errorCounter=0;
  //Only works on relative small files (will write a better version)
  char *justFile=basename((char *)theFile);
  char newFile[FILENAME_MAX];
  unsigned int numBytes=0;
  char *buffer=readFile(theFile,&numBytes);

  //Open the output file
  sprintf(newFile,"%s/%s",theDir,justFile);
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

int copyFileToFile(const char *theFile, const char *newFile) 
{ 
  static int errorCounter=0; 
  //Only works on relative small files (will write a better version) 
  unsigned int numBytes=0; 
  char *buffer=readFile(theFile,&numBytes); 

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

char *readFile(const char *theFile, unsigned int *numBytes)
{
  static int errorCounter=0;
  char *buffer;
  //Open the input file
  FILE *fin = fopen(theFile,"r");
  if(!fin) {
    if(errorCounter<100) {
      syslog(LOG_ERR,"Error reading file %s -- Error %s\n",
	     theFile,strerror(errno));
      fprintf(stderr,"Error reading file %s-- Error %s\n",
	      theFile,strerror(errno));
      errorCounter++;
    }
    return NULL;
  }
		   
  fseek(fin,0,SEEK_END);
  (*numBytes)=ftell(fin);
  fseek(fin,SEEK_SET,0);
  buffer=malloc((*numBytes));
  fread(buffer,1,(*numBytes),fin);
  fclose(fin);
  return buffer;
}
