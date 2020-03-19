#include "TriggerEfficiencyCalculator.h"
#include "TriggerFitCurves2016.h"
// #include "TriggerFitCurves2017.h"
// #include "TriggerFitCurves2018.h"
#include "Muon.h"
#include "Electron.h"
#include "TRandom.h"

TriggerEfficiencyCalculator::TriggerEfficiencyCalculator(NanoAODTree& nat)
: theNanoAODTree_(nat)
{}

TriggerEfficiencyCalculator::~TriggerEfficiencyCalculator()
{}

std::tuple<float, float, float> TriggerEfficiencyCalculator::getTriggerScaleFactor(const std::vector<Jet>& selectedJets)
{
    extractInformationFromEvent(selectedJets);
    std::tuple<float, float, float> dataEfficiency       = calculateDataTriggerEfficiency();
    std::tuple<float, float, float> monteCarloEfficiency = calculateMonteCarloTriggerEfficiency();
    float scaleFactorCentral = (std::get<0>(monteCarloEfficiency) > 0) ? (std::get<0>(dataEfficiency) / std::get<0>(monteCarloEfficiency)) : 1.;
    float scaleFactorUp      = (std::get<2>(monteCarloEfficiency) > 0) ? (std::get<1>(dataEfficiency) / std::get<2>(monteCarloEfficiency)) : 1.;
    float scaleFactorDown    = (std::get<1>(monteCarloEfficiency) > 0) ? (std::get<2>(dataEfficiency) / std::get<1>(monteCarloEfficiency)) : 1.;
    return {scaleFactorCentral, scaleFactorUp, scaleFactorDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator::getDataTriggerEfficiency(const std::vector<Jet>& selectedJets)
{
    extractInformationFromEvent(selectedJets);
    return calculateDataTriggerEfficiency();
}

std::tuple<float, float, float> TriggerEfficiencyCalculator::getMonteCarloTriggerEfficiency(const std::vector<Jet>& selectedJets)
{
    extractInformationFromEvent(selectedJets);
    return calculateMonteCarloTriggerEfficiency();
}

std::tuple<std::tuple<float,float,float>, std::tuple<float,float,float>, std::tuple<float,float,float>> TriggerEfficiencyCalculator::getScaleFactorDataAndMonteCarloEfficiency(const std::vector<Jet>& selectedJets)
{
    extractInformationFromEvent(selectedJets);
    std::tuple<float, float, float> dataEfficiency       = calculateDataTriggerEfficiency();
    std::tuple<float, float, float> monteCarloEfficiency = calculateMonteCarloTriggerEfficiency();
    float scaleFactorCentral = (std::get<0>(monteCarloEfficiency) > 0) ? (std::get<0>(dataEfficiency) / std::get<0>(monteCarloEfficiency)) : 1.;
    float scaleFactorUp      = (std::get<2>(monteCarloEfficiency) > 0) ? (std::get<1>(dataEfficiency) / std::get<2>(monteCarloEfficiency)) : 1.;
    float scaleFactorDown    = (std::get<1>(monteCarloEfficiency) > 0) ? (std::get<2>(dataEfficiency) / std::get<1>(monteCarloEfficiency)) : 1.;
    return {{scaleFactorCentral, scaleFactorUp, scaleFactorDown} , dataEfficiency , monteCarloEfficiency};

}

void TriggerEfficiencyCalculator::simulateTrigger(OutputTree* theOutputTree)
{
    theOutputTree_ = theOutputTree;
    simulateTrigger_ = true;
    createTriggerSimulatedBranches();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TriggerEfficiencyCalculator_2016::TriggerEfficiencyCalculator_2016(NanoAODTree& nat)
: TriggerEfficiencyCalculator(nat)
{}

TriggerEfficiencyCalculator_2016::~TriggerEfficiencyCalculator_2016()
{}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataTriggerEfficiency()
{
    std::tuple<float, float, float> Double90Double30Efficiency = calculateDataDouble90Double30Efficiency();
    std::tuple<float, float, float> Quad45Efficiency           = calculateDataQuad45Efficiency          ();
    std::tuple<float, float, float> AndEfficiency              = calculateDataAndEfficiency             ();
    if(simulateTrigger_)
    {
        float Double90Double30Random = gRandom->Rndm();
        if(Double90Double30Random < std::get<0>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Simulated"    ) = 1;
        if(Double90Double30Random < std::get<1>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedUp"  ) = 1;
        if(Double90Double30Random < std::get<2>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedDown") = 1;
        
        float Quad45Random = gRandom->Rndm();
        if(Quad45Random < std::get<0>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_Simulated"    ) = 1;
        if(Quad45Random < std::get<1>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedUp"  ) = 1;
        if(Quad45Random < std::get<2>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedDown") = 1;

        theOutputTree_->userFloat("SimulatedTrigger_jet_pt2"  ) = pt2_  ;
        theOutputTree_->userFloat("SimulatedTrigger_jet_pt4"  ) = pt4_  ;
        theOutputTree_->userFloat("SimulatedTrigger_jet_sumPt") = sumPt_;
    }

    // std::cout << "Data Efficiency -> Double90Double30Efficiency = " << Double90Double30Efficiency << " Quad45Efficiency = " << Quad45Efficiency << " AndEfficiency = " << AndEfficiency << " Double90Double30Efficiency * AndEfficiency = " << (Double90Double30Efficiency * AndEfficiency) << std::endl;
    float efficiencyCentral = std::get<0>(Double90Double30Efficiency) + std::get<0>(Quad45Efficiency) - (std::get<0>(Double90Double30Efficiency) * std::get<0>(AndEfficiency));
    float efficiencyUp      = std::get<1>(Double90Double30Efficiency) + std::get<1>(Quad45Efficiency) - (std::get<1>(Double90Double30Efficiency) * std::get<2>(AndEfficiency));
    float efficiencyDown    = std::get<2>(Double90Double30Efficiency) + std::get<2>(Quad45Efficiency) - (std::get<2>(Double90Double30Efficiency) * std::get<1>(AndEfficiency));
    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloTriggerEfficiency()
{
    std::tuple<float, float, float> Double90Double30Efficiency = calculateMonteCarloDouble90Double30Efficiency();
    std::tuple<float, float, float> Quad45Efficiency           = calculateMonteCarloQuad45Efficiency          ();
    std::tuple<float, float, float> AndEfficiency              = calculateMonteCarloAndEfficiency             ();

    if(simulateTrigger_)
    {
        float Double90Double30Random = gRandom->Rndm();
        if(Double90Double30Random < std::get<0>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMc"    ) = 1;
        if(Double90Double30Random < std::get<1>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcUp"  ) = 1;
        if(Double90Double30Random < std::get<2>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcDown") = 1;
        
        float Quad45Random = gRandom->Rndm();
        if(Quad45Random < std::get<0>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMc"    ) = 1;
        if(Quad45Random < std::get<1>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcUp"  ) = 1;
        if(Quad45Random < std::get<2>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcDown") = 1;
    }


    // std::cout << "MonteCarlo Efficiency -> Double90Double30Efficiency = " << Double90Double30Efficiency << " Quad45Efficiency = " << Quad45Efficiency << " AndEfficiency = " << AndEfficiency << " Double90Double30Efficiency * AndEfficiency = " << (Double90Double30Efficiency * AndEfficiency) << std::endl;
    float efficiencyCentral = std::get<0>(Double90Double30Efficiency) + std::get<0>(Quad45Efficiency) - (std::get<0>(Double90Double30Efficiency) * std::get<0>(AndEfficiency));
    float efficiencyUp      = std::get<1>(Double90Double30Efficiency) + std::get<1>(Quad45Efficiency) - (std::get<1>(Double90Double30Efficiency) * std::get<2>(AndEfficiency));
    float efficiencyDown    = std::get<2>(Double90Double30Efficiency) + std::get<2>(Quad45Efficiency) - (std::get<2>(Double90Double30Efficiency) * std::get<1>(AndEfficiency));
    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataDouble90Double30Efficiency()
{
    float bTagEffJet0        = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[0]);
    float bTagEffJet1        = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[1]);
    float bTagEffJet2        = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[2]);
    float bTagEffJet3        = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[3]);
    float effL1              = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_L1filterHTPair.first                 ->Eval(sumPt_            );
    float effQuad30CaloJet   = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30Pair.first           ->Eval(pt4_              );
    float effDouble90CaloJet = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90Pair.first         ->Eval(pt2_              );
    float effQuad30PFJet     = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.first  ->Eval(pt4_              );
    float effDouble90PFJet   = TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.first->Eval(pt2_              );

    float bTagEffJet0Error        = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[0]);
    float bTagEffJet1Error        = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[1]);
    float bTagEffJet2Error        = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[2]);
    float bTagEffJet3Error        = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[3]);
    float effL1Error              = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_L1filterHTPair.second                 , sumPt_            );
    float effQuad30CaloJetError   = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30Pair.second           , pt4_              );
    float effDouble90CaloJetError = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90Pair.second         , pt2_              );
    float effQuad30PFJetError     = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.second  , pt4_              );
    float effDouble90PFJetError   = getFitError(TriggerFitCurves2016::fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.second, pt2_              );


    float efficiencyCentral = computeDouble90Double30Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad30CaloJet, effDouble90CaloJet, effQuad30PFJet, effDouble90PFJet);
    float efficiencyUp      = computeDouble90Double30Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad30CaloJet + effQuad30CaloJetError, effDouble90CaloJet + effDouble90CaloJetError, effQuad30PFJet + effQuad30PFJetError, effDouble90PFJet + effDouble90PFJetError);
    float efficiencyDown    = computeDouble90Double30Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad30CaloJet - effQuad30CaloJetError, effDouble90CaloJet - effDouble90CaloJetError, effQuad30PFJet - effQuad30PFJetError, effDouble90PFJet - effDouble90PFJetError);
    
    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataQuad45Efficiency()
{
    float bTagEffJet0      = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[0]);
    float bTagEffJet1      = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[1]);
    float bTagEffJet2      = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[2]);
    float bTagEffJet3      = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[3]);
    float effL1            = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_L1filterHTPair.first               ->Eval(sumPt_            );
    float effQuad45CaloJet = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_              );
    float effQuad45PFJet   = TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_              );

    float bTagEffJet0Error      = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[0]);
    float bTagEffJet1Error      = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[1]);
    float bTagEffJet2Error      = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[2]);
    float bTagEffJet3Error      = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[3]);
    float effL1Error            = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_L1filterHTPair.second               , sumPt_            );
    float effQuad45CaloJetError = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_QuadCentralJet45Pair.second         , pt4_              );
    float effQuad45PFJetError   = getFitError(TriggerFitCurves2016::fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_              );

    float efficiencyCentral = computeQuad45Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad45CaloJet, effQuad45PFJet);
    float efficiencyUp      = computeQuad45Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
    float efficiencyDown    = computeQuad45Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataAndEfficiency()
{
    float effL1            = TriggerFitCurves2016::fSingleMuon_And_Efficiency_L1filterQuad45HTPair.first         ->Eval(sumPt_);
    float effQuad45CaloJet = TriggerFitCurves2016::fSingleMuon_And_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_  );
    float effQuad45PFJet   = TriggerFitCurves2016::fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_  );

    float effL1Error            = getFitError(TriggerFitCurves2016::fSingleMuon_And_Efficiency_L1filterQuad45HTPair.second         , sumPt_);
    float effQuad45CaloJetError = getFitError(TriggerFitCurves2016::fSingleMuon_And_Efficiency_QuadCentralJet45Pair.second         , pt4_  );
    float effQuad45PFJetError   = getFitError(TriggerFitCurves2016::fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_  );

    float efficiencyCentral = computeAndEfficiency(effL1, effQuad45CaloJet, effQuad45PFJet);
    float efficiencyUp      = computeAndEfficiency(effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
    float efficiencyDown    = computeAndEfficiency(effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}


std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloDouble90Double30Efficiency()
{
    float bTagEffJet0        = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[0]);
    float bTagEffJet1        = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[1]);
    float bTagEffJet2        = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[2]);
    float bTagEffJet3        = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[3]);
    float effL1              = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_L1filterHTPair.first                 ->Eval(sumPt_            );
    float effQuad30CaloJet   = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_QuadCentralJet30Pair.first           ->Eval(pt4_              );
    float effDouble90CaloJet = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90Pair.first         ->Eval(pt2_              );
    float effQuad30PFJet     = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.first  ->Eval(pt4_              );
    float effDouble90PFJet   = TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.first->Eval(pt2_              );

    float bTagEffJet0Error        = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[0]);
    float bTagEffJet1Error        = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[1]);
    float bTagEffJet2Error        = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[2]);
    float bTagEffJet3Error        = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[3]);
    float effL1Error              = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_L1filterHTPair.second                 , sumPt_            );
    float effQuad30CaloJetError   = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_QuadCentralJet30Pair.second           , pt4_              );
    float effDouble90CaloJetError = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90Pair.second         , pt2_              );
    float effQuad30PFJetError     = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.second  , pt4_              );
    float effDouble90PFJetError   = getFitError(TriggerFitCurves2016::fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.second, pt2_              );


    float efficiencyCentral = computeDouble90Double30Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad30CaloJet, effDouble90CaloJet, effQuad30PFJet, effDouble90PFJet);
    float efficiencyUp      = computeDouble90Double30Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad30CaloJet + effQuad30CaloJetError, effDouble90CaloJet + effDouble90CaloJetError, effQuad30PFJet + effQuad30PFJetError, effDouble90PFJet + effDouble90PFJetError);
    float efficiencyDown    = computeDouble90Double30Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad30CaloJet - effQuad30CaloJetError, effDouble90CaloJet - effDouble90CaloJetError, effQuad30PFJet - effQuad30PFJetError, effDouble90PFJet - effDouble90PFJetError);
    
    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloQuad45Efficiency()
{
    float bTagEffJet0      = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[0]);
    float bTagEffJet1      = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[1]);
    float bTagEffJet2      = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[2]);
    float bTagEffJet3      = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[3]);
    float effL1            = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_L1filterHTPair.first               ->Eval(sumPt_            );
    float effQuad45CaloJet = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_              );
    float effQuad45PFJet   = TriggerFitCurves2016::fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_              );

    float bTagEffJet0Error      = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[0]);
    float bTagEffJet1Error      = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[1]);
    float bTagEffJet2Error      = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[2]);
    float bTagEffJet3Error      = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[3]);
    float effL1Error            = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_L1filterHTPair.second               , sumPt_            );
    float effQuad45CaloJetError = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_QuadCentralJet45Pair.second         , pt4_              );
    float effQuad45PFJetError   = getFitError(TriggerFitCurves2016::fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_              );

    float efficiencyCentral = computeQuad45Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad45CaloJet, effQuad45PFJet);
    float efficiencyUp      = computeQuad45Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
    float efficiencyDown    = computeQuad45Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloAndEfficiency()
{
    float effL1            = TriggerFitCurves2016::fTTbar_And_Efficiency_L1filterQuad45HTPair.first         ->Eval(sumPt_);
    float effQuad45CaloJet = TriggerFitCurves2016::fTTbar_And_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_  );
    float effQuad45PFJet   = TriggerFitCurves2016::fTTbar_And_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_  );

    float effL1Error            = getFitError(TriggerFitCurves2016::fTTbar_And_Efficiency_L1filterQuad45HTPair.second         , sumPt_);
    float effQuad45CaloJetError = getFitError(TriggerFitCurves2016::fTTbar_And_Efficiency_QuadCentralJet45Pair.second         , pt4_  );
    float effQuad45PFJetError   = getFitError(TriggerFitCurves2016::fTTbar_And_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_  );

    float efficiencyCentral = computeAndEfficiency(effL1, effQuad45CaloJet, effQuad45PFJet);
    float efficiencyUp      = computeAndEfficiency(effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
    float efficiencyDown    = computeAndEfficiency(effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}


void TriggerEfficiencyCalculator_2016::extractInformationFromEvent(std::vector<Jet> selectedJets)
{

    assert(selectedJets.size()==4);

    uint16_t positionInVector = 0;
    for(const auto& theJet : selectedJets)
    {
        deepFlavBVector[positionInVector++] = get_property(theJet, Jet_btagDeepFlavB); //This has to be the deep flavor!!
    }
    
    stable_sort(selectedJets.begin(), selectedJets.end(), [](const Jet & a, const Jet & b) -> bool
    {
        return ( a.P4().Pt() > b.P4().Pt() );
    });

    pt2_ = selectedJets[1].P4().Pt();
    pt4_ = selectedJets[3].P4().Pt();

    sumPt_ = 0;
    for (uint ij = 0; ij < *(theNanoAODTree_.nJet); ++ij)
    {
        // here preselect jets
        Jet jet (ij, &theNanoAODTree_);

        // Jet ID flags bit1 is loose (always false in 2017 since it does not exist), bit2 is tight, bit3 is tightLepVeto
        // but note that bit1 means idx 0 and so on
        int jetId = get_property(jet, Jet_jetId); 

        if (!(jetId & (1 << 1))) // tight jet Id
            continue;

        bool isElectron = false;
        for (uint candIt = 0; candIt < *(theNanoAODTree_.nElectron); ++candIt)
        {
            Electron theElectron (candIt, &theNanoAODTree_);
            if(get_property(theElectron, Electron_pfRelIso03_all) > 0.3) continue;
            if(jet.getIdx() == get_property(theElectron, Electron_jetIdx))
            {
                isElectron = true;
                break;
            }
        }
        if(isElectron) continue;

        bool isMuon = false;
        for (uint candIt = 0; candIt < *(theNanoAODTree_.nMuon); ++candIt)
        {
            Muon theMuon (candIt, &theNanoAODTree_);
            if(get_property(theMuon, Muon_pfRelIso04_all) > 0.3) continue;
            if(jet.getIdx() == get_property(theMuon, Muon_jetIdx))
            {
                isMuon = true;
                break;
            }
        }
        if(isMuon) continue;

        if (jet.P4().Pt() >= 10. && std::abs(jet.P4().Eta()) < 3.) sumPt_ += jet.P4().Pt();
    }

}


void  TriggerEfficiencyCalculator_2016::createTriggerSimulatedBranches()
{
    theOutputTree_->declareUserIntBranch("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Simulated"      , 0);
    theOutputTree_->declareUserIntBranch("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedUp"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedDown"  , 0);

    theOutputTree_->declareUserIntBranch("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMc"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcUp"  , 0);
    theOutputTree_->declareUserIntBranch("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcDown", 0);

    theOutputTree_->declareUserIntBranch("HLT_QuadJet45_TripleBTagCSV_p087_Simulated"                 , 0);
    theOutputTree_->declareUserIntBranch("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedUp"               , 0);
    theOutputTree_->declareUserIntBranch("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedDown"             , 0);

    theOutputTree_->declareUserIntBranch("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMc"               , 0);
    theOutputTree_->declareUserIntBranch("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcUp"             , 0);
    theOutputTree_->declareUserIntBranch("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcDown"           , 0);

    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt2"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt4"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_sumPt", -999);
}

void  TriggerEfficiencyCalculator_2017::createTriggerSimulatedBranches()
{
    theOutputTree_->declareUserIntBranch("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_Simulated"      , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_SimulatedUp"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_SimulatedDown"  , 0);

    theOutputTree_->declareUserIntBranch("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_SimulatedMc"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_SimulatedMcUp"  , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0_SimulatedMcDown", 0);

    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt1"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt2"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt3"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt4"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_sumPt", -999);

}

void  TriggerEfficiencyCalculator_2018::createTriggerSimulatedBranches()
{
    theOutputTree_->declareUserIntBranch("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_Simulated"      , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_SimulatedUp"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_SimulatedDown"  , 0);

    theOutputTree_->declareUserIntBranch("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_SimulatedMc"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_SimulatedMcUp"  , 0);
    theOutputTree_->declareUserIntBranch("HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_SimulatedMcDown", 0);

    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt1"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt2"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt3"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_pt4"  , -999);
    theOutputTree_->declareUserFloatBranch("SimulatedTrigger_jet_sumPt", -999);
}

