import numpy 
import os
import math
import matplotlib
import root_numpy
import pandas
import glob
import ROOT
import root_pandas
from keras.models import load_model
from hep_ml import reweight
from root_numpy import root2array
from matplotlib import pyplot as plt
from numpy.lib.recfunctions import stack_arrays
from sklearn.model_selection import train_test_split
from hep_ml.metrics_utils import ks_2samp_weighted
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
from matplotlib.colors import LogNorm
from sklearn.externals.joblib import dump, load
#My modules
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter

def PrepareFeaturesFromSkim(skim,features,tag):
	#Get features
	data = skim[features]
	#Convert it as input in the model
	X = data.as_matrix()
	scaler = load('./mymodels/std_scaler_%s.bin'%tag)
	X = scaler.transform(X)
	return X

def PrepareModel(tag):
	model = load_model(tag)
	print("Loaded model and weights from mymodels")	
	return model

def root2pandas(files_path, tree_name, **kwargs):
	# -- create list of .root files to process

	# -- process ntuples into rec arrays
	ss = stack_arrays([root2array(fpath, tree_name, **kwargs).view(numpy.recarray) for fpath in files_path])

	try:
		return pandas.DataFrame(ss)
	except Exception:
		return pandas.DataFrame(ss.data)

def flatten(column):
	try:
		return np.array([v for e in column for v in e])
	except (TypeError, ValueError):
		return column

def preparedataformodel(ioriginal, itarget, variables):
	original  = ioriginal[variables].reset_index(drop=True)
	target    = itarget[variables].reset_index(drop=True)
	tfactor   = float( float(len(target.index)) / float(len(original.index)) )
	original_weights = numpy.ones(dtype='float64',shape=len(original))
	original_weights = numpy.multiply(original_weights,tfactor)
	target_weights   = numpy.ones(dtype='float64',shape=len(target))
	return original,target,original_weights,target_weights,tfactor

def preparedataforprediction(ioriginal,tfactor, variables):
	original  = ioriginal[variables].reset_index(drop=True)
	original_weights = numpy.ones(dtype='float64',shape=len(original))
	original_weights = numpy.multiply(original_weights,tfactor)
	return original,original_weights
   
def fitreweightermodel(original,target,original_weights,target_weights,tfactor, model_args):
	print "[INFO] Fitting BDT-reweighter ..."
	model                = bdtreweighter.reweightermodel(original,target,original_weights,target_weights,model_args) 
	ws_unnormalized      = model.predict_weights(original,original_weights,lambda x: numpy.mean(x, axis=0))
	#Normalized all reweighter weights to 1
	weights = numpy.multiply(ws_unnormalized, (1/ws_unnormalized.sum())  )
	#Give them the normalization using the transfer factor derived in control regions
	totalnorm  = int(len(original))*tfactor
	weights    = numpy.multiply(weights,totalnorm)
	print "[INFO] Event yields report in control region derivation:"
	print "   -The sum of original weights                 = ",int(len(original)),"+/-",math.sqrt(len(original) )
	print "   -The sum of target weights                   = ",int(len(target)),"+/-",math.sqrt(len(target))
	print "   -The transfer factor                         = ",tfactor,"+/-",tfactor*math.sqrt(  (math.sqrt(len(target))/len(target))**2 + (math.sqrt(len(original))/len(original))**2   )
	print "   -The sum of model weights                    = ",weights.sum(),"+/-",math.sqrt(numpy.square(weights).sum() )
	return weights,model

def getmodelweights(original,original_weights,model,tfactor):
	print "[INFO] Running prediction from BDT-reweighter ..."
	ws_unnormalized = model.predict_weights(original,original_weights,lambda x: numpy.mean(x, axis=0))
	#Normalized all reweighter weights to 1
	weights = numpy.multiply(ws_unnormalized, (1/ws_unnormalized.sum())  )	
	#Give them the normalization using the transfer factor derived in control regions
	totalnorm  = int(len(original))*tfactor
	weights    = numpy.multiply(weights,totalnorm)
	print "[INFO] Event yields report in prediction:"
	print "   -The transfer factor from control regions        = ",tfactor
	print "   -The sum of model weights                        = ",weights.sum(),"+/-",math.sqrt(numpy.square(weights).sum() )
	return weights


def pandas2root(tree_dataframe, tree_name, rootfile_name, mode="w"):
	tree_dataframe.to_root('%s'%rootfile_name, key='%s'%tree_name, mode=mode, store_index=False)

def roothist2root(year,region,hist_name, rootfile_name, inputSkim = False):
	if inputSkim:
		folderName = 'inputskims'
	else:
		folderName = 'outputskims'
	file = ROOT.TFile.Open('%s/%s/SKIM_%s.root'%(folderName,year,region))
	print '%s/%s/SKIM_%s.root'%(folderName,year,region)
	hfile =  ROOT.TFile('%s'%rootfile_name, 'RECREATE')
	h=file.Get(hist_name)
	h.Write()
	hfile.Write()
	hfile.Close()

def pandas2historoot(dataframe,hist_name, rootfile_name):
	ntotal = len(dataframe)
	print "N total is ",ntotal
	ntotalweight = dataframe['MLWeight'].sum()
	print "N total weighted is ",ntotalweight
	hfile =  ROOT.TFile('%s'%rootfile_name, 'RECREATE')
	eff_histo=ROOT.TH1D(hist_name,hist_name,6,0,6)
	eff_histo.Fill("Ntot_uw",ntotal)
	eff_histo.Fill("Ntot_w",ntotalweight)
	eff_histo.Fill("Ntrig_uw",ntotal)
	eff_histo.Fill("Ntrig_w",ntotalweight)
	eff_histo.Fill("Nsel_uw",ntotal)
	eff_histo.Fill("Nsel_w",ntotalweight)
	eff_histo.Write()
	hfile.Write()
	hfile.Close()


def mergedata2root(treefile,histofile,rootfile_name):
	os.system('hadd -f %s %s %s'%(rootfile_name,treefile,histofile) )
	os.system('rm %s %s'%(treefile,histofile) )

def TotalMCEvents(sample):
	file = ROOT.TFile.Open(sample)
	h=file.Get('eff_histo')
	file.Close()
	w=h.GetBinContent(1)
	return w
