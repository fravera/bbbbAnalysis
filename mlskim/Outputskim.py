import numpy 
import os
import root_numpy
import pandas
import glob
import ast
import argparse
import sys
import copy
from  ConfigParser import *
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
from sklearn.externals.joblib import dump, load
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def TreeDevelopment(sample,fourthbtagscore,columns_input,case):
	#Correct acccording to the type of data
	columns = copy.copy(columns_input)
	if "GGHH4B" not in sample: columns.remove('HH_reweight')
	if "Data" in sample:
		 columns.remove('bTagScaleFactor_central')
		 columns.remove('genWeight')
		 columns.remove('PUWeight') 
    #Common branches
	skim  = data.root2pandas('inputskims/%s/SKIM_%s.root'%(case,sample),'bbbbTree', branches=columns)
	value  = numpy.ones(dtype='float64',shape=len(skim))
	skim['Weight_110_GGF'] = value
	skim['Weight_210_GGF'] = value
	skim['Weight_110_VBF'] = value
	skim['Weight_210_VBF'] = value		 	
	skim['Weight_110_GGF_tfactor'] = value
	skim['Weight_210_GGF_tfactor'] = value
	skim['Weight_110_VBF_tfactor'] = value
	skim['Weight_210_VBF_tfactor'] = value	
	##Branches for data only
	if "Data" in sample:
		 skim['bTagScaleFactor_central'] = value
		 skim['genWeight'] = value		 
		 skim['PUWeight']  = value
		 skim['XS'] = value		     	 
	#Create btagging flag for convenience
	b4 = skim.HH_btag_b4_deepCSV >  fourthbtagscore
	b3 = skim.HH_btag_b4_deepCSV <= fourthbtagscore
	del skim['HH_btag_b4_deepCSV']
	skim['nBtag'] = numpy.select([b4, b3], [4, 3], default=0)
	return skim

def MakeOutputSkims(samples,fourthbtagscore,columns,cases):
	for sample in samples:
	   for case in cases:
		#Read Columns
		os.system('mkdir outputskims')
		os.system('mkdir outputskims/%s/'%case)
		#Create eventweight = Lumi*XS*bTagSF/TotalMCEvents
		skim = TreeDevelopment(sample,fourthbtagscore,columns,case) 
		#Save it in a root file (bbbbtree)
		data.pandas2root(skim,'bbbbTree','outputskims/%s/SKIM_%s_tree.root'%(case,sample)  )
		#Save it in a root file (eff_histo) 
		data.roothist2root(sample,case,'eff_histo','outputskims/%s/SKIM_%s_hist.root'%(case,sample))
		#Merge tree and efficiency histos in root file
		data.mergedata2root('outputskims/%s/SKIM_%s_tree.root'%(case,sample), 'outputskims/%s/SKIM_%s_hist.root'%(case,sample), 'outputskims/%s/SKIM_%s.root'%(case,sample))
		print '[INFO] Saving output skim for',sample,' in ',case
		
#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
for x in range(len(directory)):
  print "      *",directory[x]
samples     = ast.literal_eval(cfgparser.get("configuration","samples"))
print "    -The list of samples:"
for x in range(len(samples)):
  print "      *",samples[x]
variables   = ast.literal_eval(cfgparser.get("configuration","variables"))
print "    -The list of variables:"
for x in range(len(variables)):
  print "      *",variables[x]
btaggingcut = ast.literal_eval(cfgparser.get("configuration","btagcut"))
##########Make microskims
print "[INFO] Making skims . . . "
MakeOutputSkims(samples,btaggingcut,variables,directory)