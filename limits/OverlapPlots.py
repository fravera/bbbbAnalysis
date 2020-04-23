from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph, TLegend
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array

inputFileName = "Full/Limits_statOnly.root"
massList  = [90.  , 125., 300.   , 600.  , 1000.  ]
colorList = [ROOT.kBlue, ROOT.kRed, ROOT.kOrange, ROOT.kGreen, ROOT.kViolet]

inputFile = TFile(inputFileName)
theCanvas = TCanvas("limitMapCentral", "limitMapCentral", 1400, 800)
theCanvas.SetLogy()
firstValue = True

for (massY, color) in zip(massList, colorList):
    inputGraphName = "CentralLimit_massY_" + str(massY)
    theGraph = inputFile.Get(inputGraphName)
    theGraph.SetLineColor(color)
    theGraph.SetMarkerColor(color)
    theGraph.SetLineSize(3)
    theGraph.SetMarkerStyle(20)
    theGraph.SetMarkerSize(0.4)
    if firstValue:
        theGraph.Draw("apl")
        firstValue = False
    else:
        theGraph.Draw("same pl")

    
raw_input("Press Enter to continue...")
