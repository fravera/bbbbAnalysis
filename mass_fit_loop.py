import numpy as np
from ROOT import TFile, TH1F, TCanvas, gROOT, kTRUE, gPad, TF1, gStyle
gROOT.SetBatch(kTRUE) # Do not display canvas
import sys, os
import pandas as pd

directory = 'root://cmseos.fnal.gov//store/user/fravera/bbbb_ntuples/fullSubmission_v10/'

# Create arrays containing mX and mY values
MX_vals = np.arange(300,1200,100)
MX_vals = np.append(MX_vals, np.arange(1400,2000,200))
MY_vals = np.array([60,  70, 80, 90, 100, 125, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400])

how_many_y_masses = []

c = TCanvas('c', 'Mass Fits')
c.Divide(2,1)
filename_save = 'mass_fits.pdf'
c.Print(filename_save+"[") # This allows one to build a pdf and add multiple canvases to the same pdf file. This line opens the pdf file.

# Create Pandas DataFrame in which to store mass data for easy retrieval later.
df = pd.DataFrame(columns=["m_X","m_Y","m_H","sigma"])

count_X = 0
for MX in MX_vals: # Loop through mX values
    count_Y = 0
    for MY in MY_vals: # Loop through mY values
        mass_info = 'SKIM_NMSSM_XYH_bbbb_MX_' + str(MX) + '_NANOAOD_v7_MY_' + str(MY)
        print("[INFO] Processing mass values mX = {} and mY = {}.".format(MX,MY))
        filename = mass_info + '/output/bbbbNtuple_0.root'
        try: 
            full_file = directory + filename
            f = TFile.Open(full_file)
            t = f.Get("bbbbTree")
        except (IOError, ReferenceError):
            df = df.append({"m_X":MX,"m_Y":MY,"m_H":0,"sigma":0},ignore_index=True)
            continue

        c.cd(1)
        gPad.SetLogy()
        name1 = mass_info + "_H1_m"
        name1 = 'Reco mH for gen mX = {} and gen mY = {}'.format(MX,MY)
        h1 = TH1F("h1",name1,30,0,400)
        try:
            t.Draw("H1_m >> h1","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0") # Select events with cuts
            # t.Draw("H1_m >> h1") # Select events without cuts
        except: 
            df = df.append({"m_X":MX,"m_Y":MY,"m_H":0,"sigma":0},ignore_index=True) # Null data for non-existent events
            continue
        gStyle.SetTitleFontSize(0.05) # Attempt to change title font size... winds up changing title position.
        h1.Draw()

        gaussFit1 = TF1("gaussfit1","gaus",80,160) # Build fit function (Gaussian)
        h1.Fit(gaussFit1,"R") # Fit Gaussian to histograms

        # Fit parameters
        norm1  = gaussFit1.GetParameter(0)
        mean1  = gaussFit1.GetParameter(1)
        sigma1 = gaussFit1.GetParameter(2)
        
        gStyle.SetOptStat(0) # Remove stat box
        gStyle.SetOptFit(11) # Replace stat box with fit stats
        c.Update() # Update canvas



        c.cd(2)
        gPad.SetLogy()
        name2 = mass_info + "_H2_m"
        name2 = 'Reco mY for gen mX = {} and gen mY = {}'.format(MX,MY)
        h2 = TH1F("h2",name2,30,0,700)
        try:
            t.Draw("H2_m >> h2","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0") # Select events with cuts
            # t.Draw("H2_m >> h2") # Select events without cuts
        except:
            df = df.append({"m_X":MX,"m_Y":MY,"m_H":0,"sigma":0},ignore_index=True) # Null data for non-existent events
            continue
        gStyle.SetTitleFontSize(0.05)
        h2.Draw()

        gaussFit2 = TF1("gaussfit2","gaus",MY*0.5,MY*1.5)
        h2.Fit(gaussFit2,"R")

        norm2  = gaussFit1.GetParameter(0)
        mean2  = gaussFit1.GetParameter(1)
        sigma2 = gaussFit1.GetParameter(2)

        gStyle.SetOptStat(0)
        gStyle.SetOptFit(11)
        c.Update()
    
        # Append mass values to DataFrame for later processing.
        df = df.append({"m_X":MX,"m_Y":MY,"m_H":mean1,"sigma":sigma1},ignore_index=True)


        # print("Saving as {}.pdf".format(filename))
        c.Print(filename_save) # This line saves the canvas to the previously opened pdf file.
        count_Y += 1
    count_X += 1
    how_many_y_masses.append(count_Y)   

df.to_csv("higgs_mass_fits.csv",index=False) # Save DataFrame values as csv.
c.Print(filename_save + ']') # This line closes the pdf file. 

        