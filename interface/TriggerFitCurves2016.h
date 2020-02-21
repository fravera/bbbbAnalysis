#include "TFile.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TGraphErrors.h"
#include "Math/WrappedMultiTF1.h"

namespace TriggerFitCurves2016
{
    TFile triggerFitFile("data/TriggerEfficiencies_MuonPt30_matched_TTBarCut_fitResults.root");
    class KFitResult : public TFitResult
    {
    public:
        using TFitResult::TFitResult;
        KFitResult* ResetModelFunction(TF1* func){
            this->SetModelFunction(std::shared_ptr<IModelFunction>(dynamic_cast<IModelFunction*>(ROOT::Math::WrappedMultiTF1(*func).Clone())));
            return this;
        }
    };
    std::pair<TF1*,KFitResult*> createPair(TF1* theFunction, KFitResult* theFitResult )
    {
        theFitResult->ResetModelFunction(theFunction);
        return {theFunction, theFitResult};
    }

    std::pair<TF1*, KFitResult*> fSingleMuon_Double90Quad30_Efficiency_L1filterHTPair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_L1filterHT"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_L1filterHT_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Quad45_Efficiency_L1filterHTPair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_L1filterHT"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_L1filterHT_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Quad45_Efficiency_QuadCentralJet45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadCentralJet45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadCentralJet45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_And_Efficiency_L1filterQuad45HTPair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_And_Efficiency_L1filterQuad45HT"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_And_Efficiency_L1filterQuad45HT_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_And_Efficiency_QuadCentralJet45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadCentralJet45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadCentralJet45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Double90Quad30_Efficiency_L1filterHTPair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_L1filterHT"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_L1filterHT_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Double90Quad30_Efficiency_QuadCentralJet30Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadCentralJet30"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadCentralJet30_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoubleCentralJet90_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Quad45_Efficiency_L1filterHTPair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_L1filterHT"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_L1filterHT_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Quad45_Efficiency_QuadCentralJet45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadCentralJet45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadCentralJet45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_And_Efficiency_L1filterQuad45HTPair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_And_Efficiency_L1filterQuad45HT"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_And_Efficiency_L1filterQuad45HT_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_And_Efficiency_QuadCentralJet45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_And_Efficiency_QuadCentralJet45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_And_Efficiency_QuadCentralJet45_FitResult")
    );

    std::pair<TF1*, KFitResult*> fTTbar_And_Efficiency_QuadPFCentralJetLooseID45Pair = createPair(
        ((TGraphErrors*)triggerFitFile.Get("TTbar_And_Efficiency_QuadPFCentralJetLooseID45"))->GetFunction("cdf"),
        (KFitResult*)triggerFitFile.Get("TTbar_And_Efficiency_QuadPFCentralJetLooseID45_FitResult")
    );


};
