#!/usr/bin/env bash

# This code uses: https://github.com/ara-software/AraRoot/tree/master/AraCorrelator/RayTraceCorrelator_support/make_timing_tables
# Copy this code to: .../AraRoot/AraCorrelator/RayTraceCorrelator_support/make_timing_tables
# Change OUTDIR

set -euo pipefail

EXEC="./makeRTArrivalTimeTables"
SYSTEMATIC=0
ANALYTIC=0
MAX_PROCS=10
MODEL=40

# Generalize the output directory
if [ $ANALYTIC -eq 1 ]
then
				analytic="_analytic"
else
				analytic=""
fi

OUTDIR="/fs/project/PAS0654/ARA_DATA/rt_tables/rt_tables2026/nofz_${MODEL}${analytic}"

run_job() {
    local station="$1"
    local radius="$2"

    echo "[$(date '+%F %T')] Starting station=${station} radius=${radius} systematic=${SYSTEMATIC} analytic=${ANALYTIC} iceModel=${MODEL}"
    "$EXEC" "$station" "$radius" "$OUTDIR" "$SYSTEMATIC" "${ANALYTIC}"
    echo "[$(date '+%F %T')] Finished station=${station} radius=${radius} systematic=${SYSTEMATIC} analytic=${ANALYTIC} iceModel=${MODEL}"
}

launch_range() {
    local station="$1"
    local rmin="$2"
    local rmax="$3"
    local step="$4"

    for radius in $(seq "$rmin" "$step" "$rmax"); do
        run_job "$station" "$radius" &

        while [[ $(jobs -rp | wc -l) -ge $MAX_PROCS ]]; do
            sleep 1
        done
    done
}

if [[ ! -x "$EXEC" ]]; then
    echo "Error: executable '$EXEC' not found or not executable."
    exit 1
fi

# Reproduce the files from your list
launch_range 100  800 1200 10
launch_range   2 2390 2930 10
launch_range   3 3150 3590 10
launch_range   4 3170 3600 10
launch_range   5 4130 4730 10

wait
echo "All jobs completed."
