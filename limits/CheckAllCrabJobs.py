#!/usr/bin/python

import subprocess
import os
import sys
import argparse
import time
# import re

crabFolders =[]

year = sys.argv[1]

for arg in sys.argv[2:]:
    crabFolders.append(arg)

while len(crabFolders) > 0:
    for folder in crabFolders:
        if folder == "NULL": continue
        statusCommand = "crab status --dir "+ folder
        print folder
        status  = os.popen(statusCommand).readlines()
        totalNumber    = 99999
        failedNumber   = 0
        finishedNumber = 0
        runningNumber  = 0
        for line in status:
            if line.find('Jobs status:') != -1 and line.find('% (') and line.find(')') != -1:
                totalNumberStr = line[line.find('/')+1:line.find(')')]
                totalNumberStr = filter(lambda x: x.isdigit(), totalNumberStr)
                totalNumber = int(totalNumberStr)
            if line.find('failed') != -1 and line.find('% (') != -1 and line.find(')') != -1:
                failedNumberStr = line[line.find('% (')+3:line.find('/')]
                failedNumberStr = filter(lambda x: x.isdigit(), failedNumberStr)
                sys.stdout.flush()
                failedNumber = int(failedNumberStr)
            if line.find('finished') != -1 and line.find('% (') and line.find(')') != -1:
                finishedNumberStr = line[line.find('% (')+3:line.find('/')]
                finishedNumberStr = filter(lambda x: x.isdigit(), finishedNumberStr)
                finishedNumber = int(finishedNumberStr)
            if line.find('running') != -1 and line.find('% (') and line.find(')') != -1:
                runningNumberStr = line[line.find('% (')+3:line.find('/')]
                runningNumberStr = filter(lambda x: x.isdigit(), runningNumberStr)
                runningNumber = int(runningNumberStr)
            if line.find('Publication status:') != -1:
                break
        failedOverRemaining = 0
        if totalNumber-finishedNumber>0 : failedOverRemaining = float(failedNumber)/float(totalNumber-finishedNumber)
        finishedOverTotal  = float(finishedNumber)/float(totalNumber)
        # print "Total number = ", totalNumber
        # print "Running  = ", runningNumber, " (", float(runningNumber)/float(totalNumber)*100.,"%)"
        # print "Failed  = ", failedNumber, " (", float(failedNumber)/float(totalNumber)*100.,"%)"
        # print "Finished  = ", finishedNumber, " (", float(finishedNumber)/float(totalNumber)*100.,"%)"
        if failedNumber >0:
            print "Failed, resubmit command:"
            resubmitCommand = "crab resubmit --dir " + folder
            print resubmitCommand
            os.system(resubmitCommand)
        if totalNumber    == 99999:
            print "Folder not found"
            print "submitting from scratch"
            currentFolder = os.getcwd()
            os.chdir(folder + "/..")
            crabPurgeCommans = 'crab purge --dir=crab_combine'
            removeCrabFolderCommand = "rm -r crab_combine"
            os.system(removeCrabFolderCommand)
            combineArguments = "combineTool.py -M AsymptoticLimits  --run blind  --job-mode crab3 --custom-crab ../../prepareModels/custom_crab.py --task-name combine -d datacard" + str(year) + "_selectionbJets_SignalRegion.root"
            os.system(combineArguments)
            os.chdir(currentFolder)

        if finishedOverTotal == 1.:
            print "All jobs from folder ", folder, " were finished, removing from the checking list"
            currentFolder = os.getcwd()
            os.chdir(folder + "/..")
            for x in range(0, 10):
                retrieveCommand = "crab getoutput -d crab_combine"
                os.system(retrieveCommand)
                if os.path.exists("crab_combine/results/combine_output_1.tar") : break
                time.sleep(2)
            unTarCommand = "tar xf crab_combine/results/combine_output_1.tar"
            os.system(unTarCommand)
            os.chdir(currentFolder)
            crabFolders[crabFolders.index(folder)] = "NULL"
            continue
        # if finishedNumber>0 and failedOverRemaining>0.1:
        # # if failedOverRemaining>0.1:
        #     print "resubmitting jobs for folder ", folder, ":"
        #     # statusCommand = "crab resubmit --maxmemory=10000 --dir "+ folder
        #     statusCommand = "crab resubmit --dir "+ folder
        #     os.system(statusCommand)
    if crabFolders.count("NULL") == len(crabFolders) : break
    print "Sleaping 3 min before next check"
    print
    print
    time.sleep(180) #sleep for 3 min


print "All jobs completed!"
