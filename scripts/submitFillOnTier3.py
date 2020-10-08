import os
import sys
import argparse
import getpass
import subprocess

sampleFileName    = "sampleCfg.cfg"
selectionFileName = "selectionCfg.cfg"
configFileName    = "config.cfg"
listOfSampleTypes = ["data", "datadriven", "backgrounds", "signals"]
outputHistogramFolder = "DataPlots"
    
def printPair(thePair):
    theString = thePair[0] + " ="
    for theValue in thePair[1] : theString = theString + " " + theValue + ","
    return theString[:-1] + "\n"

def CreateNewConfigFile(inputCfgName, outputCfgName, signalPair, mergePair = ("", [""])):
    outputCfg = open(outputCfgName, "w")
    firstSample = True
    with open(inputCfgName) as inputFile:
        for line in inputFile.readlines():
            if "#" in line[0]: continue
            isSampleType = False
            for sampleType in listOfSampleTypes:
                if sampleType in line:
                    isSampleType = True
                    if firstSample:
                        firstSample = False
                        outputCfg.write(printPair(signalPair))
            if isSampleType: continue
            if "numberOfThreads" in line:
                outputCfg.write("numberOfThreads = 1\n")
                continue
            if "sampleCfg" in line:
                outputCfg.write("sampleCfg = " + sampleFileName + "\n")
                continue
            if "cutCfg" in line:
                outputCfg.write("cutCfg    = " + selectionFileName + "\n")
                continue
            if "outputFolder" in line:
                outputCfg.write("outputFolder = " + outputHistogramFolder + "\n")
                continue
            outputCfg.write(line)
            if "[merge]" in line: break
        inputFile.close()
    if mergePair[0] != "": outputCfg.write(printPair(mergePair))


def getMergedSampleList(inputCfgName):
    listOfMergedSamples = []
    isInMergedSession = False
    with open(inputCfgName) as inputFile:
        for line in inputFile.readlines():
            if "[merge]" in line:
                isInMergedSession = True
                continue
            if not isInMergedSession: continue
            key, value = line.rstrip("\n").split("=")
            key = ''.join(key.split())
            listOfSamples = value.split(",")
            theNewListOfSamples = []
            for sample in listOfSamples:
                sample = ''.join(sample.split())
                theNewListOfSamples.append(sample)
            listOfMergedSamples.append((key, theNewListOfSamples))
        inputFile.close()
    return listOfMergedSamples

def getValue(inputCfgName, keyName):
    with open(inputCfgName) as inputFile:
        for line in inputFile.readlines():
            if "=" not in line: continue
            if "#" in line: continue
            key, value = line.rstrip("\n").split("=")
            key = ''.join(key.split())
            if keyName in key:
                return value


def getSampleList(inputCfgName):
    listOfMergedSamples = getMergedSampleList(inputCfgName)
    typeOfMergedSample = {}
    for mergedSample in listOfMergedSamples:
        typeOfMergedSample[mergedSample[0]] = ""
    listOfSamples = []

    with open(inputCfgName) as inputFile:
        for line in inputFile.readlines():
            if "=" not in line: continue
            if "#" in line: continue
            key, value = line.rstrip("\n").split("=")
            key = ''.join(key.split())
            if key not in listOfSampleTypes: continue
            currentistOfSamples = value.split(",")
            for sample in currentistOfSamples:
                sample = ''.join(sample.split())
                isAmongMerged = False
                for mergedSample in listOfMergedSamples:
                    if sample in mergedSample[1]:
                        isAmongMerged = True
                        if typeOfMergedSample[mergedSample[0]] == "": typeOfMergedSample[mergedSample[0]] = key
                        if typeOfMergedSample[mergedSample[0]] != key:
                            print "Merged samples are of different type!!!"
                            exit(1)
                        break
                if isAmongMerged: continue
                listOfSamples.append( (key, [sample], "", [""]) )
                # print listOfSamples[-1][0], listOfSamples[-1][1], listOfSamples[-1][2], listOfSamples[-1][3]
        inputFile.close()
    
    for mergedSample in listOfMergedSamples:
        listOfSamples.append((typeOfMergedSample[mergedSample[0]], mergedSample[1], mergedSample[0], mergedSample[1]))
        # print listOfSamples[-1][0], listOfSamples[-1][1], listOfSamples[-1][2], listOfSamples[-1][3]

    return listOfSamples

def writeln(f, line):
    f.write(line + '\n')

###########

outputFolderBase = "root://cmseos.fnal.gov//store/user/{0}/bbbb_histograms/"

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--cfg'       ,  dest = 'cfg'       ,  help = 'config file filelist'     ,  required = True        )
parser.add_argument('--tag'       ,  dest = 'tag'       ,  help = 'production tag'           ,  required = True        )

args = parser.parse_args()

executable = "bin/fill_histograms.exe"

username = getpass.getuser()
print "... Welcome", username

outputFolder = outputFolderBase.format(username) + "/" + args.tag

jobsDir                = 'jobsFill_' + args.tag
outCfgNameBareProto    = 'config_{0}.txt'
outScriptNameBareProto = 'job_{0}.sh'
outListNameProto       = (jobsDir + '/' + outCfgNameBareProto)
outScriptNameProto     = (jobsDir + '/' + outScriptNameBareProto)
EOSconfigFolderProto   = outputFolder + '/configFile/'
EOSconfigProto         = (EOSconfigFolderProto + outCfgNameBareProto)
outFileNameProto       = (outputFolder + '/bbbbHistograms_{0}.root')

cmssw_base    = os.environ['CMSSW_BASE']
cmssw_version = os.environ['CMSSW_VERSION']
scram_arch    = os.environ['SCRAM_ARCH']


tarName      = 'bbbbHistograms.tar.gz' #%s_tar.tgz' % cmssw_version
bbbbWorkDir  = os.getcwd()
tarLFN       = bbbbWorkDir + '/tars/' + tarName

tarEOSdestLFN         = outputFolder + '/analysis_tar/' + tarName


if os.path.isdir(jobsDir):
    print "... working folder", jobsDir, " already exists, exit"
    sys.exit()

cmd='mkdir -p ' + jobsDir
if os.system(cmd) != 0:
    print "... Not able to execute command \"", cmd, "\", exit"
    sys.exit()

theListOfSamples = getSampleList(args.cfg)

for sample in theListOfSamples:
    sampleName = sample[1][0]
    if sample[2] != "":  sampleName = sample[2]
    CreateNewConfigFile(args.cfg, outListNameProto.format(sampleName), (sample[0],sample[1]), (sample[2],sample[3]) )
    command = 'xrdcp -f -s %s %s' % (outListNameProto.format(sampleName), EOSconfigProto.format(sampleName))
    if os.system(command) != 0:
        print "... Not able to execute command \"", command, "\", exit"
        sys.exit()

### NOTE: I must be in bbbb
to_include = [
    'bin/',
    'lib/',
    'plotterListFiles/'
]

command = 'tar -zcf {0} '.format(tarLFN)
for ti in to_include:
    command += ti + ' '

print '** INFO: Going to tar executable folder into', tarName
if os.system(command) != 0:
    print "... Not able to execute command \"", command, "\", exit"
    sys.exit()
print '** INFO: tar finished and saved in:', tarLFN

command = 'xrdcp -f -s %s %s' % (tarLFN, tarEOSdestLFN)
if os.system(command) != 0:
    print "... Not able to execute command \"", command, "\", exit"
    sys.exit()

command = 'xrdcp -f -s %s %s' % (getValue(args.cfg, "sampleCfg"), (EOSconfigFolderProto + "/" + sampleFileName))
if os.system(command) != 0:
    print "... Not able to execute command \"", command, "\", exit"
    sys.exit()

command = 'xrdcp -f -s %s %s' % (getValue(args.cfg, "cutCfg"), (EOSconfigFolderProto + "/" + selectionFileName))
if os.system(command) != 0:
    print "... Not able to execute command \"", command, "\", exit"
    sys.exit()
        

for sample in theListOfSamples:
    sampleName = sample[1][0]
    if sample[2] != "":  sampleName = sample[2]
    outScriptName  = outScriptNameProto.format(sampleName)
    this_full_command = executable + ' ' + configFileName
    outScript      = open(outScriptName, 'w')
    writeln(outScript, '#!/bin/bash')
    writeln(outScript, '{') ## start of redirection..., keep stderr and stdout in a single file, it's easier
    writeln(outScript, 'echo "... starting job on " `date` #Date/time of start of job')
    writeln(outScript, 'echo "... running on: `uname -a`" #Condor job is running on this node')
    writeln(outScript, 'echo "... system software: `cat /etc/redhat-release`" #Operating System on that node')
    writeln(outScript, 'source /cvmfs/cms.cern.ch/cmsset_default.sh')
    writeln(outScript, 'export SCRAM_ARCH=%s' % scram_arch)
    # writeln(outScript, 'cd %s/src/' % cmssw_version)
    writeln(outScript, 'eval `scramv1 project CMSSW %s`' % cmssw_version)
    writeln(outScript, 'cd %s/src' % cmssw_version)
    writeln(outScript, 'eval `scramv1 runtime -sh`')
    writeln(outScript, 'echo "... retrieving bbbb executables tarball"')
    writeln(outScript, 'xrdcp -f -s %s .' % tarEOSdestLFN) ## force overwrite CMSSW tar
    writeln(outScript, 'echo "... uncompressing bbbb executables tarball"')
    writeln(outScript, 'tar -xzf %s' % tarName)
    writeln(outScript, 'rm %s' % tarName)
    writeln(outScript, 'echo "... retrieving filelist"')
    writeln(outScript, 'xrdcp -f -s %s %s' % (EOSconfigProto.format(sampleName), configFileName)) ## force overwrite file list
    writeln(outScript, 'xrdcp -f -s %s .' % ((EOSconfigFolderProto + "/" + sampleFileName   ))) ## force overwrite file list
    writeln(outScript, 'xrdcp -f -s %s .' % ((EOSconfigFolderProto + "/" + selectionFileName))) ## force overwrite file list
    writeln(outScript, 'export CPP_BOOST_PATH=/cvmfs/sft.cern.ch/lcg/views/LCG_89/x86_64-slc6-gcc62-opt')
    writeln(outScript, 'export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./lib:${CPP_BOOST_PATH}/lib')
    writeln(outScript, 'echo "... executing the command:"')
    writeln(outScript, 'echo "%s"' % this_full_command)
    writeln(outScript, 'echo "... starting execution"')
    writeln(outScript, '%s' % this_full_command)
    writeln(outScript, 'echo "... execution finished with status $?"')
    outputFileName = outputHistogramFolder + "/outPlotter.root"
    outputEOSName  = outFileNameProto.format(sampleName)
    writeln(outScript, 'echo "... copying output file %s to EOS in %s"' % (outputFileName, outputEOSName))
    writeln(outScript, 'xrdcp -s -f %s %s' % (outputFileName, outputEOSName)) ## no force overwrite output in destination
    writeln(outScript, 'echo "... copy done with status $?"')
    writeln(outScript, 'cd ${_CONDOR_SCRATCH_DIR}')
    writeln(outScript, 'rm -rf %s' % cmssw_version)
    writeln(outScript, 'echo "... job finished with status $?"')
    writeln(outScript, 'echo "... finished job on " `date`')
    writeln(outScript, 'echo "... exiting script"')
    writeln(outScript, '} 2>&1') ## end of redirection
    outScript.close()

for sample in theListOfSamples:
    sampleName = sample[1][0]
    if sample[2] != "":  sampleName = sample[2]
    command = "%s/scripts/t3submit %s" % (bbbbWorkDir, outScriptNameProto.format(sampleName))
    if os.system(command) != 0:
        print "... Not able to execute command \"", command, "\", exit"
        sys.exit()
 