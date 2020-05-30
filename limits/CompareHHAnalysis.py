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

inputFileName = "/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_13/src/HiggsAnalysis/CombinedLimit/Hig16/hig-17-009-master/spin0/CombineResults_" + append + ".txt"

inputFile = open(inputFileName, "r")

brazilianPlots = {}
brazilianPlots = [
    TGraph(),
    TGraphAsymmErrors(),
    TGraphAsymmErrors()
]
brazilianPlots[0].SetNameTitle("CentralLimit", " Central Limit")
brazilianPlots[0].GetXaxis().SetTitle("m_{X} [GeV]")
brazilianPlots[0].SetMaximum(10000)
brazilianPlots[0].SetMinimum(1)
brazilianPlots[0].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(h(b#bar{b}) h(b#bar{b})) [fb]")
brazilianPlots[1].SetNameTitle("1SigmaLimit", " 1 Sigma Limit")
brazilianPlots[0].SetMaximum(10000)
brazilianPlots[0].SetMinimum(1)
brazilianPlots[1].GetXaxis().SetTitle("m_{X} [GeV]")
brazilianPlots[1].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(h(b#bar{b}) h(b#bar{b})) [fb]")
brazilianPlots[2].SetNameTitle("2SigmaLimit", " 2 Sigma Limit")
brazilianPlots[0].SetMaximum(10000)
brazilianPlots[0].SetMinimum(1)
brazilianPlots[2].GetXaxis().SetTitle("m_{X} [GeV]")
brazilianPlots[2].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(h(b#bar{b}) h(b#bar{b})) [fb]")
brazilianPlots[1].SetFillColor(ROOT.kGreen)
brazilianPlots[2].SetFillColor(ROOT.kYellow)


massPoint          = "Mass point = "
LimitTag2SigmaDown = "Expected  2.5%: r < "
LimitTag1SigmaDown = "Expected 16.0%: r < "
LimitTagCentral    = "Expected 50.0%: r < "
LimitTag1SigmaUp   = "Expected 84.0%: r < "
LimitTag2SigmaUp   = "Expected 97.5%: r < "

multiplicator = 1000.

for line in inputFile:
    if massPoint in line:
        massX = float(line[line.find(massPoint) + len(massPoint) : ])
        if massX >= 580: multiplicator=2.
    if LimitTag2SigmaDown in line:
        limit2SigmaDown = float(line[line.find(LimitTag2SigmaDown) + len(LimitTag2SigmaDown) : ])*multiplicator
    if LimitTag1SigmaDown in line:
        limit1SigmaDown = float(line[line.find(LimitTag1SigmaDown) + len(LimitTag1SigmaDown) : ])*multiplicator
    if LimitTagCentral in line:
        limitCentral    = float(line[line.find(LimitTagCentral) + len(LimitTagCentral) : ])*multiplicator
    if LimitTag1SigmaUp in line:
        limit1SigmaUp   = float(line[line.find(LimitTag1SigmaUp) + len(LimitTag1SigmaUp) : ])*multiplicator
    if LimitTag2SigmaUp in line:
        limit2SigmaUp   = float(line[line.find(LimitTag2SigmaUp) + len(LimitTag2SigmaUp) : ])*multiplicator
        brazilianPlots[0].SetPoint(brazilianPlots[0].GetN()  , massX, limitCentral)
        brazilianPlots[1].SetPoint(brazilianPlots[1].GetN()  , massX, limitCentral)
        brazilianPlots[2].SetPoint(brazilianPlots[2].GetN()  , massX,  limitCentral)
        brazilianPlots[1].SetPointError(brazilianPlots[1].GetN()-1, 0., 0., limitCentral - limit1SigmaDown, limit1SigmaUp - limitCentral)
        brazilianPlots[2].SetPointError(brazilianPlots[2].GetN()-1, 0., 0., limitCentral - limit2SigmaDown, limit2SigmaUp - limitCentral)

inputComparisonName = "Limits" + year + "Limits_" + append +  ".root"

inputComparison = TFile(inputComparisonName)
comparisonPlot = inputComparison.Get("CentralLimit_massY_125.0")
comparisonPlot.SetLineColor(ROOT.kRed)
comparisonPlot.SetMarkerColor(ROOT.kRed)
comparisonPlot.SetLineWidth(3)
comparisonPlot.SetMarkerStyle(20)
comparisonPlot.SetMarkerSize(1.)

outputFile = TFile("HHanalysisComparison" + year + "_" + append +  ".root","RECREATE")
for plot in brazilianPlots:
    plot.Write()
theBrazilianCanvas = TCanvas("BrazilianLimits", "BrazilianLimits", 1200, 800)
brazilianPlots[2].SetTitle("")
brazilianPlots[2].Draw("a3")
brazilianPlots[2].GetYaxis().SetRangeUser(1.,10000.)
brazilianPlots[1].Draw("Same3")
brazilianPlots[0].Draw("same")
comparisonPlot.Draw("pl same")
theBrazilianCanvas.SetLogy()
theBrazilianCanvas.Write()

theLegend  = TLegend(0.4,0.5,0.88,0.88)
theLegend.SetTextSize(0.04)
theLegend.AddEntry(brazilianPlots[0], "2016 X #rightarrow h(bb)h(bb)"         , "lp")
theLegend.AddEntry(brazilianPlots[1], "68% expected"                             , "f" )
theLegend.AddEntry(brazilianPlots[2], "95% expected"                             , "f" )
theLegend.AddEntry(comparisonPlot   , "X #rightarrow Y(bb)H(bb), m_{Y} = 125 GeV", "lp")
theLegend.Draw("same")

theBrazilianCanvas.SaveAs("HHanalysisComparison" + year + "_" + append +  ".png")

