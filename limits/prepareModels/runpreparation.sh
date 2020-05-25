#### Step0. Define pythonpath 
export PYTHONPATH=/uscms_data/d3/guerrero/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/limits/physicsModels
#### Step1. Prepare histograms
python prepareHistos.py --config config/outputhistos2018.cfg
#### Step2. Prepare datacards per category and their combinations then their associated workspace
python makeDatacardsAndWorkspaces.py --config config/outputhistos2018.cfg
# python makeDatacardsAndWorkspaces.py --config config/outputhistos2018.cfg --no-comb --card-only