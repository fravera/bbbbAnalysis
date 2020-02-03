#include "Riostream.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"
#include "TArrayD.h"
#include "TClass.h"
#include "TCollection.h"
#include "TKey.h"
#include "TROOT.h"
#include <iostream>
#include <cmath>

// g++  -std=c++17 -I `root-config --incdir` -o Unroll2Dplots Unroll2Dplots.cc `root-config --libs` -O3 && ./Unroll2Dplots ..//2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root \
data_BTagCSV_Background/selectionbJetsAndTrigger_SignalRegion/data_BTagCSV_Background_selectionbJetsAndTrigger_SignalRegion_HH_m_H2_m

// ./Unroll2Dplots ..//2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root \
sig_privateMC_NMSSM_bbbb_MX_700_MY_300/selectionbJetsAndTrigger_SignalRegion/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_selectionbJetsAndTrigger_SignalRegion_HH_m_H2_m

// ./Unroll2Dplots ..//2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root \
data_BTagCSV/selectionbJetsAndTrigger_SignalRegion/data_BTagCSV_selectionbJetsAndTrigger_SignalRegion_HH_m_H2_m

//g++  -std=c++17 -I `root-config --incdir` -o Unroll2Dplots Unroll2Dplots.cc `root-config --libs` -O3 && ./Unroll2Dplots ../plotResults/2016DataPlots_NMSSM_XYH_bbbb/outPlotter.root data_BTagCSV_BackgroundTriggerMatched selectionbJetsAndTrigger_SignalRegion HH_m_H2_m

// ./scripts/Unroll2Dplots 2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root data_BTagCSV_dataDriven selectionbJetsAndTrigger_3bTag_SignalRegion HH_m_H2_m selectionbJetsAndTrigger_3bTag_ControlRegionBlinded selectionbJetsAndTrigger_3bTag_SideBandBlinded selectionbJetsAndTrigger_4bTag_ControlRegionBlinded selectionbJetsAndTrigger_4bTag_SideBandBlinded selectionbJetsAndTrigger_4bTag_SignalRegion
// ./scripts/Unroll2Dplots 2016DataPlots_NMSSM_XYH_bbbb_PtRegressedAndHigherLevel_VCR_30_VSR_10/outPlotter.root data_BTagCSV_dataDriven_noTrigMatch selectionbJets_3bTag_SignalRegion HH_m_H2_m selectionbJets_3bTag_ControlRegionBlinded selectionbJets_3bTag_SideBandBlinded selectionbJets_4bTag_ControlRegionBlinded selectionbJets_4bTag_SideBandBlinded selectionbJets_4bTag_SignalRegion

float higgsMass = 120;
float konigsbergLine = 800;

//-------------------------------------------------------------------------------------------------------------------------------------//

bool isNeededBin(TH2F *the2Dplot, uint xBin, uint yBin)
{
    float mX = the2Dplot->GetXaxis()->GetBinCenter(xBin);
    float mY = the2Dplot->GetYaxis()->GetBinCenter(yBin);

    if( (mX - mY > higgsMass) && (mX - mY < higgsMass + konigsbergLine) )
    {
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------//

void FillRebinnedPlot (TH2F *the2DsourcePlot, TH2F *the2DtargetPlot)
{

    for(uint xBin = 1; xBin <= the2DsourcePlot->GetNbinsX(); xBin++)
    {
        for(uint yBin = 1; yBin <= the2DsourcePlot->GetNbinsY(); yBin++)
        {
            uint newXBin = the2DtargetPlot->GetXaxis()->FindBin(the2DsourcePlot->GetXaxis()->GetBinCenter(xBin));
            uint newYBin = the2DtargetPlot->GetYaxis()->FindBin(the2DsourcePlot->GetYaxis()->GetBinCenter(yBin));
            float previousContent = the2DtargetPlot->GetBinContent(newXBin,newYBin);
            float previousError   = the2DtargetPlot->GetBinError  (newXBin,newYBin);
            the2DtargetPlot->SetBinContent(newXBin,newYBin, previousContent + the2DsourcePlot->GetBinContent(xBin,yBin));
            the2DtargetPlot->SetBinError  (newXBin,newYBin, sqrt(previousError*previousError + the2DsourcePlot->GetBinError(xBin,yBin)*the2DsourcePlot->GetBinError(xBin,yBin)));
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------//

void Rebin2DPlot(TH2F *&the2Dplot, bool rebinX=true, uint xBinStart=1, uint yBinStart=1)
{
    std::cout<<"Rebinning\n";
  
    uint nXbin = the2Dplot->GetNbinsX();
    uint nYbin = the2Dplot->GetNbinsY();
    the2Dplot->SetName("oldPlot");
    
    uint xBin = xBinStart;
    uint yBin = yBinStart;

    bool allNeededBinsAreNotEmpty = true;
    for(xBin = xBinStart; xBin <= nXbin; xBin++)
    {
        for(yBin = yBinStart; yBin <= nYbin; yBin++)
        {
            if(isNeededBin(the2Dplot, xBin, yBin) && the2Dplot->GetBinContent(xBin,yBin) <= 5)
            {
                allNeededBinsAreNotEmpty = false;
                std::cout << "First empty needed bin = " << xBin << " - " << yBin << std::endl;
                break;
            }
        }

        if(!allNeededBinsAreNotEmpty) break;
    }

    if(allNeededBinsAreNotEmpty) return;

    xBinStart = xBin;
    yBinStart = yBin;

    TArrayD theOriginalBinArray;
    TArrayD theUntouchedBinArray;
    uint binForRebinStart;
    uint binListSize = 0;
    const double *xBinArray;
    const double *yBinArray;
    if(rebinX)
    {
        theOriginalBinArray = *the2Dplot->GetXaxis()->GetXbins();
        if(theOriginalBinArray.GetSize() == 0)
        {
            std::cout<<"Empty\n";
            double binArray[nXbin+1];
            for(xBin = 1; xBin <= nXbin; xBin++)
            {
                binArray[xBin-1] = the2Dplot->GetXaxis()->GetBinLowEdge(xBin);
            }
            binArray[nXbin] = the2Dplot->GetXaxis()->GetBinUpEdge(nXbin);
            theOriginalBinArray.Set(nXbin+1,binArray);
        }

        theUntouchedBinArray = *the2Dplot->GetYaxis()->GetXbins();
        if(theUntouchedBinArray.GetSize() == 0)
        {
            std::cout<<"Empty\n";
            double binArray[nYbin+1];
            for(yBin = 1; yBin <= nYbin; yBin++)
            {
                binArray[yBin-1] = the2Dplot->GetYaxis()->GetBinLowEdge(yBin);
            }
            binArray[nYbin] = the2Dplot->GetYaxis()->GetBinUpEdge(nYbin);
            theUntouchedBinArray.Set(nYbin+1,binArray);
        }

        binForRebinStart = xBinStart;
    }
    else
    {

        theUntouchedBinArray = *the2Dplot->GetXaxis()->GetXbins();
        if(theUntouchedBinArray.GetSize() == 0)
        {
            double binArray[nXbin+1];
            for(xBin = 1; xBin <= nXbin; xBin++)
            {
                binArray[xBin-1] = the2Dplot->GetXaxis()->GetBinLowEdge(xBin);
            }
            binArray[nXbin] = the2Dplot->GetXaxis()->GetBinUpEdge(nXbin);
            theUntouchedBinArray.Set(nXbin+1,binArray);
        }

        theOriginalBinArray = *the2Dplot->GetYaxis()->GetXbins();
        if(theOriginalBinArray.GetSize() == 0)
        {
            double binArray[nYbin+1];
            for(yBin = 1; yBin <= nYbin; yBin++)
            {
                binArray[yBin-1] = the2Dplot->GetYaxis()->GetBinLowEdge(yBin);
            }
            binArray[nYbin] = the2Dplot->GetYaxis()->GetBinUpEdge(nYbin);
            theOriginalBinArray.Set(nYbin+1,binArray);
        }

        binForRebinStart = yBinStart;
    }

    std::cout<<"Axis " << (rebinX ? "X" : "Y") << " number of bins = " << theOriginalBinArray.GetSize()<<std::endl;
    std::cout<<"First empty bin = " << binForRebinStart <<std::endl;
    double theBinList[theOriginalBinArray.GetSize()];

    //Because it is a great idea
    for(uint bin = 0; bin<theOriginalBinArray.GetSize(); ++bin)
    {
        if(int(bin) < int(binForRebinStart-2)) theBinList[binListSize] = theOriginalBinArray.At(bin);
        else
        {
            // bin += rebinX ? 3 : 1; //skip one more bin
            bin += 1;
            theBinList[binListSize] = theOriginalBinArray.At(bin);
            // if(bin >= theOriginalBinArray.GetSize() - (rebinX ? 4 : 2))
            if(bin >= theOriginalBinArray.GetSize() - 2)
            {
                theBinList[binListSize] = theOriginalBinArray.At(theOriginalBinArray.GetSize()-1);
                break;
            }
            //if just 1 bin remains, I merge it with the last one
        }
        binListSize++;
    }
    std::cout<<"New number of Bins = " <<binListSize<<std::endl;

    uint xBinListSize;
    uint yBinListSize;
    if(rebinX)
    {
        xBinListSize = binListSize;
        yBinListSize = the2Dplot->GetNbinsY() + 1;
        xBinArray = theBinList;
        yBinArray = theUntouchedBinArray.GetArray();
    }
    else
    {
        xBinListSize = the2Dplot->GetNbinsX() + 1;
        yBinListSize = binListSize;
        xBinArray = theUntouchedBinArray.GetArray();
        yBinArray = theBinList;
    }

    TH2F *theRebinnedPlot = new TH2F("newPlot","newPlot",xBinListSize-1,xBinArray,yBinListSize-1,yBinArray);
    theRebinnedPlot->SetDirectory(0);

    FillRebinnedPlot(the2Dplot,theRebinnedPlot);

    delete the2Dplot;
    the2Dplot = theRebinnedPlot;

    Rebin2DPlot(the2Dplot, !rebinX, xBinStart, yBinStart);

}

//-------------------------------------------------------------------------------------------------------------------------------------//

TH1F* UnrollPlot(TH2F* the2Dplot)
{
    uint nXbin = the2Dplot->GetNbinsX();
    uint nYbin = the2Dplot->GetNbinsY();
    uint numberOfBins = nXbin*nYbin;
    float theBinArray[numberOfBins+1];
    theBinArray[0] = the2Dplot->GetXaxis()->GetBinLowEdge(1);
    
    uint position = 1;
    for(uint yBin=1; yBin<=nYbin; ++yBin)
    {
        for(uint xBin=1; xBin<=nXbin; ++xBin)
        {
            theBinArray[position] = theBinArray[position-1] + the2Dplot->GetXaxis()->GetBinWidth(xBin);
            ++position;
        }
    }

    std::string unRolledPlotName = std::string(the2Dplot->GetName()) + "_Unrolled";
    TH1F *the1Dplot = new TH1F(unRolledPlotName.data(),unRolledPlotName.data(),numberOfBins,theBinArray);
    the1Dplot->SetDirectory(0);

    uint newBinNumber = 1;
    for(uint yBin = 1; yBin <= nYbin; yBin++)
    {
        for(uint xBin = 1; xBin <= nXbin; xBin++)
        {
            the1Dplot->SetBinContent(xBin+(yBin-1)*nXbin, the2Dplot->GetBinContent(xBin,yBin));
            the1Dplot->SetBinError(xBin+(yBin-1)*nXbin, the2Dplot->GetBinError(xBin,yBin));
        }
    }
    return the1Dplot;
}

//-------------------------------------------------------------------------------------------------------------------------------------//

int main(int argc, char *argv[])
{
    if(argc < 5)
    {
        std::cout << "Usage: ./Unroll2Dplots <fileName> <dataset> <selection> <variable> <otherSelectionToUnroll - optional>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string dataDataset = argv[2];
    std::string selection   = argv[3];
    std::string variable    = argv[4];
    std::vector<std::string> selectionsToUnrollList = {selection};
    for(uint i=5; i<argc; ++i) selectionsToUnrollList.push_back(argv[i]);

    TFile theInputFile(argv[1], "UPDATE");
    std::string dataHistogramName = dataDataset + "/" + selection + "/" + dataDataset + "_" + selection + "_"  + variable;
    TH2F *the2Dplot = (TH2F*)theInputFile.Get(dataHistogramName.data());
    the2Dplot->SetDirectory(0);
   
    Rebin2DPlot(the2Dplot);

    uint nXbin = the2Dplot->GetNbinsX();
    uint nYbin = the2Dplot->GetNbinsY();
    const double* xBinArray = the2Dplot->GetXaxis()->GetXbins()->GetArray();
    const double* yBinArray = the2Dplot->GetYaxis()->GetXbins()->GetArray();

    TIter next(theInputFile.GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) 
    {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TDirectoryFile")) continue;
        std::string theCurrentDataDataset = key->ReadObj()->GetName();
        std::cout << theCurrentDataDataset <<std::endl;
        for(const auto& selectionName : selectionsToUnrollList)
        {
            theInputFile.cd();
            theInputFile.cd((theCurrentDataDataset + "/" + selectionName).data());
            std::string theCurrentDirectory = theCurrentDataDataset + "/" + selectionName + "/";
            std::string theCurrentHistogramName = theCurrentDataDataset + "_" + selectionName + "_"  + variable;
            TH2F *theCurrent2Dplot = (TH2F*)theInputFile.Get((theCurrentDirectory+theCurrentHistogramName).data());
            std::string theRebinnedPlotName = theCurrentHistogramName + "_Rebinned";
            TH2F *theRebinnedPlot = new TH2F(theRebinnedPlotName.data(),theRebinnedPlotName.data(),nXbin,xBinArray,nYbin,yBinArray);

            FillRebinnedPlot(theCurrent2Dplot,theRebinnedPlot);

            theRebinnedPlot->Write(theRebinnedPlot->GetName(), TObject::kOverwrite);

            TH1F* the1Dplot = UnrollPlot(theRebinnedPlot);
            the1Dplot->Write(the1Dplot->GetName(), TObject::kOverwrite);
        }
    }

    theInputFile.Close();

}
