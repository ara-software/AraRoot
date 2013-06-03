#!/bin/bash
source  /home/jdavies/.bashrc
BIN=/home/jdavies/repositories/InstallDir/AraRoot/trunk/bin/getADCmVConversion2
BASE_DIR=/unix/ara/data/calibration/ARA02/
PED=440
RUN=462
BLOCK=0

$BIN $BASE_DIR $PED $RUN $BLOCK 
