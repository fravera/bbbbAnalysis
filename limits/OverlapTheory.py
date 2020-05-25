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
parser.add_argument('--year'   , dest='year'   , help='year'   , required = True)
parser.add_argument('--systematics'   , dest='systematics'   , help='systematics'   , action="store_true", default = False, required = False)

args = parser.parse_args()
year = args.year
append = "statOnly"
if args.systematics : append = "syst"

inputFileName = "Limits" + year + "Limits_" + append +  ".root"
inputTheoryFileName = "/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_5/src/bbbbAnalysis/HXSG_NMSSM_recommendations_00.root"
inputTheoryPlotName = "g_bbbb"
massList  = [100.]
colorList = [ROOT.kBlack]

inputFile = TFile(inputFileName)
theCanvas = TCanvas("limitMapCentral", "limitMapCentral", 1200, 800)
theLegend  = TLegend(0.37,0.5,0.88,0.88)
theLegend.SetTextSize(0.04)
# theLegend.SetNColumns(2);

theCanvas.SetLogy()

for (massY, color) in zip(massList, colorList):
    inputGraph2sigmaName = "2SigmaLimit_massY_" + str(massY)
    theGraph2sigma = inputFile.Get(inputGraph2sigmaName)
    theGraph2sigma.SetTitle("")
    theGraph2sigma.GetXaxis().SetTitle("m_{X} [GeV]")
    theGraph2sigma.GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
    theGraph2sigma.GetYaxis().SetRangeUser(1.,1.e4)
    theGraph2sigma.GetXaxis().SetRangeUser(300.,1100.)
    theGraph2sigma.Draw("a3")

    inputGraph1sigmaName = "1SigmaLimit_massY_" + str(massY)
    theGraph1sigma = inputFile.Get(inputGraph1sigmaName)
    theGraph1sigma.Draw("same 3")

    inputGraphName = "CentralLimit_massY_" + str(massY)
    theGraph = inputFile.Get(inputGraphName)
    theGraph.SetLineColor(color)
    theGraph.SetMarkerColor(color)
    theGraph.SetLineWidth(2)
    theGraph.SetMarkerStyle(20)
    theGraph.SetMarkerSize(0.7)
    theGraph.Draw("same pl")

    theLegend.AddEntry(theGraph, "Median expected, m_{Y} = " + str(int(massY)) + " GeV", "lp")
    theLegend.AddEntry(theGraph1sigma, "68% expected"                             , "f" )
    theLegend.AddEntry(theGraph2sigma, "95% expected"                             , "f" )

inputTheoryFile = TFile(inputTheoryFileName)
theTheoryGraph = inputTheoryFile.Get(inputTheoryPlotName)
theScaledTheoryGraph = TGraphAsymmErrors()
for it in range(0,theTheoryGraph.GetN()):
    pointX = ROOT.Double(0)
    pointY = ROOT.Double(0)
    theTheoryGraph.GetPoint(it, pointX, pointY)
    errorXup   = theTheoryGraph.GetErrorXhigh(it)
    errorXdown = theTheoryGraph.GetErrorXlow (it)
    errorYup   = theTheoryGraph.GetErrorYhigh(it)
    errorYdown = theTheoryGraph.GetErrorYlow (it)
    theScaledTheoryGraph.SetPoint(it, pointX, pointY*1000.)
    theScaledTheoryGraph.SetPointError(it, errorXdown, errorXup, errorYdown*1000., errorYup*1000.)
theScaledTheoryGraph.SetLineColor(ROOT.kBlue)
theScaledTheoryGraph.SetLineWidth(2)
theScaledTheoryGraph.SetFillStyle(3002)
theScaledTheoryGraph.SetFillColor(ROOT.kBlue)
theScaledTheoryGraph.Draw("same pl3")
inputTheoryFile.Close()

inputFile.Close()

theLegend.AddEntry(theScaledTheoryGraph, "#splitline{(pp #rightarrow H_{MSSM}) #times BR(H_{S}(b#bar{b}) h(b#bar{b}))}{m(H_{S}) = 100-110 GeV}", "plf")

theLegend.Draw("same")

theCanvas.SaveAs("Limits" + year + "Limits_" + append +  "_Theory.png")
