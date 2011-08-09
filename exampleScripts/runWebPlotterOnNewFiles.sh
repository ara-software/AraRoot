#!/bin/bash
export ARA_UTIL_INSTALL_DIR=/home/rjn/install/
export ARA_WEBPLOTTER_CONFIG_DIR=/home/rjn/ara/AraRoot/AraWebPlotter/config
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${ARA_UTIL_INSTALL_DIR}/lib
export PATH=${PATH}:${ARA_UTIL_INSTALL_DIR}/bin

echo "ARA_UTIL_INSTALL_DIR = $ARA_UTIL_INSTALL_DIR"
echo "ARA_WEBPLOTTER_CONFIG_DIR = $ARA_WEBPLOTTER_CONFIG_DIR"

for rootdir in /unix/anita1/ara/data/fromWisconsin/root/run*; do
    runname=`basename $rootdir`
#    echo $runname
    runno=${runname:3:4} #only valid up to 9999
#    echo $runno
    rootfile=${rootdir}/event${runno}.root
    if [ -f $rootfile ]
	then

	webdir=/unix/www/html/uhen/ara/monitor/headers/archive/run${runno}
#	echo $webdir
	if [ -d $webdir ] 
	    then
	    echo "Done $runno"
	else
	echo "Not done $runno"
	    cd /home/rjn/ara/AraRoot/AraWebPlotter
	    time ./AraWebPlotter $rootfile	    
	fi
    fi
done
cd /home/rjn/ara/AraRoot/AraWebPlotter
time ./AraTimeWebPlotter
