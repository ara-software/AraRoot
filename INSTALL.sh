#!/bin/bash

#INSTALL.sh written by Jonathan Davies 25/5/12 jdavies@hep.ucl.ac.uk
#This script will be the only thing to run when installing AraRoot
#Just run this script in the AraRoot source code directory


ERROR_FLAG=0

#Set some colours for the error messages
CYAN="\033[36m"
END="\033[0m"


#This is where we handle arguments passed to the script
#0  Re-build programs and libraries that have been modified since last build
#1  Complete re-build -- remove everything from the build directory and start over
#99 De-bugging - build verbosely

MODE=$1
if [ -z "$1" ]; then
    echo "Usage " $0 " 0 <re-build modified bin / libs> 1 <re-build all>"
    ERROR_FLAG=1
fi

#Check that the correct environmental variables exist
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

#If we're running a test
if [ $MODE == "test" ]; then
	echo "Running test suite"
	cd $ARA_ROOT_DIR/build
	ctest --verbose
	exit
fi

#First thing to do is to go into the build directory and get rid of everything there
mkdir -p $ARA_ROOT_DIR/build #make this directory if it doesn't exist already (it won't be in the git checkout by default)
cd $ARA_ROOT_DIR/build

#This forces a complete re-build
if [ $MODE == 1 ] || [ $MODE == 99 ] ; then
#    echo "Passed the test Mode == 1 or 99"
    rm -r $ARA_ROOT_DIR/build/*
fi

#Now we run cmake to produce the makefiles. The install prefix is where things will be installed to.
cmake ../ -DCMAKE_INSTALL_PREFIX=${ARA_UTIL_INSTALL_DIR}

if [ $MODE == 99 ] ; then
    make VERBOSE=1
else
    make
fi

make #VERBOSE=1
if [ $? -eq 0 ] ; then
    echo "build - success"
    make install
    
else
    echo "build - fail"
fi



