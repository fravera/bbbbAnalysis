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

const std::string gGlobalSelection     = "H1_m > 125-20 && H1_m < 125+20";
// std::string gGlobalSelection     = "H1_m > 125-20 && H1_m < 125+20 && H2_m > 125-20 && H2_m < 125+20 && HH_m > 300-40 && HH_m < 300+40";
const std::string gSignalSelection     = "NbJets >= 4";// && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0";
const std::string gBackgroundSelection = "NbJets == 3";

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

std::pair<float,float> getBestCutForSignal(const TH1F* signalPlot, const TH1F* backgroundPlot)
{
    TH2F *test = new TH2F("test","test", signalPlot->GetNbinsX(), 0, signalPlot->GetNbinsX(), signalPlot->GetNbinsX(), 0, signalPlot->GetNbinsX());
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
            test->Fill(minBin, maxBin, currentDicrimination);
            if(currentDicrimination > maxDicrimination)
            {
                maxDicrimination = currentDicrimination;
                lowCutWithMaxDicrimination  = signalPlot->GetXaxis()->GetBinLowEdge(minBin);
                highCutWithMaxDicrimination = signalPlot->GetXaxis()->GetBinUpEdge (maxBin);
            }
        }
    }
    int theMin, theMax, nada;
    test->GetMaximumBin(theMin, theMax, nada);
    std::cout << theMin << " - " << theMax << std::endl;
    std::cout << maxDicrimination << std::endl;
    TCanvas testC;
    test->Draw("colz");
    testC.SaveAs("test.png");
    return std::make_pair(lowCutWithMaxDicrimination, highCutWithMaxDicrimination);
}

std::string getMassCut(float yMass, std::unique_ptr<TChain>& signalTree, const std::string& signalSelection, const std::string globalSelection)
{
    std::cout<<(signalSelection  + " && " + globalSelection)<<std::endl;

    float mHH_mean, mHH_sigma, mH2_mean, mH2_sigma;
    TH1F *mHHplot = new TH1F("mHHplot","mHHplot", 200, 0., 2400.);
    signalTree->Draw( "HH_m>>mHHplot", (signalSelection  + " && " + globalSelection).c_str(), "goff");
    TF1* mHHfit = new TF1("mHHfit", "gaus", 0, 2400);
    mHHplot->Fit(mHHfit);
    mHH_mean  = mHHfit->GetParameter(1);
    mHH_sigma = mHHfit->GetParameter(2) * 2.;

    delete mHHplot;
    delete mHHfit ;
    
    if(yMass != 125)
    {

        TH1F *mH2plot = new TH1F("mH2plot","mH2plot", 200, 0., 2400.);
        signalTree->Draw( "H2_m>>mH2plot", (signalSelection  + " && " + globalSelection).c_str(), "goff");
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

void OverlapTrees(int year, const std::string& signalFileNameList, const std::string& backgroundFileNameList, const std::vector<std::tuple<std::string,int,float,float>>& variableList)
{
    std::vector<std::string> signalFileList     = splitByLine(signalFileNameList    );
    std::vector<std::string> backgroundFileList = splitByLine(backgroundFileNameList);
    auto signalTree     = getChainFromFileList(signalFileList    , "bbbbTree");
    auto backgroundTree = getChainFromFileList(backgroundFileList, "bbbbTree");

    std::string massSelectionString = getMassCut( (signalFileNameList.find("125") == std::string::npos) ? -1 : 125, signalTree, gSignalSelection, gGlobalSelection);
    std::string fullGlobalSelection = gGlobalSelection + " && " + massSelectionString;
    
    for(const auto& variableTuple : variableList)
    {
        auto variable = std::get<0>(variableTuple);
        std::string signalPlotName     = variable + "_signal"    ;
        std::string backgroundPlotName = variable + "_background";
        TH1F *signalPlot     = new TH1F(signalPlotName    .c_str(), signalPlotName    .c_str(), std::get<1>(variableTuple), std::get<2>(variableTuple), std::get<3>(variableTuple));
        TH1F *backgroundPlot = new TH1F(backgroundPlotName.c_str(), backgroundPlotName.c_str(), std::get<1>(variableTuple), std::get<2>(variableTuple), std::get<3>(variableTuple));
        signalTree    ->Draw( (variable + ">>" + signalPlotName    ).c_str(), (gSignalSelection     + " && " + fullGlobalSelection).c_str(), "goff");
        backgroundTree->Draw( (variable + ">>" + backgroundPlotName).c_str(), (gBackgroundSelection + " && " + fullGlobalSelection).c_str(), "goff");

        auto cutPair = getBestCutForSignal(signalPlot, backgroundPlot);
        std::cout << cutPair.first << " < " << variable << " < " << cutPair.second << std::endl;

        TCanvas theCanvas(("CutOptimization_" + variable).c_str(),("CutOptimization_" + variable).c_str());
        backgroundPlot->Scale(signalPlot->Integral("width")/backgroundPlot->Integral("width"));
        backgroundPlot->SetLineColor(kBlue);
        backgroundPlot->Draw();
        signalPlot    ->SetLineColor(kRed);
        signalPlot    ->Draw("same");
        TLine *theLowCutLine = new TLine(cutPair.first, 0., cutPair.first, backgroundPlot->GetMaximum());
        theLowCutLine->SetLineColor(kOrange);
        theLowCutLine->SetLineWidth(3);
        theLowCutLine ->Draw("same");
        TLine *theHighCutLine = new TLine(cutPair.second, 0., cutPair.second, backgroundPlot->GetMaximum());
        theHighCutLine->SetLineColor(kOrange);
        theHighCutLine->SetLineWidth(3);
        theHighCutLine ->Draw("same");
        backgroundPlot->GetYaxis()->SetRangeUser(0., 1.05*std::max(signalPlot->GetMaximum(),backgroundPlot->GetMaximum()));
        theCanvas.SaveAs((std::string(theCanvas.GetName()) + "_" + std::to_string(year) + ".png").c_str());
        delete signalPlot;
        delete backgroundPlot;
    }

    return;

}

void OverlapAllVariables(const std::string& signalFileName, const std::string& backgroundFileName)
{
    gROOT->SetBatch(true);
    int year = 0;
    if(signalFileName.find("2016") != std::string::npos) year = 2016;
    if(signalFileName.find("2017") != std::string::npos) year = 2017;
    if(signalFileName.find("2018") != std::string::npos) year = 2018;
    OverlapTrees(year,signalFileName, backgroundFileName, {
        // {"H1_bb_DeltaR"       , 100, 0., 5.}, 
        // {"H1_H2_sphericity"   , 100, 0., 1.},
        // {"FourBjet_sphericity", 100, 0., 1.},
        // {"abs_costh_H1_ggfcm" , 100, 0., 1.},
        // {"abs_costh_H2_ggfcm" , 100, 0., 1.},
        // {"HH_pt" , 200, 0., 1000.},
        // {"H1_pt" , 200, 0., 1000.},
        // {"H2_pt" , 200, 0., 1000.}
        {"H1_eta-H2_eta" , 200, 0., 10.}
        });
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

    theTree->Draw(("H2_m:HH_m >>" + variable    ).c_str(), (variable + "*(" + gBackgroundSelection + " && " + gGlobalSelection + ")").c_str(), "colz");
    theTree->Draw(("H2_m:HH_m >>" + variableNorm).c_str(), (gBackgroundSelection + " && " + gGlobalSelection).c_str(), "colz");
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
    
    theTree->Draw(( variable + ":HH_m >>" + variable    ).c_str(), (gBackgroundSelection + " && " + gGlobalSelection).c_str(), "colz");
    
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
        
        std::string massSelectionString = getMassCut(yMass, signalTree, gSignalSelection, gGlobalSelection);
        std::string fullGlobalSelection = gGlobalSelection + " && " + massSelectionString;
    
        std::string signalPlotName     = variable + "_signal"    ;
        std::string backgroundPlotName = variable + "_background";
        TH1F *signalPlot     = new TH1F(signalPlotName    .c_str(), signalPlotName    .c_str(), nBins, variableMin, variableMax);
        TH1F *backgroundPlot = new TH1F(backgroundPlotName.c_str(), backgroundPlotName.c_str(), nBins, variableMin, variableMax);
        std::cout<<(gSignalSelection     + " && " + fullGlobalSelection)<<std::endl;
        signalTree    ->Draw( (variable + ">>" + signalPlotName    ).c_str(), (gSignalSelection     + " && " + fullGlobalSelection).c_str(), "goff");
        backgroundTree->Draw( (variable + ">>" + backgroundPlotName).c_str(), (gBackgroundSelection + " && " + fullGlobalSelection).c_str(), "goff");

        auto cutPair = getBestCutForSignal(signalPlot, backgroundPlot);
        std::cout<<"MassX = " << xMass << " - MassX = " << yMass << " - Best cut = "<< cutPair.first << " < " << variable << " < " << cutPair.second <<std::endl;

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

        theLowCutPlot ->Fill(xMass, yMass, cutPair.first );
        theHighCutPlot->Fill(xMass, yMass, cutPair.second);
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
