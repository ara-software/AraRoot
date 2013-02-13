#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <run num>" 1>&2
   exit 1
fi


#Only need to edit these two lines to point to the local directories 
RAW_BASE_DIR=~/temp/AraRootFilterTest/raw_data
ROOT_BASE_DIR=~/temp/AraRootFilterTest/root

PED_FILE=/Users/jdavies/temp/AraRootFilterTest/raw_data/run_000948/pedestalValues.run000948.dat
OUT_PREF=~/temp/AraRootFilterTest/raw_data_filtered



RUN_NUM=$1
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`
echo $RUN_NUM $RUN_WITH_ZEROES

RAW_DIR=${RAW_BASE_DIR}/run_${RUN_WITH_ZEROES}
ROOT_DIR=${ROOT_BASE_DIR}/run${RUN_NUM}
EVENT_FILE=${ROOT_DIR}/event${RUN_NUM}.root
SENSOR_HK_FILE=${ROOT_DIR}/sensorHk${RUN_NUM}.root
EVENT_HK_FILE=${ROOT_DIR}/eventHk${RUN_NUM}.root
#exit 1
#echo ${RAW_DIR}

if [[ -d $ROOT_DIR ]]; then
    echo "Output dir exists"
else
    mkdir ${ROOT_DIR}
fi

echo "Starting Event File"
EVENT_FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/event/ev_*/*; 
do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
    ${ARA_UTIL_INSTALL_DIR}/bin/quickL1EventFilter ${EVENT_FILE_LIST} ${PED_FILE} ${OUT_PREF} ${RUN_NUM}
    #cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi



