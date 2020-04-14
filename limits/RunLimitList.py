from ROOT import TH2D, TCanvas
import subprocess
from array import array
import os
import argparse
import os.path

selection              = "selectionbJets_SignalRegion"
inputFile              = "2016DataPlots_NMSSM_XYH_bbbb_Fast_triggerTurnOnCut/outPlotter.root"
# inputFile              = "2016DataPlots_NMSSM_XYH_bbbb_Fast_dataTriggerEfficiency/outPlotter.root"
bbbbAnalysisFolderPath = "/uscms/home/fravera/nobackup/DiHiggs_v1/CMSSW_10_2_5/src/bbbbAnalysis"
backgroundDataset      = "data_BTagCSV_dataDriven"
dataDataset            = "data_BTagCSV"
inputFileFullPath = bbbbAnalysisFolderPath + "/" + inputFile

 
signalList = [
    "sig_NMSSM_bbbb_MX_300_MY_60" ,
    "sig_NMSSM_bbbb_MX_300_MY_70" ,
    "sig_NMSSM_bbbb_MX_300_MY_80" ,
    "sig_NMSSM_bbbb_MX_300_MY_90" ,
    "sig_NMSSM_bbbb_MX_300_MY_100",
    "sig_NMSSM_bbbb_MX_300_MY_125",
    "sig_NMSSM_bbbb_MX_300_MY_150",
    "sig_NMSSM_bbbb_MX_400_MY_60" ,
    "sig_NMSSM_bbbb_MX_400_MY_70" ,
    "sig_NMSSM_bbbb_MX_400_MY_80" ,
    "sig_NMSSM_bbbb_MX_400_MY_90" ,
    "sig_NMSSM_bbbb_MX_400_MY_100",
    "sig_NMSSM_bbbb_MX_400_MY_125",
    "sig_NMSSM_bbbb_MX_400_MY_150",
    "sig_NMSSM_bbbb_MX_400_MY_200",
    "sig_NMSSM_bbbb_MX_400_MY_250",
    "sig_NMSSM_bbbb_MX_500_MY_60" ,
    "sig_NMSSM_bbbb_MX_500_MY_70" ,
    "sig_NMSSM_bbbb_MX_500_MY_80" ,
    "sig_NMSSM_bbbb_MX_500_MY_90" ,
    "sig_NMSSM_bbbb_MX_500_MY_100",
    "sig_NMSSM_bbbb_MX_500_MY_125",
    "sig_NMSSM_bbbb_MX_500_MY_150",
    "sig_NMSSM_bbbb_MX_500_MY_200",
    "sig_NMSSM_bbbb_MX_500_MY_250",
    "sig_NMSSM_bbbb_MX_500_MY_300",
    "sig_NMSSM_bbbb_MX_600_MY_60" ,
    "sig_NMSSM_bbbb_MX_600_MY_70" ,
    "sig_NMSSM_bbbb_MX_600_MY_80" ,
    "sig_NMSSM_bbbb_MX_600_MY_90" ,
    "sig_NMSSM_bbbb_MX_600_MY_100",
    "sig_NMSSM_bbbb_MX_600_MY_125",
    "sig_NMSSM_bbbb_MX_600_MY_150",
    "sig_NMSSM_bbbb_MX_600_MY_200",
    "sig_NMSSM_bbbb_MX_600_MY_250",
    "sig_NMSSM_bbbb_MX_600_MY_300",
    "sig_NMSSM_bbbb_MX_600_MY_400",
    "sig_NMSSM_bbbb_MX_700_MY_60" ,
    "sig_NMSSM_bbbb_MX_700_MY_70" ,
    "sig_NMSSM_bbbb_MX_700_MY_80" ,
    "sig_NMSSM_bbbb_MX_700_MY_90" ,
    "sig_NMSSM_bbbb_MX_700_MY_100",
    "sig_NMSSM_bbbb_MX_700_MY_125",
    "sig_NMSSM_bbbb_MX_700_MY_150",
    "sig_NMSSM_bbbb_MX_700_MY_200",
    "sig_NMSSM_bbbb_MX_700_MY_250",
    "sig_NMSSM_bbbb_MX_700_MY_300",
    "sig_NMSSM_bbbb_MX_700_MY_400",
    "sig_NMSSM_bbbb_MX_700_MY_500",
    "sig_NMSSM_bbbb_MX_800_MY_60" ,
    "sig_NMSSM_bbbb_MX_800_MY_70" ,
    "sig_NMSSM_bbbb_MX_800_MY_80" ,
    "sig_NMSSM_bbbb_MX_800_MY_90" ,
    "sig_NMSSM_bbbb_MX_800_MY_100",
    "sig_NMSSM_bbbb_MX_800_MY_125",
    "sig_NMSSM_bbbb_MX_800_MY_150",
    "sig_NMSSM_bbbb_MX_800_MY_200",
    "sig_NMSSM_bbbb_MX_800_MY_250",
    "sig_NMSSM_bbbb_MX_800_MY_300",
    "sig_NMSSM_bbbb_MX_800_MY_400",
    "sig_NMSSM_bbbb_MX_800_MY_500",
    "sig_NMSSM_bbbb_MX_800_MY_600",
    "sig_NMSSM_bbbb_MX_900_MY_60" ,
    "sig_NMSSM_bbbb_MX_900_MY_70" ,
    "sig_NMSSM_bbbb_MX_900_MY_80" ,
    "sig_NMSSM_bbbb_MX_900_MY_90" ,
    "sig_NMSSM_bbbb_MX_900_MY_100",
    "sig_NMSSM_bbbb_MX_900_MY_125",
    "sig_NMSSM_bbbb_MX_900_MY_150",
    "sig_NMSSM_bbbb_MX_900_MY_200",
    "sig_NMSSM_bbbb_MX_900_MY_250",
    "sig_NMSSM_bbbb_MX_900_MY_300",
    "sig_NMSSM_bbbb_MX_900_MY_400",
    "sig_NMSSM_bbbb_MX_900_MY_500",
    "sig_NMSSM_bbbb_MX_900_MY_600",
    "sig_NMSSM_bbbb_MX_900_MY_700"
]

combineArguments = "-M AsymptoticLimits -D data_BTagCSV -t -1 --run blind"

xBinning = [250, 350, 450, 550, 650, 750, 850, 950]
yBinning = [55, 65, 75, 85, 95, 112.5, 137.5, 175, 225, 275, 350, 450, 550, 650, 750]

centralLimitMap = TH2D("CentralLimitMap","Central Limit [pb]; m_{X} [GeV]; m_{Y} [GeV]", len(xBinning)-1, array('d',xBinning), len(yBinning)-1, array('d',yBinning))


parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--runAll'   , dest='runAll'   , help='Run all steps'   , action='store_true', default = False, required = False)
parser.add_argument('--datacard' , dest='datacard' , help='Create datacard' , action='store_true', default = False, required = False)
parser.add_argument('--workspace', dest='workspace', help='Create workspace', action='store_true', default = False, required = False)
parser.add_argument('--limits'   , dest='limits'   , help='Run limits'      , action='store_true', default = False, required = False)
args = parser.parse_args()

if args.runAll:
    createDatacard  = True
    createWorkSpace = True
    runLimits       = True
else:
    createDatacard  = args.datacard
    createWorkSpace = args.workspace
    runLimits       = args.limits
 

for signal in signalList:

    print "Current signal = ", signal
    massX =  signal[ signal.find("_MX_") + len("_MX_"): signal.find("_MY_" ) ]
    massY =  signal[ signal.find("_MY_") + len("_MY_"): ]

    datacardName = "limits/datacard_" + signal + ".txt"
    removeOldDatacard = "rm -f " + datacardName
    os.system(removeOldDatacard)
    workspaceName = datacardName[:-4] + ".root"
    removeOldWorkspace = "rm -f " + workspaceName
    os.system(removeOldWorkspace)

    if createDatacard:
        makeDatacardCommand = "python limits/make_datacard_NMMSM_XYH_bbbb.py --sel " + selection + " --fileIn " + inputFileFullPath + " --bkgs " + backgroundDataset + " --sigs " + signal + " --cardOut " + datacardName
        os.system(makeDatacardCommand)

    if os.path.isfile('filename.txt'):
        print "File ", datacardName, " does not exist, skipping"
        continue

    if createWorkSpace:
        massPoint = massX * 10000 + massY
        createWorkspaceCommand = "text2workspace.py " + datacardName + " -D " + dataDataset + " -m " + massPoint
        os.system(createWorkspaceCommand)

    if runLimits:
        combineCommand = "combine " + workspaceName + " " + combineArguments

        out = subprocess.Popen(combineCommand.split(), 
            stdout=subprocess.PIPE, 
            stderr=subprocess.STDOUT)

        stdout,stderr = out.communicate()
        print(stdout)
        if stderr != None :
            print(stderr)
            break

        centralLimitTag = "Expected 50.0%: r < "
        for line in stdout.splitlines():
            if centralLimitTag in line:
                centralLimit = line[line.find(centralLimitTag) + len(centralLimitTag) : ]
                centralLimitMap.Fill(float(massX),float(massY),float(centralLimit))

if runLimits:
    theCanvas = TCanvas("centralLimitMap", "centralLimitMap", 1400, 800)
    centralLimitMap.Draw("colz text")
    centralLimitMap.SetStats(0)
    centralLimitMap.SetMaximum(2)
    centralLimitMap.SetMinimum(0.01)
    theCanvas.SetLogz()
    theCanvas.SaveAs("centralLimitMap.png")
    theCanvas.SaveAs("centralLimitMap.root")

    raw_input("Press Enter to continue...")
