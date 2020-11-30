#include "Riostream.h"
#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TROOT.h"


// g++  -std=c++17 -I `root-config --incdir`  -o Plot2DLimitMap Plot2DLimitMap.cc `root-config --libs` -O3

void Plot2DLimitMap(std::string inputFileName, std::string year, std::string option = "syst")
{
    gROOT->SetBatch(true);

    std::string histogramName = "Limits_" + year + "/Option_" + option + "/LimitMapCentral_" + year + "_" + option;
    auto *inputFile = new TFile(inputFileName.c_str());
    if(inputFile == nullptr)
    {
        std::cout<< "File " << inputFileName << " does not exist, aborting..." << std::endl;
        abort();
    }
    TH2D* limitMap = (TH2D*)inputFile->Get(histogramName.c_str());
    if(limitMap == nullptr)
    {
        std::cout<< "Histogram " << histogramName << " does not exist, aborting..." << std::endl;
        abort();
    }
    limitMap->SetDirectory(0);

    std::string canvasName = "CentralLimitMap_" + year + "_" + option;
    TCanvas* theCanvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 800);
    limitMap->Draw("colz");

    std::string plotTitle = "Central Limit " + year;
    limitMap->SetTitle(plotTitle.c_str());
    limitMap->GetXaxis()->SetLabelFont(62);
    limitMap->GetXaxis()->SetLabelSize(0.045);
    limitMap->GetXaxis()->SetTitleFont(62);
    limitMap->GetXaxis()->SetTitleSize(0.045);
    limitMap->GetYaxis()->SetLabelFont(62);
    limitMap->GetYaxis()->SetLabelSize(0.045);
    limitMap->GetYaxis()->SetTitleFont(62);
    limitMap->GetYaxis()->SetTitleSize(0.045);
    limitMap->GetYaxis()->SetTitleOffset(1.25);
    limitMap->GetZaxis()->SetLabelFont(62);
    limitMap->GetZaxis()->SetLabelSize(0.035);
    limitMap->GetZaxis()->SetTitleFont(62);
    limitMap->GetZaxis()->SetTitleSize(0.045);
    limitMap->SetMinimum(1.);
    limitMap->SetMaximum(2000.);
    limitMap->SetStats(false);
    theCanvas->SetLogz();

    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").c_str());
    
    gROOT->SetBatch(false);
    
    delete inputFile;
    return;
}

int main(int argc, char** argv)
{

    if(argc!=2)
    {
        std::cout<<"Usage: ./Plot2DLimitMap <inputFile>"<<std::endl;
        exit(EXIT_FAILURE);
    }


    std::vector<std::string> optionList {"statOnly", "syst"};
    std::vector<std::string> yearList {"2016", "2017", "2018", "RunII"};

    for(const auto & year : yearList)
    {
        for(const auto & option : optionList)
        {
            Plot2DLimitMap(argv[1], year, option);
        }
    }

    return 0;

}