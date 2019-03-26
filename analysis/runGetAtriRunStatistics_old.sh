#!/bin/bash

#Return 0 on success, 1 on failure

if [ "$1" = "" ] | [ "$2" = "" ] | [ "$3" = "" ]
then
   echo "usage: `basename $0` <stationId> <runNumber> <disk>" 1>&2
   echo "stationIds are:"
   for station in  ARA01 ARA02 ARA03 TestBed
   do
       echo $station
   done
   exit 1
fi

STATION=$1
RUN_NUM=$2
DISK=$3
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`
RUN_DIR=/mnt/data/disk${3}/${STATION}/2013/raw_data/run_${RUN_WITH_ZEROES}
OUT_FILE=~/jdavies/temp/runInfo_${STATION}_run${RUN_NUM}.root

EVENT_FILE_LIST=`mktemp runList.XXXX`
for file in ${RUN_DIR}/event/ev_*/*; 
do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
  fi
done

#To keep Hagar happy - make all the text output redirect to a file
TEXT_OUTPUT=`mktemp runStats.XXXX`

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
#getAtriRunStatistics <file list> <outFileName> <run>
    ${ARA_UTIL_INSTALL_DIR}/bin/getAtriRunStatistics ${EVENT_FILE_LIST} ${OUT_FILE} ${RUN_NUM} >> ${TEXT_OUTPUT}
    rm ${EVENT_FILE_LIST}
    rm ${TEXT_OUTPUT}
else
    rm ${EVENT_FILE_LIST}
    rm ${TEXT_OUTPUT}
    exit 1
fi





