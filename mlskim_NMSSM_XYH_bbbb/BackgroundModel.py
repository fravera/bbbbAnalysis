import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
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

modelFileName = 'reweighter.pkl'

class ReweightModelAndTransferFactors:
  def __init__(self, reweightMethod, transferFactor, renormalizationTransferFactor):
    self.reweightMethod = reweightMethod
    self.transferFactor = transferFactor
    self.renormalizationTransferFactor = renormalizationTransferFactor

trainingVariables = ['H1_pt', 'H2_pt', 'H1_eta', 'H2_eta', 'HH_m', 'H1_bb_DeltaR', 'H2_bb_DeltaR', 'H2_m']

def SelectControlRegion(sample,validation,selection):
	datacr,datarest = selector.eventSelection(sample,selection,'CR',validation)
	return datacr,datarest

def BuildReweightingModel(data_3b, data_4b,seed):
	print "[INFO] Processing predicted model"
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	# variablescr = ['H1_pt', 'H2_pt', 'H1_eta', 'H2_eta', 'HH_m', 'H1_bb_DeltaR', 'H2_bb_DeltaR']
	variablescr = trainingVariables
	modelargs= [100, 0.1, 5, 300, 0.5, seed]
	originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
	print "transfer factor = ", transferfactor
	# plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"original")
	#######################################
	##Folding Gradient Boosted Reweighter
	#######################################
	foldingcr_weights,reweightermodel,renormtransferfactor = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  
	# plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"model")
	########################################
	##GB ROC AUC
	########################################
	bdtreweighter.roc_auc_measurement(originalcr,targetcr,originalcr_weights,foldingcr_weights)
	########################################
	##Update 3b dataframe for modeling
	########################################
	theReweightModelAndTransferFactors = ReweightModelAndTransferFactors(reweightermodel,transferfactor,renormtransferfactor)
	with open(modelFileName, 'w') as reweighterOutputFile:
		pickle.dump(theReweightModelAndTransferFactors, reweighterOutputFile)

	return foldingcr_weights,reweightermodel,transferfactor,renormtransferfactor

def CreatePredictionModel(reweightermodel,transferfactor,renormtransferfactor,data_3b):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	variables = trainingVariables
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,reweightermodel,transferfactor,renormtransferfactor)
	return folding_weights


def addModelWeight(dataset,datasetAntiBtag,valFlag,selection, seed):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_3b_categ,data_rest_3b_categ = SelectControlRegion(datasetAntiBtag,valFlag,selection)
	data_cr_4b_categ,data_rest_4b_categ = SelectControlRegion(dataset,valFlag,selection)
	#Get weights ,model, transferfactor for CR data
	print "[INFO] Building BDT-reweighting model for Weight_forBackground in the CR data"
	weights_cr_categ,reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ=BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,seed)
	# #loading formula
	# with open(modelFileName) as reweighterInputFile:
	# 	theReweightModelAndTransferFactors = pickle.load(reweighterInputFile)

	# reweightermodel_categ      = theReweightModelAndTransferFactors.reweightMethod 
	# transferfactor_categ       = theReweightModelAndTransferFactors.transferFactor
	# renormtransferfactor_categ = theReweightModelAndTransferFactors.renormalizationTransferFactor 

	#Get weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction Weight_forBackground in the other data"
	weights_rest_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ,data_rest_3b_categ)
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight Weight_forBackground to the dataframes"
	data_cr_3b_categ["Weight_forBackground"]      = weights_cr_categ 
	data_rest_3b_categ["Weight_forBackground"]    = weights_rest_categ
	#Merge (concatenate) them
	print "[INFO] Returning dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	value = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	valuef = numpy.multiply(value,transferfactor_categ)
	datasetwithweight["Weight_forBackground"+"_tfactor"] = valuef
	del dataset
	return datasetwithweight

def RunReweightingModel(dataset, datasetAntiBtag, year, region, validation, selection, seed):
	datasetWithWeights=addModelWeight(dataset, datasetAntiBtag, validation,selection, seed)
	print 'Here comes the dataset with reweighting-model weights . . .'
	print datasetWithWeights.head()

	#Save modeling dataframe tree to a root file
	if seed!=2019:
	   data.pandas2root(datasetWithWeights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree_%s.root'%(year,seed))
	   data.roothist2root(year,region, 'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist_%s.root'%(year,seed),True)
	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL_%s_%s.root outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(year,region,seed,year,seed,year,seed))
	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(year,seed,year,seed))	

	else:
		data.pandas2root(datasetWithWeights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree.root'%year)
		data.roothist2root(year,region, 'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist.root'%year,True)
		os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL_%s.root outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(year,region,year,year))
		os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(year,year))	

# def RunReweightingModelwithSignal(dataset,signal,case,tag,seed):
# 	dataset_signal     = pandas.concat( (signal,dataset),  ignore_index=True)
# 	datasetWithWeights1=AddModelWeight(dataset_signal,'Weight_210_GGF','GGF',True,seed)
# 	datasetWithWeights2=AddModelWeight(datasetWithWeights1,'Weight_210_VBF','VBF',True,seed)
# 	datasetWithWeights3=AddModelWeight(datasetWithWeights2,'Weight_110_GGF','GGF',False,seed)
# 	datasetWithWeights =AddModelWeight(datasetWithWeights3,'Weight_110_VBF','VBF',False,seed)
# 	print 'Here comes the dataset with reweighting-model weights . . .'
# 	print datasetWithWeights.head()
# 	#Save modeling dataframe tree to a root file
# 	if seed!=2019:
# 	   data.pandas2root(datasetWithWeights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree_%s.root'%(case,seed)  )
# 	   data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist_%s.root'%(case,seed)  )
# 	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL_%s.root outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,seed,case,seed,case,seed))
# 	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,seed,case,seed) )
# 	else:
# 	   data.pandas2root(datasetWithWeights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree.root'%(case)  )
# 	   data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist.root'%(case)  )
# 	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL.root outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case,case))
# 	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case) )

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory[0]
randseed    = ast.literal_eval(cfgparser.get("configuration","seed"))
print "    -The random seed:"
print "      *",randseed 
minpt       = ast.literal_eval(cfgparser.get("configuration","minbjetpt"))
print "    -The min b-jet pt (OPTION TO BE REMOVED FOR THE NEXT JOBS (changing PT>25 GeV to PT>MinPT)!):"
print "      *",minpt 
selection   = "fullRegion" #ast.literal_eval(cfgparser.get("configuration","selection"))
print "    -The selection:"
print "      *",selection
minMY       = 140 #ast.literal_eval(cfgparser.get("configuration","minMY"))
print "    -The minMY:"
print "      *",minMY
maxMX       = 1000 #ast.literal_eval(cfgparser.get("configuration","maxMX"))
print "    -The maxMX:"
print "      *",maxMX
##########Make microskims
print "[INFO] Making background model . . . "

# dataset_Plus          = data.root2pandas('/eos/uscms/store/user/fravera/bbbb_ntuples/NMSSM_XYH_bbbb_basicSelection_Plus_v12/SKIM_BTagCSV_Data/output/bbbbNtuple_0.root'         ,'bbbbTree')
# dataset_Plus_AntiBtag = data.root2pandas('/eos/uscms/store/user/fravera/bbbb_ntuples/NMSSM_XYH_bbbb_basicSelection_Plus_AntiBtag_v12/SKIM_BTagCSV_Data/output/bbbbNtuple_0.root','bbbbTree')
dataset_Plus               = data.root2pandas(['outputskims/%s/SKIM_Data_Plus.root'%directory[0]]         ,'bbbbTree')
dataset_Plus_AntiBtag      = data.root2pandas(['outputskims/%s/SKIM_Data_Plus_AntiBtag.root'%directory[0]],'bbbbTree')
dataset_skim_Plus          = dataset_Plus[ (dataset_Plus.H1_b1_pt > minpt) & (dataset_Plus.H1_b2_pt > minpt) & (dataset_Plus.H2_b1_pt > minpt) & (dataset_Plus.H2_b2_pt > minpt) & (dataset_Plus.H2_m > minMY) & (dataset_Plus.HH_m < maxMX) ]
dataset_skim_Plus_AntiBtag = dataset_Plus_AntiBtag[ (dataset_Plus_AntiBtag.H1_b1_pt > minpt) & (dataset_Plus_AntiBtag.H1_b2_pt > minpt) & (dataset_Plus_AntiBtag.H2_b1_pt > minpt) & (dataset_Plus_AntiBtag.H2_b2_pt > minpt)  & (dataset_Plus_AntiBtag.H2_m > minMY) & (dataset_Plus_AntiBtag.HH_m < maxMX) ]
RunReweightingModel(dataset_skim_Plus, dataset_skim_Plus_AntiBtag,directory[0],"Data_Plus_AntiBtag",True,selection,randseed)


# # dataset          = data.root2pandas('/eos/uscms/store/user/fravera/bbbb_ntuples/NMSSM_XYH_bbbb_basicSelection_v12/SKIM_BTagCSV_Data/output/bbbbNtuple_0.root'         ,'bbbbTree')
# # dataset_AntiBtag = data.root2pandas('/eos/uscms/store/user/fravera/bbbb_ntuples/NMSSM_XYH_bbbb_basicSelection_AntiBtag_v12/SKIM_BTagCSV_Data/output/bbbbNtuple_0.root','bbbbTree')
# dataset               = data.root2pandas('outputskims/%s/SKIM_Data.root'%directory[0]         ,'bbbbTree')
# dataset_AntiBtag      = data.root2pandas('outputskims/%s/SKIM_Data_AntiBtag.root'%directory[0],'bbbbTree')
# dataset_skim          = dataset[ (dataset.H1_b1_pt > minpt) & (dataset.H1_b2_pt > minpt) & (dataset.H2_b1_pt > minpt) & (dataset.H2_b2_pt > minpt)  & (dataset_Plus.H2_m > minMY) & (dataset_Plus.HH_m < maxMX) ]
# dataset_skim_AntiBtag = dataset_AntiBtag[ (dataset_AntiBtag.H1_b1_pt > minpt) & (dataset_AntiBtag.H1_b2_pt > minpt) & (dataset_AntiBtag.H2_b1_pt > minpt) & (dataset_AntiBtag.H2_b2_pt > minpt)  & (dataset_AntiBtag.H2_m > minMY) & (dataset_AntiBtag.HH_m < maxMX) ]
# RunReweightingModel(dataset_skim, dataset_skim_AntiBtag,directory[0],"Data_AntiBtag",False,selection,randseed)
