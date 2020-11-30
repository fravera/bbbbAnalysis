#include "Riostream.h"
#include "TH1F.h"
#include "TFile.h"
#include "TROOT.h"
#include "TCanvas.h"
#include <vector>
#include <map>
#include <unordered_map>
#include "TTree.h"
#include "TChain.h"
#include "TColor.h"
#include "TTreeFormula.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TGraph.h"
#include "TLegend.h"
#include <future>

// cd privateTools && g++  -std=c++17 -g -I `root-config --incdir` -o PlotCutFlow PlotCutFlow.cc `root-config --libs` -O3; cd ..

int numberOfThreads = 4;

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


std::vector<std::string> splitByLine(const std::string& inputFileName)
{
    std::vector<std::string> fileNameList;
    std::ifstream fList (inputFileName);
    if (!fList.good())
    {
        std::cerr << "*** Sample::openFileAndTree : ERROR : could not open file " << inputFileName << std::endl;
        abort();
    }
    std::string line;
    while (std::getline(fList, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) fileNameList.emplace_back(line);
    }
    return fileNameList;
}

std::shared_ptr<TChain> getChainFromFileList(std::vector<std::string>& fileNameList, const std::string& treeName)
{
    std::shared_ptr<TChain> theChain = std::make_shared<TChain>(treeName.c_str());
    for(const auto& fileName : fileNameList) theChain->Add(fileName.c_str());

    return std::move(theChain);
}

std::string getSampleNameFromFile(std::string fileName)
{
    size_t end_pos = fileName.rfind('.');
    size_t start_pos = fileName.rfind('/')+1;
    return fileName.substr(start_pos, end_pos-start_pos);

}

std::pair<std::string, std::string> getMxMyFromSample(std::string sampleName)
{
    std::string mxStringStart = "_MX_";
    std::string mxStringEnd   = "_NANOAOD_";
    std::string myStringStart = "_MY_";
    size_t mxStart = sampleName.find(mxStringStart) + mxStringStart.size();
    size_t mxEnd   = sampleName.find(mxStringEnd);
    size_t myStart = sampleName.find(myStringStart) + myStringStart.size();
    size_t myEnd   = sampleName.size();

    std::string mx = sampleName.substr(mxStart, mxEnd-mxStart);
    std::string my = sampleName.substr(myStart, myEnd-myStart);

    return std::make_pair(mx, my);
}


class CutFlowSelection
{
    public:
        CutFlowSelection(){};
        CutFlowSelection(int year)
        : fYear(year)
        {};


        using CutInfo = std::tuple<std::string, std::string, std::string, Color_t>;
        
    private:
        int fYear;
        std::string triggerSelection           = "( HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 || HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 )";
        std::string selectionbJetsPt           = "H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30";
        std::string selectionbJetsPtRegressed  = "H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30";
        std::string selectionbJetsEta          = "H1_b1_eta > -2.4 && H1_b1_eta < 2.4 && H1_b2_eta > -2.4 && H1_b2_eta < 2.4 && H2_b1_eta > -2.4 && H2_b1_eta < 2.4 && H2_b2_eta > -2.4 && H2_b2_eta < 2.4";
        std::string vetoIsoLeptons             = "IsolatedElectron_pt<15 && IsolatedMuon_pt<10";
        std::string preselectionCut            = selectionbJetsPt + " && " + selectionbJetsPtRegressed + " && " + selectionbJetsEta + " && " + vetoIsoLeptons;
        std::string fourBtaggedJets            = "NbJets >= 4 ";
        std::string triggerMatching            = "( HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched>0 || HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)";
        std::string signalRegionSelection      = "H1_m > 125-20 && H1_m < 125+20";
        std::shared_ptr<TTreeFormula> fTriggerFormula;
        std::shared_ptr<TTreeFormula> fPreselectionFormula;
        std::shared_ptr<TTreeFormula> fFourBjetFormula;
        std::shared_ptr<TTreeFormula> fTriggerMatchingFormula;
        std::shared_ptr<TTreeFormula> fSignalRegionFormula;
        std::shared_ptr<TTreeReaderValue<float>> fTriggerSFReader;
        std::shared_ptr<TTreeReaderValue<float>> fBtagSFReader;

        std::map<int, std::pair<std::string, std::string>> fTriggerCut
        {
            {2016, {"(HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 || HLT_QuadJet45_TripleBTagCSV_p087_Fired==1)","(HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched>0 || HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched > 0)"}},
            {2017, {"(HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_Fired==1 || HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_Fired==1)","(HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_ObjectMatched>0 || HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07_ObjectMatched > 0)"}},
            {2018, {"HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_Fired==1", "HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_ObjectMatched>0"}}
        };

    public:
        std::vector<CutInfo> getCutFlowList()
        {
            std::vector<CutInfo> outputVector {
                {"Trigger"                       , fTriggerCut[fYear].first .c_str(), "None"                   , kBlack },
                {"Preselection (4jets, 3 b-jets)", preselectionCut          .c_str(), "None"                   , kGreen },
                {"4-btagged jets"                , fourBtaggedJets          .c_str(), "bTagScaleFactor_central", kRed   },
                {"Trigger matching"              , fTriggerCut[fYear].second.c_str(), "triggerScaleFactor"     , kBlue  },
                {"Signal Region"                 , signalRegionSelection    .c_str(), "None"                   , kOrange}};
            
            return outputVector;
        }

};

class EfficiencyEvaluator
{
    using CutFlowInfo = std::tuple<std::string, std::shared_ptr<TTreeFormula>, std::shared_ptr<TTreeReaderValue<float>>, Color_t>;

    public:
        EfficiencyEvaluator(std::string inputFileName, int year)
        : fYear (year)
        {
            fFileList = splitByLine(inputFileName    );
            
            fSampleName = getSampleNameFromFile(inputFileName);
            auto pairMxMy   = getMxMyFromSample(fSampleName);
            
            fCutFlowSelection = std::make_unique<CutFlowSelection>(fYear);
            fCutInfoVector = fCutFlowSelection->getCutFlowList();

            auto massPair = getMxMyFromSample(getSampleNameFromFile(inputFileName));
            std::string sampleName = "sig_MX_" + massPair.first + "_MY_" + massPair.second;

            fCutFlowHistogram =  std::make_shared<TH1F>(
                ("CutFlow_" + sampleName).c_str(),
                ("Cut Flow " + sampleName).c_str(),
                fCutInfoVector.size(), 0 , fCutInfoVector.size());
            fRelativeCutFlowHistogram =  std::make_shared<TH1F>(
                ("RelativeCutFlow_" + sampleName).c_str(),
                ("RelativeCut Flow " + sampleName).c_str(),
                fCutInfoVector.size(), 0 , fCutInfoVector.size());

            fCutFlowHistogram->GetYaxis()->SetRangeUser(0.01,1.);
            fCutFlowHistogram->SetStats(false);
            
            fRelativeCutFlowHistogram->GetYaxis()->SetRangeUser(0.,1.);
            fRelativeCutFlowHistogram->SetStats(false);
            
            int binCounter = 1;
            for(const auto& cutInfo: fCutInfoVector)
            {
                fCutFlowHistogram->GetXaxis()->SetBinLabel(binCounter, std::get<0>(cutInfo).c_str() );
                fRelativeCutFlowHistogram->GetXaxis()->SetBinLabel(binCounter++, std::get<0>(cutInfo).c_str() );
            }
        }

        ~EfficiencyEvaluator() {};

        void fillEfficiencyPlot()
        {
            float numberOfProcessedEvents   = 0;
            float numberOfTriggeredEvents   = 0;
            float numberOfPreselectedEvents = 0;
            for(const auto& fileName : fFileList)
            {
                TFile *theInputFile = TFile::Open(fileName.c_str());
                TH1D* theEfficiencyPlot = getHistogramFromFile<TH1D>(*theInputFile, "eff_histo");
                numberOfProcessedEvents   += theEfficiencyPlot->GetBinContent(theEfficiencyPlot->GetXaxis()->FindBin("Ntot_uw"));
                numberOfTriggeredEvents   += theEfficiencyPlot->GetBinContent(theEfficiencyPlot->GetXaxis()->FindBin("Ntrg_uw"));
                numberOfPreselectedEvents += theEfficiencyPlot->GetBinContent(theEfficiencyPlot->GetXaxis()->FindBin("Nsel_uw"));
                
                TTree* theInputTree = (TTree*)theInputFile->Get("bbbbTree");
                auto theTreeReader = std::make_shared<TTreeReader>(theInputTree);

                std::vector<CutFlowInfo> fCutFlowVector;

                for(const auto& cutInfo: fCutInfoVector)
                {
                    std::string                              cutName    = std::get<0>(cutInfo);
                    std::shared_ptr<TTreeFormula>            cutFormula = std::make_shared<TTreeFormula>(std::get<0>(cutInfo).c_str(), std::get<1>(cutInfo).c_str(), theInputTree);
                    std::shared_ptr<TTreeReaderValue<float>> cutWeight;
                    if(std::get<2>(cutInfo) != "None")       cutWeight = std::make_shared<TTreeReaderValue<float>>(*theTreeReader.get(), std::get<2>(cutInfo).c_str() ) ;
                    Color_t                                  cutColor   = std::get<3>(cutInfo);

                    CutFlowInfo theCutFlow {cutName, cutFormula, cutWeight, cutColor};
                    fCutFlowVector.emplace_back(std::move(theCutFlow));
                }   

                for(int it=0; it<theInputTree->GetEntries(); ++it)
                {
                    if(it%25000 == 0) std::cout << "Sample " << fSampleName << " - Entry " << it << std::endl;
                    theInputTree->GetEntry(it);
                    theTreeReader->Next();

                    float theWeight = 1.;
                    int binCounter = 0;

                    for(auto& cut : fCutFlowVector)
                    {
                        std::shared_ptr<TTreeFormula>            cutFormula = std::get<1>(cut);
                        if(!cutFormula->EvalInstance()) break;
                        fCutFlowHistogram->Fill(binCounter++, theWeight);
                    }
                }

                fCutFlowHistogram->SetBinContent(1, numberOfTriggeredEvents  );
                fCutFlowHistogram->SetBinContent(2, numberOfPreselectedEvents);
                theInputFile->Close();
                delete theInputFile;
            }

            float normalization = numberOfProcessedEvents;
            for(int bin = 1; bin <= fCutInfoVector.size(); ++bin)
            {
                float theBinContent = fCutFlowHistogram->GetBinContent(bin);
                fRelativeCutFlowHistogram->SetBinContent(bin, theBinContent/normalization);
                normalization = theBinContent;
            }

            fCutFlowHistogram->Scale(1./numberOfProcessedEvents);

        }

        void draw() 
        {
            TCanvas theCanvas;
            theCanvas.SetLogy();
            fCutFlowHistogram->DrawClone();
            theCanvas.SaveAs("CutFlow.png");
        }

        std::pair<std::shared_ptr<TH1F>,std::shared_ptr<TH1F>> getCutFlowPlot() 
        {
            return std::make_pair(fCutFlowHistogram, fRelativeCutFlowHistogram);
        }
        
    private:
        std::vector<std::string> fFileList;
        std::vector<CutFlowSelection::CutInfo> fCutInfoVector;
        std::unique_ptr<CutFlowSelection> fCutFlowSelection;
        int fYear;
        float numberOfProcessedEvents;
        std::string fSampleName;
        std::shared_ptr<TH1F> fCutFlowHistogram;
        std::shared_ptr<TH1F> fRelativeCutFlowHistogram;

};


class CutFlowPlotter
{
  public:
    CutFlowPlotter(int year, int mX)
    : fYear(year)
    , fMY(mX)
    {};

    CutFlowPlotter(){};

    void addPoint(int mX)
    {
        fMxList.emplace_back(mX);
        std::sort(fMxList.begin(), fMxList.end());
    }

    void createCutContainer(std::vector<CutFlowSelection::CutInfo> theCutInfoList)
    {
        for(const auto& theCutInfo : theCutInfoList)
        {
            fEfficiencyPairList.emplace_back(std::make_pair(std::get<0>(theCutInfo), std::make_tuple(std::vector<float>(fMxList.size(), 0.), std::get<3>(theCutInfo))));
        }
    }

    void fill(int mX, const std::shared_ptr<TH1F> theCutFlowPlot)
    {
        size_t index = find(fMxList.begin(), fMxList.end(), mX) - fMxList.begin();
        for(auto& cut : fEfficiencyPairList)
        {
            float efficiency = theCutFlowPlot->GetBinContent(theCutFlowPlot->GetXaxis()->FindBin(cut.first.c_str()));
            std::get<0>(cut.second)[index] = efficiency;
        }
    }

    std::shared_ptr<TCanvas> producePlots()
    {
        std::shared_ptr<TCanvas> cutFlowEfficiencyCanvas = std::make_shared<TCanvas>();
        TLegend *theLegend = new TLegend(0.3,0.70,0.88,0.88);

        bool isFirstPlot = true;
        for(auto& cut : fEfficiencyPairList)
        {
            TGraph *theGraph = new TGraph(fMxList.size(), fMxList.data(), std::get<0>(cut.second).data());
            theGraph->SetMarkerColor(std::get<1>(cut.second));
            theGraph->SetLineColor  (std::get<1>(cut.second));
            theGraph->SetLineWidth(2);
            theGraph->SetMarkerStyle(20);
            theGraph->SetMarkerSize(0.8);
            theLegend->AddEntry(theGraph, cut.first.c_str(), "pl");
            if(isFirstPlot)
            {
                isFirstPlot = false;
                theGraph->SetTitle(("Cut flow efficiency - m_{Y} = " + std::to_string(fMY) + " GeV - Run " + std::to_string(fYear)).c_str());
                theGraph->GetYaxis()->SetRangeUser(0., 1.1);
                theGraph->GetYaxis()->SetTitle("efficiency");
                theGraph->GetXaxis()->SetTitle("m_{X} [GeV]");
                theGraph->GetYaxis()->SetTitleSize(0.05);
                theGraph->GetYaxis()->SetTitleFont(62);
                theGraph->GetYaxis()->SetTitleOffset(0.95);
                theGraph->GetYaxis()->SetLabelFont(62); 
                theGraph->GetYaxis()->SetLabelSize(0.04);
                theGraph->GetXaxis()->SetTitleSize(0.05);
                theGraph->GetXaxis()->SetTitleFont(62);
                theGraph->GetXaxis()->SetTitleOffset(0.95);
                theGraph->GetXaxis()->SetLabelFont(62); 
                theGraph->GetXaxis()->SetLabelSize(0.04);
                theGraph->Draw("apl");
            }
            else 
                theGraph->Draw("pl same");
        }
        theLegend->Draw("same");

        return cutFlowEfficiencyCanvas;
    }



    private:
    int fYear;
    int fMY;
    std::vector<float> fMxList;
    std::vector<std::pair<std::string, std::tuple<std::vector<float>,Color_t>>> fEfficiencyPairList;
};


std::vector<std::string> signalList = 
    {
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_60.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_70.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_80.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1000_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1100_NANOAOD_v7_Full_MY_900.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_1000.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1200_NANOAOD_v7_Full_MY_900.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_1000.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_1200.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1400_NANOAOD_v7_Full_MY_900.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_1000.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_1200.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_1400.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1600_NANOAOD_v7_Full_MY_900.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_1000.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_1200.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_1400.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_1600.txt",
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_1800_NANOAOD_v7_Full_MY_900.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_100.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_1000.txt",
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_1200.txt",
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_125.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_1400.txt",
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_150.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_1600.txt",
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_1800.txt",
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_200.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_250.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_300.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_400.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_500.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_600.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_700.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_800.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_90.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_2000_NANOAOD_v7_Full_MY_900.txt" ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_300_NANOAOD_v7_Full_MY_90.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_200.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_250.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_400_NANOAOD_v7_Full_MY_90.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_200.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_250.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_300.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_500_NANOAOD_v7_Full_MY_90.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_200.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_250.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_300.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_400.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_600_NANOAOD_v7_Full_MY_90.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_200.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_250.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_300.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_400.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_500.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_700_NANOAOD_v7_Full_MY_90.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_200.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_250.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_300.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_400.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_500.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_600.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_800_NANOAOD_v7_Full_MY_90.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_100.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_125.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_150.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_200.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_250.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_300.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_400.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_500.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_60.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_600.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_70.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_700.txt"  ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_80.txt"   ,
        "FileList_NMSSM_XYH_bbbb_MX_900_NANOAOD_v7_Full_MY_90.txt"   ,
    };


int main(int argc, char** argv)
{
    ROOT::EnableThreadSafety();
    gSystem->ResetSignal(kSigSegmentationViolation, kTRUE);

    if(argc != 2)
    {
        std::cout << "Usage: ./privateTools/PlotCutFlow.C <year>";
        return EXIT_FAILURE;
    }
    int year = atoi(argv[1]);

    std::string folderName = "plotterListFiles/" + std::to_string(year) + "Resonant_NMSSM_XYH_bbbb/Signal/";
    std::string outputFileName = "CutFlow_" + std::to_string(year) + ".root";
    TFile outputFile(outputFileName.c_str(), "recreate");

    auto cutListModule = CutFlowSelection(year).getCutFlowList();

    std::map<int, CutFlowPlotter> theCutFlowPlotList;

    for(const auto& signal : signalList)
    {
        std::pair<std::string, std::string> pairMxMy = getMxMyFromSample(signal);
        int mX = atoi(pairMxMy.first .c_str());
        int mY = atoi(pairMxMy.second.c_str());
        if(mX == 1000 && mY == 150 && year == 2017) continue;
        if (theCutFlowPlotList.find(mY) == theCutFlowPlotList.end()) theCutFlowPlotList[mY] = CutFlowPlotter(year, mY);
        theCutFlowPlotList[mY].addPoint(mX);
    }
    for(auto& theCutFlowPlot: theCutFlowPlotList)
    {
        theCutFlowPlot.second.createCutContainer(cutListModule);
    }

    auto produceCutFlowPlot = [&outputFile, &folderName, &theCutFlowPlotList, year](std::string& sampleName)
    {
        std::pair<std::string, std::string> pairMxMy = getMxMyFromSample(sampleName);
        int mX = atoi(pairMxMy.first .c_str());
        int mY = atoi(pairMxMy.second.c_str());
        if(mX == 1000 && mY == 150 && year == 2017) return;
        std::string inputFileName = folderName + sampleName;
        EfficiencyEvaluator theEfficiencyEvaluator(inputFileName, year);
        theEfficiencyEvaluator.fillEfficiencyPlot();
        auto cutFlowPlot = theEfficiencyEvaluator.getCutFlowPlot();
        theCutFlowPlotList[mY].fill(mX, cutFlowPlot.first);
        outputFile.WriteObject(cutFlowPlot.first.get(), cutFlowPlot.first->GetName());
        outputFile.WriteObject(cutFlowPlot.second.get(), cutFlowPlot.second->GetName());
    };

    std::vector<std::future<void>> theFutureVector;

    for(int threadNumber = 0; threadNumber<numberOfThreads; threadNumber++)
    {
        theFutureVector.emplace_back(std::async(std::launch::async, produceCutFlowPlot, std::ref(signalList[threadNumber])));
    }

    int signalListCounter = numberOfThreads;

    for(int signalListCounter = numberOfThreads; signalListCounter<signalList.size(); )
    {
        for(int threadNumber = 0; threadNumber<numberOfThreads; threadNumber++)
        {
            if(theFutureVector[threadNumber].wait_for(std::chrono::milliseconds(1000)) == std::future_status::ready)
                theFutureVector[threadNumber] = std::async(std::launch::async, produceCutFlowPlot, std::ref(signalList[signalListCounter++]));
        }
    }

    for(int threadNumber = 0; threadNumber<numberOfThreads; threadNumber++)
    {
        theFutureVector[threadNumber].wait();
    }

    for(auto& theCutFlowPlot: theCutFlowPlotList)
    {
        std::shared_ptr<TCanvas> theCutFlowCanvas = theCutFlowPlot.second.producePlots();
        std::string canvasName = "CutFlow_MY_" + std::to_string(theCutFlowPlot.first) + "_" + std::to_string(year);
        outputFile.WriteObject(theCutFlowCanvas.get(), canvasName.c_str());
        theCutFlowCanvas->SaveAs((canvasName + ".png").c_str());
    }

    outputFile.Close();

}