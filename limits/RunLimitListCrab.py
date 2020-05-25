from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array

enableSystematics = True

bbbbAnalysisFolderPath = "/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_5/src/bbbbAnalysis"
dataDataset            = "data_BTagCSV"

selection              = "selectionbJets_SignalRegion"
backgroundDataset      = "data_BTagCSV_dataDriven"
xBinning = [250, 350, 450, 550, 650, 750, 850, 950, 1050, 1150, 1300, 1500, 1700, 1900, 2100]
yBinning = [55, 65, 75, 85, 95, 112.5, 137.5, 175, 225, 275, 350, 450, 550, 650, 750, 850, 950, 1100, 1300, 1500, 1700, 1900]
# signalList = [
#     "sig_NMSSM_bbbb_MX_300_MY_125", "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_500_MY_125", "sig_NMSSM_bbbb_MX_700_MY_125", "sig_NMSSM_bbbb_MX_800_MY_125", "sig_NMSSM_bbbb_MX_900_MY_125", "sig_NMSSM_bbbb_MX_1000_MY_125", "sig_NMSSM_bbbb_MX_1100_MY_125", "sig_NMSSM_bbbb_MX_1200_MY_125", "sig_NMSSM_bbbb_MX_1400_MY_125", "sig_NMSSM_bbbb_MX_1600_MY_125", "sig_NMSSM_bbbb_MX_1800_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_125"
# ]

signalList = [
      "sig_NMSSM_bbbb_MX_300_MY_60", "sig_NMSSM_bbbb_MX_300_MY_70", "sig_NMSSM_bbbb_MX_300_MY_80", "sig_NMSSM_bbbb_MX_300_MY_90", "sig_NMSSM_bbbb_MX_300_MY_100", "sig_NMSSM_bbbb_MX_300_MY_125"
    , "sig_NMSSM_bbbb_MX_400_MY_60", "sig_NMSSM_bbbb_MX_400_MY_70", "sig_NMSSM_bbbb_MX_400_MY_80", "sig_NMSSM_bbbb_MX_400_MY_90", "sig_NMSSM_bbbb_MX_400_MY_100", "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_400_MY_150", "sig_NMSSM_bbbb_MX_400_MY_200", "sig_NMSSM_bbbb_MX_400_MY_250"
    , "sig_NMSSM_bbbb_MX_500_MY_60", "sig_NMSSM_bbbb_MX_500_MY_70", "sig_NMSSM_bbbb_MX_500_MY_80", "sig_NMSSM_bbbb_MX_500_MY_90", "sig_NMSSM_bbbb_MX_500_MY_100", "sig_NMSSM_bbbb_MX_500_MY_125", "sig_NMSSM_bbbb_MX_500_MY_150", "sig_NMSSM_bbbb_MX_500_MY_200", "sig_NMSSM_bbbb_MX_500_MY_250", "sig_NMSSM_bbbb_MX_500_MY_300"
    , "sig_NMSSM_bbbb_MX_600_MY_60", "sig_NMSSM_bbbb_MX_600_MY_70", "sig_NMSSM_bbbb_MX_600_MY_80", "sig_NMSSM_bbbb_MX_600_MY_90", "sig_NMSSM_bbbb_MX_600_MY_100", "sig_NMSSM_bbbb_MX_600_MY_125", "sig_NMSSM_bbbb_MX_600_MY_150", "sig_NMSSM_bbbb_MX_600_MY_200", "sig_NMSSM_bbbb_MX_600_MY_250", "sig_NMSSM_bbbb_MX_600_MY_300", "sig_NMSSM_bbbb_MX_600_MY_400"
    , "sig_NMSSM_bbbb_MX_700_MY_60", "sig_NMSSM_bbbb_MX_700_MY_70", "sig_NMSSM_bbbb_MX_700_MY_80", "sig_NMSSM_bbbb_MX_700_MY_90", "sig_NMSSM_bbbb_MX_700_MY_100", "sig_NMSSM_bbbb_MX_700_MY_125", "sig_NMSSM_bbbb_MX_700_MY_150", "sig_NMSSM_bbbb_MX_700_MY_200", "sig_NMSSM_bbbb_MX_700_MY_250", "sig_NMSSM_bbbb_MX_700_MY_300", "sig_NMSSM_bbbb_MX_700_MY_400", "sig_NMSSM_bbbb_MX_700_MY_500"
    , "sig_NMSSM_bbbb_MX_800_MY_60", "sig_NMSSM_bbbb_MX_800_MY_70", "sig_NMSSM_bbbb_MX_800_MY_80", "sig_NMSSM_bbbb_MX_800_MY_90", "sig_NMSSM_bbbb_MX_800_MY_100", "sig_NMSSM_bbbb_MX_800_MY_125", "sig_NMSSM_bbbb_MX_800_MY_150", "sig_NMSSM_bbbb_MX_800_MY_200", "sig_NMSSM_bbbb_MX_800_MY_250", "sig_NMSSM_bbbb_MX_800_MY_300", "sig_NMSSM_bbbb_MX_800_MY_400", "sig_NMSSM_bbbb_MX_800_MY_500", "sig_NMSSM_bbbb_MX_800_MY_600"
    , "sig_NMSSM_bbbb_MX_900_MY_60", "sig_NMSSM_bbbb_MX_900_MY_70", "sig_NMSSM_bbbb_MX_900_MY_80", "sig_NMSSM_bbbb_MX_900_MY_90", "sig_NMSSM_bbbb_MX_900_MY_100", "sig_NMSSM_bbbb_MX_900_MY_125", "sig_NMSSM_bbbb_MX_900_MY_150", "sig_NMSSM_bbbb_MX_900_MY_200", "sig_NMSSM_bbbb_MX_900_MY_250", "sig_NMSSM_bbbb_MX_900_MY_300", "sig_NMSSM_bbbb_MX_900_MY_400", "sig_NMSSM_bbbb_MX_900_MY_500", "sig_NMSSM_bbbb_MX_900_MY_600", "sig_NMSSM_bbbb_MX_900_MY_700"
    , "sig_NMSSM_bbbb_MX_1000_MY_60", "sig_NMSSM_bbbb_MX_1000_MY_70", "sig_NMSSM_bbbb_MX_1000_MY_80", "sig_NMSSM_bbbb_MX_1000_MY_90", "sig_NMSSM_bbbb_MX_1000_MY_100", "sig_NMSSM_bbbb_MX_1000_MY_125", "sig_NMSSM_bbbb_MX_1000_MY_150", "sig_NMSSM_bbbb_MX_1000_MY_200", "sig_NMSSM_bbbb_MX_1000_MY_250", "sig_NMSSM_bbbb_MX_1000_MY_300", "sig_NMSSM_bbbb_MX_1000_MY_400", "sig_NMSSM_bbbb_MX_1000_MY_500", "sig_NMSSM_bbbb_MX_1000_MY_600", "sig_NMSSM_bbbb_MX_1000_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100"
    , "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125", "sig_NMSSM_bbbb_MX_1100_MY_150", "sig_NMSSM_bbbb_MX_1100_MY_200", "sig_NMSSM_bbbb_MX_1100_MY_250", "sig_NMSSM_bbbb_MX_1100_MY_300", "sig_NMSSM_bbbb_MX_1100_MY_400", "sig_NMSSM_bbbb_MX_1100_MY_500", "sig_NMSSM_bbbb_MX_1100_MY_600", "sig_NMSSM_bbbb_MX_1100_MY_700", "sig_NMSSM_bbbb_MX_1100_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_900"
    , "sig_NMSSM_bbbb_MX_1200_MY_90", "sig_NMSSM_bbbb_MX_1200_MY_100", "sig_NMSSM_bbbb_MX_1200_MY_125", "sig_NMSSM_bbbb_MX_1200_MY_150", "sig_NMSSM_bbbb_MX_1200_MY_200", "sig_NMSSM_bbbb_MX_1200_MY_250", "sig_NMSSM_bbbb_MX_1200_MY_300", "sig_NMSSM_bbbb_MX_1200_MY_400", "sig_NMSSM_bbbb_MX_1200_MY_500", "sig_NMSSM_bbbb_MX_1200_MY_600", "sig_NMSSM_bbbb_MX_1200_MY_700", "sig_NMSSM_bbbb_MX_1200_MY_800", "sig_NMSSM_bbbb_MX_1200_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_125"
    , "sig_NMSSM_bbbb_MX_1400_MY_90", "sig_NMSSM_bbbb_MX_1400_MY_100", "sig_NMSSM_bbbb_MX_1400_MY_150", "sig_NMSSM_bbbb_MX_1400_MY_200", "sig_NMSSM_bbbb_MX_1400_MY_250", "sig_NMSSM_bbbb_MX_1400_MY_300", "sig_NMSSM_bbbb_MX_1400_MY_400", "sig_NMSSM_bbbb_MX_1400_MY_500", "sig_NMSSM_bbbb_MX_1400_MY_600", "sig_NMSSM_bbbb_MX_1400_MY_700", "sig_NMSSM_bbbb_MX_1400_MY_800", "sig_NMSSM_bbbb_MX_1400_MY_900", "sig_NMSSM_bbbb_MX_1400_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_125"
    , "sig_NMSSM_bbbb_MX_1600_MY_90", "sig_NMSSM_bbbb_MX_1600_MY_100", "sig_NMSSM_bbbb_MX_1600_MY_150", "sig_NMSSM_bbbb_MX_1600_MY_200", "sig_NMSSM_bbbb_MX_1600_MY_250", "sig_NMSSM_bbbb_MX_1600_MY_300", "sig_NMSSM_bbbb_MX_1600_MY_400", "sig_NMSSM_bbbb_MX_1600_MY_500", "sig_NMSSM_bbbb_MX_1600_MY_600", "sig_NMSSM_bbbb_MX_1600_MY_700", "sig_NMSSM_bbbb_MX_1600_MY_800", "sig_NMSSM_bbbb_MX_1600_MY_900", "sig_NMSSM_bbbb_MX_1600_MY_1000", "sig_NMSSM_bbbb_MX_1600_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_125"
    , "sig_NMSSM_bbbb_MX_1800_MY_90", "sig_NMSSM_bbbb_MX_1800_MY_100", "sig_NMSSM_bbbb_MX_1800_MY_150", "sig_NMSSM_bbbb_MX_1800_MY_200", "sig_NMSSM_bbbb_MX_1800_MY_250", "sig_NMSSM_bbbb_MX_1800_MY_300", "sig_NMSSM_bbbb_MX_1800_MY_400", "sig_NMSSM_bbbb_MX_1800_MY_500", "sig_NMSSM_bbbb_MX_1800_MY_600", "sig_NMSSM_bbbb_MX_1800_MY_700", "sig_NMSSM_bbbb_MX_1800_MY_800", "sig_NMSSM_bbbb_MX_1800_MY_900", "sig_NMSSM_bbbb_MX_1800_MY_1000", "sig_NMSSM_bbbb_MX_1800_MY_1200", "sig_NMSSM_bbbb_MX_1800_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_1600"
    , "sig_NMSSM_bbbb_MX_2000_MY_90", "sig_NMSSM_bbbb_MX_2000_MY_100", "sig_NMSSM_bbbb_MX_2000_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_150", "sig_NMSSM_bbbb_MX_2000_MY_200", "sig_NMSSM_bbbb_MX_2000_MY_250", "sig_NMSSM_bbbb_MX_2000_MY_300", "sig_NMSSM_bbbb_MX_2000_MY_400", "sig_NMSSM_bbbb_MX_2000_MY_500", "sig_NMSSM_bbbb_MX_2000_MY_600", "sig_NMSSM_bbbb_MX_2000_MY_700", "sig_NMSSM_bbbb_MX_2000_MY_800", "sig_NMSSM_bbbb_MX_2000_MY_900", "sig_NMSSM_bbbb_MX_2000_MY_1000", "sig_NMSSM_bbbb_MX_2000_MY_1200", "sig_NMSSM_bbbb_MX_2000_MY_1400", "sig_NMSSM_bbbb_MX_2000_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_1800"
]

crabTaskName = "combine"

combineArguments = "combineTool.py -M AsymptoticLimits  --run blind  --job-mode crab3 --custom-crab ../../prepareModels/custom_crab.py --task-name " + crabTaskName + " "

if not enableSystematics : 
    combineArguments += " --freezeParameters allConstrainedNuisances "
    outputAppend = "statOnly"
else : 
    outputAppend = "syst"


parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--year', dest='year', help='Output Directory', default = ""   , required = True)
args = parser.parse_args()

year = args.year

yearFolderName = os.getcwd() + "/Limits" + str(year)
os.chdir(yearFolderName)

for signal in signalList:

    if not os.path.exists(signal):
        print "Workspace ", signal, " does not exist, skypping"
        continue
    
    os.chdir(signal)
    if os.path.exists('crab_%s'%crabTaskName):
        crabPurgeCommans = 'crab purge --dir=crab_%s'%crabTaskName
    os.system('rm -rf crab_%s'%crabTaskName)

    workspaceName = "datacard" + str(year) + "_selectionbJets_SignalRegion.root"
    combineToolCommand = combineArguments + " -d " + workspaceName
    if not os.path.exists(workspaceName):
        print "Workspace ", workspaceName, " does not exist, skypping"
    else:
        os.system(combineToolCommand)

    os.chdir(yearFolderName)
    