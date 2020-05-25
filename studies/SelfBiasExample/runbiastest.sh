#Self-bias test configuration
toys=1000
datasets=(2016)
ggf_selsignals=(GGFcateg1_ggHH_kl_1_kt_1)
ggf_u_injs=(5.0 10.0 15.0 20.0 25.0 30.0 35.0 40.0)

for dataset in ${datasets[@]} #Loop over datasets
do
    cd selfbiasmodel${dataset}
    #Loop over the regions and signals
    for selsignal in ${ggf_selsignals[@]}  
    do
    # Make the stat only limit
    source ../make_limit.sh ${selsignal} ${dataset}
    mkdir testoutput
    mkdir testoutput/${selsignal}
    mv higgsCombine_${selsignal}* testoutput/${selsignal}
         #Loop over u_inj
         for  u_inj in ${ggf_u_injs[@]} 
         do
             #Run injection test
             source ../make_biastest.sh ${selsignal} ${u_inj} $toys $dataset
             mv higgsCombine_${selsignal}* testoutput/${selsignal}
             mv fitresult_*.txt testoutput/${selsignal}
         done
    mkdir testplots
    python ../plot_sig_vs_inj.py --study selfbiasmodel${dataset} --selsignal ${selsignal} --dataset $dataset
    done
    #get back to the main folder
    cd ..
done
