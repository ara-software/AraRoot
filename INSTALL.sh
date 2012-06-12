#!/bin/bash

#INSTAL.sh written by Jonathan Davies 25/5/12 jdavies@hep.ucl.ac.uk
#This script will be the only thing to run when installing AraRoot
#Just run this script in the AraRoot source code directory

#First thing we need to do is check that the correct environmental variables exist
ERROR_FLAG=0

#Set some colours for the error messages
CYAN="\033[36m"
END="\033[0m"

if [ -z "$ARA_UTIL_INSTALL_DIR" ]; then
    echo -e $CYAN "\tError: Need to set ARA_UTIL_INSTALL_DIR environmental variable"
    echo -e "\t\tThis should point to where you want the installation files to be placed" $END


    ERROR_FLAG=1
#    exit 1
fi  

if [ -z "$ARA_ROOT_DIR" ]; then
    echo -e $CYAN "\tError: Need to set ARA_ROOT_DIR environmental variable"
    echo -e "\t\tThis should point to where the source code for AraRoot is located" $END
    ERROR_FLAG=1
#    exit 1
fi  

#Might want to check to see if this is a MAC system and let the user know that DYLD_LIBRARY_PATH is set
unamestr=`uname`

if [ $unamestr == 'Darwin' ]; then
    if [ -z $DYLD_LIBRARY_PATH ]; then

        echo -e $CYAN "\tError: DYLD_LIBRARY_PATH must be set in Mac OS X (Darwin) " $END

	ERROR_FLAG=1
    fi
elif [ -z $LD_LIBRARY_PATH ]; then
        echo -e $CYAN "\tError: LD_LIBRARY_PATH must be set" $END

	ERROR_FLAG=1
fi    

#Handling the exit status from before
if [ $ERROR_FLAG == 1 ]; then
#    echo "exiting"
    exit 1
fi

#First thing to do is to go into the build directory and get rid of everything there
cd $ARA_ROOT_DIR/build
rm -r ./*

#Now we run cmake to produce the makefiles. The install prefix is where things will be installed to.

cmake ../ -DCMAKE_INSTALL_PREFIX=${ARA_UTIL_INSTALL_DIR}

make
make install