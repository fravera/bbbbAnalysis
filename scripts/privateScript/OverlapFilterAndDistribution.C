#include "Riostream.h"
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "THStack.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"

void OverlapEfficiencyAndDistributions(TVirtualPad *theCanvas, TFile* inputFile, std::string triggerName, std::string filterName, std::string dataDataset, std::string signalDataset, std::string ttbarDataset, std::string wjetDataset)
{
    std::vector<std::string> backgroundNames    = {wjetDataset, ttbarDataset };
    std::vector<std::string> dataAndSignalNames = {dataDataset, signalDataset};
    std::vector<std::string> allDatasetNames;
    allDatasetNames.insert(allDatasetNames.end(), dataAndSignalNames.begin(), dataAndSignalNames.end());
    allDatasetNames.insert(allDatasetNames.end(), backgroundNames   .begin(), backgroundNames   .end());

    theCanvas->cd();

    TGraphAsymmErrors* firstGraph = (TGraphAsymmErrors*)inputFile->Get( (allDatasetNames[0] + "_" + triggerName + "_Efficiency_" + filterName).data() );
    firstGraph->Draw("apl");
    firstGraph->GetYaxis()->SetRangeUser(0., 1.2);

    TH1F* theDataDistribution = (TH1F*)inputFile->Get( (dataDataset + "_" + triggerName + "_Distribution_" + filterName).data() );
    theDataDistribution->SetDirectory(0);
    float scaleValue = 0.8/theDataDistribution->GetBinContent(theDataDistribution->GetMaximumBin());

    THStack * theBackgroundStack = new THStack("hs1"," stacked");
    for(const auto & backgroundName : backgroundNames)
    {
        TH1F* theBackgroundDistribution = (TH1F*)inputFile->Get( (backgroundName + "_" + triggerName + "_Distribution_" + filterName).data() );
        theBackgroundDistribution->SetDirectory(0);
        theBackgroundDistribution->Scale(scaleValue);
        theBackgroundDistribution->SetFillStyle(3002);
        theBackgroundDistribution->SetFillColor(theBackgroundDistribution->GetLineColor());
        // theBackgroundDistribution->SetFillColorAlpha(theBackgroundDistribution->GetLineColor(), 0.5);
        theBackgroundStack->Add(theBackgroundDistribution);
    }
    theBackgroundStack->Draw("same HIST");

    theDataDistribution->Scale(scaleValue);
    theDataDistribution->SetMarkerStyle(20);
    theDataDistribution->SetMarkerSize(0.3);
    // theDataDistribution->SetFillColor(theDataDistribution->GetMarkerColor());
    // theDataDistribution->SetFillStyle(1001);
    theDataDistribution->Draw("same C E0");

    TH1F* theSignalDistribution = (TH1F*)inputFile->Get( (signalDataset + "_" + triggerName + "_Distribution_" + filterName).data() );

    theSignalDistribution->SetDirectory(0);
    theSignalDistribution->Scale(scaleValue);
    // theSignalDistribution->SetFillColor(theSignalDistribution->GetMarkerColor());
    // theSignalDistribution->SetFillStyle(1001);
    theSignalDistribution->Draw("same HIST");
    
    for(const auto & datasetName : allDatasetNames)
    {
        if(datasetName == wjetDataset) continue;
        TGraphAsymmErrors* theGraph = (TGraphAsymmErrors*)inputFile->Get( (datasetName + "_" + triggerName + "_Efficiency_" + filterName).data() );
        theGraph->Draw("pl same");
    }
}

template<typename ...Strings>
void OverlapAllEfficiencyAndDistributionsInFile(std::string inputFileName, std::string dataDataset, std::string signalDataset, Strings... backgrounds)
{
    TFile *inputFile = new TFile(inputFileName.data());

    std::string triggerName = "Double90Quad30";
    TCanvas *theCanvasDouble90Quad30Overlap = new TCanvas((triggerName + "_Overlap").data(), (triggerName + "_Overlap").data(), 1400, 800);
    theCanvasDouble90Quad30Overlap->DivideSquare(6,0.005,0.005);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(1), inputFile, triggerName, "L1filterHT"                 , dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(1), inputFile, triggerName, "L1filter"                   , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(2), inputFile, triggerName, "QuadCentralJet30"           , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(3), inputFile, triggerName, "DoubleCentralJet90"         , dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(4), inputFile, triggerName, "BTagCaloCSVp087TripleBflav" , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(4), inputFile, triggerName, "BTagCaloCSVp087Triple"      , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(5), inputFile, triggerName, "QuadPFCentralJetLooseID30"  , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30Overlap->cd(6), inputFile, triggerName, "DoublePFCentralJetLooseID90", dataDataset, signalDataset, backgrounds...);
    theCanvasDouble90Quad30Overlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasDouble90Quad30Overlap->GetName()) + ".png").data());
    delete theCanvasDouble90Quad30Overlap;

    TCanvas *theCanvasDouble90Quad30BTagOverlap = new TCanvas((triggerName + "_BTagOverlap").data(), (triggerName + "_BTagOverlap").data(), 1400, 800);
    theCanvasDouble90Quad30BTagOverlap->DivideSquare(6,0.005,0.005);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30BTagOverlap->cd(1), inputFile, triggerName, "BTagCaloCSVp087Triple"                , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30BTagOverlap->cd(2), inputFile, triggerName, "BTagCaloCSVp087TripleBflav"           , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30BTagOverlap->cd(3), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt0to50"   , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30BTagOverlap->cd(4), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt50to150" , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30BTagOverlap->cd(5), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt150to300", dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasDouble90Quad30BTagOverlap->cd(6), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt300toInf", dataDataset, signalDataset, backgrounds...);
    theCanvasDouble90Quad30BTagOverlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasDouble90Quad30BTagOverlap->GetName()) + ".png").data());
    delete theCanvasDouble90Quad30BTagOverlap;

    triggerName = "Quad45";
    TCanvas *theCanvasQuad45Overlap = new TCanvas((triggerName + "_Overlap").data(), (triggerName + "_Overlap").data(), 1400, 800);
    theCanvasQuad45Overlap->DivideSquare(4,0.005,0.005);
    OverlapEfficiencyAndDistributions(theCanvasQuad45Overlap->cd(1), inputFile, triggerName, "L1filterHT"                , dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasQuad45Overlap->cd(1), inputFile, triggerName, "L1filter"                  , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45Overlap->cd(2), inputFile, triggerName, "QuadCentralJet45"          , dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasQuad45Overlap->cd(3), inputFile, triggerName, "BTagCaloCSVp087TripleBflav", dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45Overlap->cd(3), inputFile, triggerName, "BTagCaloCSVp087Triple"     , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45Overlap->cd(4), inputFile, triggerName, "QuadPFCentralJetLooseID45" , dataDataset, signalDataset, backgrounds...);
    theCanvasQuad45Overlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad45Overlap->GetName()) + ".png").data());
    delete theCanvasQuad45Overlap;

    TCanvas *theCanvasQuad45BTagOverlap = new TCanvas((triggerName + "_BTagOverlap").data(), (triggerName + "_BTagOverlap").data(), 1400, 800);
    theCanvasQuad45BTagOverlap->DivideSquare(6,0.005,0.005);
    OverlapEfficiencyAndDistributions(theCanvasQuad45BTagOverlap->cd(1), inputFile, triggerName, "BTagCaloCSVp087Triple"                , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45BTagOverlap->cd(2), inputFile, triggerName, "BTagCaloCSVp087TripleBflav"           , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45BTagOverlap->cd(3), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt0to50"   , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45BTagOverlap->cd(4), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt50to150" , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45BTagOverlap->cd(5), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt150to300", dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasQuad45BTagOverlap->cd(6), inputFile, triggerName, "BTagCaloCSVp087TripleBflav_Pt300toInf", dataDataset, signalDataset, backgrounds...);
    theCanvasQuad45BTagOverlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad45BTagOverlap->GetName()) + ".png").data());
    delete theCanvasQuad45BTagOverlap;


    triggerName = "And";
    TCanvas *theCanvasAndOverlap = new TCanvas((triggerName + "_Overlap").data(), (triggerName + "_Overlap").data(), 1400, 800);
    theCanvasAndOverlap->DivideSquare(4,0.005,0.005);
    OverlapEfficiencyAndDistributions(theCanvasAndOverlap->cd(1), inputFile, triggerName, "L1filterQuad45HT"          , dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasAndOverlap->cd(1), inputFile, triggerName, "L1filterQuad45"            , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasAndOverlap->cd(2), inputFile, triggerName, "QuadCentralJet45"          , dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasAndOverlap->cd(3), inputFile, triggerName, "BTagCaloCSVp087TripleBflav", dataDataset, signalDataset, backgrounds...);
    // OverlapEfficiencyAndDistributions(theCanvasAndOverlap->cd(3), inputFile, triggerName, "BTagCaloCSVp087Triple"     , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributions(theCanvasAndOverlap->cd(4), inputFile, triggerName, "QuadPFCentralJetLooseID45" , dataDataset, signalDataset, backgrounds...);
    theCanvasAndOverlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasAndOverlap->GetName()) + ".png").data());
    // delete theCanvasAndOverlap;

}

void OverlapAllEfficiencyAndDistributions()
{
    gROOT->SetBatch(true);
    OverlapAllEfficiencyAndDistributionsInFile<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched.root"           , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    OverlapAllEfficiencyAndDistributionsInFile<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched.root"         , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    OverlapAllEfficiencyAndDistributionsInFile<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    OverlapAllEfficiencyAndDistributionsInFile<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    gROOT->SetBatch(false);
}

