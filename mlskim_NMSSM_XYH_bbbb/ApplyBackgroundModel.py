import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
import subprocess
import ast
from  ConfigParser import *
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector
import cPickle as pickle
from modules.ConfigurationReader import ConfigurationReader
import modules.Constants as const
from modules.ReweightModelAndTransferFactor import ReweightModelAndTransferFactor
# from ROOT import TFile, TTree, TBranch
import ROOT
import threading

def CreatePredictionModel(reweightermodel,transferfactor,normalization,dataset_3bTag, backgroundWeightName):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	original_weights = numpy.ones(dtype='float64',shape=len(dataset_3bTag))
	original_weights = numpy.multiply(original_weights,transferfactor)

	folding_weights= data.getmodelweights(dataset_3bTag,original_weights,reweightermodel,transferfactor,normalization)
    
	dataset_3bTag[backgroundWeightName] = folding_weights
	return dataset_3bTag[[backgroundWeightName]]


def getWeightsForBackground(dataset, theReweightModelAndTransferFactor, backgroundWeightName):
	
	reweightermodel      = theReweightModelAndTransferFactor.reweightMethod 
	transferfactor       = theReweightModelAndTransferFactor.transferFactor
	normalization        = theReweightModelAndTransferFactor.normalization

	#Get weights for the dataset
	weights = CreatePredictionModel(reweightermodel, transferfactor, normalization, dataset, backgroundWeightName)
	
	del dataset
	return weights 


def updateFile(fileName, theBackgroudWeights):
    data.pandas2root(theBackgroudWeights,'bbbbTree', fileName, mode='a')


def ApplyBDTweightsToFileList(fileList, treeName, trainingVariables, theReweightModelAndTransferFactor, backgroundWeightName):
	print "Staring with list of ", len(fileList), " files"
	for theRootFileName in fileList:
		theRootFile = ROOT.TFile.Open(theRootFileName)
		theTree = theRootFile.Get(const.treeName)
		branchAlreadyExists = False
		for branch in theTree.GetListOfBranches():
			if branch.GetName() == backgroundWeightName :
				print "Error: branch ", backgroundWeightName, " already exists!!! skypping..."
				branchAlreadyExists = True
		theRootFile.Close()
		if branchAlreadyExists: continue
		singleListFile = [theRootFileName]
		theDataFile = data.root2pandas(singleListFile, treeName, branches=trainingVariables)
		theBackgroudWeights = getWeightsForBackground(theDataFile, theReweightModelAndTransferFactor, backgroundWeightName)
		updateFile(singleListFile[0], theBackgroudWeights)
	print "Done with list of ", len(fileList), " files"


#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--dir'       , dest='bdtModelDir',  help='Name of config file',   required = True)
parser.add_argument('--signals'   , dest='signalPath' ,  help='Name of config file',   required = False, default = "None")
parser.add_argument('--singleFile', dest='singleFile' ,  help='Name of config file',   required = False, default = "None")
args = parser.parse_args()
bdtModelDir = args.bdtModelDir
signalPath  = args.signalPath
singleFile  = args.singleFile

configFileName = bdtModelDir + "/" + const.outputConfigFileName
configFile = ConfigurationReader(configFileName)

backgroundWeightName        = const.weightBranchPrefix + configFile.backgroundWeightName   
skimFolder                  = configFile.skimFolder
trainingVariables           = configFile.trainingVariables   
threadNumber                = configFile.threadNumber

modelFileName = bdtModelDir + "/" + const.modelFileName

if singleFile == "None":
	if signalPath == "None":
		out = subprocess.Popen(['eos', const.eosPath, 'ls', skimFolder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdout,stderr = out.communicate()
		fileList = stdout.split()
		for fileNameIt in range(0,len(fileList)):
			fileList[fileNameIt] = const.eosPath + "/" + skimFolder + "/" + fileList[fileNameIt]

	else:
		out = subprocess.Popen(['eos', const.eosPath, 'ls', signalPath], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdoutFirst,stderr = out.communicate()
		fileList = []
		for signalFolderName in stdoutFirst.split():
			if "SKIM_NMSSM_XYH_bbbb" in signalFolderName:
				signalOutputPath = signalPath + "/" + signalFolderName + "/output/"
				out = subprocess.Popen(['eos', const.eosPath, 'ls', signalOutputPath], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
				stdoutSecond,stderr = out.communicate()
				for signalFileName in stdoutSecond.split():
					fileList.append(const.eosPath + "/" + signalOutputPath + signalFileName)
else:
	fileList = [singleFile] 


# loading formula
with open(modelFileName) as reweighterInputFile:
	theReweightModelAndTransferFactor = pickle.load(reweighterInputFile)

skimFileListOfList = [[] for i in range(threadNumber)]

listNumber = 0
for fileName in fileList:
    if ".root" in fileName:
		skimFileListOfList[listNumber].append(fileName)
		listNumber = listNumber + 1
		if listNumber >= threadNumber: 
			listNumber = 0

threads = list()
for index in range(threadNumber):
	x = threading.Thread(target=ApplyBDTweightsToFileList, args=(skimFileListOfList[index], const.treeName, trainingVariables, theReweightModelAndTransferFactor, backgroundWeightName))
	threads.append(x)
	x.start()

for thread in threads:
	thread.join()
	