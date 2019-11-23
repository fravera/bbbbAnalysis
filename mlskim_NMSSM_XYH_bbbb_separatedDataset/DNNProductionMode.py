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
	trainer.GetConfusionMatrix(X,y,w,NNmodel,labels,0.4,'all_%s'%tag)
	## ROC AUC for train and test samples
	trainer.GetROCAUC(X_test,y_test,NNmodel,'test_%s'%tag)
	trainer.GetROCAUC(X_train,y_train,NNmodel,'train_%s'%tag)
	trainer.GetROCAUC(X,y,NNmodel,'all_%s'%tag)
	# Plots the output distribution for test sample
	trainer.GetNNDistributionComparison(X_train,y_train,X_test,y_test,NNmodel,True,tag)

def PrepareSample(sample,btagregion,eventcategory):
	ievents = data.root2pandas(sample,'bbbbTree')
	events  = selector.eventselection(ievents,btagregion,eventcategory)
	return events

#############ML CODE IS BELOW ######################
features = ['H1_pt', 'H2_pt','H1_eta', 'H2_eta','JJ_j1_pt','JJ_j2_pt','JJ_eta','h1h2_deltaEta','h1j1_deltaR','h1j2_deltaR','h2j1_deltaR','h2j2_deltaR','j1etaj2eta','costh_HH_b1_cm','costh_HH_b2_cm']
labels=['GGF-HH','VBF-HH']
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data
signal2016      = PrepareSample('outputskims/2016case1/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root','4b','preVBF')
background2016  = PrepareSample('outputskims/2016case1/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.root','4b','preVBF')
signal2017      = PrepareSample('outputskims/2017case1/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root','4b','preVBF')
background2017  = PrepareSample('outputskims/2017case1/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.root','4b','preVBF')
#NNDesign 2016 (case1)
parameters2016 = {'epochs': 50, 'batch_size': 600}
hiddenlayers2016=[100,100]
dropout2016=0.1
tag2016='DNN1_2016case1'
#NNDesign 2017 (case1)
parameters2017 = {'epochs': 50, 'batch_size': 200}
hiddenlayers2017=[20,20]
dropout2017=0.1
tag2017='DNN1_2017case1'
##RunTraining 2016
RunTraining(signal2016,background2016,features,hiddenlayers2016,dropout2016,parameters2016,tag2016)
RunTraining(signal2017,background2017,features,hiddenlayers2017,dropout2017,parameters2017,tag2017)