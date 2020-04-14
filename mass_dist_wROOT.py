# This program prints the mass distribution for samples with varying values of the selection criteria, Delta R.

print("[INFO] Importing libraries...")
from ROOT import TFile, TH1F, TCanvas, gROOT, kTRUE, gPad
import numpy as np
gROOT.SetBatch(kTRUE)

num_bins = 30
lower_bound = 0
upper_bound = 1600 # 300 for mH, 800 for mY, 1600 for mX

print("[INFO] Building canvas...")
c1 = TCanvas("c1", "Mass Distribution") # Canvas object
c1.Divide(4,2) # Divide canvas into 8 pads (4 columns, 2 rows)

c1.cd(1)
gPad.SetLogy() # Set y-axis to log scale
f1 = TFile("test_NMSSM_XYH_bbbb_MC_005.root") # Open ROOT file
t1 = f1.Get("bbbbTree") # Extract ROOT tree
h1 = TH1F("h1","DeltaR = 0.05", num_bins, lower_bound, upper_bound) # Create histogram object
t1.Draw("HH_m >> h1","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff") # Fill histogram with events that pass the selection criteria
h1.Draw("hist e1") # Draw histogram

c1.cd(2)
gPad.SetLogy()
f2 = TFile("test_NMSSM_XYH_bbbb_MC_010.root")
t2 = f2.Get("bbbbTree")
h2 = TH1F("h2","DeltaR = 0.10", num_bins, lower_bound, upper_bound)
t2.Draw("HH_m >> h2","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h2.Draw("hist e1")

c1.cd(3)
gPad.SetLogy()
f3 = TFile("test_NMSSM_XYH_bbbb_MC_015.root")
t3 = f3.Get("bbbbTree")
h3 = TH1F("h3","DeltaR = 0.15", num_bins, lower_bound, upper_bound)
t3.Draw("HH_m >> h3","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h3.Draw("hist e1")

c1.cd(4)
gPad.SetLogy()
f4 = TFile("test_NMSSM_XYH_bbbb_MC_020.root")
t4 = f4.Get("bbbbTree")
h4 = TH1F("h4","DeltaR = 0.20", num_bins, lower_bound, upper_bound)
t4.Draw("HH_m >> h4","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h4.Draw("hist e1")

c1.cd(5)
gPad.SetLogy()
f5 = TFile("test_NMSSM_XYH_bbbb_MC_025.root")
t5 = f5.Get("bbbbTree")
h5 = TH1F("h5","DeltaR = 0.25", num_bins, lower_bound, upper_bound)
t5.Draw("HH_m >> h5","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h5.Draw("hist e1")

c1.cd(6)
gPad.SetLogy()
f6 = TFile("test_NMSSM_XYH_bbbb_MC_030.root")
t6 = f6.Get("bbbbTree")
h6 = TH1F("h6","DeltaR = 0.30", 30, lower_bound, upper_bound)
t6.Draw("HH_m >> h6","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h6.Draw("hist e1")

c1.cd(7)
gPad.SetLogy()
f7 = TFile("test_NMSSM_XYH_bbbb_MC_035.root")
t7 = f7.Get("bbbbTree")
h7 = TH1F("h7","DeltaR = 0.35", 30, lower_bound, upper_bound)
t7.Draw("HH_m >> h7","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h7.Draw("hist e1")

c1.cd(8)
gPad.SetLogy()
f8 = TFile("test_NMSSM_XYH_bbbb_MC_040.root")
t8 = f8.Get("bbbbTree")
h8 = TH1F("h8","DeltaR = 0.40", 30, lower_bound, upper_bound)
t8.Draw("HH_m >> h8","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h8.Draw("hist e1")

c1.Draw() # Draw canvas
c1.SaveAs("distribution_mX.pdf") # Save canvas