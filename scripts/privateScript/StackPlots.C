#include "Riostream.h"
#include "TFile.h"
#include "THStack.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TLegend.h"
#include <algorithm>

#define NO_PLOT "null"

void StackPlot(TVirtualPad *theCanvas, TFile* inputFile, std::string selectionAndRegion, std::string variableName, std::vector<std::string> datasetList, std::vector<std::string> datasetNameList, std::vector<EColor> colorList, std::string modelDataset, std::string modelName, std::string dataDataset, std::string dataName)
{

    assert(datasetList.size() == datasetNameList.size() == colorList.size());
    std::vector<TH1F*> plotList;

    auto theLegend = new TLegend(0.30,0.7,0.88,0.88);
    theLegend->SetNColumns(3);
    std::vector<std::tuple<TH1F*, const char*, const char*>> legendList;    

    for(size_t dIt=0; dIt<datasetList.size(); ++dIt)
    {
        const std::string &datasetName = datasetList[dIt];
        std::string plotName = datasetName + "/" + selectionAndRegion + "/" + datasetName + "_" + selectionAndRegion + "_" + variableName;
        TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
        if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
        datasetPlot->SetDirectory(0);
        datasetPlot->SetFillColor(colorList[dIt]);
        datasetPlot->SetFillStyle(3002);
        datasetPlot->SetNameTitle(datasetName.data(), datasetName.data());
        plotList.emplace_back(datasetPlot);
    }

    std::stable_sort(plotList.begin(), plotList.end(), [](const TH1F* aPlot, const TH1F* bPlot) -> bool
        {return aPlot->Integral() < bPlot->Integral();}
    );

    theCanvas->cd();

    THStack * theBackgroundStack = new THStack(variableName.data(), variableName.data());
    for(const auto & plot : plotList)
    {
        legendList.insert(legendList.begin(), {plot, plot->GetName(), "f"});
        theBackgroundStack->Add(plot);
    }
    theBackgroundStack->Draw("HIST e2");
    theBackgroundStack->GetXaxis()->SetTitle(plotList[0]->GetXaxis()->GetTitle());
    theBackgroundStack->GetYaxis()->SetTitle(plotList[0]->GetYaxis()->GetTitle());

    float theMaxY = theBackgroundStack->GetMaximum();


    if(modelDataset != NO_PLOT)
    {
        std::string plotName = modelDataset + "/" + selectionAndRegion + "/" + modelDataset + "_" + selectionAndRegion + "_" + variableName;
        TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
        if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
        datasetPlot->SetDirectory(0);
        datasetPlot->SetNameTitle(modelName.data(), modelName.data());
        datasetPlot->SetMarkerStyle(20);
        datasetPlot->SetMarkerSize(0.3);
        datasetPlot->SetMarkerColor(kRed);
        datasetPlot->SetLineColor(kRed);
        datasetPlot->Draw("same C E0");
        float theLocalMax = datasetPlot->GetMaximum();
        if(theLocalMax > theMaxY) theMaxY = theLocalMax;
        legendList.insert(legendList.begin(), {datasetPlot, datasetPlot->GetName(), "lpe"});
    }

    if(dataDataset != NO_PLOT)
    {
        std::string plotName = dataDataset + "/" + selectionAndRegion + "/" + dataDataset + "_" + selectionAndRegion + "_" + variableName;
        TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
        if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
        datasetPlot->SetDirectory(0);
        datasetPlot->SetNameTitle(dataName.data(), dataName.data());
        datasetPlot->SetMarkerStyle(20);
        datasetPlot->SetMarkerSize(0.3);
        datasetPlot->SetMarkerColor(kBlack);
        datasetPlot->SetLineColor(kBlack);
        datasetPlot->Draw("same C E0");
        float theLocalMax = datasetPlot->GetMaximum();
        if(theLocalMax > theMaxY) theMaxY = theLocalMax;
        legendList.insert(legendList.begin(), {datasetPlot, datasetPlot->GetName(), "lpe"});
    }

    // theBackgroundStack->GetYaxis()->SetRangeUser(0., theMaxY);
    theBackgroundStack->SetMaximum(theMaxY*100.);
    theBackgroundStack->SetMinimum(0.1);

    for(auto& legend : legendList) theLegend->AddEntry(std::get<0>(legend), std::get<1>(legend), std::get<2>(legend));
    theLegend->Draw("same");
    
    theCanvas->SetLogy();
}

void StackAllDatasets(TVirtualPad *theCanvas, TFile* inputFile, std::string selectionAndRegion, std::string variableName, std::string modelDataset, std::string modelName, std::string dataDataset, std::string dataName)
{

    std::vector<std::string> datasetList     = {"ggF_Hbb","VBF_Hbb","QCD"   ,"ttbar","WH"    ,"ZH"  ,"ZZ"  ,"ttH"   };
    std::vector<std::string> datasetNameList = {"ggF_Hbb","VBF_Hbb","QCD"   ,"ttbar","WH"    ,"ZH"  ,"ZZ"  ,"ttH"   };
    std::vector<EColor>      colorList       = {kRed     , kBlue   , kYellow, kGreen, kViolet, kCyan, kGray, kOrange};
      
    StackPlot(theCanvas, inputFile, selectionAndRegion, variableName, datasetList, datasetNameList, colorList, modelDataset, modelName, dataDataset, dataName);

}

void StackAllVariables(std::string inputFileName, std::string selectionAndRegion, std::string modelDataset=NO_PLOT, std::string modelName="data", std::string dataDataset=NO_PLOT, std::string dataName="data")
{
    gROOT->SetBatch();

    TFile inputFile(inputFileName.data());

    std::string canvasName = "BackgroundOverlap_" + selectionAndRegion;
    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 800);

    std::vector<std::string> variableVector = {"H1_b1_ptRegressed", "H1_b2_ptRegressed", "H2_b1_ptRegressed", "H2_b2_ptRegressed", "H1_m", "H2_m", "HH_m"};
    theCanvas->DivideSquare(variableVector.size(),0.005,0.005);
    for(size_t vIt = 0; vIt<variableVector.size(); ++vIt)
        StackAllDatasets(theCanvas->cd(vIt+1), &inputFile, selectionAndRegion, variableVector[vIt], modelDataset, modelName, dataDataset, dataName);
    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());

    inputFile.Close();
    delete theCanvas;

    gROOT->SetBatch(false);
}

void doAllStack()
{
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsLMR_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model");
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsHMR_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model");
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsLMR_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model", "data_BTagCSV_background", "data");
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsHMR_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model", "data_BTagCSV_background", "data");

    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsLMR_VetoLepton_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model");
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsHMR_VetoLepton_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model");
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsLMR_VetoLepton_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model", "data_BTagCSV_background", "data");
    StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "selectionbJetsHMR_VetoLepton_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model", "data_BTagCSV_background", "data");
}



