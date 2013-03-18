#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <run num> <out file>" 1>&2
   exit 1
fi

RAW_BASE_DIR=$ARA03_2013_DATA

RUN_NUM=$1
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`
OUT_FILE=$2
RAW_DIR=${RAW_BASE_DIR}/run_${RUN_WITH_ZEROES}


echo "Starting Event File"
EVENT_FILE_LIST=`mktemp event.XXXX`
for file in ${RAW_DIR}/event/ev_*/*; 
do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
      #echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
    ${ARA_UTIL_INSTALL_DIR}/bin/getAtriRunStatistics ${EVENT_FILE_LIST} ${OUT_FILE} ${RUN_NUM}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi

