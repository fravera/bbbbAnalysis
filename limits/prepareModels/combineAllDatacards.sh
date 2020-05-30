cd LimitsAll
for n in $(cat ../prepareModels/listOfSamples_$1.txt )
do
    rm -rf $n
    mkdir -p $n
    cd $n
    combineCards.py c2016=../../Limits2016/$n/datacard2016_selectionbJets_SignalRegion.txt c2017=../../Limits2017/$n/datacard2017_selectionbJets_SignalRegion.txt c2018=../../Limits2018/$n/datacard2018_selectionbJets_SignalRegion.txt > datacardAll_selectionbJets_SignalRegion.txt
    text2workspace.py datacardAll_selectionbJets_SignalRegion.txt
    cd ..
    sleep 120
done
cd ..