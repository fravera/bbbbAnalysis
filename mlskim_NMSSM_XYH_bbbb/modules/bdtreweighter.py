import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
from scipy import interp
from hep_ml import reweight
from root_numpy import root2array
from matplotlib import pyplot as plt
from numpy.lib.recfunctions import stack_arrays
from sklearn.model_selection import train_test_split
from hep_ml.metrics_utils import ks_2samp_weighted
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
from sklearn.metrics import roc_curve,auc
from matplotlib.colors import LogNorm
from sklearn.utils.class_weight import compute_sample_weight
from sklearn.model_selection import StratifiedKFold

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


def roc_auc_study(data,labels,weights,args,outputDirectory,tag):
		cv = StratifiedKFold(n_splits=2,shuffle=True,random_state=args[2])
		tprs = []
		aucs = []
		mean_fpr = numpy.linspace(0, 1, 100)
		matplotlib.rcParams.update({'font.size': 8})
		plt.figure(figsize=[200, 100], dpi=50)
		fig, (ax1, ax2) = plt.subplots(1, 2)
		fig.subplots_adjust(wspace=0.2)

		i = 0
		for train, test in cv.split(data, labels, weights):
			classifier = GradientBoostingClassifier(n_estimators=args[0],subsample=args[1],random_state=args[2])
			probas_    = classifier.fit(data[train], labels[train],sample_weight=weights[train]).predict_proba(data[test])
			# Compute ROC curve and area the curve
			fpr, tpr, thresholds = roc_curve(labels[test], probas_[:, 1], sample_weight=weights[test])
			#Draw Classifier Output
			datatest           =pandas.DataFrame({'GBscore':probas_[:,1]})
			datatest['Weights']=pandas.DataFrame(weights[test])
			datatest['Class']  =pandas.DataFrame(labels[test])  
			datatest_signal    =datatest[datatest.Class==0]
			datatest_background=datatest[datatest.Class==1]
			hist_settings = {'bins': 20, 'density': True, 'alpha': 0.3}
			ax2.hist(datatest_signal['GBscore']     ,weights=datatest_signal['Weights'],    label='Model (fold %i)'%i,range=[0,1],histtype="step",**hist_settings)
			ax2.hist(datatest_background['GBscore'] ,weights=datatest_background['Weights'],label='Target(fold %i)'%i,range=[0,1],**hist_settings)
			#Draw ROC Curves
			tprs.append(interp(mean_fpr, fpr, tpr))
			tprs[-1][0] = 0.0
			roc_auc = auc(fpr, tpr)
			aucs.append(roc_auc)
			ax1.plot(fpr, tpr, lw=1, alpha=0.3,label='ROC fold %d (AUC = %0.2f)' % (i, roc_auc))
			i += 1
		mean_tpr = numpy.mean(tprs, axis=0)
		mean_tpr[-1] = 1.0
		mean_auc = auc(mean_fpr, mean_tpr)
		std_auc = numpy.std(aucs)
		print "  **Mean ROC (AUC) in %s = "%tag, mean_auc	
		ax1.plot([0, 1], [0, 1], linestyle='--', lw=2, color='r',label='Chance', alpha=.8)    
		ax1.plot(mean_fpr, mean_tpr, color='b',label=r'Mean ROC (AUC = %0.2f $\pm$ %0.2f)' % (mean_auc, std_auc),lw=2, alpha=.8)  	       
		std_tpr = numpy.std(tprs, axis=0)
		tprs_upper = numpy.minimum(mean_tpr + std_tpr, 1)
		tprs_lower = numpy.maximum(mean_tpr - std_tpr, 0)
		ax1.fill_between(mean_fpr, tprs_lower, tprs_upper, color='grey', alpha=.2,label=r'$\pm$ 1 std. dev.')        
		ax1.set_xlim([-0.05, 1.05])
		ax1.set_ylim([-0.05, 1.05])
		ax1.set_xlabel('False Positive Rate')
		ax1.set_ylabel('True Positive Rate')
		ax1.set_title('ROC Curve')
		ax1.legend(loc="lower right",fontsize="small")
		ax2.set_xlabel('Discriminant score')
		ax2.set_ylabel('Normalized Units')
		ax2.set_title('')
		ax2.legend(loc="lower right",fontsize="small")
		plt.savefig("%s/roc_curve_%s.png"%(outputDirectory,tag))


def discrimination_test(original,target,weights,args,outputDirectory,name):
	data   = numpy.concatenate([original, target])
	labels = numpy.array([0] * len(original) + [1] * len(target))
	w      = numpy.concatenate([weights / weights.sum() * len(target), [1] * len(target)])
	roc_auc_study(data,labels,w,args,outputDirectory,name)

def ks_comparison(variables,ks_noweight,ks_weight):
	print "[INFO] Runninng KS-test on each variable to define the bdt-reweighter paramaters:"
	mean=0
	for i in range(0, len(variables) ):
		ratio = ks_weight[i] / ks_noweight[i] 
		print "   -KS distance ratio in %s (after/before) = (%0.3f/%0.3f)  = %0.3f"%(variables[i],ks_weight[i],ks_noweight[i], ratio)
		mean+=ratio 
	print "                     **The average KS distance ratio = %0.3f"%(mean/len(variables))


def ks_test(original, target, variables, original_weights):
	ksresults=[] ##original_weights
	i = 0
	for id, column in enumerate(variables, 1):

		ks =  ks_2samp_weighted(original[column], target[column], weights1=original_weights, weights2=numpy.ones(len(target), dtype=int))
		ksresults.append(ks)
		i+=1
	return ksresults 