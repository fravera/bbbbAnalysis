Set of tools to compare the distributions of some analysis variables for the LO, LO+jj (privately produced) and NLO samples.

Steps to reproduce the study:

1. run the usual skims. For this study on 10/04/2020 this commands were used:

```
# this prepares the tar and ships it to EOS - do not remove this line!
python scripts/submitSkimOnTier3.py --tag=$1 --xrdcp-tar-only --do-tar --do-xrdcp-tar

#############################################################################################

#### HH + JJ
python scripts/submitSkimOnTier3.py --input=inputFiles/2018NonResonantDiHiggs4BDataSets/gg_HH_bbbb_012j_nanoAOD_Petrucciani_2018.txt      --puWeight=weights/2018/GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8_PUweights.root       --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.01043  --njobs=200

### SM reweighted LO
python scripts/submitSkimOnTier3.py --kl-rew 1 --kl-map weights/gg_HH_4B_EFTnodes_2018.root --outputName GGHH4B_rew_kl_1_SM \
        --input=inputFiles/2018NonResonantDiHiggs4BDataSets/GluGluToHHTo4B_EFTnodes_TuneCP5_PSWeights_13TeV-madgraph-pythia8.txt     --puWeight=weights/2018/GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8_PUweights.root     --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal  --xs=0.01043    --njobs=200

### NLO
python scripts/submitSkimOnTier3.py --input=NLOlists/HH_4B_2018_cHHH1.txt      --puWeight=weights/2018/GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8_PUweights.root       --tag=$1 --cfg=config/skim_2018NonResonantDiHiggs4B.cfg --is-signal --xs=0.01043  --njobs=200 --is-nlo-sig
```

2. run the plots with fill_histograms on the selected variable. For this study on 10/04/2020 the configs and filelists in `compare_HH_LO_LOjj_NLO/configs` were used

3. run ``make_plot_ggHHjets.py`` to compare the 3 samples on a given variable and selection. You can turn on/off PS and LHE scale uncertainty bands, and set log y on/off.
To run all plots at once use ``plot_all_ggHHjets.sh``.
Note : you may need to create the output folder ``ggHHjjplots`` to store the output plots inside.