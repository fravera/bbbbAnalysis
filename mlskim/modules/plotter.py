import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
from root_numpy import root2array
from matplotlib import pyplot as plt
from numpy.lib.recfunctions import stack_arrays
from hep_ml.metrics_utils import ks_2samp_weighted
from scipy import stats

def Draw1DHistosComparison(original, target, variables, original_weights, norm, tag):
	#Create folder for plots
	os.system("mkdir myplots")
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': 10, 'density': True, 'alpha': 0.5}
	else:
	  hist_settings = {'bins': 10, 'density': None, 'alpha': 0.5}
	#Create figure on matplotlib
	matplotlib.rcParams.update({'font.size': 35})
	plt.figure(figsize=[80, 60], dpi=50)
	i = 0
	for id, column in enumerate(variables, 1):
		xlim = numpy.percentile(numpy.hstack([target[column]]), [0.01, 99.99])
		plt.subplot(4, 3, id)
		plt.hist(original[column],label='Bkg. Model', weights=original_weights, range=xlim, **hist_settings)
		plt.hist(target[column],  label='4 btag (Target)', range=xlim, **hist_settings)
		plt.legend(loc='best')
		plt.title(column,fontsize=40)
		i+=1
	plt.savefig("myplots/distibutions_%s.png"%tag)
	plt.close('all')

def DrawDNNScoreComparison(yhat,y, norm, tag):
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': 20, 'density': True, 'alpha': 0.5}
	else:
	  hist_settings = {'bins': 20, 'density': None, 'alpha': 0.5}
	#Create dataframes
	datayhat= numpy.array(yhat)
	datay= numpy.array(y)
	datatrain= pandas.DataFrame({'DNN':datayhat[:,0]})
	datatrain['Class'] = pandas.DataFrame(datay)  
	datatrain_signal=datatrain[datatrain.Class==1]
	datatrain_background=datatrain[datatrain.Class==0]
	#Create figure on matplotlib
	fig, (ax1, ax2) = plt.subplots(nrows=2)

	matplotlib.rcParams.update({'font.size': 20})
	plt.figure(figsize=(7,7))
	xlim = numpy.percentile(numpy.hstack(datatrain_signal['DNN']), [0.01, 99.99])
	plt.hist(datatrain_signal['DNN'],label='Signal',  range=xlim, **hist_settings)
	plt.hist(datatrain_background['DNN'],  label='Background', range=xlim, **hist_settings)
	plt.legend(loc='best')
	plt.title('DNN output (%s)'%tag,fontsize=20) 
	plt.savefig("myplots/nndistibutions_%s.png"%tag)    

def DrawDNNScore(yhat,y, norm, tag):
	#Normalize or not?
	if norm is True:
	  hist_settings = {'bins': 10, 'density': True, 'alpha': 0.5}
	else:
	  hist_settings = {'bins': 10, 'density': None, 'alpha': 0.5}
	#Create dataframes
	datayhat= numpy.array(yhat)
	datay= numpy.array(y)
	datatrain= pandas.DataFrame({'DNN':datayhat[:,0]})
	datatrain['Class'] = pandas.DataFrame(datay)  
	datatrain_signal=datatrain[datatrain.Class==1]
	datatrain_background=datatrain[datatrain.Class==0]
	#Create figure on matplotlib
	matplotlib.rcParams.update({'font.size': 20})
	plt.figure(figsize=(7,7))
	xlim = numpy.percentile(numpy.hstack(datatrain_signal['DNN']), [0.01, 99.99])
	plt.hist(datatrain_signal['DNN'],label='Signal',  range=xlim, **hist_settings)
	plt.hist(datatrain_background['DNN'],  label='Background', range=xlim, **hist_settings)
	plt.legend(loc='best')
	plt.title('DNN output (%s)'%tag,fontsize=20) 
	plt.savefig("myplots/nndistibution_%s.png"%tag) 



def CreateDataFrameForPlot(scores,labels):
	datayhat= numpy.array(scores)
	datay= numpy.array(labels)
	data= pandas.DataFrame({'DNN':datayhat[:,0]})
	data['Class'] = pandas.DataFrame(datay)  
	data_signal=data[data.Class==1]
	data_background=data[data.Class==0]
	return data_signal,data_background

def DrawDNNScoreComparison(yhat_train,y_train,yhat_test,y_test, norm, tag):
	#Create dataframes for train and test 
	datatrain_signal,datatrain_background = CreateDataFrameForPlot(yhat_train,y_train)
	datatest_signal,datatest_background   = CreateDataFrameForPlot(yhat_test,y_test)
	#Format of the histograms
	nbins = 40.0
	binwidth = float(1/nbins)
	dnnbin   = [                     (x*binwidth) for x in range(0,int(nbins+1))] 
	ratiobin = [ float(binwidth)/2 + (x*binwidth) for x in range(0,int(nbins)  )]
	print dnnbin
	print ratiobin 
	#KS TEST
	sig_d,sig_p = stats.ks_2samp(datatrain_signal['DNN'], datatest_signal['DNN'])
	bkg_d,bkg_p = stats.ks_2samp(datatrain_background['DNN'], datatest_background['DNN'])
	#Create figure on matplotlib
	matplotlib.rcParams.update({'font.size': 15})
	fig = plt.figure(1, figsize=(15, 15))
	ax1 = plt.subplot2grid((3, 1), (0, 0), rowspan=2)
	ax2 = plt.subplot2grid((3, 1), (2, 0))
	plot1 = ax1.hist(datatrain_signal['DNN'],label='Sig(train+val)', range=[0,1], color='blue',  bins=dnnbin, density=True, alpha=0.5)
	plot2 = ax1.hist(datatest_signal['DNN'] ,label='Sig(test)' , range=[0,1], color='darkblue', linewidth=2, histtype="step", bins=dnnbin, density=True, alpha=0.4)
	plot3 = ax1.hist(datatrain_background['DNN'],label='Bkg(train+val)', range=[0,1], color='red',  bins=dnnbin, density=True, alpha=0.5)
	plot4 = ax1.hist(datatest_background['DNN'] ,label='Bkg(test)' , range=[0,1], color='darkred', linewidth=2, histtype="step", bins=dnnbin, density=True, alpha=0.4)
	plot5 = ax1.text(0.35, 0.01,'P(KS-test) sig(bkg) = %.2f(%.2f)'%(sig_p,bkg_p),fontsize=10)
	ax1.set_ylabel("A. U.",fontsize=15)
	ax1.set_title('DNN output (%s)'%tag,fontsize=15) 
	ax1.legend(loc='upper center')
	plots5 = ax2.bar(ratiobin, height=( (plot2[0]-plot1[0])/plot1[0]), alpha=0.4, edgecolor='darkblue', linewidth=1.5, color='blue',width = binwidth)
	plots6 = ax2.bar(ratiobin, height=( (plot4[0]-plot3[0])/plot3[0]), alpha=0.4, edgecolor='darkred', linewidth=1.5, color='red',width = binwidth)
	ax2.set_ylim([-0.5, 0.5])
	ax2.axhline(y=0,ls='--',c='k',zorder=11,lw=1) 
	ax2.axhline(y=0.2,ls='--',c='k',zorder=11,lw=1) 
	ax2.axhline(y=-0.2,ls='--',c='k',zorder=11,lw=1) 
	ax2.set_ylabel('(Test-Model)/Model',fontsize=15)
	ax2.set_xlabel("DNN",fontsize=20)
	plt.savefig("myplots/nndistibutioncomparison_%s.png"%tag) 


def Draw2DHisto(data,var1,var2,x1,x2,y1,y2,tag):
	plt.figure(figsize=(10,10)) # square canvas
	_ = plt.hist2d(data['%s'%var1], data['%s'%var2], range=[[x1, x2], [y1, y2]], bins=100, cmap='viridis', norm=LogNorm())
	plt.xlabel("%s"%var1, fontsize=20) 
	plt.ylabel("%s"%var2, fontsize=20)
	plt.title("%s"%tag,fontsize=20)
	_ = plt.colorbar() 
	plt.savefig("myplots/mh1mh2plane_%s.png"%tag) 