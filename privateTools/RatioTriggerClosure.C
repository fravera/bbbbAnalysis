#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGaxis.h"
#include <string>
#include <TROOT.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TGraphAsymmErrors.h>



std::tuple<TGraphAsymmErrors*, TH1D*> dividePlots(const TGraphAsymmErrors* referencePlot, const TH1D* inputPlot)
{
    std::string ratioPlotName = std::string(inputPlot->GetName()) + "_ratio";
    std::string errorPlotName = ratioPlotName + "Error";
    int numberOfBins = inputPlot->GetNbinsX();
    TH1D* ratioPlot  = (TH1D*)inputPlot->Clone(ratioPlotName.data());
    TGraphAsymmErrors* ratioError = (TGraphAsymmErrors*)referencePlot->Clone(errorPlotName.data());
    for(int nBin = 1; nBin<=numberOfBins; ++nBin)
    {
        double x,referenceValue;
        referencePlot->GetPoint(nBin-1,x,referenceValue);
        if(referenceValue == 0.) 
        {
            referenceValue = 1.;
        }
        // std::cout<<referenceValue<<" " << ratioPlot->GetBinContent(nBin)<<" ";
        ratioPlot->SetBinContent(nBin,inputPlot->GetBinContent(nBin)/referenceValue);
        // std::cout << ratioPlot->GetBinContent(nBin)<<std::endl;
        ratioPlot->SetBinError  (nBin,inputPlot->GetBinError  (nBin)/referenceValue);
        ratioError->SetPoint(nBin-1,x,1);
        ratioError->SetPointError(nBin-1,referencePlot->GetErrorXlow(nBin-1), referencePlot->GetErrorXhigh(nBin-1), referencePlot->GetErrorYlow(nBin-1)/referenceValue, referencePlot->GetErrorYhigh(nBin-1)/referenceValue);
    }

    // ratioError->SetFillStyle(3002);
    // ratioError->SetFillColor(kBlack);

    return {ratioError,ratioPlot};
}

std::tuple<TGraph*,TGraphAsymmErrors*>  AddSystematicToPlot(const TH1D* inputPlot, const TH1D* inputPlotUp, const TH1D* inputPlotDown, float xMin, float xMax)
{
    std::string valuePlotName = std::string(inputPlot->GetName()) + "_value";
    std::string errorPlotName = std::string(inputPlot->GetName()) + "_syst";
    int numberOfBins = inputPlot->GetNbinsX();
    TGraph* outputPlot  = new TGraph();
    outputPlot->SetNameTitle(valuePlotName.data(),valuePlotName.data());
    TGraphAsymmErrors* outputError  = new TGraphAsymmErrors();
    outputError->SetNameTitle(errorPlotName.data(),errorPlotName.data());
    for(int nBin = 1; nBin<=numberOfBins; ++nBin)
    {
        // if(inputPlot->GetBinCenter(nBin)<xMin || inputPlot->GetBinCenter(nBin)>xMax) continue;
        // if(inputPlot->GetBinContent(nBin) == 0.) continue;
        outputPlot->SetPoint(outputPlot->GetN(), inputPlot->GetBinCenter(nBin), inputPlot->GetBinContent(nBin));
        outputError->SetPoint(outputError->GetN(), inputPlot->GetBinCenter(nBin), inputPlot->GetBinContent(nBin));
        outputError->SetPointError(outputError->GetN()-1, inputPlot->GetBinWidth(nBin)/2.,inputPlot->GetBinWidth(nBin)/2., -(inputPlotDown->GetBinContent(nBin)-inputPlot->GetBinContent(nBin)), +(inputPlotUp  ->GetBinContent(nBin)-inputPlot->GetBinContent(nBin)));
    }

    outputError->SetFillStyle(3002);
    outputError->SetFillColor(kBlack);

    return {outputPlot,outputError};
}


void RatioPlot(TVirtualPad *theCanvas, TH1D *referenceHistogram, TH1D *referenceHistogramUp, TH1D *referenceHistogramDown, TH1D *inputHistogram, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "", std::string title = "")
{
 
    if(rebinNumber!=1)
    {
        referenceHistogram->Rebin(rebinNumber);
        referenceHistogramUp->Rebin(rebinNumber);
        referenceHistogramDown->Rebin(rebinNumber);
        inputHistogram->Rebin(rebinNumber);
    }

    std::tuple<TGraph*,TGraphAsymmErrors*> refPlotAndBand = AddSystematicToPlot(referenceHistogram,referenceHistogramUp,referenceHistogramDown, xMin, xMax);
    std::tuple<TGraphAsymmErrors*, TH1D*>  theRatioPlots  = dividePlots(std::get<1>(refPlotAndBand), inputHistogram);

    
    auto theLegend = new TLegend(0.15,0.75,0.88,0.88);
    theLegend->SetNColumns(3);
    theLegend->SetTextSize(0.05);


    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.35, 1, 1.0);
    pad1->SetLeftMargin(0.12);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    inputHistogram->Draw("E");               // Draw inputHistogram
    inputHistogram->SetAxisRange(xMin,xMax);               // Draw inputHistogram
    inputHistogram->SetMaximum(inputHistogram->GetMaximum()*1.4);
    std::get<0>(refPlotAndBand)->SetMarkerStyle(20);
    std::get<0>(refPlotAndBand)->SetMarkerSize(0.4);
    std::get<0>(refPlotAndBand)->Draw("same p");         // Draw referenceHistogram on top of inputHistogram
    // std::get<1>(refPlotAndBand)->Draw("same p");         // Draw referenceHistogram on top of inputHistogram
    theLegend->AddEntry(std::get<0>(refPlotAndBand),"weighted", "pl");
    theLegend->AddEntry(std::get<1>(refPlotAndBand),"weighted unc.", "f");
    std::get<0>(refPlotAndBand)->GetYaxis()->SetTitleSize(0.07);
    std::get<0>(refPlotAndBand)->GetYaxis()->SetTitleFont(62);
    std::get<0>(refPlotAndBand)->GetYaxis()->SetTitleOffset(0.95);
    std::get<0>(refPlotAndBand)->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    std::get<0>(refPlotAndBand)->GetYaxis()->SetLabelSize(0.06);
    std::get<0>(refPlotAndBand)->GetYaxis()->SetTitle(yAxis.data());
    std::get<0>(refPlotAndBand)->SetTitle(title.data());
    std::get<0>(refPlotAndBand)->SetLineColor(kBlack);
    std::get<0>(refPlotAndBand)->SetMarkerColor(kBlack);
    std::get<0>(refPlotAndBand)->SetLineWidth(1);
    std::get<1>(refPlotAndBand)->Draw("sameE2");         // Draw referenceHistogram on top of inputHistogram
    float yMaximum = std::get<0>(refPlotAndBand)->GetMaximum();


    theLegend->AddEntry(inputHistogram,"triggered", "epl");
    inputHistogram->SetStats(0);          // No statistics on upper plot
    inputHistogram->SetTitle(title.data()); // Remove the ratio title
    inputHistogram->Draw("E same");               // Draw inputHistogram
    // inputHistogram settings
    inputHistogram->SetLineColor(kRed);
    inputHistogram->SetMarkerColor(kRed);
    inputHistogram->SetLineWidth(2);
    // if(yMaximum < inputHistogram->GetMaximum()) yMaximum = inputHistogram->GetMaximum();

    // std::get<0>(refPlotAndBand)->SetMaximum(yMaximum * 1.3);
    // gROOT->ForceStyle();
    theLegend->Draw("");

    // lower plot will be in pad
    theCanvas->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.35);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->SetGridy(); // horizontal grid
    pad2->Draw();
    pad2->SetLeftMargin(0.12);
    pad2->SetBottomMargin(0.3);
    pad2->cd();       // pad2 becomes the current pad

    // // Define the ratio plot


    auto *ratio = std::get<1>(theRatioPlots);
    auto *ratioError = std::get<0>(theRatioPlots);
    // TH1D *ratio = (TH1D*)inputHistogramVector.at(hIt)->Clone("ratio");
    ratio->SetLineColor(kRed);
    ratio->SetMarkerColor(kRed);
    ratio->SetMinimum(0.5);  // Define Y ..
    ratio->SetMaximum(1.5); // .. range
    ratio->SetStats(0);      // No statistics on lower plot
    // ratio->Divide(referenceHistogram);
    ratio->SetMarkerStyle(21);
    ratio->SetMarkerSize(0.3);

    // Ratio plot (ratio) settings
    ratio->SetTitle(""); // Remove the ratio title

    
    ratio->SetAxisRange(xMin,xMax);               // Draw inputHistogram
    ratio->Draw("ep");       // Draw the ratio plot
    ratioError->Draw("same E2");
    // Y axis ratio plot settings
    ratio->GetYaxis()->SetTitle("ratio");
    ratio->GetYaxis()->SetNdivisions(505);
    ratio->GetYaxis()->SetTitleSize(0.1);
    ratio->GetYaxis()->SetTitleFont(62);
    ratio->GetYaxis()->SetTitleOffset(0.5);
    ratio->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    ratio->GetYaxis()->SetLabelSize(0.1);

    // X axis ratio plot settings
    ratio->GetXaxis()->SetTitle(xAxis.data());
    ratio->GetXaxis()->SetTitleSize(0.15);
    ratio->GetXaxis()->SetTitleFont(62);
    ratio->GetXaxis()->SetTitleOffset(0.85);
    ratio->GetXaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    ratio->GetXaxis()->SetLabelSize(0.13);

    theCanvas->cd();

}

void RatioPlotFromFileVariation(TVirtualPad *theCanvas, std::string referenceFileName, std::string referenceHistogramName, std::string inputFileName, std::string inputHistogramName, std::string inputHistogramUpName, std::string inputHistogramDownName, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "") 
{
    TFile referenceFile(referenceFileName.data());
    TH1D *referenceHistogram = (TH1D*)referenceFile.Get(referenceHistogramName.data());
    if(referenceHistogram == NULL)
    {
        std::cerr<<"referenceHistogram " << referenceHistogramName << " does not exist\n";
        return;
    }
    referenceHistogram->SetDirectory(0);
    referenceFile.Close();

    TFile inputFile(inputFileName.data());
    TH1D* inputHistogram = (TH1D*)inputFile.Get(inputHistogramName.data());
    if(inputHistogram == NULL)
    {
        std::cerr<<"inputHistogram " << inputHistogramName << " does not exist\n";
        return;
    }
    inputHistogram->SetDirectory(0);
    TH1D* inputHistogramUp = (TH1D*)inputFile.Get(inputHistogramUpName.data());
    if(inputHistogramUp == NULL)
    {
        std::cerr<<"inputHistogramUp " << inputHistogramUpName << " does not exist\n";
        return;
    }
    inputHistogramUp->SetDirectory(0);
    TH1D* inputHistogramDown = (TH1D*)inputFile.Get(inputHistogramDownName.data());
    if(inputHistogramDown == NULL)
    {
        std::cerr<<"inputHistogramDown " << inputHistogramDownName << " does not exist\n";
        return;
    }
    inputHistogramDown->SetDirectory(0);
    
    inputFile.Close();

    RatioPlot(theCanvas, inputHistogram, inputHistogramUp, inputHistogramDown, referenceHistogram, xMin, xMax, rebinNumber, xAxis, yAxis, "");
    
    return;
}

void RatioTriggerClosure(bool applyTurnOnCut)
{
    std::string datasetAppend   = applyTurnOnCut ? "_turnOnCut"       : "_noTurnOnCut"        ;
    std::string variationAppend = applyTurnOnCut ? "_HLT_SimulatedMc" : "_triggerMcEfficiency";

    gROOT->SetBatch();

    auto makeTriggerVariationNames = [&datasetAppend, &variationAppend](std::string dataset, std::string variableName, std::string selection) -> std::vector<string>
    {
        if(dataset == "singleMuon") variationAppend = "_triggerDataEfficiency";
        return 
        {
            "/" + dataset + "NotTriggered" + datasetAppend + "/PtAndEtaBaseCuts_" + selection + "/" + dataset + "NotTriggered" + datasetAppend + "_PtAndEtaBaseCuts_" + selection + "_" + variableName                           ,
            "/" + dataset + "NotTriggered" + datasetAppend + "/PtAndEtaBaseCuts_" + selection + "/" + dataset + "NotTriggered" + datasetAppend + "_PtAndEtaBaseCuts_" + selection + "_" + variableName + variationAppend + "Up"  ,
            "/" + dataset + "NotTriggered" + datasetAppend + "/PtAndEtaBaseCuts_" + selection + "/" + dataset + "NotTriggered" + datasetAppend + "_PtAndEtaBaseCuts_" + selection + "_" + variableName + variationAppend + "Down"
        };
        // "/ttBarNotTriggered/notTriggeredSelectionScaledUp_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaledUp_" + selection + "_" + variableName                             ,
        // "/ttBarNotTriggered/notTriggeredSelectionScaledDown_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaledDown_" + selection + "_" + variableName                         };
        // "/ttBarNotTriggered/notTriggeredSelectionScaled_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaled_" + selection + "_" + variableName + "_triggerScaleFactorUp"  ,
        // "/ttBarNotTriggered/notTriggeredSelectionScaled_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaled_" + selection + "_" + variableName + "_triggerScaleFactorDown"};
    };
    
    std::vector<std::string> inputFileNameList  {"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root"};
    
    // TCanvas *theCanvasTriggerClosureTTbar1 = new TCanvas(("TriggerClosureTTbar1" + datasetAppend).data(), ("TriggerClosureTTbar1" + datasetAppend).data(), 1400, 800);
    // theCanvasTriggerClosureTTbar1->DivideSquare(8,0.005,0.005);
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_FirstJetPt"      , inputFileNameList, makeTriggerVariationNames("FirstJetPt"      , "Full"), histogramColorList, false, -1, 0.   , 600., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_SecondJetPt"     , inputFileNameList, makeTriggerVariationNames("SecondJetPt"     , "Full"), histogramColorList, false, -1, 0.   , 400., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_ThirdJetPt"      , inputFileNameList, makeTriggerVariationNames("ThirdJetPt"      , "Full"), histogramColorList, false, -1, 0.   , 250., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_FourthJetPt"     , inputFileNameList, makeTriggerVariationNames("FourthJetPt"     , "Full"), histogramColorList, false, -1, 0.   , 200., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_FirstJetEta"     , inputFileNameList, makeTriggerVariationNames("FirstJetEta"     , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_SecondJetEta"    , inputFileNameList, makeTriggerVariationNames("SecondJetEta"    , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_ThirdJetEta"     , inputFileNameList, makeTriggerVariationNames("ThirdJetEta"     , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_FourthJetEta"    , inputFileNameList, makeTriggerVariationNames("FourthJetEta"    , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // theCanvasTriggerClosureTTbar1->SaveAs((std::string(theCanvasTriggerClosureTTbar1->GetName()) + ".png").data());
  
    // TCanvas *theCanvasTriggerClosureTTbar2 = new TCanvas(("TriggerClosureTTbar2" + datasetAppend).data(), ("TriggerClosureTTbar2" + datasetAppend).data(), 1400, 800);
    // theCanvasTriggerClosureTTbar2->DivideSquare(8,0.005,0.005);
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_FirstJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("FirstJetDeepCSV" , "Full"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_SecondJetDeepCSV", inputFileNameList, makeTriggerVariationNames("SecondJetDeepCSV", "Full"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_ThirdJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("ThirdJetDeepCSV" , "Full"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_FourthJetDeepCSV", inputFileNameList, makeTriggerVariationNames("FourthJetDeepCSV", "Full"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_H1_m"            , inputFileNameList, makeTriggerVariationNames("H1_m"            , "Full"), histogramColorList, false, -1, 0.   , 300. , 1, "m [GeV]"    , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_H2_m"            , inputFileNameList, makeTriggerVariationNames("H2_m"            , "Full"), histogramColorList, false, -1, 0.   , 800. , 1, "m [GeV]"    , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_Full/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_Full_HH_m"            , inputFileNameList, makeTriggerVariationNames("HH_m"            , "Full"), histogramColorList, false, -1, 200. , 1200., 1, "m [GeV]"    , "entries [a.u.]");
    // theCanvasTriggerClosureTTbar2->SaveAs((std::string(theCanvasTriggerClosureTTbar2->GetName()) + ".png").data());

    
    // TCanvas *theCanvasTriggerClosureTTbarSignalRegion1 = new TCanvas(("TriggerClosureTTbarSignalRegion1" + datasetAppend).data(), ("TriggerClosureTTbarSignalRegion1" + datasetAppend).data(), 1400, 800);
    // theCanvasTriggerClosureTTbarSignalRegion1->DivideSquare(8,0.005,0.005);
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FirstJetPt"      , inputFileNameList, makeTriggerVariationNames("FirstJetPt"      , "SignalRegion"), histogramColorList, false, -1, 0.   , 600., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_SecondJetPt"     , inputFileNameList, makeTriggerVariationNames("SecondJetPt"     , "SignalRegion"), histogramColorList, false, -1, 0.   , 400., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_ThirdJetPt"      , inputFileNameList, makeTriggerVariationNames("ThirdJetPt"      , "SignalRegion"), histogramColorList, false, -1, 0.   , 250., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FourthJetPt"     , inputFileNameList, makeTriggerVariationNames("FourthJetPt"     , "SignalRegion"), histogramColorList, false, -1, 0.   , 200., 1, "p_{T} [GeV]", "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FirstJetEta"     , inputFileNameList, makeTriggerVariationNames("FirstJetEta"     , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_SecondJetEta"    , inputFileNameList, makeTriggerVariationNames("SecondJetEta"    , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_ThirdJetEta"     , inputFileNameList, makeTriggerVariationNames("ThirdJetEta"     , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FourthJetEta"    , inputFileNameList, makeTriggerVariationNames("FourthJetEta"    , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    // theCanvasTriggerClosureTTbarSignalRegion1->SaveAs((std::string(theCanvasTriggerClosureTTbarSignalRegion1->GetName()) + ".png").data());
  
    TCanvas *theCanvasTriggerClosureTTbarSignalRegion2 = new TCanvas(("TriggerClosureTTbarSignalRegion2" + datasetAppend).data(), ("TriggerClosureTTbarSignalRegion2" + datasetAppend).data(), 1400, 1000);
    theCanvasTriggerClosureTTbarSignalRegion2->DivideSquare(6,0.005,0.005);
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FirstJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("FirstJetDeepCSV" , "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_SecondJetDeepCSV", inputFileNameList, makeTriggerVariationNames("SecondJetDeepCSV", "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_ThirdJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("ThirdJetDeepCSV" , "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FourthJetDeepCSV", inputFileNameList, makeTriggerVariationNames("FourthJetDeepCSV", "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
// void RatioPlotFromFileVariation(TVirtualPad *theCanvas, std::string referenceFileName, std::string referenceHistogramName, std::string inputFileName, std::string inputHistogramName, std::string inputHistogramUpName, std::string inputHistogramDownName, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "", std::string referencePlotName = "", std::string inputPlotName = "", bool normByBin = false) 
    RatioPlotFromFileVariation(theCanvasTriggerClosureTTbarSignalRegion2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_H1_m"            , "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", makeTriggerVariationNames("ttBar", "H1_m"            , "SignalRegion")[0],makeTriggerVariationNames("ttBar", "H1_m"            , "SignalRegion")[1],makeTriggerVariationNames("ttBar", "H1_m"            , "SignalRegion")[2], 90.  , 160. , 4, "m_{Hreco} [GeV]"    , "entries [a.u.]");
    RatioPlotFromFileVariation(theCanvasTriggerClosureTTbarSignalRegion2->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_H2_m"            , "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", makeTriggerVariationNames("ttBar", "H2_m"            , "SignalRegion")[0],makeTriggerVariationNames("ttBar", "H2_m"            , "SignalRegion")[1],makeTriggerVariationNames("ttBar", "H2_m"            , "SignalRegion")[2], 0.   , 800. , 4, "m_{Yreco} [GeV]"    , "entries [a.u.]");
    RatioPlotFromFileVariation(theCanvasTriggerClosureTTbarSignalRegion2->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "ttBarTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/ttBarTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_HH_m"            , "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", makeTriggerVariationNames("ttBar", "HH_m"            , "SignalRegion")[0],makeTriggerVariationNames("ttBar", "HH_m"            , "SignalRegion")[1],makeTriggerVariationNames("ttBar", "HH_m"            , "SignalRegion")[2], 200. , 1200., 4, "m_{Xreco} [GeV]"    , "entries [a.u.]");
    theCanvasTriggerClosureTTbarSignalRegion2->SaveAs((std::string(theCanvasTriggerClosureTTbarSignalRegion2->GetName()) + ".png").data());

 
    TCanvas *theCanvasTriggerClosureSingleMuonSignalRegion2 = new TCanvas(("TriggerClosureSingleMuonSignalRegion2" + datasetAppend).data(), ("TriggerClosureSingleMuonSignalRegion2" + datasetAppend).data(), 1400, 1000);
    theCanvasTriggerClosureSingleMuonSignalRegion2->DivideSquare(6,0.005,0.005);
    // RatioPlotFromFile(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "SingleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/SingleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FirstJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("FirstJetDeepCSV" , "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "SingleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/SingleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_SecondJetDeepCSV", inputFileNameList, makeTriggerVariationNames("SecondJetDeepCSV", "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "SingleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/SingleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_ThirdJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("ThirdJetDeepCSV" , "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
    // RatioPlotFromFile(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "SingleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/SingleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_FourthJetDeepCSV", inputFileNameList, makeTriggerVariationNames("FourthJetDeepCSV", "SignalRegion"), histogramColorList, false, -1, 0.   , 1.   , 1, "deepFlav"   , "entries [a.u.]");
// void RatioPlotFromFileVariation(TVirtualPad *theCanvas, std::string referenceFileName, std::string referenceHistogramName, std::string inputFileName, std::string inputHistogramName, std::string inputHistogramUpName, std::string inputHistogramDownName, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "", std::string referencePlotName = "", std::string inputPlotName = "", bool normByBin = false) 
    RatioPlotFromFileVariation(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "singleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/singleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_H1_m"            , "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", makeTriggerVariationNames("singleMuon", "H1_m"            , "SignalRegion")[0],makeTriggerVariationNames("singleMuon", "H1_m"            , "SignalRegion")[1],makeTriggerVariationNames("singleMuon", "H1_m"            , "SignalRegion")[2], 90.  , 160. , 4, "m_{Hreco} [GeV]"    , "entries [a.u.]");
    RatioPlotFromFileVariation(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "singleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/singleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_H2_m"            , "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", makeTriggerVariationNames("singleMuon", "H2_m"            , "SignalRegion")[0],makeTriggerVariationNames("singleMuon", "H2_m"            , "SignalRegion")[1],makeTriggerVariationNames("singleMuon", "H2_m"            , "SignalRegion")[2], 0.   , 800. , 4, "m_{Yreco} [GeV]"    , "entries [a.u.]");
    RatioPlotFromFileVariation(theCanvasTriggerClosureSingleMuonSignalRegion2->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", "singleMuonTriggered" + datasetAppend + "/PtAndEtaBaseCuts_SignalRegion/singleMuonTriggered" + datasetAppend + "_PtAndEtaBaseCuts_SignalRegion_HH_m"            , "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_withSingleMuon/outPlotter.root", makeTriggerVariationNames("singleMuon", "HH_m"            , "SignalRegion")[0],makeTriggerVariationNames("singleMuon", "HH_m"            , "SignalRegion")[1],makeTriggerVariationNames("singleMuon", "HH_m"            , "SignalRegion")[2], 200. , 1200., 4, "m_{Xreco} [GeV]"    , "entries [a.u.]");
    theCanvasTriggerClosureSingleMuonSignalRegion2->SaveAs((std::string(theCanvasTriggerClosureSingleMuonSignalRegion2->GetName()) + ".png").data());

    gROOT->SetBatch(false);

}


void RatioAllTriggerClosure()
{
    // RatioTriggerClosure(true );
    RatioTriggerClosure(false);

}