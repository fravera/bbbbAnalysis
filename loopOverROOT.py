import pandas as pd
from ROOT import TFile
import matplotlib.pyplot as plt

def goLoop(filename,treename,branches,min_cuts=None,max_cuts=None):
    """
    branches = list of names (strings) of branches
    min_cuts = dictionary of branches and their desired min values
    max_cuts = dictionary of branches and their desired max values
    """

    f = TFile(filename)
    t = f.Get(treename)

    df = pd.DataFrame(columns=branches)

    for evt in t:
        try:
            for cut in min_cuts:
                if getattr(evt, cut) < min_cuts[i]: continue
            for cut in max_cuts:
                if getattr(evt, cut) < max_cuts[i]: continue
        except:
        for branch in branches:
            df = df.append({branch:getattr(evt,branch)},ignore_index=True)

    return df


df20 = goLoop('test_NMSSM_XYH_bbbb_MC_selectedJets_20GeVpT.root','bbbbTree',['H1_b1_ptRegressed',
   ...: 'H1_b2_ptRegressed','H2_b1_ptRegressed','H2_b2_ptRegressed'])