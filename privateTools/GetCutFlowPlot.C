#include "Riostream.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"
#include "TROOT.h"


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

void GetCutFlowPlot(int mX, int mY)
{
    gROOT->SetBatch(true);

    std::vector<int>     years  {2016, 2017, 2018};
    std::vector<Color_t> colors {kRed, kBlue, kGreen};

    std::string plotRelativeCutName = "RelativeCutFlow_sig_MX_" + std::to_string(mX) + "_MY_" + std::to_string(mY);
    std::string plotCutName         = "CutFlow_sig_MX_" + std::to_string(mX) + "_MY_" + std::to_string(mY);

    TCanvas theCanvas("theCanvas","theCanvas", 1400, 800);
    theCanvas.Divide(2);
    TLegend *theCutLegend = new TLegend(0.2,0.75,0.4,0.88);
    TLegend *theRelativeCutLegend = new TLegend(0.2,0.75,0.4,0.88);
    for(size_t i=0; i<years.size(); ++i)
    {
        std::string fileName = "CutFlow_" + std::to_string(years[i]) + ".root";
        TFile inputFile(fileName.c_str());

        theCanvas.cd(1);
        auto theCutPlot = getHistogramFromFile<TH1F>(inputFile, plotCutName);

        theCutPlot->SetLineColor(colors[i]);
        theCutPlot->SetLineWidth(2);
        if(i==0)
        {
            theCutPlot->GetYaxis()->SetRangeUser(0.001,5.);
            theCutPlot->GetYaxis()->SetTitle("efficiency");
            theCutPlot->Draw("");
        }
        else
            theCutPlot->Draw("same");

        theCutLegend->AddEntry(theCutPlot, ("Run " + std::to_string(years[i])).c_str(), "pl");

        theCanvas.cd(2);
        auto theRelativeCutPlot = getHistogramFromFile<TH1F>(inputFile, plotRelativeCutName);

        theRelativeCutPlot->SetLineColor(colors[i]);
        theRelativeCutPlot->SetLineWidth(2);
        if(i==0)
        {
            theRelativeCutPlot->GetYaxis()->SetRangeUser(0.,1.2);
            theRelativeCutPlot->GetYaxis()->SetTitle("relative efficiency");
            theRelativeCutPlot->Draw("");
        }
        else
            theRelativeCutPlot->Draw("same");

        theRelativeCutLegend->AddEntry(theRelativeCutPlot, ("Run " + std::to_string(years[i])).c_str(), "pl");

        inputFile.Close(); 

    }
    theCanvas.cd(1)->SetLogy();
    theCanvas.cd(1);
    theCutLegend->Draw("same");
    theCanvas.cd(2);
    theRelativeCutLegend->Draw("same");
    std::string canvasName = "CutFLow_MX_" + std::to_string(mX) + "_MY_" + std::to_string(mY) + ".png";
    theCanvas.SaveAs(canvasName.c_str());

    gROOT->SetBatch(false);

}

void GetCutAllFlowPlots()
{
    GetCutFlowPlot(300 , 125 );
    GetCutFlowPlot(400 , 80  );
    GetCutFlowPlot(600 , 125 );
    GetCutFlowPlot(700 , 300 );
    GetCutFlowPlot(900 , 500 );
    GetCutFlowPlot(1000, 125 );
    GetCutFlowPlot(1200, 800 );
    GetCutFlowPlot(1400, 500 );
    GetCutFlowPlot(1600, 300 );
    GetCutFlowPlot(1600, 1000);
    GetCutFlowPlot(1800, 600 );
    GetCutFlowPlot(1800, 1400);
}