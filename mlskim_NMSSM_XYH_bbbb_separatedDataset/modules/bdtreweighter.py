import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
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

def reweightermodel(ioriginal,itarget,ioriginal_weights,itarget_weights,args): 
	numpy.random.seed(args[5]) #Fix any random seed using numpy arrays
	reweighter_base = reweight.GBReweighter(n_estimators=args[0], learning_rate=args[1], max_depth=args[2], min_samples_leaf=args[3],gb_args={'subsample': args[4]})
	reweighter = reweight.FoldingReweighter(reweighter_base, random_state=args[5], n_folds=2, verbose=True)
	reweighter.fit(ioriginal,itarget,ioriginal_weights,itarget_weights)
	return reweighter

def check_ks_of_expression(expression):
	col_original = original_test.eval(expression, engine='python')
	col_target = target_test.eval(expression, engine='python')
	w_target = numpy.ones(len(col_target), dtype='float')
	print('Variable: %s'%expression)
	print('No reweight   KS:', ks_2samp_weighted(col_original, col_target, 
												 weights1=original_weights_test, weights2=w_target))
	print('GB Reweight   KS:', ks_2samp_weighted(col_original, col_target,
												 weights1=gb_weights_test, weights2=w_target))

def roc_auc_measurement(original,target,original_weights,folding_weights):
	data = numpy.concatenate([original, target])
	labels = numpy.array([0] * len(original) + [1] * len(target))
	weights = {}
	weights['original'] = original_weights
	weights['bdtreweighter'] = folding_weights
	for name, new_weights in weights.items():
		W = numpy.concatenate([new_weights / new_weights.sum() * len(target), [1] * len(target)])
		Xtr, Xts, Ytr, Yts, Wtr, Wts = train_test_split(data, labels, W, random_state=42, train_size=0.51)
		clf = GradientBoostingClassifier(subsample=0.3, n_estimators=30).fit(Xtr, Ytr, sample_weight=Wtr)
		print("ROC AUC in %s: "%name, roc_auc_score(Yts, clf.predict_proba(Xts)[:, 1], sample_weight=Wts))

