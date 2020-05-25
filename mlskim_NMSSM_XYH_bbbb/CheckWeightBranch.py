import os
import argparse
import ROOT
import modules.Constants as const
import sys
import subprocess


#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--eosPath', dest='eosPath',  help='Eos path'   ,   required = True)
parser.add_argument('--weight' , dest='weight' ,  help='weight name',   required = True)
args = parser.parse_args()
eosPath = args.eosPath
weight  = args.weight

out = subprocess.Popen(['eos', const.eosPath, 'ls', eosPath], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
stdoutFirst,stderr = out.communicate()
fileList = []
for signalFolderName in stdoutFirst.split():
	if "SKIM_" in signalFolderName:
		signalOutputPath = eosPath + "/" + signalFolderName + "/output/"
		out = subprocess.Popen(['eos', const.eosPath, 'ls', signalOutputPath], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdoutSecond,stderr = out.communicate()
		for signalFileName in stdoutSecond.split():
			fileList.append(const.eosPath + "/" + signalOutputPath + signalFileName)
	
for fileName in fileList:
    if ".root" in fileName:
		inputFile = ROOT.TFile.Open(fileName)
		branchFound = False
		for branch in inputFile.bbbbTree.GetListOfBranches():
			if( branch.GetName() == weight ):
				branchFound = True
		if not branchFound:
			print "missing branch ", weight, " for ", fileName
