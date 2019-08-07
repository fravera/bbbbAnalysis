#!/usr/bin/env python

# Select Theano as backend for Keras
from os import environ
import os 
environ['KERAS_BACKEND'] = 'tensorflow'

# Set architecture of system (AVX instruction set is not supported on SWAN)
environ['THEANO_FLAGS'] = 'gcc.cxxflags=-march=corei7'


from ROOT import TMVA, TFile, TTree, TCut,TChain
from subprocess import call
from os.path import isfile

from keras.models import Sequential
from keras.layers.core import Dense
from keras.optimizers import Adam
from keras.layers import Dropout
from keras.utils.vis_utils import plot_model

# Setup TMVA
#os.system('rm -r mynnmodels1')
#os.system('mkdir mynnmodels1')
TMVA.PyMethodBase.PyInitialize()

output = TFile.Open('VBFQCDKiller.root', 'RECREATE')
factory = TMVA.Factory('TMVAClassification', output,
        '!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification')
#Load Data
#Load 2016 SIMULATION
file_VBF_HH_2016        = "../../inputsamples/2016altjobs/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root"
#file_QCD_200_300_2016   = "../inputsamples/2016/SKIM_QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_300_500_2016   = "../../inputsamples/2016altjobs/SKIM_QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_500_700_2016   = "../../inputsamples/2016altjobs/SKIM_QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_700_1000_2016  = "../../inputsamples/2016altjobs/SKIM_QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_1000_1500_2016 = "../../inputsamples/2016altjobs/SKIM_QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_1500_2000_2016 = "../../inputsamples/2016altjobs/SKIM_QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
file_QCD_2000_Inf_2016  = "../../inputsamples/2016altjobs/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root"
#Load 2017 SIMULATION
file_VBF_HH_2017        = "../../inputsamples/2017altjobs/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root"
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
#COMBINE THE INFORMATION ALONG THE THREE YEARS
ch_sig = TChain("bbbbTree")
ch_bkg = TChain("bbbbTree")
#COMBINE VBF-HH SIGNAL
ch_sig.AddFile(file_VBF_HH_2016)
ch_sig.AddFile(file_VBF_HH_2017)
#ch_bkg.AddFile(file_QCD_200_300_2016)
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

dataloader = TMVA.DataLoader('VBFQCDKiller')
dataloader.AddVariable("HH_b3_pt")
dataloader.AddVariable("HH_b4_pt")
dataloader.AddVariable("JJ_j1_qgl")
dataloader.AddVariable("JJ_j2_qgl")
dataloader.AddVariable("H1_m")
dataloader.AddVariable("H2_m")
dataloader.AddVariable("H1_bb_deltaR")
dataloader.AddVariable("H2_bb_deltaR")
dataloader.AddVariable("JJ_m")
dataloader.AddVariable("j1j2_deltaEta") 
dataloader.AddVariable("abs_costh_JJ_j1_cm:=abs(costh_JJ_j1_cm)")

trainingsamplefraction = 0.50 #0.70
nTrain_Signal          = int(ch_sig.GetEntries('VBFEvent==1')*trainingsamplefraction) 
nTrain_Background      = int(ch_bkg.GetEntries('VBFEvent==1')*trainingsamplefraction)
print("[INFO] ML TRAINING STARTING . . .")
print("[INFO] Signal/Background Training Fraction is %f"%trainingsamplefraction)
dataloader.AddSignalTree(ch_sig, 1.0)
dataloader.AddBackgroundTree(ch_bkg, 1.0)
dataloader.SetSignalWeightExpression('XS')
dataloader.SetBackgroundWeightExpression('XS')
dataloader.PrepareTrainingAndTestTree(TCut('VBFEvent==1'),'nTrain_Signal=%i:nTrain_Background=%i:SplitMode=Random:!V:SplitSeed=10'%(nTrain_Signal,nTrain_Background))

#nTrees =[200,250,300,350,400,450]
#nCuts = [200,250,300,350,400,450]
#nDepth = [2]

nTrees = [250]
nCuts  = [200]
nDepth = [2]

#nTrees =[225]
#nCuts = [200,250]
#nDepth = [2]

for i in nTrees:
  for j in nCuts:
    for k in nDepth:
        factory.BookMethod(dataloader, TMVA.Types.kBDT, 'BDT_%i_%i_%i'%(i,j,k),
          '!H:!V:NTrees=%i:MinNodeSize=2.5:BoostType=Grad:Shrinkage=0.05:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=%i:MaxDepth=%i'%(i,j,k))


#print("[INFO] Deep Neural Network Training Starting . . .")
#Method: DNN
#nTrain_batchsize       = int(nTrain_Signal/15)
#nodes =[100,110,120,130,140,150]
#nlayers = [3]
##nodes =[110]
##nlayers = [3]
##for i in nodes:
##  for j in nlayers:
##       model = Sequential()
##       model.add(Dense(i, kernel_initializer='glorot_normal', activation='relu', input_dim=12))
##       count = 0 
##       while count < j-1: 
##         model.add( Dropout(0.20, noise_shape=None, seed=100)   ) 
##         model.add(Dense(i, kernel_initializer='glorot_normal', activation='relu'))
##         count += 1         
##       model.add( Dropout(0.20, noise_shape=None, seed=100)   ) 
##       model.add(Dense(2, activation='softmax'))
##       # Set loss and optimizer
##       model.compile(loss='categorical_crossentropy', optimizer=Adam(lr=0.001), metrics=['accuracy',])
##       # Store model to file
##       model.save('mynnmodels1/model_nn_%i_%i.h5'%(j,i))
##       model.summary()
##       #Book DNN method
##       factory.BookMethod(dataloader, TMVA.Types.kPyKeras, 'NN-%ilayers-%i'%(j,i) ,
##             'H:!V:VarTransform=G:FilenameModel=mynnmodels1/model_nn_%i_%i.h5:NumEpochs=50:TriesEarlyStopping=3:BatchSize=%i'%(j,i,nTrain_batchsize))
##       plot_model(model, 'mynnmodels1/NN-%ilayers-%i.eps'%(j,i), show_shapes=True)
# Run training, test and evaluation
factory.TrainAllMethods()
factory.TestAllMethods()
factory.EvaluateAllMethods()