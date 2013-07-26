#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <run num>" 1>&2
   exit 1
fi


#Only need to edit these two lines to point to the local directories 
RAW_BASE_DIR=~/ara/data/fromWisconsin2013/ARA02/raw_data/
OUT_DIR=~/ara/data/fromWisconsin2013/ARA02/raw_data/filtered/


RUN_NUM=$1
RUN_WITH_ZEROES=`printf %06d $RUN_NUM`
echo $RUN_NUM $RUN_WITH_ZEROES

RAW_DIR=${RAW_BASE_DIR}/run_${RUN_WITH_ZEROES}
#exit 1
#echo ${RAW_DIR}

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
    ${ARA_UTIL_INSTALL_DIR}/bin/quickOneInTenFilter ${EVENT_FILE_LIST} ${OUT_DIR} ${RUN_NUM}
    #cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi



