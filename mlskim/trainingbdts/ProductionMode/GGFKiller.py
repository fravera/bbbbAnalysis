#!/usr/bin/env python

# Select Tensorflow as backend for Keras
from os import environ
import os
import argparse 
#environ['KERAS_BACKEND'] = 'tensorflow'
# Set architecture of system (AVX instruction set is not supported on SWAN)
#environ['THEANO_FLAGS'] = 'gcc.cxxflags=-march=corei7'
from ROOT import TMVA, TFile, TTree, TCut,TChain
#from subprocess import call
from os.path import isfile

def  RunTraining(dataset,optimization):
	 # Setup TMVA
	 TMVA.PyMethodBase.PyInitialize()
	 output = TFile.Open('GGFKiller_%s_%s.root'%(dataset,optimization), 'RECREATE')
	 factory = TMVA.Factory('TMVAClassification%s%s'%(dataset,optimization), output,
			 '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
	 #Locate and add data files
	 file_GGF_HH_2016  = "../FullNtuples/New2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_PM.root"
	 file_VBF_HH_2016  = "../FullNtuples/New2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PM.root"
	 file_VBF2_HH_2016 = "../FullNtuples/New2016/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PM.root"
	 file_GGF_HH_2017  = "../FullNtuples/New2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_PM.root"
	 file_VBF_HH_2017  = "../FullNtuples/New2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_PM.root"
	 file_VBF2_HH_2017 = "../FullNtuples/New2017/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph_PM.root"
	 file_GGF_HH_2018  = "../FullNtuples/New2018/SKIM_GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8_PM.root"
	 file_VBF_HH_2018  = "../FullNtuples/New2018/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8_PM.root"
	 file_VBF2_HH_2018 = "../FullNtuples/New2018/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_TuneCP5_PSWeights_13TeV-madgraph-pythia8_PM.root"
	 #Add the data files
	 ch_sig = TChain("bbbbTree")
	 ch_bkg = TChain("bbbbTree")
	 #AddfILES 
	 if optimization=="SM":
		 seed=2020      
		 if dataset=='2016':
			ch_sig.AddFile(file_VBF_HH_2016)
			ch_bkg.AddFile(file_GGF_HH_2016)
		 elif dataset=='2017':
			ch_sig.AddFile(file_VBF_HH_2017)
			ch_bkg.AddFile(file_GGF_HH_2017)
		 elif dataset=='2018':
			ch_sig.AddFile(file_VBF_HH_2018)
			ch_bkg.AddFile(file_GGF_HH_2018)
		 else:
			ch_sig.AddFile(file_VBF_HH_2016)
			ch_bkg.AddFile(file_GGF_HH_2016)
			ch_sig.AddFile(file_VBF_HH_2017)
			ch_bkg.AddFile(file_GGF_HH_2017)	       	    	       
			ch_sig.AddFile(file_VBF_HH_2018)
			ch_bkg.AddFile(file_GGF_HH_2018)
	 else: 
		 if dataset=='2016':
			seed=2017
			ch_sig.AddFile(file_VBF2_HH_2016)
			ch_bkg.AddFile(file_GGF_HH_2016)
		 elif dataset=='2017':
			seed=2020
			ch_sig.AddFile(file_VBF2_HH_2017)
			ch_bkg.AddFile(file_GGF_HH_2017)
		 elif dataset=='2018':
			seed=2021
			ch_sig.AddFile(file_VBF2_HH_2018)
			ch_bkg.AddFile(file_GGF_HH_2018)
		 else:
			ch_sig.AddFile(file_VBF2_HH_2016)
			ch_bkg.AddFile(file_GGF_HH_2016)
			ch_sig.AddFile(file_VBF2_HH_2017)
			ch_bkg.AddFile(file_GGF_HH_2017)	       	    	       
			ch_sig.AddFile(file_VBF2_HH_2018)
			ch_bkg.AddFile(file_GGF_HH_2018)	 	
	 #Load data to TMVA
	 dataloader = TMVA.DataLoader('GGFKiller')
	 if optimization=="SM":
		 dataloader.AddVariable("H1_pt")
		 dataloader.AddVariable("H2_pt")
		 dataloader.AddVariable("JJ_j1_pt")
		 dataloader.AddVariable("JJ_j2_pt")
		 dataloader.AddVariable("abs_JJ_eta:=abs(JJ_eta)")
		 dataloader.AddVariable("h1h2_deltaR")
		 dataloader.AddVariable("h1j1_deltaR")
		 dataloader.AddVariable("h1j2_deltaR")
		 dataloader.AddVariable("h2j1_deltaR")
		 dataloader.AddVariable("h2j2_deltaR")
		 #dataloader.AddVariable("abs_j1etaj2eta:=abs(j1etaj2eta)")
		 dataloader.AddVariable("abs_costh_JJ_j1_vbfcm:=abs(costh_JJ_j1_vbfcm)")
		 dataloader.AddVariable("abs_costh_JJ_j2_vbfcm:=abs(costh_JJ_j2_vbfcm)")
		 #dataloader.AddVariable("j1j2_deltaEta")
		 dataloader.AddVariable("JJ_m")
	 else:
		 dataloader.AddVariable("H1_pt")
		 dataloader.AddVariable("H2_pt")
		 dataloader.AddVariable("JJ_j1_pt")
		 dataloader.AddVariable("JJ_j2_pt")
		 dataloader.AddVariable("abs_JJ_eta:=abs(JJ_eta)")
		 dataloader.AddVariable("h1h2_deltaR")
		 dataloader.AddVariable("h1j1_deltaR")
		 dataloader.AddVariable("h1j2_deltaR")
		 dataloader.AddVariable("h2j1_deltaR")
		 dataloader.AddVariable("h2j2_deltaR")
		 dataloader.AddVariable("abs_costh_JJ_j1_vbfcm:=abs(costh_JJ_j1_vbfcm)")
		 dataloader.AddVariable("abs_costh_JJ_j2_vbfcm:=abs(costh_JJ_j2_vbfcm)")
		 #dataloader.AddVariable("j1j2_deltaEta")
		 dataloader.AddVariable("JJ_m")     	 
	 trainingsamplefraction = 0.50
	 nTrain_Signal          = int(ch_sig.GetEntries('VBFEvent==1')*trainingsamplefraction) 
	 nTrain_Background      = int(ch_bkg.GetEntries('VBFEvent==1')*trainingsamplefraction)
	 print("[INFO] ML TRAINING STARTING . . .")
	 print("[INFO] Signal/Background Training Fraction is %f"%trainingsamplefraction)
	 dataloader.AddSignalTree(ch_sig, 1.0)
	 dataloader.AddBackgroundTree(ch_bkg, 1.0)
	 dataloader.SetSignalWeightExpression('XS*preVBFSelEff')
	 dataloader.SetBackgroundWeightExpression('XS*preVBFSelEff')
	 dataloader.PrepareTrainingAndTestTree(TCut('VBFEvent==1'),'nTrain_Signal=%i:nTrain_Background=%i:SplitMode=Random:!V:SplitSeed=%i'%(nTrain_Signal,nTrain_Background,seed))
	 print("[INFO] Boosted Decision Tree Training Starting . . .")
	 if optimization=='SM':
		 #best option FOR SM
		 if dataset=='2016':
			 nTrees =[200]
			 nCuts = [200]
			 nDepth = [2]
		 elif dataset=='2017':
			 nTrees =[250]
			 nCuts = [200]
			 nDepth = [2]
		 else:
			 nTrees =[200]
			 nCuts = [200]
			 nDepth = [2]      	           	 
	 else:
		 #best option for BSM
		 if dataset=='2016':
			 nTrees =[200]
			 nCuts = [200]
			 nDepth = [2]
		 elif dataset=='2017':
			 nTrees =[300]
			 nCuts = [350]
			 nDepth = [2]
		 elif dataset=='2018':
			 nTrees =[200]
			 nCuts = [200]
			 nDepth = [2]
		 else:
			 nTrees =[160]
			 nCuts = [160]
			 nDepth = [3] 
	 for i in nTrees:
	   for j in nCuts:
		 for k in nDepth:
			 factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT_%i_%i_%i'%(i,j,k),
			   '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))
	 # Run training, test and evaluation
	 factory.TrainAllMethods()
	 factory.TestAllMethods()
	 factory.EvaluateAllMethods()
#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--dataset', dest='dataset',  help='Year of the dataset',   required = True)
parser.add_argument('--optimization', dest='optimization',  help='Optimization: SM or BSM',   required = True)
args = parser.parse_args()
dataset = args.dataset
optimization = args.optimization
###########Read Config file

##########Make microskims
print "[INFO] Training starts now"
#Create background sample with bdt weights model in signal region
RunTraining(dataset,optimization)