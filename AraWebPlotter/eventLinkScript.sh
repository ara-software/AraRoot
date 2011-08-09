#!/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <event data dir>" 1>&2
   exit 1
fi

EVENT_DATA_DIR=$1
WEBPLOTTER_EVENTLINK_DIR=`getDirFromConfig --eventLinkDir`

for file in ${EVENT_DATA_DIR}/ev*.dat; 
  do echo $file; 
  ln -sf $file ${WEBPLOTTER_EVENTLINK_DIR};  
done
