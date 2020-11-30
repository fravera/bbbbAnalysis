#include "Riostream.h"
#include "TH1F.h"
#include "TFile.h"
#include "TROOT.h"
#include "TCanvas.h"
#include <vector>
#include <map>
#include "TChain.h"

std::string selection = "NbJets >= 4 && H1_m > 125-20 && H1_m < 125+20";

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

void getVariableInSignalRegion(TVirtualPad *theCanvas, std::string fileName, std::string variable, int year, float xMin, float xMax)
{
    gROOT->SetBatch(true);
    std::vector<std::string> fileList     = splitByLine(fileName    );
    auto theTree     = getChainFromFileList(fileList    , "bbbbTree");

    auto sampleName = getSampleNameFromFile(fileName);
    auto pairMxMy   = getMxMyFromSample(sampleName);

    theCanvas->cd();

    TH1F *variablePlot = new TH1F((sampleName + "_" + variable).c_str(), ("m_{X} = " + pairMxMy.first + " Gev - m_{Y} = " + pairMxMy.second + "; m_{X} [GeV]; entries [a.u.]" ).c_str()
        , 50, xMin, xMax);
    std::string plotMxKinFirCmd = variable + std::string(">>") + sampleName + "_" + variable;
    theTree->Draw(plotMxKinFirCmd.c_str(), selection.data(), "");
    variablePlot->SetLineColor(kBlue);
    variablePlot->SetMaximum(variablePlot->GetMaximum()*1.2);
    
}

void plotVariableInSignalRegion(std::string variable, int year, int mX, int mY, float xMin, float xMax)
{
    gROOT->SetBatch(true);
    auto getFileName = [year](std::pair<int,int> massPoint) -> std::string
    {
        return "plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_MX_" 
            + std::to_string(massPoint.first) + "_NANOAOD_v7_Full_MY_" + std::to_string(massPoint.second) + ".txt";
    };

    std::string canvasName = "Variable_" + variable + "_MX_" + std::to_string(mX) + "_MY_" + std::to_string(mY) + "_" + std::to_string(year);
    TCanvas *theCanvasKinFitImpact = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1400, 1000);
    getVariableInSignalRegion(theCanvasKinFitImpact,getFileName({mX,mY}), variable, year, xMin, xMax);
    theCanvasKinFitImpact->SaveAs((std::string(theCanvasKinFitImpact->GetName()) + ".png").c_str());
    gROOT->SetBatch(false);


}

