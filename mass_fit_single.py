import numpy as np
from ROOT import TFile, TH1F, TCanvas, gROOT, kTRUE, gPad, TF1, gStyle, TLatex, TPaveStats
gROOT.SetBatch(kTRUE)


f1 = TFile("test_NMSSM_XYH_bbbb_MC_020.root")
t1 = f1.Get("bbbbTree")

c1 = TCanvas("c1", "Mass Distributions")
c1.Divide(2,1)

c1.cd(1)
gPad.SetLogy()

h1 = TH1F("h1","H1_m", 30, 0, 400)
t1.Draw("H1_m >> h1","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
gStyle.SetTitleFontSize(0.2)
h1.Draw()
# gPad.Update()
# st = h1.GetListOfFunctions().FindObject("stats")


gaussFit1 = TF1("gaussfit","gaus",90,150)
h1.Fit(gaussFit1,"R")
norm  = gaussFit1.GetParameter(0)
mean  = gaussFit1.GetParameter(1)
sigma = gaussFit1.GetParameter(2)

# mean_label = TLatex(0,0,"Test = {}".format(mean))
# st.AddText(0,0,'Test')
# h1.Modify()

gStyle.SetOptStat(0)
gStyle.SetOptFit(11)
c1.Update()

# h1.SetStats(0)
# obj = h1.FindObject("stats")


c1.cd(2)
gPad.SetLogy()

h2 = TH1F("h2","H2_m", 30, 0, 700)
t1.Draw("H2_m >> h2","gen_H1_b1_matchedflag >= 0 && gen_H1_b2_matchedflag >= 0 && gen_H2_b1_matchedflag >= 0 && gen_H2_b2_matchedflag >= 0","goff")
h2.Draw()

gaussFit2 = TF1("gaussfit"," gaus ",200,400)
h2.Fit(gaussFit2,"R")

c1.Draw()
gStyle.SetOptStat(0)
gStyle.SetOptFit(11)
c1.Update()
# ps = c1.GetPrimitive("stats").GetListOfLines()

c1.SaveAs("mass_fit_single.png")