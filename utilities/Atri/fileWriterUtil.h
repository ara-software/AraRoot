/* 
   Utility functions, mostly file I/O and file handling

   M2 Consulting, July 2010
   ped@m2-consult.com
*/

#ifndef ARA_UTIL_H
#define ARA_UTIL_H

#include "araAtriStructures.h"
#include <stdio.h>
#include <syslog.h>
#include <stdint.h>
#include <zlib.h>
#include <sys/time.h>

typedef struct {
  gzFile         currentFilePtr;
  int            maxEvents;  // Number of events per file
  int            maxFiles;   // Number of files per subdir
  int            writeCount; // Events written to current file
  int            fileCount;  // Files written to current subdir
  int            dirCount;   // Subdirs written to current dir
  char           compressionLevel[10]; 
  char           filePrefix[FILENAME_MAX];
  char           currentFileName[FILENAME_MAX];
  char           currentDirName[FILENAME_MAX];
  char           currentSubDirName[FILENAME_MAX];
  const char*    linkDir;
  int            currentRunNumber;
  struct timeval startTime;
} ARAWriterStruct_t;

void initWriter(ARAWriterStruct_t* writer, 
                int runNumber,
                int compression, 
		int maxFiles, int maxEvents, const char* filePrefix,
		const char* topDir, const char* linkDir);

void closeWriter(ARAWriterStruct_t* writer);
int newWriterFile(ARAWriterStruct_t* writer);
int newWriterSubDir(ARAWriterStruct_t* writer);
int writeBuffer(ARAWriterStruct_t* writer, char* buffer, int len, int *new_file_flag );

int makeDirectories(const char *theTmpDir);
int is_dir(const char *path);
int makeLink(const char *theFile, const char *theLinkDir);
int moveFile(const char *theFile, const char *theDir);
int copyFile(const char *theFile, const char *theDir);
int copyFileToFile(const char *theFile, const char *newFile);
char *readFile(const char *theFile, unsigned int *numBytes);
int removeFile(const char *theFile);

int checkPidFile(const char *fileName);
int writePidFile(const char *fileName);
void sigUsr1Handler(int sig);
void sigUsr2Handler(int sig);

void setNextTime(struct timeval* nextTime, const struct timeval* lastTime, 
                 const struct timeval* timeInterval );
void setTimeInterval(struct timeval* timeInterval, double step);
void getFileStartTime( ARAWriterStruct_t* writer , unsigned int *second , unsigned int *usec );

uint32_t getRunNumber();

//Generic Header Stuff
void fillGenericHeader(void *thePtr, AraDataStructureType_t type, unsigned int numBytes);
int checkPacket(void *thePtr);
unsigned int simpleIntCrc(unsigned int *p, unsigned int n);
const char *araDataTypeAsString(AraDataStructureType_t type);

unsigned int grayToBinary(unsigned int gray);

#endif // ARA_UTIL_H
