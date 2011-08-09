#/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <time num>" 1>&2
   exit 1
fi

TIME_NUM=$1
RAW_BASE_DIR=/unix/anita1/ara/data/pole11
ROOT_BASE_DIR=/unix/anita1/ara/data/pole11/root

if [[ -d $ROOT_BASE_DIR ]]; then
    echo "Output dir exists"
else
    mkdir ${ROOT_BASE_DIR}
fi

echo "Starting Event File"
EVENT_FILE_LIST=`mktemp`
for file in ${RAW_BASE_DIR}/event/ev_${TIME_NUM}/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
  fi
done

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
    ./makeAraEventTree ${EVENT_FILE_LIST} ${ROOT_BASE_DIR}/event_${TIME_NUM}.root
#    cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi



