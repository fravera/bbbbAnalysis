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

def SelectRegions(sample,btagregion,category,validation):
	datacr,samplerest = selector.eventselection(sample,btagregion,category,'CR',validation)
	del sample
	datasr,datarest   = selector.eventselection(samplerest,btagregion,category,'SR',validation)	
	del samplerest
	return datacr,datasr,datarest

def BuildReweightingModel(data_3b, data_4b,category,tag,bkgparams,bkgclassifierparams):
	print "[INFO] Processing predicted model for %s"%(tag)
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#700/600/300/200
	############################################################################
	modelargs = bkgparams
	if category =='GGF':
		 variablescr = ['nJet_ec','H1_m','H2_m','H1_pt','H2_pt','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','HH_m','costh_HH_b1_ggfcm','HH_btag_b3_bscore','HH_btag_b3_bres']
	elif category == 'VBF':
		 variablescr = ['nJet_ec','HH_m','H1_pt','H2_pt','h1h2_deltaEta','JJ_m','j1j2_deltaEta']
	else:
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta']  	
	#######################################
	##Prepare data to create the model
	#######################################
	originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor = data.preparedataformodel(data_3b,data_4b,variablescr)
	plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, originalcr_weights,True,"%s_original"%tag)
	
	#######################################
	##Folding Gradient Boosted Reweighter (2-fold BDT reweighter)
	#######################################
	foldingcr_weights,reweightermodel = data.fitreweightermodel(originalcr,targetcr,originalcr_weights,targetcr_weights,transferfactor,modelargs)  
	plotter.Draw1DHistosComparison(originalcr, targetcr, variablescr, foldingcr_weights,True,"%s_model"%tag)
	
	########################################
	## KS Test (as the developers of the method do), currently used for optimization/check of the parameters
	########################################
	ksresult_original = bdtreweighter.ks_test(originalcr, targetcr, variablescr, originalcr_weights)
	ksresult_model    = bdtreweighter.ks_test(originalcr, targetcr, variablescr, foldingcr_weights)	
	bdtreweighter.ks_comparison(variablescr,ksresult_original,ksresult_model)
	
	########################################
	## GB ROC AUC Test Study (Very slow test, needs to train a classifier in cross-validation)
	########################################
	bdtreweighter.discrimination_test(originalcr,targetcr,originalcr_weights,bkgclassifierparams,"%s"%tag,"original")
	bdtreweighter.discrimination_test(originalcr,targetcr,foldingcr_weights,bkgclassifierparams,"%s"%tag,"model")
	return foldingcr_weights,reweightermodel,transferfactor

def CreatePredictionModel(reweightermodel,transferfactor,data_3b,data_4b,category,valflag,srflag):
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#
	############################################################################
	if category =='GGF':
		 variables = ['nJet_ec','H1_m','H2_m','H1_pt','H2_pt','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','HH_m','costh_HH_b1_ggfcm','HH_btag_b3_bscore','HH_btag_b3_bres']
	elif category == 'VBF':
		 variables = ['nJet_ec','HH_m','H1_pt','H2_pt','h1h2_deltaEta','JJ_m','j1j2_deltaEta']
	else:
		 variables = ['H1_pt','H2_pt','H1_eta','H2_eta']          
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	target,target_weights     = data.preparedataforprediction(data_4b,1,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,target,target_weights,reweightermodel,transferfactor,valflag,srflag)
	########################################
	## KS Test (as the developers of the method do), done just for the validation signal region as we are currently blinded
	########################################
	#if valflag==True and srflag==True:
	#	ksresult_original = bdtreweighter.ks_test(original, target, variables, original_weights)
	#	ksresult_model    = bdtreweighter.ks_test(original, target, variables, folding_weights)	
	#	bdtreweighter.ks_comparison(variables,ksresult_original,ksresult_model)
	return folding_weights


def AddModelWeights(dataset,bkgparams,bkgclassifierparams,weightname,categ,valflag,tag):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	print "\n"*5
	print "[INFO] Preparing data and building BDT-reweighting model for %s using the 3b/4b CR data"%weightname
	data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ = SelectRegions(dataset,'3b',categ,valflag)
	del dataset
	data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ = SelectRegions(data_rest_3b_categ,'4b',categ,valflag)
	#Get weights ,model, transferfactor for CR data
	weights_cr_categ,reweightermodel_categ,transferfactor_categ=BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_%s_%sCR'%(tag,weightname,categ),bkgparams,bkgclassifierparams)
	del data_cr_4b_categ 
	#Get weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction %s in the 3b SR data"%weightname
	weights_sr_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,data_sr_3b_categ,data_sr_4b_categ,categ,valflag,True)		
	print "[INFO] Calculating BDT-reweighting model prediction %s in the rest of the data"%weightname
	weights_rest_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,data_rest_3b_categ,data_rest_4b_categ,categ,valflag,False)
	del data_sr_4b_categ,data_rest_4b_categ
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight %s"%weightname
	data_cr_3b_categ[weightname]      = weights_cr_categ 
	data_sr_3b_categ[weightname]      = weights_sr_categ
	data_rest_3b_categ[weightname]    = weights_rest_categ
	print "[INFO] Creating a region id flag used for debugging (-1=Rest,0=3-CR,1=3b-SR)"
	data_sr_3b_categ["SelectedRegion_%s"%weightname]      =  1.0
	data_cr_3b_categ["SelectedRegion_%s"%weightname]      =  0 
	data_rest_3b_categ["SelectedRegion_%s"%weightname]    = -1.0
	print "[INFO] Adding training weight for MVA training"
	if weightname=='Weight_AnaGGF':
		l = int(len(data_sr_3b_categ.index) / 2)
		data_sr_3b_categ = data_sr_3b_categ.reset_index(drop=True)
		data_sr_3b_categ.loc[:l,"Weight_MVA"] = 0.
		data_sr_3b_categ.loc[l:,"Weight_MVA"] = 2.0  
	#Merge (concatenate) them
	print "[INFO] Concatenating all events in the dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ), ignore_index=True   )
	del data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ
	value = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	valuef = numpy.multiply(value,transferfactor_categ)
	datasetwithweight[weightname+"_tfactor"] = valuef
	datasetwithweight=datasetwithweight.reset_index(drop=True)
	return datasetwithweight

def RunReweightingModel(dataset,bkgparams,classifierparams,case,directory,tag,valflag,catflag):
	#Add weights for analysis
	if valflag==True and catflag==True:
		dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValGGF','GGF',valflag,'%s%s'%(tag,case))
	elif valflag==True and catflag==False:
		dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_ValVBF','VBF',valflag,'%s%s'%(tag,case))
	elif valflag==False and catflag==True:
		dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaGGF','GGF',valflag,'%s%s'%(tag,case))
	else:
		dataset=AddModelWeights(dataset,bkgparams,classifierparams,'Weight_AnaVBF','VBF',valflag,'%s%s'%(tag,case))    	
	return dataset

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file',   required = True)
parser.add_argument('--casename', dest='casename',  help='Case name',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
case           = args.casename
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
ggfanabkgparams = ast.literal_eval(cfgparser.get("configuration","ggfanabkgparams"))
print "    -The ggf-hh ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggfanabkgparams
vbfanabkgparams = ast.literal_eval(cfgparser.get("configuration","vbfanabkgparams"))
print "    -The vbf-hh ana background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfanabkgparams
ggfvalbkgparams = ast.literal_eval(cfgparser.get("configuration","ggfvalbkgparams"))
print "    -The ggf-hh val background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",ggfvalbkgparams
vbfvalbkgparams = ast.literal_eval(cfgparser.get("configuration","vbfvalbkgparams"))
print "    -The vbf-hh val background parameters (trees, learning rate, max depth, min sample leaf, subsample, randomseed):"
print "      *",vbfvalbkgparams
ggfanaclassifierparams = ast.literal_eval(cfgparser.get("configuration","ggfanaclassifierparams"))
print "    -The ggf-hh ana classifier parameters (trees, subsample, randomseed):"
print "      *",ggfanaclassifierparams
vbfanaclassifierparams = ast.literal_eval(cfgparser.get("configuration","vbfanaclassifierparams"))
print "    -The vbf-hh ana classifier parameters (trees, subsample, randomseed):"
print "      *",vbfanaclassifierparams
ggfvalclassifierparams  = ast.literal_eval(cfgparser.get("configuration","ggfvalclassifierparams"))
print "    -The ggf-hh val classifier parameters (trees, subsample, randomseed):"
print "      *",ggfvalclassifierparams
vbfvalclassifierparams = ast.literal_eval(cfgparser.get("configuration","vbfvalclassifierparams"))
print "    -The vbf-hh val classifier parameters (trees, subsample, randomseed):"
print "      *",vbfvalclassifierparams
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tag name:"
print "      *",tag 
print "    -The case of event selection a.k.a. foldername:"
print "      *",case 

##########Make background miodel
print "[INFO] Making background model . . . "
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
datalist =[]
datalist.append('outputskims/%s/%s/SKIM_Data.root'%(case,directory))
dataset = data.root2pandas(datalist,'bbbbTree')
#Add VBF weights for analysis/validation region
dataset = RunReweightingModel(dataset,vbfanabkgparams,vbfanaclassifierparams,case,directory,tag,False,False)
dataset = RunReweightingModel(dataset,vbfvalbkgparams,vbfvalclassifierparams,case,directory,tag,True,False)
#Add GGF weights for analysis/validation region
dataset = RunReweightingModel(dataset,ggfanabkgparams,ggfanaclassifierparams,case,directory,tag,False,True)
dataset = RunReweightingModel(dataset,ggfvalbkgparams,ggfvalclassifierparams,case,directory,tag,True,True)
#Save everything in a root file
print "[INFO] Saving background model . . . "
data.pandas2root(dataset,'bbbbTree',   'outputskims/%s/%s/SKIM_BKG_MODEL_tree.root'%(case,directory)  )
data.roothist2root(datalist,'eff_histo','outputskims/%s/%s/SKIM_BKG_MODEL_hist.root'%(case,directory)  )
os.system("hadd -f outputskims/%s/%s/SKIM_BKG_MODEL.root outputskims/%s/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(case,directory,case,directory,case,directory))
os.system("rm outputskims/%s/%s/SKIM_BKG_MODEL_tree.root outputskims/%s/%s/SKIM_BKG_MODEL_hist.root"%(case,directory,case,directory) )			
