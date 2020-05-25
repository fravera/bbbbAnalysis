#Self-bias test configuration
toys=1000
datasets=(2016)
ggf_selsignals=(selectionbJets_SignalRegion)
ggf_u_injs=(0.5 1.0 1.5 2.0 2.5 3.0 3.5 4.0 4.5 5.0)

for dataset in ${datasets[@]} #Loop over datasets
do
    cd SelfBiasTest${dataset}
    #Loop over the regions and signals
    for selsignal in ${ggf_selsignals[@]}  
    do
    # Make the stat only limit
    source ../prepareModels/make_limit.sh ${selsignal} ${dataset}
    rm -rf testoutput/${selsignal}
    mkdir -p testoutput/${selsignal}
    mv higgsCombine_${selsignal}* testoutput/${selsignal}
         #Loop over u_inj
         for  u_inj in ${ggf_u_injs[@]} 
         do
             #Run injection test
             source ../prepareModels/make_biastest.sh ${selsignal} ${u_inj} $toys $dataset
             mv higgsCombine_${selsignal}* testoutput/${selsignal}
             mv fitresult_*.txt testoutput/${selsignal}
         done
    rm -rf testplots
    mkdir -p testplots
    python ../prepareModels/plot_sig_vs_inj.py --study SelfBiasTest${dataset} --selsignal ${selsignal} --dataset $dataset
    done
    #get back to the main folder
    cd ..
done
