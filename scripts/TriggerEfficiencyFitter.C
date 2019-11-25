#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"


TGraphAsymmErrors* TriggerEfficiencyFitter(TFile *inputFile, std::string sampleName, std::string referenceTriggerName, std::string histogramName, 
    std::string selectionName, std::string normalizationSelectionName, TF1 *fittingFunction, double *parameters,
    std::string histogramTitle, std::string xAxisTitle, Color_t lineColor)
{
    std::string cutTmpName = Form("%s/%s_%s/%s_%s_%s_%s",sampleName.data(),selectionName.data(),referenceTriggerName.data(),sampleName.data(),selectionName.data(),referenceTriggerName.data(),histogramName.data());
    std::cout<<cutTmpName<<std::endl;
    TH1D *cutTmp    = (TH1D*)inputFile->Get(cutTmpName.data());
    TH1D *cutHisto  = (TH1D*)cutTmp->Clone(("cut "+selectionName).data());
    cutHisto->SetDirectory(0);

    std::string normalizationTmpName = Form("%s/%s_%s/%s_%s_%s_%s",sampleName.data(),normalizationSelectionName.data(),referenceTriggerName.data(), sampleName.data(),normalizationSelectionName.data(),referenceTriggerName.data(), histogramName.data());
    std::cout<<normalizationTmpName<<std::endl;
    TH1D *normalizationTmp    = (TH1D*)inputFile->Get(normalizationTmpName.data());
    TH1D *normalizationHisto  = (TH1D*)normalizationTmp->Clone(("Normalization "+selectionName).data());
    normalizationHisto->SetDirectory(0);
    
    TGraphAsymmErrors *g_efficiency = new TGraphAsymmErrors();
    g_efficiency->Divide(cutHisto,normalizationHisto,"cl=0.683 b(1,1) mode");
    g_efficiency->SetTitle(histogramTitle.data());
    g_efficiency->GetXaxis()->SetTitle(xAxisTitle.data());
    g_efficiency->GetYaxis()->SetTitle("efficiency");
    g_efficiency->SetNameTitle(("Efficiency_"+selectionName+"_"+histogramName).data(), ("Efficiency "+selectionName+"_"+histogramName).data());
    g_efficiency->GetYaxis()->SetRangeUser(0., 1.2);
    g_efficiency->SetLineColor(lineColor);

    return g_efficiency;
}

void MeasureTriggerEfficiency(std::string inputFileName, std::string sampleName, std::string referenceTriggerName, std::string outputFileName, Color_t lineColor)
{
    TF1 *dummyFormula = new TF1();
    double dummyParameters[1] = {0.};

    TFile *inputFile = new TFile(inputFileName.data());
    TFile *outputFile = new TFile(outputFileName.data(),"RECREATE");
    TGraphAsymmErrors *efficiency;


    // HLT_DoubleJet90_Double30_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FourHighetJetPtSum", 
        "L1triggerDouble90Double30AndPrevious", "Normalization", dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula, dummyParameters,
    //     "Quad Central Jet 30 GeV", "p_{1}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula, dummyParameters,
    //     "Quad Central Jet 30 GeV", "p_{2}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula, dummyParameters,
    //     "Quad Central Jet 30 GeV", "p_{3}^{T}", lineColor);
    // efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ForthJetPt", 
        "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula, dummyParameters,
        "Quad Central Jet 30 GeV", "p_{4}^{T}", lineColor);
    efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetPt", 
    //     "DoubleCentralJet90AndPrevious", "QuadCentralJet30AndPrevious", dummyFormula, dummyParameters,
    //     "Double Central Jet 90 GeV", "p_{1}^{T}", lineColor);
    // efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "SecondJetPt", 
        "DoubleCentralJet90AndPrevious", "QuadCentralJet30AndPrevious", dummyFormula, dummyParameters,
        "Double Central Jet 90 GeV", "p_{2}^{T}", lineColor);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetDeepCSV", 
        "BTagCaloCSVp087SingleDouble90Double30AndPrevious", "DoubleCentralJet90AndPrevious", dummyFormula, dummyParameters,
        "BTag CaloCSV p087 Triple", "DeepCSV_{1}", lineColor);
    efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstPtOrderedJetDeepCSV", 
    //     "SelectionForBTagDouble90Double30", "NormalizationForBTagDouble90Double30", dummyFormula, dummyParameters,
    //     "BTag CaloCSV p087 Single", "DeepCSV", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula, dummyParameters,
    //     "Quad PF Central Jet Loose ID 30 GeV", "p_{1}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula, dummyParameters,
    //     "Quad PF Central Jet Loose ID 30 GeV", "p_{2}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula, dummyParameters,
    //     "Quad PF Central Jet Loose ID 30 GeV", "p_{3}^{T}", lineColor);
    // efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ForthJetPt", 
        "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula, dummyParameters,
        "Quad PF Central Jet Loose ID 30 GeV", "p_{4}^{T}", lineColor);
    efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetPt", 
    //     "DoublePFCentralJetLooseID90AndPrevious", "QuadPFCentralJetLooseID30AndPrevious", dummyFormula, dummyParameters,
    //     "Double PF Central Jet Loose ID 90 GeV", "p_{1}^{T}", lineColor);
    // efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "SecondJetPt", 
        "DoublePFCentralJetLooseID90AndPrevious", "QuadPFCentralJetLooseID30AndPrevious", dummyFormula, dummyParameters,
        "Double PF Central Jet Loose ID 90 GeV", "p_{2}^{T}", lineColor);
    efficiency->Write();



    // _QuadJet45_TripleBTagCSV_p087
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FourHighetJetPtSum", 
        "L1triggerQuad45AndPrevious", "Normalization", dummyFormula, dummyParameters,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula, dummyParameters,
    //     "Quad Central Jet 45 GeV", "p_{1}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula, dummyParameters,
    //     "Quad Central Jet 45 GeV", "p_{2}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula, dummyParameters,
    //     "Quad Central Jet 45 GeV", "p_{3}^{T}", lineColor);
    // efficiency->Write();
    
    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ForthJetPt", 
        "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula, dummyParameters,
        "Quad Central Jet 45 GeV", "p_{4}^{T}", lineColor);
    efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetDeepCSV", 
        "BTagCaloCSVp087SingleQuad45AndPrevious", "QuadCentralJet45AndPrevious", dummyFormula, dummyParameters,
        "BTag CaloCSV p087 Triple", "DeepCSV_{1}", lineColor);
    efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstPtOrderedJetDeepCSV", 
    //     "SelectionForBTagQuad45", "NormalizationForBTagQuad45", dummyFormula, dummyParameters,
    //     "BTag CaloCSV p087 Single", "DeepCSV", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula, dummyParameters,
    //     "Quad PF Central Jet Loose ID 45 GeV", "p_{1}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula, dummyParameters,
    //     "Quad PF Central Jet Loose ID 45 GeV", "p_{2}^{T}", lineColor);
    // efficiency->Write();

    // efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula, dummyParameters,
    //     "Quad PF Central Jet Loose ID 45 GeV", "p_{3}^{T}", lineColor);
    // efficiency->Write();

    efficiency = TriggerEfficiencyFitter(inputFile, sampleName, referenceTriggerName, "ForthJetPt", 
        "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula, dummyParameters,
        "Quad PF Central Jet Loose ID 45 GeV", "p_{4}^{T}", lineColor);
    efficiency->Write();


    inputFile->Close();
    outputFile->Close();

    delete inputFile;
    delete outputFile;

    return;
}

void ProduceAllTriggerEfficiencies()
{

    // // SingleMuon_RunBF
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunBF","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu20_Matched.root",kBlue);

    // // SingleMuon_RunGH
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24","TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20","TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunGH","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu20_Matched.root",kBlue);

    // // TTbar
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","TTbar","HLT_IsoMu24","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","TTbar","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","TTbar","HLT_IsoMu20","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30_CMVAM/outPlotter.root","TTbar","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu24","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu20","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu20","TriggerEfficiency_TTbar_4bLooseSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_4bLooseSelection_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerEfficiency_TTbar_4bSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_4bSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu20","TriggerEfficiency_TTbar_4bSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_4bSelection_IsoMu20_Matched.root",kBlue);


//    SingleMuon
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24","TriggerEfficiency_SingleMuon_triggerMatched_IsoMu24.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_triggerMatched_IsoMu24_Matched.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20","TriggerEfficiency_SingleMuon_triggerMatched_IsoMu20.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_triggerMatched_IsoMu20_Matched.root",kBlue);

    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerUnMatched","HLT_IsoMu24","TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu24.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerUnMatched","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu24_Matched.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerUnMatched","HLT_IsoMu20","TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu20.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerUnMatched","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24","TriggerEfficiency_SingleMuon_triggerMatched_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_triggerMatched_MuonPt40_ForthJetPt30_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20","TriggerEfficiency_SingleMuon_triggerMatched_MuonPt40_ForthJetPt30_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_triggerMatched_MuonPt40_ForthJetPt30_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24","TriggerEfficiency_SingleMuon_triggerMatched_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_triggerMatched_4bLooseSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20","TriggerEfficiency_SingleMuon_triggerMatched_4bLooseSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_triggerMatched_4bLooseSelection_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24","TriggerEfficiency_SingleMuon_triggerMatched_4bSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_SingleMuon_triggerMatched_4bSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20","TriggerEfficiency_SingleMuon_triggerMatched_4bSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_SingleMuon_triggerMatched_4bSelection_IsoMu20_Matched.root",kBlue);

    // TTbar
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24","TriggerEfficiency_TTbar_triggerMatched_IsoMu24.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_triggerMatched_IsoMu24_Matched.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20","TriggerEfficiency_TTbar_triggerMatched_IsoMu20.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_triggerMatched_IsoMu20_Matched.root",kBlue);

    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerUnMatched","HLT_IsoMu24","TriggerEfficiency_TTbar_triggerUnMatched_IsoMu24.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerUnMatched","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_triggerUnMatched_IsoMu24_Matched.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerUnMatched","HLT_IsoMu20","TriggerEfficiency_TTbar_triggerUnMatched_IsoMu20.root",kBlue);
    MeasureTriggerEfficiency("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerUnMatched","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_triggerUnMatched_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24","TriggerEfficiency_TTbar_triggerMatched_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_triggerMatched_MuonPt40_ForthJetPt30_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20","TriggerEfficiency_TTbar_triggerMatched_MuonPt40_ForthJetPt30_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_triggerMatched_MuonPt40_ForthJetPt30_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24","TriggerEfficiency_TTbar_triggerMatched_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_triggerMatched_4bLooseSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20","TriggerEfficiency_TTbar_triggerMatched_4bLooseSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bLooseSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_triggerMatched_4bLooseSelection_IsoMu20_Matched.root",kBlue);

    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24","TriggerEfficiency_TTbar_triggerMatched_4bSelection_IsoMu24.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24_Matched","TriggerEfficiency_TTbar_triggerMatched_4bSelection_IsoMu24_Matched.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20","TriggerEfficiency_TTbar_triggerMatched_4bSelection_IsoMu20.root",kBlue);
    // MeasureTriggerEfficiency("2016DataPlots_4bSelection/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu20_Matched","TriggerEfficiency_TTbar_triggerMatched_4bSelection_IsoMu20_Matched.root",kBlue);


}








