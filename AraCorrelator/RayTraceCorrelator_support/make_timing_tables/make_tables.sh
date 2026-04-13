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
source /cvmfs/ara.opensciencegrid.org/trunk/alma9/setup.sh
## We need to point to our AraSim version with the systematics
export ARA_SIM_DIR="/home/amachtay/5SA_Help/analysis/template_analysis/AraSim/" ## This path is on the systematics branch
export LD_LIBRARY_PATH="$ARA_SIM_DIR:$ARA_UTIL_INSTALL_DIR/lib:$ARA_DEPS_INSTALL_DIR/lib:$NUPHASE_INSTALL_DIR/lib:$MISC_UTIL_DIR/lib:$ROOT_BUILD_DIR/lib"
export PATH="$ARA_SIM_DIR:$ARA_UTIL_INSTALL_DIR/bin:$ARA_DEPS_INSTALL_DIR/bin:$NUPHASE_INSTALL_DIR/bin:$MISC_UTIL_DIR/bin:$ROOT_BUILD_DIR/bin:$PATH"

cd /data/user/amachtay/ray_trace_tables/AraRoot/AraCorrelator/RayTraceCorrelator_support/make_timing_tables

./makeRTArrivalTimeTables "${station}" "${radius}" "$outputdir" "${systematic}" 

./makeRTArrivalTimeTables "${station}" 300 "$outputdir" "${systematic}" 

