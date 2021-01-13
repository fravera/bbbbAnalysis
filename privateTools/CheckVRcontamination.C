#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TH2Poly.h"


template<typename Hist>
Hist* getHistogramFromFile(TFile& inputFile, std::string histogramName)
{
    Hist* histogram = (Hist*)inputFile.Get(histogramName.data());
    if(histogram == nullptr)
    {
        // std::cout<< "Histogram " << histogramName << " does not exist" << std::endl;
        return nullptr;
    }
    histogram->SetDirectory(0);

    return histogram;
}

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


void fillFromHistogram(TH2Poly* thePoly, TH2F* theInputHistogram)
{
    for(int nBinX = 1; nBinX<=theInputHistogram->GetNbinsX(); ++nBinX)
    {
        float xValue = theInputHistogram->GetXaxis()->GetBinCenter(nBinX);
        for(int nBinY = 1; nBinY<=theInputHistogram->GetNbinsY(); ++nBinY)
        {
            float yValue      = theInputHistogram->GetYaxis()->GetBinCenter(nBinY);
            float zValue      = theInputHistogram->GetBinContent(nBinX,nBinY);
            float zValueError = theInputHistogram->GetBinError(nBinX,nBinY);
            int polyBin = thePoly->FindBin(xValue, yValue);
            if(polyBin<0) std::cout<<"Bin overflow!!!"<<std::endl;
            thePoly->SetBinContent(polyBin, zValue + thePoly->GetBinContent(polyBin));
            // std::cout<<zValue<<std::endl;
            // exit(0);
            thePoly->SetBinError(polyBin, sqrt(zValueError*zValueError + thePoly->GetBinError(polyBin)*thePoly->GetBinError(polyBin)));
        }
    }
}

float getIntegral(TH2Poly* thePoly)
{
    float totalEntries = 0.;
    for(int bin=1; bin<=thePoly->GetNumberOfBins(); ++bin)
    {
        totalEntries += thePoly->GetBinContent(bin);
    }
    return totalEntries;
}

std::vector<float> getBinning(const std::vector<int>& listOfPoints)
{
    int numberOfPoints = listOfPoints.size();
    std::vector<float> binning(numberOfPoints+1);
    for (int point=0; point<numberOfPoints-1; ++point) binning[point+1] = (listOfPoints[point]+listOfPoints[point+1])/2.;
    binning[0]              = 2.*listOfPoints[0] - binning[1];
    binning[numberOfPoints] = 2.*listOfPoints[numberOfPoints-1] - binning[numberOfPoints-1];
    return binning;
}

float getLimit(TH2D* inputPlot, float xMass, float yMass)
{
    int xBin = inputPlot->GetXaxis()->FindBin(xMass);
    int yBin = inputPlot->GetYaxis()->FindBin(yMass);

    return inputPlot->GetBinContent(xBin, yBin);
}

float getRegionEntries(TH1F* hist, std::string binName)
{
    return hist->GetBinContent(hist->GetXaxis()->FindBin(binName.data()));
}

void ProduceVariationList(std::string year, std::string plotInputFileName, std::string limitInputFileName, std::string variationSystematicFileName, int signalStrength)
{

    std::string variationSystematicPlotName = "SystematicPoly";
    std::string fourBEnntriesPlotName  = "FourBtag_entries";

    TFile variationSystematicFile(variationSystematicFileName.data());
    auto variationSystematicPlot = getHistogramFromFile<TH2Poly>(variationSystematicFile, variationSystematicPlotName);
    auto fourBEnntriesPlot       = getHistogramFromFile<TH2Poly>(variationSystematicFile, fourBEnntriesPlotName      );
    variationSystematicFile.Close();

    std::vector<int> listOfXmasses = {300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1400, 1600, 1800, 2000};
    std::vector<int> listOfYmasses = {60, 70, 80, 90, 100, 125, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800};
    
    std::string plotName = "biasFromVRsignalPresence_"+std::string(year);
    TH1F* biasFromVRpresence = new TH1F(plotName.data(), ("Bias from VR signal presence " + std::string(year) + "; BKG shape bias [%]").data(), 100, 0, 30);
    std::string plot2DName = "biasFromVRsignalPresence2D_"+std::string(year);
    auto xBinning = getBinning(listOfXmasses);
    auto yBinning = getBinning(listOfYmasses);
    TH2F* biasFromVRpresence2D = new TH2F(plot2DName.data(), ("Bias from VR signal presence " + std::string(year) + "; m_{X} [GeV]; m_{Y} [GeV]; BKG shape bias [%]").data(), xBinning.size()-1, xBinning.data(), yBinning.size()-1, yBinning.data());
    biasFromVRpresence2D->SetMinimum( 0.);
    biasFromVRpresence2D->SetMaximum(30.);

    std::string plot2DLocalName = "biasFromVRsignalPresence2DLocal_"+std::string(year)+"_signalStrength_"+ std::to_string(signalStrength);
    TH2F* biasFromVRpresence2DLocal = new TH2F(plot2DLocalName.data(), ("Bias to BKG shape syst - signal strenght = " + std::to_string(signalStrength) + " x 95% CL #sigma x BR - year " + std::string(year) + "; m_{X} [GeV]; m_{Y} [GeV]; Max relative BKG shape bias").data(), xBinning.size()-1, xBinning.data(), yBinning.size()-1, yBinning.data());
    biasFromVRpresence2DLocal->SetMinimum( 0.);
    biasFromVRpresence2DLocal->SetMaximum(1.5);

    TFile plotInputFile(plotInputFileName.data());
    std::string backgroundHistogramName = "/data_BTagCSV_dataDriven_kinFit/selectionbJets_SignalRegion/data_BTagCSV_dataDriven_kinFit_selectionbJets_SignalRegion_HH_kinFit_m_H2_m";
    auto backgroundHistogram = getHistogramFromFile<TH2F>(plotInputFile, backgroundHistogramName);
    
    std::string folderName  = "/Limits_" + year + "/Option_syst/";

    TFile limitInputFile(limitInputFileName.data());
    std::string limitCentralMapName = "LimitMapCentral_" + year + "_syst";
    auto limitCentralMap = getHistogramFromFile<TH2D>(limitInputFile, folderName + limitCentralMapName);

    std::vector<float> entryDifferenceList;

    for(auto xMass : listOfXmasses)
    {
        float crossSection = 100.;
        if (xMass >=  600.) crossSection = 10.;
        if (xMass >= 1600.) crossSection = 1.;

        for(auto yMass : listOfYmasses)
        {
            // std::cout<<"####################################################################################################"<<std::endl;
            // std::cout<<"####################################################################################################"<<std::endl;
            // std::cout<<"####################################################################################################"<<std::endl;
            // std::cout<<"####################################################################################################"<<std::endl;
            if( (xMass - yMass) < 125 ) continue;
            if( xMass > 1000 && yMass < 90) continue;

            std::string signalHistogramName_4b = "sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "/sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass);
            std::string signalHistogramName_3b = "sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "_3bScaled/sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "_3bScaled";
            
            auto signalHistogram_4b  = getHistogramFromFile<TH1F>(plotInputFile, signalHistogramName_4b);
            auto signalHistogram_3b  = getHistogramFromFile<TH1F>(plotInputFile, signalHistogramName_3b);
            if(signalHistogram_4b == nullptr || signalHistogram_3b == nullptr ) continue;

            float centralLimit = getLimit(limitCentralMap, xMass, yMass);
            float entriesSignalFourB  = getRegionEntries(signalHistogram_4b, "selectionbJets_ValidationRegionBlinded")*centralLimit*5/crossSection;
            float entriesSignalThreeB = getRegionEntries(signalHistogram_3b, "selectionbJets_ValidationRegionBlinded")*centralLimit*5/crossSection;
            // std::cout<<entriesSignalFourB << " - " << entriesSignalThreeB << std::endl;

            float entriesDifference = entriesSignalFourB - entriesSignalThreeB;
            float errorBias = entriesDifference/600.*100.;
            biasFromVRpresence->Fill(errorBias);
            biasFromVRpresence2D->Fill(xMass, yMass, errorBias);

            entryDifferenceList.emplace_back(errorBias);

            std::string fourBSignalHistogramName = "sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "/selectionbJets_ValidationRegionBlinded/sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m";
            auto fourBSignalxMyMHistogram  = getHistogramFromFile<TH2F>(plotInputFile, fourBSignalHistogramName);
            fourBSignalxMyMHistogram->Scale(float(signalStrength)*centralLimit/crossSection);
            std::string threeBSignalHistogramName = "sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "_3bScaled/selectionbJets_ValidationRegionBlinded/sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "_3bScaled_selectionbJets_ValidationRegionBlinded_HH_kinFit_m_H2_m";
            auto threeBSignalxMyMHistogram  = getHistogramFromFile<TH2F>(plotInputFile, threeBSignalHistogramName);
            threeBSignalxMyMHistogram->Scale(float(signalStrength)*centralLimit/crossSection);

            auto fourBEnntriesPlotPlus4bsig = static_cast<TH2Poly*>(fourBEnntriesPlot->Clone("fourBEnntriesPlotPlus4bsig"));
            auto fourBEnntriesPlotPlus3bsig = static_cast<TH2Poly*>(fourBEnntriesPlot->Clone("fourBEnntriesPlotPlus3bsig"));
            // std::cout<<getIntegral(fourBEnntriesPlotPlus4bsig)<< " - " << getIntegral(fourBEnntriesPlotPlus3bsig) << std::endl;
            fillFromHistogram(fourBEnntriesPlotPlus4bsig, fourBSignalxMyMHistogram );
            fillFromHistogram(fourBEnntriesPlotPlus3bsig, threeBSignalxMyMHistogram);
            // std::cout<<getIntegral(fourBEnntriesPlotPlus4bsig)<< " - " << getIntegral(fourBEnntriesPlotPlus3bsig) << std::endl;

            std::vector<float> ratioWtrNoSignal;
            for(int bin=1; bin<=fourBEnntriesPlotPlus4bsig->GetNumberOfBins(); ++bin)
            {
                float deviationPlusSignal = (fourBEnntriesPlotPlus4bsig->GetBinContent(bin) - fourBEnntriesPlotPlus3bsig->GetBinContent(bin))/fourBEnntriesPlotPlus4bsig->GetBinContent(bin);
                float totalDeviation = variationSystematicPlot->GetBinContent(bin)/100.;
                float deviationContribution = deviationPlusSignal/totalDeviation;
                // std::cout<<deviationPlusSignal<<" - "<<totalDeviation<< " - " <<deviationContribution<<std::endl;
                ratioWtrNoSignal.emplace_back(deviationContribution);
            }


            biasFromVRpresence2DLocal->Fill(xMass, yMass, *std::max_element(ratioWtrNoSignal.begin(), ratioWtrNoSignal.end()));

        }
    }

    plotInputFile.Close();
    limitInputFile.Close();

    TCanvas* cBiasFromVRpresence = new TCanvas(plotName.data(), plotName.data());
    biasFromVRpresence->Draw();
    cBiasFromVRpresence->SaveAs((plotName + ".png").data());
    delete biasFromVRpresence;
    delete cBiasFromVRpresence;

    TCanvas* cBiasFromVRpresence2D = new TCanvas(plot2DName.data(), plot2DName.data());
    biasFromVRpresence2D->Draw("colz");
    biasFromVRpresence2D->SetStats(0);
    cBiasFromVRpresence2D->SaveAs((plot2DName + ".png").data());
    delete biasFromVRpresence2D;
    delete cBiasFromVRpresence2D;


    TCanvas* cBiasFromVRpresence2DLocal = new TCanvas(plot2DLocalName.data(), plot2DLocalName.data());
    // int bnpx = cBiasFromVRpresence2DLocal->GetWindowWidth();
    // int bnpy = cBiasFromVRpresence2DLocal->GetWindowHeight();
    // float ratio = 0.5;
    // cBiasFromVRpresence2DLocal->SetWindowSize(bnpx*ratio, bnpy*ratio);
    cBiasFromVRpresence2DLocal->SetRightMargin(0.15);
    cBiasFromVRpresence2DLocal->SetLeftMargin(0.15);
    cBiasFromVRpresence2DLocal->SetTopMargin(0.1);
    cBiasFromVRpresence2DLocal->SetBottomMargin(0.1);
    biasFromVRpresence2DLocal->Draw("colz");
    biasFromVRpresence2DLocal->GetXaxis()->SetLabelSize(0.035);
    biasFromVRpresence2DLocal->GetYaxis()->SetLabelSize(0.035);
    biasFromVRpresence2DLocal->SetStats(0);
    cBiasFromVRpresence2DLocal->SaveAs((plot2DLocalName + ".png").data());
    delete biasFromVRpresence2DLocal;
    delete cBiasFromVRpresence2DLocal;

    std::cout << *std::max_element(entryDifferenceList.begin(), entryDifferenceList.end()) << std::endl;;

}

void ProduceAllVariationList(int plotVersion, int limitVersion)
{
    gROOT->SetBatch(true);
    // std::vector<std::string> yearList {"2016"};
    std::vector<std::string> yearList {"2016", "2017", "2018"};
    // std::vector<int> signalStrengthList {1};
    std::vector<int> signalStrengthList {1, 2, 3, 4, 5};

    for(auto year : yearList)
    {
        std::string plotInputFileName  = "DataPlots_fullSubmission_" + year + "_v" + std::to_string(plotVersion) + "/outPlotter.root";
        std::string limitInputFileName = "limits/Limits_fullSubmission_v" + std::to_string(limitVersion) + ".root";
        std::string shapeInputFileName = "CanvasShapeSystematic_" + year + "_minEntries_700.root";
        for(auto signalStrength : signalStrengthList)
        {
            ProduceVariationList(year, plotInputFileName, limitInputFileName, shapeInputFileName, signalStrength);
        }
    }
    gROOT->SetBatch(false);
}