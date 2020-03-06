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

void RatioPlot(TVirtualPad *theCanvas, TH1D *referenceHistogram, std::vector<TH1D*> inputHistogramVector, std::vector<EColor> plotColorVector, bool normalize = false, float normalizeValue = -1, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "", std::string title = "")
{
    assert(inputHistogramVector == plotColorVector);
    
    if(rebinNumber!=1)
    {
        referenceHistogram->Rebin(rebinNumber);
        for(auto inputHistogram : inputHistogramVector) inputHistogram->Rebin(rebinNumber);
    }
    referenceHistogram->SetAxisRange(xMin,xMax);
    for(auto inputHistogram : inputHistogramVector) inputHistogram->SetAxisRange(xMin,xMax);
    if(normalize){
        if(normalizeValue < 0.) 
        for(auto inputHistogram : inputHistogramVector) 
        {
            normalizeValue = float(referenceHistogram->Integral(-1,999999999))/float(inputHistogram->Integral(-1,999999999));
            std::cout<<normalizeValue<<std::endl;
            inputHistogram->Scale(normalizeValue);
        }
    }

    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.35, 1, 1.0);
    pad1->SetLeftMargin(0.12);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    referenceHistogram->Draw("E");         // Draw referenceHistogram on top of inputHistogram
    // Y axis inputHistogram plot settings
    referenceHistogram->SetStats(0);          // No statistics on upper plot
    referenceHistogram->GetYaxis()->SetTitleSize(0.05);
    referenceHistogram->GetYaxis()->SetTitleFont(62);
    referenceHistogram->GetYaxis()->SetTitleOffset(0.9);
    referenceHistogram->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    referenceHistogram->GetYaxis()->SetLabelSize(0.06);
    referenceHistogram->GetYaxis()->SetTitle(yAxis.data()); // Remove the ratio title
    // referenceHistogram settings
    referenceHistogram->SetLineColor(kBlack);
    referenceHistogram->SetLineWidth(2);


    for(uint hIt = 0; hIt <inputHistogramVector.size(); ++hIt)
    {
        TH1D* inputHistogram = inputHistogramVector.at(hIt);
        inputHistogram->SetStats(0);          // No statistics on upper plot
        inputHistogram->SetTitle(title.data()); // Remove the ratio title
        inputHistogram->Draw("E same");               // Draw inputHistogram
        // inputHistogram settings
        assert(plotColorVector.at(hIt) != kBlack);
        inputHistogram->SetLineColor(plotColorVector.at(hIt));
        inputHistogram->SetLineWidth(2);
        if(referenceHistogram->GetMaximum() < inputHistogram->GetMaximum()) referenceHistogram->SetMaximum(inputHistogram->GetMaximum()*1.1);
    }
    // gROOT->ForceStyle();

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

        TH1D *ratio = (TH1D*)inputHistogramVector.at(hIt)->Clone("ratio");
        ratio->SetLineColor(plotColorVector.at(hIt));
        ratio->SetMinimum(0.5);  // Define Y ..
        ratio->SetMaximum(1.5); // .. range
        ratio->SetStats(0);      // No statistics on lower plot
        ratio->Divide(referenceHistogram);
        ratio->SetMarkerStyle(21);
        ratio->SetMarkerSize(0.3);

        // Ratio plot (ratio) settings
        ratio->SetTitle(""); // Remove the ratio title

        if(hIt==0)
        {
            ratio->Draw("ep");       // Draw the ratio plot
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
            ratio->GetXaxis()->SetLabelSize(0.09);
        }
        else ratio->Draw("ep same");       // Draw the ratio plot
    }

    theCanvas->cd();

}

void RatioPlotFromFile(TVirtualPad *theCanvas, std::string referenceFileName, std::string referenceHistogramName, std::vector<std::string> inputFileNameVector, std::vector<std::string> inputHistogramNameVector, std::vector<EColor> plotColorVector, bool normalize = false, float normalizeValue = -1, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "") 
{
    assert(inputFileNameVector == inputHistogramNameVector == plotColorVector);
    TFile referenceFile(referenceFileName.data());
    TH1D *referenceHistogram = (TH1D*)referenceFile.Get(referenceHistogramName.data());
    if(referenceHistogram == NULL)
    {
        std::cerr<<"referenceHistogram does not exist\n";
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
            std::cerr<<"inputHistogram does not exist\n";
            return;
        }
        inputHistogram->SetDirectory(0);
        inputHistogramVector.emplace_back(inputHistogram);
        inputFile.Close();
    }
    
    RatioPlot(theCanvas, referenceHistogram, inputHistogramVector, plotColorVector,  normalize, normalizeValue, xMin, xMax, rebinNumber, xAxis, yAxis);
    
    return;
}

void RatioAllMC()
{
    gROOT->SetBatch();

    TCanvas *theCanvasControlRegionPlusBeforeBDT = new TCanvas("ControlRegionPlusBeforeBDT", "ControlRegionPlusBeforeBDT", 1400, 800);
    theCanvasControlRegionPlusBeforeBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"               , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      }, {kRed}, true , -1,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      }, {kRed}, true , -1,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     }, {kRed}, true , -1, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     }, {kRed}, true , -1, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, true , -1,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, true , -1,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       }, {kRed}, true , -1,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       }, {kRed}, true , -1,  300 , 1000, 4, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, true , -1,  8900, 9600, 3, "m_{X} [GeV]"     );
    // RatioPlotFromFile(theCanvasControlRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, -1,  8900, 9600, 1, "m_{X} [GeV]"     );
    theCanvasControlRegionPlusBeforeBDT->SaveAs((std::string(theCanvasControlRegionPlusBeforeBDT->GetName()) + ".png").data());

    TCanvas *theCanvasSignalRegionPlusBeforeBDT = new TCanvas("SignalRegionPlusBeforeBDT", "SignalRegionPlusBeforeBDT", 1400, 800);
    theCanvasSignalRegionPlusBeforeBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, true , -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, true , -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, true , -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, true , -1,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      }, {kRed}, true , -1,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      }, {kRed}, true , -1,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     }, {kRed}, true , -1, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     }, {kRed}, true , -1, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, true , -1,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, true , -1,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       }, {kRed}, true , -1,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       }, {kRed}, true , -1,  300 , 1000, 1, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, true , -1,  8900, 9600, 3, "m_{X} [GeV]"     );
    // RatioPlotFromFile(theCanvasSignalRegionPlusBeforeBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_AntiBtag/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_AntiBtag_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, -1,  8900, 9600, 1, "m_{X} [GeV]"     );
    theCanvasSignalRegionPlusBeforeBDT->SaveAs((std::string(theCanvasSignalRegionPlusBeforeBDT->GetName()) + ".png").data());



    TCanvas *theCanvasControlRegionPlusAfterBDT = new TCanvas("ControlRegionPlusAfterBDT", "ControlRegionPlusAfterBDT", 1400, 800);
    theCanvasControlRegionPlusAfterBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root", "data_BTagCSV_Plus_Background/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_Background_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b1_pt"                                             , {"2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_pt"                      }, {kRed}, false, -1,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_pt"                      }, {kRed}, false, -1,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_eta"                     }, {kRed}, false, -1, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_eta"                     }, {kRed}, false, -1, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, false, -1,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, false, -1,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_H2_m"                       }, {kRed}, false, -1,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m"                       }, {kRed}, false, -1,  300 , 1000, 4, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasControlRegionPlusAfterBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" , "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root" }, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_ControlRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_ControlRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, -1,  8900, 9600, 3, "m_{X} [GeV]"     );
    theCanvasControlRegionPlusAfterBDT->SaveAs((std::string(theCanvasControlRegionPlusAfterBDT->GetName()) + ".png").data());

    TCanvas *theCanvasSignalRegionPlusAfterBDT = new TCanvas("SignalRegionPlusAfterBDT", "SignalRegionPlusAfterBDT", 1400, 800);
    theCanvasSignalRegionPlusAfterBDT->DivideSquare(9,0.005,0.005);
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b1_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_b2_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    // RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b1_pt"                   , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_b2_pt"                   }, {kRed}, false, -1,  0   , 1000, 4, "_Plus_Background");
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_pt"                      }, {kRed}, false, -1,  0   , 600 , 4, "pT_{H} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_pt"                      }, {kRed}, false, -1,  0   , 800 , 4, "pT_{Y} [GeV]"    );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_eta"                     }, {kRed}, false, -1, -5   ,    5, 2, "#eta_{H}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_eta"                     }, {kRed}, false, -1, -5   ,    5, 2, "#eta_{Y}"        );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H1_bb_DeltaR"               }, {kRed}, false, -1,  0   ,    5, 1, "#DeltaR_{bb(H)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_bb_DeltaR"               }, {kRed}, false, -1,  0   ,    5, 1, "#DeltaR_{bb(Y)}" );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_H2_m"                       }, {kRed}, false, -1,  130 , 900 , 4, "m_{Y} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       , {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m"                       }, {kRed}, false, -1,  300 , 1000, 1, "m_{X} [GeV]"     );
    RatioPlotFromFile(theCanvasSignalRegionPlusAfterBDT->cd(9),"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root", "data_BTagCSV_Plus_MXless1000_MYgreater140/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_MXless1000_MYgreater140_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled", {"2016DataPlots_NMSSM_XYH_bbbb_bJetKinVariablesOnly/outPlotter.root"}, {"data_BTagCSV_Plus/selectionbJetsAndTrigger_SignalRegionPlusBlinded/data_BTagCSV_Plus_selectionbJetsAndTrigger_SignalRegionPlusBlinded_HH_m_H2_m_Rebinned_Unrolled"}, {kRed}, false, -1,  8900, 9600, 3, "m_{X} [GeV]"     );
    theCanvasSignalRegionPlusAfterBDT->SaveAs((std::string(theCanvasSignalRegionPlusAfterBDT->GetName()) + ".png").data());

    return;
}


void RatioSlices(std::string canvasName, std::string referenceFileName, std::string referenceHistogramName, std::string inputFileName, std::string inputHistogramName, bool normalize = false, float normalizeValue = -1, float xMin=0, float xMax = 1500, int rebinNumber = 1, std::string xAxis = "", std::string yAxis = "") 
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
    uint rebinFactor = 6;

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
            RatioPlot(theCanvas->cd(y), referenceHistogram, {inputHistogram}, {kRed}, normalize, normalizeValue, xMin, xMax, rebinNumber, xAxis, yAxis, Form("%.1f < M_{Y} < %.1f", mYmin, mYmax));
        }

        theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());
        delete theCanvas;

        // if( (i+1)*nPadPerCanvas >= numberOfYbins) break;

    }

}


void RatioAllVariables(std::string canvasName, std::string referenceFileName, std::string referenceDatasetName , std::string referenceCutName, std::string targetFileName, std::string targetDatasetName , std::string targetCutName, bool normalize)
{

    std::string referenceHistPrototype = referenceDatasetName +  "/" + referenceCutName + "/" + referenceDatasetName +  "_" + referenceCutName;
    std::string targetHistPrototype    = targetDatasetName    +  "/" + targetCutName    + "/" + targetDatasetName    +  "_" + targetCutName   ;
    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 800);
    theCanvas->DivideSquare(9,0.005,0.005);
    RatioPlotFromFile(theCanvas->cd(1),referenceFileName ,referenceHistPrototype + "_H1_pt"                       , {targetFileName} , {targetHistPrototype + "_H1_pt"                      }, {kRed} , normalize, -1,     0,   600, 4, "pT_{H} [GeV]");
    RatioPlotFromFile(theCanvas->cd(2),referenceFileName ,referenceHistPrototype + "_H2_pt"                       , {targetFileName} , {targetHistPrototype + "_H2_pt"                      }, {kRed} , normalize, -1,     0,   800, 4, "pT_{Y} [GeV]");
    RatioPlotFromFile(theCanvas->cd(3),referenceFileName ,referenceHistPrototype + "_H1_eta"                      , {targetFileName} , {targetHistPrototype + "_H1_eta"                     }, {kRed} , normalize, -1,    -5,     5, 2, "#eta_{H}");
    RatioPlotFromFile(theCanvas->cd(4),referenceFileName ,referenceHistPrototype + "_H2_eta"                      , {targetFileName} , {targetHistPrototype + "_H2_eta"                     }, {kRed} , normalize, -1,    -5,     5, 2, "#eta_{Y}");
    RatioPlotFromFile(theCanvas->cd(5),referenceFileName ,referenceHistPrototype + "_H1_bb_DeltaR"                , {targetFileName} , {targetHistPrototype + "_H1_bb_DeltaR"               }, {kRed} , normalize, -1,     0,     5, 1, "#DeltaR_{bb(H)}");
    RatioPlotFromFile(theCanvas->cd(6),referenceFileName ,referenceHistPrototype + "_H2_bb_DeltaR"                , {targetFileName} , {targetHistPrototype + "_H2_bb_DeltaR"               }, {kRed} , normalize, -1,     0,     5, 1, "#DeltaR_{bb(Y)}");
    RatioPlotFromFile(theCanvas->cd(7),referenceFileName ,referenceHistPrototype + "_H2_m"                        , {targetFileName} , {targetHistPrototype + "_H2_m"                       }, {kRed} , normalize, -1,   130,   900, 4, "m_{Y} [GeV]");
    RatioPlotFromFile(theCanvas->cd(8),referenceFileName ,referenceHistPrototype + "_HH_m"                        , {targetFileName} , {targetHistPrototype + "_HH_m"                       }, {kRed} , normalize, -1,   300,  1000, 1, "m_{X} [GeV]");
    RatioPlotFromFile(theCanvas->cd(9),referenceFileName ,referenceHistPrototype + "_HH_m_H2_m_Rebinned_Unrolled" , {targetFileName} , {targetHistPrototype + "_HH_m_H2_m_Rebinned_Unrolled"}, {kRed} , normalize, -1,     0, 52000, 9, "m_{X}*m_{Y}");
    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());
    delete theCanvas;

    RatioSlices(canvasName, referenceFileName, referenceHistPrototype + "_HH_m_H2_m_Rebinned", targetFileName, targetHistPrototype + "_HH_m_H2_m_Rebinned", normalize, 0.0770485, 0, 2400, 3, "m_{X} [GeV]");
  
}


void RatioAll()
{
    gROOT->SetBatch();

    RatioAllVariables("ControlRegionBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_ControlRegionBlinded",true);

    RatioAllVariables("ControlRegionAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_ControlRegionBlinded",false);

    RatioAllVariables("SideBandBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_SideBandBlinded",true);

    RatioAllVariables("SideBandAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_SideBandBlinded",false);


    RatioAllVariables("ControlRegionDownBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionDownBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_ControlRegionDownBlinded",true);

    RatioAllVariables("ControlRegionDownAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionDownBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_ControlRegionDownBlinded",false);

    RatioAllVariables("SideBandDownBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandDownBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_SideBandDownBlinded",true);

    RatioAllVariables("SideBandDownAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandDownBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_SideBandDownBlinded",false);


    RatioAllVariables("ControlRegionUpBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionUpBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_ControlRegionUpBlinded",true);

    RatioAllVariables("ControlRegionUpAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_ControlRegionUpBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_ControlRegionUpBlinded",false);

    RatioAllVariables("SideBandUpBeforeBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandUpBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_3bTag_SideBandUpBlinded",true);

    RatioAllVariables("SideBandUpAfterBDT", "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV" , "selectionbJetsAndTrigger_4bTag_SideBandUpBlinded", 
    "2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root", "data_BTagCSV_dataDriven" , "selectionbJetsAndTrigger_3bTag_SideBandUpBlinded",false);

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

void RatioTriggerClosure()
{

    gROOT->SetBatch();

    auto makeTriggerVariationNames = [](std::string variableName, std::string selection) -> std::vector<string>
    {
        return {"/ttBarNotTriggered/notTriggeredSelectionScaled_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaled_" + selection + "_" + variableName                             ,
                "/ttBarNotTriggered/notTriggeredSelectionScaled_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaled_" + selection + "_" + variableName + "_triggerMcEfficiencyUp"  ,
                "/ttBarNotTriggered/notTriggeredSelectionScaled_" + selection + "/ttBarNotTriggered_notTriggeredSelectionScaled_" + selection + "_" + variableName + "_triggerMcEfficiencyDown"};
    };
    
    std::vector<std::string> inputFileNameList  {"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_notTriggered/outPlotter.root", "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_notTriggered/outPlotter.root", "2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_notTriggered/outPlotter.root"};
    std::vector<EColor>      histogramColorList {kRed                                                                      , kBlue                                                                     , kGreen                                                                    };
    
    TCanvas *theCanvasTriggerClosureTTbar1 = new TCanvas("TriggerClosureTTbar1", "TriggerClosureTTbar1", 1400, 800);
    theCanvasTriggerClosureTTbar1->DivideSquare(8,0.005,0.005);
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_FirstJetPt"      , inputFileNameList, makeTriggerVariationNames("FirstJetPt"      , "Full"), histogramColorList, false, -1, 0.   , 600., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_SecondJetPt"     , inputFileNameList, makeTriggerVariationNames("SecondJetPt"     , "Full"), histogramColorList, false, -1, 0.   , 400., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_ThirdJetPt"      , inputFileNameList, makeTriggerVariationNames("ThirdJetPt"      , "Full"), histogramColorList, false, -1, 0.   , 250., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_FourthJetPt"     , inputFileNameList, makeTriggerVariationNames("FourthJetPt"     , "Full"), histogramColorList, false, -1, 0.   , 200., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_FirstJetEta"     , inputFileNameList, makeTriggerVariationNames("FirstJetEta"     , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_SecondJetEta"    , inputFileNameList, makeTriggerVariationNames("SecondJetEta"    , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_ThirdJetEta"     , inputFileNameList, makeTriggerVariationNames("ThirdJetEta"     , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar1->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_FourthJetEta"    , inputFileNameList, makeTriggerVariationNames("FourthJetEta"    , "Full"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    theCanvasTriggerClosureTTbar1->SaveAs((std::string(theCanvasTriggerClosureTTbar1->GetName()) + ".png").data());
  
    TCanvas *theCanvasTriggerClosureTTbar2 = new TCanvas("TriggerClosureTTbar2", "TriggerClosureTTbar2", 1400, 800);
    theCanvasTriggerClosureTTbar2->DivideSquare(8,0.005,0.005);
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_FirstJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("FirstJetDeepCSV" , "Full"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_SecondJetDeepCSV", inputFileNameList, makeTriggerVariationNames("SecondJetDeepCSV", "Full"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_ThirdJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("ThirdJetDeepCSV" , "Full"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_FourthJetDeepCSV", inputFileNameList, makeTriggerVariationNames("FourthJetDeepCSV", "Full"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_H1_m"            , inputFileNameList, makeTriggerVariationNames("H1_m"            , "Full"), histogramColorList, false, -1, 0.   , 300., 1, "m [GeV]"    , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_H2_m"            , inputFileNameList, makeTriggerVariationNames("H2_m"            , "Full"), histogramColorList, false, -1, 0.   , 1200., 1, "m [GeV]"    , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbar2->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_Full/ttBarTriggered_triggeredMatchedSelection_Full_HH_m"            , inputFileNameList, makeTriggerVariationNames("HH_m"            , "Full"), histogramColorList, false, -1, 200. , 1200., 1, "m [GeV]"    , "entries [a.u.]");
    theCanvasTriggerClosureTTbar2->SaveAs((std::string(theCanvasTriggerClosureTTbar2->GetName()) + ".png").data());

    
    TCanvas *theCanvasTriggerClosureTTbarSignalRegion1 = new TCanvas("TriggerClosureTTbarSignalRegion1", "TriggerClosureTTbarSignalRegion1", 1400, 800);
    theCanvasTriggerClosureTTbarSignalRegion1->DivideSquare(8,0.005,0.005);
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_FirstJetPt"      , inputFileNameList, makeTriggerVariationNames("FirstJetPt"      , "SignalRegion"), histogramColorList, false, -1, 0.   , 600., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_SecondJetPt"     , inputFileNameList, makeTriggerVariationNames("SecondJetPt"     , "SignalRegion"), histogramColorList, false, -1, 0.   , 400., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_ThirdJetPt"      , inputFileNameList, makeTriggerVariationNames("ThirdJetPt"      , "SignalRegion"), histogramColorList, false, -1, 0.   , 250., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_FourthJetPt"     , inputFileNameList, makeTriggerVariationNames("FourthJetPt"     , "SignalRegion"), histogramColorList, false, -1, 0.   , 200., 1, "p_{T} [GeV]", "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_FirstJetEta"     , inputFileNameList, makeTriggerVariationNames("FirstJetEta"     , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_SecondJetEta"    , inputFileNameList, makeTriggerVariationNames("SecondJetEta"    , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_ThirdJetEta"     , inputFileNameList, makeTriggerVariationNames("ThirdJetEta"     , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion1->cd(8),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_FourthJetEta"    , inputFileNameList, makeTriggerVariationNames("FourthJetEta"    , "SignalRegion"), histogramColorList, false, -1, -2.5 , 2.5 , 1, "eta"        , "entries [a.u.]");
    theCanvasTriggerClosureTTbarSignalRegion1->SaveAs((std::string(theCanvasTriggerClosureTTbarSignalRegion1->GetName()) + ".png").data());
  
    TCanvas *theCanvasTriggerClosureTTbarSignalRegion2 = new TCanvas("TriggerClosureTTbarSignalRegion2", "TriggerClosureTTbarSignalRegion2", 1400, 800);
    theCanvasTriggerClosureTTbarSignalRegion2->DivideSquare(8,0.005,0.005);
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(1),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_FirstJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("FirstJetDeepCSV" , "SignalRegion"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(2),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_SecondJetDeepCSV", inputFileNameList, makeTriggerVariationNames("SecondJetDeepCSV", "SignalRegion"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(3),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_ThirdJetDeepCSV" , inputFileNameList, makeTriggerVariationNames("ThirdJetDeepCSV" , "SignalRegion"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(4),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_FourthJetDeepCSV", inputFileNameList, makeTriggerVariationNames("FourthJetDeepCSV", "SignalRegion"), histogramColorList, false, -1, 0.   , 1.  , 1, "deepFlav"   , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(5),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_H1_m"            , inputFileNameList, makeTriggerVariationNames("H1_m"            , "SignalRegion"), histogramColorList, false, -1, 0.   , 1200., 1, "m [GeV]"    , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(6),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_H2_m"            , inputFileNameList, makeTriggerVariationNames("H2_m"            , "SignalRegion"), histogramColorList, false, -1, 0.   , 400., 1, "m [GeV]"    , "entries [a.u.]");
    RatioPlotFromFile(theCanvasTriggerClosureTTbarSignalRegion2->cd(7),"2016DataPlots_NMSSM_XYH_bbbb_triggerClosure_triggered/outPlotter.root", "ttBarTriggered/triggeredMatchedSelection_SignalRegion/ttBarTriggered_triggeredMatchedSelection_SignalRegion_HH_m"            , inputFileNameList, makeTriggerVariationNames("HH_m"            , "SignalRegion"), histogramColorList, false, -1, 200. , 1200., 1, "m [GeV]"    , "entries [a.u.]");
    theCanvasTriggerClosureTTbarSignalRegion2->SaveAs((std::string(theCanvasTriggerClosureTTbarSignalRegion2->GetName()) + ".png").data());

    gROOT->SetBatch(false);

}