import numpy as np
from ROOT import TFile, TH1F, TCanvas, gROOT, kTRUE, gPad, TF1, gStyle
gROOT.SetBatch(kTRUE) # Do not display canvas
import sys, os
import pandas as pd

# Path to EOS directory containing job directories.
directory = 'root://cmseos.fnal.gov//store/user/srosenzw/bbbb_ntuples/'

# Arrays containing all possible mX and mY values.
MX_vals, MY_vals = np.load('mX_values.npz'), np.load('mY_values.npz')

def loopOver(jobtitle, branches, ranges, ncols, nrows, nbins, selections):
    """
    This function loops over ROOT files in the job directory contained in EOS.

    jobtitle   =   tag used to run the job on Condor
    branches   =   which branches of the bbbbTree the user would like to plot
    ranges     =   must be the same length as branches
    """

    c = TCanvas('c', jobtitle) # Define canvas
    c.Divide(ncols, nrows) # Divide canvas into columns, rows
    c.Print(jobtitle+".pdf[") # Open the pdf file to which the canvas will be saved

    for mx in MX_vals['arr_0']:
        for my in MY_vals['arr_0']:
            
            filename = jobtitle + '/SKIM_NMSSM_XYH_bbbb_MX_{}_NANOAOD_v7_MY_{}/output/bbbbNtuple_0.root'.format(str(mx),str(my))

            try:
                f = TFile.Open(directory + filename)
                t = f.Get('bbbbTree')
            except: continue
            
            if not len(branches) == len(ranges):
                raise ValueError('branches and ranges should have same length!')

            num_files = len(branches)
            for i in range(num_files):
                c.cd(i)
                gPad.SetLogy()
                title = 'SKIM, {} mX, {} mY, {}'.format(str(mx),str(my),branches[i])
                h1 = TH1F("h{}".format(i),title,nbins,ranges[i][0],ranges[i][1])
                try:
                    t.Draw("{} >> h{}".format(branches[i],i),selections)
                except: continue
                c.Print(jobtitle+'.pdf')
    c.Print(jobtitle + '.pdf]')




# # Example call
# sel = "gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0"
# loopOver('v3',["H1_m"],[[0,300]],1,1,30,sel)