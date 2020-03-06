#!/usr/bin/env python

import argparse
from os import environ
from ROOT import TMVA, TFile, TTree, TCut,TChain
from subprocess import call
from os.path import isfile

def  RunTraining(dataset,optimization):
	 # Setup TMVA
	 TMVA.PyMethodBase.PyInitialize()     
	 output = TFile.Open('GGFQCDKiller_BR_%s_%s.root'%(dataset,optimization), 'RECREATE')
	 factory = TMVA.Factory('TMVAClassification%s%s'%(dataset,optimization), output,
			 '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
	 #Load Data
	 #Load 2016 SIMULATION
	 file_GGF_HH_2016        = "../FullNtuples/New2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_BR.root"
	 file_QCD_MODEL_2016     = "../FullNtuples/New2016/SKIM_BKG_MODEL_BR.root"
	 #Load 2017 SIMULATION
	 file_GGF_HH_2017        = "../FullNtuples/New2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg_BR.root"
	 file_QCD_MODEL_2017     = "../FullNtuples/New2017/SKIM_BKG_MODEL_BR.root"
	 #Load 2018 SIMULATION
	 file_GGF_HH_2018        = "../FullNtuples/New2018/SKIM_GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8_BR.root"
	 file_QCD_MODEL_2018     = "../FullNtuples/New2018/SKIM_BKG_MODEL_BR.root"

	 #COMBINE THE INFORMATION
	 ch_sig = TChain("bbbbTree")
	 ch_bkg = TChain("bbbbTree") 
	 #AddfILES       
	 if dataset=='2016':
		ch_sig.AddFile(file_GGF_HH_2016)
		ch_bkg.AddFile(file_QCD_MODEL_2016) 
	 elif dataset=='2017': 
		ch_sig.AddFile(file_GGF_HH_2017)
		ch_bkg.AddFile(file_QCD_MODEL_2017)   
	 elif dataset=='2018': 	      
		ch_sig.AddFile(file_GGF_HH_2018)
		ch_bkg.AddFile(file_QCD_MODEL_2018)  
	 else:
		ch_sig.AddFile(file_GGF_HH_2016)
		ch_bkg.AddFile(file_QCD_MODEL_2016)  
	 #LOAD VARIABLES	
	 dataloader = TMVA.DataLoader('GGFQCDKiller')
	 dataloader.AddVariable("H1_pt")
	 dataloader.AddVariable("H2_pt")
	 dataloader.AddVariable("H1_m")
	 dataloader.AddVariable("H2_m")
	 dataloader.AddVariable("HH_m")
	 dataloader.AddVariable("h1h2_deltaEta")
	 dataloader.AddVariable("H1_bb_deltaR")
	 dataloader.AddVariable("H2_bb_deltaR")
	 dataloader.AddVariable("abs_costh_HH_b1_cm:=abs(costh_HH_b1_ggfcm)")
	 dataloader.AddVariable("HH_btag_b3_bscore")
	 dataloader.AddVariable("HH_btag_b3_bres")	 
	 trainingsamplefraction = 0.50 #next,also try 0.5 0.6
	 nTrain_Signal          = int(ch_sig.GetEntries('GGFSignalRegion==1 && (Weight_MVA==0 || Weight_MVA==1)')*trainingsamplefraction) 
	 nTrain_Background      = int(ch_bkg.GetEntries('GGFSignalRegion==1 && (Weight_MVA==0 || Weight_MVA==1)')*trainingsamplefraction)
	 print("[INFO] ML TRAINING STARTING . . .")
	 print("[INFO] Signal/Background Training Fraction is %f"%trainingsamplefraction) 
	 dataloader.AddSignalTree(ch_sig, 1.0)
	 dataloader.AddBackgroundTree(ch_bkg, 1.0)
	 dataloader.SetBackgroundWeightExpression('Weight_AnaGGF')
	 #best option FOR SM
	 if optimization=='SM':
		 if dataset=='2016':
			 seed=2019
			 nTrees =[350]
			 nCuts = [200]
			 nDepth = [2]
		 elif dataset=='2017':
			 seed=2019
			 nTrees =[300]
			 nCuts = [300]
			 nDepth = [2]
		 elif dataset=='2018':
			 seed=2020
			 nTrees =[250]
			 nCuts = [250]
			 nDepth = [2]
		 elif dataset=='20172018':
			 seed=2020
			 nTrees =[250]
			 nCuts = [250]
			 nDepth = [2]
		 else:
			 seed=2020
			 nTrees =[200]
			 nCuts = [200]
			 nDepth = [2]	 
	 else:
		 if dataset=='2016':
			 seed=2019
			 nTrees =[300]
			 nCuts = [300]
			 nDepth = [2]
		 elif dataset=='2017':
			 seed=2020
			 nTrees =[300]
			 nCuts = [300]
			 nDepth = [2]
		 elif dataset=='2018':
			 seed=2021
			 nTrees =[300]
			 nCuts = [300]
			 nDepth = [2]
		 elif dataset=='20172018':
			 seed=2020
			 nTrees =[250,300,350]
			 nCuts = [250,300,350]
			 nDepth = [2]
		 else:
			 seed=2020
			 nTrees =[200]
			 nCuts = [200]
			 nDepth = [2]
	 dataloader.PrepareTrainingAndTestTree(TCut('GGFSignalRegion==1 && (Weight_MVA==0 || Weight_MVA==1)'),'nTrain_Signal=%i:nTrain_Background=%i:SplitMode=Random:!V:SplitSeed=%i'%(nTrain_Signal,nTrain_Background,seed))
	 for i in nTrees:
	   for j in nCuts:
		 for k in nDepth:
			 factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT3_%i_%i_%i'%(i,j,k),
			   '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))
	 # Run training, test and evaluation
	 factory.TrainAllMethods()
	 factory.TestAllMethods()
	 factory.EvaluateAllMethods()
#############COMMAND CODE IS BELOW ######################################################################################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--dataset', dest='dataset',  help='Year of the dataset',   required = True)
parser.add_argument('--optimization', dest='optimization',  help='Optimization of the dataset',   required = True)
args = parser.parse_args()
dataset      = args.dataset
optimization = args.optimization
###########Read Config file

##########Make microskims
print "[INFO] Training starts now"
#Create background sample with bdt weights model in signal region
RunTraining(dataset,optimization)