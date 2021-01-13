from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph, TLegend
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array


ROOT.gROOT.SetBatch(True)

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--input'    , dest = 'input'    , help = 'input file' , required = True)
parser.add_argument('--systematics'   , dest='systematics'   , help='systematics'   , action="store_true", default = False, required = False)

args = parser.parse_args()

append = "statOnly"
if args.systematics : append = "syst"
color     = ROOT.kBlue

xMassList = [300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1400, 1600, 1800, 2000]

inputFile = TFile(args.input)

for xMass in xMassList:
    theCanvas = TCanvas("limitMapCentral", "limitMapCentral", 1200, 800)
    theLegend  = TLegend(0.47,0.6,0.88,0.88)
    theLegend.SetTextSize(0.04)
    # theLegend.SetNColumns(2);

    theCanvas.SetLogy()

    inputGraph2sigmaName = "Limits_RunII/Option_%s/2SigmaLimit_RunII_%s_massX_%i" % (append, append, xMass)
    theGraph2sigma = inputFile.Get(inputGraph2sigmaName)
    theGraph2sigma.SetTitle("")
    theGraph2sigma.GetXaxis().SetTitle("m_{Y} [GeV]")
    theGraph2sigma.GetXaxis().SetLabelFont(62)
    theGraph2sigma.GetXaxis().SetLabelSize(0.045)
    theGraph2sigma.GetXaxis().SetTitleFont(62)
    theGraph2sigma.GetXaxis().SetTitleSize(0.045)
    theGraph2sigma.GetYaxis().SetLabelFont(62)
    theGraph2sigma.GetYaxis().SetLabelSize(0.045)
    theGraph2sigma.GetYaxis().SetTitleFont(62)
    theGraph2sigma.GetYaxis().SetTitleSize(0.045)
    theGraph2sigma.GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
    theGraph2sigma.GetYaxis().SetRangeUser(1.,1.e5)
    theGraph2sigma.GetXaxis().SetRangeUser(50.,1900.)
    theGraph2sigma.Draw("a3")

    inputGraph1sigmaName = "Limits_RunII/Option_%s/1SigmaLimit_RunII_%s_massX_%i" % (append, append, xMass)
    theGraph1sigma = inputFile.Get(inputGraph1sigmaName)
    theGraph1sigma.Draw("same 3")

    inputGraphName = "Limits_RunII/Option_%s/CentralLimit_RunII_%s_massX_%i" % (append, append, xMass)
    theGraph = inputFile.Get(inputGraphName)
    theGraph.SetLineColor(color)
    theGraph.SetLineStyle(7)
    # theGraph.SetMarkerColor(color)
    theGraph.SetLineWidth(2)
    # theGraph.SetMarkerStyle(20)
    # theGraph.SetMarkerSize(0.7)
    theGraph.Draw("same pl")

    theLegend.AddEntry(theGraph, "Median expected", "lp")
    theLegend.AddEntry(theGraph1sigma, "68% expected"                             , "f" )
    theLegend.AddEntry(theGraph2sigma, "95% expected"                             , "f" )


    theLegend.Draw("same")
    theCanvas.SaveAs("LimitsRunII_Limits_" + append +  "_mX_" + str(xMass) + ".png")

inputFile.Close()
