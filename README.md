# bbbbAnalysis

## Install instructions
```
cmsrel CMSSW_10_2_5
cd CMSSW_10_2_5/src
cmsenv
git cms-addpkg PhysicsTools/KinFitter CommonTools/Utils CondFormats/JetMETObjects CondFormats/Serialization FWCore/MessageLogger FWCore/Utilities JetMETCorrections/Modules
scram b -j 8
git clone https://github.com/fravera/bbbbAnalysis.git
git checkout mlBranch
```

## Setup and compile
```
# from bbbbAnalysis/
cmsenv
source scripts/setup.sh # only needed once for every new shell
make exe -j # compiles and makes everything under test/ executable
````

## Make a skim of NanoAOD
For data:
```
skim_ntuple.exe --input inputFiles/2016_NMSSM_XYH_bbbb_Datasets/BTagCSV_Data.txt --cfg config/Resonant_NMSSM_bbbb/skim_2016Resonant_NMSSM_XYH_bbbb.cfg --is-data --output test_NMSSM_XYH_bbbb_Data.root --maxEvts 1000000
````
For fast sim signal
```
skim_ntuple.exe --cfg config/Resonant_NMSSM_bbbb/skim_2016Resonant_NMSSM_XYH_bbbb_Fast.cfg --input inputFiles/2016_NMSSM_XYH_bbbb_Datasets/NMSSM_XYH_bbbb_MX_700_NANOAOD_v7.txt --output test_NMSSM_XYH_bbbb_MC.root --is-signal --xs=1 --puWeight weights/2016_NMSSM_XYH_bbbb_weights/NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_PUweights.root --maxEvt 100000
```

## Fill histograms from skims
```
fill_histograms.exe config/Resonant_NMSSM_bbbb/MXless1000_MYgreater140/plotter_2016Resonant_NMSSM_XYH_bbbb.cfg 
````

## Make plots
"NOT WORKING FOR RESONANT ANALYSIS"
Use the ``plotter/plotter.py`` script. Styles (line colors, etc.) for the processes are defined in ``plotter/styles/plotStyles.py``
Inside the script, the subset of processes to run on is defined through ``bkgToPlot`` and  ``sigToPlot``.
Several cmd line options available to configure the plot, it's practical to make a script that produces all the plots.
Example:
```
source do_all_plots.sh
````

## Machine learning skims using pandas dataframes
Probably you may need to instull some updates of libraries, run
```
pip install --user --upgrade matplotlib
pip install --user --upgrade scikit-learn
pip install --user --upgrade hep-ml
```

Edit in the mlskim_NMSSM_XYH_bbbb/config/<file> the cuts, variable, samples and weight name
```
python mlskim_NMSSM_XYH_bbbb/BuildBackgroundModel.py --config=mlskim_NMSSM_XYH_bbbb/config/outputskim_2016.cfg 
````
this will crate a folder with the model within BackgroundModels
Now you can run the next step that will create the a new branch in the origina tree containing the weigths for the BDT weights
```
python mlskim_NMSSM_XYH_bbbb/ApplyBackgroundModel.py --dir BackgroundModels/Reweight_<weightName>
```

Note: weight name must be unique otherwise you will not be able to create the new branch in the tree
make sure you hhave write permission on the input skims
Remember to apply the weight to the fill histogram to use the while filling the plots

## Running combine
Log on a CentOS 7 machine (lxplus, cmslpc-sl7) and install combine following the instructions [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#for-end-users-that-dont-need-to-commit-or-do-any-development)

After compiling  CMSSW, do ``git clone https://github.com/UF-HH/bbbbAnalysis`` (NB: no need to compile the code with scram and make exe since it will only run combine).

Scripts for running combine are under ``limits``.

