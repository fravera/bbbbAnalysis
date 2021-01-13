#include "Riostream.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TColor.h"
#include <vector>
#include <map>
#include "TText.h"
#include "TChain.h"


std::string selection = "NbJets >= 4 && H1_m > 125-20 && H1_m < 125+20";
// std::string selection = "";

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
    {
        std::cout<<from<<" not found\n";
        return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
}



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

std::string getSampleNameFromFile(std::string fileName)
{
    size_t end_pos = fileName.rfind('.');
    size_t start_pos = fileName.rfind('/')+1;
    return fileName.substr(start_pos, end_pos-start_pos);

}

void TriggerEfficiencyTable(std::string fileName, int year)
{

    std::map<int, std::vector<std::string>> triggerFiltersMap;
    // std::vector<std::string> vectorFilters2016 = {"HLT_template_DoubleJet90_Double30_effL1","HLT_template_DoubleJet90_Double30_effQuad30CaloJet","HLT_template_DoubleJet90_Double30_effDouble90CaloJet","HLT_template_DoubleJet90_Double30_threeBtagEfficiency", "HLT_template_DoubleJet90_Double30_effQuad30PFJet","HLT_template_DoubleJet90_Double30_effDouble90PFJet","HLT_template_QuadJet45_effL1","HLT_template_QuadJet45_effQuad45CaloJet","HLT_template_QuadJet45_threeBtagEfficiency","HLT_template_QuadJet45_effQuad45PFJet","HLT_template_And_effL1","HLT_template_And_effQuad45CaloJet","HLT_template_And_effQuad45PFJet","triggertemplateEfficiency"};
    // std::vector<std::string> vectorFilters2017and2018 = {"HLT_template_effL1", "HLT_template_effQuad30CaloJet", "HLT_template_effCaloHT", "HLT_template_twoBtagEfficiency" ,"HLT_template_effQuad30PFJet", "HLT_template_effSingle75PFJet", "HLT_template_effDouble60PFJet", "HLT_template_effTriple54PFJet", "HLT_template_effQuad40PFJet", "HLT_template_effPFHT", "HLT_template_threeBtagEfficiency", "triggertemplateEfficiency"};
    std::vector<std::string> vectorFilters2016        {};
    std::vector<std::string> vectorFilters2017and2018 {};
    std::vector<std::string> sampleList = {"MC", "Data"};
    std::vector<std::string> currentEfficiencyList {"", ""};
    std::vector<EColor> colorList = {kBlue, kRed};

    triggerFiltersMap.emplace(2016, vectorFilters2016       );
    triggerFiltersMap.emplace(2017, vectorFilters2017and2018);
    triggerFiltersMap.emplace(2018, vectorFilters2017and2018);

    gROOT->SetBatch(true);
    std::vector<std::string> fileList     = splitByLine(fileName    );
    auto theTree     = getChainFromFileList(fileList    , "bbbbTree");

    TCanvas theCanvas("c1","c2", 1100, 900);
    theCanvas.DivideSquare(triggerFiltersMap[year].size() + 1);
    int filterCounter = 1;
    for(const auto & filter : triggerFiltersMap[year])
    {
        theCanvas.cd(filterCounter++);
        int sampleCounter = 0;
        float maximum = -1.;
        TH1F *firstPlotPointer = nullptr;
        float efficiency[2];
        for(const auto& sample : sampleList)
        {
            std::string& theEfficiency = currentEfficiencyList[sampleCounter];
            std::string theCurrentFilter = filter;
            if(theCurrentFilter.find("L1") != std::string::npos || theCurrentFilter == "triggertemplateEfficiency") theEfficiency = "1.";
            if(theCurrentFilter == "triggertemplateEfficiency" && sample == "MC") replace(theCurrentFilter, "template", "Mc"  );
            else                                                                  replace(theCurrentFilter, "template", sample);
            TH1F *effPlot = new TH1F(theCurrentFilter.data(), theCurrentFilter.data(), 140, -0.2, 1.2);
            if(sampleCounter==0) firstPlotPointer = effPlot;
            effPlot->SetLineColor(colorList[sampleCounter]);
            effPlot->SetStats(false);
            std::string plotCmd = theCurrentFilter + ">>" + theCurrentFilter.data();
            theTree->Draw(plotCmd.data(), ( theEfficiency + "*(" + selection + ")").data(), sampleCounter == 0 ? "" : "same");
            float currentMaximum = effPlot->GetMaximum();
            if(currentMaximum > maximum) maximum = currentMaximum;
            // TH1F *thePlot = (TH1F*) gDirectory->Get()
            TText *efficiencyLabel = new TText();
            efficiencyLabel-> SetNDC();
            efficiencyLabel -> SetTextFont(1);
            efficiencyLabel -> SetTextColor(colorList[sampleCounter]);
            efficiencyLabel -> SetTextSize(0.06);
            efficiency[sampleCounter] = effPlot->GetMean();
            std::string efficiencyString(16, '\0');
            auto efficiencyWritten = std::snprintf(&efficiencyString[0], efficiencyString.size(), "%.1f", effPlot->GetMean()*100.);
            efficiencyString.resize(efficiencyWritten);
            efficiencyLabel -> DrawText(0.2, 0.8-0.075*float(sampleCounter), (sample + " eff = " + efficiencyString + "%").data());
            std::string ratioString(16, '\0');
            auto ratioWritten = std::snprintf(&ratioString[0], ratioString.size(), "%.3f", efficiency[1]/efficiency[0]);
            ratioString.resize(ratioWritten);
            if(sampleCounter == 1 && filter != "triggertemplateEfficiency")
            {
                TText *ratioLabel = new TText();
                ratioLabel-> SetNDC();
                ratioLabel -> SetTextFont(1);
                ratioLabel -> SetTextColor(kBlack);
                ratioLabel -> SetTextSize(0.06);
                ratioLabel -> DrawText(0.2, 0.65, ("Ratio = " + ratioString).data());
            }
            // effPlot = (TH1F*)gDirectory->Get("effPlot");
            // std::cout << filter << "\t" << effPlot->GetMean() << std::endl;
            // delete effPlot;
            ++sampleCounter;
            theEfficiency += ("*"+theCurrentFilter);
        }
        firstPlotPointer->SetMaximum(maximum*1.10);
        
    }

    theCanvas.cd(triggerFiltersMap[year].size() + 1);

    float maximum = -1.;
    std::string triggerSFBranch = "triggerScaleFactor";
    TH1F *triggerSF = new TH1F(triggerSFBranch.data(), triggerSFBranch.data(), 100, -1., 2.);
    triggerSF->SetLineColor(kBlue);
    triggerSF->SetLineWidth(3);
    triggerSF->SetStats(false);
    triggerSF->GetXaxis()->SetTitle("trigger SF");
    // triggerSF->GetYaxis()->SetTitle("entries [a.u.]");
    std::string triggerSFCommand = triggerSFBranch + ">>" + triggerSFBranch.data();
    theTree->Draw(triggerSFCommand.data(), ("triggerMcEfficiency*(" + selection + ")").data(), "hist");
    float currentMaximum = triggerSF->GetMaximum();
    if(currentMaximum > maximum) maximum = currentMaximum;

    std::string triggerSFUpBranch = "triggerScaleFactorUp";
    TH1F *triggerSFUp = new TH1F(triggerSFUpBranch.data(), triggerSFUpBranch.data(), 100, -1., 2.);
    triggerSFUp->SetLineColor(kRed);
    triggerSFUp->SetLineWidth(3);
    triggerSFUp->SetStats(false);
    std::string triggerSFUpCommand = triggerSFUpBranch + ">>" + triggerSFUpBranch.data();
    theTree->Draw(triggerSFUpCommand.data(), ("triggerMcEfficiency*(" + selection + ")").data(), "hist same");
    currentMaximum = triggerSFUp->GetMaximum();
    if(currentMaximum > maximum) maximum = currentMaximum;

    std::string triggerSFDownBranch = "triggerScaleFactorDown";
    TH1F *triggerSFDown = new TH1F(triggerSFDownBranch.data(), triggerSFDownBranch.data(), 100, -1., 2.);
    triggerSFDown->SetLineColor(kGreen);
    triggerSFDown->SetLineWidth(3);
    triggerSFDown->SetStats(false);
    std::string triggerSFDownCommand = triggerSFDownBranch + ">>" + triggerSFDownBranch.data();
    theTree->Draw(triggerSFDownCommand.data(), ("triggerMcEfficiency*(" + selection + ")").data(), "hist same");
    currentMaximum = triggerSFDown->GetMaximum();
    if(currentMaximum > maximum) maximum = currentMaximum;

    std::cout<<triggerSF->Integral()<< "  " <<triggerSFDown->Integral()<< "  " <<triggerSFUp->Integral()<<std::endl;

    triggerSF->SetMaximum(maximum*1.10);

    TText *scaleFactorLabel = new TText();
    scaleFactorLabel-> SetNDC();
    scaleFactorLabel -> SetTextFont(1);
    scaleFactorLabel -> SetTextColor(kBlue);
    scaleFactorLabel -> SetTextSize(0.06);
    std::string scaleFactorString(16, '\0');
    auto written = std::snprintf(&scaleFactorString[0], scaleFactorString.size(), "%.3f", triggerSF->GetMean());
    scaleFactorString.resize(written);
    scaleFactorLabel -> DrawText(0.2, 0.725, ("SF = " + scaleFactorString).data());

    TText *scaleFactorUpLabel = new TText();
    scaleFactorUpLabel-> SetNDC();
    scaleFactorUpLabel -> SetTextFont(1);
    scaleFactorUpLabel -> SetTextColor(kRed);
    scaleFactorUpLabel -> SetTextSize(0.06);
    std::string scaleFactorUpString(16, '\0');
    auto writtenUp = std::snprintf(&scaleFactorUpString[0], scaleFactorUpString.size(), "%.3f", triggerSFUp->GetMean());
    scaleFactorUpString.resize(writtenUp);
    scaleFactorUpLabel -> DrawText(0.2, 0.8, ("SF Up= " + scaleFactorUpString).data());

    TText *scaleFactorDownLabel = new TText();
    scaleFactorDownLabel-> SetNDC();
    scaleFactorDownLabel -> SetTextFont(1);
    scaleFactorDownLabel -> SetTextColor(kGreen);
    scaleFactorDownLabel -> SetTextSize(0.06);
    std::string scaleFactorDownString(16, '\0');
    auto writtenDown = std::snprintf(&scaleFactorDownString[0], scaleFactorDownString.size(), "%.3f", triggerSFDown->GetMean());
    scaleFactorDownString.resize(writtenDown);
    scaleFactorDownLabel -> DrawText(0.2, 0.65, ("SF Down= " + scaleFactorDownString).data());


    theCanvas.SaveAs(("TriggerFilterEfficiencies_" + getSampleNameFromFile(fileName) + "_" + std::to_string(year) + ".png").data());

    gROOT->SetBatch(false);

}

void ProduceAllTriggerEfficiencyTablePerYear(int year)
{
    TriggerEfficiencyTable("plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_125.txt" , year);
    TriggerEfficiencyTable("plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_200.txt" , year);
    TriggerEfficiencyTable("plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_300.txt" , year);
    TriggerEfficiencyTable("plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_600.txt", year);
}

void ProduceAllTriggerEfficiencyTable()
{
    ProduceAllTriggerEfficiencyTablePerYear(2016);
    ProduceAllTriggerEfficiencyTablePerYear(2017);
    ProduceAllTriggerEfficiencyTablePerYear(2018);
}
