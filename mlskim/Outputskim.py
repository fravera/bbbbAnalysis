import os
import root_numpy
import pandas
import glob
import ast
import argparse
import sys
import copy
import numpy
from  ConfigParser import *
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
from sklearn.externals.joblib import dump, load
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector

def TreeDevelopment(sample,filelist,columns_input,case,directory,lepveto):
	#Correct acccording to the type of data
	columns = copy.copy(columns_input)
	if "GGHH4B" not in sample: columns.remove('HH_reweight')
	if "Data" in sample:
		 columns.remove('bTagScaleFactor_central')
		 columns.remove('genWeight')
		 columns.remove('PUWeight') 
	#Common branches
	skim   = data.root2pandas(filelist,'bbbbTree', branches=columns)
	value  = numpy.ones(dtype='float64',shape=len(skim))
	skim['Weight_ValGGF'] = value
	skim['Weight_AnaGGF'] = value
	skim['Weight_ValVBF'] = value
	skim['Weight_AnaVBF'] = value		 	
	skim['Weight_ValGGF_tfactor'] = value
	skim['Weight_AnaGGF_tfactor'] = value
	skim['Weight_ValVBF_tfactor'] = value
	skim['Weight_AnaVBF_tfactor'] = value
	skim['Weight_MVA'] = value
	##Branches for data only
	if "Data" in sample:
		 skim['bTagScaleFactor_central'] = value
		 skim['genWeight'] = value		 
		 skim['PUWeight']  = value
		 skim['XS'] = value		     	 
	##Veto on leptons
	if lepveto==True:
		skim = skim[skim.IsolatedElectron_Multiplicity==0]
		skim = skim[skim.IsolatedMuon_Multiplicity==0]
	##Create two new columns with mHH (one for SM(1), other for BSM(2) )
	skim['HH_m_1'] = skim['HH_m']
	skim['HH_m_2'] = skim['HH_m']
	return skim
def MakeOutputSkims(samples,columns,case,directory,lepveto):
	os.system('mkdir outputskims')
	os.system('mkdir outputskims/%s/'%case)
	os.system('mkdir outputskims/%s/%s'%(case,directory))
	for sample in samples:
		#Create a list of files and dataframe (but edit eos address accordingly)
		filelist = data.GetFileList('/eos/uscms/store/user/guerrero/bbbb_ntuples/%s/%s/SKIM_%s/output/*.root'%(case,directory,sample),'bbbbTree')
		skim     = TreeDevelopment(sample,filelist,columns,case,directory,lepveto) 
		#Save it in a root file (bbbbtree)
		data.pandas2root(skim,'bbbbTree','outputskims/%s/%s/SKIM_%s_tree.root'%(case,directory,sample)  )
		#Save it in a root file (eff_histo) 
		data.roothist2root(filelist,'eff_histo','outputskims/%s/%s/SKIM_%s_hist.root'%(case,directory,sample))
		#Merge tree and efficiency histos in root file
		data.mergedata2root('outputskims/%s/%s/SKIM_%s_tree.root'%(case,directory,sample), 'outputskims/%s/%s/SKIM_%s_hist.root'%(case,directory,sample), 'outputskims/%s/%s/SKIM_%s.root'%(case,directory,sample))
		print '[INFO] Saving output skim for',sample,' in ',case
#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config' ,  dest='cfgfile',  help='Name of config file',  required = True)
parser.add_argument('--casename',    dest='case',  help='Name of ntuples case', required = True)
parser.add_argument('--lepveto',    dest='lepveto', action='store_true', help='Apply Lepton veto')
parser.add_argument('--no-lepveto', dest='lepveto', action='store_false',help='Do not apply Lepton veto')
parser.set_defaults(lepveto=False)
args = parser.parse_args()
configfilename = args.cfgfile
casename       = args.case
lepveto        = args.lepveto
###########Read Config file
print "[INFO] Reading skim configuration file . . ."
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)
##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
print "    -The directory:"
print "      *",directory
samples     = ast.literal_eval(cfgparser.get("configuration","samples"))
print "    -The list of samples:"
for x in range(len(samples)):
  print "      *",samples[x]
variables   = ast.literal_eval(cfgparser.get("configuration","variables"))
print "    -The list of variables:"
for x in range(len(variables)):
  print "      *",variables[x]
print "    -The case: ",casename
print "[INFO] The lepton veto is applied: ",lepveto
##########Make microskims
print "[INFO] Making skims . . . "
MakeOutputSkims(samples,variables,casename,directory,lepveto)