for n in $(cat prepareModels/listOfSamples.txt )
do
    python prepareModels/prepareHistos.py              --config prepareModels/config/LimitsConfig_$1.cfg --signal $n
    python prepareModels/makeDatacardsAndWorkspaces.py --config prepareModels/config/LimitsConfig_$1.cfg  --no-comb --signal  $n
    sleep 15
done