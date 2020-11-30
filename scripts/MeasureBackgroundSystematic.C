#include "Riostream.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH2F.h"
#include "TF1.h"
#include "TH2Poly.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TPaveText.h"


struct Rectangle
{
    Rectangle(float xMin, float xMax, float yMin, float yMax)
    : fXmin(xMin), fXmax(xMax), fYmin(yMin), fYmax(yMax) {};
    float fXmin, fXmax, fYmin, fYmax;

    std::pair<Rectangle,Rectangle> divide(float divideValue, bool horizontal) const
    {
        if(horizontal) return divideHorizontally(divideValue);
        else           return divideVertically  (divideValue);
    }

    std::pair<Rectangle,Rectangle> divideVertically(float divideValue) const
    {
        return std::make_pair(Rectangle(fXmin, divideValue, fYmin, fYmax), Rectangle(divideValue, fXmax, fYmin, fYmax));
    }

    std::pair<Rectangle,Rectangle> divideHorizontally(float divideValue) const
    {
        return std::make_pair(Rectangle(fXmin, fXmax, fYmin, divideValue), Rectangle(fXmin, fXmax, divideValue, fYmax));
    }

    bool isInRectangle(float xValue, float yValue) const
    {
        return xValue >= fXmin && xValue < fXmax && yValue >= fYmin && yValue < fYmax;
    }

    void print() const
    {
        std::cout<<"Rectangle coordinates"<<std::endl;
        std::cout << "(" <<fXmin << " , " << fYmax  << ")" << "\t\t" << "(" <<fXmax << " , " << fYmax  << ")" << std::endl;
        std::cout << "(" <<fXmin << " , " << fYmin  << ")" << "\t\t" << "(" <<fXmax << " , " << fYmin  << ")" << std::endl;
    }
};

class ImprovedPloty : public TH2Poly
{
    public:
        ImprovedPloty() : TH2Poly() 
        {
            SetStats(false);
        }

        void setAllRectangleBins(const std::vector<Rectangle>& theBinList)
        {
            for(const auto& rectangle : theBinList) addRectangularBin(rectangle);
        }

        void addRectangularBin(Rectangle theRectBin)
        {
            TH2Poly::AddBin(theRectBin.fXmin, theRectBin.fYmin, theRectBin.fXmax, theRectBin.fYmax);
        }

        void fillFromHistogram(TH2F* theInputHistogram)
        {
            for(int nBinX = 1; nBinX<=theInputHistogram->GetNbinsX(); ++nBinX)
            {
                float xValue = theInputHistogram->GetXaxis()->GetBinCenter(nBinX);
                for(int nBinY = 1; nBinY<=theInputHistogram->GetNbinsY(); ++nBinY)
                {
                    float yValue      = theInputHistogram->GetYaxis()->GetBinCenter(nBinY);
                    float zValue      = theInputHistogram->GetBinContent(nBinX,nBinY);
                    float zValueError = theInputHistogram->GetBinError(nBinX,nBinY);
                    int polyBin = FindBin(xValue, yValue);
                    if(polyBin<0) std::cout<<"Bin overflow!!!"<<std::endl;
                    SetBinContent(polyBin, zValue + GetBinContent(polyBin));
                    SetBinError(polyBin, sqrt(zValueError*zValueError + GetBinError(polyBin)*GetBinError(polyBin)));
                }
            }
        }

};

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

template<typename Hist, typename... Name>
std::array<Hist*, sizeof...(Name)> getHistogramList(std::string inputFileName, Name... histogramName)
{
    TFile inputFile(inputFileName.data());
    std::array<Hist*, sizeof...(Name)> theOutputArray =  {getHistogramFromFile<Hist>(inputFile, histogramName)...};
    inputFile.Close();
    
    return theOutputArray;
}

float getEntriesInRectangle(const TH2F *theInputHistogram, const Rectangle& theRectangle)
{
    float totalEntries = 0.;
    int nBinsX = theInputHistogram->GetNbinsX();
    int nBinsY = theInputHistogram->GetNbinsY();
    for(int nBinX = 1; nBinX<=nBinsX; ++nBinX)
    {
        float xValue = theInputHistogram->GetXaxis()->GetBinCenter(nBinX);
        for(int nBinY = 1; nBinY<=nBinsY; ++nBinY)
        {
            float yValue = theInputHistogram->GetYaxis()->GetBinCenter(nBinY);
            if(theRectangle.isInRectangle(xValue, yValue)) totalEntries += theInputHistogram->GetBinContent(nBinX,nBinY);
        }
    }
    return totalEntries;
}

std::pair<Rectangle,Rectangle> getRectangleWithEntries(const TH2F *theInputHistogram, const Rectangle& theRectangle, float numberOfEntries, bool horizontal, bool entriesInLowerRectangle)
{
    float totalEntries = getEntriesInRectangle(theInputHistogram, theRectangle);
    
    int nBinsX = theInputHistogram->GetNbinsX();
    int nBinsY = theInputHistogram->GetNbinsY();
    
    int firstNBins;
    int secondNBins;
    
    if(horizontal) firstNBins  = nBinsY;
    else firstNBins  = nBinsY;

    float entriesInNeededRectangle = 0;
    float dividerValue = 0;
    for(int bin = 1; bin<=firstNBins; ++bin)
    {
        Rectangle *theNewRectangle;
        float tmpDividerValue;
        if(horizontal)
        {
            tmpDividerValue = theInputHistogram->GetYaxis()->GetBinLowEdge(bin);
            if(entriesInLowerRectangle)
            {
                tmpDividerValue += theInputHistogram->GetYaxis()->GetBinWidth(bin);
                theNewRectangle = new Rectangle(theRectangle.fXmin, theRectangle.fXmax, theRectangle.fYmin, tmpDividerValue);
            }
            else
                theNewRectangle = new Rectangle(theRectangle.fXmin, theRectangle.fXmax, tmpDividerValue, theRectangle.fYmax);  
        }
        else
        {
            tmpDividerValue = theInputHistogram->GetXaxis()->GetBinLowEdge(bin);
            if(entriesInLowerRectangle)
            {
                tmpDividerValue += theInputHistogram->GetXaxis()->GetBinWidth(bin);
                theNewRectangle = new Rectangle(theRectangle.fXmin, tmpDividerValue, theRectangle.fYmin, theRectangle.fYmax);
            }
            else
                theNewRectangle = new Rectangle(tmpDividerValue, theRectangle.fXmax, theRectangle.fYmin, theRectangle.fYmax);
        }
        if(getEntriesInRectangle(theInputHistogram,*theNewRectangle) <= numberOfEntries) break;
        dividerValue = tmpDividerValue;
    }

    return theRectangle.divide(dividerValue, horizontal);
}


std::pair<Rectangle,Rectangle> divideFromRatio(const TH2F *theInputHistogram, const Rectangle& theRectangle, float ratioContentLowRectangle, bool horizontal)
{
    // Measure normalization
    float totalEntries = getEntriesInRectangle(theInputHistogram,theRectangle);
    int nBinsX = theInputHistogram->GetNbinsX();
    int nBinsY = theInputHistogram->GetNbinsY();
    
    int firstNBins;
    if(horizontal)
    {
        firstNBins  = nBinsY;
    }
    else
    {
        firstNBins  = nBinsY;
    }

    float currentRatio = 0.;
    float previousRatio = 0.;
    float dividerValue = 0.;
    float dividerValueBeforeLast = 0.;
    for(int nBin1 = 1; nBin1<=firstNBins; ++nBin1)
    {
        Rectangle *theNewRectangle;
        if(horizontal)
        {
            dividerValue = theInputHistogram->GetYaxis()->GetBinLowEdge(nBin1);
            theNewRectangle = new Rectangle(theRectangle.fXmin, theRectangle.fXmax, theRectangle.fYmin, dividerValue);
            float binCenter = theInputHistogram->GetYaxis()->GetBinCenter(nBin1);
            if(binCenter < theRectangle.fYmin || binCenter > theRectangle.fYmax) continue;
        }
        else
        {
            dividerValue = theInputHistogram->GetXaxis()->GetBinLowEdge(nBin1);
            theNewRectangle = new Rectangle(theRectangle.fXmin, dividerValue, theRectangle.fYmin, theRectangle.fYmax);
            float binCenter = theInputHistogram->GetXaxis()->GetBinCenter(nBin1);
            if(binCenter < theRectangle.fXmin || binCenter > theRectangle.fXmax) continue;
        }
        currentRatio = getEntriesInRectangle(theInputHistogram,*theNewRectangle)/totalEntries; 
        if(currentRatio >= ratioContentLowRectangle) break;
        else
        {
            previousRatio = currentRatio;
            dividerValueBeforeLast = dividerValue;
        }
    }

    //get the closest ratio
    float theFinalDividerValue = dividerValue;
    if( abs(previousRatio-ratioContentLowRectangle) <  abs(currentRatio-ratioContentLowRectangle)) theFinalDividerValue = dividerValueBeforeLast;

    return theRectangle.divide(theFinalDividerValue, horizontal);
}

std::vector<Rectangle> divideEqually(const TH2F *theInputHistogram, const Rectangle& theRectangle, uint numberOfDivisions, bool horizontal)
{


    std::vector<Rectangle> theOutputRectangleVector;
    std::pair<Rectangle,Rectangle> theRectanglePair = std::make_pair(Rectangle(0., 0., 0., 0.), theRectangle);
    for(uint nDiv=numberOfDivisions; nDiv>1; --nDiv)
    {
        theRectanglePair =  divideFromRatio(theInputHistogram, theRectanglePair.second, 1./float(nDiv), horizontal);
        theOutputRectangleVector.push_back(theRectanglePair.first);
    }
    theOutputRectangleVector.push_back(theRectanglePair.second);

    return theOutputRectangleVector;

}


std::vector<Rectangle> alternateHalfDivide(const TH2F *theInputHistogram, const Rectangle& theRectangle, uint numberOfIterations)
{

    bool horizontal = true;
    std::vector<Rectangle> theOutputRectangleVector = {theRectangle};
    for(uint nIt=0; nIt<numberOfIterations; ++nIt)
    {
        std::vector<std::pair<Rectangle,Rectangle>> vectorOfPairs;
        for(auto theCurrentRectangle : theOutputRectangleVector)
        {
            vectorOfPairs.emplace_back(divideFromRatio(theInputHistogram, theCurrentRectangle, 0.5, horizontal));
        }
        horizontal = !horizontal;
        theOutputRectangleVector.clear();
        for(auto rectanglePair : vectorOfPairs)
        {
            theOutputRectangleVector.emplace_back(rectanglePair.first);
            theOutputRectangleVector.emplace_back(rectanglePair.second);
        }
    }
    
    return theOutputRectangleVector;

}

std::vector<Rectangle> alternateDivideUntil(const TH2F *theInputHistogram, const Rectangle& theRectangle, uint nMaxXdivision, uint nMaxYdivision, float minBinContent)
{

    bool horizontal = true;
    std::vector<Rectangle> theOutputRectangleVector = {theRectangle};
    bool stillToBeDivided;
    do
    {
        stillToBeDivided = false;
        std::vector<Rectangle> theCurrentRectangleVector;
        for(auto theCurrentRectangle : theOutputRectangleVector)
        {
            uint numberOfDivision = 0;
            if(horizontal) numberOfDivision = nMaxYdivision;
            else           numberOfDivision = nMaxXdivision;
            float totalEventsInRectangle = getEntriesInRectangle(theInputHistogram, theCurrentRectangle);
            if(totalEventsInRectangle/minBinContent < numberOfDivision) numberOfDivision = totalEventsInRectangle/minBinContent;
            else stillToBeDivided = true;
            if(numberOfDivision == 1)
            {
                theCurrentRectangleVector.emplace_back(theCurrentRectangle);
                continue;
            }
            auto theNewRectangleVector = divideEqually(theInputHistogram, theCurrentRectangle, numberOfDivision, horizontal);
            theCurrentRectangleVector.insert(theCurrentRectangleVector.end(),theNewRectangleVector.begin(), theNewRectangleVector.end());
        }
        horizontal = !horizontal;
        theOutputRectangleVector.clear();
        for(auto rectangle : theCurrentRectangleVector) theOutputRectangleVector.emplace_back(rectangle);
    } while(stillToBeDivided);
    
    return theOutputRectangleVector;

}

void MeasureBackgroundSystematicShape(std::string inputFileName, std::string FourBtagHistogramName, std::string ThreeBtagHistogramName, float minEntriesPerRectangle, int year)
{
    auto histogramList =  getHistogramList<TH2F>(inputFileName, FourBtagHistogramName, ThreeBtagHistogramName);
    auto FourBtagHistogram  = histogramList[0];
    auto ThreeBtagHistogram = histogramList[1];

    ThreeBtagHistogram->Scale(FourBtagHistogram->Integral(0,9999999,0,9999999)/ThreeBtagHistogram->Integral(0,9999999,0,9999999));
    
    std::vector<Rectangle> theBinList;

    Rectangle theTotalRectangle
        (
            FourBtagHistogram->GetXaxis()->GetXmin(), 
            FourBtagHistogram->GetXaxis()->GetXmax(),
            FourBtagHistogram->GetYaxis()->GetXmin(), 
            FourBtagHistogram->GetYaxis()->GetXmax()
        );
    std::cout<<FourBtagHistogram->GetXaxis()->GetXmax()<<std::endl;

    float numberOfDivisions;

    auto rectanglePair_1 = getRectangleWithEntries(FourBtagHistogram, theTotalRectangle, 3. * minEntriesPerRectangle, false, false);
    auto rectanglePair_1b = divideFromRatio(FourBtagHistogram, rectanglePair_1.second, 2./3., true);
    theBinList.push_back(rectanglePair_1b.second);
    auto rectanglePair_1c = divideFromRatio(FourBtagHistogram, rectanglePair_1b.first, 1./2., false);
    theBinList.push_back(rectanglePair_1c.first);
    theBinList.push_back(rectanglePair_1c.second);
    
    // auto rectanglePair_1d = rectanglePair_1.first.divide(1100.,true);
    // theBinList.push_back(rectanglePair_1d.second);

    auto rectanglePair_1a = rectanglePair_1.first.divideHorizontally(80.);


    auto rectanglePair_2 = divideFromRatio(FourBtagHistogram, rectanglePair_1a.first, 1./2., true);
    numberOfDivisions = getEntriesInRectangle(FourBtagHistogram, rectanglePair_2.first)/minEntriesPerRectangle;
    auto rectangleVector_1 = divideEqually(FourBtagHistogram, rectanglePair_2.first, numberOfDivisions, false);
    theBinList.insert(theBinList.begin(), rectangleVector_1.begin(), rectangleVector_1.end());

    // auto rectanglePair_3 = rectanglePair_2.second.divideHorizontally(80.);
    numberOfDivisions = getEntriesInRectangle(FourBtagHistogram, rectanglePair_2.second)/minEntriesPerRectangle;
    auto rectangleVector_2 = divideEqually(FourBtagHistogram, rectanglePair_2.second, numberOfDivisions, false);
    theBinList.insert(theBinList.begin(), rectangleVector_2.begin(), rectangleVector_2.end());

    auto rectanglePair_4 = rectanglePair_1a.second.divideHorizontally(160.);
    numberOfDivisions = getEntriesInRectangle(FourBtagHistogram, rectanglePair_4.first)/minEntriesPerRectangle;
    auto theHiggsVector = divideEqually(FourBtagHistogram, rectanglePair_4.first, numberOfDivisions, false);
    theBinList.insert(theBinList.begin(), theHiggsVector.begin(), theHiggsVector.end());

    // auto rectangleVector_3 = divideEqually(FourBtagHistogram, rectanglePair_4.second, 2, false);
    // auto rectangleVector_3_0 = divideEqually(FourBtagHistogram, rectangleVector_3[0], 2, false);

    // numberOfDivisions = log(getEntriesInRectangle(FourBtagHistogram, rectanglePair_4.second)/minEntriesPerRectangle) / log(2.);
    // auto rectangleVector_3 =  alternateHalfDivide(FourBtagHistogram, rectanglePair_4.second, numberOfDivisions);
    auto rectangleVector_3 =  alternateDivideUntil(FourBtagHistogram, rectanglePair_4.second, 2, 3, minEntriesPerRectangle);
    theBinList.insert(theBinList.begin(), rectangleVector_3.begin(), rectangleVector_3.end());


    ImprovedPloty *FourBtagPoly = new ImprovedPloty();
    FourBtagPoly->SetDirectory(0);
    FourBtagPoly->SetNameTitle("FourBtag_entries","4-btag events");
    FourBtagPoly->GetXaxis()->SetTitle("m_{Xreco} [GeV]");
    FourBtagPoly->GetYaxis()->SetTitle("m_{Yreco} [GeV]");
    FourBtagPoly->GetZaxis()->SetTitle("events");
    FourBtagPoly->GetZaxis()->SetTitleOffset(1);
    FourBtagPoly->GetZaxis()->SetLabelSize(0.035);
    FourBtagPoly->GetZaxis()->SetLabelFont(62);
    FourBtagPoly->setAllRectangleBins(theBinList);

    FourBtagPoly->fillFromHistogram(FourBtagHistogram);
    // FourBtagPoly->Fill(400., 150., 10.);
    // FourBtagPoly->Fill(1600., 400., 5.);
    
    TCanvas *theCanvasShapeSystematic = new TCanvas("CanvasShapeSystematic","CanvasShapeSystematic", 1400,800);
    theCanvasShapeSystematic->Divide(2,2);
    theCanvasShapeSystematic->cd(1);
    FourBtagPoly->Draw("colz");
    // FourBtagPoly->SetMinimum(1e2);
    // FourBtagPoly->SetMaximum(1e5);
    // theCanvas->SetLogz();
    
    ImprovedPloty *ThreeBtagPoly = new ImprovedPloty();
    ThreeBtagPoly->SetDirectory(0);
    ThreeBtagPoly->SetNameTitle("ThreeBtag_entries","Bkg model events");
    ThreeBtagPoly->GetXaxis()->SetTitle("m_{Xreco} [GeV]");
    ThreeBtagPoly->GetYaxis()->SetTitle("m_{Yreco} [GeV]");
    ThreeBtagPoly->GetZaxis()->SetTitle("events");
    ThreeBtagPoly->GetZaxis()->SetTitleOffset(1);
    ThreeBtagPoly->GetZaxis()->SetLabelSize(0.035);
    ThreeBtagPoly->GetZaxis()->SetLabelFont(62);
    ThreeBtagPoly->setAllRectangleBins(theBinList);
    ThreeBtagPoly->fillFromHistogram(ThreeBtagHistogram);

    theCanvasShapeSystematic->cd(3);
    ThreeBtagPoly->Draw("colz");


    ImprovedPloty *SystematicPoly = new ImprovedPloty();
    SystematicPoly->SetDirectory(0);
    SystematicPoly->GetXaxis()->SetTitle("m_{Xreco} [GeV]");
    SystematicPoly->GetYaxis()->SetTitle("m_{Yreco} [GeV]");
    SystematicPoly->GetZaxis()->SetTitle("Shape systematic [%]");
    SystematicPoly->GetZaxis()->SetTitleOffset(1);
    SystematicPoly->GetZaxis()->SetLabelSize(0.035);
    SystematicPoly->GetZaxis()->SetLabelFont(62);
    SystematicPoly->SetNameTitle("SystematicPoly","Background Systematic Shape with 4-btag stat error");
    SystematicPoly->setAllRectangleBins(theBinList);

    ImprovedPloty *SystematicPolyNo4Bstat = new ImprovedPloty();
    SystematicPolyNo4Bstat->SetDirectory(0);
    SystematicPolyNo4Bstat->GetXaxis()->SetTitle("m_{Xreco} [GeV]");
    SystematicPolyNo4Bstat->GetYaxis()->SetTitle("m_{Yreco} [GeV]");
    SystematicPolyNo4Bstat->GetZaxis()->SetTitle("Shape systematic [%]");
    SystematicPolyNo4Bstat->GetZaxis()->SetTitleOffset(1);
    SystematicPolyNo4Bstat->GetZaxis()->SetLabelSize(0.035);
    SystematicPolyNo4Bstat->GetZaxis()->SetLabelFont(62);
    SystematicPolyNo4Bstat->SetNameTitle("SystematicPolyNo4Bstat","Background Systematic Shape");
    SystematicPolyNo4Bstat->setAllRectangleBins(theBinList);

    for(int bin=1; bin<=SystematicPoly->GetNumberOfBins(); ++bin)
    {
        float statError = 1./sqrt(FourBtagPoly->GetBinContent(bin));
        float deviation = (ThreeBtagPoly->GetBinContent(bin) - FourBtagPoly->GetBinContent(bin))/FourBtagPoly->GetBinContent(bin);
        // std::cout<<statError<< " - " << deviation <<std::endl;
        SystematicPoly->SetBinContent(bin, sqrt(statError*statError + deviation*deviation)*100);
        SystematicPolyNo4Bstat->SetBinContent(bin, sqrt(deviation*deviation)*100);
    }

    float sysMin = 0.;
    float sysMax = 20.;
    theCanvasShapeSystematic->cd(2);
    SystematicPolyNo4Bstat->SetMinimum(sysMin);
    SystematicPolyNo4Bstat->SetMaximum(sysMax);
    SystematicPolyNo4Bstat->Draw("colz");


    theCanvasShapeSystematic->cd(4);
    if(SystematicPoly->GetMaximum()> sysMax) std::cout<< "Overlow!!! value = " << SystematicPoly->GetMaximum() <<std::endl;
    SystematicPoly->SetMinimum(sysMin);
    SystematicPoly->SetMaximum(sysMax);
    SystematicPoly->Draw("colz");

    theCanvasShapeSystematic->SaveAs((std::string(theCanvasShapeSystematic->GetName()) + "_" + std::to_string(year) + "_minEntries_" + std::to_string(int(minEntriesPerRectangle))+ ".png").data());

    TFile outputFile((std::string(theCanvasShapeSystematic->GetName()) + "_" + std::to_string(year) + "_minEntries_" + std::to_string(int(minEntriesPerRectangle))+ ".root").data(),"RECREATE");
    FourBtagPoly->Write();
    ThreeBtagPoly->Write();
    SystematicPolyNo4Bstat->Write();
    SystematicPoly->Write();
    outputFile.Close();
    
}

void doMeasureShape(float minEntriesPerRectangle, std::string tagName, int year)
{
    gROOT->SetBatch(true);
    std::string inputFileName = "DataPlots_" + tagName + "/outPlotter.root";
    MeasureBackgroundSystematicShape( inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        minEntriesPerRectangle, year);
    gROOT->SetBatch(false);
}


template<typename Hist>
std::tuple<Hist*, Hist*> dividePlots(const Hist* referencePlot, const Hist* inputPlot)
{
    std::string ratioPlotName = std::string(inputPlot->GetName()) + "_ratio";
    std::string errorPlotName = ratioPlotName + "Error";
    int numberOfBins = inputPlot->GetNbinsX();
    Hist* ratioPlot  = (Hist*)inputPlot->Clone(ratioPlotName.data());
    Hist* ratioError = (Hist*)inputPlot->Clone(errorPlotName.data());
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


void MeasureBackgroundSystematicNormalization(std::string inputFileName, std::string FourBtagHistogramName, std::string ThreeBtagHistogramName, int year)
{
    auto histogramList =  getHistogramList<TH1F>(inputFileName, FourBtagHistogramName, ThreeBtagHistogramName);
    auto FourBtagHistogram  = histogramList[0];
    auto ThreeBtagHistogram = histogramList[1];

    FourBtagHistogram->Rebin(2);
    ThreeBtagHistogram->Rebin(2);
    
    TCanvas *theCanvasNormalization = new TCanvas("NormalizationSyst","NormalizationSyst", 1000,800);
    auto theRatioPlots = dividePlots(FourBtagHistogram, ThreeBtagHistogram);
    TH1F *ratio = std::get<0>(theRatioPlots);
    TH1F *ratioError = std::get<1>(theRatioPlots);
    ratio->SetTitle("Background Normalization Error");
    ratio->SetLineColor(kRed);
    ratio->SetLineWidth(2);
    ratio->SetMarkerColor(kRed);
    ratio->SetMinimum(0.9);  // Define Y ..
    ratio->SetMaximum(1.2); // .. range
    ratio->SetStats(0);      // No statistics on lower plot
    // ratio->Divide(referenceHistogram);
    ratio->SetMarkerStyle(21);
    ratio->SetMarkerSize(0.3);

    ratio->SetAxisRange(90.,160.);
    ratio->Draw("ep");       // Draw the ratio plot
    ratioError->Draw("same E2");
    // Y axis ratio plot settings
    ratio->GetYaxis()->SetTitle("ratio");
    ratio->GetXaxis()->SetTitle("m_{Hreco} [GeV]");

    auto theLegend = new TLegend(0.2,0.75,0.8,0.88);
    theLegend->SetNColumns(2);
    theLegend->SetTextSize(0.05);
    theLegend->AddEntry(ratio     ,"bkg model"  , "pl");
    theLegend->AddEntry(ratioError,"4b-tag unc.", "f" );
    theLegend->Draw("same");
    theCanvasNormalization->SaveAs((std::string(theCanvasNormalization->GetName()) + "_" + std::to_string(year) + ".png").data());


    TH1F * ratioDeviationHistogram = new TH1F("Ratio deviation", "Ratio deviation", 12, -0.1, 0.1);

    float maxDeviation = -1.;
    float minBinContent = 1e20;
    for(int bin=1; bin<ratio->GetNbinsX(); ++bin )
    {
        if(ratio->GetBinContent(bin)==0.) continue;
        float currentDeviation = ratio->GetBinContent(bin) - 1.;
        if(abs(currentDeviation)>maxDeviation) maxDeviation = abs(currentDeviation);
        float currentBinContent = FourBtagHistogram->GetBinContent(bin);
        if(currentBinContent<minBinContent) minBinContent = currentBinContent;
        ratioDeviationHistogram->Fill(currentDeviation);
    }
    std::cout<< "Maximum normalization deviation = " << maxDeviation*100 <<"%"<<std::endl;
    std::cout<< "Miniumum bin content = " << minBinContent<<std::endl;
    
    TCanvas *theCanvasratioDeviation = new TCanvas("NormalizationratioDeviationSyst","NormalizationratioDeviationSyst", 1000,800);
    ratioDeviationHistogram->Fit("gaus");
    ratioDeviationHistogram->GetXaxis()->SetTitle("1-ratio");
    gStyle->SetOptFit();
    ratioDeviationHistogram->Draw();
    theCanvasratioDeviation->SaveAs((std::string(theCanvasratioDeviation->GetName()) + "_" + std::to_string(year) + ".png").data());

    return;
}

void doMeasureNorm(std::string tagName, int year)
{
    gROOT->SetBatch(true);
    std::string inputFileName = "DataPlots_" + tagName + "/outPlotter.root";
    
    gROOT->ForceStyle();
    MeasureBackgroundSystematicNormalization(inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_H1_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_H1_m", year);
    gROOT->SetBatch(false);
}

void MeasureBackgroundSystematicCRhole(TVirtualPad *theCanvas, std::string inputFileName, std::string FourBtagHistogramName, std::string ThreeBtagHistogramName, std::string ThreeBtagHistogramHoleName,float CRcutMin, float CRcutMax, int rebinNumber, int year)
{
    auto histogramList =  getHistogramList<TH2F>(inputFileName, FourBtagHistogramName, ThreeBtagHistogramName, ThreeBtagHistogramHoleName);
    auto FourBtagHistogram      = histogramList[0];
    auto ThreeBtagHistogram     = histogramList[1];
    auto ThreeBtagHistogramHole = histogramList[2];

    int yBinMin, yBinMax;
    
    yBinMin = FourBtagHistogram->GetYaxis()->FindBin(CRcutMin);
    if(FourBtagHistogram->GetYaxis()->GetBinLowEdge(yBinMin) < CRcutMin) --yBinMin;
    yBinMax = FourBtagHistogram->GetYaxis()->FindBin(CRcutMax);
    if(FourBtagHistogram->GetYaxis()->GetBinLowEdge(yBinMax) + FourBtagHistogram->GetYaxis()->GetBinWidth(yBinMax) > CRcutMax) --yBinMin;

    TH1D* FourBtagHistogramProjection      = FourBtagHistogram     ->ProjectionX("FourBtagHistogramProjection"     , yBinMin, yBinMax);
    TH1D* ThreeBtagHistogramProjection     = ThreeBtagHistogram    ->ProjectionX("ThreeBtagHistogramProjection"    , yBinMin, yBinMax);
    TH1D* ThreeBtagHistogramHoleProjection = ThreeBtagHistogramHole->ProjectionX("ThreeBtagHistogramHoleProjection", yBinMin, yBinMax);

    ThreeBtagHistogramProjection    ->Scale(FourBtagHistogramProjection->Integral(0,9999999)/ThreeBtagHistogramProjection    ->Integral(0,9999999));
    ThreeBtagHistogramHoleProjection->Scale(FourBtagHistogramProjection->Integral(0,9999999)/ThreeBtagHistogramHoleProjection->Integral(0,9999999));

    FourBtagHistogramProjection     ->Rebin(rebinNumber);
    ThreeBtagHistogramProjection    ->Rebin(rebinNumber);
    ThreeBtagHistogramHoleProjection->Rebin(rebinNumber);

    TCanvas *theCanvasNormalization = new TCanvas("CRHoleSyst","CRHoleSyst", 1000,800);
    auto theRatioPlots     = dividePlots(FourBtagHistogramProjection, ThreeBtagHistogramProjection    );
    auto theRatioHolePlots = dividePlots(FourBtagHistogramProjection, ThreeBtagHistogramHoleProjection);
    auto *ratio     = std::get<0>(theRatioPlots);
    auto *ratioHole = std::get<0>(theRatioHolePlots);
    auto *ratioError = std::get<1>(theRatioPlots);
    ratio->SetTitle("Background Normalization Error");
    ratio->SetMinimum(0.5);  // Define Y ..
    ratio->SetMaximum(1.7); // .. range
    ratio->SetStats(0);      // No statistics on lower plot
    // ratio->Divide(referenceHistogram);
    ratio->SetLineColor(kRed);
    ratio->SetLineWidth(2);
    ratio->SetMarkerColor(kRed);
    ratio->SetMarkerStyle(21);
    ratio->SetMarkerSize(0.3);
    ratio->Draw("ep");       // Draw the ratio plot

    ratioHole->SetLineColor(kBlue);
    ratioHole->SetLineWidth(2);
    ratioHole->SetMarkerColor(kBlue);
    ratioHole->SetMarkerStyle(21);
    ratioHole->SetMarkerSize(0.3);
    ratioHole->Draw("same");       // Draw the ratio plot

    ratioError->Draw("same E2");
    // Y axis ratio plot settings
    ratio->GetYaxis()->SetTitle("ratio");
    ratio->GetXaxis()->SetTitle("m_{Xreco} [GeV]");

    auto theLegend = new TLegend(0.2,0.75,0.8,0.88);
    theLegend->SetNColumns(3);
    theLegend->SetTextSize(0.03);
    theLegend->AddEntry(ratio     ,"bkg model"     , "pl");
    theLegend->AddEntry(ratioHole ,"bkg model hole", "pl");
    theLegend->AddEntry(ratioError,"4b-tag unc."   , "f" );
    theLegend->Draw("same");
    theCanvasNormalization->SaveAs((std::string(theCanvasNormalization->GetName()) + "_" + std::to_string(int(CRcutMin)) + "_" + std::to_string(int(CRcutMax)) + ".png").data());
    delete theCanvasNormalization;

    TH1F *ratioDeviationHistogram     = new TH1F("ratio Deviation", ("ratio Deviation - CR hole from " + std::to_string(int(CRcutMin)) + " to " + std::to_string(int(CRcutMax)) + " GeV").data(), 15, -1., 1.);
    TH1F *ratioHoleDeviationHistogram = new TH1F("ratioHole Deviation", "ratio Deviation", 15, -1., 1.);
    ratioDeviationHistogram    ->SetLineColor(kRed);
    ratioHoleDeviationHistogram->SetLineColor(kBlue);


    for(int bin=1; bin<ratio->GetNbinsX(); ++bin )
    {
        if(ratio->GetBinContent(bin)==0.) continue;
        float currentDeviation     = ratio    ->GetBinContent(bin) - 1.;
        float currentDeviationHole = ratioHole->GetBinContent(bin) - 1.;
        ratioDeviationHistogram    ->Fill(currentDeviation    );
        ratioHoleDeviationHistogram->Fill(currentDeviationHole);
    }

    float ratioMax      = ratioDeviationHistogram    ->GetMaximum();
    float rationHoleMax = ratioHoleDeviationHistogram->GetMaximum();
    float theMax = rationHoleMax>ratioMax ? rationHoleMax : ratioMax;
    ratioDeviationHistogram->SetMaximum(theMax*1.4);
    
    // TCanvas *theCanvasratioDeviation = new TCanvas("NormalizationratioDeviationSyst","NormalizationratioDeviationSyst", 1000,800);
    theCanvas->cd();
    TF1 *theFirstGaus = new TF1("theFirstGaus","gaus", -2,2);
    theFirstGaus->SetLineColor(kRed);
    ratioDeviationHistogram->Fit(theFirstGaus,"0");
    ratioDeviationHistogram->SetStats(0);
    ratioDeviationHistogram    ->Draw();
    ratioDeviationHistogram->GetXaxis()->SetTitle("1-ratio");
    theFirstGaus->Draw("same");
    // gStyle->SetOptFit();
    // TPaveText *st = (TPaveText*)ratioDeviationHistogram->FindObject("stats");
    // st->SetX1NDC(.15);
    // st->SetX2NDC(.5);
    TF1 *theSecondGaus = new TF1("theSecondGaus","gaus", -2,2);
    theSecondGaus->SetLineColor(kBlue);
    ratioHoleDeviationHistogram->Fit(theSecondGaus,"0");
    ratioHoleDeviationHistogram->Draw("same");
    theSecondGaus->Draw("same");

    auto theRatioLegend = new TLegend(0.2,0.75,0.8,0.88);
    theRatioLegend->SetNColumns(2);
    theRatioLegend->SetTextSize(0.05);
    theRatioLegend->AddEntry(ratioDeviationHistogram    ,"bkg model"  , "l");
    theRatioLegend->AddEntry(ratioHoleDeviationHistogram,"bkg model CR hole", "l" );
    theRatioLegend->Draw("same");

    float ratioMean          = theFirstGaus ->GetParameter(1);
    float ratioMeanError     = theFirstGaus ->GetParError(1);
    float ratioHoleMean      = theSecondGaus->GetParameter(1);
    float ratioHoleMeanError = theSecondGaus->GetParError(1);
    float tValue = abs(ratioMean - ratioHoleMean) / sqrt(ratioHoleMean*ratioHoleMean + ratioHoleMeanError*ratioHoleMeanError);

    std::cout << "Hole in CR: from " << CRcutMin << " to " << CRcutMax << " GeV" << std::endl;
    std::cout << "t student between means = " << tValue << std::endl;

    ratioDeviationHistogram->SetTitle( ( std::to_string(int(CRcutMin)) + " to " + std::to_string(int(CRcutMax)) + " GeV - t Student = " + std::to_string(tValue)).data() );

    // theCanvasratioDeviation->SaveAs((std::string(theCanvasratioDeviation->GetName()) + "_" + std::to_string(int(CRcutMin)) + "_" + std::to_string(int(CRcutMax)) + ".png").data());
    // delete theCanvasratioDeviation;

}

void doMeasureCRhole(std::string tagName, int year)
{
    gROOT->ForceStyle();
    gROOT->SetBatch(true);
    std::string canvasName = "Systematic_CRHole_" + std::to_string(year);
    std::string inputFileName = "DataPlots_" + tagName + "/outPlotter.root";
    
    TCanvas *theCanvas = new TCanvas(canvasName.data(), canvasName.data(), 1400, 800);
    theCanvas->DivideSquare(6,0.005,0.005);

    
    MeasureBackgroundSystematicCRhole(theCanvas->cd(1), inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit_VRcutForSyst_65_105/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_VRcutForSyst_65_105_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        65, 105, 4, year);
    MeasureBackgroundSystematicCRhole(theCanvas->cd(2), inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit_VRcutForSyst_145_185/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_VRcutForSyst_145_185_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        145, 185, 4, year);
    MeasureBackgroundSystematicCRhole(theCanvas->cd(3), inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit_VRcutForSyst_185_225/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_VRcutForSyst_185_225_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        185, 225, 4, year);
    MeasureBackgroundSystematicCRhole(theCanvas->cd(4), inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit_VRcutForSyst_225_265/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_VRcutForSyst_225_265_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        225, 265, 4, year);
    MeasureBackgroundSystematicCRhole(theCanvas->cd(5), inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit_VRcutForSyst_265_305/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_VRcutForSyst_265_305_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        265, 305, 4, year);
    MeasureBackgroundSystematicCRhole(theCanvas->cd(6), inputFileName,
        "data_BTagCSV/selectionbJets_ValidationRegionBlinded/data_BTagCSV_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        "data_BTagCSV_dataDriven_kinFit_VRcutForSyst_305_345/selectionbJets_ValidationRegionBlinded/data_BTagCSV_dataDriven_kinFit_VRcutForSyst_305_345_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m",
        305, 345, 4, year);

    theCanvas->SaveAs((std::string(theCanvas->GetName()) + ".png").data());
    
    gROOT->SetBatch(false);
}

void measureAllBkgSystematics(std::string tagName, int year)
{
    gROOT->SetBatch(true);
    doMeasureShape( 700, tagName, year);
    // doMeasureShape( 800, tagName, year);
    // doMeasureShape( 900, tagName, year);
    // doMeasureShape(1000, tagName, year);
    // doMeasureShape(1200, tagName, year);
    // doMeasureShape(1500, tagName, year);
    doMeasureNorm(tagName, year);
    // doMeasureCRhole(tagName, year);
    gROOT->SetBatch(false);
}

void applySingleBackgroundShapeVariations(std::string plotterFileName, std::string datasetName, std::string selectionName, std::string variable, float minEntriesPerRectangle, int year)
{

    std::string variationSystematicFileName = "CanvasShapeSystematic_" + std::to_string(year) + "_minEntries_" + std::to_string(int(minEntriesPerRectangle)) + ".root";
    std::string variationSystematicPlotName      = "SystematicPolyNo4Bstat";
    std::string variationSystematicPlot4bUncName = "SystematicPoly";

    TFile variationSystematicFile(variationSystematicFileName.data());
    auto variationSystematicPlot      = getHistogramFromFile<TH2Poly>(variationSystematicFile, variationSystematicPlotName     );
    auto variationSystematicPlot4bUnc = getHistogramFromFile<TH2Poly>(variationSystematicFile, variationSystematicPlot4bUncName);
    variationSystematicFile.Close();

    TFile plotterFile(plotterFileName.data(),"UPDATE");
    std::string inputHistogramPath = datasetName +  "/" + selectionName + "/";
    std::string inputHistogramName = datasetName +  "_" + selectionName + "_" + variable;
    std::string inputHistogramFullName = inputHistogramPath + inputHistogramName;
    std::string variationNamePrototype = "_BKG_minEntries_" + std::to_string(int(minEntriesPerRectangle));
    std::string variationNameUp        = variationNamePrototype + "_up"        ;
    std::string variationNameDown      = variationNamePrototype + "_down"      ;
    std::string variationName4bUncUp   = variationNamePrototype + "_4bUnc_up"  ;
    std::string variationName4bUncDown = variationNamePrototype + "_4bUnc_down";

    auto inputHistogramFull = getHistogramFromFile<TH2F>(plotterFile, inputHistogramFullName);

    TH2F* variationHistogramUp        = (TH2F*)inputHistogramFull->Clone( (inputHistogramName+variationNameUp       ).data() );
    TH2F* variationHistogramDown      = (TH2F*)inputHistogramFull->Clone( (inputHistogramName+variationNameDown     ).data() );
    TH2F* variationHistogram4bUncUp   = (TH2F*)inputHistogramFull->Clone( (inputHistogramName+variationName4bUncUp  ).data() );
    TH2F* variationHistogram4bUncDown = (TH2F*)inputHistogramFull->Clone( (inputHistogramName+variationName4bUncDown).data() );


    for(int nBinX = 1; nBinX<=inputHistogramFull->GetNbinsX(); ++nBinX)
    {
        float xValue = inputHistogramFull->GetXaxis()->GetBinCenter(nBinX);
        for(int nBinY = 1; nBinY<=inputHistogramFull->GetNbinsY(); ++nBinY)
        {
            float yValue      = inputHistogramFull->GetYaxis()->GetBinCenter(nBinY);
            float zValue      = inputHistogramFull->GetBinContent(nBinX,nBinY);
            float zValueError = inputHistogramFull->GetBinError(nBinX,nBinY);
            int polyBin = variationSystematicPlot->FindBin(xValue, yValue);
            if(polyBin<0) std::cout<<"Bin overflow!!! for "<< xValue <<" - " << yValue<<std::endl;
            float variation      = variationSystematicPlot     ->GetBinContent(polyBin)/100.;
            float variation4bUnc = variationSystematicPlot4bUnc->GetBinContent(polyBin)/100.;
            variationHistogramUp       ->SetBinContent(nBinX, nBinY, zValue      * (1. + variation     ) );
            variationHistogramDown     ->SetBinContent(nBinX, nBinY, zValue      * (1. - variation     ) );
            variationHistogramUp       ->SetBinError  (nBinX, nBinY, zValueError * (1. + variation     ) );
            variationHistogramDown     ->SetBinError  (nBinX, nBinY, zValueError * (1. - variation     ) );
            variationHistogram4bUncUp  ->SetBinContent(nBinX, nBinY, zValue      * (1. + variation4bUnc) );
            variationHistogram4bUncDown->SetBinContent(nBinX, nBinY, zValue      * (1. - variation4bUnc) );
            variationHistogram4bUncUp  ->SetBinError  (nBinX, nBinY, zValueError * (1. + variation4bUnc) );
            variationHistogram4bUncDown->SetBinError  (nBinX, nBinY, zValueError * (1. - variation4bUnc) );
        }
    }

    plotterFile.cd(inputHistogramPath.data());
    variationHistogramUp       ->Write();
    variationHistogramDown     ->Write();
    variationHistogram4bUncUp  ->Write();
    variationHistogram4bUncDown->Write();
    plotterFile.cd();

    plotterFile.Close();
}

void applyAllBackgroundShapeVariations(std::string tagName, int year)
{
    std::vector<int> minBinContentList = {700};
    // std::vector<int> minBinContentList = {700, 800, 900, 1000, 1200, 1500};
    std::string datasetName = "data_BTagCSV_dataDriven_kinFit";
    std::string selectionName = "selectionbJets_SignalRegion";
    std::string variable = "HH_kinFit_m_H2_m";

    std::string inputFileName = "DataPlots_" + tagName + "/outPlotter.root";

    std::cout<<inputFileName<<std::endl;
    for(auto minBinContent : minBinContentList)
    {
        std::cout<<minBinContent<<std::endl;
        applySingleBackgroundShapeVariations(inputFileName, datasetName, selectionName, variable, minBinContent, year);
    }

}

