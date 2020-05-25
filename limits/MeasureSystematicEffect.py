from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph, TLegend, TAxis
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array

# ROOT.gROOT.SetBatch(True)

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--year'   , dest='year'   , help='year'   , required = True)
args = parser.parse_args()
year = args.year

statOnlyInputFileName = "Limits" + year + "Limits_statOnly.root"
systematicInputFileName = "Limits" + year + "Limits_syst.root"

statOnlyInputFile = ROOT.TFile(statOnlyInputFileName)
systematicInputFile = ROOT.TFile(systematicInputFileName)

statOnlyInputHistogram = statOnlyInputFile.Get("LimitMapCentral")
statOnlyInputHistogram.SetDirectory(0)
systematicInputHistogram = systematicInputFile.Get("LimitMapCentral")
systematicInputHistogram.SetDirectory(0)

outputPlot = systematicInputHistogram.Clone("SystematicEffect")
nXBins = systematicInputHistogram.GetNbinsX()+1
nYBins = systematicInputHistogram.GetNbinsY()+1

for xBin in range(1, nXBins):
    for yBin in range(1, nYBins):
        if statOnlyInputHistogram.GetBinContent(xBin,yBin) == 0: 
            outputPlot.SetBinContent(xBin,yBin, -100.)
        else:
            outputPlot.SetBinContent(xBin,yBin, (systematicInputHistogram.GetBinContent(xBin,yBin) / statOnlyInputHistogram.GetBinContent(xBin,yBin) -1.)*100.)

theCanvas = TCanvas("SystematicEffect", "SystematicEffect", 1200, 800)
outputPlot.Draw("colz text")
outputPlot.SetMinimum(0.)
outputPlot.SetMaximum(50.)

theCanvas.SaveAs("SystematicEffect" + year + ".png")
