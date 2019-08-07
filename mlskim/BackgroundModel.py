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

def SelectControlRegion(sample,btagregion,category,validation):
	datacr,datarest = selector.eventselection(sample,btagregion,category,'CR',validation)
	return datacr,datarest

def BuildReweightingModel(data_3b, data_4b,category,tag,seed):
	print "[INFO] Processing predicted model for %s"%(tag)
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#700/600/300/200
	############################################################################
	if category =='GGF':
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','HH_m','H1_bb_deltaR','H2_bb_deltaR']
		 modelargs= [100,0.1,5,400,0.5,seed]
	elif category == 'VBF':
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','HH_m','JJ_m','j1j2_deltaEta']
		 modelargs= [100,0.1,5,300,0.5,seed]
	else:
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta']  	
	originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
	plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"original_%s"%tag)
	#######################################
	##Folding Gradient Boosted Reweighter
	#######################################
	foldingcr_weights,reweightermodel,renormtransferfactor = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  
	plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"model_%s"%tag)
	########################################
	##GB ROC AUC
	########################################
	bdtreweighter.roc_auc_measurement(originalcr,targetcr,originalcr_weights,foldingcr_weights)
	########################################
	##Update 3b dataframe for modeling
	########################################
	return foldingcr_weights,reweightermodel,transferfactor,renormtransferfactor

def CreatePredictionModel(reweightermodel,transferfactor,renormtransferfactor,data_3b,category):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','HH_m','H1_bb_deltaR','H2_bb_deltaR']
	elif category == 'VBF':
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','HH_m','JJ_m','j1j2_deltaEta']
	else:
		 variables = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta']          
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,reweightermodel,transferfactor,renormtransferfactor)
	return folding_weights


def AddModelWeight(dataset,weightname,categ,valflag,seed):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_3b_categ,data_rest_3b_categ = SelectControlRegion(dataset,'3b',categ,valflag)
	data_cr_4b_categ,data_rest_4b_categ = SelectControlRegion(dataset,'4b',categ,valflag)
	#Get weights ,model, transferfactor for CR data
	print "[INFO] Building BDT-reweighting model for %s in the CR data"%weightname
	weights_cr_categ,reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ=BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'CR_%s_%s'%(categ,weightname),seed)
	#Get weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction %s in the other data"%weightname
	weights_rest_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ,data_rest_3b_categ,categ)
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight %s to the dataframes"%weightname
	data_cr_3b_categ[weightname]      = weights_cr_categ 
	data_rest_3b_categ[weightname]    = weights_rest_categ
	#Merge (concatenate) them
	print "[INFO] Returning dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	value = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	valuef = numpy.multiply(value,transferfactor_categ)
	datasetwithweight[weightname+"_tfactor"] = valuef
	del dataset
	return datasetwithweight

def RunReweightingModel(dataset,case,seed):
	datasetwithweights1=AddModelWeight(dataset,'Weight_210_GGF','GGF',True,seed)
	datasetwithweights2=AddModelWeight(datasetwithweights1,'Weight_210_VBF','VBF',True,seed)
	datasetwithweights3=AddModelWeight(datasetwithweights2,'Weight_110_GGF','GGF',False,seed)
	datasetwithweights =AddModelWeight(datasetwithweights3,'Weight_110_VBF','VBF',False,seed)
	print 'Here comes the dataset with reweighting-model weights . . .'
	print datasetwithweights.head()
	#Save modeling dataframe tree to a root file
	if seed!=2019:
	   data.pandas2root(datasetwithweights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree_%s.root'%(case,seed)  )
	   data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist_%s.root'%(case,seed)  )
	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL_%s.root outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,seed,case,seed,case,seed))
	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,seed,case,seed) )
	else:
	   data.pandas2root(datasetwithweights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree.root'%(case)  )
	   data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist.root'%(case)  )
	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL.root outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case,case))
	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case) )			

def RunReweightingModelwithSignal(dataset,signal,case,tag,seed):
	dataset_signal     = pandas.concat( (signal,dataset),  ignore_index=True)
	datasetwithweights1=AddModelWeight(dataset_signal,'Weight_210_GGF','GGF',True,seed)
	datasetwithweights2=AddModelWeight(datasetwithweights1,'Weight_210_VBF','VBF',True,seed)
	datasetwithweights3=AddModelWeight(datasetwithweights2,'Weight_110_GGF','GGF',False,seed)
	datasetwithweights =AddModelWeight(datasetwithweights3,'Weight_110_VBF','VBF',False,seed)
	print 'Here comes the dataset with reweighting-model weights . . .'
	print datasetwithweights.head()
	#Save modeling dataframe tree to a root file
	if seed!=2019:
	   data.pandas2root(datasetwithweights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree_%s.root'%(case,seed)  )
	   data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist_%s.root'%(case,seed)  )
	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL_%s.root outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,seed,case,seed,case,seed))
	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree_%s.root outputskims/%s/SKIM_BKG_MODEL_hist_%s.root"%(case,seed,case,seed) )
	else:
	   data.pandas2root(datasetwithweights,'bbbbTree', 'outputskims/%s/SKIM_BKG_MODEL_tree.root'%(case)  )
	   data.roothist2root('Data','%s'%(case),'eff_histo','outputskims/%s/SKIM_BKG_MODEL_hist.root'%(case)  )
	   os.system("hadd -f outputskims/%s/SKIM_BKG_MODEL.root outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case,case))
	   os.system("rm outputskims/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/SKIM_BKG_MODEL_hist.root"%(case,case) )

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
##########Make microskims
print "[INFO] Making background model . . . "
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
dataset      = data.root2pandas('outputskims/%s/SKIM_Data.root'%(directory[0]),'bbbbTree')
datasetskim  = dataset[ (dataset.HH_b1_pt > minpt) & (dataset.HH_b2_pt > minpt) & (dataset.HH_b3_pt > minpt) & (dataset.HH_b4_pt > minpt) ]
RunReweightingModel(datasetskim,directory[0],randseed)