import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays

	
def eventMassRegion(data,massregion,selection,validation=False):
	# 2016 is (115,)
	trueCenter = 120
	if validation:
		center = 220
	else:
		center = trueCenter
	signalRegionHalfSize  = 20
	# original
	controlRegionHalfSize = 40
	# wide CR
	# controlRegionHalfSize = 50
	# narrow CR
	# controlRegionHalfSize = 30

	if   massregion == 'CR':
		# selected = data[ (data.H2_m > (trueCenter+signalRegionHalfSize))  (data.H2_m < (trueCenter - signalRegionHalfSize)) ]
		# & ((data.H1_m >= (center-controlRegionHalfSize) &  data.H1_m <= (center-signalRegionHalfSize)) | (data.H1_m >= (center+signalRegionHalfSize)  & data.H1_m <= (center+controlRegionHalfSize))) ]
		if validation:
			if selection == "fullRegion":
				selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) | ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			# original without trigger matching
			if selection == "fullRegionWithoutTriggerMatching":
				selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize))) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) | ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			# CR high M_H1
			if selection == "CR_high_M_H1":
				selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			# CR low M_H1
			if selection == "CR_low_M_H1":
				selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) ) ]
			# CR high M_H2
			if selection == "CR_high_M_H2":
				selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) | ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			# CR low M_H2
			if selection == "CR_low_M_H2":
				selected = data[ ((data.H2_m < (trueCenter-signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) | ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			# rejected = data[ ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & (   (data.H1_m <= (center-controlRegionHalfSize)) |  ( (data.H1_m >= (center-signalRegionHalfSize))   &   (data.H1_m <= (center+signalRegionHalfSize)) ) | (data.H1_m >= (center+controlRegionHalfSize) ) )   ]
			rejected = data[ ( (data.H1_m <= (center-controlRegionHalfSize)) |  ( (data.H1_m >= (center-signalRegionHalfSize))   &   (data.H1_m <= (center+signalRegionHalfSize)) ) | (data.H1_m >= (center+controlRegionHalfSize) ) )   ]
			# rejected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & (   (data.H1_m <= (center-controlRegionHalfSize)) |  ( (data.H1_m >= (center-signalRegionHalfSize))   &   (data.H1_m <= (center+signalRegionHalfSize)) ) | (data.H1_m >= (center+controlRegionHalfSize) ) )   ]
		else:
			# selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize))) & ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) | ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			selected = data[ ((data.H2_m > (trueCenter+signalRegionHalfSize)) | (data.H2_m < (trueCenter-signalRegionHalfSize)))  & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & ( ( (data.H1_m >= (center-controlRegionHalfSize)) &    (data.H1_m <= (center-signalRegionHalfSize)) ) | ( (data.H1_m >= (center+signalRegionHalfSize))   & (data.H1_m <= (center+controlRegionHalfSize) ) ) ) ]
			# rejected = data[ ((data.HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched > 0) | (data.HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)) & (data.H1_b1_pt > 30) & (data.H1_b2_pt > 30) & (data.H2_b1_pt > 30) & (data.H2_b2_pt > 30) & (data.H1_b1_ptRegressed > 30) & (data.H1_b2_ptRegressed > 30) & (data.H2_b1_ptRegressed > 30) & (data.H2_b2_ptRegressed > 30) & (data.H1_b1_eta > -2.4) & (data.H1_b1_eta < 2.4) & (data.H1_b2_eta > -2.4) & (data.H1_b2_eta < 2.4) & (data.H2_b1_eta > -2.4) & (data.H2_b1_eta < 2.4) & (data.H2_b2_eta > -2.4) & (data.H2_b2_eta < 2.4) & (   (data.H1_m <= (center-controlRegionHalfSize)) |  ( (data.H1_m >= (center-signalRegionHalfSize))   &   (data.H1_m <= (center+signalRegionHalfSize)) ) | (data.H1_m >= (center+controlRegionHalfSize) ) )   ]
			rejected = data[ (   (data.H1_m <= (center-controlRegionHalfSize)) |  ( (data.H1_m >= (center-signalRegionHalfSize))   &   (data.H1_m <= (center+signalRegionHalfSize)) ) | (data.H1_m >= (center+controlRegionHalfSize) ) )   ]

	elif massregion == 'SR':
		selected = data[ ((data.H2_m > trueCenter+signalRegionHalfSize) | (data.H2_m < trueCenter-signalRegionHalfSize)) & ((data.H1_m >= (center-signalRegionHalfSize)) & (data.H1_m <= (center+signalRegionHalfSize))) ]
		rejected = pandas.DataFrame()
	else:
		selected = data
		rejected = pandas.DataFrame()
		print "[INFO] The mass region was not specify, the data is mass region inclusive! Then rejected dataframe is empty!"
	return selected, rejected

def eventSelection(data,selection,massRegion=None,validation=None):
	dataSelected, dataRejected = eventMassRegion(data,massRegion,selection,validation)
	print "[INFO] DATAFRAME SLICING REPORT BELOW" 
	print "   -Number of events in dataset (before) = ",len(data) 
	print "   -Number of selected events            = ",len(dataSelected)  
	print "   -Number of rejected events            = ",len(dataRejected)
	print "   -Number of events in dataset (after)  = ",int(len(dataSelected)+len(dataRejected))
	return dataSelected, dataRejected