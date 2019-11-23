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

def TreeDevelopment(sample,columns_input,case):
	#Correct acccording to the type of data
	columns = copy.copy(columns_input)
	#Common branches
	skim  = data.root2pandas(['inputskims/%s/SKIM_%s.root'%(case,sample)],'bbbbTree', branches=columns)
	value  = numpy.ones(dtype='float64',shape=len(skim))
	skim['Weight_forBackground'] = value
	skim['Weight_forBackground_tfactor'] = value
	##Branches for data only
	if "Data" in sample:
		 skim['bTagScaleFactor_central'] = value
		 skim['genWeight'] = value		 
		 skim['PUWeight']  = value
		 skim['XS'] = value		     	 
	return skim

def MakeOutputSkims(samples,columns,cases):
	for sample in samples:
	   for case in cases:
		#Read Columns
		os.system('mkdir outputskims')
		os.system('mkdir outputskims/%s/'%case)
		#Create eventweight = Lumi*XS*bTagSF/TotalMCEvents
		skim = TreeDevelopment(sample,columns,case) 
		#Save it in a root file (bbbbtree)
		data.pandas2root(skim,'bbbbTree','outputskims/%s/SKIM_%s_tree.root'%(case,sample)  )
		#Save it in a root file (eff_histo) 
		data.roothist2root(case,sample,'eff_histo','outputskims/%s/SKIM_%s_hist.root'%(case,sample),True)
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
##########Make microskims
print "[INFO] Making skims . . . "
MakeOutputSkims(samples,variables,directory)