#include "Riostream.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TChain.h"
#include "TLine.h"
#include "thread"
#include <boost/algorithm/string/replace.hpp>

const std::string gGlobalSelection     = "H1_m > 125-20 && H1_m < 125+20";
// std::string gGlobalSelection     = "H1_m > 125-20 && H1_m < 125+20 && H2_m > 125-20 && H2_m < 125+20 && HH_m > 300-40 && HH_m < 300+40";
const std::string gSignalSelection     = "NbJets >= 4";// && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0";
const std::string gBackgroundSelection = "NbJets == 3";
// const std::string gBackgroundSelection = "NbJets >= 4";

std::map<int, std::string> gtriggerCut
{
    {2016, "( (HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched>0) || (HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0) )"},
    {2017, "( (HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_Fired==1 && HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_ObjectMatched>0) || (HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_Fired==1 && HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_ObjectMatched > 0) )"},
    {2018, "(HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_Fired==1 && HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_ObjectMatched>0)"}
};

const std::string gSignalWeights = "bTagScaleFactor_central * triggerScaleFactor";


std::vector<std::string> splitByLine(const std::string& inputFileName)
{
    std::vector<std::string> fileNameList;
    std::ifstream fList (inputFileName);
    if (!fList.good())
    {
        std::cerr << "*** Sample::openFileAndTree : ERROR : could not open file " << inputFileName << std::endl;
        abort();
    }
    std::string line;
    while (std::getline(fList, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) fileNameList.emplace_back(line);
    }
    return fileNameList;
}

std::unique_ptr<TChain> getChainFromFileList(std::vector<std::string>& fileNameList, const std::string& treeName)
{
    std::unique_ptr<TChain> theChain = std::make_unique<TChain>(treeName.c_str());
    for(const auto& fileName : fileNameList) theChain->Add(fileName.c_str());

    return std::move(theChain);
}

std::tuple<float,float,TH2F*> getBestCutForSignal(const TH1F* signalPlot, const TH1F* backgroundPlot)
{
    TH2F *cutRangePlot = new TH2F((std::string(signalPlot->GetName()) + "cutRangePlot").c_str(),(std::string(signalPlot->GetName()) + "cutRangePlot").c_str(), 
        signalPlot->GetNbinsX(), signalPlot->GetXaxis()->GetXmin(), signalPlot->GetXaxis()->GetXmax(), 
        signalPlot->GetNbinsX(), signalPlot->GetXaxis()->GetXmin(), signalPlot->GetXaxis()->GetXmax()
    );
    float lowCutWithMaxDicrimination  = -999;
    float highCutWithMaxDicrimination = -999;
    float maxDicrimination            = 0.;
    for(int minBin=1; minBin<signalPlot->GetNbinsX(); ++minBin)
    {

        float sumSignalBinContent            = 0.;
        float sumBackgroundBinContent        = 0.;
        
        for(int maxBin=minBin; maxBin<=signalPlot->GetNbinsX(); ++maxBin)
        {
            sumSignalBinContent     += signalPlot    ->GetBinContent(maxBin);
            sumBackgroundBinContent += backgroundPlot->GetBinContent(maxBin);
            if(sumBackgroundBinContent <= 0.) continue;

            float currentDicrimination = sumSignalBinContent/sqrt(sumBackgroundBinContent);
            cutRangePlot->Fill(signalPlot->GetXaxis()->GetBinCenter(minBin), signalPlot->GetXaxis()->GetBinCenter (maxBin), currentDicrimination);
            if(currentDicrimination > maxDicrimination)
            {
                maxDicrimination = currentDicrimination;
                lowCutWithMaxDicrimination  = signalPlot->GetXaxis()->GetBinCenter(minBin);
                highCutWithMaxDicrimination = signalPlot->GetXaxis()->GetBinCenter (maxBin);
            }
        }
    }
    // int theMin, theMax, nada;
    // cutRangePlot->GetMaximumBin(theMin, theMax, nada);
    // std::cout << theMin << " - " << theMax << std::endl;
    // std::cout << maxDicrimination << std::endl;
    cutRangePlot->GetXaxis()->SetTitle("min cut");
    cutRangePlot->GetYaxis()->SetTitle("max cut");
    TCanvas cutRangePlotC;
    cutRangePlot->Draw("colz");
    cutRangePlotC.SaveAs("test.png");
    return std::make_tuple(lowCutWithMaxDicrimination, highCutWithMaxDicrimination, cutRangePlot);
}

std::string getMassCut(float yMass, std::string nameForThread, std::unique_ptr<TChain>& signalTree, const std::string& signalSelection, const std::string globalSelection)
{
    std::cout<<(signalSelection  + " && " + globalSelection)<<std::endl;

    float mHH_mean, mHH_sigma, mH2_mean, mH2_sigma;
    std::string plotNameHH = "mHHplot" + nameForThread;
    TH1F *mHHplot = new TH1F(plotNameHH.c_str(),plotNameHH.c_str(), 200, 0., 2400.);
    signalTree->Draw( ("HH_m>>" + plotNameHH).c_str(), (signalSelection  + " && " + globalSelection).c_str(), "goff");
    TF1* mHHfit = new TF1("mHHfit", "gaus", 0, 2400);
    mHHplot->Fit(mHHfit);
    mHH_mean  = mHHfit->GetParameter(1);
    mHH_sigma = mHHfit->GetParameter(2) * 2.;

    delete mHHplot;
    delete mHHfit ;
    
    if(yMass != 125)
    {

        std::string plotNameHH = "mH2plot" + nameForThread;
        TH1F *mH2plot = new TH1F(plotNameHH.c_str(),plotNameHH.c_str(), 200, 0., 2400.);
        signalTree->Draw( ("H2_m>>" + plotNameHH).c_str(), (signalSelection  + " && " + globalSelection).c_str(), "goff");
        TF1* mH2fit = new TF1("mH2fit", "gaus", 0, 2400);
        mH2plot->Fit(mH2fit);
        mH2_mean  = mH2fit->GetParameter(1);
        mH2_sigma = mH2fit->GetParameter(2) * 2.;
        delete mH2plot;
        delete mH2fit ;
    }
    else
    {
        mH2_mean  = 125.;
        mH2_sigma =  20.;
    }
    
    std::string massSelectionString = "H2_m > " + std::to_string(mH2_mean-mH2_sigma) + " && H2_m < " + std::to_string(mH2_mean+mH2_sigma) + " && HH_m > " + std::to_string(mHH_mean-mHH_sigma) + " && HH_m < " + std::to_string(mHH_mean+mHH_sigma);
    std::cout<<massSelectionString<<std::endl;

    return massSelectionString;
}

void OverlapTrees(int year, const std::string signal, const std::string& signalFileNameList, const std::string& backgroundFileNameList, const std::vector<std::tuple<std::string,int,float,float>>& variableList)
{
    std::vector<std::string> signalFileList     = splitByLine(signalFileNameList    );
    std::vector<std::string> backgroundFileList = splitByLine(backgroundFileNameList);
    auto signalTree     = getChainFromFileList(signalFileList    , "bbbbTree");
    auto backgroundTree = getChainFromFileList(backgroundFileList, "bbbbTree");

    std::string nameForThread = std::to_string(year) + signal;

    std::string massSelectionString = getMassCut( (signalFileNameList.find("125") == std::string::npos) ? -1 : 125, nameForThread, signalTree, gSignalSelection, gGlobalSelection);
    std::string fullGlobalSelection = gGlobalSelection + " && " + massSelectionString;
    
    for(const auto& variableTuple : variableList)
    {
        auto variable = std::get<0>(variableTuple);
        std::string signalPlotName     = variable + "_signal"     + std::to_string(year);
        std::string backgroundPlotName = variable + "_background" + std::to_string(year);
        TH1F *signalPlot     = new TH1F(signalPlotName    .c_str(), signalPlotName    .c_str(), std::get<1>(variableTuple), std::get<2>(variableTuple), std::get<3>(variableTuple));
        TH1F *backgroundPlot = new TH1F(backgroundPlotName.c_str(), backgroundPlotName.c_str(), std::get<1>(variableTuple), std::get<2>(variableTuple), std::get<3>(variableTuple));
        signalTree    ->Draw( (variable + ">>" + signalPlotName    ).c_str(), ( gSignalWeights + " * (" + gSignalSelection     + " && " + fullGlobalSelection + ")").c_str(), "goff");
        backgroundTree->Draw( (variable + ">>" + backgroundPlotName).c_str(), (gBackgroundSelection + " && " + fullGlobalSelection + " && " + gtriggerCut[year]).c_str(), "goff");

        auto cutTuple = getBestCutForSignal(signalPlot, backgroundPlot);
        std::cout << std::get<0>(cutTuple) << " < " << variable << " < " << std::get<1>(cutTuple) << std::endl;
        auto cutPlot2D = std::get<2>(cutTuple);
        TH1D* lowCutSignificance  = cutPlot2D->ProjectionY((signalPlotName+"_lowCutSignificance" ).c_str(), 1, 1);
        TH1D* highCutSignificance = cutPlot2D->ProjectionX((signalPlotName+"_highCutSignificance").c_str(), cutPlot2D->GetNbinsY(), cutPlot2D->GetNbinsY());

        TCanvas theCanvas(("CutOptimization_" + signalPlotName).c_str(),("CutOptimization_" + signalPlotName).c_str());
        lowCutSignificance->Scale(1./lowCutSignificance->GetBinContent(std::get<1>(variableTuple)));
        float thePlotMaximum = lowCutSignificance->GetMaximum();
        lowCutSignificance ->SetLineColor(kBlack );
        lowCutSignificance->GetXaxis()->SetTitle(variable.c_str());
        lowCutSignificance->GetYaxis()->SetTitle("significance");
        lowCutSignificance ->Draw("hist");
        highCutSignificance->Scale(1./highCutSignificance->GetBinContent(1));
        highCutSignificance->SetLineColor(kViolet);
        highCutSignificance->Draw("same hist");
        if(highCutSignificance->GetMaximum() > thePlotMaximum) thePlotMaximum = highCutSignificance->GetMaximum();

        backgroundPlot->Scale(thePlotMaximum/backgroundPlot->GetMaximum());
        signalPlot    ->Scale(thePlotMaximum/signalPlot    ->GetMaximum());
        backgroundPlot->SetLineColor(kBlue);
        backgroundPlot->Draw("same");
        signalPlot    ->SetLineColor(kRed);
        signalPlot    ->Draw("same");
        TLine *theLowCutLine = new TLine(std::get<0>(cutTuple), 0., std::get<0>(cutTuple), backgroundPlot->GetMaximum());
        theLowCutLine->SetLineColor(kOrange);
        theLowCutLine->SetLineWidth(3);
        // theLowCutLine ->Draw("same");
        TLine *theHighCutLine = new TLine(std::get<1>(cutTuple), 0., std::get<1>(cutTuple), backgroundPlot->GetMaximum());
        theHighCutLine->SetLineColor(kOrange);
        theHighCutLine->SetLineWidth(3);
        // theHighCutLine ->Draw("same");
        lowCutSignificance->SetMaximum(thePlotMaximum*1.1);
        boost::replace_all(variable , " " , "");
        std::string makeDirCommand = "mkdir -p CutOptimization" + std::to_string(year) + "/" + variable;
        system(makeDirCommand.c_str()); 
        theCanvas.SaveAs(("CutOptimization" + std::to_string(year) + "/" + variable + "/" + std::string(theCanvas.GetName()) + "_" + std::to_string(year) + "_" + signal + ".png").c_str());
        delete signalPlot;
        delete backgroundPlot;
        delete lowCutSignificance;
        delete highCutSignificance;
        delete cutPlot2D;
    }

    return;

}


void OverlapAllVariables(int year, std::string signal)
{
    gROOT->SetBatch(true);
    
    std::string signalFileName     = "plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_" + signal + "_Fast.txt";
    std::string backgroundFileName = "plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/" + (year == 2018 ? "JetHT" : "BTagCSV") + "_Data_FileList_NMSSM_XYH_bbbb.txt";

    OverlapTrees(year,signal,signalFileName, backgroundFileName, 
        {
            {"H1_bb_DeltaR"       , 100, 0., 5.},
            // {"H1_H2_sphericity"   , 100, 0., 1.},
            // {"FourBjet_sphericity", 100, 0., 1.},
            {"abs_costh_H1_ggfcm" , 100, 0., 1.},
            // {"abs_costh_H2_ggfcm" , 100, 0., 1.},
            {"HH_pt" , 200, 0., 1000.},
            {"H1_pt" , 200, 0., 1000.},
            {"H2_pt" , 200, 0., 1000.},
            {"H1_eta-H2_eta" , 200, 0., 10.},
            // {"FirstBtaggedJet_PtRegRes", 200, 0., 1.},
            // {"SecondBtaggedJet_PtRegRes", 200, 0., 1.},
            // {"ThirdBtaggedJet_PtRegRes", 200, 0., 1.},
            // {"FourthBtaggedJet_PtRegRes", 200, 0., 1.},
            // {"FirstBtaggedJet_pt + SecondBtaggedJet_pt", 200, 0., 2000.},
            // {"FirstBtaggedJet_pt + SecondBtaggedJet_pt + ThirdBtaggedJet_pt", 200, 0., 2000.},
            {"FirstBtaggedJet_pt + SecondBtaggedJet_pt + ThirdBtaggedJet_pt + FourthBtaggedJet_pt", 200, 0., 2000.},
            // {"minDeltaRbJets"       , 100, 0., 5.}, 
            {"maxDeltaRbJets"       , 100, 0., 5.}, 
            // {"minDeltaEtabJets"     , 100, 0., 5.}, 
            {"maxDeltaEtabJets"     , 100, 0., 5.}, 
            // {"minDeltaPhibJets"     , 100, 0., 5.}, 
            {"maxDeltaPhibJets"     , 100, 0., 5.},
            {"FirstBtaggedJet_deepCSV + SecondBtaggedJet_deepCSV + ThirdBtaggedJet_deepCSV", 200, 0., 3.},
            {"FirstBtaggedJet_PtRegRes + SecondBtaggedJet_PtRegRes + ThirdBtaggedJet_PtRegRes + FourthBtaggedJet_PtRegRes", 200, 0., 2.}
        });
    gROOT->SetBatch(false);
}


void OverlapAllSignalVariables(int year)
{
    ROOT::EnableThreadSafety();
    gROOT->SetBatch(true);

    std::vector<std::string> signalList = 
    {
        "MX_300_MY_60",
        "MX_300_MY_70",
        "MX_300_MY_80",
        "MX_300_MY_90",
        "MX_300_MY_100",
        "MX_300_MY_125",
        "MX_400_MY_60",
        "MX_400_MY_70",
        "MX_400_MY_80",
        "MX_400_MY_90",
        "MX_400_MY_100",
        "MX_400_MY_125",
        "MX_400_MY_150",
        "MX_400_MY_200",
        "MX_400_MY_250",
        "MX_500_MY_60",
        "MX_500_MY_70",
        "MX_500_MY_80",
        "MX_500_MY_90",
        "MX_500_MY_100",
        "MX_500_MY_125",
        "MX_500_MY_150",
        "MX_500_MY_200",
        "MX_500_MY_250",
        "MX_500_MY_300",
        "MX_600_MY_60",
        "MX_600_MY_70",
        "MX_600_MY_80",
        "MX_600_MY_90",
        "MX_600_MY_100",
        "MX_600_MY_125",
        "MX_600_MY_150",
        "MX_600_MY_200",
        "MX_600_MY_250",
        "MX_600_MY_300",
        "MX_600_MY_400",
        "MX_700_MY_60",
        "MX_700_MY_70",
        "MX_700_MY_80",
        "MX_700_MY_90",
        "MX_700_MY_100",
        "MX_700_MY_125",
        "MX_700_MY_150",
        "MX_700_MY_200",
        "MX_700_MY_250",
        "MX_700_MY_300",
        "MX_700_MY_400",
        "MX_700_MY_500",
        "MX_800_MY_60",
        "MX_800_MY_70",
        "MX_800_MY_80",
        "MX_800_MY_90",
        "MX_800_MY_100",
        "MX_800_MY_125",
        "MX_800_MY_150",
        "MX_800_MY_200",
        "MX_800_MY_250",
        "MX_800_MY_300",
        "MX_800_MY_400",
        "MX_800_MY_500",
        "MX_800_MY_600",
        "MX_900_MY_60",
        "MX_900_MY_70",
        "MX_900_MY_80",
        "MX_900_MY_90",
        "MX_900_MY_100",
        "MX_900_MY_125",
        "MX_900_MY_150",
        "MX_900_MY_200",
        "MX_900_MY_250",
        "MX_900_MY_300",
        "MX_900_MY_400",
        "MX_900_MY_500",
        "MX_900_MY_600",
        "MX_900_MY_700",
        "MX_1000_MY_60",
        "MX_1000_MY_70",
        "MX_1000_MY_80",
        "MX_1000_MY_90",
        "MX_1000_MY_100",
        "MX_1000_MY_125",
        "MX_1000_MY_150",
        "MX_1000_MY_200",
        "MX_1000_MY_250",
        "MX_1000_MY_300",
        "MX_1000_MY_400",
        "MX_1000_MY_500",
        "MX_1000_MY_600",
        "MX_1000_MY_700",
        "MX_1000_MY_800",
        "MX_1100_MY_90",
        "MX_1100_MY_100",
        "MX_1100_MY_90",
        "MX_1100_MY_100",
        "MX_1100_MY_125",
        "MX_1100_MY_150",
        "MX_1100_MY_200",
        "MX_1100_MY_250",
        "MX_1100_MY_300",
        "MX_1100_MY_400",
        "MX_1100_MY_500",
        "MX_1100_MY_600",
        "MX_1100_MY_700",
        "MX_1100_MY_800",
        "MX_1100_MY_900",
        "MX_1200_MY_90",
        "MX_1200_MY_100",
        "MX_1200_MY_125",
        "MX_1200_MY_150",
        "MX_1200_MY_200",
        "MX_1200_MY_250",
        "MX_1200_MY_300",
        "MX_1200_MY_400",
        "MX_1200_MY_500",
        "MX_1200_MY_600",
        "MX_1200_MY_700",
        "MX_1200_MY_800",
        "MX_1200_MY_900",
        "MX_1200_MY_1000",
        "MX_1400_MY_125",
        "MX_1400_MY_90",
        "MX_1400_MY_100",
        "MX_1400_MY_150",
        "MX_1400_MY_200",
        "MX_1400_MY_250",
        "MX_1400_MY_300",
        "MX_1400_MY_400",
        "MX_1400_MY_500",
        "MX_1400_MY_600",
        "MX_1400_MY_700",
        "MX_1400_MY_800",
        "MX_1400_MY_900",
        "MX_1400_MY_1000",
        "MX_1400_MY_1200",
        "MX_1600_MY_125",
        "MX_1600_MY_90",
        "MX_1600_MY_100",
        "MX_1600_MY_150",
        "MX_1600_MY_200",
        "MX_1600_MY_250",
        "MX_1600_MY_300",
        "MX_1600_MY_400",
        "MX_1600_MY_500",
        "MX_1600_MY_600",
        "MX_1600_MY_700",
        "MX_1600_MY_800",
        "MX_1600_MY_900",
        "MX_1600_MY_1000",
        "MX_1600_MY_1200",
        "MX_1600_MY_1400",
        "MX_1800_MY_125",
        "MX_1800_MY_90",
        "MX_1800_MY_100",
        "MX_1800_MY_150",
        "MX_1800_MY_200",
        "MX_1800_MY_250",
        "MX_1800_MY_300",
        "MX_1800_MY_400",
        "MX_1800_MY_500",
        "MX_1800_MY_600",
        "MX_1800_MY_700",
        "MX_1800_MY_800",
        "MX_1800_MY_900",
        "MX_1800_MY_1000",
        "MX_1800_MY_1200",
        "MX_1800_MY_1400",
        "MX_1800_MY_1600",
        "MX_2000_MY_90",
        "MX_2000_MY_100",
        "MX_2000_MY_125",
        "MX_2000_MY_150",
        "MX_2000_MY_200",
        "MX_2000_MY_250",
        "MX_2000_MY_300",
        "MX_2000_MY_400",
        "MX_2000_MY_500",
        "MX_2000_MY_600",
        "MX_2000_MY_700",
        "MX_2000_MY_800",
        "MX_2000_MY_900",
        "MX_2000_MY_1000",
        "MX_2000_MY_1200",
        "MX_2000_MY_1400",
        "MX_2000_MY_1600",
        "MX_2000_MY_1800"
    };

    for(const auto& signal : signalList)
    {
        // std::thread thread2016(OverlapAllVariables, 2016, signal);
        // std::thread thread2017(OverlapAllVariables, 2017, signal);
        // std::thread thread2018(OverlapAllVariables, 2018, signal);

        // thread2016.join();
        // thread2017.join();
        // thread2018.join();
        OverlapAllVariables(year, signal);
        // OverlapAllVariables(2017, signal);
        // OverlapAllVariables(2018, signal);
    }
    gROOT->SetBatch(false);
}


void PlotDistributionVsMasses(int year, const std::string& fileNameList, const std::string& variable)
{
    gROOT->SetBatch(true);
    std::vector<std::string> fileList = splitByLine(fileNameList    );
    auto theTree = getChainFromFileList(fileList, "bbbbTree");

    std::string variableNorm = variable + "_norm";
    TH2F *variablePlot     = new TH2F(variable    .c_str(), variable    .c_str(),200, 200., 2400., 200, 0., 2200.);
    TH2F *variableNormPlot = new TH2F(variableNorm.c_str(), variableNorm.c_str(),200, 200., 2400., 200, 0., 2200.);

    theTree->Draw(("H2_m:HH_m >>" + variable    ).c_str(), (variable + "*(" + gBackgroundSelection + " && " + gGlobalSelection + " && " + gtriggerCut[year] + ")").c_str(), "colz");
    theTree->Draw(("H2_m:HH_m >>" + variableNorm).c_str(), (gBackgroundSelection + " && " + gGlobalSelection + " && " + gtriggerCut[year]).c_str(), "colz");
    variablePlot->Divide(variableNormPlot);

    TCanvas theCanvas(("DistributionVsMasses_" + variable).c_str(),("DistributionVsMasses_" + variable).c_str());
    variablePlot->Draw("colz");
    theCanvas.SaveAs((std::string(theCanvas.GetName()) + "_" + std::to_string(year) + ".png").c_str());
    gROOT->SetBatch(false);

    return;
}


void PlotDistributionVsMX(int year, const std::string& fileNameList, const std::string& variable, float min, float max)
{
    gROOT->SetBatch(true);
    std::vector<std::string> fileList = splitByLine(fileNameList    );
    auto theTree = getChainFromFileList(fileList, "bbbbTree");

    std::string variableNorm = variable + "_norm";
    TH2F *variablePlot     = new TH2F(variable    .c_str(), variable    .c_str(),200, 200., 2400., 200, min, max);
    
    theTree->Draw(( variable + ":HH_m >>" + variable    ).c_str(), (gBackgroundSelection + " && " + gGlobalSelection + " && " + gtriggerCut[year]).c_str(), "colz");
    
    TCanvas theCanvas(("DistributionVsMX_" + variable).c_str(),("DistributionVsMX_" + variable).c_str());
    variablePlot->Draw("colz");
    theCanvas.SaveAs((std::string(theCanvas.GetName()) + "_" + std::to_string(year) + ".png").c_str());
    gROOT->SetBatch(false);

    return;
}



void GetBestCut(int year, const std::string& signalFileNameList, const std::string& backgroundFileNameList, std::string variable, int nBins, float variableMin, float variableMax)
{
    constexpr int numberOfXbins = 14;
    float xBinning[numberOfXbins+1] = {250, 350, 450, 550, 650, 750, 850, 950, 1050, 1150, 1300, 1500, 1700, 1900, 2100};
    constexpr int numberOfYbins = 21;
    float yBinning[numberOfYbins+1] = {55, 65, 75, 85, 95, 112.5, 137.5, 175, 225, 275, 350, 450, 550, 650, 750, 850, 950, 1100, 1300, 1500, 1700, 1900};

    gROOT->SetBatch(true);

    std::vector<std::string> signalFileList     = splitByLine(signalFileNameList    );
    std::vector<std::string> backgroundFileList = splitByLine(backgroundFileNameList);
    auto backgroundTree = getChainFromFileList(backgroundFileList, "bbbbTree");

    std::string lowCutPlotName = "BestLowCut_" + variable;
    TH2F *theLowCutPlot = new TH2F(lowCutPlotName.c_str(), lowCutPlotName.c_str(), numberOfXbins, xBinning, numberOfYbins, yBinning);
    std::string highCutPlotName = "BestHighCut_" + variable;
    TH2F *theHighCutPlot = new TH2F(highCutPlotName.c_str(), highCutPlotName.c_str(), numberOfXbins, xBinning, numberOfYbins, yBinning);
    std::string mXbeginString = "_MX_"      ;
    std::string mXendString   = "_NANOAOD_" ;
    std::string mYbeginString = "_MY_"      ;
    std::string mYendString   = "/output"   ;

    for(auto& signalFile : signalFileList)
    {
        int mXbegin = signalFile.find(mXbeginString) + mXbeginString.size();
        int mXend   = signalFile.find(mXendString);
        int mYbegin = signalFile.find(mYbeginString) + mYbeginString.size();
        int mYend   = signalFile.find(mYendString);
        int xMass = atoi(signalFile.substr(mXbegin, mXend-mXbegin).c_str());
        int yMass = atoi(signalFile.substr(mYbegin, mYend-mYbegin).c_str());

        std::vector<std::string> singleListVector {signalFile};
        auto signalTree     = getChainFromFileList(singleListVector , "bbbbTree");
        
        std::string nameForThread = std::to_string(year) + signalFile;
        std::string massSelectionString = getMassCut(yMass, nameForThread, signalTree, gSignalSelection, gGlobalSelection);
        std::string fullGlobalSelection = gGlobalSelection + " && " + massSelectionString;
    
        std::string signalPlotName     = variable + "_signal"    ;
        std::string backgroundPlotName = variable + "_background";
        TH1F *signalPlot     = new TH1F(signalPlotName    .c_str(), signalPlotName    .c_str(), nBins, variableMin, variableMax);
        TH1F *backgroundPlot = new TH1F(backgroundPlotName.c_str(), backgroundPlotName.c_str(), nBins, variableMin, variableMax);
        std::cout<<(gSignalSelection     + " && " + fullGlobalSelection)<<std::endl;
        signalTree    ->Draw( (variable + ">>" + signalPlotName    ).c_str(), ( gSignalWeights + "* (" + gSignalSelection     + " && " + fullGlobalSelection + ")").c_str(), "goff");
        backgroundTree->Draw( (variable + ">>" + backgroundPlotName).c_str(), (gBackgroundSelection + " && " + fullGlobalSelection + " && " + gtriggerCut[year]).c_str(), "goff");

        auto cutTuple = getBestCutForSignal(signalPlot, backgroundPlot);
        std::cout<<"MassX = " << xMass << " - MassX = " << yMass << " - Best cut = "<< std::get<0>(cutTuple) << " < " << variable << " < " << std::get<1>(cutTuple) <<std::endl;

        // TCanvas theCanvas(("CutOptimization_" + variable).c_str(),("CutOptimization_" + variable).c_str());
        // backgroundPlot->Scale(signalPlot->Integral("width")/backgroundPlot->Integral("width"));
        // backgroundPlot->SetLineColor(kBlue);
        // backgroundPlot->Draw();
        // signalPlot    ->SetLineColor(kRed);
        // signalPlot    ->Draw("same");
        // backgroundPlot->GetYaxis()->SetRangeUser(0., 1.05*std::max(signalPlot->GetMaximum(),backgroundPlot->GetMaximum()));
        // theCanvas.SaveAs((std::string(theCanvas.GetName()) + "_" + std::to_string(year) + ".png").c_str());
        
        delete signalPlot;
        delete backgroundPlot;

        theLowCutPlot ->Fill(xMass, yMass, std::get<0>(cutTuple));
        theHighCutPlot->Fill(xMass, yMass, std::get<1>(cutTuple));
    }

    gROOT->SetBatch(false);

    TCanvas theCanvas(("BestCut_" + variable).c_str(),("BestCut_" + variable).c_str(), 1400, 600);
    theCanvas.Divide(2,1);
    theCanvas.cd(1);
    theLowCutPlot ->Draw("colz");
    theCanvas.cd(2);
    theHighCutPlot->Draw("colz");
    theCanvas.SaveAs((std::string(theCanvas.GetName()) + "_" + std::to_string(year) + ".png").c_str());

    return;

}
