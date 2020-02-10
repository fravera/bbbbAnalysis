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

std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F>> TriggerEfficiencyByFilter(std::string inputFileName, std::string filterCut, std::string variable, std::string normalizationCut, std::string plotName, int nBins, float xMin, float xMax, float expectedNumberOfEvents, Color_t theColor)
{

    TFile* inputFile = TFile::Open(inputFileName.data());
    if(inputFile == nullptr)
    {
        std::cout << "File " << inputFileName << " does not exist. Aborting..." << std::endl;
        return {std::shared_ptr<TGraphAsymmErrors>(nullptr), std::shared_ptr<TH1F>(nullptr)};
    }

    TTree *inputTree = (TTree*)inputFile->Get("bbbbTree");
    // TH1F  *weightHistogram = (TH1F*)inputFile->Get("eff_histo");

    TTreeReader theTreeReader(inputTree);
    TTreeReaderValue<float> weightHandler  (theTreeReader, "weight"         );
    TTreeReaderValue<float> btag_SFHandler (theTreeReader, "btag_SF"        );
    TTreeReaderValue<float> variableHandler(theTreeReader, variable.data()  );
    
    TH1F *filterEfficiencyHistogram = new TH1F("filterEfficiencyHistogram", "filterEfficiencyHistogram", nBins, xMin, xMax);
    filterEfficiencyHistogram->SetDirectory(0);
    filterEfficiencyHistogram->Sumw2();
    TTreeFormula filterEfficiencyCut("filterEfficiencyCut", filterCut.data(), inputTree);

    std::shared_ptr<TH1F> filterNormalizationHistogram = std::make_shared<TH1F>(("Distribution_"+plotName).data(), ("Distribution "+plotName).data(), nBins, xMin, xMax);
    filterNormalizationHistogram->SetDirectory(0);
    filterNormalizationHistogram->Sumw2();
    TTreeFormula normalizationEfficiencyCut("normalizationEfficiencyCut", normalizationCut.data(), inputTree);

    for(int it=0; it<inputTree->GetEntries(); ++it)
    {
        inputTree->GetEntry(it);
        theTreeReader.Next();

        if(normalizationEfficiencyCut.EvalInstance())
        {
            float variableValue = *variableHandler.Get();
            float weightValue   = *weightHandler  .Get();
            // float btag_SFValue  = *btag_SFHandler .Get();

            filterNormalizationHistogram->Fill(variableValue,weightValue);

            if(filterEfficiencyCut.EvalInstance())
            {
                filterEfficiencyHistogram->Fill(variableValue,weightValue);
            }
        }
    }

    std::shared_ptr<TGraphAsymmErrors> g_efficiency = std::make_shared<TGraphAsymmErrors>();
    g_efficiency->Divide(filterEfficiencyHistogram,filterNormalizationHistogram.get(),"cl=0.683 b(1,1) mode");
    g_efficiency->GetXaxis()->SetTitle(variable.data());
    g_efficiency->GetYaxis()->SetTitle("efficiency");
    g_efficiency->SetNameTitle(("Efficiency_"+plotName).data(), ("Efficiency "+plotName).data());
    g_efficiency->GetYaxis()->SetRangeUser(0., 1.2);
    g_efficiency->SetLineColor(theColor);
    g_efficiency->SetMarkerColor(theColor);
    g_efficiency->Draw("apl");

    if(expectedNumberOfEvents>0.)
    {
        float eventsGenerated = static_cast<TH1F*>(inputFile->Get("eff_histo"))->GetBinContent(2);
        filterNormalizationHistogram->Scale(expectedNumberOfEvents/eventsGenerated);
    }
    filterNormalizationHistogram->SetLineColor(theColor);
    filterNormalizationHistogram->SetMarkerColor(theColor);
    return {g_efficiency, filterNormalizationHistogram};

    inputFile->Close();
    delete inputFile;
    delete filterEfficiencyHistogram;
}


void ProduceAllTriggerEfficienciesFiles(std::string singleMuonInputFileName, std::string ttbarInputFileName, std::string wjetInputFileName, std::string xyhInputSignal, std::string outputFileName)
{
    float luminosity = 37400.; //pb-1

    float ttbarCrossSection =  730.6; //pb
    float wjetCrossSection  = 50260.; //pb
    float xyhCrossSection   =     1.; //pb

    float ttbarExpectedEvents = luminosity*ttbarCrossSection;
    float wjetExpectedEvents  = luminosity*wjetCrossSection ;
    float xyhExpectedEvents   = luminosity*xyhCrossSection  ;
    
    
    gROOT->SetBatch();
    std::vector<std::string> inputFilesNames = {singleMuonInputFileName, ttbarInputFileName , wjetInputFileName , xyhInputSignal   };
    std::vector<std::string> datasetName     = {"SingleMuon"           , "TTbar"            , "WJetsToLNu"      , "NMSSM_XYH_bbbb" };
    std::vector<float>       expectedEvents  = {-1.                    , ttbarExpectedEvents, wjetExpectedEvents, xyhExpectedEvents};
    std::vector<Color_t>      theColorVector = {kBlack                 , kBlue              , kRed              , kGreen           };

    TFile outputFile(outputFileName.data(),"RECREATE");

    for(uint it =0; it<inputFilesNames.size(); ++it)
    {
        std::tuple<std::shared_ptr<TGraphAsymmErrors>,std::shared_ptr<TH1F>> efficiencyAndDistribution;

        std::string triggerName = "Double90Quad30";

        std::string normalizationCutDouble90Quad30 = "1";
        std::string filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& QuadCentralJet30>=1";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutDouble90Quad30, "fourHighestJetPt_sum"          , normalizationCutDouble90Quad30, "L1filter"                   , 50, 100, 600, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
        filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& QuadCentralJet30>=4";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutDouble90Quad30, "jetForthHighestPt_pt"          , normalizationCutDouble90Quad30, "QuadCentralJet30"           , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
        filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& DoubleCentralJet90>=2";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutDouble90Quad30, "jetSecondHighestPt_pt"         , normalizationCutDouble90Quad30, "DoubleCentralJet90"         , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
        filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& BTagCaloCSVp087Triple>=1";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutDouble90Quad30, "jetFirstHighestDeepCSV_deepCSV", normalizationCutDouble90Quad30, "BTagCaloCSVp087Triple"      , 50, 0  , 1  , expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& BTagCaloCSVp087Triple>=3";
        filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& QuadPFCentralJetLooseID30>=4";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutDouble90Quad30, "jetForthHighestPt_pt"          , normalizationCutDouble90Quad30, "QuadPFCentralJetLooseID30"  , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutDouble90Quad30 = filterCutDouble90Quad30;
        filterCutDouble90Quad30 = normalizationCutDouble90Quad30 + "&& DoublePFCentralJetLooseID90>=2";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutDouble90Quad30, "jetSecondHighestPt_pt"         , normalizationCutDouble90Quad30, "DoublePFCentralJetLooseID90", 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
 

        triggerName = "Quad45";

        std::string normalizationCutQuad45 = "1";
        std::string filterCutQuad45 = normalizationCutQuad45 + "&& QuadCentralJet45>=1";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutQuad45        , "fourHighestJetPt_sum"          , normalizationCutQuad45        , "L1filter"                   , 50, 100, 600, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutQuad45 = filterCutQuad45;
        filterCutQuad45 = normalizationCutQuad45 + "&& QuadCentralJet45>=4";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutQuad45        , "jetForthHighestPt_pt"          , normalizationCutQuad45        , "QuadCentralJet45"           , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutQuad45 = filterCutQuad45;
        filterCutQuad45 = normalizationCutQuad45 + "&& BTagCaloCSVp087Triple>=1";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutQuad45        , "jetFirstHighestDeepCSV_deepCSV", normalizationCutQuad45        , "BTagCaloCSVp087Triple"      , 50, 0  , 1  , expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutQuad45 = normalizationCutQuad45 + "&& BTagCaloCSVp087Triple>=3";
        filterCutQuad45 = normalizationCutQuad45 + "&& QuadPFCentralJetLooseID45>=4";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutQuad45        , "jetForthHighestPt_pt"          , normalizationCutQuad45        , "QuadPFCentralJetLooseID45"  , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
    

        triggerName = "And";
        
        std::string normalizationCutAnd = filterCutDouble90Quad30;
        std::string filterCutAnd = normalizationCutAnd + "&& QuadCentralJet45>=1";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutAnd           , "fourHighestJetPt_sum"          , normalizationCutAnd           , "L1filterQuad45"             , 50, 100, 600, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutAnd = filterCutAnd;
        filterCutAnd = normalizationCutAnd + "&& QuadCentralJet45>=4";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutAnd           , "jetForthHighestPt_pt"          , normalizationCutAnd           , "QuadCentralJet45"           , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutAnd = filterCutAnd;
        filterCutAnd = normalizationCutAnd + "&& BTagCaloCSVp087Triple>=1";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutAnd           , "jetFirstHighestDeepCSV_deepCSV", normalizationCutAnd           , "BTagCaloCSVp087Triple"      , 50, 0  , 1  , expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());
        
        normalizationCutAnd = normalizationCutAnd + "&& BTagCaloCSVp087Triple>=3";
        filterCutAnd = normalizationCutAnd + "&& QuadPFCentralJetLooseID45>=4";
        efficiencyAndDistribution = TriggerEfficiencyByFilter(inputFilesNames[it], filterCutAnd           , "jetForthHighestPt_pt"          , normalizationCutAnd           , "QuadPFCentralJetLooseID45"  , 50, 20 , 200, expectedEvents[it], theColorVector[it]);
        outputFile.WriteObject(std::get<0>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<0>(efficiencyAndDistribution)->GetName()).data());
        outputFile.WriteObject(std::get<1>(efficiencyAndDistribution).get(), (datasetName[it] + "_" + triggerName + "_" + std::get<1>(efficiencyAndDistribution)->GetName()).data());

    }

    outputFile.Close();
    gROOT->SetBatch(false);

}


void ProduceAllTriggerEfficiencies()
{
    ProduceAllTriggerEfficienciesFiles("SingleMuon_Data_forTrigger_MuonPt30_matched.root"  , "TTbar_MC_forTrigger_MuonPt30_matched.root"  , "WJetsToLNu_Data_forTrigger_MuonPt30_matched.root"  , "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_matched.root",   "TriggerEfficiencies_MuonPt30_matched.root"  );
    // ProduceAllTriggerEfficienciesFiles("SingleMuon_Data_forTrigger_MuonPt30_unMatched.root", "TTbar_MC_forTrigger_MuonPt30_unMatched.root", "WJetsToLNu_Data_forTrigger_MuonPt30_unMatched.root", "NMSSM_XYHbbbb_privateProduction_forTrigger_MuonPt30_unMatched.root", "TriggerEfficiencies_MuonPt30_unMatched.root");
}
