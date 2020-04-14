import pandas as pd
from ROOT import TFile
import matplotlib.pyplot as plt

def goLoop(filename,treename,branches):
    """
    branches = list of names (strings) of branches
    """

    f = TFile(filename)
    t = f.Get(treename)

    df = pd.DataFrame(columns=branches)

    for evt in t:
        for i in range(len(branches)):
            df = df.append({branches[i]:getattr(evt,branches[i])},ignore_index=True)

    return df


