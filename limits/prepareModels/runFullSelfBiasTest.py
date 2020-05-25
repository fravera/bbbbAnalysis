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

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',    dest='cfgfile',        help='Name of config file with MC information',   required = True)
parser.add_argument('--signal',    dest='signal',         help='Name of the signal'                     ,   required = True)
args = parser.parse_args()
configFileName = args.cfgfile
signal         = args.signal

print "[INFO] Reading configuration file . . ."
with open(configFileName) as templateConfiguration:
	signalConfiguration = templateConfiguration.read()
signalConfiguration = signalConfiguration.replace("${signalTemplate}",args.signal)
cfgparser = ConfigParser()
cfgparser.optionxform = lambda option: option # preserve lower-upper case
cfgparser.readfp(StringIO(signalConfiguration))

folder       = ast.literal_eval(cfgparser.get("configuration","folder"))
categandobs  = ast.literal_eval(cfgparser.get("configuration","categandobs"))
datasetYear  = ast.literal_eval(cfgparser.get("configuration","dataset"))

os.system("rm -rf " + folder)
os.system("mkdir -p " + folder)

# create plot file
print "[INFO] Creating plot file . . ."

createFileTastCommand = "python prepareModels/prepareHistos.py  --config " + configFileName + " --signal " + signal
createPlotTask = sp.Popen(createFileTastCommand.split(), stdout=sp.PIPE)
streamdata = createPlotTask.communicate()[0]
codeReturned = createPlotTask.returncode
if codeReturned != 0:
    print "createPlotTask failed, Aborting"
    print streamdata
    sys.exit(-1)

# create datacard and workspace
print "[INFO] Creating datacard and workspace . . ."

makeDatacardTaskCommand = "python prepareModels/makeDatacardsAndWorkspaces.py --config " + configFileName + " --no-comb --no-bbb --addScaleSignal --signal " + signal
makeDatacardTask = sp.Popen(makeDatacardTaskCommand.split(), stdout=sp.PIPE)
streamdata = makeDatacardTask.communicate()[0]
codeReturned = makeDatacardTask.returncode
if codeReturned != 0:
    print "makeDatacardTask failed, Aborting"
    print streamdata
    sys.exit(-1)

time.sleep(1)

injectionStrengthList   = [0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0]
toys     = 1000

# workSpaceName = folder + "/datacard" + datasetYear + "_" + categandobs[0][0] + ".root"
workSpaceName = "datacard" + datasetYear + "_" + categandobs[0][0] + ".root"

mainDirectory = os.getcwd()
os.chdir(folder)


#Make limit for the point
print "[INFO] Running limit . . ."

makeLimitCommand = "combine -M AsymptoticLimits -D data_obs --run blind --setParameters r=1,myscale=0 --freezeParameters allConstrainedNuisances " + workSpaceName
makeLimitTask = sp.Popen(makeLimitCommand.split(), stdout=sp.PIPE)
streamdata = makeLimitTask.communicate()[0]
codeReturned = makeLimitTask.returncode
if codeReturned != 0:
    print "makeLimitTask failed, Aborting"
    print streamdata
    sys.exit(-1)

for injectionStrength in injectionStrengthList:
    #Run injection test
    runBiasCommand = "source " + mainDirectory + "/prepareModels/make_biastest.sh " + workSpaceName + " " + str(injectionStrength) + " " + str(toys)
    os.system(runBiasCommand)

plotterCommand = "python " + mainDirectory + "/prepareModels/plot_sig_vs_inj.py --study SelfBiasTest" + datasetYear + " --selsignal " + args.signal + " --dataset " + datasetYear + " --injectionList " + " ".join(str(x) for x in injectionStrengthList)
os.system(plotterCommand)

os.chdir(mainDirectory)

