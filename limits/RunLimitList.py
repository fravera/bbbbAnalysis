from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array

# combineArguments = "-M AsymptoticLimits -D data_BTagCSV -t -1 --run blind"
# combineArguments = "-M AsymptoticLimits -D data_BTagCSV -t -1 --run blind --freezeParameters allConstrainedNuisances "

enableSystematics = False
# inputFile              = "2016DataPlots_NMSSM_XYH_bbbb_all_openClose/outPlotter.root"
inputFile              = "2016DataPlots_NMSSM_XYH_bbbb_all_Full_xsec_test/outPlotter.root"
bbbbAnalysisFolderPath = "/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_5/src/bbbbAnalysis"
dataDataset            = "data_BTagCSV"
inputFileFullPath = bbbbAnalysisFolderPath + "/" + inputFile
xBinning = [250, 350, 450, 550, 650, 750, 850, 950, 1050, 1150, 1300, 1500, 1700, 1900, 2100]

# analysisRegion = "LMR"
# selection              = "selectionbJetsLMR_SignalRegion"
# backgroundDataset      = "data_BTagCSV_dataDriven_LMR"
# yBinning = [55, 65, 75, 85, 95, 112.5, 137.5]
# signalList = [
#       "sig_NMSSM_bbbb_MX_300_MY_60", "sig_NMSSM_bbbb_MX_300_MY_70", "sig_NMSSM_bbbb_MX_300_MY_80", "sig_NMSSM_bbbb_MX_300_MY_90", "sig_NMSSM_bbbb_MX_300_MY_100", "sig_NMSSM_bbbb_MX_300_MY_125"
#     , "sig_NMSSM_bbbb_MX_400_MY_60", "sig_NMSSM_bbbb_MX_400_MY_70", "sig_NMSSM_bbbb_MX_400_MY_80", "sig_NMSSM_bbbb_MX_400_MY_90", "sig_NMSSM_bbbb_MX_400_MY_100", "sig_NMSSM_bbbb_MX_400_MY_125"
#     , "sig_NMSSM_bbbb_MX_500_MY_60", "sig_NMSSM_bbbb_MX_500_MY_70", "sig_NMSSM_bbbb_MX_500_MY_80", "sig_NMSSM_bbbb_MX_500_MY_90", "sig_NMSSM_bbbb_MX_500_MY_100", "sig_NMSSM_bbbb_MX_500_MY_125"
#     , "sig_NMSSM_bbbb_MX_600_MY_60", "sig_NMSSM_bbbb_MX_600_MY_70", "sig_NMSSM_bbbb_MX_600_MY_80", "sig_NMSSM_bbbb_MX_600_MY_90", "sig_NMSSM_bbbb_MX_600_MY_100", "sig_NMSSM_bbbb_MX_600_MY_125"
#     , "sig_NMSSM_bbbb_MX_700_MY_60", "sig_NMSSM_bbbb_MX_700_MY_70", "sig_NMSSM_bbbb_MX_700_MY_80", "sig_NMSSM_bbbb_MX_700_MY_90", "sig_NMSSM_bbbb_MX_700_MY_100", "sig_NMSSM_bbbb_MX_700_MY_125"
#     , "sig_NMSSM_bbbb_MX_800_MY_60", "sig_NMSSM_bbbb_MX_800_MY_70", "sig_NMSSM_bbbb_MX_800_MY_80", "sig_NMSSM_bbbb_MX_800_MY_90", "sig_NMSSM_bbbb_MX_800_MY_100", "sig_NMSSM_bbbb_MX_800_MY_125"
#     , "sig_NMSSM_bbbb_MX_900_MY_60", "sig_NMSSM_bbbb_MX_900_MY_70", "sig_NMSSM_bbbb_MX_900_MY_80", "sig_NMSSM_bbbb_MX_900_MY_90", "sig_NMSSM_bbbb_MX_900_MY_100", "sig_NMSSM_bbbb_MX_900_MY_125"
#     , "sig_NMSSM_bbbb_MX_1000_MY_60", "sig_NMSSM_bbbb_MX_1000_MY_70", "sig_NMSSM_bbbb_MX_1000_MY_80", "sig_NMSSM_bbbb_MX_1000_MY_90", "sig_NMSSM_bbbb_MX_1000_MY_100", "sig_NMSSM_bbbb_MX_1000_MY_125"
#     , "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125"
#     , "sig_NMSSM_bbbb_MX_1200_MY_90", "sig_NMSSM_bbbb_MX_1200_MY_100", "sig_NMSSM_bbbb_MX_1200_MY_125"
#     , "sig_NMSSM_bbbb_MX_1400_MY_90", "sig_NMSSM_bbbb_MX_1400_MY_100", "sig_NMSSM_bbbb_MX_1400_MY_125"
#     , "sig_NMSSM_bbbb_MX_1600_MY_90", "sig_NMSSM_bbbb_MX_1600_MY_100", "sig_NMSSM_bbbb_MX_1600_MY_125"
#     , "sig_NMSSM_bbbb_MX_1800_MY_90", "sig_NMSSM_bbbb_MX_1800_MY_100", "sig_NMSSM_bbbb_MX_1800_MY_125"
#     , "sig_NMSSM_bbbb_MX_2000_MY_90", "sig_NMSSM_bbbb_MX_2000_MY_100", "sig_NMSSM_bbbb_MX_2000_MY_125"
# ]


# analysisRegion = "HMR"
# selection              = "selectionbJetsHMR_SignalRegion"
# backgroundDataset      = "data_BTagCSV_dataDriven_HMR"
# yBinning = [112.5, 137.5, 175, 225, 275, 350, 450, 550, 650, 750, 850, 950, 1100, 1300, 1500, 1700, 1900]
# signalList = [
#       "sig_NMSSM_bbbb_MX_300_MY_125"
#     , "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_400_MY_150", "sig_NMSSM_bbbb_MX_400_MY_200", "sig_NMSSM_bbbb_MX_400_MY_250"
#     , "sig_NMSSM_bbbb_MX_500_MY_125", "sig_NMSSM_bbbb_MX_500_MY_150", "sig_NMSSM_bbbb_MX_500_MY_200", "sig_NMSSM_bbbb_MX_500_MY_250", "sig_NMSSM_bbbb_MX_500_MY_300"
#     , "sig_NMSSM_bbbb_MX_600_MY_125", "sig_NMSSM_bbbb_MX_600_MY_150", "sig_NMSSM_bbbb_MX_600_MY_200", "sig_NMSSM_bbbb_MX_600_MY_250", "sig_NMSSM_bbbb_MX_600_MY_300", "sig_NMSSM_bbbb_MX_600_MY_400"
#     , "sig_NMSSM_bbbb_MX_700_MY_125", "sig_NMSSM_bbbb_MX_700_MY_150", "sig_NMSSM_bbbb_MX_700_MY_200", "sig_NMSSM_bbbb_MX_700_MY_250", "sig_NMSSM_bbbb_MX_700_MY_300", "sig_NMSSM_bbbb_MX_700_MY_400", "sig_NMSSM_bbbb_MX_700_MY_500"
#     , "sig_NMSSM_bbbb_MX_800_MY_125", "sig_NMSSM_bbbb_MX_800_MY_150", "sig_NMSSM_bbbb_MX_800_MY_200", "sig_NMSSM_bbbb_MX_800_MY_250", "sig_NMSSM_bbbb_MX_800_MY_300", "sig_NMSSM_bbbb_MX_800_MY_400", "sig_NMSSM_bbbb_MX_800_MY_500", "sig_NMSSM_bbbb_MX_800_MY_600"
#     , "sig_NMSSM_bbbb_MX_900_MY_125", "sig_NMSSM_bbbb_MX_900_MY_150", "sig_NMSSM_bbbb_MX_900_MY_200", "sig_NMSSM_bbbb_MX_900_MY_250", "sig_NMSSM_bbbb_MX_900_MY_300", "sig_NMSSM_bbbb_MX_900_MY_400", "sig_NMSSM_bbbb_MX_900_MY_500", "sig_NMSSM_bbbb_MX_900_MY_600", "sig_NMSSM_bbbb_MX_900_MY_700"
#     , "sig_NMSSM_bbbb_MX_1000_MY_125", "sig_NMSSM_bbbb_MX_1000_MY_150", "sig_NMSSM_bbbb_MX_1000_MY_200", "sig_NMSSM_bbbb_MX_1000_MY_250", "sig_NMSSM_bbbb_MX_1000_MY_300", "sig_NMSSM_bbbb_MX_1000_MY_400", "sig_NMSSM_bbbb_MX_1000_MY_500", "sig_NMSSM_bbbb_MX_1000_MY_600", "sig_NMSSM_bbbb_MX_1000_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100"
#     , "sig_NMSSM_bbbb_MX_1100_MY_125", "sig_NMSSM_bbbb_MX_1100_MY_150", "sig_NMSSM_bbbb_MX_1100_MY_200", "sig_NMSSM_bbbb_MX_1100_MY_250", "sig_NMSSM_bbbb_MX_1100_MY_300", "sig_NMSSM_bbbb_MX_1100_MY_400", "sig_NMSSM_bbbb_MX_1100_MY_500", "sig_NMSSM_bbbb_MX_1100_MY_600", "sig_NMSSM_bbbb_MX_1100_MY_700", "sig_NMSSM_bbbb_MX_1100_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_900"
#     , "sig_NMSSM_bbbb_MX_1200_MY_125", "sig_NMSSM_bbbb_MX_1200_MY_150", "sig_NMSSM_bbbb_MX_1200_MY_200", "sig_NMSSM_bbbb_MX_1200_MY_250", "sig_NMSSM_bbbb_MX_1200_MY_300", "sig_NMSSM_bbbb_MX_1200_MY_400", "sig_NMSSM_bbbb_MX_1200_MY_500", "sig_NMSSM_bbbb_MX_1200_MY_600", "sig_NMSSM_bbbb_MX_1200_MY_700", "sig_NMSSM_bbbb_MX_1200_MY_800", "sig_NMSSM_bbbb_MX_1200_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_1000"
#     , "sig_NMSSM_bbbb_MX_1400_MY_125", "sig_NMSSM_bbbb_MX_1400_MY_150", "sig_NMSSM_bbbb_MX_1400_MY_200", "sig_NMSSM_bbbb_MX_1400_MY_250", "sig_NMSSM_bbbb_MX_1400_MY_300", "sig_NMSSM_bbbb_MX_1400_MY_400", "sig_NMSSM_bbbb_MX_1400_MY_500", "sig_NMSSM_bbbb_MX_1400_MY_600", "sig_NMSSM_bbbb_MX_1400_MY_700", "sig_NMSSM_bbbb_MX_1400_MY_800", "sig_NMSSM_bbbb_MX_1400_MY_900", "sig_NMSSM_bbbb_MX_1400_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_1200"
#     , "sig_NMSSM_bbbb_MX_1600_MY_125", "sig_NMSSM_bbbb_MX_1600_MY_150", "sig_NMSSM_bbbb_MX_1600_MY_200", "sig_NMSSM_bbbb_MX_1600_MY_250", "sig_NMSSM_bbbb_MX_1600_MY_300", "sig_NMSSM_bbbb_MX_1600_MY_400", "sig_NMSSM_bbbb_MX_1600_MY_500", "sig_NMSSM_bbbb_MX_1600_MY_600", "sig_NMSSM_bbbb_MX_1600_MY_700", "sig_NMSSM_bbbb_MX_1600_MY_800", "sig_NMSSM_bbbb_MX_1600_MY_900", "sig_NMSSM_bbbb_MX_1600_MY_1000", "sig_NMSSM_bbbb_MX_1600_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_1400"
#     , "sig_NMSSM_bbbb_MX_1800_MY_125", "sig_NMSSM_bbbb_MX_1800_MY_150", "sig_NMSSM_bbbb_MX_1800_MY_200", "sig_NMSSM_bbbb_MX_1800_MY_250", "sig_NMSSM_bbbb_MX_1800_MY_300", "sig_NMSSM_bbbb_MX_1800_MY_400", "sig_NMSSM_bbbb_MX_1800_MY_500", "sig_NMSSM_bbbb_MX_1800_MY_600", "sig_NMSSM_bbbb_MX_1800_MY_700", "sig_NMSSM_bbbb_MX_1800_MY_800", "sig_NMSSM_bbbb_MX_1800_MY_900", "sig_NMSSM_bbbb_MX_1800_MY_1000", "sig_NMSSM_bbbb_MX_1800_MY_1200", "sig_NMSSM_bbbb_MX_1800_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_1600"
#     , "sig_NMSSM_bbbb_MX_2000_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_150", "sig_NMSSM_bbbb_MX_2000_MY_200", "sig_NMSSM_bbbb_MX_2000_MY_250", "sig_NMSSM_bbbb_MX_2000_MY_300", "sig_NMSSM_bbbb_MX_2000_MY_400", "sig_NMSSM_bbbb_MX_2000_MY_500", "sig_NMSSM_bbbb_MX_2000_MY_600", "sig_NMSSM_bbbb_MX_2000_MY_700", "sig_NMSSM_bbbb_MX_2000_MY_800", "sig_NMSSM_bbbb_MX_2000_MY_900", "sig_NMSSM_bbbb_MX_2000_MY_1000", "sig_NMSSM_bbbb_MX_2000_MY_1200", "sig_NMSSM_bbbb_MX_2000_MY_1400", "sig_NMSSM_bbbb_MX_2000_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_1800"
# ]


analysisRegion = "Full"
selection              = "selectionbJets_SignalRegion"
backgroundDataset      = "data_BTagCSV_dataDriven"
yBinning = [55, 65, 75, 85, 95, 112.5, 137.5, 175, 225, 275, 350, 450, 550, 650, 750, 850, 950, 1100, 1300, 1500, 1700, 1900]
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


if enableSystematics : 
    combineArguments = "-M AsymptoticLimits -D data_BTagCSV --run blind"
    outputAppend = "syst"
else                 : 
    combineArguments = "-M AsymptoticLimits -D data_BTagCSV --run blind --freezeParameters allConstrainedNuisances "
    outputAppend = "statOnly"


limitMap2sigmaDown = TH2D("LimitMap2sigmaDown","2 Sigma Down Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMap1sigmaDown = TH2D("LimitMap1sigmaDown","1 Sigma Down Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMapCentral    = TH2D("LimitMapCentral"   ,"Central Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMap1sigmaUp   = TH2D("LimitMap1sigmaUp"  ,"1 Sigma Up Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMap2sigmaUp   = TH2D("LimitMap2sigmaUp"  ,"2 Sigma Up Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--runAll'   , dest='runAll'   , help='Run all steps'   , action='store_true', default = False, required = False)
parser.add_argument('--datacard' , dest='datacard' , help='Create datacard' , action='store_true', default = False, required = False)
parser.add_argument('--workspace', dest='workspace', help='Create workspace', action='store_true', default = False, required = False)
parser.add_argument('--limits'   , dest='limits'   , help='Run limits'      , action='store_true', default = False, required = False)
args = parser.parse_args()

if args.runAll:
    createDatacard  = True
    createWorkSpace = True
    runLimits       = True
else:
    createDatacard  = args.datacard
    createWorkSpace = args.workspace
    runLimits       = args.limits

yMassesList = []

brazilianPlotList = {}
for signal in signalList:
    massYTmp =  float(signal[ signal.find("_MY_") + len("_MY_"): ])
    if massYTmp not in yMassesList: 
        # yMassesList.append(massYTmp)
        brazilianPlotList[massYTmp] = [
            TGraph(),
            TGraphAsymmErrors(),
            TGraphAsymmErrors()
        ]
        brazilianPlotList[massYTmp][0].SetNameTitle("CentralLimit_massY_" + str(massYTmp), " Central Limit - m_{X} = " + str(massYTmp))
        brazilianPlotList[massYTmp][0].GetXaxis().SetTitle("m_{X} [GeV]")
        brazilianPlotList[massYTmp][0].SetMaximum(10000)
        brazilianPlotList[massYTmp][0].SetMinimum(1)
        brazilianPlotList[massYTmp][0].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
        brazilianPlotList[massYTmp][1].SetNameTitle("1SigmaLimit_massY_"  + str(massYTmp), " 1 Sigma Limit - m_{X} = " + str(massYTmp))
        brazilianPlotList[massYTmp][0].SetMaximum(10000)
        brazilianPlotList[massYTmp][0].SetMinimum(1)
        brazilianPlotList[massYTmp][1].GetXaxis().SetTitle("m_{X} [GeV]")
        brazilianPlotList[massYTmp][1].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
        brazilianPlotList[massYTmp][2].SetNameTitle("2SigmaLimit_massY_"  + str(massYTmp), " 2 Sigma Limit - m_{X} = " + str(massYTmp))
        brazilianPlotList[massYTmp][0].SetMaximum(10000)
        brazilianPlotList[massYTmp][0].SetMinimum(1)
        brazilianPlotList[massYTmp][2].GetXaxis().SetTitle("m_{X} [GeV]")
        brazilianPlotList[massYTmp][2].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
        brazilianPlotList[massYTmp][1].SetFillColor(ROOT.kGreen)
        brazilianPlotList[massYTmp][2].SetFillColor(ROOT.kYellow)


for signal in signalList:

    print "Current signal = ", signal
    massXstring =  signal[ signal.find("_MX_") + len("_MX_"): signal.find("_MY_" ) ]
    massYstring =  signal[ signal.find("_MY_") + len("_MY_"): ]
    massX = float(massXstring)
    massY = float(massYstring)

    outputFolderName = "limits/" + analysisRegion + "/"
    datacardName = outputFolderName + "datacard_" + signal + ".txt"
    removeOldDatacard = "rm -f " + datacardName
    os.system(removeOldDatacard)
    workspaceName = datacardName[:-4] + ".root"
    removeOldWorkspace = "rm -f " + workspaceName
    os.system(removeOldWorkspace)

    if createDatacard:
        makeDatacardCommand = "python limits/make_datacard_NMMSM_XYH_bbbb.py --sel " + selection + " --fileIn " + inputFileFullPath + " --bkgs " + backgroundDataset + " --sigs " + signal + " --cardOut " + datacardName
        os.system(makeDatacardCommand)

    if os.path.isfile('filename.txt'):
        print "File ", datacardName, " does not exist, skipping"
        continue

    if createWorkSpace:
        massPoint = massX * 10000 + massY
        createWorkspaceCommand = "text2workspace.py " + datacardName + " -D " + dataDataset + " -m " + str(massPoint)
        os.system(createWorkspaceCommand)

    if runLimits:
        combineCommand = "combine " + workspaceName + " " + combineArguments

        out = subprocess.Popen(combineCommand.split(), 
            stdout=subprocess.PIPE, 
            stderr=subprocess.STDOUT)

        stdout,stderr = out.communicate()
        print(stdout)
        if stderr != None :
            print(stderr)
            break            

        LimitTag2SigmaDown = "Expected  2.5%: r < "
        LimitTag1SigmaDown = "Expected 16.0%: r < "
        LimitTagCentral    = "Expected 50.0%: r < "
        LimitTag1SigmaUp   = "Expected 84.0%: r < "
        LimitTag2SigmaUp   = "Expected 97.5%: r < "
        multiplier = 100.
        if massX >= 600: multiplier = 10.
        if massX >= 1600: multiplier = 1.
        for line in stdout.splitlines():
            if LimitTag2SigmaDown in line:
                limit2SigmaDown = float(line[line.find(LimitTag2SigmaDown) + len(LimitTag2SigmaDown) : ])*multiplier
                limitMap2sigmaDown.Fill(massX,massY,limit2SigmaDown)
            if LimitTag1SigmaDown in line:
                limit1SigmaDown = float(line[line.find(LimitTag1SigmaDown) + len(LimitTag1SigmaDown) : ])*multiplier
                limitMap1sigmaDown.Fill(massX,massY,limit1SigmaDown)
            if LimitTagCentral in line:
                limitCentral    = float(line[line.find(LimitTagCentral) + len(LimitTagCentral) : ])*multiplier
                limitMapCentral.Fill(massX,massY,limitCentral)
            if LimitTag1SigmaUp in line:
                limit1SigmaUp   = float(line[line.find(LimitTag1SigmaUp) + len(LimitTag1SigmaUp) : ])*multiplier
                limitMap1sigmaUp.Fill(massX,massY,limit1SigmaUp)
            if LimitTag2SigmaUp in line:
                limit2SigmaUp   = float(line[line.find(LimitTag2SigmaUp) + len(LimitTag2SigmaUp) : ])*multiplier
                limitMap2sigmaUp.Fill(massX,massY,limit2SigmaUp)

        brazilianPlotList[massY][0].SetPoint(brazilianPlotList[massY][0].GetN()  , massX, limitCentral)
        brazilianPlotList[massY][1].SetPoint(brazilianPlotList[massY][1].GetN()  , massX, limitCentral)
        brazilianPlotList[massY][2].SetPoint(brazilianPlotList[massY][2].GetN()  , massX,  limitCentral)
        brazilianPlotList[massY][1].SetPointError(brazilianPlotList[massY][1].GetN()-1, 0., 0., limitCentral - limit1SigmaDown, limit1SigmaUp - limitCentral)
        brazilianPlotList[massY][2].SetPointError(brazilianPlotList[massY][2].GetN()-1, 0., 0., limitCentral - limit2SigmaDown, limit2SigmaUp - limitCentral)

if runLimits:
    theCanvas = TCanvas("limitMapCentral", "limitMapCentral", 1400, 800)
    limitMapCentral.Draw("colz text")
    limitMapCentral.SetStats(0)
    limitMapCentral.SetMaximum(2000)
    limitMapCentral.SetMinimum(1)
    theCanvas.SetLogz()
    theCanvas.SaveAs(outputFolderName + "limitMapCentral_" + outputAppend + ".png")

    limitMap2sigmaDown.SetStats(0)
    limitMap2sigmaDown.SetMaximum(2000)
    limitMap2sigmaDown.SetMinimum(1)

    limitMap1sigmaDown.SetStats(0)
    limitMap1sigmaDown.SetMaximum(2000)
    limitMap1sigmaDown.SetMinimum(1)

    limitMap1sigmaUp.SetStats(0)
    limitMap1sigmaUp.SetMaximum(2000)
    limitMap1sigmaUp.SetMinimum(1)

    limitMap2sigmaUp.SetStats(0)
    limitMap2sigmaUp.SetMaximum(2000)
    limitMap2sigmaUp.SetMinimum(1)

    outputFile = TFile(outputFolderName + "Limits_" + outputAppend + ".root", "RECREATE")
    theCanvas.Write()
    limitMap2sigmaDown.Write()
    limitMap1sigmaDown.Write()
    limitMapCentral   .Write()
    limitMap1sigmaUp  .Write()
    limitMap2sigmaUp  .Write()

    for mappPoint in brazilianPlotList:
        for plot in brazilianPlotList[mappPoint]:
            plot.Write()
        theBrazilianCanvas = TCanvas("BrazilianLimits_massY_" + str(mappPoint), "BrazilianLimits_massY_" + str(mappPoint), 1400, 800)
        brazilianPlotList[mappPoint][2].Draw("a3")
        brazilianPlotList[mappPoint][1].Draw("Same3")
        brazilianPlotList[mappPoint][0].Draw("same")
        theBrazilianCanvas.SetLogy()
        theBrazilianCanvas.Write()

    raw_input("Press Enter to continue...")
