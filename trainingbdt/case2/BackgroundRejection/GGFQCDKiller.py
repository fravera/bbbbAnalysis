#!/usr/bin/env python

# Select Theano as backend for Keras
from os import environ
environ['KERAS_BACKEND'] = 'theano'

# Set architecture of system (AVX instruction set is not supported on SWAN)
environ['THEANO_FLAGS'] = 'gcc.cxxflags=-march=corei7'


from ROOT import TMVA, TFile, TTree, TCut,TChain
from subprocess import call
from os.path import isfile

from keras.models import Sequential
from keras.layers.core import Dense
from keras.optimizers import Adam
from keras.layers import Dropout
# Setup TMVA
TMVA.PyMethodBase.PyInitialize()

output = TFile.Open('GGFQCDKiller.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
        '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
#Load Data
#Load 2016 SIMULATION
file_GGF_HH_2016        = "../../inputsamples/2016altjobs/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.root"
#file_QCD_200_300_2016   = "../inputsamples/2016/SKIM_QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_300_500_2016   = "../../inputsamples/2016altjobs/SKIM_QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_500_700_2016   = "../../inputsamples/2016altjobs/SKIM_QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_700_1000_2016  = "../../inputsamples/2016altjobs/SKIM_QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_1000_1500_2016 = "../../inputsamples/2016altjobs/SKIM_QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_1500_2000_2016 = "../../inputsamples/2016altjobs/SKIM_QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_2000_Inf_2016  = "../../inputsamples/2016altjobs/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
#Load 2017 SIMULATION
file_GGF_HH_2017        = "../../inputsamples/2017altjobs/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.root"
#file_QCD_200_300_2017   = "../inputsamples/2017/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8.root" 
file_QCD_300_500_2017   = "../../inputsamples/2017altjobs/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8.root"
file_QCD_500_700_2017   = "../../inputsamples/2017altjobs/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8.root"
file_QCD_700_1000_2017  = "../../inputsamples/2017altjobs/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8.root"
file_QCD_1000_1500_2017 = "../../inputsamples/2017altjobs/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8.root"
file_QCD_1500_2000_2017 = "../../inputsamples/2017altjobs/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8.root"
file_QCD_2000_Inf_2017  = "../../inputsamples/2017altjobs/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8.root"
#Load 2018 SIMULATION
#file_QCD_200_300_2018   = "../inputsamples/2018/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8.root"
file_QCD_300_500_2018   = "../../inputsamples/2018altjobs/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8.root"
file_QCD_500_700_2018   = "../../inputsamples/2018altjobs/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8.root"
file_QCD_700_1000_2018  = "../../inputsamples/2018altjobs/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8.root"
file_QCD_1000_1500_2018 = "../../inputsamples/2018altjobs/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8.root"
file_QCD_1500_2000_2018 = "../../inputsamples/2018altjobs/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8.root"
file_QCD_2000_Inf_2018  = "../../inputsamples/2018altjobs/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8.root"
#COMBINE THE INFORMATION
ch_sig = TChain("bbbbTree")
ch_bkg = TChain("bbbbTree")

ch_sig.AddFile(file_GGF_HH_2016)
ch_sig.AddFile(file_GGF_HH_2017)
#ch_bkg.AddFile(file_QCD_200_300_2016)
#ch_bkg.AddFile(file_QCD_200_300_2017)
#ch_bkg.AddFile(file_QCD_200_300_2018)
ch_bkg.AddFile(file_QCD_300_500_2016)
ch_bkg.AddFile(file_QCD_300_500_2017)
ch_bkg.AddFile(file_QCD_300_500_2018)
ch_bkg.AddFile(file_QCD_500_700_2016)
ch_bkg.AddFile(file_QCD_500_700_2017)
ch_bkg.AddFile(file_QCD_500_700_2018)
ch_bkg.AddFile(file_QCD_700_1000_2016)
ch_bkg.AddFile(file_QCD_700_1000_2017)
ch_bkg.AddFile(file_QCD_700_1000_2018)
ch_bkg.AddFile(file_QCD_1000_1500_2016)
ch_bkg.AddFile(file_QCD_1000_1500_2017)
ch_bkg.AddFile(file_QCD_1000_1500_2018)
ch_bkg.AddFile(file_QCD_1500_2000_2016)
ch_bkg.AddFile(file_QCD_1500_2000_2017)
ch_bkg.AddFile(file_QCD_1500_2000_2018)
ch_bkg.AddFile(file_QCD_2000_Inf_2016)
ch_bkg.AddFile(file_QCD_2000_Inf_2017)
ch_bkg.AddFile(file_QCD_2000_Inf_2018)

dataloader = TMVA.DataLoader('GGFQCDKiller')
dataloader.AddVariable("HH_b3_pt")
dataloader.AddVariable("HH_b4_pt")
dataloader.AddVariable("HH_b3_eta:=abs(HH_b3_eta)")
dataloader.AddVariable("HH_b4_eta:=abs(HH_b4_eta)")
dataloader.AddVariable("H1_m")
dataloader.AddVariable("H2_m")
dataloader.AddVariable("H1_bb_deltaR")
dataloader.AddVariable("H2_bb_deltaR")
dataloader.AddVariable("H1_bb_deltaPhi")
dataloader.AddVariable("H2_bb_deltaPhi")
dataloader.AddVariable("abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)")
dataloader.AddVariable("abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)")

trainingsamplefraction = 0.50 #next,also try 0.5 0.6
nTrain_Signal          = int(ch_sig.GetEntries('VBFEvent==0 || (VBFEvent==1 && BDT1<0)')*trainingsamplefraction) 
nTrain_Background      = int(ch_bkg.GetEntries('VBFEvent==0 || (VBFEvent==1 && BDT1<0)')*trainingsamplefraction)
print("[INFO] ML TRAINING STARTING . . .")
print("[INFO] Signal/Background Training Fraction is %f"%trainingsamplefraction) 
#999
dataloader.AddSignalTree(ch_sig, 1.0)
dataloader.AddBackgroundTree(ch_bkg, 1.0)
dataloader.SetSignalWeightExpression('XS')
dataloader.SetBackgroundWeightExpression('XS')
dataloader.PrepareTrainingAndTestTree(TCut('VBFEvent==0 || (VBFEvent==1 && BDT1<0)'),'nTrain_Signal=%i:nTrain_Background=%i:SplitMode=Random:!V:SplitSeed=999'%(nTrain_Signal,nTrain_Background))

nTrees =[350]
nCuts = [400]
nDepth = [2]

#nTrees =[300]
#nCuts = [200]
#nDepth = [2]

for i in nTrees:
  for j in nCuts:
    for k in nDepth:
        factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT_%i_%i_%i'%(i,j,k),
          '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))

# Run training, test and evaluation
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()