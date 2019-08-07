#!/usr/bin/env python

# Select Tensorflow as backend for Keras
from os import environ
import os 
#environ['KERAS_BACKEND'] = 'tensorflow'
# Set architecture of system (AVX instruction set is not supported on SWAN)
#environ['THEANO_FLAGS'] = 'gcc.cxxflags=-march=corei7'
from ROOT import TMVA, TFile, TTree, TCut,TChain
from subprocess import call
from os.path import isfile
from keras.models import Sequential
from keras.layers.core import Dense
from keras.optimizers import Adam
from keras.layers import Dropout
from keras.callbacks import EarlyStopping
from keras.utils.vis_utils import plot_model

# Setup TMVA
#os.system('rm -r mynnmodels')
#os.system('mkdir mynnmodels')
TMVA.PyMethodBase.PyInitialize()
output = TFile.Open('GGFKiller.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
        '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
#Locate and add data files
file_VBF_HH_2016 = "../../inputsamples/BDT1/2016vbf.root"
file_VBF_HH_2017 = "../../inputsamples/BDT1/2017vbf.root"
file_GGF_HH_2016 = "../../inputsamples/BDT1/2016ggf.root"
file_GGF_HH_2017 = "../../inputsamples/BDT1/2017ggf.root"
ch_sig = TChain("bbbbTree")
ch_bkg = TChain("bbbbTree")
ch_sig.AddFile(file_VBF_HH_2016)
ch_sig.AddFile(file_VBF_HH_2017)   
ch_bkg.AddFile(file_GGF_HH_2016)
ch_bkg.AddFile(file_GGF_HH_2017)
#Load data to TMVA
dataloader = TMVA.DataLoader('GGFKiller')
dataloader.AddVariable("abs_H1_eta:=abs(H1_eta)")
dataloader.AddVariable("abs_H2_eta:=abs(H2_eta)")
dataloader.AddVariable("H1_pt")
dataloader.AddVariable("H2_pt")
dataloader.AddVariable("JJ_j1_pt")
dataloader.AddVariable("JJ_j2_pt")
dataloader.AddVariable("abs_JJ_eta:=abs(JJ_eta)")
dataloader.AddVariable("h1h2_deltaEta")
dataloader.AddVariable("h1j1_deltaR")
dataloader.AddVariable("h1j2_deltaR")
dataloader.AddVariable("h2j1_deltaR")
dataloader.AddVariable("h2j2_deltaR")
dataloader.AddVariable("abs_j1etaj2eta:=abs(j1etaj2eta)")
dataloader.AddVariable("abs_costh_HH_b1_cm:=abs(costh_HH_b1_cm)")
dataloader.AddVariable("abs_costh_HH_b2_cm:=abs(costh_HH_b2_cm)")

trainingsamplefraction = 0.50
nTrain_Signal          = int(ch_sig.GetEntries('VBFEvent==1')*trainingsamplefraction) 
nTrain_Background      = int(ch_bkg.GetEntries('VBFEvent==1')*trainingsamplefraction)

print("[INFO] ML TRAINING STARTING . . .")
print("[INFO] Signal/Background Training Fraction is %f"%trainingsamplefraction)
dataloader.AddSignalTree(ch_sig, 1.0)
dataloader.AddBackgroundTree(ch_bkg, 1.0)
dataloader.PrepareTrainingAndTestTree(TCut('VBFEvent==1'),'nTrain_Signal=%i:nTrain_Background=%i:SplitMode=Random:!V:SplitSeed=10000'%(nTrain_Signal,nTrain_Background))

print("[INFO] Boosted Decision Tree Training Starting . . .")
#best options
#nTrees =[230,240]
#nCuts = [200,250]
#nDepth = [2]
#nTrees =[100,120,140,160,180,200,220,240,260,280,300,320,340,360,380,400]
#nCuts = [100,120,140,160,180,200,220,240,260,280,300,320,340,360,380,400]
#nDepth = [2]

#nTrees =[350,375]
#nCuts = [275,300,325,350,375]
#nDepth = [2]

nTrees =[350]
nCuts = [275]
nDepth = [2]

for i in nTrees:
  for j in nCuts:
    for k in nDepth:
        factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT_%i_%i_%i'%(i,j,k),
          '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))

# Run training, test and evaluation
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()