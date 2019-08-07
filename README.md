# bbbbAnalysis

## Install instructions
```
cmsrel CMSSW_10_2_5
cd CMSSW_10_2_5/src
cmsenv
git cms-addpkg PhysicsTools/KinFitter CommonTools/Utils CondFormats/JetMETObjects CondFormats/Serialization FWCore/MessageLogger FWCore/Utilities JetMETCorrections/Modules
scram b -j 8
git clone https://github.com/UF-HH/bbbbAnalysis
```

## Setup and compile
```
# from bbbbAnalysis/
cmsenv
source scripts/setup.sh # only needed once for every new shell
make exe -j # compiles and makes everything under test/ executable
````

## Make a skim of NanoAOD
```
## please, fill me with some examples of usage
````

## Fill histograms from skims
```
fill_histograms.exe config/<config_name>.cfg
````

## Make plots
Use the ``plotter/plotter.py`` script. Styles (line colors, etc.) for the processes are defined in ``plotter/styles/plotStyles.py``
Inside the script, the subset of processes to run on is defined through ``bkgToPlot`` and  ``sigToPlot``.
Several cmd line options available to configure the plot, it's practical to make a script that produces all the plots.
Example:
```
source do_all_plots.sh
````

## Machine learning skims using pandas dataframes
All developments take place inside mlskim. First, we merge(hadd!) the bbbbntuple files on eos in the inputskims folder by running:
```
cd mlskim/inputskims
source haddsamples.sh

````
Next, one finds two main python scripts inside mlskim folder: Outputskim.py and BackgroundModel.py. The Outputskim.py code is able to skim over the inputskims files adding branches with new ML variables in data and MC samples. The parameters of the skims are specified in the config files in the folder config. To execute the outputskims (2016+2017+2018):
```
source runOutputskim.sh

````
Then, the BackgroundModel.py code creates a background model using the control region information and 3-btag data. This creates four weights (Weight_210_GGF,Weight_210_VBF,Weight_110_GGF,Weight_110_VBF) as branches in the output file (SKIM_MODEL_BKG.root). The parameters are specified in the config files in the folder config. To execute the background model (2016+2017+2018):
```
source runBkgmodel.sh

````
## Running combine
Log on a CentOS 7 machine (lxplus, cmslpc-sl7) and install combine following the instructions [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/#for-end-users-that-dont-need-to-commit-or-do-any-development)

After compiling  CMSSW, do ``git clone https://github.com/UF-HH/bbbbAnalysis`` (NB: no need to compile the code with scram and make exe since it will only run combine).

Scripts for running combine are under ``limits``.

