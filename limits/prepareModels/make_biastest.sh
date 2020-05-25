#!/bin/bash

#Configuration

DATACARD=$1
injname=$2
NTOYS=$3


RLIMITMAX=`bc -l <<< "$injname * 200"`
RLIMITMIN=`bc -l <<< "$injname * -200"`
##Run tests
echo
echo " ============================================== "
echo "... running on injection $injname"
echo " ============================================== "
echo
if [ "$injname" == "0" ]; then
    SIGSTRENGTH="0"
else
    SIGSTRENGTH="$injname"
fi
#### Step 1 : generate toys with injected signal and bkgs (MODEL) fixed
echo "[Info] Step 1. generating toys with (data bkg.model) + (r*signal)"
combine -M GenerateOnly -t ${NTOYS} \
 --dataset data_obs --rMin $RLIMITMIN --rMax $RLIMITMAX --setParameters r=${SIGSTRENGTH},myscale=0 \
 --saveToys -n _inj_${SIGSTRENGTH}_toys \
 --freezeParameters r,myscale \
 ${DATACARD} 
#### Step 2 : fit the toys with the "real" b + "signal induced" b + r x s model
echo "[Info] Step 2. fitting toys with (data bkg.model) + (signal-induced bkg.model) + (r*signal)"
combine -M MultiDimFit -t ${NTOYS} \
--dataset data_obs --rMin $RLIMITMIN --rMax $RLIMITMAX \
--algo singles --toysFile higgsCombine_inj_${SIGSTRENGTH}_toys.GenerateOnly.mH120.123456.root \
-n _inj_${SIGSTRENGTH}_fitresults \
--setParameters myscale=${SIGSTRENGTH} \
${DATACARD} | tee "fitresult_${SIGSTRENGTH}.txt"

#### Step 3 :
echo "[Info] Step 3. fitting toys with (data bkg.model) + (r*signal) as a cross-check" 
combine -M MultiDimFit -t ${NTOYS} \
--dataset data_obs --rMin $RLIMITMIN --rMax $RLIMITMAX \
--algo singles --toysFile higgsCombine_inj_${SIGSTRENGTH}_toys.GenerateOnly.mH120.123456.root \
-n _inj_${SIGSTRENGTH}_myscale_0_fitresults \
--setParameters myscale=0 \
${DATACARD} | tee "fitresult_${SIGSTRENGTH}_myscale_0.txt"