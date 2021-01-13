from ROOT import TH2D, TH1D, TCanvas, TFile, TGraphAsymmErrors, TGraph, TLegend, TAxis
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
parser.add_argument('--impacts', dest = 'impacts', help = 'Measure impacts', action='store_true', default=False)
args = parser.parse_args()

yearList = ["2016", "2017", "2018"]

LimitOptions = ["freeze_CMS_bkgnorm", "freeze_CMS_bkgShape", "freeze_lumi_13TeV", "freeze_CMS_l1prefiring", "freeze_CMS_trg_eff", "freeze_CMS_eff_b_b", "freeze_CMS_eff_b_c", "freeze_CMS_eff_b_udsg", "freeze_CMS_PU", "freeze_CMS_scale_j_Total", "freeze_CMS_res_j", "freeze_CMS_res_j_breg", "freeze_autoMCStats"]

inputFile  = ROOT.TFile(args.input)

for year in yearList:
    statOnlyHistogramName = "Limits_%s/Option_statOnly/LimitMapCentral_%s_statOnly" % (year, year)
    statOnlyHistogram = inputFile.Get(statOnlyHistogramName)
    systHistogramName = "Limits_%s/Option_syst/LimitMapCentral_%s_syst" % (year, year)
    systHistogram = inputFile.Get(systHistogramName)
    systHistogram.Add(statOnlyHistogram, -1.)
    systHistogram.Divide(statOnlyHistogram)
    systHistogram.SetNameTitle("SistematicImpact_%s_%s" %(year, "syst"), "Sistematic Impact - %s - %s" %(year, "syst"))
    theCanvas = TCanvas("SistematicImpact_%s_%s" %(year, "syst"), "Sistematic Impact - %s - %s" %(year, "syst"), 1200, 800)
    systHistogram.SetStats(False)
    systHistogram.Draw("colz")
    theCanvas.SaveAs("SistematicImpact_%s_%s.png" %(year, "syst"))

    if args.impacts:
        for option in LimitOptions:
            optionHistogramName = "Limits_%s/Option_%s/LimitMapCentral_%s_%s" % (year, option, year, option)
            print optionHistogramName
            optionHistogram = inputFile.Get(optionHistogramName)
            # systematicEffect = TH2D()
            # systematicEffect.Add(optionHistogram, statOnlyHistogram, 1., -1.)
            optionHistogram.Add(statOnlyHistogram, -1.)
            optionHistogram.Divide(statOnlyHistogram)
            optionHistogram.Add(systHistogram, optionHistogram, 1., -1.)
            optionHistogram.Divide(systHistogram)
            optionHistogram.SetNameTitle("SistematicImpact_%s_%s" %(year, option), "Sistematic Impact - %s - %s" %(year, option))
            theCanvas = TCanvas("SistematicImpact_%s_%s" %(year, option), "Sistematic Impact - %s - %s" %(year, option), 1200, 800)
            optionHistogram.SetStats(False)
            optionHistogram.SetMinimum(0.)
            optionHistogram.SetMaximum(1.)
            optionHistogram.Draw("colz")

            theCanvas.SaveAs("SistematicImpact_%s_%s.png" %(year, option))



# statOnlyInputFileName = "Limits" + year + "Limits_statOnly.root"
# systematicInputFileName = "Limits" + year + "Limits_syst.root"

# statOnlyInputFile = ROOT.TFile(statOnlyInputFileName)
# systematicInputFile = ROOT.TFile(systematicInputFileName)

# statOnlyInputHistogram = statOnlyInputFile.Get("LimitMapCentral")
# statOnlyInputHistogram.SetDirectory(0)
# systematicInputHistogram = systematicInputFile.Get("LimitMapCentral")
# systematicInputHistogram.SetDirectory(0)
# systematicInputHistogramUp = systematicInputFile.Get("LimitMap1sigmaUp")
# systematicInputHistogramUp.SetDirectory(0)
# systematicInputHistogramDown = systematicInputFile.Get("LimitMap1sigmaDown")
# systematicInputHistogramDown.SetDirectory(0)

# outputPlot = systematicInputHistogram.Clone("SystematicEffect")
# nXBins = systematicInputHistogram.GetNbinsX()+1
# nYBins = systematicInputHistogram.GetNbinsY()+1


# listOfXmasses = [300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1400, 1600, 1800, 2000]
# listOfYmasses = [60, 70, 80, 90, 100, 125, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800]

# for xBin in range(1, nXBins):
#     print "\\begin{table}[htb]"
#     print "\centering"
#     print "\\begin{tabular}{c|c|c|c|c}"
#     for yBin in range(1, nYBins):
#         xMass          = listOfXmasses[xBin-1]
#         yMass          = listOfYmasses[yBin-1]
#         if (xMass - yMass < 125) : continue
#         if (xMass > 1000 and yMass < 90) : continue
#         statOnlyLimit  = statOnlyInputHistogram      .GetBinContent(xBin,yBin)
#         systLimit      = systematicInputHistogram    .GetBinContent(xBin,yBin)
#         systLimitUp    = systematicInputHistogramUp  .GetBinContent(xBin,yBin)
#         systLimitDown  = systematicInputHistogramDown.GetBinContent(xBin,yBin)
#         limitDeltaUp   = systLimitUp - systLimit
#         limitDeltaDown = systLimit - systLimitDown

#         if statOnlyLimit == 0: increment = 0
#         else                 : increment = (systLimit / statOnlyLimit -1.)*100.
#         outputPlot.SetBinContent(xBin,yBin, increment)
#         print str(xMass)+ "\t&\t"+ str(yMass)+ "\t&\t$"+ str(round(systLimit, 2))+ "^{+"+ str(round(limitDeltaUp, 2))+ "}_{-"+ str(round(limitDeltaDown, 2))+ "}$\t&\t"+ str(round(increment, 2))+ "\%\t\\\\"
#     print "\\end{tabular}"
#     print "\\caption{\\label{results:tab:"+ str(year)+ "SystLimits_Mx_"+ str(xMass)+ "} Limit with systematics for "+ str(year)+  " for \\mX = " +  str(xMass) +  " GeV signal hypothesis.}"
#     print "\\end{table}"
#     print 
#     print


# theCanvas = TCanvas("SystematicEffect", "SystematicEffect", 1200, 800)
# outputPlot.Draw("colz")
# outputPlot.SetMinimum(0.)
# outputPlot.SetMaximum(50.)

# theCanvas.SaveAs("SystematicEffect" + year + ".png")
