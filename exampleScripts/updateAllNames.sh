#!/bin/bash

function doFindAndReplace() {
    if [[ $3 = "" ]]; then
	echo "`basename $0` <old> <new> <file> "
	exit 1
    fi
    
    OLD=$1
    NEW=$2
    FILE=$3
    #cp $3 $3.old

    sed "s/${1}/${2}/g" $3 > $3.new
    mv $3.new $3
}


for file in *.h *.cxx Makefile; do
    echo $file
    doFindAndReplace AraRawRFChannel AraRawTestBedRFChannel $file
    doFindAndReplace AraHkData AraTestBedHkData $file
    doFindAndReplace AraTriggerMonitor AraTestBedTriggerMonitor $file
    doFindAndReplace FullAraHkEvent FullAraTestBedHkEvent $file
    doFindAndReplace RawAraEvent RawAraTestBedStationEvent $file
    doFindAndReplace RawAraHeader RawAraTestBedStationHeader $file
    doFindAndReplace UsefulAraEvent UsefulAraTestBedStationEvent $file
    doFindAndReplace RFChannelFull_t AraTestBedRFChannelFull_t $file
    doFindAndReplace RFChannelPedSubbed_t AraTestBedRFChannelPedSubbed_t $file
    doFindAndReplace TemperatureDataStruct_t AraTestBedTemperatureDataStruct_t $file
    doFindAndReplace RFPowerDataStruct_t AraTestBedRFPowerDataStruct_t $file
    doFindAndReplace DACDataStruct_t AraTestBedDACDataStruct_t $file
    doFindAndReplace SimpleScalerStruct_t AraTestBedSimpleScalerStruct_t $file
    doFindAndReplace AraEventHeader_t AraTestBedEventHeader_t $file
    doFindAndReplace AraEventBody_t AraTestBedEventBody_t $file
    doFindAndReplace AraHkBody_t AraTestBedHkBody_t $file
    doFindAndReplace PedSubbedEventBody_t AraTestBedPedSubbedEventBody_t $file
    doFindAndReplace CHANNELS_PER_CHIP CHANNELS_PER_LAB3 $file
    doFindAndReplace MAX_NUMBER_SAMPLES MAX_NUMBER_SAMPLES_LAB3 $file
    doFindAndReplace ACTIVE_CHIPS LAB3_PER_TESTBED $file
    doFindAndReplace EFFECTIVE_SAMPLES EFFECTIVE_LAB3_SAMPLES $file
    doFindAndReplace ANTS_PER_STATION ANTS_PER_TESTBED $file
    doFindAndReplace RFCHANS_PER_STATION RFCHANS_PER_TESTBED $file
    doFindAndReplace TRANS_PER_STATION TRANS_PER_TESTBED $file
    doFindAndReplace TOTAL_ANTS TOTAL_ANTS_PER_TESTBED $file
    doFindAndReplace NUM_DIGITIZED_CHANNELS NUM_DIGITIZED_TESTBED_CHANNELS $file
    doFindAndReplace araStructures.h araTestbedStructures.h $file
    doFindAndReplace araDefines.h araTestbedDefines.h $file
done
