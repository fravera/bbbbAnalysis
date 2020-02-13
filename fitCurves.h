#include "TFile.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TGraphErrors.h"

namespace TriggerFitCurves2016
{
    TFile triggerFitFile("data/TriggerEfficiencies_MuonPt30_matched_TTBarCut_fitResults.root");

    TF1* fSingleMuon_Double90Quad30_Efficiency_L1filterHT = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_L1filterHT"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Double90Quad30_Efficiency_L1filterHT_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_L1filterHT_FitResult");

    TF1* fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30_FitResult");

    TF1* fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90_FitResult");

    TF1* fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple_FitResult");

    TF1* fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30_FitResult");

    TF1* fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90_FitResult");

    TF1* fSingleMuon_Quad45_Efficiency_L1filterHT = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_L1filterHT"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Quad45_Efficiency_L1filterHT_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_L1filterHT_FitResult");

    TF1* fSingleMuon_Quad45_Efficiency_QuadCentralJet45 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadCentralJet45"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Quad45_Efficiency_QuadCentralJet45_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadCentralJet45_FitResult");

    TF1* fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple_FitResult");

    TF1* fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf");
    TFitResult* fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45_FitResult");

    TF1* fSingleMuon_And_Efficiency_L1filterQuad45HT = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_And_Efficiency_L1filterQuad45HT"))->GetFunction("cdf");
    TFitResult* fSingleMuon_And_Efficiency_L1filterQuad45HT_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_And_Efficiency_L1filterQuad45HT_FitResult");

    TF1* fSingleMuon_And_Efficiency_QuadCentralJet45 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadCentralJet45"))->GetFunction("cdf");
    TFitResult* fSingleMuon_And_Efficiency_QuadCentralJet45_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadCentralJet45_FitResult");

    TF1* fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45 = ((TGraphErrors*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf");
    TFitResult* fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45_FitResult = (TFitResult*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45_FitResult");

    TF1* fTTbar_Double90Quad30_Efficiency_L1filterHT = ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_L1filterHT"))->GetFunction("cdf");
    TFitResult* fTTbar_Double90Quad30_Efficiency_L1filterHT_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_L1filterHT_FitResult");

    TF1* fTTbar_Double90Quad30_Efficiency_QuadCentralJet30 = ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadCentralJet30"))->GetFunction("cdf");
    TFitResult* fTTbar_Double90Quad30_Efficiency_QuadCentralJet30_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadCentralJet30_FitResult");

    TF1* fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90 = ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"))->GetFunction("cdf");
    TFitResult* fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoubleCentralJet90_FitResult");

    TF1* fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple = ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf");
    TFitResult* fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple_FitResult");

    TF1* fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30 = ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"))->GetFunction("cdf");
    TFitResult* fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30_FitResult");

    TF1* fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90 = ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90"))->GetFunction("cdf");
    TFitResult* fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90_FitResult");

    TF1* fTTbar_Quad45_Efficiency_L1filterHT = ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_L1filterHT"))->GetFunction("cdf");
    TFitResult* fTTbar_Quad45_Efficiency_L1filterHT_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_L1filterHT_FitResult");

    TF1* fTTbar_Quad45_Efficiency_QuadCentralJet45 = ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadCentralJet45"))->GetFunction("cdf");
    TFitResult* fTTbar_Quad45_Efficiency_QuadCentralJet45_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadCentralJet45_FitResult");

    TF1* fTTbar_Quad45_Efficiency_BTagCaloCSVp087Triple = ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf");
    TFitResult* fTTbar_Quad45_Efficiency_BTagCaloCSVp087Triple_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple_FitResult");

    TF1* fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45 = ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf");
    TFitResult* fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45_FitResult");

    TF1* fTTbar_And_Efficiency_L1filterQuad45HT = ((TGraphErrors*)triggerFitFile.Get("TTbar_And_Efficiency_L1filterQuad45HT"))->GetFunction("cdf");
    TFitResult* fTTbar_And_Efficiency_L1filterQuad45HT_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_And_Efficiency_L1filterQuad45HT_FitResult");

    TF1* fTTbar_And_Efficiency_QuadCentralJet45 = ((TGraphErrors*)triggerFitFile.Get("TTbar_And_Efficiency_QuadCentralJet45"))->GetFunction("cdf");
    TFitResult* fTTbar_And_Efficiency_QuadCentralJet45_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_And_Efficiency_QuadCentralJet45_FitResult");

    TF1* fTTbar_And_Efficiency_QuadPFCentralJetLooseID45 = ((TGraphErrors*)triggerFitFile.Get("TTbar_And_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf");
    TFitResult* fTTbar_And_Efficiency_QuadPFCentralJetLooseID45_FitResult = (TFitResult*)triggerFitFile.Get("TTbar_And_Efficiency_QuadPFCentralJetLooseID45_FitResult");


};
