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
		for branch in theTree.GetListOfBranches():
			if branch.GetName() == backgroundWeightName :
				print "Error: branch ", backgroundWeightName, " already exists!!! Aborting..."
				sys.exit()
		theRootFile.Close()
		singleListFile = [theRootFileName]
		theDataFile = data.root2pandas(singleListFile, treeName, branches=trainingVariables)
		theBackgroudWeights = getWeightsForBackground(theDataFile, theReweightModelAndTransferFactor, backgroundWeightName)
		updateFile(singleListFile[0], theBackgroudWeights)
	print "Done with list of ", len(fileList), " files"


#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--dir', dest='bdtModelDir',  help='Name of config file',   required = True)
args = parser.parse_args()
bdtModelDir = args.bdtModelDir

configFileName = bdtModelDir + "/" + const.outputConfigFileName
configFile = ConfigurationReader(configFileName)

backgroundWeightName        = const.weightBranchPrefix + configFile.backgroundWeightName   
skimFolder                  = configFile.skimFolder
trainingVariables           = configFile.trainingVariables   
threadNumber                = configFile.threadNumber

modelFileName = bdtModelDir + "/" + const.modelFileName

out = subprocess.Popen(['eos', const.eosPath, 'ls', skimFolder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
stdout,stderr = out.communicate()

# loading formula
with open(modelFileName) as reweighterInputFile:
	theReweightModelAndTransferFactor = pickle.load(reweighterInputFile)

skimFileListOfList = [[] for i in range(threadNumber)]

listNumber = 0
for fileName in stdout.split():
    if ".root" in fileName:
		skimFileListOfList[listNumber].append(const.eosPath + "/" + skimFolder + "/" + fileName)
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
	