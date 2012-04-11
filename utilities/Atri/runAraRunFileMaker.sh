#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <run num>" 1>&2
   exit 1
fi


#Only need to edit these two lines to point to the local directories 
RAW_BASE_DIR=~/ara/data/miniATRI
ROOT_BASE_DIR=~/ara/data/miniATRI/root
#RAW_BASE_DIR=/tmp/fakeData
#ROOT_BASE_DIR=/tmp/fakeRoot


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
    ./makeAraEventTree ${EVENT_FILE_LIST} ${EVENT_FILE}
    #cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi


echo "Starting Sensor Hk File"
SENSOR_HK_FILE_LIST=`mktemp sensor.XXXX`
for file in ${RAW_DIR}/sensorHk/sensorHk_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${SENSOR_HK_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${SENSOR_HK_FILE_LIST} | wc -l` -gt 0 ; then
    ./makeAraSensorHkTree ${SENSOR_HK_FILE_LIST} ${SENSOR_HK_FILE} ${RUN_NUM}
#    cat ${SENSOR_HK_FILE_LIST}
    rm ${SENSOR_HK_FILE_LIST}
    echo "Done Sensor Hk File"
else
    rm ${SENSOR_HK_FILE_LIST}
    echo "No sensor hk files"
fi



echo "Starting Event Hk File"
EVENT_HK_FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/eventHk/eventHk_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_HK_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_HK_FILE_LIST} | wc -l` -gt 0 ; then
    ./makeAraEventHkTree ${EVENT_HK_FILE_LIST} ${EVENT_HK_FILE} ${RUN_NUM}
#    cat ${EVENT_HK_FILE_LIST}
    rm ${EVENT_HK_FILE_LIST}
    echo "Done Event Hk File"
else
    rm ${EVENT_HK_FILE_LIST}
    echo "No event hk files"
fi



