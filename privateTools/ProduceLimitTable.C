#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"


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

std::pair<float, float> getMeanAndSigma(TH1F* theInputPlot, float expectedMass)
{
    float xMean  = expectedMass;
    float xSigma = expectedMass/10.;
    TF1 gaussianFunction("guassianFunction", "gaus", expectedMass - 2. * xSigma, expectedMass + 2. * xSigma);
    gaussianFunction.SetParameter(1,expectedMass);
    gaussianFunction.SetParameter(2,expectedMass/10.);
    theInputPlot->Fit(&gaussianFunction,"0QR");
    xMean  = gaussianFunction.GetParameter(1);
    xSigma = gaussianFunction.GetParameter(2);
    gaussianFunction.SetRange(xMean - xSigma, xMean + xSigma);
    theInputPlot->Fit(&gaussianFunction,"0QR");
    xMean  = gaussianFunction.GetParameter(1);
    xSigma = gaussianFunction.GetParameter(2);
    auto theMeanAndSigmaPair = std::make_pair(xMean, xSigma);
    return theMeanAndSigmaPair;
}

float getEntriesInRectangle(TH2F* inputPlot, float xCenter, float xHalfSize, float yCenter, float yHalfSize)
{
    auto xAxis = inputPlot->GetXaxis();
    auto yAxis = inputPlot->GetYaxis();

    int xMinBin = xAxis->FindBin(xCenter - xHalfSize);
    int xMaxBin = xAxis->FindBin(xCenter + xHalfSize);

    int yMinBin = yAxis->FindBin(yCenter - yHalfSize);
    int yMaxBin = yAxis->FindBin(yCenter + yHalfSize);

    float numberOfEntries = 0.;

    for(int xBin = xMinBin; xBin <= xMaxBin; ++xBin )
    {
        for(int yBin = yMinBin; yBin <= yMaxBin; ++yBin )
        {
            numberOfEntries += inputPlot->GetBinContent(xBin, yBin);
        }
    }

    return numberOfEntries;
}

float getLimit(TH2D* inputPlot, float xMass, float yMass)
{
    int xBin = inputPlot->GetXaxis()->FindBin(xMass);
    int yBin = inputPlot->GetYaxis()->FindBin(yMass);

    return inputPlot->GetBinContent(xBin, yBin);
}

void ProduceLimitTable(std::string year, std::string plotInputFileName, std::string limitInputFileName)
{
    float numberOfSigmas = 2.;

    std::vector<int> listOfXmasses = {300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1400, 1600, 1800, 2000};
    std::vector<int> listOfYmasses = {60, 70, 80, 90, 100, 125, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800};

    TFile plotInputFile(plotInputFileName.data());
    std::string backgroundHistogramName = "/data_BTagCSV_dataDriven_kinFit/selectionbJets_SignalRegion/data_BTagCSV_dataDriven_kinFit_selectionbJets_SignalRegion_HH_kinFit_m_H2_m";
    auto backgroundHistogram = getHistogramFromFile<TH2F>(plotInputFile, backgroundHistogramName);
    
    std::string folderName  = "/Limits_" + year + "/Option_syst/";

    TFile limitInputFile(limitInputFileName.data());
    std::string limitCentralMapName = "LimitMapCentral_" + year + "_syst";
    auto limitCentralMap = getHistogramFromFile<TH2D>(limitInputFile, folderName + limitCentralMapName);

    std::string limitMap1sigmaUpName = "LimitMap1sigmaUp_" + year + "_syst";
    auto LimitMap1sigmaUp = getHistogramFromFile<TH2D>(limitInputFile, folderName + limitMap1sigmaUpName);

    std::string limitMap1sigmaDownName = "LimitMap1sigmaDown_" + year + "_syst";
    auto LimitMap1sigmaDown = getHistogramFromFile<TH2D>(limitInputFile, folderName + limitMap1sigmaDownName);


    for(auto xMass : listOfXmasses)
    {
        float crossSection = 100.;
        if (xMass >=  600.) crossSection = 10.;
        if (xMass >= 1600.) crossSection = 1.;

        std::cout<<"\\begin{table}[htb!]"<<std::endl;
        std::cout<<"\\centering"<<std::endl;
        if(year!="RunII")
        {
            std::cout<<"\\begin{tabular}{c|c|c|c}"<<std::endl;
            std::cout<<"\\mY [GeV]  & signal events [fb$^{-1}$] & background events & 95\\% C.L. limit [fb] \\\\"<<std::endl;
        }
        else
        {
            std::cout<<"\\begin{tabular}{c|c}"<<std::endl;
            std::cout<<"\\mY [GeV]  & 95\\% C.L. limit [fb] \\\\"<<std::endl;
        }
        
        for(auto yMass : listOfYmasses)
        {
            if( (xMass - yMass) < 125 ) continue;
            if( xMass > 1000 && yMass < 90) continue;

            float entriesInBackground = -1;
            float entriesInSignal     = -1;
            if(year!="RunII")
            {
                std::string xMassVariable = "HH_kinFit_m";
                std::string yMassVariable = "H2_m";
                std::string xMyMVariable  = "HH_kinFit_m_H2_m";
                std::string signalHistogramNamePrototype = "sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "/selectionbJets_SignalRegion/sig_NMSSM_bbbb_MX_" + std::to_string(xMass) + "_MY_" + std::to_string(yMass) + "_selectionbJets_SignalRegion_";
                auto signalxMyMHistogram  = getHistogramFromFile<TH2F>(plotInputFile, signalHistogramNamePrototype + xMyMVariable );
                auto signalxMassHistogram = getHistogramFromFile<TH1F>(plotInputFile, signalHistogramNamePrototype + xMassVariable);
                auto signalyMassHistogram = getHistogramFromFile<TH1F>(plotInputFile, signalHistogramNamePrototype + yMassVariable);
                if(signalxMyMHistogram == nullptr) continue;

                auto xMeanAndSigma = getMeanAndSigma(signalxMassHistogram, xMass);
                auto yMeanAndSigma = getMeanAndSigma(signalyMassHistogram, yMass);

                if(yMass == 125) yMeanAndSigma = std::make_pair(125.f, 12.5f);
        
                // std::cout<<"Searching X from " <<  xMeanAndSigma.first - xMeanAndSigma.second * numberOfSigmas << " to " << xMeanAndSigma.first + xMeanAndSigma.second * numberOfSigmas <<std::endl;
                // std::cout<<"Searching Y from " <<  yMeanAndSigma.first - yMeanAndSigma.second * numberOfSigmas << " to " << yMeanAndSigma.first + yMeanAndSigma.second * numberOfSigmas <<std::endl;

                entriesInBackground = getEntriesInRectangle(backgroundHistogram, xMeanAndSigma.first, xMeanAndSigma.second * numberOfSigmas, yMeanAndSigma.first, yMeanAndSigma.second * numberOfSigmas);
                entriesInSignal     = getEntriesInRectangle(signalxMyMHistogram, xMeanAndSigma.first, xMeanAndSigma.second * numberOfSigmas, yMeanAndSigma.first, yMeanAndSigma.second * numberOfSigmas)/crossSection;
            }

            float centralLimit = getLimit(limitCentralMap, xMass, yMass);
            float up1SigmaLimit = getLimit(LimitMap1sigmaUp, xMass, yMass);
            float down1SigmaLimit = getLimit(LimitMap1sigmaDown, xMass, yMass);
            float limitDeltaUp     = up1SigmaLimit - centralLimit;
            float limitDeltaDown   = centralLimit - down1SigmaLimit;

            std::cout << std::fixed << std::setprecision(3) << yMass << "\t&\t";
            if(year!="RunII") std::cout << entriesInSignal << "\t&\t" << entriesInBackground << "\t&\t";
            std::cout << centralLimit << "$^{+" << limitDeltaUp << "}_{-" << limitDeltaDown << "}$\t\\\\" << std::endl;
        }

        std::cout<<"\\end{tabular}"<<std::endl;
        std::cout<<"\\caption{\\label{results:tab:"<<year<<"Limits_Mx_"<<xMass<<"} ";
        if(year!="RunII") std::cout << "Number of events per background and signal (normalized for the cross-section time branching fraction) and the corresponding limit for "<<year<< " for \\mX = " << xMass << " GeV signal hypothesis.}"<<std::endl;
        else std::cout << "95\\% C.L. limit for "<<year<< " for \\mX = " << xMass << " GeV signal hypothesis.}"<<std::endl;
        std::cout<<"\\end{table}"<<std::endl<<std::endl<<std::endl;


    }

    plotInputFile.Close();
    limitInputFile.Close();

}

void ProduceAllLimitTable(int plotVersion, int limitVersion)
{
    std::vector<std::string> yearList {"2016", "2017", "2018", "RunII"};

    for(auto year : yearList)
    {
        std::string plotInputFileName  = "DataPlots_fullSubmission_" + year + "_v" + std::to_string(plotVersion) + "/outPlotter.root";
        std::string limitInputFileName = "limits/Limits_fullSubmission_v" + std::to_string(limitVersion) + ".root";
        ProduceLimitTable(year, plotInputFileName, limitInputFileName);
    }
}