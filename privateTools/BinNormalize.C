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
#include <TPaveText.h>
#include <TLatex.h>


void normalizeByBinSize1D(TH1D* inputPlot)
{
    for(int nBin = 1; nBin<=inputPlot->GetNbinsX(); ++nBin)
    {
        float binWidth = inputPlot->GetXaxis()->GetBinWidth(nBin);
        inputPlot->SetBinContent(nBin,inputPlot->GetBinContent(nBin)/binWidth);
        inputPlot->SetBinError(nBin,inputPlot->GetBinError(nBin)/binWidth);
    }
    return;

}


void plot1D(std::string inputFileName, std::string dataset, std::string selection, std::string variable, std::string title, std::string xAxis, std::string yAxis, float xMin, float xMax, int rebin = -1)
{
    TFile inputFile(inputFileName.data());
    std::string inputHistogramName = dataset +  "/" + selection + "/" + dataset +  "_" + selection + "_" + variable;
    TH1D *inputHistogram = (TH1D*)inputFile.Get(inputHistogramName.data());
    inputHistogram->SetDirectory(0);
    inputHistogram->SetStats(0);
    inputHistogram->SetTitle(title.data());
    normalizeByBinSize1D(inputHistogram);
    if(rebin>0) inputHistogram->Rebin(rebin);

    inputHistogram->GetXaxis()->SetTitle(xAxis.data());
    inputHistogram->GetXaxis()->SetRangeUser(xMin, xMax);
    inputHistogram->GetYaxis()->SetTitleOffset(1.3);
    inputHistogram->GetYaxis()->SetTitle(yAxis.data());

    TCanvas *theCanvas = new TCanvas(("1Dplot_" + variable).data(),("1Dplot_" + variable).data(),1200,800);
    inputHistogram->Draw("colz");
    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());

}

void doPlot1D()
{
    gROOT->ForceStyle();
    plot1D("2016DataPlots_NMSSM_XYH_bbbb_quickTestSignal/outPlotter.root", "sig_NMSSM_bbbb_MX_700_MY_300", "selectionbJets_Full", "H1_m", "m_{Hreco} distribution - m_{X} = 700 GeV  m_{Y} = 300 GeV", "m_{Hreco} [GeV]", "events/GeV^{2} [a.u.]", 60, 190, 4);
    plot1D("2016DataPlots_NMSSM_XYH_bbbb_quickTestSignal/outPlotter.root", "sig_NMSSM_bbbb_MX_700_MY_300", "selectionbJets_Full", "H2_m", "m_{Yreco} distribution - m_{X} = 700 GeV  m_{Y} = 300 GeV", "m_{Yreco} [GeV]", "events/GeV^{2} [a.u.]", 100, 500);
    plot1D("2016DataPlots_NMSSM_XYH_bbbb_quickTestSignal/outPlotter.root", "sig_NMSSM_bbbb_MX_700_MY_300", "selectionbJets_Full", "HH_m", "m_{Xreco} distribution - m_{X} = 700 GeV  m_{Y} = 300 GeV", "m_{Xreco} [GeV]", "events/GeV^{2} [a.u.]", 400, 1000);
    // plot2D("2016DataPlots_NMSSM_XYH_bbbb_all_Full/outPlotter.root", "data_BTagCSV_dataDriven", "selectionbJets_SignalRegion", "HH_m_H2_m", "Extimated background distribution");
}

void normalizeByBinSize2D(TH2D* inputPlot)
{
    for(int nBinX = 1; nBinX<=inputPlot->GetNbinsX(); ++nBinX)
    {
        for(int nBinY = 1; nBinY<=inputPlot->GetNbinsY(); ++nBinY)
        {
            float binWidth = inputPlot->GetXaxis()->GetBinWidth(nBinX)*inputPlot->GetYaxis()->GetBinWidth(nBinY);
            inputPlot->SetBinContent(nBinX,nBinY,inputPlot->GetBinContent(nBinX,nBinY)/binWidth);
            inputPlot->SetBinError(nBinX,nBinY,inputPlot->GetBinError(nBinX,nBinY)/binWidth);
        }
    }
    return;

}

void plot2D(std::string inputFileName, std::string dataset, std::string selection, std::string variable, std::string title, int year, bool logZ=false)
{
    TFile inputFile(inputFileName.data());
    std::string inputHistogramName = dataset +  "/" + selection + "/" + dataset +  "_" + selection + "_" + variable;
    TH2D *inputHistogram = (TH2D*)inputFile.Get(inputHistogramName.data());
    if(inputHistogram==nullptr) 
    {
        std::cout<<"Plot " << inputHistogramName << " does not exist, aborting..." << std::endl;
        return;
    }
    inputHistogram->SetDirectory(0);
    inputHistogram->SetStats(0);
    inputHistogram->SetTitle(title.data());
    normalizeByBinSize2D(inputHistogram);

    inputHistogram->GetXaxis()->SetTitle("m_{Xreco} [GeV]");
    inputHistogram->GetYaxis()->SetTitle("m_{Yreco} [GeV]");
    inputHistogram->GetYaxis()->SetTitleOffset(1.);
    inputHistogram->GetZaxis()->SetTitle("events/GeV^{2}");
    inputHistogram->GetZaxis()->SetTitleOffset(0.5);
    inputHistogram->GetZaxis()->SetTitleSize(0.045);
    inputHistogram->GetZaxis()->SetTitleFont(62);
    inputHistogram->GetZaxis()->SetLabelFont(62);
    inputHistogram->GetZaxis()->SetLabelSize(0.04);


    TCanvas *theCanvas = new TCanvas((variable + "_" + dataset + "_" + std::to_string(year)).data(),"",1400,800);
    inputHistogram->Draw("colz");
    if(logZ) theCanvas->SetLogz();
    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());

}

void doPlot2D(int year)
{
    // gROOT->SetBatch(true);
    gROOT->ForceStyle();
    plot2D(std::to_string(year) + "DataPlots_NMSSM_XYH_bbbb_Full_syst_trgData/outPlotter.root", "data_BTagCSV_dataDriven_kinFit", "selectionbJets_SignalRegion", "HH_kinFit_m_H2_m", "Extimated background distribution"                     , year, true);
    // plot2D(std::to_string(year) + "DataPlots_NMSSM_XYH_bbbb_Full_syst_trgData/outPlotter.root", "sig_NMSSM_bbbb_MX_700_MY_300"  , "selectionbJets_SignalRegion", "HH_kinFit_m_H2_m", "Signal distribution - m_{X} = 700 GeV  m_{Y} = 300 GeV", year, true);
    // gROOT->SetBatch(false);
}

// void doPlot2D(int year, std::string datasetName)
// {

// }


void overlapSlices(std::string inputFileName, std::string backgroundDataset, std::string signalDataset, std::string selection, std::string variable, float yMin, float yMax, float scale = 1.)
{

    gStyle->SetTitleFontSize(0.3); 
    TFile inputFile(inputFileName.data());
    std::string backgroundHistogramName = backgroundDataset +  "/" + selection + "/" + backgroundDataset +  "_" + selection + "_" + variable;
    TH2D *backgroundHistogram = (TH2D*)inputFile.Get(backgroundHistogramName.data());
    backgroundHistogram->SetDirectory(0);
    normalizeByBinSize2D(backgroundHistogram);

    std::string signalHistogramName = signalDataset +  "/" + selection + "/" + signalDataset +  "_" + selection + "_" + variable;
    TH2D *signalHistogram = (TH2D*)inputFile.Get(signalHistogramName.data());
    signalHistogram->SetDirectory(0);
    normalizeByBinSize2D(signalHistogram);
    signalHistogram->Scale(scale);

    auto theYaxis = backgroundHistogram->GetYaxis();
    int rebinValue = 1;
    int  firstBin = theYaxis->FindBin(yMin);
    int  lastBin  = theYaxis->FindBin(yMax);
    int  nBins = (lastBin - firstBin)/(rebinValue+1);
    TCanvas *theCanvas = new TCanvas("UnrolledPlot", "UnrolledPlot", 1200, 800);
    theCanvas->Divide(nBins,1,0,0);

    int canvasCounter = 1;
    float latexOffset = 0.05;
    for(int bin = firstBin; bin <=lastBin; bin+=(rebinValue+1))
    {
        theCanvas->cd(canvasCounter++)->SetLogy();
        float mYmin = theYaxis->GetBinLowEdge(bin); 
        float mYmax = theYaxis->GetBinLowEdge(bin+rebinValue) + theYaxis->GetBinWidth(bin+rebinValue); 
        TH1D* backgroundHistogramSlice = backgroundHistogram->ProjectionX(Form("background_Mx_projection_My_%.1f_%.1f"    ,mYmin, mYmax), bin, bin+rebinValue);
        backgroundHistogramSlice->SetDirectory(0);
        TH1D* signalHistogramSlice     = signalHistogram    ->ProjectionX(Form("signal_Mx_projection_My_%.1f_%.1f"        ,mYmin, mYmax), bin, bin+rebinValue);
        signalHistogramSlice->SetDirectory(0);
        backgroundHistogramSlice->GetYaxis()->SetRangeUser(0.001,5000.);
        backgroundHistogramSlice->GetXaxis()->SetNdivisions(305);
        backgroundHistogramSlice->SetTitle(Form("%.0f < M_{Yreco} < %.0f", mYmin, mYmax));
        backgroundHistogramSlice->GetXaxis()->SetTitle("m_{Xreco} [GeV]");
        backgroundHistogramSlice->GetXaxis()->SetTitleSize(0.15);
        backgroundHistogramSlice->GetXaxis()->SetTitleOffset(0.25);
        backgroundHistogramSlice->GetXaxis()->SetLabelSize(0.1);
        backgroundHistogramSlice->GetXaxis()->SetLabelOffset(-0.05);
        backgroundHistogramSlice->SetLineWidth(2);
        backgroundHistogramSlice->SetLineColor(kBlue);
        backgroundHistogramSlice->SetStats(0);
        backgroundHistogramSlice->Draw("hist");
        signalHistogramSlice->SetLineColor(kRed);
        signalHistogramSlice->SetLineWidth(2);
        signalHistogramSlice->Draw("hist same");

        TLatex theLatex;
        theLatex.SetTextSize(0.05);
        // theCanvas->cd(0);
        // theLatex.DrawLatex(latexOffset + (float(canvasCounter)-1.)* 1./float(nBins),0.8,Form("%.1f < M_{Yreco} < %.1f", mYmin, mYmax));
        // theLatex.DrawLatex(0.2,0.8,Form("%.1f < M_{Yreco} < %.1f", mYmin, mYmax));
        // TPaveText pt(.1,.9,.9,1.);
        // pt.AddText(Form("%.0f < M_{Yreco} < %.0f", mYmin, mYmax));
        // pt.Draw("same");
        std::cout<< Form("%.0f < M_{Yreco} < %.0f", mYmin, mYmax) <<std::endl;

    }
            
}


void doOverlapSlices(int year)
{
    gROOT->ForceStyle();
    overlapSlices(std::to_string(year) + "DataPlots_NMSSM_XYH_bbbb_Full_syst/outPlotter.root", "data_BTagCSV_dataDriven", "sig_NMSSM_bbbb_MX_700_MY_300", "selectionbJets_SignalRegion", "HH_m_H2_m", 180, 350, 10);
}

void getBinList(int year)
{
    std::string inputFileName = std::to_string(year) + "DataPlots_NMSSM_XYH_bbbb_Full_syst/outPlotter.root";
    std::string datasetName  = "data_BTagCSV_dataDriven";
    std::string selection    = "selectionbJets_SignalRegion";
    std::string variable     = "HH_m_H2_m_Rebinned";
    gROOT->ForceStyle();
    plot2D( std::to_string(year) + "DataPlots_NMSSM_XYH_bbbb_Full_syst/outPlotter.root", datasetName, "selectionbJets_SignalRegion", "HH_m_H2_m_Rebinned", "Signal distribution - m_{X} = 700 GeV  m_{Y} = 300 GeV  ", year);
    TFile inputFile(inputFileName.data());
    std::string histogramName = datasetName +  "/" + selection + "/" + datasetName +  "_" + selection + "_" + variable;
    TH2D *histogram = (TH2D*)inputFile.Get(histogramName.data());
    histogram->SetDirectory(0);
    inputFile.Close();

    int numberOfBinX = histogram->GetNbinsX();
    int numberOfBinY = histogram->GetNbinsY();
    std::cout<<"Number of bin X = " << numberOfBinX<< std::endl;
    for(int nBin = 1; nBin<=numberOfBinX+1; ++nBin) std::cout<<histogram->GetXaxis()->GetBinLowEdge(nBin)<< ", ";
    std::cout<<std::endl;
    
    std::cout<<"Number of bin Y = " << numberOfBinY<< std::endl;
    for(int nBin = 1; nBin<=numberOfBinY+1; ++nBin) std::cout<<histogram->GetYaxis()->GetBinLowEdge(nBin)<< ", ";
    std::cout<<std::endl;

}