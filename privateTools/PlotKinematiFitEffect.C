#include "Riostream.h"
#include "TH1F.h"
#include "TFile.h"
#include "TROOT.h"
#include "TCanvas.h"
#include <vector>
#include <map>
#include "TChain.h"

std::string selection = "NbJets >= 4";



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

std::pair<std::string, std::string> getMxMyFromSample(std::string sampleName)
{
    std::string mxStringStart = "_MX_";
    std::string mxStringEnd   = "_NANOAOD_";
    std::string myStringStart = "_MY_";
    size_t mxStart = sampleName.find(mxStringStart) + mxStringStart.size();
    size_t mxEnd   = sampleName.find(mxStringEnd);
    size_t myStart = sampleName.find(myStringStart) + myStringStart.size();
    size_t myEnd   = sampleName.size();

    std::string mx = sampleName.substr(mxStart, mxEnd-mxStart);
    std::string my = sampleName.substr(myStart, myEnd-myStart);

    return std::make_pair(mx, my);
}

void PlotKinematiFitEffectSample(TVirtualPad *theCanvas, std::string fileName, int year)
{
    gROOT->SetBatch(true);
    std::vector<std::string> fileList     = splitByLine(fileName    );
    auto theTree     = getChainFromFileList(fileList    , "bbbbTree");

    auto sampleName = getSampleNameFromFile(fileName);
    auto pairMxMy   = getMxMyFromSample(sampleName);
    float mxValue = stof(pairMxMy.first );

    theCanvas->cd();

    TH1F *mxKinFitPlot = new TH1F((sampleName + "_kinFit").c_str(), ("m_{X} = " + pairMxMy.first + " Gev - m_{Y} = " + pairMxMy.second + "; m_{X} [GeV]; entries [a.u.]" ).c_str()
        , 50, mxValue*0.4, mxValue*1.4);
    std::string plotMxKinFirCmd = std::string("HH_kinFit_m>>") + (sampleName + "_kinFit").c_str();
    theTree->Draw(plotMxKinFirCmd.c_str(), selection.data(), "");
    mxKinFitPlot->SetLineColor(kBlue);
    mxKinFitPlot->SetStats(false);
    mxKinFitPlot->SetMaximum(mxKinFitPlot->GetMaximum()*1.2);
    
    TH1F *mxPlot = new TH1F(sampleName.c_str(), ("m_{X} = " + pairMxMy.first + " Gev - m_{Y} = " + pairMxMy.second + "; m_{X} [GeV]; entries [a.u.]" ).c_str()
        , 50, mxValue*0.4, mxValue*1.4);
    std::string plotMxCmd = std::string("HH_m>>") + sampleName.c_str();
    theTree->Draw(plotMxCmd.c_str(), selection.data(), "same");
    mxPlot->SetLineColor(kRed);
    mxPlot->SetStats(false);
}

void PlotKinematiFitEffectYear(int year)
{
    gROOT->SetBatch(true);
    std::vector<std::pair<int, int>> massHypothesisList
    {
        {300 ,125 },
        {400 ,150 },
        {500 ,200 },
        {700 ,300 },
        {900 ,700 },
        {1000,200 },
        {1000,700 },
        {1200,500 },
        {1600,1200}
    };

    auto getFileName = [year](std::pair<int,int> massPoint) -> std::string
    {
        return "plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_MX_" 
            + std::to_string(massPoint.first) + "_NANOAOD_v7_Full_MY_" + std::to_string(massPoint.second) + ".txt";
    };

    std::string canvasName = "KinFitImpact_" + std::to_string(year);
    TCanvas *theCanvasKinFitImpact = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1400, 1000);
    theCanvasKinFitImpact->DivideSquare(massHypothesisList.size(),0.005,0.005);
    int canvasCounter = 1;
    for(const auto& massPoint : massHypothesisList) PlotKinematiFitEffectSample(theCanvasKinFitImpact->cd(canvasCounter++),getFileName(massPoint), year);
    theCanvasKinFitImpact->SaveAs((std::string(theCanvasKinFitImpact->GetName()) + ".png").c_str());
    gROOT->SetBatch(false);

}

void PlotKinematiFitEffectAll()
{
    PlotKinematiFitEffectYear(2016);
    PlotKinematiFitEffectYear(2017);
    PlotKinematiFitEffectYear(2018);
}
