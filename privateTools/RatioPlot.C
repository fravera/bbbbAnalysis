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

void normalizeByBinSize(TH1D* inputPlot)
{
    for(int nBin = 1; nBin<=inputPlot->GetNbinsX(); ++nBin)
    {
        float binWidth = inputPlot->GetXaxis()->GetBinWidth(nBin);
        inputPlot->SetBinContent(nBin,inputPlot->GetBinContent(nBin)/binWidth);
        inputPlot->SetBinError(nBin,inputPlot->GetBinError(nBin)/binWidth);
    }
    return;

}

template<typename Hist>
Hist* getHistogramFromFile(TFile& inputFile, std::string histogramName)
{
    Hist* histogram = (Hist*)inputFile.Get(histogramName.data());
    if(histogram == nullptr)
    {
        std::cout<< "Histogram " << histogramName << " does not exist" << std::endl;
        exit(EXIT_FAILURE);
    }
    histogram->SetDirectory(0);

    return histogram;
}

std::tuple<TH1D*, TH1D*> dividePlots(const TH1D* referencePlot, const TH1D* inputPlot)
{
    std::string ratioPlotName = std::string(inputPlot->GetName()) + "_ratio";
    std::string errorPlotName = ratioPlotName + "Error";
    int numberOfBins = inputPlot->GetNbinsX();
    TH1D* ratioPlot  = (TH1D*)inputPlot->Clone(ratioPlotName.data());
    TH1D* ratioError = (TH1D*)inputPlot->Clone(errorPlotName.data());
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

    return {ratioPlot,ratioError};
}

std::tuple<TH1D*, TH1D*> splitErrorAndPlots(const TH1D* inputPlot)
{
    std::string valuePlotName = std::string(inputPlot->GetName()) + "_value";
    std::string errorPlotName = valuePlotName + "Error";
    int numberOfBins = inputPlot->GetNbinsX();
    TH1D* outputPlot  = (TH1D*)inputPlot->Clone(valuePlotName.data());
    TH1D* outputError = (TH1D*)inputPlot->Clone(errorPlotName.data());
    for(int nBin = 1; nBin<=numberOfBins; ++nBin)
    {
        outputPlot->SetBinError  (nBin,0.);
    }

    outputError->SetFillStyle(3002);
    outputError->SetFillColor(kBlack);

    return {outputPlot,outputError};
}


void RatioPlot(TVirtualPad *theCanvas, TH1D *referenceHistogram, std::vector<TH1D*> inputHistogramVector, std::vector<EColor> plotColorVector, bool normalize, float& normalizeValue, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "", std::string title = "", std::string referencePlotName = "", std::vector<std::string> inputPlotNameVector = std::vector<std::string>(), bool normByBin = false)
{

    // std::cout<<normalize<<std::endl;
    // std::cout<<normalizeValue<<std::endl;

    if(inputPlotNameVector.size() == 0) inputPlotNameVector = std::vector<std::string>(plotColorVector.size(),"");
    assert(inputHistogramVector.size() == plotColorVector.size() == inputPlotNameVector.size());
    
    if(rebinNumber!=1)
    {
        referenceHistogram->Rebin(rebinNumber);
        for(auto inputHistogram : inputHistogramVector) inputHistogram->Rebin(rebinNumber);
    }
    referenceHistogram->SetAxisRange(xMin,xMax);
    for(auto inputHistogram : inputHistogramVector) inputHistogram->SetAxisRange(xMin,xMax);
    if(normalize){
        for(auto inputHistogram : inputHistogramVector) 
        {
            if(normalizeValue < 0.) 
                normalizeValue = float(referenceHistogram->Integral(-1,999999999))/float(inputHistogram->Integral(-1,999999999));
            std::cout<<normalizeValue<<std::endl;
            inputHistogram->Scale(normalizeValue);
        }
    }
    auto theRatioPlots = dividePlots(referenceHistogram, inputHistogramVector.at(0));
    if(normByBin)
    {
        normalizeByBinSize(referenceHistogram);
        for(auto inputHistogram : inputHistogramVector) 
        {
            normalizeByBinSize(inputHistogram);
        }
    }
    auto theLegend = new TLegend(0.3,0.75,0.88,0.88);
    theLegend->SetNColumns(3);
    theLegend->SetTextSize(0.05);


    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.35, 1, 1.0);
    pad1->SetLeftMargin(0.12);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    auto referenceHistAndError = splitErrorAndPlots(referenceHistogram);
    std::get<0>(referenceHistAndError)->Draw("hist");         // Draw referenceHistogram on top of inputHistogram
    theLegend->AddEntry(std::get<0>(referenceHistAndError),referencePlotName.data(), "pl");
    theLegend->AddEntry(std::get<1>(referenceHistAndError),(referencePlotName + " unc.").data(), "f");
    // Y axis inputHistogram plot settings
    std::get<0>(referenceHistAndError)->SetStats(0);          // No statistics on upper plot
    std::get<0>(referenceHistAndError)->GetYaxis()->SetTitleSize(0.07);
    std::get<0>(referenceHistAndError)->GetYaxis()->SetTitleFont(62);
    std::get<0>(referenceHistAndError)->GetYaxis()->SetTitleOffset(0.95);
    std::get<0>(referenceHistAndError)->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    std::get<0>(referenceHistAndError)->GetYaxis()->SetLabelSize(0.06);
    std::get<0>(referenceHistAndError)->GetYaxis()->SetTitle(yAxis.data());
    std::get<0>(referenceHistAndError)->SetTitle(title.data());
    // std::get<0>(referenceHistAndError) settings
    std::get<0>(referenceHistAndError)->SetLineColor(kBlack);
    std::get<0>(referenceHistAndError)->SetMarkerColor(kBlack);
    std::get<0>(referenceHistAndError)->SetLineWidth(1);
    std::get<1>(referenceHistAndError)->Draw("same E2");         // Draw referenceHistogram on top of inputHistogram
    float yMaximum = std::get<0>(referenceHistAndError)->GetMaximum();


    for(uint hIt = 0; hIt <inputHistogramVector.size(); ++hIt)
    {
        TH1D* inputHistogram = inputHistogramVector.at(hIt);
        theLegend->AddEntry(inputHistogram,inputPlotNameVector[hIt].data(), "epl");
        inputHistogram->SetStats(0);          // No statistics on upper plot
        inputHistogram->SetTitle(title.data()); // Remove the ratio title
        inputHistogram->Draw("E same");               // Draw inputHistogram
        // inputHistogram settings
        assert(plotColorVector.at(hIt) != kBlack);
        inputHistogram->SetLineColor(plotColorVector.at(hIt));
        inputHistogram->SetMarkerColor(plotColorVector.at(hIt));
        inputHistogram->SetLineWidth(2);
        // if(yMaximum < inputHistogram->GetMaximum()) yMaximum = inputHistogram->GetMaximum();
    }
    if(std::string(inputHistogramVector.at(0)->GetName()).find("selectionbJets_ControlRegionAndSignalRegion") != std::string::npos) yMaximum = 18000;
    std::get<0>(referenceHistAndError)->SetMaximum(yMaximum * 1.3);
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

        // Define the ratio plot
    for(uint hIt = 0; hIt <inputHistogramVector.size(); ++hIt)
    {

        TH1D *ratio = std::get<0>(theRatioPlots);
        TH1D *ratioError = std::get<1>(theRatioPlots);
        // TH1D *ratio = (TH1D*)inputHistogramVector.at(hIt)->Clone("ratio");
        ratio->SetLineColor(plotColorVector.at(hIt));
        ratio->SetMarkerColor(plotColorVector.at(hIt));
        ratio->SetMinimum(0.5);  // Define Y ..
        ratio->SetMaximum(1.5); // .. range
        ratio->SetStats(0);      // No statistics on lower plot
        // ratio->Divide(referenceHistogram);
        ratio->SetMarkerStyle(21);
        ratio->SetMarkerSize(0.3);

        // Ratio plot (ratio) settings
        ratio->SetTitle(""); // Remove the ratio title

        if(hIt==0)
        {
            ratio->SetAxisRange(xMin,xMax);
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
        }
        else ratio->Draw("ep same");       // Draw the ratio plot
    }

    theCanvas->cd();

}

void RatioPlotFromFile(TVirtualPad *theCanvas, std::string referenceFileName, std::string referenceHistogramName, std::vector<std::string> inputFileNameVector, std::vector<std::string> inputHistogramNameVector, std::vector<EColor> plotColorVector, bool normalize, float& normalizeValue, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "", std::string referencePlotName = "", std::vector<std::string> inputPlotNameVector = std::vector<std::string>(), bool normByBin = false) 
{
    assert(inputFileNameVector == inputHistogramNameVector == plotColorVector);
    TFile referenceFile(referenceFileName.data());
    TH1D *referenceHistogram = (TH1D*)referenceFile.Get(referenceHistogramName.data());
    if(referenceHistogram == NULL)
    {
        std::cerr<<"referenceHistogram " << referenceHistogramName << " does not exist\n";
        return;
    }
    referenceHistogram->SetDirectory(0);
    referenceFile.Close();

    std::vector<TH1D*> inputHistogramVector;
    for(uint plotIt=0; plotIt<inputFileNameVector.size(); ++plotIt)
    {
        TFile inputFile(inputFileNameVector[plotIt].data());
        TH1D *inputHistogram = (TH1D*)inputFile.Get(inputHistogramNameVector[plotIt].data());
        if(inputHistogram == NULL)
        {
            std::cerr<<"inputHistogram " << inputHistogramNameVector[plotIt] << " does not exist\n";
            return;
        }
        inputHistogram->SetDirectory(0);
        inputHistogramVector.emplace_back(inputHistogram);
        inputFile.Close();
    }
    
    RatioPlot(theCanvas, referenceHistogram, inputHistogramVector, plotColorVector,  normalize, normalizeValue, xMin, xMax, rebinNumber, xAxis, yAxis, "", referencePlotName, inputPlotNameVector, normByBin);
    
    return;
}

void RatioAllMC()
{
    gROOT->SetBatch();
    float normalizationValue = -1.;

    TCanvas *theCanvasControlRegionPlusBeforeBDT = new TCanvas("ControlRegionPlusBeforeBDT", "ControlRegionPlusBeforeBDT", 1400, 800);
    theCanvasControlRegionPlusBeforeBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      }, {kRed}, true , normalizationValue,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      }, {kRed}, true , normalizationValue,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     }, {kRed}, true , normalizationValue, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     }, {kRed}, true , normalizationValue, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, true , normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, true , normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       }, {kRed}, true , normalizationValue,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       }, {kRed}, true , normalizationValue,  300 , 1000, 4, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, true , normalizationValue,  8900, 9600, 3, "m_{X} [GeV]"     );
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, normalizationValue,  8900, 9600, 1, "m_{X} [GeV]"     );
    theCanvasControlRegionPlusBeforeBDT->SaveAs((std::string(theCanvasControlRegionPlusBeforeBDT->GetName()) + ".png").data());

    TCanvas *theCanvasSignalRegionPlusBeforeBDT = new TCanvas("SignalRegionPlusBeforeBDT", "SignalRegionPlusBeforeBDT", 1400, 800);
    theCanvasSignalRegionPlusBeforeBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, true , normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, true , normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, true , normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, true , normalizationValue,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      }, {kRed}, true , normalizationValue,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      }, {kRed}, true , normalizationValue,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     }, {kRed}, true , normalizationValue, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     }, {kRed}, true , normalizationValue, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, true , normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, true , normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       }, {kRed}, true , normalizationValue,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       }, {kRed}, true , normalizationValue,  300 , 1000, 1, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, true , normalizationValue,  8900, 9600, 3, "m_{X} [GeV]"     );
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, normalizationValue,  8900, 9600, 1, "m_{X} [GeV]"     );
    theCanvasSignalRegionPlusBeforeBDT->SaveAs((std::string(theCanvasSignalRegionPlusBeforeBDT->GetName()) + ".png").data());



    TCanvas *theCanvasControlRegionPlusAfterBDT = new TCanvas("ControlRegionPlusAfterBDT", "ControlRegionPlusAfterBDT", 1400, 800);
    theCanvasControlRegionPlusAfterBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      }, {kRed}, false, normalizationValue,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      }, {kRed}, false, normalizationValue,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     }, {kRed}, false, normalizationValue, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     }, {kRed}, false, normalizationValue, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, false, normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, false, normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       }, {kRed}, false, normalizationValue,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       }, {kRed}, false, normalizationValue,  300 , 1000, 4, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, normalizationValue,  8900, 9600, 3, "m_{X} [GeV]"     );
    theCanvasControlRegionPlusAfterBDT->SaveAs((std::string(theCanvasControlRegionPlusAfterBDT->GetName()) + ".png").data());

    TCanvas *theCanvasSignalRegionPlusAfterBDT = new TCanvas("SignalRegionPlusAfterBDT", "SignalRegionPlusAfterBDT", 1400, 800);
    theCanvasSignalRegionPlusAfterBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, false, normalizationValue,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      }, {kRed}, false, normalizationValue,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      }, {kRed}, false, normalizationValue,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     }, {kRed}, false, normalizationValue, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     }, {kRed}, false, normalizationValue, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, false, normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, false, normalizationValue,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       }, {kRed}, false, normalizationValue,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       }, {kRed}, false, normalizationValue,  300 , 1000, 1, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, normalizationValue,  8900, 9600, 3, "m_{X} [GeV]"     );
    theCanvasSignalRegionPlusAfterBDT->SaveAs((std::string(theCanvasSignalRegionPlusAfterBDT->GetName()) + ".png").data());

    return;
}


void RatioSlices(std::string canvasName, std::string referenceFileName, std::string referenceHistogramName, std::string inputFileName, std::string inputHistogramName, bool normalize = false, float normalizeValue = -1, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "events/GeV") 
{

    TFile referenceFile(referenceFileName.data());
    TH2F *referenceHistogram2D = (TH2F*)referenceFile.Get(referenceHistogramName.data());
    if(referenceHistogram2D == NULL)
    {
        std::cerr<<"referenceHistogram2D does not exist\n";
        return;
    }
    referenceHistogram2D->SetDirectory(0);

    TFile inputFile(inputFileName.data());
    TH2F *inputHistogram2D = (TH2F*)inputFile.Get(inputHistogramName.data());
    if(inputHistogram2D == NULL)
    {
        std::cerr<<"inputHistogram2D does not exist\n";
        return;
    }
    inputHistogram2D->SetDirectory(0);

    referenceFile.Close();
    inputFile.Close();
    
    uint numberOfYbins = referenceHistogram2D->GetNbinsY();
    uint nPadPerCanvas = 4;
    uint rebinFactor = 10;

    std::cout << "numberOfYbins = " << numberOfYbins << std::endl;
    std::cout << "nPadPerCanvas = " << nPadPerCanvas << std::endl;
    std::cout << "rebinFactor   = " << rebinFactor   << std::endl;

    for(uint i=0; i <= numberOfYbins/(nPadPerCanvas*rebinFactor); ++i)
    { 
        std::string tmpCanvasName = canvasName + "_MX_projections_" + to_string(i);
        TCanvas *theCanvas = new TCanvas(tmpCanvasName.data(), tmpCanvasName.data(), 1400, 800);
        theCanvas->DivideSquare(nPadPerCanvas,0.005,0.005);
        
        for(uint y = 1; y<=nPadPerCanvas; ++y)
        {
            uint yBinMin = (y-1)*rebinFactor +1 + i*rebinFactor*nPadPerCanvas;
            if(yBinMin>=numberOfYbins) break;
            uint yBinMax = yBinMin+rebinFactor-1;
            // std::cout<<yBinMin<<" - "<<yBinMax<<std::endl;
            float mYmin = referenceHistogram2D->GetYaxis()->GetBinLowEdge(yBinMin);
            float mYmax = referenceHistogram2D->GetYaxis()->GetBinUpEdge (yBinMax);
            TH1D* referenceHistogram = referenceHistogram2D->ProjectionX(Form("reference_Mx_projection_My_%.1f_%.1f",mYmin, mYmax), yBinMin, yBinMax);
            TH1D* inputHistogram     = inputHistogram2D    ->ProjectionX(Form("input_Mx_projection_My_%.1f_%.1f"    ,mYmin, mYmax), yBinMin, yBinMax);
            // if(inputHistogram->GetEntries()==0) continue;
            int correctedRebinNumber = rebinNumber;
            if(mYmin<125. && mYmax>125.) correctedRebinNumber*=3;
            RatioPlot(theCanvas->cd(y), referenceHistogram, {inputHistogram}, {kRed}, normalize, normalizeValue, xMin, xMax, correctedRebinNumber, xAxis, yAxis, Form("%.1f < M_{Yreco} < %.1f", mYmin, mYmax),"4b-tag",{"BKG model"}, true);
        }

        theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());
        delete theCanvas;

        // if( (i+1)*nPadPerCanvas >= numberOfYbins) break;

    }

}


void RatioAllVariables(std::string canvasName, std::string referenceFileName, std::string referenceDatasetName , std::string referenceCutName, std::string targetFileName, std::string targetDatasetName , std::string targetCutName, bool normalize, std::string region = "HMR", bool useKinFitVariables = false)
{

    float minH2_m;
    float maxH2_m;
    int   rebinH2_m;
    // float minUnroll_m;
    // float maxUnroll_m;
    // int   rebinUnroll_m;
    if(region == "LMR")
    {
        minH2_m = 0;
        maxH2_m = 150;
        rebinH2_m = 2;
        // minUnroll_m = 0.;
        // maxUnroll_m = 3000.;
        // rebinUnroll_m = 9.;
    }
    else if(region == "HMR")
    {
        minH2_m = 100;
        maxH2_m = 2400;
        rebinH2_m = 2;
        // minUnroll_m = 110000.;
        // maxUnroll_m = 250000.;
        // rebinUnroll_m = 9.;
    }
    else if(region == "Full")
    {
        minH2_m = 30;
        maxH2_m = 1800;
        rebinH2_m = 4;
        // minUnroll_m = 110000.;
        // maxUnroll_m = 250000.;
        // rebinUnroll_m = 9.;
    }
    else
    {
        std::cout<<"Region not specified"<<std::endl;
        return;
    }
    std::string append = useKinFitVariables ? "_kinFit" : "";

    std::string legEntry;
    if(canvasName.find("BeforeBDT") == std::string::npos) legEntry = "BKG model";
    else legEntry = "3b-tag scaled";
    float scaleValue = -1;

    std::string referenceHistPrototype = referenceDatasetName +  "/" + referenceCutName + "/" + referenceDatasetName +  "_" + referenceCutName;
    std::string targetHistPrototype    = targetDatasetName    +  "/" + targetCutName    + "/" + targetDatasetName    +  "_" + targetCutName   ;
    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 800);
    theCanvas->DivideSquare(6,0.005,0.005);
    RatioPlotFromFile(theCanvas->cd(1),referenceFileName ,referenceHistPrototype + "_H1" + append + "_pt"                       , {targetFileName} , {targetHistPrototype + "_H1" + append + "_pt"                      }, {kRed} , normalize, scaleValue,     0,   600, 4, "pT_{H" + append + "reco} [GeV]","events","4b-tag",{legEntry});
    scaleValue = -1;
    RatioPlotFromFile(theCanvas->cd(4),referenceFileName ,referenceHistPrototype + "_H2_pt"                       , {targetFileName} , {targetHistPrototype + "_H2_pt"                      }, {kRed} , normalize, scaleValue,     0,   800, 4, "pT_{Yreco} [GeV]","events","4b-tag",{legEntry});
    scaleValue = -1;
    RatioPlotFromFile(theCanvas->cd(2),referenceFileName ,referenceHistPrototype + "_H1" + append + "_eta"                      , {targetFileName} , {targetHistPrototype + "_H1" + append + "_eta"                     }, {kRed} , normalize, scaleValue,    -4,     4, 2, "#eta_{H" + append + "reco}","events","4b-tag",{legEntry});
    scaleValue = -1;
    RatioPlotFromFile(theCanvas->cd(5),referenceFileName ,referenceHistPrototype + "_H2_eta"                      , {targetFileName} , {targetHistPrototype + "_H2_eta"                     }, {kRed} , normalize, scaleValue,    -4,     4, 2, "#eta_{Yreco}","events","4b-tag",{legEntry});
    scaleValue = -1;
    // RatioPlotFromFile(theCanvas->cd(5),referenceFileName ,referenceHistPrototype + "_H1_bb_DeltaR"                , {targetFileName} , {targetHistPrototype + "_H1_bb_DeltaR"               }, {kRed} , normalize, scaleValue,     0,     5, 1, "#DeltaR_{bb(H)}","events","4b-tag",{legEntry});
    // scaleValue = -1;
    // RatioPlotFromFile(theCanvas->cd(6),referenceFileName ,referenceHistPrototype + "_H2_bb_DeltaR"                , {targetFileName} , {targetHistPrototype + "_H2_bb_DeltaR"               }, {kRed} , normalize, scaleValue,     0,     5, 1, "#DeltaR_{bb(Y)}","events","4b-tag",{legEntry});
    // scaleValue = -1;
    RatioPlotFromFile(theCanvas->cd(3),referenceFileName ,referenceHistPrototype + "_H2_m"                        , {targetFileName} , {targetHistPrototype + "_H2_m"                       }, {kRed} , normalize, scaleValue, minH2_m, maxH2_m, rebinH2_m, "m_{Yreco} [GeV]","events/GeV","4b-tag",{legEntry},true);
    scaleValue = -1;
    RatioPlotFromFile(theCanvas->cd(6),referenceFileName ,referenceHistPrototype + "_HH" + append + "_m"                        , {targetFileName} , {targetHistPrototype + "_HH" + append + "_m"                       }, {kRed} , normalize, scaleValue,   250,  2000, 4, "m" + append + "_{Xreco} [GeV]","events/GeV","4b-tag",{legEntry},true);
    // RatioPlotFromFile(theCanvas->cd(9),referenceFileName ,referenceHistPrototype + "_HH_m_H2_m_Rebinned_Unrolled" , {targetFileName} , {targetHistPrototype + "_HH_m_H2_m_Rebinned_Unrolled"}, {kRed} , normalize, scaleValue, minUnroll_m, maxUnroll_m, rebinUnroll_m, "m_{X}*m_{Y}");
    // scaleValue = -1;
    // RatioPlotFromFile(theCanvas->cd(3),referenceFileName ,referenceDatasetName +  "/" + "selectionbJets_ControlRegionBlinded" + "/" + referenceDatasetName +  "_" + "selectionbJets_ControlRegionBlinded" + "_H1_m"                        , {targetFileName} , {targetDatasetName +  "/" + "selectionbJets_ControlRegionAndSignalRegion" + "/" + targetDatasetName +  "_" + "selectionbJets_ControlRegionAndSignalRegion" + "_H1_m"                       }, {kRed} , normalize, 0.153512,    95,   155, 1, "m_{Hreco}","events","4b-tag",{legEntry});
    // scaleValue = -1;
    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());
    delete theCanvas;

    RatioSlices(canvasName, referenceFileName, referenceHistPrototype + "_HH" + append + "_m_H2_m", targetFileName, targetHistPrototype + "_HH" + append + "_m_H2_m", normalize, scaleValue, 250, 2200, 3, "m" + append + "_{Xreco} [GeV]","events/GeV");
  
}




void RatioBackgroundSculpting(std::string canvasName = "BackgroundSculpting", std::string fileName = "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies_norm/outPlotter.root", std::string referenceDatasetName = "data_BTagCSV" , std::string targetDatasetName = "data_BTagCSV_3btag")
{

    TFile theFile(fileName.data());
    auto *entriesIn4b = getHistogramFromFile<TH1F>(theFile, referenceDatasetName + "/" + referenceDatasetName);
    auto *entriesIn3b = getHistogramFromFile<TH1F>(theFile, targetDatasetName + "/" + targetDatasetName);
    float nEntriesIn3b = entriesIn3b->GetBinContent(entriesIn3b->GetXaxis()->FindBin("selectionbJets_SideBandBlinded"));
    float nEntriesIn4b = entriesIn4b->GetBinContent(entriesIn4b->GetXaxis()->FindBin("selectionbJets_SideBandBlinded"));
    float normalizationValue = nEntriesIn4b / nEntriesIn3b;
    theFile.Close();
    std::cout << nEntriesIn4b << " - " << nEntriesIn3b << " - " << normalizationValue << std::endl;

    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 800);
    theCanvas->DivideSquare(2,0.005,0.005);
    RatioPlotFromFile(theCanvas->cd(1),fileName ,referenceDatasetName +  "/" + "selectionbJets_ControlAndSideBandBlinded" + "/" + referenceDatasetName +  "_" + "selectionbJets_ControlAndSideBandBlinded" + "_H1_m"                        , {fileName} , {targetDatasetName +  "/" + "selectionbJets_FullRegion" + "/" + targetDatasetName +  "_" + "selectionbJets_FullRegion" + "_H1_m"                       }, {kRed} , true, normalizationValue,    65,   185, 1, "m_{Hreco}","events","4b-tag", {std::string("3b-tag scaled")});
    RatioPlotFromFile(theCanvas->cd(2),fileName ,referenceDatasetName +  "/" + "selectionbJets_ControlAndSideBandBlinded" + "/" + referenceDatasetName +  "_" + "selectionbJets_ControlAndSideBandBlinded" + "_H1_m"                        , {fileName} , {"data_BTagCSV_dataDriven/selectionbJets_FullRegion/data_BTagCSV_dataDriven_selectionbJets_FullRegion_H1_m"                       }, {kRed} , false, normalizationValue,    65,   185, 1, "m_{Hreco}","events","4b-tag", {std::string("BKG model")});

}


void RatioBackgroundSculptingOffShell(std::string canvasName = "BackgroundSculpting_offShell", std::string fileName = "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies_offShell/outPlotter.root", std::string referenceDatasetName = "data_BTagCSV_offShell" , std::string targetDatasetName = "data_BTagCSV_3btag_offShell")
{

    TFile theFile(fileName.data());
    auto *entriesIn4b = getHistogramFromFile<TH1F>(theFile, referenceDatasetName + "/" + referenceDatasetName);
    auto *entriesIn3b = getHistogramFromFile<TH1F>(theFile, targetDatasetName + "/" + targetDatasetName);
    float nEntriesIn3b = entriesIn3b->GetBinContent(entriesIn3b->GetXaxis()->FindBin("selectionbJets_offShell_SideBand"));
    float nEntriesIn4b = entriesIn4b->GetBinContent(entriesIn4b->GetXaxis()->FindBin("selectionbJets_offShell_SideBand"));
    float normalizationValue = nEntriesIn4b / nEntriesIn3b;
    theFile.Close();
    std::cout << nEntriesIn4b << " - " << nEntriesIn3b << " - " << normalizationValue << std::endl;

    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 800);
    theCanvas->DivideSquare(2,0.005,0.005);
    RatioPlotFromFile(theCanvas->cd(1),fileName ,referenceDatasetName +  "/" + "selectionbJets_offShell_FullRegion" + "/" + referenceDatasetName +  "_" + "selectionbJets_offShell_FullRegion" + "_offShell_H1_m"                        , {fileName} , {targetDatasetName +  "/" + "selectionbJets_offShell_FullRegion" + "/" + targetDatasetName +  "_" + "selectionbJets_offShell_FullRegion" + "_offShell_H1_m"                       }, {kRed} , true, normalizationValue,    145,   265, 16, "m_{Hreco}","events","4b-tag", {std::string("3b-tag scaled")});
    RatioPlotFromFile(theCanvas->cd(2),fileName ,referenceDatasetName +  "/" + "selectionbJets_offShell_FullRegion" + "/" + referenceDatasetName +  "_" + "selectionbJets_offShell_FullRegion" + "_offShell_H1_m"                        , {fileName} , {"data_BTagCSV_dataDriven_offShell/selectionbJets_offShell_FullRegion/data_BTagCSV_dataDriven_offShell_selectionbJets_offShell_FullRegion_offShell_H1_m"                       }, {kRed} , false, normalizationValue,    145,   265, 16, "m_{Hreco}","events","4b-tag", {std::string("BKG model")});

}


void RatioAll(bool useKinFitVariables=true, std::string dataDrivenDatasetName="data_BTagCSV_dataDriven_kinFit")
{
    gROOT->SetBatch();

    RatioAllVariables("ValidationRegion_AfterBDT_2016", "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ValidationRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", dataDrivenDatasetName , "selectionbJets_ValidationRegionBlinded",false, "Full", useKinFitVariables);

    RatioAllVariables("ValidationRegion_BeforeBDT_2016", "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ValidationRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJets_ValidationRegionBlinded",true, "Full", useKinFitVariables);


    RatioAllVariables("ValidationRegion_AfterBDT_2017", "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ValidationRegionBlinded", 
    "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", dataDrivenDatasetName , "selectionbJets_ValidationRegionBlinded",false, "Full", useKinFitVariables);

    RatioAllVariables("ValidationRegion_BeforeBDT_2017", "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ValidationRegionBlinded", 
    "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJets_ValidationRegionBlinded",true, "Full", useKinFitVariables);


    RatioAllVariables("ValidationRegion_AfterBDT_2018", "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ValidationRegionBlinded", 
    "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", dataDrivenDatasetName , "selectionbJets_ValidationRegionBlinded",false, "Full", useKinFitVariables);

    RatioAllVariables("ValidationRegion_BeforeBDT_2018", "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ValidationRegionBlinded", 
    "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJets_ValidationRegionBlinded",true, "Full", useKinFitVariables);



    RatioAllVariables("ControlRegion_AfterBDT_2016", "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ControlRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", dataDrivenDatasetName , "selectionbJets_ControlRegionBlinded",false, "Full", useKinFitVariables);

    RatioAllVariables("ControlRegion_BeforeBDT_2016", "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ControlRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJets_ControlRegionBlinded",true, "Full", useKinFitVariables);


    RatioAllVariables("ControlRegion_AfterBDT_2017", "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ControlRegionBlinded", 
    "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", dataDrivenDatasetName , "selectionbJets_ControlRegionBlinded",false, "Full", useKinFitVariables);

    RatioAllVariables("ControlRegion_BeforeBDT_2017", "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ControlRegionBlinded", 
    "2017DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJets_ControlRegionBlinded",true, "Full", useKinFitVariables);


    RatioAllVariables("ControlRegion_AfterBDT_2018", "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ControlRegionBlinded", 
    "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", dataDrivenDatasetName , "selectionbJets_ControlRegionBlinded",false, "Full", useKinFitVariables);

    RatioAllVariables("ControlRegion_BeforeBDT_2018", "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV" , "selectionbJets_ControlRegionBlinded", 
    "2018DataPlots_NMSSM_XYH_bbbb_dataDrivenStudies/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJets_ControlRegionBlinded",true, "Full", useKinFitVariables);


    // RatioAllVariables("SideBand_AfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_Full/outPlotter.root", "data_BTagCSV" , "selectionbJets_SideBandBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_Full/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJets_SideBandBlinded",false, "Full");


    // RatioAllVariables("ControlRegion_LMR_AfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV" , "selectionbJetsLMR_ControlRegionBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV_dataDriven_LMR" , "selectionbJetsLMR_ControlRegionBlinded",false, "LMR");

    // RatioAllVariables("ControlRegion_LMR_BeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV" , "selectionbJetsLMR_ControlRegionBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJetsLMR_ControlRegionBlinded",true, "LMR");

    // RatioAllVariables("SideBand_LMR_AfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV" , "selectionbJetsLMR_SideBandBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV_dataDriven_LMR" , "selectionbJetsLMR_SideBandBlinded",false, "LMR");

    // RatioAllVariables("ControlRegion_HMR_AfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV" , "selectionbJetsHMR_ControlRegionBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV_dataDriven_HMR" , "selectionbJetsHMR_ControlRegionBlinded",false, "HMR");

    // RatioAllVariables("ControlRegion_HMR_BeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV" , "selectionbJetsHMR_ControlRegionBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV_3btag" , "selectionbJetsHMR_ControlRegionBlinded",true, "HMR");

    // RatioAllVariables("SideBand_HMR_AfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV" , "selectionbJetsHMR_SideBandBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_all_openClose_copy/outPlotter.root", "data_BTagCSV_dataDriven_HMR" , "selectionbJetsHMR_SideBandBlinded",false, "HMR");

    // RatioAllVariables("ControlRegionBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_ControlRegionBlinded",true);

    // RatioAllVariables("ControlRegionAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_ControlRegionBlinded",false);

    // RatioAllVariables("SideBandBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_SideBandBlinded",true);

    // RatioAllVariables("SideBandAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_SideBandBlinded",false);


    // RatioAllVariables("ControlRegionDownBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionDownBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_ControlRegionDownBlinded",true);

    // RatioAllVariables("ControlRegionDownAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionDownBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_ControlRegionDownBlinded",false);

    // RatioAllVariables("SideBandDownBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandDownBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_SideBandDownBlinded",true);

    // RatioAllVariables("SideBandDownAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandDownBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_SideBandDownBlinded",false);


    // RatioAllVariables("ControlRegionUpBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionUpBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_ControlRegionUpBlinded",true);

    // RatioAllVariables("ControlRegionUpAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionUpBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_ControlRegionUpBlinded",false);

    // RatioAllVariables("SideBandUpBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandUpBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_SideBandUpBlinded",true);

    // RatioAllVariables("SideBandUpAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandUpBlinded", 
    // "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_SideBandUpBlinded",false);
    
    gROOT->SetBatch(false);

}


void RatioAllBackground()
{
    gROOT->SetBatch();

    RatioAllVariables("ControlRegionAfterBDTBackgroundHMR", "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_background" , "selectionbJetsHMR_ControlRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_dataDriven_backgroundHMR" , "selectionbJetsHMR_ControlRegionBlinded",false);

    RatioAllVariables("SideBandAfterBDTBackgroundHMR", "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_background" , "selectionbJetsHMR_SideBandBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_dataDriven_backgroundHMR" , "selectionbJetsHMR_SideBandBlinded",false);

    RatioAllVariables("ControlRegionAfterBDTBackgroundLMR", "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_background" , "selectionbJetsLMR_ControlRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_dataDriven_backgroundLMR" , "selectionbJetsLMR_ControlRegionBlinded",false);

    RatioAllVariables("SideBandAfterBDTBackgroundLMR", "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_background" , "selectionbJetsLMR_SideBandBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_background/outPlotter.root", "data_BTagCSV_dataDriven_backgroundLMR" , "selectionbJetsLMR_SideBandBlinded",false);
   
    gROOT->SetBatch(false);

}



void DivideTH2D()
{

    gStyle->SetPalette(kRainBow);
    TFile theFile("2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root");
    TH2F *extimatedHistogram = (TH2F*)theFile.Get("data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_SignalRegionPlus/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_SignalRegionPlus_HH_m_H2_m");
    extimatedHistogram->SetDirectory(0);
    extimatedHistogram->RebinX(2);
    extimatedHistogram->RebinY(2);
    TH2F *referenceHistogram = (TH2F*)theFile.Get("data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m");
    referenceHistogram->SetDirectory(0);
    referenceHistogram->RebinX(2);
    referenceHistogram->RebinY(2);
    
    extimatedHistogram->Divide(referenceHistogram);

    extimatedHistogram->Draw("colz");
}
