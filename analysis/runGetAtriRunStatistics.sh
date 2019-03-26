#!/bin/bash

#Return 0 on success, 1 on failure
if [ "$1" = "" ] | [ "$2" = "" ]
then
   echo "usage: `basename $0` <stationId> <runNumber> " 1>&2
   echo "stationIds are:"
   for station in  1 2 3
   do
       echo ARA0$station
   done
   exit 1
fi

STATION=ARA0$1
RUN_NUM=$2
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`

TARGZ_DIR=/home/uzair/Documents/AraRootStuff/AraData/dummytardata
SPS_ARA_FILE=SPS-ARA-FILTERED-run-${RUN_WITH_ZEROES}.${STATION}
CURRENT_DIR=/home/uzair/Documents/AraRootStuff/aradata/test

TARGZ_FILE=${SPS_ARA_FILE}.tar.gz
DATTAR_FILE=${SPS_ARA_FILE}.dat.tar

echo "STEP1: Copying .tar.gz files..."
cp ${TARGZ_DIR}/${TARGZ_FILE} .
echo "STEP2: tar-xf-ing .tar.gz files.."
tar -xf ${TARGZ_FILE}
echo "STEP3: Removing .tar.gz files.."
rm ${TARGZ_FILE}
echo "STEP4: tar-xf-ing .dat.tar files.."
tar -xf ${DATTAR_FILE}
echo "STEP5: Removing extra .tar and .xml files.."
rm *.xml
rm *.tar

RUN_DIR=${CURRENT_DIR}/run_${RUN_WITH_ZEROES}
OUT_FILE=${CURRENT_DIR}/runInfo_${STATION}_run${RUN_NUM}.root

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
    echo "STEP6: Run ATRI statistics executable and make .txt and .root files"
    ${ARA_UTIL_INSTALL_DIR}/bin/getAtriRunStatistics ${EVENT_FILE_LIST} ${OUT_FILE} ${RUN_NUM} >> ${TEXT_OUTPUT}
    echo "STEP7: Remove extra text output and run_${RUN_WITH_ZEROES} directory" 
    rm ${EVENT_FILE_LIST}
    rm ${TEXT_OUTPUT}
    rm -rf run_${RUN_WITH_ZEROES}
else
    rm ${EVENT_FILE_LIST}
    rm ${TEXT_OUTPUT}
    rm -rf run_${RUN_WITH_ZEROES}
    exit 1
fi

echo "STEP8: Append Station ID in output script"
echo "STATION_ID= $1" >> out_run${RUN_NUM}.txt





