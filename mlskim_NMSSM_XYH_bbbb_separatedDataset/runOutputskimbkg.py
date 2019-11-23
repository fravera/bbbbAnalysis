import numpy 
import os
import root_numpy
import pandas
import glob
from root_numpy import root2array
from sklearn.preprocessing import StandardScaler
from numpy.lib.recfunctions import stack_arrays
from keras.models import load_model
from keras.models import model_from_json
from sklearn.externals.joblib import dump, load
#My modules
import modules.datatools as data
import modules.plotter as plotter
import modules.bdtreweighter as bdtreweighter
import modules.selections as selector
import modules.training as trainer


def TreeDevelopment(sample,fourthbtagscore,columns_input,case):
	columns = columns_input
	while 'bTagScaleFactor_central' in columns: columns.remove('bTagScaleFactor_central')
	while 'genWeight' in columns: columns.remove('genWeight')
	while 'PUWeight'  in columns:  columns.remove('PUWeight')
	skim   = data.root2pandas('inputskims/%s/SKIM_%s.root'%(case,sample),'bbbbTree', branches=columns)
	value  = numpy.ones(dtype='float64',shape=len(skim))
	skim['bTagScaleFactor_central'] = value
	skim['genWeight'] = value		 
	skim['PUWeight']  = value
	skim['MLWeight']  = value
	skim['Weight_110_GGF'] = value
	skim['Weight_210_GGF'] = value
	skim['Weight_110_VBF'] = value
	skim['Weight_210_VBF'] = value
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
		
#############ML CODE IS BELOW ######################
#Get data and create panda dataframes with specific variables, a.k.a. slicing the data 
variables     = ['HH_b1_pt','HH_b2_pt','HH_b3_pt','HH_b4_pt','JJ_j1_pt','JJ_j2_pt',
'HH_b1_eta','HH_b2_eta','HH_b3_eta','HH_b4_eta','JJ_j1_eta','JJ_j2_eta','JJ_j1_qgl','JJ_j2_qgl',
'HH_b1_deepCSV','HH_b2_deepCSV','HH_b3_deepCSV','HH_b4_deepCSV','JJ_j1_deepCSV','JJ_j2_deepCSV',
'H1_pt', 'H2_pt','H1_eta', 'H2_eta','H1_m','H2_m','HH_m','H1rand_m','H2rand_m','nJet','H1_bb_deltaR','H2_bb_deltaR','H1_bb_deltaPhi','H2_bb_deltaPhi','H1_bb_deltaEta','H2_bb_deltaEta',
'costh_HH_b1_cm','costh_HH_b2_cm','costh_JJ_j1_cm','costh_JJ_j2_cm','JJ_eta','JJ_pt','JJ_m','j1j2_deltaEta','h1h2_deltaEta',
'h1j1_deltaR','h1j2_deltaR','h2j1_deltaR','h2j2_deltaR','j1etaj2eta','JJ_j1_rawpt','JJ_j2_rawpt','JJ_j1_puId','JJ_j1_jetId','JJ_j2_puId','JJ_j2_jetId',
'BDT1','BDT2','BDT3','VBFEvent','HH_btag_b4_deepCSV','XS','bTagScaleFactor_central','genWeight','PUWeight']
bkg2016     = ['BKGSTUDIES2016']
bkg2017     = ['BKGSTUDIES2017']
#datasets
samplesbkg = ['GluGluToHHTo4B_node_SM_13TeV-madgraph_3000','GluGluToHHTo4B_node_SM_13TeV-madgraph_6000',
'GluGluToHHTo4B_node_SM_13TeV-madgraph_9000','VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_8000',
'VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_10000','VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_12000']
#Make microskims 2016
MakeOutputSkims(samplesbkg, 0.6324,variables,bkg2016)
#Make microskims 2017
MakeOutputSkims(samplesbkg, 0.4941,variables,bkg2017)
