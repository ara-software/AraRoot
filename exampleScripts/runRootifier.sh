#!/bin/bash
export ARA_UTIL_INSTALL_DIR=/home/rjn/install/
export ARA_WEBPLOTTER_CONFIG_DIR=/home/rjn/ara/AraRoot/AraWebPlotter/config
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ARA_UTIL_INSTALL_DIR}/lib
export PATH=${PATH}:${ARA_UTIL_INSTALL_DIR}/bin

echo "ARA_UTIL_INSTALL_DIR = $ARA_UTIL_INSTALL_DIR"
echo "ARA_WEBPLOTTER_CONFIG_DIR = $ARA_WEBPLOTTER_CONFIG_DIR"

for rawdir in /unix/anita1/ara/data/fromWisconsin/raw/*; do
    runname=`basename $rawdir`
#    echo $runname
    runno=${runname:6:4} #only valid up to 9999
#    echo $runno
    rootdir=/unix/anita1/ara/data/fromWisconsin/root/run${runno}
    echo $rootdir
    if [ -d $rootdir ] 
	then
	echo "Done $runno"
    else
#	echo "Not done $runno"
	cd /home/rjn/ara/AraRoot/utilities
	./runAraRunFileMaker.sh $runno
	
    fi
done