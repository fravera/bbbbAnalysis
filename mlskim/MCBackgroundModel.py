import numpy 
import os
import root_numpy
import pandas
import glob
import argparse
import sys
import ast
import ROOT
from  ConfigParser import *
from hep_ml import reweight
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def SelectRegionforBackgroundRejectionTraining(sample,btagregion,category,validation):
	datacr,datasrrest = selector.eventselection(sample,btagregion,category,'CR',validation)
	datasr,datarest   = selector.eventselection(datasrrest,btagregion,category,'SR',validation)
	return datacr,datasr,datarest

def SelectRegionforProductionModeTraining(sample,btagregion,category,validation):
	dataselected,datarest = selector.eventselection(sample,btagregion,category,'None',validation)
	return dataselected

def BuildReweightingModel(data_3b, data_4b,category,tag,seed):
	print "[INFO] Processing predicted model for %s"%(tag)
	############################################################################
	##Let's slice data one more time to have the inputs for the bdt reweighting#700/600/300/200
	############################################################################
	if category =='GGF':
		 variablescr = ['nJet_ec','H1_pt','H2_pt','H1_m','H2_m', 'HH_m','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','costh_HH_b1_ggfcm','costh_HH_b2_ggfcm']
		 modelargs= [55,0.1,2,50,0.5,seed] 
	else:
		 variablescr = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta']  	
		 modelargs= [50,0.1,5,100,0.5,seed]
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
		 variables = ['nJet_ec','H1_pt','H2_pt','H1_m','H2_m', 'HH_m','h1h2_deltaEta','H1_bb_deltaR','H2_bb_deltaR','costh_HH_b1_ggfcm','costh_HH_b2_ggfcm']
	elif category == 'VBF':
		 variables = ['H1_pt', 'H2_pt','H1_eta','H2_eta']
	else:
		 variables = ['H1_pt', 'H2_pt']          
	original,original_weights = data.preparedataforprediction(data_3b,transferfactor,variables)
	########################################3############
	##Folding Gradient Boosted Reweighter (and DQM plots)
	#####################################################
	folding_weights= data.getmodelweights(original,original_weights,reweightermodel,transferfactor,renormtransferfactor)
	return folding_weights


def AddModelWeight(case,directory,mcsample,dataset,weightname,categ,valflag,tag,seed):
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_3b_categ,data_sr_3b_categ,data_rest_3b_categ = SelectRegionforBackgroundRejectionTraining(dataset,'3b',categ,valflag)
	data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ = SelectRegionforBackgroundRejectionTraining(dataset,'4b',categ,valflag)
	#Get weights ,model, transferfactor for CR data
	print "[INFO] Building BDT-reweighting model for %s in the CR MC"%weightname
	weights_cr_categ,reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ=BuildReweightingModel(data_cr_3b_categ,data_cr_4b_categ,categ,'%s_CR_%s_%s'%(tag,categ,weightname),seed)
	#Get weights for the dataset
	print "[INFO] Calculating BDT-reweighting model prediction %s in the SR MC"%weightname
	weights_sr_categ=CreatePredictionModel(reweightermodel_categ,transferfactor_categ,renormtransferfactor_categ,data_sr_3b_categ,categ)
	#Add the weights to the CR & the rest
	print "[INFO] Adding weight %s to the dataframes"%weightname
	data_cr_3b_categ[weightname]     = weights_cr_categ
	data_cr_3b_categ["GGFSignalRegion"] = 0	 
	data_sr_3b_categ[weightname]     = weights_sr_categ
	data_sr_3b_categ["GGFSignalRegion"] = 1
	#Compute the sample efficiency
	srefficiency    = float (len(data_sr_4b_categ) / data.TotalMCEvents('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)))
	crefficiency    = float (len(data_cr_4b_categ) / data.TotalMCEvents('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)))
	print "[INFO] Adding efficiency branch to the dataframes"
	data_sr_3b_categ["SelEff"]        = srefficiency 
	data_cr_3b_categ["SelEff"]        = crefficiency 		
	#Merge (concatenate) them
	print "[INFO] Returning dataframe"
	datasetwithweight = pandas.concat( (data_cr_3b_categ,data_sr_3b_categ), ignore_index=True   )
	value = numpy.ones(dtype='float64',shape=len(datasetwithweight))
	valuef = numpy.multiply(value,transferfactor_categ)
	datasetwithweight[weightname+"_tfactor"] = valuef
	del dataset
	return datasetwithweight

def RunReweightingModel(mcsamples,case,directory,tag,seed):
	os.system('mkdir trainingbdts')
	os.system('mkdir trainingbdts/%s'%case)
	os.system('mkdir trainingbdts/%s/%s'%(case,directory) )	
	for mcsample in mcsamples:
		#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
		mcsampleframe = data.root2pandas('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample),'bbbbTree')
		datasetwithweights = AddModelWeight(case,directory,mcsample,mcsampleframe,'Weight_SR_GGF','GGF',False,"%s_%s"%(tag,mcsample),seed)
		print 'Saving mc sample with reweighting-model weights . . .'
		#Save modeling dataframe tree to a root file
		if seed!=2019:
		   data.pandas2root(datasetwithweights,'bbbbTree', 'trainingbdts/%s/%s/SKIM_%s_BR_%s.root'%(case,directory,mcsample,seed)  )
		else:
		   data.pandas2root(datasetwithweights,'bbbbTree', 'trainingbdts/%s/%s/SKIM_%s_BR.root'%(case,directory,mcsample)  )
		del datasetwithweights,mcsampleframe  

def RunSignalSelection(signalsamples,case,directory,tag):
	os.system('mkdir trainingbdts')
	os.system('mkdir trainingbdts/%s'%case)
	os.system('mkdir trainingbdts/%s/%s'%(case,directory) )	
	#First prepare data to train for background rejection
	for mcsample in signalsamples:
		#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
		mcsampleframe = data.root2pandas('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample),'bbbbTree')
		#Slice the data sample to take only events with three/four b-tags among the two categories
		data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ = SelectRegionforBackgroundRejectionTraining(mcsampleframe,'4b','GGF',False)
		#Add the weights to the CR & the rest
		print "[INFO] Adding branches to the dataframes"
		data_cr_4b_categ['Weight_SR_GGF']     = 1
		data_cr_4b_categ["GGFSignalRegion"]      = 0	 
		data_sr_4b_categ['Weight_SR_GGF']     = 1
		data_sr_4b_categ["GGFSignalRegion"]      = 1
		data_rest_4b_categ['Weight_SR_GGF']   = 1
		data_rest_4b_categ["GGFSignalRegion"]    = -1
		#Compute the sample efficiency
		srefficiency    = float (len(data_sr_4b_categ)   / data.TotalMCEvents('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)))
		crefficiency    = float (len(data_cr_4b_categ)   / data.TotalMCEvents('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)))
		restefficiency  = float (len(data_rest_4b_categ) / data.TotalMCEvents('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)))
		print "[INFO] Adding efficiency branch to the dataframes"
		data_sr_4b_categ["SelEff"]        = srefficiency 
		data_cr_4b_categ["SelEff"]        = crefficiency			 
		#Merge (concatenate) them
		print "[INFO] Saving mc signal sample for background rejection"
		dataset = pandas.concat( (data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ), ignore_index=True   )
		data.pandas2root(dataset,'bbbbTree', 'trainingbdts/%s/%s/SKIM_%s_BR.root'%(case,directory,mcsample)  )
		del dataset
	#Second prepare data to train for production mode
	for mcsample in signalsamples:
		#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
		mcsampleframe = data.root2pandas('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample),'bbbbTree')
		#Slice the data sample to take only events with three/four b-tags among the two categories
		data_4b_prevbf_categ = SelectRegionforProductionModeTraining(mcsampleframe,'4b','preVBF',False)
		#Compute the sample efficiency
		preVBFefficiency    = float (len(data_4b_prevbf_categ)   / data.TotalMCEvents('outputskims/%s/%s/SKIM_%s.root'%(case,directory,mcsample)))
		print "[INFO] Adding efficiency branch to the dataframes: ",preVBFefficiency
		data_4b_prevbf_categ["preVBFSelEff"] = preVBFefficiency
		#Merge (concatenate) them
		print "[INFO] Saving mc signal sample for background rejection"
		data.pandas2root(data_4b_prevbf_categ,'bbbbTree', 'trainingbdts/%s/%s/SKIM_%s_PM.root'%(case,directory,mcsample)  )
		del data_4b_prevbf_categ       

def RunBkgModelSelection(bkgmodel,case,directory,tag):
	os.system('mkdir trainingbdts')
	os.system('mkdir trainingbdts/%s'%case)
	os.system('mkdir trainingbdts/%s/%s'%(case,directory) )	
	#First prepare data to train for background rejection
	#Get data and create panda dataframes with specific variables, a.k.a. slicing the data. Then, create background model based on control region data
	sampleframe = data.root2pandas('outputskims/%s/%s/SKIM_%s.root'%(case,directory,bkgmodel),'bbbbTree')
	#Slice the data sample to take only events with three/four b-tags among the two categories
	data_cr_4b_categ,data_sr_4b_categ,data_rest_4b_categ = SelectRegionforBackgroundRejectionTraining(sampleframe,'3b','GGF',False)
	#Add the weights to the CR & the rest
	print "[INFO] Adding branches to the dataframes"	 
	data_sr_4b_categ['Weight_SR_GGF']   = 1
	data_sr_4b_categ["GGFSignalRegion"] = 1
	#Compute the sample efficiency
	print "[INFO] Adding efficiency branch to the dataframes"
	data_sr_4b_categ["SelEff"]          = 1 		 
	data_sr_4b_categ["XS"]              = 1
	#Merge (concatenate) them
	print "[INFO] Saving mc signal sample for background rejection"
	data.pandas2root(data_sr_4b_categ,'bbbbTree', 'trainingbdts/%s/%s/SKIM_%s_BR.root'%(case,directory,bkgmodel)  )
	del sampleframe 

#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',   dest='cfgfile',   help='Name of config file',   required = True)
parser.add_argument('--sample',   dest='samplename',   help='Type of sample: 0 signal, 1 bkg, 2 both',   required = True)
parser.add_argument('--casename', dest='casename',  help='Case name',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
sampletorun    = args.samplename
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
randseed    = ast.literal_eval(cfgparser.get("configuration","seed"))
print "    -The random seed:"
print "      *",randseed 
signalsamples  = ast.literal_eval(cfgparser.get("configuration","signalsamples"))
print "    -The list of signal samples:"
for x in range(len(signalsamples)):
  print "      *",signalsamples[x]
qcdsamples     = ast.literal_eval(cfgparser.get("configuration","qcdsamples"))
print "    -The list of qcd samples:"
for x in range(len(qcdsamples)):
  print "      *",qcdsamples[x]
bdtbkgsamples  = ast.literal_eval(cfgparser.get("configuration","bdtbkgsamples"))
print "    -The list of bdtbkg samples:"
print "      *",bdtbkgsamples[0] 
tag         = ast.literal_eval(cfgparser.get("configuration","tag"))
print "    -The tag:"
print "      *",tag  
##########Make samples to train
if sampletorun=='2':
   print "[INFO] Preparing bdt-background model"
   #Create signal sample in signal region
   RunBkgModelSelection(bdtbkgsamples[0],case,directory,tag)
elif sampletorun=='1':
   print "[INFO] Preparing MC background model"
   #Create background sample with bdt weights model in signal region
   RunReweightingModel(qcdsamples,case,directory,tag,randseed)
elif sampletorun=='0':   
   print "[INFO] Preparing MC signal model"
   #Create signal sample in signal region
   RunSignalSelection(signalsamples,case,directory,tag)
else:
   print "[WARNING] Type of sample is not specified"