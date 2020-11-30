#include "Riostream.h"
#include "TFile.h"
#include "THStack.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TList.h"
#include <algorithm>

#define NO_PLOT "null"

int rebinValue = 4;

template<typename THA, typename THB>
std::tuple<TH1F*, TH1F*> dividePlots(const THA* referencePlot, const THB* inputPlot)
{
    std::string ratioPlotName = std::string(inputPlot->GetName()) + "_ratio";
    std::string errorPlotName = ratioPlotName + "Error";
    int numberOfBins = inputPlot->GetNbinsX();
    TH1F* ratioPlot  = (TH1F*)inputPlot->Clone(ratioPlotName.data());
    TH1F* ratioError = (TH1F*)inputPlot->Clone(errorPlotName.data());
    for(int nBin = 1; nBin<=numberOfBins; ++nBin)
    {
        float referenceValue = referencePlot->GetBinContent(nBin);
        float referenceError = referencePlot->GetBinError  (nBin);
        if(referenceValue == 0.) 
        {
            referenceValue = 1.;
            referenceError = 1.;
        }
        ratioPlot->SetBinContent(nBin,inputPlot->GetBinContent(nBin)/referenceValue);
        ratioPlot->SetBinError  (nBin,inputPlot->GetBinError  (nBin)/referenceValue);
        ratioError->SetBinContent(nBin,1.);
        ratioError->SetBinError(nBin,referenceError/referenceValue);
    }

    ratioError->SetFillStyle(3002);
    ratioError->SetFillColor(kBlack);
    ratioError->SetMarkerSize(0.);

    return {ratioPlot,ratioError};
}

std::tuple<TH1F*, TH1F*> splitErrorAndPlots(const THStack* inputStack)
{
    std::string valuePlotName = std::string(inputStack->GetName()) + "_value";
    std::string errorPlotName = valuePlotName + "Error";
    auto histogramList = inputStack->GetHists();
    const TH1F* theFirstPlot = (TH1F*)histogramList->At(0);

    int numberOfBins = theFirstPlot->GetNbinsX();
    TH1F* outputPlot  = (TH1F*)theFirstPlot->Clone(valuePlotName.data());
    outputPlot->SetDirectory(0);
    TH1F* outputError = (TH1F*)theFirstPlot->Clone(errorPlotName.data());
    outputError->SetDirectory(0);
    for(int nHist=0; nHist<histogramList->GetSize(); ++nHist)
    {
        TH1F* inputPlot = (TH1F*)histogramList->At(nHist);
        for(int nBin = 1; nBin<=numberOfBins; ++nBin)
        {
            if(nHist == 0)
            {
                outputPlot->SetBinError  (nBin,0.);
                outputPlot->SetBinContent (nBin,0.);
                outputError->SetBinError (nBin,0.);
                outputError->SetBinContent (nBin,0.);
            }
            outputPlot ->SetBinContent(nBin, outputPlot->GetBinContent(nBin) + inputPlot->GetBinContent(nBin) );
            outputError->SetBinContent(nBin, outputError->GetBinContent(nBin) + inputPlot->GetBinContent(nBin) );
            outputError->SetBinError  (nBin, sqrt(outputError->GetBinError(nBin)*outputError->GetBinError(nBin) + inputPlot->GetBinError(nBin)*inputPlot->GetBinError(nBin) ));
        }
    }

    outputError->SetFillStyle(3002);
    outputError->SetFillColor(kBlack);

    return {outputPlot,outputError};
}

template<typename TH>
void normalizeByBinSize1D(TH* inputPlot)
{
    for(int nBin = 1; nBin<=inputPlot->GetNbinsX(); ++nBin)
    {
        float binWidth = inputPlot->GetXaxis()->GetBinWidth(nBin);
        inputPlot->SetBinContent(nBin,inputPlot->GetBinContent(nBin)/binWidth);
        inputPlot->SetBinError(nBin,inputPlot->GetBinError(nBin)/binWidth);
    }
    return;

}


void StackPlot(TVirtualPad *theCanvas, TFile* inputFile, const std::string& selectionAndRegion, const std::string& variableName, const std::vector<std::string>& datasetList, const std::vector<std::string>& datasetNameList, std::vector<EColor> colorList, const std::string& modelDataset, const std::string& modelName, const std::vector<std::string>& signalDatasetList, const std::string& signalName, std::string dataDataset, std::string dataName, bool normalizeBinByBin, const std::string& xAxisName, const std::string& yAxisName)
{

    double totalIntegral = 0.;
    double totalIntegralError = 0.;
    double qcdIntegral   = 0.;
    double qcdIntegralError = 0.;
    double ttbarIntegral = 0.;
    double ttbarIntegralError = 0.;

    assert(datasetList.size() == datasetNameList.size() == colorList.size());
    std::vector<TH1F*> plotList;

    auto theLegend = new TLegend(0.45,0.6,0.88,0.88);
    theLegend->SetNColumns(3);
    theLegend->SetTextSize(0.04);
    auto theSignalLegend = new TLegend(0.45,0.5,0.88,0.6);
    theSignalLegend->SetTextSize(0.04);

    std::vector<std::tuple<TH1F*, const char*, const char*>> legendList;    

    for(size_t dIt=0; dIt<datasetList.size(); ++dIt)
    {
        const std::string &datasetName = datasetList[dIt];
        std::string plotName = datasetName + "/" + selectionAndRegion + "/" + datasetName + "_" + selectionAndRegion + "_" + variableName;
        TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
        if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
        datasetPlot->SetDirectory(0);
        datasetPlot->Rebin(rebinValue);
        datasetPlot->SetFillColor(colorList[dIt]);
        datasetPlot->SetFillStyle(3002);
        datasetPlot->SetNameTitle(datasetName.data(), datasetName.data());
        plotList.emplace_back(datasetPlot);
        if(datasetName == "ttbar") ttbarIntegral = datasetPlot->IntegralAndError(-1,999999999,ttbarIntegralError);
        if(datasetName == "QCD")   qcdIntegral   = datasetPlot->IntegralAndError(-1,999999999,qcdIntegralError);
        double theCurrentError = 0;
        totalIntegral += datasetPlot->IntegralAndError(-1,999999999,theCurrentError);
        totalIntegralError = sqrt(totalIntegralError*totalIntegralError + theCurrentError*theCurrentError);
    }
    std::cout << "QCD   BKG ratio = " << qcdIntegral/totalIntegral*100.   << " +- " << qcdIntegral/totalIntegral*100.   *sqrt(totalIntegralError*totalIntegralError/(totalIntegral*totalIntegral) + qcdIntegralError*qcdIntegralError/(qcdIntegral*qcdIntegral) )         << " \%"<<std::endl;
    std::cout << "ttbar BKG ratio = " << ttbarIntegral/totalIntegral*100. << " +- " << ttbarIntegral/totalIntegral*100. *sqrt(totalIntegralError*totalIntegralError/(totalIntegral*totalIntegral) + ttbarIntegralError*ttbarIntegralError/(ttbarIntegral*ttbarIntegral) ) << " \%"<<std::endl;

    std::stable_sort(plotList.begin(), plotList.end(), [](const TH1F* aPlot, const TH1F* bPlot) -> bool
        {return aPlot->Integral() < bPlot->Integral();}
    );

    theCanvas->cd();
        // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.25, 1, 1.0);
    pad1->SetLeftMargin(0.12);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad


    THStack * theBackgroundStack = new THStack(variableName.data(), variableName.data());
    for(auto & plot : plotList)
    {
        legendList.insert(legendList.begin(), {plot, plot->GetName(), "f"});
        if(normalizeBinByBin) normalizeByBinSize1D(plot);
        theBackgroundStack->Add(plot);
    }
    theBackgroundStack->Draw("HIST");
    theBackgroundStack->SetTitle("");
    theBackgroundStack->GetXaxis()->SetTitle(xAxisName.data());
    theBackgroundStack->GetYaxis()->SetTitle(yAxisName.data());
    theBackgroundStack->GetYaxis()->SetTitleSize(0.06);
    theBackgroundStack->GetYaxis()->SetTitleFont(62);
    theBackgroundStack->GetYaxis()->SetTitleOffset(0.7);
    theBackgroundStack->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    theBackgroundStack->GetYaxis()->SetLabelSize(0.055);

    std::tuple<TH1F*, TH1F*> backgroundAndError  = splitErrorAndPlots(theBackgroundStack);
    std::get<1>(backgroundAndError)->Draw("same e2");

    float theMaxY = theBackgroundStack->GetMaximum();

    std::string plotName = modelDataset + "/" + selectionAndRegion + "/" + modelDataset + "_" + selectionAndRegion + "_" + variableName;
    TH1F* bkgModelPlot = (TH1F*)inputFile->Get( plotName.data() );
    if(bkgModelPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
    bkgModelPlot->SetDirectory(0);
    bkgModelPlot->Rebin(rebinValue);
    if(normalizeBinByBin) normalizeByBinSize1D(bkgModelPlot);
    bkgModelPlot->SetNameTitle(modelName.data(), modelName.data());
    bkgModelPlot->SetMarkerStyle(20);
    bkgModelPlot->SetMarkerSize(0.3);
    bkgModelPlot->SetMarkerColor(kRed);
    bkgModelPlot->SetLineColor(kRed);
    bkgModelPlot->Draw("same C E0");
    float theLocalMax = bkgModelPlot->GetMaximum();
    if(theLocalMax > theMaxY) theMaxY = theLocalMax;
    legendList.insert(legendList.begin(), {bkgModelPlot, bkgModelPlot->GetName(), "lpe"});

    if(dataDataset != NO_PLOT)
    {
        std::string plotName = dataDataset + "/" + selectionAndRegion + "/" + dataDataset + "_" + selectionAndRegion + "_" + variableName;
        TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
        if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
        datasetPlot->SetDirectory(0);
        datasetPlot->Rebin(rebinValue);
        if(normalizeBinByBin) normalizeByBinSize1D(datasetPlot);
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

    theBackgroundStack->SetMaximum(theMaxY*100.);
    theBackgroundStack->SetMinimum(0.1);


    for(size_t dIt=0; dIt<signalDatasetList.size(); ++dIt)
    {
        const std::string &datasetName = signalDatasetList[dIt];
        std::string plotName = datasetName + "/" + selectionAndRegion + "/" + datasetName + "_" + selectionAndRegion + "_" + variableName;
        TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
        if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
        datasetPlot->SetDirectory(0);
        if(normalizeBinByBin) normalizeByBinSize1D(datasetPlot);
        datasetPlot->SetLineColor(kBlue);
        datasetPlot->SetLineWidth(2);
        datasetPlot->SetNameTitle(datasetName.data(), datasetName.data());
        datasetPlot->Draw("same hist");
        if(dIt==0) theSignalLegend->AddEntry(datasetPlot, signalName.data(), "l");
    }

    for(auto& legend : legendList) theLegend->AddEntry(std::get<0>(legend), std::get<1>(legend), std::get<2>(legend));
    theLegend->Draw("same");
    if(signalDatasetList.size()>0) theSignalLegend->Draw("same");
    
    pad1->SetLogy();

    theCanvas->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.25);
    pad2->SetTopMargin(0);
    pad2->SetGridx(); // vertical grid
    pad2->SetGridy(); // horizontal grid
    pad2->Draw();
    pad2->SetLeftMargin(0.12);
    pad2->SetBottomMargin(0.33);
    pad2->cd();       // pad2 becomes the current pad

    // Define the ratio plot

    auto theRatioPlots = dividePlots(std::get<1>(backgroundAndError), bkgModelPlot);

    auto *ratio = std::get<0>(theRatioPlots);
    auto *ratioError = std::get<1>(theRatioPlots);
    // ratio->SetLineColor(plotColorVector.at(hIt));
    // ratio->SetMarkerColor(plotColorVector.at(hIt));
    ratio->SetMinimum(0.5);  // Define Y ..
    ratio->SetMaximum(3.); // .. range
    ratio->SetStats(0);      // No statistics on lower plot
    // ratio->Divide(referenceHistogram);
    ratio->SetMarkerStyle(21);
    ratio->SetMarkerSize(0.3);

    // Ratio plot (ratio) settings
    ratio->SetTitle(""); // Remove the ratio title

    ratio->Draw("ep");       // Draw the ratio plot
    ratioError->Draw("same E2");
    // Y axis ratio plot settings
    ratio->GetYaxis()->SetTitle("ratio");
    ratio->GetYaxis()->SetNdivisions(505);
    ratio->GetYaxis()->SetTitleSize(0.18);
    ratio->GetYaxis()->SetTitleFont(62);
    ratio->GetYaxis()->SetTitleOffset(0.2);
    ratio->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    ratio->GetYaxis()->SetLabelSize(0.13);

    // X axis ratio plot settings
    ratio->GetXaxis()->SetTitle(xAxisName.data());
    ratio->GetXaxis()->SetTitleSize(0.17);
    ratio->GetXaxis()->SetTitleFont(62);
    ratio->GetXaxis()->SetTitleOffset(0.85);
    ratio->GetXaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    ratio->GetXaxis()->SetLabelSize(0.15);

    theCanvas->cd();

}


// void StackPlot(TVirtualPad *theCanvas, TFile* inputFile, const std::string& selectionAndRegion, const std::string& variableName, const std::vector<std::string>& datasetList, const std::vector<std::string>& datasetNameList, std::vector<EColor> colorList, const std::string& modelDataset, const std::string& modelName, const std::vector<std::string>& signalDatasetList, const std::string& signalName, std::string dataDataset, std::string dataName, bool normalizeBinByBin, const std::string& xAxisName, const std::string& yAxisName)
// {

//     assert(datasetList.size() == datasetNameList.size() == colorList.size());
//     std::vector<TH1F*> plotList;

//     auto theLegend = new TLegend(0.30,0.7,0.88,0.88);
//     theLegend->SetNColumns(3);
//     std::vector<std::tuple<TH1F*, const char*, const char*>> legendList;    

//     for(size_t dIt=0; dIt<datasetList.size(); ++dIt)
//     {
//         const std::string &datasetName = datasetList[dIt];
//         std::string plotName = datasetName + "/" + selectionAndRegion + "/" + datasetName + "_" + selectionAndRegion + "_" + variableName;
//         TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
//         if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
//         datasetPlot->SetDirectory(0);
//         datasetPlot->SetFillColor(colorList[dIt]);
//         datasetPlot->SetFillStyle(3002);
//         datasetPlot->SetNameTitle(datasetName.data(), datasetName.data());
//         plotList.emplace_back(datasetPlot);
//     }

//     std::stable_sort(plotList.begin(), plotList.end(), [](const TH1F* aPlot, const TH1F* bPlot) -> bool
//         {return aPlot->Integral() < bPlot->Integral();}
//     );

//     theCanvas->cd();

//     THStack * theBackgroundStack = new THStack(variableName.data(), variableName.data());
//     for(auto & plot : plotList)
//     {
//         legendList.insert(legendList.begin(), {plot, plot->GetName(), "f"});
//         if(normalizeBinByBin) normalizeByBinSize1D(plot);
//         theBackgroundStack->Add(plot);
//     }
//     theBackgroundStack->Draw("HIST");
//     theBackgroundStack->SetTitle("");
//     theBackgroundStack->GetXaxis()->SetTitle(xAxisName.data());
//     theBackgroundStack->GetYaxis()->SetTitle(yAxisName.data());
//     std::tuple<TH1F*, TH1F*> backgroundAndError  = splitErrorAndPlots(theBackgroundStack);
//     std::get<1>(backgroundAndError)->Draw("same e2");


//     float theMaxY = theBackgroundStack->GetMaximum();


//     if(modelDataset != NO_PLOT)
//     {
//         std::string plotName = modelDataset + "/" + selectionAndRegion + "/" + modelDataset + "_" + selectionAndRegion + "_" + variableName;
//         TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
//         if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
//         datasetPlot->SetDirectory(0);
//         if(normalizeBinByBin) normalizeByBinSize1D(datasetPlot);
//         datasetPlot->SetNameTitle(modelName.data(), modelName.data());
//         datasetPlot->SetMarkerStyle(20);
//         datasetPlot->SetMarkerSize(0.3);
//         datasetPlot->SetMarkerColor(kRed);
//         datasetPlot->SetLineColor(kRed);
//         datasetPlot->Draw("same C E0");
//         float theLocalMax = datasetPlot->GetMaximum();
//         if(theLocalMax > theMaxY) theMaxY = theLocalMax;
//         legendList.insert(legendList.begin(), {datasetPlot, datasetPlot->GetName(), "lpe"});
//     }

//     if(dataDataset != NO_PLOT)
//     {
//         std::string plotName = dataDataset + "/" + selectionAndRegion + "/" + dataDataset + "_" + selectionAndRegion + "_" + variableName;
//         TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
//         if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
//         datasetPlot->SetDirectory(0);
//         if(normalizeBinByBin) normalizeByBinSize1D(datasetPlot);
//         datasetPlot->SetNameTitle(dataName.data(), dataName.data());
//         datasetPlot->SetMarkerStyle(20);
//         datasetPlot->SetMarkerSize(0.3);
//         datasetPlot->SetMarkerColor(kBlack);
//         datasetPlot->SetLineColor(kBlack);
//         datasetPlot->Draw("same C E0");
//         float theLocalMax = datasetPlot->GetMaximum();
//         if(theLocalMax > theMaxY) theMaxY = theLocalMax;
//         legendList.insert(legendList.begin(), {datasetPlot, datasetPlot->GetName(), "lpe"});
//     }

//     theBackgroundStack->SetMaximum(theMaxY*100.);
//     theBackgroundStack->SetMinimum(0.1);



//     for(size_t dIt=0; dIt<signalDatasetList.size(); ++dIt)
//     {
//         const std::string &datasetName = signalDatasetList[dIt];
//         std::string plotName = datasetName + "/" + selectionAndRegion + "/" + datasetName + "_" + selectionAndRegion + "_" + variableName;
//         TH1F* datasetPlot = (TH1F*)inputFile->Get( plotName.data() );
//         if(datasetPlot == nullptr) std::cout<<"Plot "<< plotName << " does not exist" <<std::endl;
//         datasetPlot->SetDirectory(0);
//         if(normalizeBinByBin) normalizeByBinSize1D(datasetPlot);
//         datasetPlot->SetLineColor(kBlue);
//         datasetPlot->SetLineWidth(2);
//         datasetPlot->SetNameTitle(datasetName.data(), datasetName.data());
//         datasetPlot->Draw("same hist");
//         if(dIt==0) legendList.push_back({datasetPlot, signalName.data(), "l"});
//     }

//     theLegend->AddEntry(std::get<1>(backgroundAndError), "BKG unc.", "f");
//     for(auto& legend : legendList) theLegend->AddEntry(std::get<0>(legend), std::get<1>(legend), std::get<2>(legend));
//     theLegend->Draw("same");
    
//     theCanvas->SetLogy();
// }

void StackAllDatasets(TVirtualPad *theCanvas, TFile* inputFile, const std::string& selectionAndRegion, const std::string& variableName, const std::string& modelDataset, const std::string& modelName, const std::vector<std::string>& signalDatasetList, const std::string& signalName, const std::string& dataDataset, const std::string& dataName, bool normalizeBinByBin, const std::string& xAxisName, const std::string& yAxisName)
{

    std::vector<std::string> datasetList     = {"ggF_Hbb","VBF_Hbb","QCD"   ,"ttbar","WH"    ,"ZH"  ,"ZZ"  ,"ttH"   };
    std::vector<std::string> datasetNameList = {"ggF_Hbb","VBF_Hbb","QCD"   ,"ttbar","WH"    ,"ZH"  ,"ZZ"  ,"ttH"   };
    std::vector<EColor>      colorList       = {kRed     , kBlue   , kYellow, kGreen, kViolet, kCyan, kGray, kOrange};
      
    StackPlot(theCanvas, inputFile, selectionAndRegion, variableName, datasetList, datasetNameList, colorList, modelDataset, modelName, signalDatasetList, signalName, dataDataset, dataName, normalizeBinByBin, xAxisName, yAxisName);

}

void StackAllVariables(const std::string& inputFileName, const std::string& selectionAndRegion, int year, const std::string& modelDataset=NO_PLOT, const std::string& modelName="BKG model",  const std::vector<std::string>& signalDatasetList = {}, const std::string& signalName="X{#rightarrow}YH", const std::string& dataDataset=NO_PLOT, const std::string& dataName="data")
{
    gROOT->SetBatch();

    TFile inputFile(inputFileName.data());

    std::string canvasName = "BackgroundOverlap_" + selectionAndRegion + "_" + std::to_string(year);
    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 900);

    std::vector<std::string> variableVector = {"HH_kinFit_m"};
    std::vector<bool>        normalizeBinByBinVector = {true};
    std::vector<std::string> xAxisVector = {"m^{kinFit}_{Xreco} [GeV]"};
    std::vector<std::string> yAxisVector = {"events/GeV"};
    // std::vector<std::string> variableVector = {"H1_b1_ptRegressed", "H1_b2_ptRegressed", "H2_b1_ptRegressed", "H2_b2_ptRegressed", "H1_m", "H2_m", "HH_m", "FourBjet_sphericity"};
    // std::vector<bool>        normalizeBinByBinVector = {true, true, true, true, true, true, true, false};
    // std::vector<std::string> xAxisVector = {"pT(Hb1) [GeV]", "pT(Hb2) [GeV]", "pT(Yb1) [GeV]", "pT(Yb2) [GeV]", "m_{Hreco} [GeV]", "m_{Yreco} [GeV]", "m_{Xreco} [GeV]", "Four b-jet sphericity" };
    // std::vector<std::string> yAxisVector = {"events/GeV", "events/GeV", "events/GeV", "events/GeV", "events/GeV", "events/GeV", "events/GeV", "events/GeV"};
    theCanvas->DivideSquare(variableVector.size(),0.005,0.005);
    for(size_t vIt = 0; vIt<variableVector.size(); ++vIt)
        StackAllDatasets(theCanvas->cd(vIt+1), &inputFile, selectionAndRegion, variableVector[vIt], modelDataset, modelName, signalDatasetList, signalName, dataDataset, dataName, normalizeBinByBinVector[vIt], xAxisVector[vIt], yAxisVector[vIt]);
    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());

    inputFile.Close();
    delete theCanvas;

    gROOT->SetBatch(false);
}

void doAllStack()
{
    
    // const std::vector<std::string> signalForLMR = 
    // {
    //       "sig_NMSSM_bbbb_MX_300_MY_60", "sig_NMSSM_bbbb_MX_300_MY_70", "sig_NMSSM_bbbb_MX_300_MY_80", "sig_NMSSM_bbbb_MX_300_MY_90", "sig_NMSSM_bbbb_MX_300_MY_100", "sig_NMSSM_bbbb_MX_300_MY_125", "sig_NMSSM_bbbb_MX_300_MY_150", "sig_NMSSM_bbbb_MX_400_MY_60"
    //     , "sig_NMSSM_bbbb_MX_400_MY_70", "sig_NMSSM_bbbb_MX_400_MY_80", "sig_NMSSM_bbbb_MX_400_MY_90", "sig_NMSSM_bbbb_MX_400_MY_100", "sig_NMSSM_bbbb_MX_400_MY_125"
    //     , "sig_NMSSM_bbbb_MX_500_MY_60", "sig_NMSSM_bbbb_MX_500_MY_70", "sig_NMSSM_bbbb_MX_500_MY_80", "sig_NMSSM_bbbb_MX_500_MY_90", "sig_NMSSM_bbbb_MX_500_MY_100", "sig_NMSSM_bbbb_MX_500_MY_125"
    //     , "sig_NMSSM_bbbb_MX_600_MY_60", "sig_NMSSM_bbbb_MX_600_MY_70", "sig_NMSSM_bbbb_MX_600_MY_80", "sig_NMSSM_bbbb_MX_600_MY_90", "sig_NMSSM_bbbb_MX_600_MY_100", "sig_NMSSM_bbbb_MX_600_MY_125"
    //     , "sig_NMSSM_bbbb_MX_700_MY_60", "sig_NMSSM_bbbb_MX_700_MY_70", "sig_NMSSM_bbbb_MX_700_MY_80", "sig_NMSSM_bbbb_MX_700_MY_90", "sig_NMSSM_bbbb_MX_700_MY_100", "sig_NMSSM_bbbb_MX_700_MY_125"
    //     , "sig_NMSSM_bbbb_MX_800_MY_60", "sig_NMSSM_bbbb_MX_800_MY_70", "sig_NMSSM_bbbb_MX_800_MY_80", "sig_NMSSM_bbbb_MX_800_MY_90", "sig_NMSSM_bbbb_MX_800_MY_100", "sig_NMSSM_bbbb_MX_800_MY_125"
    //     , "sig_NMSSM_bbbb_MX_900_MY_60", "sig_NMSSM_bbbb_MX_900_MY_70", "sig_NMSSM_bbbb_MX_900_MY_80", "sig_NMSSM_bbbb_MX_900_MY_90", "sig_NMSSM_bbbb_MX_900_MY_100", "sig_NMSSM_bbbb_MX_900_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1000_MY_60", "sig_NMSSM_bbbb_MX_1000_MY_70", "sig_NMSSM_bbbb_MX_1000_MY_80", "sig_NMSSM_bbbb_MX_1000_MY_90", "sig_NMSSM_bbbb_MX_1000_MY_100", "sig_NMSSM_bbbb_MX_1000_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1400_MY_90", "sig_NMSSM_bbbb_MX_1400_MY_100", "sig_NMSSM_bbbb_MX_1400_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1600_MY_90", "sig_NMSSM_bbbb_MX_1600_MY_100", "sig_NMSSM_bbbb_MX_1600_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1800_MY_90", "sig_NMSSM_bbbb_MX_1800_MY_100", "sig_NMSSM_bbbb_MX_1800_MY_125"
    //     // , "sig_NMSSM_bbbb_MX_2000_MY_90", "sig_NMSSM_bbbb_MX_2000_MY_100", "sig_NMSSM_bbbb_MX_2000_MY_125"
    // };
    // const std::vector<std::string> signalForHMR = 
    // {
    //       "sig_NMSSM_bbbb_MX_300_MY_125"
    //     , "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_400_MY_150", "sig_NMSSM_bbbb_MX_400_MY_200", "sig_NMSSM_bbbb_MX_400_MY_250", "sig_NMSSM_bbbb_MX_500_MY_125"
    //     , "sig_NMSSM_bbbb_MX_500_MY_150", "sig_NMSSM_bbbb_MX_500_MY_200", "sig_NMSSM_bbbb_MX_500_MY_250", "sig_NMSSM_bbbb_MX_500_MY_300", "sig_NMSSM_bbbb_MX_600_MY_125"
    //     , "sig_NMSSM_bbbb_MX_600_MY_150", "sig_NMSSM_bbbb_MX_600_MY_200", "sig_NMSSM_bbbb_MX_600_MY_250", "sig_NMSSM_bbbb_MX_600_MY_300", "sig_NMSSM_bbbb_MX_600_MY_400", "sig_NMSSM_bbbb_MX_700_MY_125"
    //     , "sig_NMSSM_bbbb_MX_700_MY_150", "sig_NMSSM_bbbb_MX_700_MY_200", "sig_NMSSM_bbbb_MX_700_MY_250", "sig_NMSSM_bbbb_MX_700_MY_300", "sig_NMSSM_bbbb_MX_700_MY_400", "sig_NMSSM_bbbb_MX_700_MY_500", "sig_NMSSM_bbbb_MX_800_MY_125"
    //     , "sig_NMSSM_bbbb_MX_800_MY_150", "sig_NMSSM_bbbb_MX_800_MY_200", "sig_NMSSM_bbbb_MX_800_MY_250", "sig_NMSSM_bbbb_MX_800_MY_300", "sig_NMSSM_bbbb_MX_800_MY_400", "sig_NMSSM_bbbb_MX_800_MY_500", "sig_NMSSM_bbbb_MX_800_MY_600", "sig_NMSSM_bbbb_MX_900_MY_125"
    //     , "sig_NMSSM_bbbb_MX_900_MY_150", "sig_NMSSM_bbbb_MX_900_MY_200", "sig_NMSSM_bbbb_MX_900_MY_250", "sig_NMSSM_bbbb_MX_900_MY_300", "sig_NMSSM_bbbb_MX_900_MY_400", "sig_NMSSM_bbbb_MX_900_MY_500", "sig_NMSSM_bbbb_MX_900_MY_600", "sig_NMSSM_bbbb_MX_900_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1000_MY_150", "sig_NMSSM_bbbb_MX_1000_MY_200", "sig_NMSSM_bbbb_MX_1000_MY_250", "sig_NMSSM_bbbb_MX_1000_MY_300", "sig_NMSSM_bbbb_MX_1000_MY_400", "sig_NMSSM_bbbb_MX_1000_MY_500", "sig_NMSSM_bbbb_MX_1000_MY_600", "sig_NMSSM_bbbb_MX_1000_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1100_MY_150", "sig_NMSSM_bbbb_MX_1100_MY_200", "sig_NMSSM_bbbb_MX_1100_MY_250", "sig_NMSSM_bbbb_MX_1100_MY_300", "sig_NMSSM_bbbb_MX_1100_MY_400", "sig_NMSSM_bbbb_MX_1100_MY_500", "sig_NMSSM_bbbb_MX_1100_MY_600", "sig_NMSSM_bbbb_MX_1100_MY_700", "sig_NMSSM_bbbb_MX_1100_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1200_MY_150", "sig_NMSSM_bbbb_MX_1200_MY_200", "sig_NMSSM_bbbb_MX_1200_MY_250", "sig_NMSSM_bbbb_MX_1200_MY_300", "sig_NMSSM_bbbb_MX_1200_MY_400", "sig_NMSSM_bbbb_MX_1200_MY_500", "sig_NMSSM_bbbb_MX_1200_MY_600", "sig_NMSSM_bbbb_MX_1200_MY_700", "sig_NMSSM_bbbb_MX_1200_MY_800", "sig_NMSSM_bbbb_MX_1200_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1400_MY_150", "sig_NMSSM_bbbb_MX_1400_MY_200", "sig_NMSSM_bbbb_MX_1400_MY_250", "sig_NMSSM_bbbb_MX_1400_MY_300", "sig_NMSSM_bbbb_MX_1400_MY_400", "sig_NMSSM_bbbb_MX_1400_MY_500", "sig_NMSSM_bbbb_MX_1400_MY_600", "sig_NMSSM_bbbb_MX_1400_MY_700", "sig_NMSSM_bbbb_MX_1400_MY_800", "sig_NMSSM_bbbb_MX_1400_MY_900", "sig_NMSSM_bbbb_MX_1400_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1600_MY_150", "sig_NMSSM_bbbb_MX_1600_MY_200", "sig_NMSSM_bbbb_MX_1600_MY_250", "sig_NMSSM_bbbb_MX_1600_MY_300", "sig_NMSSM_bbbb_MX_1600_MY_400", "sig_NMSSM_bbbb_MX_1600_MY_500", "sig_NMSSM_bbbb_MX_1600_MY_600", "sig_NMSSM_bbbb_MX_1600_MY_700", "sig_NMSSM_bbbb_MX_1600_MY_800", "sig_NMSSM_bbbb_MX_1600_MY_900", "sig_NMSSM_bbbb_MX_1600_MY_1000", "sig_NMSSM_bbbb_MX_1600_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_125"
    //     , "sig_NMSSM_bbbb_MX_1800_MY_150", "sig_NMSSM_bbbb_MX_1800_MY_200", "sig_NMSSM_bbbb_MX_1800_MY_250", "sig_NMSSM_bbbb_MX_1800_MY_300", "sig_NMSSM_bbbb_MX_1800_MY_400", "sig_NMSSM_bbbb_MX_1800_MY_500", "sig_NMSSM_bbbb_MX_1800_MY_600", "sig_NMSSM_bbbb_MX_1800_MY_700", "sig_NMSSM_bbbb_MX_1800_MY_800", "sig_NMSSM_bbbb_MX_1800_MY_900", "sig_NMSSM_bbbb_MX_1800_MY_1000", "sig_NMSSM_bbbb_MX_1800_MY_1200", "sig_NMSSM_bbbb_MX_1800_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_1600"
    //     // , "sig_NMSSM_bbbb_MX_2000_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_150", "sig_NMSSM_bbbb_MX_2000_MY_200", "sig_NMSSM_bbbb_MX_2000_MY_250", "sig_NMSSM_bbbb_MX_2000_MY_300", "sig_NMSSM_bbbb_MX_2000_MY_400", "sig_NMSSM_bbbb_MX_2000_MY_500", "sig_NMSSM_bbbb_MX_2000_MY_600", "sig_NMSSM_bbbb_MX_2000_MY_700", "sig_NMSSM_bbbb_MX_2000_MY_800", "sig_NMSSM_bbbb_MX_2000_MY_900", "sig_NMSSM_bbbb_MX_2000_MY_1000", "sig_NMSSM_bbbb_MX_2000_MY_1200", "sig_NMSSM_bbbb_MX_2000_MY_1400", "sig_NMSSM_bbbb_MX_2000_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_1800"
    // };


    const std::vector<std::string> signalForLMR = 
    {
        //   /* "sig_NMSSM_bbbb_MX_300_MY_60", "sig_NMSSM_bbbb_MX_300_MY_70", "sig_NMSSM_bbbb_MX_300_MY_80" ,*/ "sig_NMSSM_bbbb_MX_300_MY_90"/* , "sig_NMSSM_bbbb_MX_300_MY_100", "sig_NMSSM_bbbb_MX_300_MY_125", "sig_NMSSM_bbbb_MX_300_MY_150" */
        // /* , "sig_NMSSM_bbbb_MX_400_MY_60", "sig_NMSSM_bbbb_MX_400_MY_70", "sig_NMSSM_bbbb_MX_400_MY_80" */, "sig_NMSSM_bbbb_MX_400_MY_90"/* , "sig_NMSSM_bbbb_MX_400_MY_100", "sig_NMSSM_bbbb_MX_400_MY_125" */
        // /* , "sig_NMSSM_bbbb_MX_500_MY_60", "sig_NMSSM_bbbb_MX_500_MY_70", "sig_NMSSM_bbbb_MX_500_MY_80" */, "sig_NMSSM_bbbb_MX_500_MY_90"/* , "sig_NMSSM_bbbb_MX_500_MY_100", "sig_NMSSM_bbbb_MX_500_MY_125" */
        // /* , "sig_NMSSM_bbbb_MX_600_MY_60", "sig_NMSSM_bbbb_MX_600_MY_70", "sig_NMSSM_bbbb_MX_600_MY_80" */, "sig_NMSSM_bbbb_MX_600_MY_90"/* , "sig_NMSSM_bbbb_MX_600_MY_100", "sig_NMSSM_bbbb_MX_600_MY_125" */
        // /* , "sig_NMSSM_bbbb_MX_700_MY_60", "sig_NMSSM_bbbb_MX_700_MY_70", "sig_NMSSM_bbbb_MX_700_MY_80" */, "sig_NMSSM_bbbb_MX_700_MY_90"/* , "sig_NMSSM_bbbb_MX_700_MY_100", "sig_NMSSM_bbbb_MX_700_MY_125" */
        // /* , "sig_NMSSM_bbbb_MX_800_MY_60", "sig_NMSSM_bbbb_MX_800_MY_70", "sig_NMSSM_bbbb_MX_800_MY_80" */, "sig_NMSSM_bbbb_MX_800_MY_90"/* , "sig_NMSSM_bbbb_MX_800_MY_100", "sig_NMSSM_bbbb_MX_800_MY_125" */
        // /* , "sig_NMSSM_bbbb_MX_900_MY_60", "sig_NMSSM_bbbb_MX_900_MY_70", "sig_NMSSM_bbbb_MX_900_MY_80" */, "sig_NMSSM_bbbb_MX_900_MY_90"/* , "sig_NMSSM_bbbb_MX_900_MY_100", "sig_NMSSM_bbbb_MX_900_MY_125" */
        // /* , "sig_NMSSM_bbbb_MX_1000_MY_60", "sig_NMSSM_bbbb_MX_1000_MY_70", "sig_NMSSM_bbbb_MX_1000_MY_80" */, "sig_NMSSM_bbbb_MX_1000_MY_90"/* , "sig_NMSSM_bbbb_MX_1000_MY_100", "sig_NMSSM_bbbb_MX_1000_MY_125" */
        // , "sig_NMSSM_bbbb_MX_1100_MY_90"/* , "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125" */
        // , "sig_NMSSM_bbbb_MX_1400_MY_90"/* , "sig_NMSSM_bbbb_MX_1400_MY_100", "sig_NMSSM_bbbb_MX_1400_MY_125" */
        // , "sig_NMSSM_bbbb_MX_1600_MY_90"/* , "sig_NMSSM_bbbb_MX_1600_MY_100", "sig_NMSSM_bbbb_MX_1600_MY_125" */
        // , "sig_NMSSM_bbbb_MX_1800_MY_90"/* , "sig_NMSSM_bbbb_MX_1800_MY_100", "sig_NMSSM_bbbb_MX_1800_MY_125" */
        // // , "sig_NMSSM_bbbb_MX_2000_MY_90", "sig_NMSSM_bbbb_MX_2000_MY_100", "sig_NMSSM_bbbb_MX_2000_MY_125"
    };
    const std::vector<std::string> signalForHMR = 
    {
        // //   "sig_NMSSM_bbbb_MX_300_MY_125"
        // // , "sig_NMSSM_bbbb_MX_400_MY_125", "sig_NMSSM_bbbb_MX_400_MY_150", "sig_NMSSM_bbbb_MX_400_MY_200", "sig_NMSSM_bbbb_MX_400_MY_250"
        // /* , "sig_NMSSM_bbbb_MX_500_MY_125", "sig_NMSSM_bbbb_MX_500_MY_150", "sig_NMSSM_bbbb_MX_500_MY_200", "sig_NMSSM_bbbb_MX_500_MY_250" ,*/ "sig_NMSSM_bbbb_MX_500_MY_300"/*  */
        // /* , "sig_NMSSM_bbbb_MX_600_MY_125", "sig_NMSSM_bbbb_MX_600_MY_150", "sig_NMSSM_bbbb_MX_600_MY_200", "sig_NMSSM_bbbb_MX_600_MY_250" */, "sig_NMSSM_bbbb_MX_600_MY_300"/* , "sig_NMSSM_bbbb_MX_600_MY_400" */
        // /* , "sig_NMSSM_bbbb_MX_700_MY_125", "sig_NMSSM_bbbb_MX_700_MY_150", "sig_NMSSM_bbbb_MX_700_MY_200", "sig_NMSSM_bbbb_MX_700_MY_250" */, "sig_NMSSM_bbbb_MX_700_MY_300"/* , "sig_NMSSM_bbbb_MX_700_MY_400", "sig_NMSSM_bbbb_MX_700_MY_500" */
        // /* , "sig_NMSSM_bbbb_MX_800_MY_125", "sig_NMSSM_bbbb_MX_800_MY_150", "sig_NMSSM_bbbb_MX_800_MY_200", "sig_NMSSM_bbbb_MX_800_MY_250" */, "sig_NMSSM_bbbb_MX_800_MY_300"/* , "sig_NMSSM_bbbb_MX_800_MY_400", "sig_NMSSM_bbbb_MX_800_MY_500", "sig_NMSSM_bbbb_MX_800_MY_600" */
        // /* , "sig_NMSSM_bbbb_MX_900_MY_125", "sig_NMSSM_bbbb_MX_900_MY_150", "sig_NMSSM_bbbb_MX_900_MY_200", "sig_NMSSM_bbbb_MX_900_MY_250" */, "sig_NMSSM_bbbb_MX_900_MY_300"/* , "sig_NMSSM_bbbb_MX_900_MY_400", "sig_NMSSM_bbbb_MX_900_MY_500", "sig_NMSSM_bbbb_MX_900_MY_600", "sig_NMSSM_bbbb_MX_900_MY_700" */
        // /* , "sig_NMSSM_bbbb_MX_1000_MY_125", "sig_NMSSM_bbbb_MX_1000_MY_150", "sig_NMSSM_bbbb_MX_1000_MY_200", "sig_NMSSM_bbbb_MX_1000_MY_250" */, "sig_NMSSM_bbbb_MX_1000_MY_300"/* , "sig_NMSSM_bbbb_MX_1000_MY_400", "sig_NMSSM_bbbb_MX_1000_MY_500", "sig_NMSSM_bbbb_MX_1000_MY_600", "sig_NMSSM_bbbb_MX_1000_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100" */
        // /* , "sig_NMSSM_bbbb_MX_1100_MY_125", "sig_NMSSM_bbbb_MX_1100_MY_150", "sig_NMSSM_bbbb_MX_1100_MY_200", "sig_NMSSM_bbbb_MX_1100_MY_250" */, "sig_NMSSM_bbbb_MX_1100_MY_300"/* , "sig_NMSSM_bbbb_MX_1100_MY_400", "sig_NMSSM_bbbb_MX_1100_MY_500", "sig_NMSSM_bbbb_MX_1100_MY_600", "sig_NMSSM_bbbb_MX_1100_MY_700", "sig_NMSSM_bbbb_MX_1100_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_900" */
        // /* , "sig_NMSSM_bbbb_MX_1200_MY_125", "sig_NMSSM_bbbb_MX_1200_MY_150", "sig_NMSSM_bbbb_MX_1200_MY_200", "sig_NMSSM_bbbb_MX_1200_MY_250" */, "sig_NMSSM_bbbb_MX_1200_MY_300"/* , "sig_NMSSM_bbbb_MX_1200_MY_400", "sig_NMSSM_bbbb_MX_1200_MY_500", "sig_NMSSM_bbbb_MX_1200_MY_600", "sig_NMSSM_bbbb_MX_1200_MY_700", "sig_NMSSM_bbbb_MX_1200_MY_800", "sig_NMSSM_bbbb_MX_1200_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_1000" */
        // /* , "sig_NMSSM_bbbb_MX_1400_MY_125", "sig_NMSSM_bbbb_MX_1400_MY_150", "sig_NMSSM_bbbb_MX_1400_MY_200", "sig_NMSSM_bbbb_MX_1400_MY_250" */, "sig_NMSSM_bbbb_MX_1400_MY_300"/* , "sig_NMSSM_bbbb_MX_1400_MY_400", "sig_NMSSM_bbbb_MX_1400_MY_500", "sig_NMSSM_bbbb_MX_1400_MY_600", "sig_NMSSM_bbbb_MX_1400_MY_700", "sig_NMSSM_bbbb_MX_1400_MY_800", "sig_NMSSM_bbbb_MX_1400_MY_900", "sig_NMSSM_bbbb_MX_1400_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_1200" */
        // /* , "sig_NMSSM_bbbb_MX_1600_MY_125", "sig_NMSSM_bbbb_MX_1600_MY_150", "sig_NMSSM_bbbb_MX_1600_MY_200", "sig_NMSSM_bbbb_MX_1600_MY_250" */, "sig_NMSSM_bbbb_MX_1600_MY_300"/* , "sig_NMSSM_bbbb_MX_1600_MY_400", "sig_NMSSM_bbbb_MX_1600_MY_500", "sig_NMSSM_bbbb_MX_1600_MY_600", "sig_NMSSM_bbbb_MX_1600_MY_700", "sig_NMSSM_bbbb_MX_1600_MY_800", "sig_NMSSM_bbbb_MX_1600_MY_900", "sig_NMSSM_bbbb_MX_1600_MY_1000", "sig_NMSSM_bbbb_MX_1600_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_1400" */
        // /* , "sig_NMSSM_bbbb_MX_1800_MY_125", "sig_NMSSM_bbbb_MX_1800_MY_150", "sig_NMSSM_bbbb_MX_1800_MY_200", "sig_NMSSM_bbbb_MX_1800_MY_250" */, "sig_NMSSM_bbbb_MX_1800_MY_300"/* , "sig_NMSSM_bbbb_MX_1800_MY_400", "sig_NMSSM_bbbb_MX_1800_MY_500", "sig_NMSSM_bbbb_MX_1800_MY_600", "sig_NMSSM_bbbb_MX_1800_MY_700", "sig_NMSSM_bbbb_MX_1800_MY_800", "sig_NMSSM_bbbb_MX_1800_MY_900", "sig_NMSSM_bbbb_MX_1800_MY_1000", "sig_NMSSM_bbbb_MX_1800_MY_1200", "sig_NMSSM_bbbb_MX_1800_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_1600" */
        // // , "sig_NMSSM_bbbb_MX_2000_MY_125", "sig_NMSSM_bbbb_MX_2000_MY_150", "sig_NMSSM_bbbb_MX_2000_MY_200", "sig_NMSSM_bbbb_MX_2000_MY_250", "sig_NMSSM_bbbb_MX_2000_MY_300", "sig_NMSSM_bbbb_MX_2000_MY_400", "sig_NMSSM_bbbb_MX_2000_MY_500", "sig_NMSSM_bbbb_MX_2000_MY_600", "sig_NMSSM_bbbb_MX_2000_MY_700", "sig_NMSSM_bbbb_MX_2000_MY_800", "sig_NMSSM_bbbb_MX_2000_MY_900", "sig_NMSSM_bbbb_MX_2000_MY_1000", "sig_NMSSM_bbbb_MX_2000_MY_1200", "sig_NMSSM_bbbb_MX_2000_MY_1400", "sig_NMSSM_bbbb_MX_2000_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_1800"
    };

    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsLMR_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model", signalForLMR, "X{#rightarrow}YH");
    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsHMR_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model", signalForHMR, "X{#rightarrow}YH");
    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsLMR_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model", {}, "X{#rightarrow}YH", "data_BTagCSV_background", "data");
    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsHMR_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model", {}, "X{#rightarrow}YH", "data_BTagCSV_background", "data");

    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsLMR_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model", signalForLMR, "X{#rightarrow}YH");
    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsHMR_SignalRegion"        , "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model", signalForHMR, "X{#rightarrow}YH");
    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsLMR_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundLMR", "Bkg model", {}, "X{#rightarrow}YH", "data_BTagCSV_background", "data");
    // StackAllVariables("2016DataPlots_NMSSM_XYH_bbbb_all_v1/outPlotter.root", "selectionbJetsHMR_ControlRegionBlinded", "data_BTagCSV_dataDriven_backgroundHMR", "Bkg model", {}, "X{#rightarrow}YH", "data_BTagCSV_background", "data");

    StackAllVariables("DataPlots_fullSubmission_2016_v17/outPlotter.root", "selectionbJets_SignalRegion"           , 2016, "data_BTagCSV_dataDriven_kinFit", "Bkg model", {"sig_NMSSM_bbbb_MX_700_MY_300"}, "X#rightarrowYH (m_{X} = 700 GeV, m_{Y} = 300 GeV)");
    StackAllVariables("DataPlots_fullSubmission_2016_v17/outPlotter.root", "selectionbJets_ValidationRegionBlinded", 2016, "data_BTagCSV_dataDriven_kinFit", "Bkg model", {"sig_NMSSM_bbbb_MX_700_MY_300"}, "X#rightarrowYH (m_{X} = 700 GeV, m_{Y} = 300 GeV)", "data_BTagCSV", "data");

    StackAllVariables("DataPlots_fullSubmission_2017_v17/outPlotter.root", "selectionbJets_SignalRegion"           , 2017, "data_BTagCSV_dataDriven_kinFit", "Bkg model", {"sig_NMSSM_bbbb_MX_700_MY_300"}, "X#rightarrowYH (m_{X} = 700 GeV, m_{Y} = 300 GeV)");
    StackAllVariables("DataPlots_fullSubmission_2017_v17/outPlotter.root", "selectionbJets_ValidationRegionBlinded", 2017, "data_BTagCSV_dataDriven_kinFit", "Bkg model", {"sig_NMSSM_bbbb_MX_700_MY_300"}, "X#rightarrowYH (m_{X} = 700 GeV, m_{Y} = 300 GeV)", "data_BTagCSV", "data");

    StackAllVariables("DataPlots_fullSubmission_2018_v17/outPlotter.root", "selectionbJets_SignalRegion"           , 2018, "data_BTagCSV_dataDriven_kinFit", "Bkg model", {"sig_NMSSM_bbbb_MX_700_MY_300"}, "X#rightarrowYH (m_{X} = 700 GeV, m_{Y} = 300 GeV)");
    StackAllVariables("DataPlots_fullSubmission_2018_v17/outPlotter.root", "selectionbJets_ValidationRegionBlinded", 2018, "data_BTagCSV_dataDriven_kinFit", "Bkg model", {"sig_NMSSM_bbbb_MX_700_MY_300"}, "X#rightarrowYH (m_{X} = 700 GeV, m_{Y} = 300 GeV)", "data_BTagCSV", "data");

}



// {
//     "sig_NMSSM_bbbb_MX_300_MY_60", "sig_NMSSM_bbbb_MX_300_MY_70", "sig_NMSSM_bbbb_MX_300_MY_80", "sig_NMSSM_bbbb_MX_300_MY_90", "sig_NMSSM_bbbb_MX_300_MY_100", "sig_NMSSM_bbbb_MX_300_MY_125",
//     "sig_NMSSM_bbbb_MX_300_MY_150", "sig_NMSSM_bbbb_MX_400_MY_60", "sig_NMSSM_bbbb_MX_400_MY_70", "sig_NMSSM_bbbb_MX_400_MY_80", "sig_NMSSM_bbbb_MX_400_MY_90", "sig_NMSSM_bbbb_MX_400_MY_100", "sig_NMSSM_bbbb_MX_400_MY_125",
//     "sig_NMSSM_bbbb_MX_500_MY_60", "sig_NMSSM_bbbb_MX_500_MY_70", "sig_NMSSM_bbbb_MX_500_MY_80", "sig_NMSSM_bbbb_MX_500_MY_90", "sig_NMSSM_bbbb_MX_500_MY_100", "sig_NMSSM_bbbb_MX_500_MY_125",
//     "sig_NMSSM_bbbb_MX_600_MY_60", "sig_NMSSM_bbbb_MX_600_MY_70", "sig_NMSSM_bbbb_MX_600_MY_80", "sig_NMSSM_bbbb_MX_600_MY_90", "sig_NMSSM_bbbb_MX_600_MY_100", "sig_NMSSM_bbbb_MX_600_MY_125",
//     "sig_NMSSM_bbbb_MX_700_MY_60", "sig_NMSSM_bbbb_MX_700_MY_70", "sig_NMSSM_bbbb_MX_700_MY_80", "sig_NMSSM_bbbb_MX_700_MY_90", "sig_NMSSM_bbbb_MX_700_MY_100", "sig_NMSSM_bbbb_MX_700_MY_125",
//     "sig_NMSSM_bbbb_MX_800_MY_60", "sig_NMSSM_bbbb_MX_800_MY_70", "sig_NMSSM_bbbb_MX_800_MY_80", "sig_NMSSM_bbbb_MX_800_MY_90", "sig_NMSSM_bbbb_MX_800_MY_100", "sig_NMSSM_bbbb_MX_800_MY_125",
//     "sig_NMSSM_bbbb_MX_900_MY_60", "sig_NMSSM_bbbb_MX_900_MY_70", "sig_NMSSM_bbbb_MX_900_MY_80", "sig_NMSSM_bbbb_MX_900_MY_90", "sig_NMSSM_bbbb_MX_900_MY_100", "sig_NMSSM_bbbb_MX_900_MY_125",
//     "sig_NMSSM_bbbb_MX_1000_MY_60", "sig_NMSSM_bbbb_MX_1000_MY_70", "sig_NMSSM_bbbb_MX_1000_MY_80", "sig_NMSSM_bbbb_MX_1000_MY_90", "sig_NMSSM_bbbb_MX_1000_MY_100", "sig_NMSSM_bbbb_MX_1000_MY_125",
//     "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125",
//     "sig_NMSSM_bbbb_MX_1400_MY_90", "sig_NMSSM_bbbb_MX_1400_MY_100", "sig_NMSSM_bbbb_MX_1400_MY_125",
//     "sig_NMSSM_bbbb_MX_1600_MY_90", "sig_NMSSM_bbbb_MX_1600_MY_100", "sig_NMSSM_bbbb_MX_1600_MY_125",
//     "sig_NMSSM_bbbb_MX_1800_MY_90", "sig_NMSSM_bbbb_MX_1800_MY_100", "sig_NMSSM_bbbb_MX_1800_MY_125",
//     "sig_NMSSM_bbbb_MX_2000_MY_90", "sig_NMSSM_bbbb_MX_2000_MY_100", "sig_NMSSM_bbbb_MX_2000_MY_125"
// };


// {
//     "sig_NMSSM_bbbb_MX_300_MY_125",
//     "sig_NMSSM_bbbb_MX_400_MY_125",
//     "sig_NMSSM_bbbb_MX_400_MY_150", "sig_NMSSM_bbbb_MX_400_MY_200", "sig_NMSSM_bbbb_MX_400_MY_250", "sig_NMSSM_bbbb_MX_500_MY_125",
//     "sig_NMSSM_bbbb_MX_500_MY_150", "sig_NMSSM_bbbb_MX_500_MY_200", "sig_NMSSM_bbbb_MX_500_MY_250", "sig_NMSSM_bbbb_MX_500_MY_300", "sig_NMSSM_bbbb_MX_600_MY_125",
//     "sig_NMSSM_bbbb_MX_600_MY_150", "sig_NMSSM_bbbb_MX_600_MY_200", "sig_NMSSM_bbbb_MX_600_MY_250", "sig_NMSSM_bbbb_MX_600_MY_300", "sig_NMSSM_bbbb_MX_600_MY_400", "sig_NMSSM_bbbb_MX_700_MY_125",
//     "sig_NMSSM_bbbb_MX_700_MY_150", "sig_NMSSM_bbbb_MX_700_MY_200", "sig_NMSSM_bbbb_MX_700_MY_250", "sig_NMSSM_bbbb_MX_700_MY_300", "sig_NMSSM_bbbb_MX_700_MY_400", "sig_NMSSM_bbbb_MX_700_MY_500", "sig_NMSSM_bbbb_MX_800_MY_125",
//     "sig_NMSSM_bbbb_MX_800_MY_150", "sig_NMSSM_bbbb_MX_800_MY_200", "sig_NMSSM_bbbb_MX_800_MY_250", "sig_NMSSM_bbbb_MX_800_MY_300", "sig_NMSSM_bbbb_MX_800_MY_400", "sig_NMSSM_bbbb_MX_800_MY_500", "sig_NMSSM_bbbb_MX_800_MY_600", "sig_NMSSM_bbbb_MX_900_MY_125",
//     "sig_NMSSM_bbbb_MX_900_MY_150", "sig_NMSSM_bbbb_MX_900_MY_200", "sig_NMSSM_bbbb_MX_900_MY_250", "sig_NMSSM_bbbb_MX_900_MY_300", "sig_NMSSM_bbbb_MX_900_MY_400", "sig_NMSSM_bbbb_MX_900_MY_500", "sig_NMSSM_bbbb_MX_900_MY_600", "sig_NMSSM_bbbb_MX_900_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_125",
//     "sig_NMSSM_bbbb_MX_1000_MY_150", "sig_NMSSM_bbbb_MX_1000_MY_200", "sig_NMSSM_bbbb_MX_1000_MY_250", "sig_NMSSM_bbbb_MX_1000_MY_300", "sig_NMSSM_bbbb_MX_1000_MY_400", "sig_NMSSM_bbbb_MX_1000_MY_500", "sig_NMSSM_bbbb_MX_1000_MY_600", "sig_NMSSM_bbbb_MX_1000_MY_700", "sig_NMSSM_bbbb_MX_1000_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_90", "sig_NMSSM_bbbb_MX_1100_MY_100", "sig_NMSSM_bbbb_MX_1100_MY_125",
//     "sig_NMSSM_bbbb_MX_1100_MY_150", "sig_NMSSM_bbbb_MX_1100_MY_200", "sig_NMSSM_bbbb_MX_1100_MY_250", "sig_NMSSM_bbbb_MX_1100_MY_300", "sig_NMSSM_bbbb_MX_1100_MY_400", "sig_NMSSM_bbbb_MX_1100_MY_500", "sig_NMSSM_bbbb_MX_1100_MY_600", "sig_NMSSM_bbbb_MX_1100_MY_700", "sig_NMSSM_bbbb_MX_1100_MY_800", "sig_NMSSM_bbbb_MX_1100_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_125",
//     "sig_NMSSM_bbbb_MX_1200_MY_150", "sig_NMSSM_bbbb_MX_1200_MY_200", "sig_NMSSM_bbbb_MX_1200_MY_250", "sig_NMSSM_bbbb_MX_1200_MY_300", "sig_NMSSM_bbbb_MX_1200_MY_400", "sig_NMSSM_bbbb_MX_1200_MY_500", "sig_NMSSM_bbbb_MX_1200_MY_600", "sig_NMSSM_bbbb_MX_1200_MY_700", "sig_NMSSM_bbbb_MX_1200_MY_800", "sig_NMSSM_bbbb_MX_1200_MY_900", "sig_NMSSM_bbbb_MX_1200_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_125",
//     "sig_NMSSM_bbbb_MX_1400_MY_150", "sig_NMSSM_bbbb_MX_1400_MY_200", "sig_NMSSM_bbbb_MX_1400_MY_250", "sig_NMSSM_bbbb_MX_1400_MY_300", "sig_NMSSM_bbbb_MX_1400_MY_400", "sig_NMSSM_bbbb_MX_1400_MY_500", "sig_NMSSM_bbbb_MX_1400_MY_600", "sig_NMSSM_bbbb_MX_1400_MY_700", "sig_NMSSM_bbbb_MX_1400_MY_800", "sig_NMSSM_bbbb_MX_1400_MY_900", "sig_NMSSM_bbbb_MX_1400_MY_1000", "sig_NMSSM_bbbb_MX_1400_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_125",
//     "sig_NMSSM_bbbb_MX_1600_MY_150", "sig_NMSSM_bbbb_MX_1600_MY_200", "sig_NMSSM_bbbb_MX_1600_MY_250", "sig_NMSSM_bbbb_MX_1600_MY_300", "sig_NMSSM_bbbb_MX_1600_MY_400", "sig_NMSSM_bbbb_MX_1600_MY_500", "sig_NMSSM_bbbb_MX_1600_MY_600", "sig_NMSSM_bbbb_MX_1600_MY_700", "sig_NMSSM_bbbb_MX_1600_MY_800", "sig_NMSSM_bbbb_MX_1600_MY_900", "sig_NMSSM_bbbb_MX_1600_MY_1000", "sig_NMSSM_bbbb_MX_1600_MY_1200", "sig_NMSSM_bbbb_MX_1600_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_125",
//     "sig_NMSSM_bbbb_MX_1800_MY_150", "sig_NMSSM_bbbb_MX_1800_MY_200", "sig_NMSSM_bbbb_MX_1800_MY_250", "sig_NMSSM_bbbb_MX_1800_MY_300", "sig_NMSSM_bbbb_MX_1800_MY_400", "sig_NMSSM_bbbb_MX_1800_MY_500", "sig_NMSSM_bbbb_MX_1800_MY_600", "sig_NMSSM_bbbb_MX_1800_MY_700", "sig_NMSSM_bbbb_MX_1800_MY_800", "sig_NMSSM_bbbb_MX_1800_MY_900", "sig_NMSSM_bbbb_MX_1800_MY_1000", "sig_NMSSM_bbbb_MX_1800_MY_1200", "sig_NMSSM_bbbb_MX_1800_MY_1400", "sig_NMSSM_bbbb_MX_1800_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_125",
//     "sig_NMSSM_bbbb_MX_2000_MY_150", "sig_NMSSM_bbbb_MX_2000_MY_200", "sig_NMSSM_bbbb_MX_2000_MY_250", "sig_NMSSM_bbbb_MX_2000_MY_300", "sig_NMSSM_bbbb_MX_2000_MY_400", "sig_NMSSM_bbbb_MX_2000_MY_500", "sig_NMSSM_bbbb_MX_2000_MY_600", "sig_NMSSM_bbbb_MX_2000_MY_700", "sig_NMSSM_bbbb_MX_2000_MY_800", "sig_NMSSM_bbbb_MX_2000_MY_900", "sig_NMSSM_bbbb_MX_2000_MY_1000", "sig_NMSSM_bbbb_MX_2000_MY_1200", "sig_NMSSM_bbbb_MX_2000_MY_1400", "sig_NMSSM_bbbb_MX_2000_MY_1600", "sig_NMSSM_bbbb_MX_2000_MY_1800"
// };