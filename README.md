# ARA ROOT-based  Software
######  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)
######  & Jonathan Davies (jdavies@hep.ucl.ac.uk)

## Website
---
http://www.hep.ucl.ac.uk/uhen/ara/araroot

##Introduction
---
ARA ROOT based software, AraRoot, comprises several libraries and executables.

### Libraries
* libAraEvent.so  -- Contains all of the class definitions for storing and manipulating Ara Event and Housekeeping data
* libAraDisplay.so -- The library for AraDisplay (the MagicDisplay spin off)
* libAraWebPlotter.so -- The library for the AraWebPlotter
* libAraCorrelator.so -- Used to correlate events using plane wave or distance to vertex approximations		    

### Executables
* makeIcrrEventTree -- Program to convert the raw ARA TestBed / Station One data into a ROOT format
*  makeAtriEventTree -- Program to convert the raw ARA Atri electronics type station data into a ROOT format
*  AraWebRootFileMaker -- Program to make ROOT files for the webplotter
*  AraWebPlotter -- Program that reads these files and plots stuff
*  exampleLoop -- An example of analysis code that illustrates to the user how to load AraRoot files, loop through them, create "Useful" objects and perform some sort of analysis
*  exampleLoopL2 -- An example of analysis code that illustrates to the user how to process L0 or L1 (RawEvent or UsefulEvent) ROOT files and produce L2 files

### Scripts
*  runAraTestBedEventFileMaker.sh -- Script for calling makeAraEventTree to create TestBed root files
*  runAraOneEventFileMaker.sh -- Script for calling makeAraEventTree to create Station One root files
*  runAtriEventFileMaker.sh -- Script for calling makeAraEventTree to create ATRI station root files
* runAtriEventFileMakerForcedStationId.sh -- Script for calling makeAraEventTree to create ATRI station root files, but forcing a particular stationId into the event objects

### ROOT Macros
* runAraDisplay.C -- Script for starting AraDisplay


## Prerequisites
---
* ROOT -- As the name ARA ROOT suggests this is needed
* FFTW3 -- Fastest Fourier Transform in the West - used for FFT fun
* libRootFftwWrapper -- a ROOT wrapper for FFTW 3 downloadable from http://www.hep.ucl.ac.uk/uhen/libRootFftwWrapper/
* GSL -- Needed by ROOT's Mathmore library
* sqlite3 -- Need for loading of antenna information and station geometry
* CMake -- Cross platform Makefile generation - the current version of AraRoot has transitioned to using CMake instead of bespoke Makefiles - hopefully this will make things a bit easier for everybody to install things!

## Installation
---
1. Checkout the code from the ARA Git repository, eg.: `git clone https://github.com/ara-software/AraRoot.git`

2. Define the `ARA_UTIL_INSTALL_DIR` to point to the location you want the library installed (the library files will end up in `(ARA_UTIL_INSTALL_DIR)/lib` and the header files in `(ARA_UTIL_INSTALL_DIR)/include` and the executables in `(ARA_UTIL_INSTALL_DIR)/bin)`. If this is not defined the installation directory will default to /usr/local (probably not ideal!).

3. Define `ARA_ROOT_DIR` to point to the location where you cloned this repository into.

4. Do `bash INSTALL.sh <MODE>` in the directory of the source code (i.e. in `ARA_ROOT_DIR`) - cmake will take care of the rest. <MODE> should be one of the following:
  - 0 - re-build bins / libs that have been modified
  - 1 - re-build all
  - 99 - re-build debugging mode - this will produce more verbose output from the build process

## Adding Your Own Analysis Code to the CMake build structure
---
Create your `.cxx` file in analysis and edit the `analysis/CMakeLists.txt` file. You will need to copy the lines that involved "exampleLoop", replacing it with the name of your executable

You should then be able to run `bash INSTALL.sh 0` which will notice that there is something that it hasn't built yet and do the (hopefully) right thing and build it for you.

The executable will be created in two loactions: `${ARA_ROOT_DIR}/build/analysis` and `${ARA_UTIL_INSTALL_DIR}/bin`.


## Troubleshooting
---
There are a myriad of ways that one can run into problems with the software:
1. Path problems -- the bane of poorly organised code. By far the easiest way to use the ARA root code is to just set the `ARA_UTIL_INSTALL_DIR` and have it point to the location you want to install all the packages. If things are set up correctly you will end up with a `ARA_UTIL_INSTALL_DIR/include` and `ARA_UTIL_INSTALL_DIR/lib`, `ARA_UTIL_INSTALL_DIR/bin`, `ARA_UTIL_INSTALL_DIR/scripts` and `ARA_UTIL_INSTALL_DIR/share/araCalib` all populated with essential headers, libraries and calibration constants. 

Most problems are fixed by running `bash INSTALL.sh 1` to re-build AraRoot from scratch. 
If this doesn't fix things try removing everything from the `ARA_UTIL_INSTALL_DIR`, re-installating `libRootFftwWrapper` and then re-running `bash INSTALL.sh` with either 1 or 99.

If problems persist, seek help!