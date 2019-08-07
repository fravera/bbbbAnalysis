import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays


def eventbtagregion(data,btagregion):
	#Select b-tagging region
	if btagregion   == '4b':
			selected = data[  data.nBtag==4 ]
			rejected = data[  data.nBtag!=4 ]
	elif btagregion == '3b': 
			selected = data[  data.nBtag==3 ]
			rejected = data[  data.nBtag!=3 ]
	else:
			selected = data
			rejected = pandas.DataFrame()
			print "[INFO] The b-tagging region was not specify, the data is b-tagging inclusive! Then rejected dataframe is empty!" 
	return  selected,rejected	
	
def eventcategory(data,category):
	if category   == 'VBF':
		 selected = data[  ((data.BDT1 >=0) & (data.JJ_j1_jetId>0) & (data.JJ_j2_jetId>0) & (data.JJ_j1_puId>0) & (data.JJ_j2_puId>0)) ]
		 rejected = data[ ~((data.BDT1 >=0) & (data.JJ_j1_jetId>0) & (data.JJ_j2_jetId>0) & (data.JJ_j1_puId>0) & (data.JJ_j2_puId>0)) ]
	elif category == 'GGF':
		 selected = data[ ~((data.BDT1 >=0) & (data.JJ_j1_jetId>0) & (data.JJ_j2_jetId>0) & (data.JJ_j1_puId>0) & (data.JJ_j2_puId>0)) ]   	
		 rejected = data[  ((data.BDT1 >=0) & (data.JJ_j1_jetId>0) & (data.JJ_j2_jetId>0) & (data.JJ_j1_puId>0) & (data.JJ_j2_puId>0)) ]
	else:
		 selected = data
		 rejected = pandas.DataFrame()
		 print "[INFO] The hh-category was not specify, the data is category inclusive! Then rejected dataframe is empty!"
	return selected,rejected

def eventmassregion(data,massregion,validation=False):
	# 2016 is (115,)
	if validation:
		center1 = 205
		center2 = 190
	else:
		center1 = 120
		center2 = 110    	
	radius1  = 45
	radius2  = 60

	if   massregion == 'CR':
		selected = data[ (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1) & (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) <= radius2) ]
		rejected = data[ (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1) & (((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >  radius2) |  (((data.H1_m-center1)**2 +(data.H2_m-center2)**2)**(0.5) < radius1) ] 
	elif massregion == 'SR':
		selected = data[((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) <  radius1 ]
		rejected = data[((data.H1_m-center1)**2+(data.H2_m-center2)**2)**(0.5) >= radius1 ]
	else:
		selected = data
		rejected = pandas.DataFrame()
		print "[INFO] The mass region was not specify, the data is mass region inclusive! Then rejected dataframe is empty!"
	return selected,rejected

def eventselection(data,btagregion=None,category=None,massregion=None,validation=None):
	databtagregionselected,databtagregionrejected = eventbtagregion(data,btagregion)
	datacategoryselected,datacategoryrejected     = eventcategory(databtagregionselected,category) 
	datamassregionselected,datamassregionrejected = eventmassregion(datacategoryselected,massregion,validation) 
	dataselected = datamassregionselected
	datarejected = pandas.concat((databtagregionrejected,datacategoryrejected,datamassregionrejected), ignore_index=True)
	print "[INFO] DATAFRAME SLICING REPORT BELOW" 
	print "   -Number of events in dataset (before) = ",len(data) 
	print "   -Number of selected events            = ",len(dataselected)  
	print "   -Number of rejected events            = ",len(datarejected)
	print "   -Number of events in dataset (after)  = ",int(len(dataselected)+len(datarejected))
	return dataselected,datarejected