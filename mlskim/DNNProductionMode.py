import numpy 
import os
import root_numpy
import pandas
import glob
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector
import modules.training as trainer


def RunTraining(signal,background,features,hiddenlayers,dropout,design,tag):
	##Turn Data into ML-Compatible Inputs (X=[Events, Features],w=[Events, Weights],y=[Events, TruthLabels])
	X,y,w,X_train, X_test, y_train, y_test, w_train, w_test = trainer.preparedatasetfortraining(signal,background,features,tag)
	#Neural network architecture and model
	model = trainer.buildNNmodel(X_train,hiddenlayers,dropout,tag)
	NNmodel,TrainingHistory = trainer.trainNNmodel(X_train,y_train,w_train,X_test,y_test,w_test,model,design,tag)
	# -- plot training history
	trainer.historyNNmodel(TrainingHistory,tag)
	## Confusion Matrix for train and test
	trainer.GetConfusionMatrix(X,y,w,NNmodel,labels,0.5,'all_%s'%tag)
	## ROC AUC for train and test samples
	trainer.GetROCAUC(X_test,y_test,NNmodel,'test_%s'%tag)
	trainer.GetROCAUC(X_train,y_train,NNmodel,'train_%s'%tag)
	trainer.GetROCAUC(X,y,NNmodel,'all_%s'%tag)
	# Plots the output distribution for test sample
	trainer.GetNNDistributionComparison(X_train,y_train,X_test,y_test,NNmodel,True,tag)

def PrepareSample(sample,btagregion,eventcategory):
	ievents = data.root2pandas(sample,'bbbbTree')
	selectedevents,rejectedevents  = selector.eventselection(ievents,btagregion,eventcategory)
	return selectedevents

#############ML CODE IS BELOW ######################
features = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','JJ_j1_pt','JJ_j2_pt','JJ_eta','h1h2_deltaEta','h1j1_deltaR','h1j2_deltaR','h2j1_deltaR','h2j2_deltaR','j1etaj2eta','costh_HH_b1_cm','costh_HH_b2_cm']
labels=['GGF-HH','VBF-HH']
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data
signal2016     = PrepareSample('outputskims/New2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.root','4b','preVBF')
background2016 = PrepareSample('outputskims/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root','4b','preVBF')
#NNDesign 2016 (case1)
parameters2016 = {'epochs': 50, 'batch_size': 50}
hiddenlayers2016=[30,30]
dropout2016=0.05
tag2016='DNN1_2016'
##RunTraining 2016
RunTraining(signal2016,background2016,features,hiddenlayers2016,dropout2016,parameters2016,tag2016)