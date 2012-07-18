#/bin/bash
if [ "$2" = "" ]
then
   echo "usage: `basename $0` <input dir> <output file" 1>&2
   exit 1
fi

RAW_BASE_DIR=$1
#ROOT_BASE_DIR=/Users/rjn/ara/data/root

#if [[ -d $ROOT_BASE_DIR ]]; then
#    echo "Output dir exists"
#else
#    mkdir ${ROOT_BASE_DIR}
#fi

echo "Starting Event File"
EVENT_FILE_LIST=`mktemp`
for file in ${RAW_BASE_DIR}/ev_*/*; 
  do
  if [[ -f $file ]]; then
      echo $file >> ${EVENT_FILE_LIST}
#      echo `dirname $file`;
  fi
done

if  test `cat ${EVENT_FILE_LIST} | wc -l` -gt 0 ; then
    ${ARA_UTIL_INSTALL_DIR}/bin/makeAraEventTree ${EVENT_FILE_LIST} $2 1 # 1 - stationId for Station1
#    cat ${EVENT_FILE_LIST}
    rm ${EVENT_FILE_LIST}
    echo "Done Event File"
else
    rm ${EVENT_FILE_LIST}
    echo "No event files"
fi



