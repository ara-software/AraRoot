#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <run num>" 1>&2
   exit 1
fi


#Only need to edit these two lines to point to the local directories 
RAW_BASE_DIR=~/ara/data/ara_station1_ICRR_calibration/event
ROOT_BASE_DIR=~/ara/data/ara_station1_ICRR_calibration/root/AraRoot/trunk/


RUN_NUM=$1
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`
echo $RUN_NUM $RUN_WITH_ZEROES

RAW_DIR=${RAW_BASE_DIR}/run_${RUN_WITH_ZEROES}
ROOT_DIR=${ROOT_BASE_DIR}/run${RUN_NUM}
EVENT_FILE=${ROOT_DIR}/event${RUN_NUM}.root
HK_FILE=${ROOT_DIR}/hk${RUN_NUM}.root
#exit 1
#echo ${RAW_DIR}

if [[ -d $ROOT_DIR ]]; then
    echo "Output dir exists"
else
    mkdir ${ROOT_DIR}
fi

echo "Starting Event File"
EVENT_FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/ev_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
    ./makeAraEventTree ${EVENT_FILE_LIST} ${EVENT_FILE} 1 # 1 - stationId for Station1
#    cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi


echo "Starting Hk File"
HK_FILE_LIST=`mktemp hk.XXXX`
for file in ${RAW_DIR}/hk_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${HK_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${HK_FILE_LIST} | wc -l` -gt 0 ; then
    ./makeAraHkTree ${HK_FILE_LIST} ${HK_FILE}
#    cat ${HK_FILE_LIST}
    rm ${HK_FILE_LIST}
    echo "Done Hk File"
else
    rm ${HK_FILE_LIST}
    echo "No hk files"
fi



