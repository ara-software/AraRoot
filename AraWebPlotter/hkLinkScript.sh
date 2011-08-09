#!/bin/bash
if [ "$1" = "" ]
then
   echo "usage: `basename $0` <hk data dir>" 1>&2
   exit 1
fi

HK_DATA_DIR=$1
WEBPLOTTER_HKLINK_DIR=`getDirFromConfig --hkLinkDir`

for file in ${HK_DATA_DIR}/hk*.dat; 
  do echo $file; 
  ln -sf $file ${WEBPLOTTER_HKLINK_DIR};  
done
