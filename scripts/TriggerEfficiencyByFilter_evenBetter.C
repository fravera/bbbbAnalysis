#include <Riostream.h>
#include <TH1F.h>
#include <TTree.h>
#include <TTreeFormula.h>
#include <TFile.h>
#include <TCanvas.h>
#include "TTreeReaderArray.h"
#include "TTreeReader.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
#include <thread>



template<typename T , typename V>
void customBinCreator(std::vector<T>& theOutputBinVector, V&& xMin)
{
    return;
}

template<typename T, typename V, typename... BinList>
void customBinCreator(std::vector<T>& theOutputBinVector, V&& xMin, V&& xMax, V&& binSize, BinList&&... theBinList)
{
    uint nBins = (xMax - xMin)/binSize;
    if(theOutputBinVector.size()==0) theOutputBinVector.push_back(xMin);
    for(uint bin=1; bin<=nBins; ++bin) theOutputBinVector.push_back( xMin + (xMax-xMin)*(float(bin)/float(nBins)) );
    customBinCreator(theOutputBinVector, xMax, theBinList...);
}

template<typename T, typename V, typename... BinList>
void customBinCreator(std::vector<T>& theOutputBinVector, V&& xMin, BinList&&... theBinList);


struct TriggerEfficiencyTool
{
    TriggerEfficiencyTool(std::string datasetName, TTree* inputTree, TTreeReader* theTreeReader, std::string triggerName, std::string filterCut, std::string variable, std::string normalizationCut, std::string plotTitle, std::vector<float> binVector, Color_t theColor)
        : fVariableHandler(*theTreeReader, variable.data())
        , fFilterEfficiencyCut("filterEfficiencyCut", filterCut.data(), inputTree)
        , fNormalizationEfficiencyCut("normalizationEfficiencyCut", normalizationCut.data(), inputTree)
    {
        std::string plotName =  plotTitle.substr(0, plotTitle.find(";"));
        fFilterEfficiencyHistogram = std::make_shared<TH1F>((datasetName + "_" + triggerName + "_Normalization_"+plotName).data(), ("Normalization "+plotTitle).data(), binVector.size()-1, binVector.data());
        fFilterEfficiencyHistogram->SetDirectory(0);
        fFilterEfficiencyHistogram->Sumw2();
        fFilterEfficiencyHistogram->SetLineColor(theColor);
        fFilterEfficiencyHistogram->SetMarkerColor(theColor);

        fFilterNormalizationHistogram = std::make_shared<TH1F>((datasetName + "_" + triggerName + "_Distribution_"+plotName).data(), ("Distribution "+plotTitle).data(), binVector.size()-1, binVector.data());
        fFilterNormalizationHistogram->SetDirectory(0);
        fFilterNormalizationHistogram->Sumw2();
        fFilterNormalizationHistogram->SetLineColor(theColor);
        fFilterNormalizationHistogram->SetMarkerColor(theColor);


        fEfficiency = std::make_shared<TGraphAsymmErrors>();
        fEfficiency->SetNameTitle((datasetName + "_" + triggerName + "_Efficiency_"+plotName).data(), ("Efficiency "+plotTitle).data());
        fEfficiency->GetYaxis()->SetRangeUser(0., 1.2);
        fEfficiency->SetLineColor(theColor);
        fEfficiency->SetMarkerColor(theColor);
    }

    ~TriggerEfficiencyTool()
    {
    }

    void fillEfficiency(float weightValue, float btag_SFValue)
    {
        if(fNormalizationEfficiencyCut.EvalInstance())
        {
            float variableValue = *fVariableHandler.Get();

            fFilterNormalizationHistogram->Fill(variableValue,weightValue);

            if(fFilterEfficiencyCut.EvalInstance())
            {
                fFilterEfficiencyHistogram->Fill(variableValue,weightValue);
            }
        }   
    }

    std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > getEfficiencyAndDistribution(float renormalizationValue)
    {
        fEfficiency->Divide(fFilterEfficiencyHistogram.get(),fFilterNormalizationHistogram.get(),"cl=0.683 b(1,1) mode");
        fFilterNormalizationHistogram->Scale(renormalizationValue);
        for(int bin=0; bin<= fFilterNormalizationHistogram->GetNbinsX(); ++bin)
        {
            float binWidth =  fFilterNormalizationHistogram->GetBinWidth(bin);
            fFilterNormalizationHistogram->SetBinContent(bin, fFilterNormalizationHistogram->GetBinContent(bin)/binWidth);
            fFilterNormalizationHistogram->SetBinError  (bin, fFilterNormalizationHistogram->GetBinError  (bin)/binWidth);
        }
        return {fEfficiency, fFilterNormalizationHistogram};
    }

    TTreeReaderValue<float> fVariableHandler;
    std::shared_ptr<TH1F> fFilterEfficiencyHistogram;
    std::shared_ptr<TH1F> fFilterNormalizationHistogram;
    std::shared_ptr<TGraphAsymmErrors> fEfficiency;
    TTreeFormula fFilterEfficiencyCut;
    TTreeFormula fNormalizationEfficiencyCut;

    // stringhe con cut!!!
};

struct DatasetEfficiencyEvaluator
{
    DatasetEfficiencyEvaluator(std::string inputFileName, std::string datasetName, float expectedNumberOfEvents)
    : fInputFileName(inputFileName)
    , fDatasetName(datasetName)
    , fRenormalizationValue(1.)
    {
        fInputFile = TFile::Open(inputFileName.data());
        if(fInputFile == nullptr)
        {
            std::cout << "File " << inputFileName << " does not exist. Aborting..." << std::endl;
        }
        fInputTree = (TTree*)fInputFile->Get("bbbbTree");
        fTheTreeReader  = new TTreeReader(fInputTree);
        fWeightHandler  = new TTreeReaderValue<float>(*fTheTreeReader, "weight" );
        fBtag_SFHandler = new TTreeReaderValue<float>(*fTheTreeReader, "btag_SF");

        if(expectedNumberOfEvents>0) fRenormalizationValue = expectedNumberOfEvents/static_cast<TH1F*>(fInputFile->Get("eff_histo"))->GetBinContent(2);
    }

    ~DatasetEfficiencyEvaluator()
    {
        for(auto& efficiencyTool : fTriggerEfficiencyToolVector) delete efficiencyTool;
        fTriggerEfficiencyToolVector.clear();
        delete fWeightHandler;
        delete fBtag_SFHandler;
        delete fTheTreeReader;
        fInputFile->Close();
        delete fInputFile;
    }

    void addTrigger(std::string triggerName, std::string filterCut, std::string variable, std::string normalizationCut, std::string plotTitle, std::vector<float> binVector, Color_t theColor)
    {
        fTriggerEfficiencyToolVector.emplace_back(new TriggerEfficiencyTool(fDatasetName, fInputTree, fTheTreeReader, triggerName, filterCut, variable, normalizationCut, plotTitle, binVector, theColor));
    }

    void addTrigger(std::string triggerName, std::string filterCut, std::string variable, std::string normalizationCut, std::string plotTitle, uint nBins, float xMin, float xMax, Color_t theColor)
    {
        std::vector<float> binVector(nBins+1);
        for(uint bin=0; bin<=nBins; ++bin) binVector[bin] = xMin + (xMax-xMin)*(float(bin)/float(nBins));
        addTrigger(triggerName, filterCut, variable, normalizationCut, plotTitle, binVector, theColor);
    }

    void fillTriggerEfficiency()
    {
        for(int it=0; it<fInputTree->GetEntries(); ++it)
        {
            fInputTree->GetEntry(it);
            fTheTreeReader->Next();
            float weightValue   = *fWeightHandler ->Get();
            float btag_SFValue  = *fBtag_SFHandler->Get();

            for(auto& triggerEfficiency : fTriggerEfficiencyToolVector ) triggerEfficiency->fillEfficiency(weightValue, btag_SFValue);
            
        }
    }

    std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > > getEfficiencyAndDistribution()
    {
        std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > > result;
        for(auto& triggerEfficiency : fTriggerEfficiencyToolVector ) result.emplace_back(triggerEfficiency->getEfficiencyAndDistribution(fRenormalizationValue));
        return result;
    }
    
    std::vector<TriggerEfficiencyTool*> fTriggerEfficiencyToolVector;
    std::string fInputFileName;
    std::string fDatasetName;
    float fRenormalizationValue;
    TFile *fInputFile;
    TTree *fInputTree;
    TTreeReader *fTheTreeReader;
    TTreeReaderValue<float> *fWeightHandler;
    TTreeReaderValue<float> *fBtag_SFHandler;
};


//--------------------------- Trigger efficiency 2016 ------------------------------------------


void ProduceAllTriggerEfficiencyInAFile2016(std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > >& theOutputResultVector, std::string inputFileName, std::string datasetName, float expectedNumberOfEvents, Color_t theColor, bool useTTbarCut)
{

    DatasetEfficiencyEvaluator theEfficiencyEvaluator(inputFileName, datasetName, expectedNumberOfEvents);

    std::string preselectionCut  = "1";
    std::string preselectionBTag = "";
    if(useTTbarCut && datasetName != "NMSSM_XYH_bbbb" ) preselectionCut   = "highestIsoElecton_pt>10. && electronTimesMuoncharge<1.";
    // if(useTTbarCut && datasetName != "SingleMuon"     ) preselectionBTag += " && jetFirstHighestDeepFlavB_hadronFlavour==5";
    std::string deepCSVJet_Pt0to50    =                                      " && jetFirstHighestDeepFlavB_pt <=  50.";
    std::string deepCSVJet_Pt50to150  = "&& jetFirstHighestDeepFlavB_pt >  50. && jetFirstHighestDeepFlavB_pt <= 150.";
    std::string deepCSVJet_Pt150to300 = "&& jetFirstHighestDeepFlavB_pt > 150. && jetFirstHighestDeepFlavB_pt <= 300.";
    std::string deepCSVJet_Pt300toInf = "&& jetFirstHighestDeepFlavB_pt > 300."                                       ;
    std::string deepCSVJet_barrelCut  = "&& abs(jetFirstHighestDeepFlavB_eta) <= 1.1"                                 ;
    std::vector<float> customBinning;


    std::string triggerName = "Double90Quad30";

    std::string normalizationCutDouble90Quad30 = preselectionCut;
    std::string filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& QuadCentralJet30>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "fourHighestJetPt_sum"          , normalizationCutDouble90Quad30, "L1filter; #sum p_{T} [GeV]; online efficiency"                   ,100, 100,1500, theColor);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "allJetPt_sum"                  , normalizationCutDouble90Quad30, "L1filterHT; #sum p_{T} [GeV]; online efficiency"                 ,100, 100,1500, theColor);
    
    normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
    filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& QuadCentralJet30>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 100., 3.,  180., 8.,  220., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "jetForthHighestPt_pt"          , normalizationCutDouble90Quad30, "QuadCentralJet30; p_{T}^{4} [GeV]; online efficiency"           , customBinning, theColor);
    
    normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
    filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& DoubleCentralJet90>=2";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "jetSecondHighestPt_pt"         , normalizationCutDouble90Quad30, "DoubleCentralJet90; p_{T}^{2} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
    filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& jetFirstHighestDeepFlavB_triggerFlag>=1";
    // filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& BTagCaloCSVp087Triple_jetFirstHighestDeepFlavB_triggerFlag>=1";  //for the future
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "jetFirstHighestDeepFlavB_deepFlavB", normalizationCutDouble90Quad30, "BTagCaloCSVp087Triple; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);

    std::string bNormCutDouble90Quad30   = normalizationCutDouble90Quad30 + preselectionBTag;
    std::string bFilterCutDouble90Quad30 = filterCutDouble90Quad30        + preselectionBTag;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutDouble90Quad30, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutDouble90Quad30       , "BTagCaloCSVp087TripleBflav; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutDouble90Quad30_Pt0to50   = normalizationCutDouble90Quad30 + preselectionBTag + deepCSVJet_Pt0to50;
    std::string bFilterCutDouble90Quad30_Pt0to50 = filterCutDouble90Quad30        + preselectionBTag + deepCSVJet_Pt0to50;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutDouble90Quad30_Pt0to50, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutDouble90Quad30_Pt0to50       , "BTagCaloCSVp087TripleBflav_Pt0to50; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutDouble90Quad30_Pt50to150   = normalizationCutDouble90Quad30 + preselectionBTag + deepCSVJet_Pt50to150;
    std::string bFilterCutDouble90Quad30_Pt50to150 = filterCutDouble90Quad30        + preselectionBTag + deepCSVJet_Pt50to150;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutDouble90Quad30_Pt50to150, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutDouble90Quad30_Pt50to150       , "BTagCaloCSVp087TripleBflav_Pt50to150; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutDouble90Quad30_Pt150to300   = normalizationCutDouble90Quad30 + preselectionBTag + deepCSVJet_Pt150to300;
    std::string bFilterCutDouble90Quad30_Pt150to300 = filterCutDouble90Quad30        + preselectionBTag + deepCSVJet_Pt150to300;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutDouble90Quad30_Pt150to300, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutDouble90Quad30_Pt150to300       , "BTagCaloCSVp087TripleBflav_Pt150to300; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);

    std::string bNormCutDouble90Quad30_Pt300toInf   = normalizationCutDouble90Quad30 + preselectionBTag + deepCSVJet_Pt300toInf;
    std::string bFilterCutDouble90Quad30_Pt300toInf = filterCutDouble90Quad30        + preselectionBTag + deepCSVJet_Pt300toInf;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutDouble90Quad30_Pt300toInf, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutDouble90Quad30_Pt300toInf       , "BTagCaloCSVp087TripleBflav_Pt300toInf; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);

    normalizationCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& BTagCaloCSVp087Triple>=3";
    filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& QuadPFCentralJetLooseID30>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 100., 3.,  150., 8.,  180., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "jetForthHighestPt_pt"          , normalizationCutDouble90Quad30, "QuadPFCentralJetLooseID30; p_{T}^{4} [GeV]; online efficiency"  , customBinning, theColor);
    
    normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
    filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& DoublePFCentralJetLooseID90>=2";
    customBinning.clear();
    customBinCreator(customBinning, 20., 70., 10.,  280., 5.,  380., 10.,  500., 20.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutDouble90Quad30, "jetSecondHighestPt_pt"         , normalizationCutDouble90Quad30, "DoublePFCentralJetLooseID90; p_{T}^{2} [GeV]; online efficiency", customBinning, theColor);


    triggerName = "Quad45";

    std::string normalizationCutQuad45 = preselectionCut;
    std::string filterCutQuad45 = normalizationCutQuad45 + "&& QuadCentralJet45>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutQuad45        , "fourHighestJetPt_sum"          , normalizationCutQuad45        , "L1filter; #sum p_{T} [GeV]; online efficiency"                   ,100, 100,1500, theColor);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutQuad45        , "allJetPt_sum"                  , normalizationCutQuad45        , "L1filterHT; #sum p_{T} [GeV]; online efficiency"                 ,100, 100,1500, theColor);
    normalizationCutQuad45 = filterCutQuad45;
    filterCutQuad45 = normalizationCutQuad45 + "&& QuadCentralJet45>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 100., 3.,  150., 8.,  180., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutQuad45        , "jetForthHighestPt_pt"          , normalizationCutQuad45        , "QuadCentralJet45; p_{T}^{4} [GeV]; online efficiency"           , customBinning, theColor);
    
    normalizationCutQuad45 = filterCutQuad45;
    filterCutQuad45 = normalizationCutQuad45 + "&& jetFirstHighestDeepFlavB_triggerFlag>=1";
    // filterCutQuad45 = normalizationCutQuad45 + "&& BTagCaloCSVp087Triple_jetFirstHighestDeepFlavB_triggerFlag>=1";  //for the future
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutQuad45        , "jetFirstHighestDeepFlavB_deepFlavB", normalizationCutQuad45        , "BTagCaloCSVp087Triple; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);
    
    std::string bNormCutQuad45 = normalizationCutQuad45 + preselectionBTag;
    std::string bFilterCutQuad45 = filterCutQuad45 + preselectionBTag;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutQuad45       , "jetFirstHighestDeepFlavB_deepFlavB", bNormCutQuad45                , "BTagCaloCSVp087TripleBflav; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
        
    std::string bNormCutQuad45_Pt0to50   = normalizationCutQuad45 + preselectionBTag + deepCSVJet_Pt0to50;
    std::string bFilterCutQuad45_Pt0to50 = filterCutQuad45        + preselectionBTag + deepCSVJet_Pt0to50;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutQuad45_Pt0to50, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutQuad45_Pt0to50       , "BTagCaloCSVp087TripleBflav_Pt0to50; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutQuad45_Pt50to150   = normalizationCutQuad45 + preselectionBTag + deepCSVJet_Pt50to150;
    std::string bFilterCutQuad45_Pt50to150 = filterCutQuad45        + preselectionBTag + deepCSVJet_Pt50to150;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutQuad45_Pt50to150, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutQuad45_Pt50to150       , "BTagCaloCSVp087TripleBflav_Pt50to150; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutQuad45_Pt150to300   = normalizationCutQuad45 + preselectionBTag + deepCSVJet_Pt150to300;
    std::string bFilterCutQuad45_Pt150to300 = filterCutQuad45        + preselectionBTag + deepCSVJet_Pt150to300;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutQuad45_Pt150to300, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutQuad45_Pt150to300       , "BTagCaloCSVp087TripleBflav_Pt150to300; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);

    std::string bNormCutQuad45_Pt300toInf   = normalizationCutQuad45 + preselectionBTag + deepCSVJet_Pt300toInf;
    std::string bFilterCutQuad45_Pt300toInf = filterCutQuad45        + preselectionBTag + deepCSVJet_Pt300toInf;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutQuad45_Pt300toInf, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutQuad45_Pt300toInf       , "BTagCaloCSVp087TripleBflav_Pt300toInf; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);

    normalizationCutQuad45 = normalizationCutQuad45 + "&& BTagCaloCSVp087Triple>=3";
    filterCutQuad45 = normalizationCutQuad45 + "&& QuadPFCentralJetLooseID45>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 30., 10.,  100., 3.,  130., 8.,  160., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutQuad45        , "jetForthHighestPt_pt"          , normalizationCutQuad45        , "QuadPFCentralJetLooseID45; p_{T}^{4} [GeV]; online efficiency"  , customBinning, theColor);


    triggerName = "And";
    
    std::string normalizationCutAnd = filterCutDouble90Quad30;
    std::string filterCutAnd = normalizationCutAnd + "&& QuadCentralJet45>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutAnd           , "fourHighestJetPt_sum"          , normalizationCutAnd           , "L1filterQuad45; #sum p_{T} [GeV]; online efficiency"             ,100, 100,1500, theColor);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutAnd           , "allJetPt_sum"                  , normalizationCutAnd           , "L1filterQuad45HT; #sum p_{T} [GeV]; online efficiency"           ,100, 100,1500, theColor);
    
    normalizationCutAnd = filterCutAnd;
    filterCutAnd = normalizationCutAnd + "&& QuadCentralJet45>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 30., 10.,  100., 3.,  130., 8.,  160., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutAnd           , "jetForthHighestPt_pt"          , normalizationCutAnd           , "QuadCentralJet45; p_{T}^{4} [GeV]; online efficiency"           , customBinning, theColor);
    
    normalizationCutAnd = filterCutAnd;
    filterCutAnd = normalizationCutAnd + "&& jetFirstHighestDeepFlavB_triggerFlag>=1";
    // filterCutAnd = normalizationCutAnd + "&& BTagCaloCSVp087Triple_jetFirstHighestDeepFlavB_triggerFlag>=1"; //for the future
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutAnd           , "jetFirstHighestDeepFlavB_deepFlavB", normalizationCutAnd           , "BTagCaloCSVp087Triple; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);
    
    std::string bNormCutAnd = normalizationCutAnd + preselectionBTag;
    std::string bFilterCutAnd = filterCutAnd + preselectionBTag;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutAnd          , "jetFirstHighestDeepFlavB_deepFlavB", bNormCutAnd                   , "BTagCaloCSVp087TripleBflav; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
        
    std::string bNormCutAnd_Pt0to50   = normalizationCutAnd + preselectionBTag + deepCSVJet_Pt0to50;
    std::string bFilterCutAnd_Pt0to50 = filterCutAnd        + preselectionBTag + deepCSVJet_Pt0to50;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutAnd_Pt0to50, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutAnd_Pt0to50       , "BTagCaloCSVp087TripleBflav_Pt0to50; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutAnd_Pt50to150   = normalizationCutAnd + preselectionBTag + deepCSVJet_Pt50to150;
    std::string bFilterCutAnd_Pt50to150 = filterCutAnd        + preselectionBTag + deepCSVJet_Pt50to150;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutAnd_Pt50to150, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutAnd_Pt50to150       , "BTagCaloCSVp087TripleBflav_Pt50to150; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);
    
    std::string bNormCutAnd_Pt150to300   = normalizationCutAnd + preselectionBTag + deepCSVJet_Pt150to300;
    std::string bFilterCutAnd_Pt150to300 = filterCutAnd        + preselectionBTag + deepCSVJet_Pt150to300;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutAnd_Pt150to300, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutAnd_Pt150to300       , "BTagCaloCSVp087TripleBflav_Pt150to300; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);

    std::string bNormCutAnd_Pt300toInf   = normalizationCutAnd + preselectionBTag + deepCSVJet_Pt300toInf;
    std::string bFilterCutAnd_Pt300toInf = filterCutAnd        + preselectionBTag + deepCSVJet_Pt300toInf;
    theEfficiencyEvaluator.addTrigger(triggerName, bFilterCutAnd_Pt300toInf, "jetFirstHighestDeepFlavB_deepFlavB", bNormCutAnd_Pt300toInf       , "BTagCaloCSVp087TripleBflav_Pt300toInf; DeepFlavB^{1}; online efficliency" , 50, 0  , 1  , theColor);

    normalizationCutAnd = normalizationCutAnd + "&& BTagCaloCSVp087Triple>=3";
    filterCutAnd = normalizationCutAnd + "&& QuadPFCentralJetLooseID45>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 30., 10.,  100., 3.,  130., 8.,  160., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCutAnd           , "jetForthHighestPt_pt"          , normalizationCutAnd           , "QuadPFCentralJetLooseID45; p_{T}^{4} [GeV]; online efficiency"  , customBinning, theColor);

    theEfficiencyEvaluator.fillTriggerEfficiency();

    theOutputResultVector = theEfficiencyEvaluator.getEfficiencyAndDistribution();
}



void ProduceAllTriggerEfficienciesFiles2016(std::string singleMuonInputFileName, std::string ttbarInputFileName, std::string wjetInputFileName, std::string xyhInputSignal, std::string outputFileName, bool useTTbarCut)
{
    float luminosity = 37400.; //pb-1

    float ttbarCrossSection =  831.76; //pb
    float wjetCrossSection  = 61526.7; //pb
    float xyhCrossSection   =      1.; //pb

    float ttbarExpectedEvents = luminosity*ttbarCrossSection;
    float wjetExpectedEvents  = luminosity*wjetCrossSection ;
    float xyhExpectedEvents   = luminosity*xyhCrossSection  ;
    
    
    gROOT->SetBatch();
    std::vector<std::string> inputFilesNames = {singleMuonInputFileName, ttbarInputFileName , wjetInputFileName , xyhInputSignal   };
    std::vector<std::string> datasetName     = {"SingleMuon"           , "TTbar"            , "WJetsToLNu"      , "NMSSM_XYH_bbbb" };
    std::vector<float>       expectedEvents  = {-1.                    , ttbarExpectedEvents, wjetExpectedEvents, xyhExpectedEvents};
    std::vector<Color_t>     theColorVector =  {kBlack                 , kBlue              , kGreen            , kRed             };
    std::vector<std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > > > vectorOfDatasetResults(inputFilesNames.size());

    std::vector<std::thread> theThreadList;
    for(uint it =0; it<inputFilesNames.size(); ++it)
    {
        theThreadList.emplace_back(std::thread(ProduceAllTriggerEfficiencyInAFile2016, std::ref(vectorOfDatasetResults[it]), std::ref(inputFilesNames[it]), std::ref(datasetName[it]), std::ref(expectedEvents[it]), std::ref(theColorVector[it]), useTTbarCut));
    }

    for(auto& theThread : theThreadList) theThread.join();

    TFile outputFile(outputFileName.data(),"RECREATE");

    for(auto& datasetVectorResult : vectorOfDatasetResults)
    {
        for(auto& efficiencyAndDistribution : datasetVectorResult)
        {
            std::get<0>(efficiencyAndDistribution).get()->Write();
            std::get<1>(efficiencyAndDistribution).get()->Write();
        }
    }

    outputFile.Close();
    gROOT->SetBatch(false);

}


void ProduceAllTriggerEfficiencies2016()
{
    ROOT::EnableThreadSafety();

    // std::thread theMatchedTriggerThread          (ProduceAllTriggerEfficienciesFiles2016, "SingleMuon_Data_forTrigger_MuonPt30_matched.root"  , "TTbar_MC_forTrigger_MuonPt30_matched.root"  , "WJetsToLNu_Data_forTrigger_MuonPt30_matched.root"  , "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_matched.root",   "TriggerEfficiencies_MuonPt30_matched.root"           , false);
    // std::thread theUnMatchedTriggerThread        (ProduceAllTriggerEfficienciesFiles2016, "SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched.root"         , false);
    std::thread theMatchedTriggerThreadTTbarCut  (ProduceAllTriggerEfficienciesFiles2016, "SingleMuon_Data_forTrigger_MuonPt30_matched.root"  , "TTbar_MC_forTrigger_MuonPt30_matched.root"  , "WJetsToLNu_Data_forTrigger_MuonPt30_matched.root"  , "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_matched.root",   "TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , true );
    // std::thread theUnMatchedTriggerThreadTTbarCut(ProduceAllTriggerEfficienciesFiles2016, "SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", true );
    // theMatchedTriggerThread          .join();
    // theUnMatchedTriggerThread        .join();
    theMatchedTriggerThreadTTbarCut  .join();
    // theUnMatchedTriggerThreadTTbarCut.join();
}


//--------------------------- Trigger efficiency 2017 ------------------------------------------


void ProduceAllTriggerEfficiencyInAFile2017(std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > >& theOutputResultVector, std::string inputFileName, std::string datasetName, float expectedNumberOfEvents, Color_t theColor, bool useTTbarCut)
{

    DatasetEfficiencyEvaluator theEfficiencyEvaluator(inputFileName, datasetName, expectedNumberOfEvents);

    std::string preselectionCut  = "1";
    std::string preselectionBTag = "";
    if(useTTbarCut && datasetName != "NMSSM_XYH_bbbb" ) preselectionCut   = "highestIsoElecton_pt>10. && electronTimesMuoncharge<1.";
    // if(useTTbarCut && datasetName != "SingleMuon"     ) preselectionBTag += " && jetFirstHighestDeepFlavB_hadronFlavour==5";
    std::vector<float> customBinning;

    std::string triggerName = "";

    std::string normalizationCut = preselectionCut;
    std::string filterCut = normalizationCut + "&& QuadCentralJet30>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "allJetPt_sum"                  , normalizationCut, "L1filterHT; #sum p_{T} [GeV]; online efficiency"                 ,100, 100,1500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& QuadCentralJet30>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 100., 3.,  180., 8.,  220., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetForthHighestPt_pt"          , normalizationCut, "QuadCentralJet30; p_{T}^{4} [GeV]; online efficiency"           , customBinning, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& CaloQuadJet30HT300>=1 && CaloQuadJet30HT300_MaxHT>=300";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "allJetPt_sum"                  , normalizationCut, "CaloQuadJet30HT300; #sum p_{T} [GeV]; online efficiency"         ,100, 20 , 1500, theColor);

    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& BTagCaloCSVp05Double_jetFirstHighestDeepFlavB_triggerFlag>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetFirstHighestDeepFlavB_deepFlavB", normalizationCut, "BTagCaloCSVp05Double; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);

    normalizationCut = normalizationCut + "&& BTagCaloCSVp05Double>=2";
    filterCut = normalizationCut + "&& PFCentralJetLooseIDQuad30>=4";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetForthHighestPt_pt"         , normalizationCut, "PFCentralJetLooseIDQuad30; p_{T}^{4} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
   
    normalizationCut = normalizationCut;
    filterCut = normalizationCut + "&& 1PFCentralJetLooseID75>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetFirstHighestPt_pt"         , normalizationCut, "1PFCentralJetLooseID75; p_{T}^{1} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& 2PFCentralJetLooseID60>=2";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetSecondHighestPt_pt"         , normalizationCut, "2PFCentralJetLooseID60; p_{T}^{2} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& 3PFCentralJetLooseID45>=3";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetThirdHighestPt_pt"         , normalizationCut, "3PFCentralJetLooseID45; p_{T}^{3} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& 4PFCentralJetLooseID40>=4";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetForthHighestPt_pt"         , normalizationCut, "4PFCentralJetLooseID40; p_{T}^{4} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& PFCentralJetsLooseIDQuad30HT300>=1 && PFCentralJetsLooseIDQuad30HT300_MaxHT>=300";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "allJetPt_sum"                  , normalizationCut, "PFCentralJetsLooseIDQuad30HT300; #sum p_{T} [GeV]; online efficiency"         ,100, 20 , 1500, theColor);

    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& BTagPFCSVp070Triple_jetFirstHighestDeepFlavB_triggerFlag>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetFirstHighestDeepFlavB_deepFlavB", normalizationCut, "BTagPFCSVp070Triple; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);

    theEfficiencyEvaluator.fillTriggerEfficiency();

    theOutputResultVector = theEfficiencyEvaluator.getEfficiencyAndDistribution();
}



void ProduceAllTriggerEfficienciesFiles2017(std::string singleMuonInputFileName, std::string ttbarInputFileName, std::string wjetInputFileName, std::string xyhInputSignal, std::string outputFileName, bool useTTbarCut)
{
    float luminosity = 51500.; //pb-1

    float ttbarCrossSection =  831.76; //pb
    float wjetCrossSection  = 61526.7; //pb
    float xyhCrossSection   =      1.; //pb

    float ttbarExpectedEvents = luminosity*ttbarCrossSection;
    float wjetExpectedEvents  = luminosity*wjetCrossSection ;
    float xyhExpectedEvents   = luminosity*xyhCrossSection  ;
    
    
    gROOT->SetBatch();
    std::vector<std::string> inputFilesNames = {/*singleMuonInputFileName, ttbarInputFileName , wjetInputFileName ,*/ xyhInputSignal   };
    std::vector<std::string> datasetName     = {/*"SingleMuon"           , "TTbar"            , "WJetsToLNu"      ,*/ "gg_HH_4B_SM_2017" };
    std::vector<float>       expectedEvents  = {/*-1.                    , ttbarExpectedEvents, wjetExpectedEvents,*/ xyhExpectedEvents};
    std::vector<Color_t>     theColorVector =  {/*kBlack                 , kBlue              , kGreen            ,*/ kRed             };
    std::vector<std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > > > vectorOfDatasetResults(inputFilesNames.size());

    std::vector<std::thread> theThreadList;
    for(uint it =0; it<inputFilesNames.size(); ++it)
    {
        theThreadList.emplace_back(std::thread(ProduceAllTriggerEfficiencyInAFile2017, std::ref(vectorOfDatasetResults[it]), std::ref(inputFilesNames[it]), std::ref(datasetName[it]), std::ref(expectedEvents[it]), std::ref(theColorVector[it]), useTTbarCut));
    }

    for(auto& theThread : theThreadList) theThread.join();

    TFile outputFile(outputFileName.data(),"RECREATE");

    for(auto& datasetVectorResult : vectorOfDatasetResults)
    {
        for(auto& efficiencyAndDistribution : datasetVectorResult)
        {
            std::get<0>(efficiencyAndDistribution).get()->Write();
            std::get<1>(efficiencyAndDistribution).get()->Write();
        }
    }

    outputFile.Close();
    gROOT->SetBatch(false);

}


void ProduceAllTriggerEfficiencies2017()
{
    ROOT::EnableThreadSafety();

    std::thread theMatchedTriggerThread          (ProduceAllTriggerEfficienciesFiles2017, ""  , ""  , ""  , "gg_HH_4B_SM_2017_forTrigger.root",   "TriggerEfficiencies_2017_MuonPt30_matched.root"           , true);
    // std::thread theUnMatchedTriggerThread        (ProduceAllTriggerEfficienciesFiles2017, "SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched.root"         , false);
    // std::thread theMatchedTriggerThreadTTbarCut  (ProduceAllTriggerEfficienciesFiles2017, "SingleMuon_Data_forTrigger_MuonPt30_matched.root"  , "TTbar_MC_forTrigger_MuonPt30_matched.root"  , "WJetsToLNu_Data_forTrigger_MuonPt30_matched.root"  , "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_matched.root",   "TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , true );
    // std::thread theUnMatchedTriggerThreadTTbarCut(ProduceAllTriggerEfficienciesFiles2017, "SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", true );
    theMatchedTriggerThread          .join();
    // theUnMatchedTriggerThread        .join();
    // theMatchedTriggerThreadTTbarCut  .join();
    // theUnMatchedTriggerThreadTTbarCut.join();
}


//--------------------------- Trigger efficiency 2018 ------------------------------------------


void ProduceAllTriggerEfficiencyInAFile2018(std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > >& theOutputResultVector, std::string inputFileName, std::string datasetName, float expectedNumberOfEvents, Color_t theColor, bool useTTbarCut)
{

    DatasetEfficiencyEvaluator theEfficiencyEvaluator(inputFileName, datasetName, expectedNumberOfEvents);

    std::string preselectionCut  = "1";
    std::string preselectionBTag = "";
    if(useTTbarCut && datasetName != "NMSSM_XYH_bbbb" ) preselectionCut   = "highestIsoElecton_pt>10. && electronTimesMuoncharge<1.";
    // if(useTTbarCut && datasetName != "SingleMuon"     ) preselectionBTag += " && jetFirstHighestDeepFlavB_hadronFlavour==5";
    std::vector<float> customBinning;

    std::string triggerName = "";

    std::string normalizationCut = preselectionCut;
    std::string filterCut = normalizationCut + "&& QuadCentralJet30>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "allJetPt_sum"                  , normalizationCut, "L1filterHT; #sum p_{T} [GeV]; online efficiency"                 ,100, 100,1500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& QuadCentralJet30>=4";
    customBinning.clear();
    customBinCreator(customBinning, 20., 100., 3.,  180., 8.,  220., 15.,  300., 30.);
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetForthHighestPt_pt"          , normalizationCut, "QuadCentralJet30; p_{T}^{4} [GeV]; online efficiency"           , customBinning, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& CaloQuadJet30HT320>=1 && CaloQuadJet30HT320_MaxHT>=320";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "allJetPt_sum"                  , normalizationCut, "CaloQuadJet30HT320; #sum p_{T} [GeV]; online efficiency"         ,100, 20 , 1500, theColor);

    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& BTagCaloDeepCSVp17Double_jetFirstHighestDeepFlavB_triggerFlag>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetFirstHighestDeepFlavB_deepFlavB", normalizationCut, "BTagCaloDeepCSVp17Double; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);
    
    normalizationCut = normalizationCut + "&& BTagCaloDeepCSVp17Double>=2";
    filterCut = normalizationCut + "&& PFCentralJetLooseIDQuad30>=4";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetForthHighestPt_pt"         , normalizationCut, "PFCentralJetLooseIDQuad30; p_{T}^{4} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
   
    normalizationCut = normalizationCut;
    filterCut = normalizationCut + "&& 1PFCentralJetLooseID75>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetFirstHighestPt_pt"         , normalizationCut, "1PFCentralJetLooseID75; p_{T}^{1} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& 2PFCentralJetLooseID60>=2";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetSecondHighestPt_pt"         , normalizationCut, "2PFCentralJetLooseID60; p_{T}^{2} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& 3PFCentralJetLooseID45>=3";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetThirdHighestPt_pt"         , normalizationCut, "3PFCentralJetLooseID45; p_{T}^{3} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& 4PFCentralJetLooseID40>=4";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetForthHighestPt_pt"         , normalizationCut, "4PFCentralJetLooseID40; p_{T}^{4} [GeV]; online efficiency"         ,100, 20 , 500, theColor);
    
    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& PFCentralJetsLooseIDQuad30HT330>=1 && PFCentralJetsLooseIDQuad30HT330_MaxHT>=330";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "allJetPt_sum"                  , normalizationCut, "PFCentralJetsLooseIDQuad30HT330; #sum p_{T} [GeV]; online efficiency"         ,100, 20 , 1500, theColor);

    normalizationCut = filterCut;
    filterCut = normalizationCut + "&& BTagPFDeepCSV4p5Triple_jetFirstHighestDeepFlavB_triggerFlag>=1";
    theEfficiencyEvaluator.addTrigger(triggerName, filterCut, "jetFirstHighestDeepFlavB_deepFlavB", normalizationCut, "BTagPFDeepCSV4p5Triple; DeepFlavB^{1}; online efficliency"      , 50, 0  , 1  , theColor);

    theEfficiencyEvaluator.fillTriggerEfficiency();

    theOutputResultVector = theEfficiencyEvaluator.getEfficiencyAndDistribution();
}



void ProduceAllTriggerEfficienciesFiles2018(std::string singleMuonInputFileName, std::string ttbarInputFileName, std::string wjetInputFileName, std::string xyhInputSignal, std::string outputFileName, bool useTTbarCut)
{
    float luminosity = 59700.; //pb-1

    float ttbarCrossSection =  831.76; //pb
    float wjetCrossSection  = 61526.7; //pb
    float xyhCrossSection   =      1.; //pb

    float ttbarExpectedEvents = luminosity*ttbarCrossSection;
    float wjetExpectedEvents  = luminosity*wjetCrossSection ;
    float xyhExpectedEvents   = luminosity*xyhCrossSection  ;
    
    
    gROOT->SetBatch();
    std::vector<std::string> inputFilesNames = {/*singleMuonInputFileName, ttbarInputFileName , wjetInputFileName ,*/ xyhInputSignal   };
    std::vector<std::string> datasetName     = {/*"SingleMuon"           , "TTbar"            , "WJetsToLNu"      ,*/ "gg_HH_4B_SM_2018" };
    std::vector<float>       expectedEvents  = {/*-1.                    , ttbarExpectedEvents, wjetExpectedEvents,*/ xyhExpectedEvents};
    std::vector<Color_t>     theColorVector =  {/*kBlack                 , kBlue              , kGreen            ,*/ kRed             };
    std::vector<std::vector<std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F> > > > vectorOfDatasetResults(inputFilesNames.size());

    std::vector<std::thread> theThreadList;
    for(uint it =0; it<inputFilesNames.size(); ++it)
    {
        theThreadList.emplace_back(std::thread(ProduceAllTriggerEfficiencyInAFile2018, std::ref(vectorOfDatasetResults[it]), std::ref(inputFilesNames[it]), std::ref(datasetName[it]), std::ref(expectedEvents[it]), std::ref(theColorVector[it]), useTTbarCut));
    }

    for(auto& theThread : theThreadList) theThread.join();

    TFile outputFile(outputFileName.data(),"RECREATE");

    for(auto& datasetVectorResult : vectorOfDatasetResults)
    {
        for(auto& efficiencyAndDistribution : datasetVectorResult)
        {
            std::get<0>(efficiencyAndDistribution).get()->Write();
            std::get<1>(efficiencyAndDistribution).get()->Write();
        }
    }

    outputFile.Close();
    gROOT->SetBatch(false);

}


void ProduceAllTriggerEfficiencies2018()
{
    ROOT::EnableThreadSafety();

    std::thread theMatchedTriggerThread          (ProduceAllTriggerEfficienciesFiles2018, ""  , ""  , ""  , "gg_HH_4B_SM_2018_forTrigger.root",   "TriggerEfficiencies_2018_MuonPt30_matched.root"           , true);
    // std::thread theUnMatchedTriggerThread        (ProduceAllTriggerEfficienciesFiles2018, "SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched.root"         , false);
    // std::thread theMatchedTriggerThreadTTbarCut  (ProduceAllTriggerEfficienciesFiles2018, "SingleMuon_Data_forTrigger_MuonPt30_matched.root"  , "TTbar_MC_forTrigger_MuonPt30_matched.root"  , "WJetsToLNu_Data_forTrigger_MuonPt30_matched.root"  , "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_matched.root",   "TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , true );
    // std::thread theUnMatchedTriggerThreadTTbarCut(ProduceAllTriggerEfficienciesFiles2018, "SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", true );
    theMatchedTriggerThread          .join();
    // theUnMatchedTriggerThread        .join();
    // theMatchedTriggerThreadTTbarCut  .join();
    // theUnMatchedTriggerThreadTTbarCut.join();
}
