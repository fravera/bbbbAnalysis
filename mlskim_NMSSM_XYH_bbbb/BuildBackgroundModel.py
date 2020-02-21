import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
import subprocess
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import *
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector
import cPickle as pickle
from modules.ConfigurationReader import ConfigurationReader
import modules.Constants as const
from modules.ReweightModelAndTransferFactors import ReweightModelAndTransferFactors


def BuildReweightingModel(data_4b, data_3b, trainingVariables, modelArguments, modelFileNameFullPath):
	print "bTagDataSize     = ", len(data_4b)
	print "AntibTagDataSize = ", len(data_3b)
	print "[INFO] Processing predicted model"
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	originalcr, targetcr, originalcr_weights, targetcr_weights, transferfactor = data.preparedataformodel(data_3b,data_4b,trainingVariables)
	print "transfer factor = ", transferfactor

	#######################################
	##Folding Gradient Boosted Reweighter
	#######################################
	foldingcr_weights,reweightermodel,renormtransferfactor = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelArguments)  
	########################################
	##GB ROC AUC
	########################################
	bdtreweighter.roc_auc_measurement(originalcr,targetcr,originalcr_weights,foldingcr_weights)
	########################################
	##Update 3b dataframe for modeling
	########################################
	theReweightModelAndTransferFactors = ReweightModelAndTransferFactors(reweightermodel,transferfactor,renormtransferfactor)
	with open(modelFileNameFullPath, 'w') as reweighterOutputFile:
		pickle.dump(theReweightModelAndTransferFactors, reweighterOutputFile)

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file',   required = True)
args = parser.parse_args()
configFileName = args.cfgfile
configFile = ConfigurationReader(configFileName)

backgroundWeightName        = configFile.backgroundWeightName   
modelArguments              = configFile.modelArguments    
minpt                       = configFile.minpt       
minRegressedPt              = configFile.minRegressedPt       
minEta                      = configFile.minEta       
maxEta                      = configFile.maxEta       
preSelection                = configFile.preSelection       
controlRegionSelection      = configFile.controlRegionSelection       
skimFolder                  = configFile.skimFolder
variables                   = configFile.variables   
trainingVariables           = configFile.trainingVariables
bTagSelection               = configFile.bTagSelection
antiBTagSelection           = configFile.antiBTagSelection

enabledBranches= list(set(variables) | set(trainingVariables) | set(const.minVariableList))
skimFolderList = skimFolder.split('/')


# create output directory
outputDirectory = const.outputDirPrefix + skimFolderList[skimFolderList.index("bbbb_ntuples")+1] + '_' + backgroundWeightName

if os.path.isdir(outputDirectory):
	print "... working folder", outputDirectory, " already exists, exit"
	sys.exit()

cmd='mkdir ' + outputDirectory
if os.system(cmd) != 0:
	print "... Not able to execute command \"", cmd, "\", exit"
	sys.exit()

orig_stdout = sys.stdout
logFile = open(outputDirectory + '/' + const.logFileName, 'w+')
sys.stdout = logFile

outputConfigFileNameFullPath = outputDirectory + '/' + const.outputConfigFileName
modelFileNameFullPath        = outputDirectory + '/' + const.modelFileName

# Copy confid files into the output directory
cmd='cp ' + configFileName + ' ' + outputConfigFileNameFullPath
if os.system(cmd) != 0:
	print "... Not able to execute command \"", cmd, "\", exit"
	sys.exit()

# Read samples directly from skims
# data_4b_and_3b = []

# for i in range(len(skimFolder_4btag_and_3btag)):
# 	out = subprocess.Popen(['eos', const.eosPath, 'ls', skimFolder_4btag_and_3btag[i]], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
# 	stdout,stderr = out.communicate()
# 	skimFileList = []
# 	for fileName in stdout.split():
# 		if ".root" in fileName:
# 			skimFileList.append(const.eosPath + "/" + skimFolder_4btag_and_3btag[i] + "/" + fileName)
# 	data_4b_and_3b.append( data.root2pandas(skimFileList, const.treeName, branches=enabledBranches) )

# type = ["4 btag", "3 btag"]
# # skim events
# for i in range(len(data_4b_and_3b)):
# 	print "   -Number of events in dataset ", type[i], " (before) = ",len(data_4b_and_3b[i]) 
# 	data_4b_and_3b[i]          = data_4b_and_3b[i][ (data_4b_and_3b[i].H1_b1_pt > minpt) & (data_4b_and_3b[i].H1_b2_pt > minpt) & (data_4b_and_3b[i].H2_b1_pt > minpt) & (data_4b_and_3b[i].H2_b2_pt > minpt)] 
# 	data_4b_and_3b[i]          = data_4b_and_3b[i][ (data_4b_and_3b[i].H1_b1_ptRegressed > minRegressedPt) & (data_4b_and_3b[i].H1_b2_ptRegressed > minRegressedPt) & (data_4b_and_3b[i].H2_b1_ptRegressed > minRegressedPt) & (data_4b_and_3b[i].H2_b2_ptRegressed > minRegressedPt)] 
# 	data_4b_and_3b[i]          = data_4b_and_3b[i][ (data_4b_and_3b[i].H1_b1_eta > minEta) & (data_4b_and_3b[i].H1_b2_eta > minEta) & (data_4b_and_3b[i].H2_b1_eta > minEta) & (data_4b_and_3b[i].H2_b2_eta > minEta)] 
# 	data_4b_and_3b[i]          = data_4b_and_3b[i][ (data_4b_and_3b[i].H1_b1_eta < maxEta) & (data_4b_and_3b[i].H1_b2_eta < maxEta) & (data_4b_and_3b[i].H2_b1_eta < maxEta) & (data_4b_and_3b[i].H2_b2_eta < maxEta)]
# 	data_4b_and_3b[i].query(preSelection, inplace = True)
# 	data_4b_and_3b[i].query(controlRegionSelection, inplace = True)
# 	print "   -Number of events in dataset ", type[i], " (after) = ",len(data_4b_and_3b[i]) 


out = subprocess.Popen(['eos', const.eosPath, 'ls', skimFolder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
stdout,stderr = out.communicate()
skimFileList = []
for fileName in stdout.split():
	if ".root" in fileName:
		skimFileList.append(const.eosPath + "/" + skimFolder + "/" + fileName)
dataset = data.root2pandas(skimFileList, const.treeName, branches=enabledBranches)

print "Number of events in dataset before cuts = ",len(dataset) 
dataset          = dataset[ (dataset.H1_b1_pt > minpt) & (dataset.H1_b2_pt > minpt) & (dataset.H2_b1_pt > minpt) & (dataset.H2_b2_pt > minpt)] 
dataset          = dataset[ (dataset.H1_b1_ptRegressed > minRegressedPt) & (dataset.H1_b2_ptRegressed > minRegressedPt) & (dataset.H2_b1_ptRegressed > minRegressedPt) & (dataset.H2_b2_ptRegressed > minRegressedPt)] 
dataset          = dataset[ (dataset.H1_b1_eta > minEta) & (dataset.H1_b2_eta > minEta) & (dataset.H2_b1_eta > minEta) & (dataset.H2_b2_eta > minEta)] 
dataset          = dataset[ (dataset.H1_b1_eta < maxEta) & (dataset.H1_b2_eta < maxEta) & (dataset.H2_b1_eta < maxEta) & (dataset.H2_b2_eta < maxEta)]
dataset.query(preSelection, inplace = True)
dataset.query(controlRegionSelection, inplace = True)
print "Number of events in dataset after cuts = ",len(dataset) 

# Run BDT reweight
BuildReweightingModel(dataset.query(bTagSelection), dataset.query(antiBTagSelection), trainingVariables, modelArguments, modelFileNameFullPath)

sys.stdout = orig_stdout
print(logFile.read())
logFile.close()