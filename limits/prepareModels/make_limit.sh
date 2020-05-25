##Inputs
SELSIG=$1
YEAR=$2

#Make limit for the point
combine \
 -M AsymptoticLimits -D data_obs --run blind --setParameters r=1,myscale=0 \
 --freezeParameters allConstrainedNuisances \
 -n _${SELSIG}_limit \
datacard${YEAR}_${SELSIG}.root