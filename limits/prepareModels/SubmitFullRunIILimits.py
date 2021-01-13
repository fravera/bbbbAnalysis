import ast
import os
import sys
import getpass
import argparse
from  ConfigParser import *
from StringIO import StringIO
import subprocess
import copy

t3SubmitScript = '/uscms/home/fravera/nobackup/DiHiggs_v2/CMSSW_10_2_5/src/bbbbAnalysis/scripts/t3submit'

def writeln(f, line):
    f.write(line + '\n')


parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--year'   , dest = 'year'   , help = 'year'           , required = True)
parser.add_argument('--tag'    , dest = 'tag'    , help = 'production tag' , required = True)
parser.add_argument('--impacts', dest = 'impacts', help = 'Measure impacts', action='store_true', default=False)

args = parser.parse_args()

if args.year == "RunII":
    yearList = ["2016", "2017", "2018"]#, "All"]
else:
    yearList = [args.year]

if not args.tag:
    print "... please provide a non-empty tag name (are you using --tag=$1 without cmd line argument?)"
    sys.exit()

username = getpass.getuser()
print "... Welcome", username

outputDir = "root://cmseos.fnal.gov//store/user/{0}/bbbb_limits/"
outputDir = outputDir.format(username)
listOfSystematics = ["CMS_bkgnorm", "CMS_bkgShape", "lumi_13TeV", "CMS_trg_eff", "CMS_l1prefiring", "CMS_eff_b_b", "CMS_eff_b_c", "CMS_eff_b_udsg", "CMS_PU", "CMS_scale_j_Total", "CMS_res_j", "CMS_res_j_breg"]

LimitOptionsForImpacts = {}
for systematic in listOfSystematics:
    LimitOptionsForImpacts["freeze_%s" % systematic] = "--freezeParameters %s" % systematic
LimitOptionsForImpacts["freeze_autoMCStats"] = "--freezeNuisanceGroups autoMCStats"

jobsDir                = 'jobsLimits_' + args.tag
outScriptNameBareProto = 'job_{0}.sh'
outScriptNameProto     = (jobsDir + '/' + outScriptNameBareProto)
outFileNameProto       = 'Limit_{0}_{1}_{2}.root'
outFileDatacardProto   = 'datacard_{0}_{1}.txt'
outputFileName         = 'higgsCombineTest.AsymptoticLimits.mH120.root'
plotFileFolderProto    = outputDir + '/' + args.tag + '/HistogramFiles_{0}/'
LimitOptions           = { "statOnly" : "--freezeParameters allConstrainedNuisances", "syst" : "" }
folderYearName         = "DatacardFolder_{0}"
folderRunIIName        = "DatacardFolder_RunII"
outPlotFileNameProto   = "outPlotter_{0}_{1}.root"

allLimitOptions = copy.deepcopy(LimitOptions)
if args.impacts: allLimitOptions.update(LimitOptionsForImpacts)
cmssw_base    = os.environ['CMSSW_BASE']
cmssw_version = os.environ['CMSSW_VERSION']
scram_arch    = os.environ['SCRAM_ARCH']

tarName      = 'LimitCalculator.tar.gz' #%s_tar.tgz' % cmssw_version
limitWorkDir = os.getcwd()
tarLFN       = limitWorkDir + '/' + tarName


### NOTE: I must be in bbbb
to_include = [
    'prepareModels/'
]

command = 'tar -zcf {0} '.format(tarLFN)
for ti in to_include:
    command += ti + ' '

print '** INFO: Going to tar executable folder into', tarName
if os.system(command) != 0:
    print "... Not able to execute command \"", command, "\", exit"
    sys.exit()
    print '** INFO: tar finished and saved in:', tarLFN
else:
    print '** INFO: Not going to tar executable folder, using', tarLFN


if os.path.isdir(jobsDir):
    print "... working folder", jobsDir, " already exists, exit"
    sys.exit()    

cmd='mkdir -p ' + jobsDir
if os.system(cmd) != 0:
    print "... Not able to execute command \"", cmd, "\", exit"
    sys.exit()

##############################
#### Ship the tarball and submit the jobs
tarEOSdestLFN         = outputDir + '/' + args.tag + '/combine_tar/' + tarName
# tarEOSdestLFN.replace('root://cmseos.fnal.gov/', '/eos/uscms')

print "** INFO: copying executables tarball to:", tarEOSdestLFN
command = 'xrdcp -f -s %s %s' % (tarLFN, tarEOSdestLFN)
if os.system(command) != 0:
    print "... Not able to execute command \"", command, "\", exit"
    sys.exit()


for year in yearList:
    configfilename  = "prepareModels/config/LimitsConfig_%s.cfg" % year
    with open(configfilename) as templateConfiguration:
        signalConfiguration = templateConfiguration.read()
    cfgparser = ConfigParser()
    cfgparser.readfp(StringIO(signalConfiguration))
    directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
    directory = directory + "/*"
    # tarEOSdestLFN.replace('root://cmseos.fnal.gov/', '/eos/uscms')
    command = 'eos cp -r %s %s' % (directory, plotFileFolderProto.format(year))
    if os.system(command) != 0:
        print "... Not able to execute command \"", command, "\", exit"
        sys.exit()
    
for signalRaw in open("prepareModels/listOfSamples.txt", 'rb').readlines():
    signal = signalRaw[:-1]
    outScriptName  = outScriptNameProto.format(signal)
    outScript      = open(outScriptName, 'w')
    writeln(outScript, '#!/bin/bash')
    writeln(outScript, '{') ## start of redirection..., keep stderr and stdout in a single file, it's easier
    writeln(outScript, 'echo "... starting job on " `date` #Date/time of start of job')
    writeln(outScript, 'echo "... running on: `uname -a`" #Condor job is running on this node')
    writeln(outScript, 'echo "... system software: `cat /etc/redhat-release`" #Operating System on that node')
    writeln(outScript, 'source /cvmfs/cms.cern.ch/cmsset_default.sh')
    writeln(outScript, 'export SCRAM_ARCH=%s' % scram_arch)
    writeln(outScript, 'eval `scramv1 project CMSSW %s`' % cmssw_version)
    writeln(outScript, 'cd %s/src' % cmssw_version)
    writeln(outScript, 'eval `scramv1 runtime -sh`')
    writeln(outScript, 'git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit')
    writeln(outScript, 'cd HiggsAnalysis/CombinedLimit')
    writeln(outScript, 'git fetch origin')
    writeln(outScript, 'git checkout v8.1.0')
    writeln(outScript, 'scramv1 b clean; scramv1 b')
    writeln(outScript, 'echo "... retrieving bbbb executables tarball"')
    writeln(outScript, 'xrdcp -f -s %s .' % tarEOSdestLFN) ## force overwrite CMSSW tar
    writeln(outScript, 'echo "... uncompressing bbbb executables tarball"')
    writeln(outScript, 'tar -xzf %s' % tarName)
    writeln(outScript, 'rm %s' % tarName)
    writeln(outScript, 'echo "... retrieving filelist"')
    writeln(outScript, 'export CPP_BOOST_PATH=/cvmfs/sft.cern.ch/lcg/views/LCG_89/x86_64-slc6-gcc62-opt')
    writeln(outScript, 'export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./lib:${CPP_BOOST_PATH}/lib')
    
    for year in yearList:
        writeln(outScript, 'echo "... preparing histos"')
        folderName = folderYearName.format(year)
        writeln(outScript, 'mkdir %s' % folderName)
        # print 'python prepareModels/prepareHistos.py              --config prepareModels/config/LimitsConfig_%s.cfg --signal %s --directory %s --folder %s'%(year,signal,plotFileFolderProto.format(year),folderName)
        writeln(outScript, 'python prepareModels/prepareHistos.py              --config prepareModels/config/LimitsConfig_%s.cfg --signal %s --directory %s --folder %s'%(year,signal,plotFileFolderProto.format(year),folderName))
        writeln(outScript, 'echo "... preparing datacard"')
        writeln(outScript, 'python prepareModels/makeDatacardsAndWorkspaces.py --config prepareModels/config/LimitsConfig_%s.cfg  --no-comb --signal  %s --folder %s'%(year,signal,folderName))
        writeln(outScript, 'while [[ "$(ps aux | grep text2workspace.py | wc -l)" -gt "1" ]]; do echo "text2workspace.py still running..."; sleep 5; done')
        writeln(outScript, 'sleep 15')
        datacardName = folderName + "/datacard" + str(year) + "_selectionbJets_SignalRegion.txt"
        outputDatacardFile  = plotFileFolderProto.format(year) + outFileDatacardProto.format(year,signal)
        writeln(outScript, 'xrdcp -s -f %s %s' % (datacardName, outputDatacardFile)) ## no force overwrite output in destination
        plotFileName       = outPlotFileNameProto.format(year,signal)
        inputPlotFileName  = folderName + "/" + plotFileName
        outputPlotFileName = plotFileFolderProto.format(year) + "/" + plotFileName
        writeln(outScript, 'xrdcp -s -f %s %s' % (inputPlotFileName, outputPlotFileName)) ## no force overwrite output in destination

    if args.year == "RunII":
        writeln(outScript, 'mkdir %s' % folderRunIIName)
        writeln(outScript, 'combineCards.py c2016=DatacardFolder_2016/datacard2016_selectionbJets_SignalRegion.txt c2017=DatacardFolder_2017/datacard2017_selectionbJets_SignalRegion.txt c2018=DatacardFolder_2018/datacard2018_selectionbJets_SignalRegion.txt > %s/datacardRunII_selectionbJets_SignalRegion.txt' % folderRunIIName)
        outputDatacardFile  = plotFileFolderProto.format("RunII") + outFileDatacardProto.format("RunII",signal)
        writeln(outScript, 'xrdcp -s -f %s %s' % (datacardName, outputDatacardFile)) ## no force overwrite output in destination
        writeln(outScript, 'text2workspace.py %s/datacardRunII_selectionbJets_SignalRegion.txt' % folderRunIIName)
        writeln(outScript, 'sleep 15')
    
    for year in yearList:
        workspaceName = folderYearName.format(year) + "/datacard" + str(year) + "_selectionbJets_SignalRegion.root"
        for option, combineCommand in allLimitOptions.items(): 
            writeln(outScript, 'echo "... running %s %s datacard"' % (year,option))
            writeln(outScript, 'combine %s -M AsymptoticLimits --run blind --X-rtd  MINIMIZER_analytic --X-rtd  FAST_VERTICAL_MORPH %s' % (workspaceName,combineCommand))
            writeln(outScript, 'echo "... execution finished with status $?"')
            outputLimitFile  = plotFileFolderProto.format(year) + outFileNameProto.format(year,signal,option)
            writeln(outScript, 'echo "... copying output file %s to EOS in %s"' % (outputFileName, outputLimitFile))
            writeln(outScript, 'xrdcp -s -f %s %s' % (outputFileName, outputLimitFile)) ## no force overwrite output in destination
            writeln(outScript, 'echo "... copy done with status $?"')
    
    if args.year == "RunII":
        workspaceName = folderRunIIName + "/datacardRunII_selectionbJets_SignalRegion.root"
        for option, combineCommand in LimitOptions.items(): 
            writeln(outScript, 'echo "... running RunII %s datacard"' % option)
            writeln(outScript, 'combine %s -M AsymptoticLimits --run blind --X-rtd  MINIMIZER_analytic --X-rtd  FAST_VERTICAL_MORPH %s' % (workspaceName,combineCommand))
            writeln(outScript, 'echo "... execution finished with status $?"')
            outputLimitFile  = plotFileFolderProto.format("RunII") + outFileNameProto.format("RunII",signal,option)
            writeln(outScript, 'echo "... copying output file %s to EOS in %s"' % (outputFileName, outputLimitFile))
            writeln(outScript, 'xrdcp -s -f %s %s' % (outputFileName, outputLimitFile)) ## no force overwrite output in destination
            writeln(outScript, 'echo "... copy done with status $?"')
    
    writeln(outScript, 'cd ${_CONDOR_SCRATCH_DIR}')
    writeln(outScript, 'rm -rf %s' % cmssw_version)
    writeln(outScript, 'echo "... job finished with status $?"')
    writeln(outScript, 'echo "... finished job on " `date`')
    writeln(outScript, 'echo "... exiting script"')
    writeln(outScript, '} 2>&1') ## end of redirection
    outScript.close()



## set directory to job directory, so that logs will be saved there
os.chdir(jobsDir)
for signalRaw in open("../prepareModels/listOfSamples.txt", 'rb').readlines():
    signal = signalRaw[:-1]
    command = "%s job_%s.sh" % (t3SubmitScript,signal)
    if os.system(command) != 0:
        print "... Not able to execute command \"", command, "\", exit"
        sys.exit()


