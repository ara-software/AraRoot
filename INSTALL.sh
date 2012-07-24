#!/bin/bash

#INSTALL.sh written by Jonathan Davies 25/5/12 jdavies@hep.ucl.ac.uk
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

make #VERBOSE=1
if [ $? -eq 0 ] ; then
    echo "success"
    make install
#Now let us create a lib / include / bin/ scripts / macros structure in the build directory
    mkdir $ARA_ROOT_DIR/build/include
    cp $ARA_ROOT_DIR/AraCorrelator/*.h $ARA_ROOT_DIR/build/include
    cp $ARA_ROOT_DIR/AraDisplay/*.h $ARA_ROOT_DIR/build/include
    cp $ARA_ROOT_DIR/AraWebPlotter/*.h $ARA_ROOT_DIR/build/include
    
    mkdir $ARA_ROOT_DIR/build/lib
    cp $ARA_ROOT_DIR/build/AraCorrelator/libAraCorrelator.so $ARA_ROOT_DIR/build/lib
    cp $ARA_ROOT_DIR/build/AraDisplay/libAraDisplay.so $ARA_ROOT_DIR/build/lib
    cp $ARA_ROOT_DIR/build/AraEvent/libAraEvent.so $ARA_ROOT_DIR/build/lib
    cp $ARA_ROOT_DIR/build/AraWebPlotter/libAraWebPlotter.so $ARA_ROOT_DIR/build/lib
    cp $ARA_ROOT_DIR/build/AraWebPlotter/configLib/libAraConfig.so $ARA_ROOT_DIR/build/lib
    cp $ARA_ROOT_DIR/build/AraWebPlotter/kvpLib/libAraKvp.so $ARA_ROOT_DIR/build/lib
    
    mkdir $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/AraCorrelator/makeCorrelationMaps $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/AraWebPlotter/AraTimeWebPlotterBin $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/AraWebPlotter/AraWebPlotterBin $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/AraWebPlotter/getDirFromConfigBin $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Atri/makeAtriEventHkTree $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Atri/makeAtriEventTree $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Atri/makeAtriSensorHkTree $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Atri/makeSimpleAtriEventTree $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Icrr/makeIcrrCalibratedEventTree $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Icrr/makeIcrrEventTree $ARA_ROOT_DIR/build/bin
    cp $ARA_ROOT_DIR/build/utilities/Icrr/makeIcrrHkTree $ARA_ROOT_DIR/build/bin
    
    mkdir $ARA_ROOT_DIR/build/scripts
    cp $ARA_ROOT_DIR/utilities/Icrr/*.sh $ARA_ROOT_DIR/build/scripts
    cp $ARA_ROOT_DIR/utilities/Atri/*.sh $ARA_ROOT_DIR/build/scripts
    
    mkdir $ARA_ROOT_DIR/build/macros
    cp $ARA_ROOT_DIR/AraDisplay/macros/runAraDisplay* $ARA_ROOT_DIR/build/macros
    
else
    echo "fail"
fi



