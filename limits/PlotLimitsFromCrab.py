from ROOT import TH2D, TCanvas, TFile, TGraphAsymmErrors, TGraph
import ROOT
import subprocess
from array import array
import os
import argparse
import os.path
from array import array
import time

enableSystematics = True

bbbbAnalysisFolderPath = "/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_5/src/bbbbAnalysis"
dataDataset            = "data_BTagCSV"

selection              = "selectionbJets_SignalRegion"
backgroundDataset      = "data_BTagCSV_dataDriven"
xBinning = [250, 350, 450, 550, 650, 750, 850, 950, 1050, 1150, 1300, 1500, 1700, 1900, 2100]
yBinning = [55, 65, 75, 85, 95, 112.5, 137.5, 175, 225, 275, 350, 450, 550, 650, 750, 850, 950, 1100, 1300, 1500, 1700, 1900]
# signalList = [
#     "sig_NMSSM_bbbb_MX_300_MY_125", "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_500_MY_125", "sig_NMSSM_bbbb_MX_700_MY_125", "sig_NMSSM_bbbb_MX_800_MY_125", "sig_NMSSM_bbbb_MX_900_MY_125", "sig_NMSSM_bbbb_MX_1000_MY_125", "sig_NMSSM_bbbb_MX_1100_MY_125", "sig_NMSSM_bbbb_MX_1200_MY_125", "sig_NMSSM_bbbb_MX_1400_MY_125", "sig_NMSSM_bbbb_MX_1600_MY_125", "sig_NMSSM_bbbb_MX_1800_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_125"
# ]

signalList = [
      "sig_NMSSM_bbbb_MX_300_MY_60", "sig_NMSSM_bbbb_MX_300_MY_70", "sig_NMSSM_bbbb_MX_300_MY_80", "sig_NMSSM_bbbb_MX_300_MY_90", "sig_NMSSM_bbbb_MX_300_MY_100", "sig_NMSSM_bbbb_MX_300_MY_125"
    , "sig_NMSSM_bbbb_MX_400_MY_60", "sig_NMSSM_bbbb_MX_400_MY_70", "sig_NMSSM_bbbb_MX_400_MY_80", "sig_NMSSM_bbbb_MX_400_MY_90", "sig_NMSSM_bbbb_MX_400_MY_100", "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_400_MY_150", "sig_NMSSM_bbbb_MX_400_MY_200", "sig_NMSSM_bbbb_MX_400_MY_250"
    , "sig_NMSSM_bbbb_MX_500_MY_60", "sig_NMSSM_bbbb_MX_500_MY_70", "sig_NMSSM_bbbb_MX_500_MY_80", "sig_NMSSM_bbbb_MX_500_MY_90", "sig_NMSSM_bbbb_MX_500_MY_100", "sig_NMSSM_bbbb_MX_500_MY_125", "sig_NMSSM_bbbb_MX_500_MY_150", "sig_NMSSM_bbbb_MX_500_MY_200", "sig_NMSSM_bbbb_MX_500_MY_250", "sig_NMSSM_bbbb_MX_500_MY_300"
    , "sig_NMSSM_bbbb_MX_600_MY_60", "sig_NMSSM_bbbb_MX_600_MY_70", "sig_NMSSM_bbbb_MX_600_MY_80", "sig_NMSSM_bbbb_MX_600_MY_90", "sig_NMSSM_bbbb_MX_600_MY_100", "sig_NMSSM_bbbb_MX_600_MY_125", "sig_NMSSM_bbbb_MX_600_MY_150", "sig_NMSSM_bbbb_MX_600_MY_200", "sig_NMSSM_bbbb_MX_600_MY_250", "sig_NMSSM_bbbb_MX_600_MY_300", "sig_NMSSM_bbbb_MX_600_MY_400"
    , "sig_NMSSM_bbbb_MX_700_MY_60", "sig_NMSSM_bbbb_MX_700_MY_70", "sig_NMSSM_bbbb_MX_700_MY_80", "sig_NMSSM_bbbb_MX_700_MY_90", "sig_NMSSM_bbbb_MX_700_MY_100", "sig_NMSSM_bbbb_MX_700_MY_125", "sig_NMSSM_bbbb_MX_700_MY_150", "sig_NMSSM_bbbb_MX_700_MY_200", "sig_NMSSM_bbbb_MX_700_MY_250", "sig_NMSSM_bbbb_MX_700_MY_300", "sig_NMSSM_bbbb_MX_700_MY_400", "sig_NMSSM_bbbb_MX_700_MY_500"
    , "sig_NMSSM_bbbb_MX_800_MY_60", "sig_NMSSM_bbbb_MX_800_MY_70", "sig_NMSSM_bbbb_MX_800_MY_80", "sig_NMSSM_bbbb_MX_800_MY_90", "sig_NMSSM_bbbb_MX_800_MY_100", "sig_NMSSM_bbbb_MX_800_MY_125", "sig_NMSSM_bbbb_MX_800_MY_150", "sig_NMSSM_bbbb_MX_800_MY_200", "sig_NMSSM_bbbb_MX_800_MY_250", "sig_NMSSM_bbbb_MX_800_MY_300", "sig_NMSSM_bbbb_MX_800_MY_400", "sig_NMSSM_bbbb_MX_800_MY_500", "sig_NMSSM_bbbb_MX_800_MY_600"
    , "sig_NMSSM_bbbb_MX_900_MY_60", "sig_NMSSM_bbbb_MX_900_MY_70", "sig_NMSSM_bbbb_MX_900_MY_80", "sig_NMSSM_bbbb_MX_900_MY_90", "sig_NMSSM_bbbb_MX_900_MY_100", "sig_NMSSM_bbbb_MX_900_MY_125", "sig_NMSSM_bbbb_MX_900_MY_150", "sig_NMSSM_bbbb_MX_900_MY_200", "sig_NMSSM_bbbb_MX_900_MY_250", "sig_NMSSM_bbbb_MX_900_MY_300", "sig_NMSSM_bbbb_MX_900_MY_400", "sig_NMSSM_bbbb_MX_900_MY_500", "sig_NMSSM_bbbb_MX_900_MY_600", "sig_NMSSM_bbbb_MX_900_MY_700"
    , "sig_NMSSM_bbbb_MX_1000_MY_60", "sig_NMSSM_bbbb_MX_1000_MY_70", "sig_NMSSM_bbbb_MX_1000_MY_80", "sig_NMSSM_bbbb_MX_1000_MY_90", "sig_NMSSM_bbbb_MX_1000_MY_100", "sig_NMSSM_bbbb_MX_1000_MY_125", "sig_NMSSM_bbbb_MX_1000_MY_150", "sig_NMSSM_bbbb_MX_1000_MY_200", "sig_NMSSM_bbbb_MX_1000_MY_250", "sig_NMSSM_bbbb_MX_1000_MY_300", "sig_NMSSM_bbbb_MX_1000_MY_400", "sig_NMSSM_bbbb_MX_1000_MY_500", "sig_NMSSM_bbbb_MX_1000_MY_600", "sig_NMSSM_bbbb_MX_1000_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100"
    , "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125", "sig_NMSSM_bbbb_MX_1100_MY_150", "sig_NMSSM_bbbb_MX_1100_MY_200", "sig_NMSSM_bbbb_MX_1100_MY_250", "sig_NMSSM_bbbb_MX_1100_MY_300", "sig_NMSSM_bbbb_MX_1100_MY_400", "sig_NMSSM_bbbb_MX_1100_MY_500", "sig_NMSSM_bbbb_MX_1100_MY_600", "sig_NMSSM_bbbb_MX_1100_MY_700", "sig_NMSSM_bbbb_MX_1100_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_900"
    , "sig_NMSSM_bbbb_MX_1200_MY_90", "sig_NMSSM_bbbb_MX_1200_MY_100", "sig_NMSSM_bbbb_MX_1200_MY_125", "sig_NMSSM_bbbb_MX_1200_MY_150", "sig_NMSSM_bbbb_MX_1200_MY_200", "sig_NMSSM_bbbb_MX_1200_MY_250", "sig_NMSSM_bbbb_MX_1200_MY_300", "sig_NMSSM_bbbb_MX_1200_MY_400", "sig_NMSSM_bbbb_MX_1200_MY_500", "sig_NMSSM_bbbb_MX_1200_MY_600", "sig_NMSSM_bbbb_MX_1200_MY_700", "sig_NMSSM_bbbb_MX_1200_MY_800", "sig_NMSSM_bbbb_MX_1200_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_125"
    , "sig_NMSSM_bbbb_MX_1400_MY_90", "sig_NMSSM_bbbb_MX_1400_MY_100", "sig_NMSSM_bbbb_MX_1400_MY_150", "sig_NMSSM_bbbb_MX_1400_MY_200", "sig_NMSSM_bbbb_MX_1400_MY_250", "sig_NMSSM_bbbb_MX_1400_MY_300", "sig_NMSSM_bbbb_MX_1400_MY_400", "sig_NMSSM_bbbb_MX_1400_MY_500", "sig_NMSSM_bbbb_MX_1400_MY_600", "sig_NMSSM_bbbb_MX_1400_MY_700", "sig_NMSSM_bbbb_MX_1400_MY_800", "sig_NMSSM_bbbb_MX_1400_MY_900", "sig_NMSSM_bbbb_MX_1400_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_125"
    , "sig_NMSSM_bbbb_MX_1600_MY_90", "sig_NMSSM_bbbb_MX_1600_MY_100", "sig_NMSSM_bbbb_MX_1600_MY_150", "sig_NMSSM_bbbb_MX_1600_MY_200", "sig_NMSSM_bbbb_MX_1600_MY_250", "sig_NMSSM_bbbb_MX_1600_MY_300", "sig_NMSSM_bbbb_MX_1600_MY_400", "sig_NMSSM_bbbb_MX_1600_MY_500", "sig_NMSSM_bbbb_MX_1600_MY_600", "sig_NMSSM_bbbb_MX_1600_MY_700", "sig_NMSSM_bbbb_MX_1600_MY_800", "sig_NMSSM_bbbb_MX_1600_MY_900", "sig_NMSSM_bbbb_MX_1600_MY_1000", "sig_NMSSM_bbbb_MX_1600_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_125"
    , "sig_NMSSM_bbbb_MX_1800_MY_90", "sig_NMSSM_bbbb_MX_1800_MY_100", "sig_NMSSM_bbbb_MX_1800_MY_150", "sig_NMSSM_bbbb_MX_1800_MY_200", "sig_NMSSM_bbbb_MX_1800_MY_250", "sig_NMSSM_bbbb_MX_1800_MY_300", "sig_NMSSM_bbbb_MX_1800_MY_400", "sig_NMSSM_bbbb_MX_1800_MY_500", "sig_NMSSM_bbbb_MX_1800_MY_600", "sig_NMSSM_bbbb_MX_1800_MY_700", "sig_NMSSM_bbbb_MX_1800_MY_800", "sig_NMSSM_bbbb_MX_1800_MY_900", "sig_NMSSM_bbbb_MX_1800_MY_1000", "sig_NMSSM_bbbb_MX_1800_MY_1200", "sig_NMSSM_bbbb_MX_1800_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_1600"
    , "sig_NMSSM_bbbb_MX_2000_MY_90", "sig_NMSSM_bbbb_MX_2000_MY_100", "sig_NMSSM_bbbb_MX_2000_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_150", "sig_NMSSM_bbbb_MX_2000_MY_200", "sig_NMSSM_bbbb_MX_2000_MY_250", "sig_NMSSM_bbbb_MX_2000_MY_300", "sig_NMSSM_bbbb_MX_2000_MY_400", "sig_NMSSM_bbbb_MX_2000_MY_500", "sig_NMSSM_bbbb_MX_2000_MY_600", "sig_NMSSM_bbbb_MX_2000_MY_700", "sig_NMSSM_bbbb_MX_2000_MY_800", "sig_NMSSM_bbbb_MX_2000_MY_900", "sig_NMSSM_bbbb_MX_2000_MY_1000", "sig_NMSSM_bbbb_MX_2000_MY_1200", "sig_NMSSM_bbbb_MX_2000_MY_1400", "sig_NMSSM_bbbb_MX_2000_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_1800"
]


if enableSystematics : 
    combineArguments = "-M AsymptoticLimits --run blind"
    outputAppend = "syst"
else                 : 
    combineArguments = "-M AsymptoticLimits --run blind --freezeParameters allConstrainedNuisances "
    outputAppend = "statOnly"


limitMap2sigmaDown = TH2D("LimitMap2sigmaDown","2 Sigma Down Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMap1sigmaDown = TH2D("LimitMap1sigmaDown","1 Sigma Down Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMapCentral    = TH2D("LimitMapCentral"   ,"Central Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMap1sigmaUp   = TH2D("LimitMap1sigmaUp"  ,"1 Sigma Up Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))
limitMap2sigmaUp   = TH2D("LimitMap2sigmaUp"  ,"2 Sigma Up Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]; #sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--year', dest='year', help='Output Directory', default = ""   , required = True)
parser.add_argument('--getOutput', dest='getOutput', help='getOutput', action='store_true', default = False, required = False)
args = parser.parse_args()

year = args.year


yMassesList = []

brazilianPlotList = {}
for signal in signalList:
    massYTmp =  float(signal[ signal.find("_MY_") + len("_MY_"): ])
    if massYTmp not in yMassesList: 
        # yMassesList.append(massYTmp)
        brazilianPlotList[massYTmp] = [
            TGraph(),
            TGraphAsymmErrors(),
            TGraphAsymmErrors()
        ]
        brazilianPlotList[massYTmp][0].SetNameTitle("CentralLimit_massY_" + str(massYTmp), " Central Limit - m_{X} = " + str(massYTmp))
        brazilianPlotList[massYTmp][0].GetXaxis().SetTitle("m_{X} [GeV]")
        brazilianPlotList[massYTmp][0].SetMaximum(10000)
        brazilianPlotList[massYTmp][0].SetMinimum(1)
        brazilianPlotList[massYTmp][0].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
        brazilianPlotList[massYTmp][1].SetNameTitle("1SigmaLimit_massY_"  + str(massYTmp), " 1 Sigma Limit - m_{X} = " + str(massYTmp))
        brazilianPlotList[massYTmp][0].SetMaximum(10000)
        brazilianPlotList[massYTmp][0].SetMinimum(1)
        brazilianPlotList[massYTmp][1].GetXaxis().SetTitle("m_{X} [GeV]")
        brazilianPlotList[massYTmp][1].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
        brazilianPlotList[massYTmp][2].SetNameTitle("2SigmaLimit_massY_"  + str(massYTmp), " 2 Sigma Limit - m_{X} = " + str(massYTmp))
        brazilianPlotList[massYTmp][0].SetMaximum(10000)
        brazilianPlotList[massYTmp][0].SetMinimum(1)
        brazilianPlotList[massYTmp][2].GetXaxis().SetTitle("m_{X} [GeV]")
        brazilianPlotList[massYTmp][2].GetYaxis().SetTitle("#sigma(pp #rightarrow X) #times BR(Y(b#bar{b}) H(b#bar{b})) [fb]")
        brazilianPlotList[massYTmp][1].SetFillColor(ROOT.kGreen)
        brazilianPlotList[massYTmp][2].SetFillColor(ROOT.kYellow)

outputFolderName = "Limits" + str(year)

for signal in signalList:

    print "Current signal = ", signal
    massXstring =  signal[ signal.find("_MX_") + len("_MX_"): signal.find("_MY_" ) ]
    massYstring =  signal[ signal.find("_MY_") + len("_MY_"): ]
    massX = float(massXstring)
    massY = float(massYstring)

    signalFolder = "Limits" + str(year) + "/" + signal
    inputFileName = signalFolder + "/higgsCombine.Test.AsymptoticLimits.mH120.root" 

    if args.getOutput or not os.path.exists(inputFileName):
        currentFolder = os.getcwd()
        os.chdir(signalFolder)
        for x in range(0, 10):
            retrieveCommand = "crab getoutput -d crab_combine"
            os.system(retrieveCommand)
            if os.path.exists("crab_combine/results/combine_output_1.tar") : break
        unTarCommand = "tar xf crab_combine/results/combine_output_1.tar"
        os.system(unTarCommand)
        os.chdir(currentFolder)

    if not os.path.exists(inputFileName) : continue

    multiplier = 100.
    if massX >= 600: multiplier = 10.
    if massX >= 1600: multiplier = 1.

    inputFile = ROOT.TFile(inputFileName)
    inputTree = inputFile.Get("limit")
    limit = []
    for entry in inputTree:         
        # Now you have acess to the leaves/branches of each entry in the tree, e.g.
        limit.append(entry.limit * multiplier)
    inputFile.Close()

    limit2SigmaDown = limit[0]
    limit1SigmaDown = limit[1]
    limitCentral    = limit[2]
    limit1SigmaUp   = limit[3]
    limit2SigmaUp   = limit[4]

    limitMap2sigmaDown.Fill(massX,massY,limit2SigmaDown)
    limitMap1sigmaDown.Fill(massX,massY,limit1SigmaDown)
    limitMapCentral   .Fill(massX,massY,limitCentral)
    limitMap1sigmaUp  .Fill(massX,massY,limit1SigmaUp)
    limitMap2sigmaUp  .Fill(massX,massY,limit2SigmaUp)

    brazilianPlotList[massY][0].SetPoint(brazilianPlotList[massY][0].GetN()  , massX, limitCentral)
    brazilianPlotList[massY][1].SetPoint(brazilianPlotList[massY][1].GetN()  , massX, limitCentral)
    brazilianPlotList[massY][2].SetPoint(brazilianPlotList[massY][2].GetN()  , massX,  limitCentral)
    brazilianPlotList[massY][1].SetPointError(brazilianPlotList[massY][1].GetN()-1, 0., 0., limitCentral - limit1SigmaDown, limit1SigmaUp - limitCentral)
    brazilianPlotList[massY][2].SetPointError(brazilianPlotList[massY][2].GetN()-1, 0., 0., limitCentral - limit2SigmaDown, limit2SigmaUp - limitCentral)

theCanvas = TCanvas("limitMapCentral", "limitMapCentral", 1400, 800)
limitMapCentral.Draw("colz")
limitMapCentral.SetStats(0)
limitMapCentral.SetMaximum(2000)
limitMapCentral.SetMinimum(1)
theCanvas.SetLogz()
theCanvas.SaveAs(outputFolderName + "limitMapCentral_" + str(year) + "_" + outputAppend + ".png")

limitMap2sigmaDown.SetStats(0)
limitMap2sigmaDown.SetMaximum(2000)
limitMap2sigmaDown.SetMinimum(1)

limitMap1sigmaDown.SetStats(0)
limitMap1sigmaDown.SetMaximum(2000)
limitMap1sigmaDown.SetMinimum(1)

limitMap1sigmaUp.SetStats(0)
limitMap1sigmaUp.SetMaximum(2000)
limitMap1sigmaUp.SetMinimum(1)

limitMap2sigmaUp.SetStats(0)
limitMap2sigmaUp.SetMaximum(2000)
limitMap2sigmaUp.SetMinimum(1)

outputFile = TFile(outputFolderName + "Limits_" + outputAppend + ".root", "RECREATE")
theCanvas.Write()
limitMap2sigmaDown.Write()
limitMap1sigmaDown.Write()
limitMapCentral   .Write()
limitMap1sigmaUp  .Write()
limitMap2sigmaUp  .Write()

for mappPoint in brazilianPlotList:
    for plot in brazilianPlotList[mappPoint]:
        plot.Write()
    theBrazilianCanvas = TCanvas("BrazilianLimits_massY_" + str(mappPoint), "BrazilianLimits_massY_" + str(mappPoint), 1400, 800)
    brazilianPlotList[mappPoint][2].Draw("a3")
    brazilianPlotList[mappPoint][1].Draw("Same3")
    brazilianPlotList[mappPoint][0].Draw("same")
    theBrazilianCanvas.SetLogy()
    theBrazilianCanvas.Write()

raw_input("Press Enter to continue...")