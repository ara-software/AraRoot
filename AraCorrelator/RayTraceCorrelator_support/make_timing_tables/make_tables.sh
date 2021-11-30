#!/bin/bash
#SBATCH --job-name=makeTables
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=04:00:00
#SBATCH --export=ALL
#SBATCH --array=0-33
#SBATCH --output=logs/make_tables_%A_%a.out
#SBATCH --error=logs/make_tables_%A_%a.err

# this is just an example slurm script of how you might make many
# of the ray trace timing tables on a cluster

# declare -a radii=( 
#   100. 111.02320645 123.26152371 136.84889595 151.93403228
#   168.68203434 187.27620324 207.92004576 230.83950166 256.28541651
#   284.53628709 315.90130945 350.72376298 389.38476746 432.30745428
#   479.96159749 532.86875528 591.60797831 656.82214716 729.22500848
#   809.60898669 898.85385677 997.93637312 1107.94095982 1230.07157922
#  1365.66490893 1516.20497132 1683.33937559 1868.89735029 2074.90976364
#  2303.63135064 2557.56539037 2839.49110356 3152.49407017 3500.
# )

declare -a radii=(
  0  150  300  450  600  750  900 1050 1200 1350 1500 1650 1800 1950
 2100 2250 2400 2550 2700 2850 3000 3150 3300 3450 3600 3750 3900 4050
 4200 4350 4500 4650 4800 4950
)
cd /mnt/home/baclark/ara/araroot_pedloader/src/AraCorrelator/RayTraceCorrelator_support/make_timing_tables
source /mnt/home/baclark/ara/energy_reco_work/energy_reco/code/env_arasim.py
# echo ${radii[SLURM_ARRAY_TASK_ID]}
./makeRTArrivalTimeTables 2 ${radii[SLURM_ARRAY_TASK_ID]} . 
