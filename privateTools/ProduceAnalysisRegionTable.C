#include "Riostream.h"
#include "TFile.h"
#include "TH1F.h"
#include "TClass.h"
#include "TCollection.h"
#include "TKey.h"
#include "TROOT.h"


std::string getSampleNameFromFile(std::string fileName)
{
    size_t end_pos = fileName.rfind('.');
    size_t start_pos = fileName.rfind('/')+1;
    return fileName.substr(start_pos, end_pos-start_pos);

}

std::pair<int, int> getMxMyFromSample(std::string sampleName)
{
    if(sampleName.size()<4) return std::make_pair(-1, -1);
    std::string mxStringStart = "_MX_";
    std::string mxStringEnd   = "_MY_";
    std::string myStringStart = "_MY_";
    if(sampleName.find(mxStringStart) == std::string::npos) return std::make_pair(-1, -1);
    size_t mxStart = sampleName.find(mxStringStart) + mxStringStart.size();
    size_t mxEnd   = sampleName.find(mxStringEnd);
    size_t myStart = sampleName.find(myStringStart) + myStringStart.size();
    size_t myEnd   = sampleName.size();

    std::string mx = sampleName.substr(mxStart, mxEnd-mxStart);
    std::string my = sampleName.substr(myStart, myEnd-myStart);

    return std::make_pair(atoi(mx.c_str()), atoi(my.c_str()));
}

void ProduceAnalysisRegionTable(int version)
{

    std::vector<std::string> binNameList = {"selectionbJets_ControlRegionBlinded", "selectionbJets_ValidationRegionBlinded", "selectionbJets_SignalRegion"};
    std::vector<int> listOfXmasses = {300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1400, 1600, 1800, 2000};
    std::vector<int> listOfYmasses = {60, 70, 80, 90, 100, 125, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800};
    std::vector<std::string> toSkipList {"_3bScaled", "_bjer_down", "_bjer_up", "_jer_down", "_jer_up", "_jes_Total_down", "_jes_Total_up"};
    std::vector<int> yearList {2016, 2017, 2018};

    std::map<int, std::map<int, std::map<int, std::vector<float>>>> mapEventsInRegions;

    for(auto mX : listOfXmasses)
    {
        for(auto mY : listOfYmasses)
        {
            for(auto year : yearList)
            {
                mapEventsInRegions[mX][mY][year] = {-1, -1, -1};
            }
        }
    }

    for(auto year : yearList)
    {
        std::string fileName = "DataPlots_fullSubmission_" + std::to_string(year) + "_v" + std::to_string(version) + "/outPlotter.root";
        std::cout << "Opening file " << fileName << std::endl;

        TFile theInputFile(fileName.data());

        TIter next(theInputFile.GetListOfKeys());
        TKey *key;
        while ((key = (TKey*)next())) 
        {
            TClass *cl = gROOT->GetClass(key->GetClassName());
            if (!cl->InheritsFrom("TDirectoryFile")) continue;
            std::string theCurrentDataset = key->ReadObj()->GetName();
// std::cout<<__LINE__<<std::endl;
            bool toSkip = false;
            for(const auto& skipName : toSkipList)
            {
                if(theCurrentDataset.find(skipName) != std::string::npos)
                {
                    toSkip=true;
                    break;
                }
            }
// std::cout<<__LINE__<<std::endl;
            if(toSkip) continue;
// std::cout<<__LINE__<<std::endl;

            TH1F* entriesPlot = (TH1F*)theInputFile.Get( (theCurrentDataset + "/" + theCurrentDataset).data() );
// std::cout<<__LINE__<<std::endl;
            

            if(theCurrentDataset=="data_BTagCSV") std::cout << theCurrentDataset << "\t";

            auto masses = getMxMyFromSample(theCurrentDataset);
            
// std::cout<<__LINE__<<std::endl;

            float crossSection = 1.;
            if (masses.first >=  0.) crossSection = 100.;
            if (masses.first >=  600.) crossSection = 10.;
            if (masses.first >= 1600.) crossSection = 1.;
// std::cout<<__LINE__<<std::endl;

            int binCounter = 0;
            for(auto& binName : binNameList)
            {
// std::cout<<__LINE__<<std::endl;
                if(theCurrentDataset=="data_BTagCSV")
                {
// std::cout<<__LINE__<<std::endl;
                    if(binName=="selectionbJets_SignalRegion") std::cout << "\tblinded";
                    else std::cout << std::fixed << std::setprecision(3) << "\t" << (float)entriesPlot->GetBinContent(entriesPlot->GetXaxis()->FindBin(binName.data()))/crossSection;
// std::cout<<__LINE__<<std::endl;
                }
                else if(masses.first>0)
                {
// std::cout<<__LINE__<<std::endl;
// std::cout<<masses.first << " " << masses.second << " " << year << " " << binCounter<<std::endl;
                    mapEventsInRegions[masses.first][masses.second][year][binCounter++] = (float)entriesPlot->GetBinContent(entriesPlot->GetXaxis()->FindBin(binName.data()))/crossSection;
// std::cout<<__LINE__<<std::endl;
                }
                
            }
            if(theCurrentDataset=="data_BTagCSV") std::cout<<endl;
        }
        theInputFile.Close();
    }


    for(auto mXTable : mapEventsInRegions)
    {

        std::cout <<"\\begin{table}[htb]" << std::endl;
        std::cout <<"\\centering" << std::endl;
        std::cout <<"\\begin{tabular}{l| c| c| c| c| c| c| c| c| c}" << std::endl;
        std::cout <<"\\mY [GeV]  \t & \\multicolumn{3}{c|}{2016}  & \\multicolumn{3}{c|}{2017}  & \\multicolumn{3}{c}{2018}   \\\\    " << std::endl;
        std::cout <<"\\hline" << std::endl;
        std::cout <<"\t & CR & VR & SR & CR & VR & SR & CR & VR & SR \\\\" << std::endl;
        std::cout <<"\\hline" << std::endl;
        for(auto mYLine : mXTable.second)
        {
            if(mXTable.first - mYLine.first < 125) continue;
            if(mXTable.first > 1000 && mYLine.first < 90) continue;
            std::cout << mYLine.first;

            for(auto yearLine : mYLine.second)
            {
                for(auto entries : yearLine.second)
                {
                    std::cout << "\t & \t" << entries;
                }
            }
            std::cout << "\t \\\\" << std::endl;
        }

        std::cout <<"\\end{tabular}" << std::endl;
        std::cout <<"\\caption{\\label{event_selection:tab:EventCountMx" << mXTable.first << "} Number of events in the analysis regions for signals with \\mX = " << mXTable.first << "~GeV. Events are normalized on the signal cross-section times BR and are expressed in 1$/$fb.}" << std::endl;
        std::cout <<"\\end{table}" << std::endl << std::endl;
    }

}
