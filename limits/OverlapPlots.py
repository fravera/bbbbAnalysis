from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph, TLegend
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array
import argparse
import os.path
from array import array

ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ForceStyle()

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--input'    , dest = 'input'    , help = 'input file' , required = True)
parser.add_argument('--systematics'   , dest='systematics'   , help='systematics'   , action="store_true", default = False, required = False)

args = parser.parse_args()
append = "statOnly"
if args.systematics : append = "syst"

massList  = [90  , 125, 300   , 600  , 1000  ]
colorList = [ROOT.kBlue, ROOT.kRed, ROOT.kOrange, ROOT.kGreen, ROOT.kViolet]

inputFile = TFile(args.input)
theCanvas = TCanvas("limitMapCentral", "limitMapCentral", 1200, 800)
theLegend  = TLegend(0.37,0.45,0.88,0.88)
theLegend.SetTextSize(0.04)

# theLegend.SetNColumns(2);

theCanvas.SetLogy()
firstValue = True

for (massY, color) in zip(massList, colorList):
    inputGraphName = "Limits_RunII/Option_%s/CentralLimit_RunII_%s_massY_%s" % (append, append, str(massY))
    print inputGraphName
    theGraph = inputFile.Get(inputGraphName)
    theGraph.SetLineColor(color)
    theGraph.SetMarkerColor(color)
    theGraph.SetLineWidth(3)
    theGraph.SetMarkerStyle(20)
    theGraph.SetMarkerSize(1.)
    theGraph.GetXaxis().SetTitle("m_{X} [GeV]")
    theGraph.GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
    theLegend.AddEntry(theGraph, "Median expected, m_{Y} = " + str(int(massY)) + " GeV", "lp")
    if firstValue:
        theGraph.SetTitle("")
        theGraph.Draw("apl")
        firstValue = False
        theGraph.GetXaxis().SetLimits(200., 2100.)
    else:
        theGraph.Draw("same pl")

theLegend.Draw("same")
theCanvas.SaveAs("LimitsRunII_Limits_" + append +  "_Overlap.png")
    
# raw_input("Press Enter to continue...")
