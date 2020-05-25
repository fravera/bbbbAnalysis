import ROOT
import argparse
import collections
import fnmatch
from  ConfigParser import *
import ast
import os
from subprocess import Popen
import subprocess as sp
from StringIO import StringIO
import sys
import time

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

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',    dest='cfgfile',        help='Name of config file with MC information',   required = True)
args = parser.parse_args()
configFileName = args.cfgfile

for signal in signalList:
    print "[INFO] Reading configuration file . . ."
    with open(configFileName) as templateConfiguration:
        signalConfiguration = templateConfiguration.read()
    signalConfiguration = signalConfiguration.replace("${signalTemplate}",signal)
    cfgparser = ConfigParser()
    cfgparser.optionxform = lambda option: option # preserve lower-upper case
    cfgparser.readfp(StringIO(signalConfiguration))

    folder       = ast.literal_eval(cfgparser.get("configuration","folder"))
    categandobs  = ast.literal_eval(cfgparser.get("configuration","categandobs"))
    datasetYear  = ast.literal_eval(cfgparser.get("configuration","dataset"))
    card_name    = "datacard%s"%datasetYear

    os.system("rm -rf " + folder)
    os.system("mkdir -p " + folder)

    # create plot file
    print "[INFO] Creating plot file . . ."

    createFileTastCommand = "python prepareModels/prepareHistos.py  --config " + configFileName + " --signal " + signal
    createPlotTask = sp.Popen(createFileTastCommand.split(), stdout=sp.PIPE)
    streamdata = createPlotTask.communicate()[0]
    codeReturned = createPlotTask.returncode
    if codeReturned != 0:
        print "createPlotTask failed, Skipping"
        print streamdata
        continue
        # sys.exit(-1)

    # create datacard and workspace
    # print "[INFO] Creating datacard and workspace . . ."

    # makeDatacardTaskCommand = "python prepareModels/makeDatacardsAndWorkspaces.py --config " + configFileName + " --no-comb --no-bbb --signal " + signal
    # makeDatacardTask = sp.Popen(makeDatacardTaskCommand.split(), stdout=sp.PIPE)
    # streamdata = makeDatacardTask.communicate()[0]
    # codeReturned = makeDatacardTask.returncode
    # if codeReturned != 0:
    #     print "makeDatacardTask failed, Skipping"
    #     print streamdata
    #     continue
    #     # sys.exit(-1)
