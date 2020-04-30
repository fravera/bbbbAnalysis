#include "Riostream.h"
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "THStack.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TLegend.h"

void OverlapEfficiencyAndDistributions(TVirtualPad *theCanvas, TFile* inputFile, std::string triggerName, std::string filterName, std::string dataDataset, std::string signalDataset, std::string ttbarDataset, std::string wjetDataset)
{
    std::vector<std::string> backgroundNames    = {wjetDataset, ttbarDataset };
    std::vector<std::string> dataAndSignalNames = {dataDataset, signalDataset};
    std::vector<std::string> allDatasetNames;
    allDatasetNames.insert(allDatasetNames.end(), dataAndSignalNames.begin(), dataAndSignalNames.end());
    allDatasetNames.insert(allDatasetNames.end(), backgroundNames   .begin(), backgroundNames   .end());
    TH1F* dataPlot               = nullptr;
    TH1F* ttbarPlot              = nullptr;
    TH1F* wjetPlot               = nullptr;
    TH1F* signalPlot             = nullptr;
    TGraphAsymmErrors* dataEff   = nullptr;
    TGraphAsymmErrors* ttbarEff  = nullptr;
    TGraphAsymmErrors* signalEff = nullptr;

    theCanvas->cd();
    
    TGraphAsymmErrors* firstGraph = (TGraphAsymmErrors*)inputFile->Get( (allDatasetNames[0] + "_" + triggerName + "_Efficiency_" + filterName).data() );
    dataEff = firstGraph;
    firstGraph->Draw("apl");
    firstGraph->GetYaxis()->SetRangeUser(0., 1.5);
    firstGraph->GetYaxis()->SetTitleOffset(1.);

    TH1F* theDataDistribution = (TH1F*)inputFile->Get( (dataDataset + "_" + triggerName + "_Distribution_" + filterName).data() );
    dataPlot = theDataDistribution;
    theDataDistribution->SetDirectory(0);
    float scaleValue = 0.8/theDataDistribution->GetBinContent(theDataDistribution->GetMaximumBin());
    if(filterName == "BTagCaloCSVp087Triple") scaleValue/=3.8;

    THStack * theBackgroundStack = new THStack("hs1"," stacked");
    for(const auto & backgroundName : backgroundNames)
    {
        TH1F* theBackgroundDistribution = (TH1F*)inputFile->Get( (backgroundName + "_" + triggerName + "_Distribution_" + filterName).data() );
        if(backgroundName == wjetDataset) wjetPlot = theBackgroundDistribution;
        if(backgroundName == ttbarDataset) ttbarPlot = theBackgroundDistribution;
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
    signalPlot = theSignalDistribution;

    theSignalDistribution->SetDirectory(0);
    theSignalDistribution->Scale(scaleValue);
    // theSignalDistribution->SetFillColor(theSignalDistribution->GetMarkerColor());
    // theSignalDistribution->SetFillStyle(1001);
    theSignalDistribution->Draw("same HIST");

    for(const auto & datasetName : allDatasetNames)
    {
        if(datasetName == wjetDataset) continue;
        TGraphAsymmErrors* theGraph = (TGraphAsymmErrors*)inputFile->Get( (datasetName + "_" + triggerName + "_Efficiency_" + filterName).data() );
        if(datasetName == dataDataset) dataEff = theGraph;
        if(datasetName == ttbarDataset) ttbarEff = theGraph;
        if(datasetName == signalDataset) signalEff = theGraph;
        theGraph->Draw("pl same");
    }

    auto theLegend = new TLegend(0.20,0.7,0.68,0.88);
    theLegend->SetNColumns(2);
    theLegend->AddEntry(dataPlot, "data", "ep");
    theLegend->AddEntry(dataEff , "eff(data)", "epl");
    theLegend->AddEntry(ttbarPlot, "ttbar", "f");
    theLegend->AddEntry(ttbarEff , "eff(ttbar)", "epl");
    theLegend->AddEntry(wjetPlot, "W+Jets", "f");
    theLegend->AddEntry(signalEff , "eff(X #rightarrow YH #rightarrow bbbb)", "epl");
    theLegend->AddEntry(signalPlot, "X #rightarrow YH #rightarrow bbbb", "l");
    theLegend->Draw("same");
}


//--------------------------- Trigger efficiency 2016 ------------------------------------------


template<typename ...Strings>
void OverlapAllEfficiencyAndDistributionsInFile2016(std::string inputFileName, std::string dataDataset, std::string signalDataset, Strings... backgrounds)
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

void OverlapAllEfficiencyAndDistributions2016()
{
    gROOT->SetBatch(true);
    // OverlapAllEfficiencyAndDistributionsInFile2016<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched.root"           , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2016<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched.root"         , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    OverlapAllEfficiencyAndDistributionsInFile2016<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2016<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    gROOT->SetBatch(false);
}


//--------------------------- Trigger efficiency 2017 ------------------------------------------



void OverlapEfficiencyAndDistributionsTmp(TVirtualPad *theCanvas, TFile* inputFile, std::string triggerName, std::string filterName, std::string dataDataset, std::string signalDataset)
{
    std::vector<std::string> dataAndSignalNames = {dataDataset, signalDataset};
    std::vector<std::string> allDatasetNames;
    allDatasetNames.insert(allDatasetNames.end(), dataAndSignalNames.begin(), dataAndSignalNames.end());
    
    theCanvas->cd();

    TGraphAsymmErrors* firstGraph = (TGraphAsymmErrors*)inputFile->Get( (allDatasetNames[1] + "_" + triggerName + "_Efficiency_" + filterName).data() );
    firstGraph->Draw("apl");
    firstGraph->GetYaxis()->SetRangeUser(0., 1.2);

    TH1F* theDataDistribution = (TH1F*)inputFile->Get( (signalDataset + "_" + triggerName + "_Distribution_" + filterName).data() );
    theDataDistribution->SetDirectory(0);
    float scaleValue = 0.8/theDataDistribution->GetBinContent(theDataDistribution->GetMaximumBin());

    theDataDistribution->Scale(scaleValue);
    theDataDistribution->SetMarkerStyle(20);
    theDataDistribution->SetMarkerSize(0.3);
    // theDataDistribution->SetFillColor(theDataDistribution->GetMarkerColor());
    // theDataDistribution->SetFillStyle(1001);
    theDataDistribution->Draw("same HIST");

}


template<typename ...Strings>
void OverlapAllEfficiencyAndDistributionsInFile2017(std::string inputFileName, std::string dataDataset, std::string signalDataset, Strings... backgrounds)
{
    TFile *inputFile = new TFile(inputFileName.data());

    std::string triggerName = "";
    TCanvas *theCanvasOverlap = new TCanvas((triggerName + "_Overlap").data(), (triggerName + "_Overlap").data(), 1400, 800);
    theCanvasOverlap->DivideSquare(11,0.005,0.005);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(1 ), inputFile, triggerName, "L1filterHT"                     , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(2 ), inputFile, triggerName, "QuadCentralJet30"               , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(3 ), inputFile, triggerName, "CaloQuadJet30HT300"             , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(4 ), inputFile, triggerName, "BTagCaloCSVp05Double"           , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(5 ), inputFile, triggerName, "PFCentralJetLooseIDQuad30"      , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(6 ), inputFile, triggerName, "1PFCentralJetLooseID75"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(7 ), inputFile, triggerName, "2PFCentralJetLooseID60"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(8 ), inputFile, triggerName, "3PFCentralJetLooseID45"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(9 ), inputFile, triggerName, "4PFCentralJetLooseID40"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(10), inputFile, triggerName, "PFCentralJetsLooseIDQuad30HT300", dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(11), inputFile, triggerName, "BTagPFCSVp070Triple"            , dataDataset, signalDataset, backgrounds...);
    theCanvasOverlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasOverlap->GetName()) + ".png").data());
    delete theCanvasOverlap;

}

void OverlapAllEfficiencyAndDistributions2017()
{
    gROOT->SetBatch(true);
    OverlapAllEfficiencyAndDistributionsInFile2017<>("TriggerEfficiencies_2017_MuonPt30_matched.root"           , "", "gg_HH_4B_SM_2017");
    // OverlapAllEfficiencyAndDistributionsInFile2017<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched.root"           , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2017<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched.root"         , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2017<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2017<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    gROOT->SetBatch(false);
}


//--------------------------- Trigger efficiency 2018 ------------------------------------------


template<typename ...Strings>
void OverlapAllEfficiencyAndDistributionsInFile2018(std::string inputFileName, std::string dataDataset, std::string signalDataset, Strings... backgrounds)
{
    TFile *inputFile = new TFile(inputFileName.data());

    std::string triggerName = "";
    TCanvas *theCanvasOverlap = new TCanvas((triggerName + "_Overlap").data(), (triggerName + "_Overlap").data(), 1400, 800);
    theCanvasOverlap->DivideSquare(11,0.005,0.005);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(1 ), inputFile, triggerName, "L1filterHT"                     , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(2 ), inputFile, triggerName, "QuadCentralJet30"               , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(3 ), inputFile, triggerName, "CaloQuadJet30HT320"             , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(4 ), inputFile, triggerName, "BTagCaloDeepCSVp17Double"       , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(5 ), inputFile, triggerName, "PFCentralJetLooseIDQuad30"      , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(6 ), inputFile, triggerName, "1PFCentralJetLooseID75"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(7 ), inputFile, triggerName, "2PFCentralJetLooseID60"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(8 ), inputFile, triggerName, "3PFCentralJetLooseID45"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(9 ), inputFile, triggerName, "4PFCentralJetLooseID40"         , dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(10), inputFile, triggerName, "PFCentralJetsLooseIDQuad30HT330", dataDataset, signalDataset, backgrounds...);
    OverlapEfficiencyAndDistributionsTmp(theCanvasOverlap->cd(11), inputFile, triggerName, "BTagPFDeepCSV4p5Triple"         , dataDataset, signalDataset, backgrounds...);
    theCanvasOverlap->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasOverlap->GetName()) + ".png").data());
    delete theCanvasOverlap;

}

void OverlapAllEfficiencyAndDistributions2018()
{
    gROOT->SetBatch(true);
    OverlapAllEfficiencyAndDistributionsInFile2018<>("TriggerEfficiencies_2018_MuonPt30_matched.root"           , "", "gg_HH_4B_SM_2018");
    // OverlapAllEfficiencyAndDistributionsInFile2018<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched.root"           , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2018<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched.root"         , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2018<std::string, std::string>("TriggerEfficiencies_MuonPt30_matched_TTBarCut.root"  , "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    // OverlapAllEfficiencyAndDistributionsInFile2018<std::string, std::string>("TriggerEfficiencies_MuonPt30_unMatched_TTBarCut.root", "SingleMuon", "NMSSM_XYH_bbbb", "TTbar", "WJetsToLNu");
    gROOT->SetBatch(false);
}
