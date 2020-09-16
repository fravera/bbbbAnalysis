#include "Riostream.h"
#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TROOT.h"

void Plot2DLimitMap(std::string inputFileName, std::string year, std::string option = "syst")
{

    gROOT->SetBatch(true);

    std::string histogramName = "Limits_" + year + "/Option_" + option + "/LimitMapCentral_" + year + "_" + option;
    TFile inputFile(inputFileName.c_str());
    TH2D* limitMap = (TH2D*)inputFile.Get(histogramName.c_str());
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
    
    return;
}