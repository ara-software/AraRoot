#!/bin/bash

## We need to pass the following variables:
# 1. Station number
# 2. The output directory to store the table in
# 3. The calpulser distance for the station
# 4. The systematic error we're including on the ice model
#	(1 for up, -1 for down, 0 (or anything) for nothing)
station=$1
outputdir=$2
radius=$3
systematic=$4

source ~/.bashrc

## Get the environment variables necessary for running makeRTArrivalTables
#source /cvmfs/ara.opensciencegrid.org/trunk/alma9/setup.sh
### We need to point to our AraSim version with the systematics
new_ray_tables ## Need to load AraProc, Fivestation, etc. + AraSim on the correct branch


cd ~/ray_trace_tables

## Cobalt wants you to put outputs on TMPDIR I guess
## So send it there and copy over 
./makeRTArrivalTimeTables "${station}" "${radius}" "${TMPDIR}" "${systematic}" 
cp "${TMPDIR}"/* "${outputdir}"
