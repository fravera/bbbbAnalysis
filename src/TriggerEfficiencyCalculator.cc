#include "TriggerEfficiencyCalculator.h"
#include "Muon.h"
#include "Electron.h"
#include "TRandom.h"

// #define useFit

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

TriggerEfficiencyCalculator_2016::TriggerEfficiencyCalculator_2016(std::string inputFileName, NanoAODTree& nat)
: TriggerEfficiencyCalculator(nat)
, fTriggerFitCurves(inputFileName)
{}

TriggerEfficiencyCalculator_2016::~TriggerEfficiencyCalculator_2016()
{}

bool TriggerEfficiencyCalculator_2016::isPassingTurnOnCuts(std::vector<std::string> listOfPassedTriggers, const std::vector<Jet>& selectedJets)
{
    extractInformationFromEvent(selectedJets);

    if(!applyTurnOnCut_) return true;
    bool Double90Doule30Passed = (std::find(listOfPassedTriggers.begin(), listOfPassedTriggers.end(), "HLT_DoubleJet90_Double30_TripleBTagCSV_p087") != listOfPassedTriggers.end());
    bool Quad45Passed          = (std::find(listOfPassedTriggers.begin(), listOfPassedTriggers.end(), "HLT_QuadJet45_TripleBTagCSV_p087") != listOfPassedTriggers.end());
    return (Double90Doule30Passed && sumPt_>double90Double30_minSumPt_ && pt2_>double90Double30_minPt2_ && pt4_>double90Double30_minPt4_) || (Quad45Passed && sumPt_>quad45_minSumPt_ && pt4_>quad45_minPt4_); 
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataTriggerEfficiency()
{
    std::tuple<float, float, float> Double90Double30Efficiency = calculateDataDouble90Double30Efficiency();
    std::tuple<float, float, float> Quad45Efficiency           = calculateDataQuad45Efficiency          ();
    std::tuple<float, float, float> AndEfficiency              = calculateDataAndEfficiency             ();
    if(simulateTrigger_)
    {

        if((sumPt_>double90Double30_minSumPt_ && pt2_>double90Double30_minPt2_ && pt4_>double90Double30_minPt4_) || !applyTurnOnCut_)
        {
            float Double90Double30Random = gRandom->Rndm();
            if(Double90Double30Random < std::get<0>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Simulated"    ) = 1;
            if(Double90Double30Random < std::get<1>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedUp"  ) = 1;
            if(Double90Double30Random < std::get<2>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedDown") = 1;
        }
        
        if((sumPt_>quad45_minSumPt_ && pt4_>quad45_minPt4_) || !applyTurnOnCut_)
        {
            float Quad45Random = gRandom->Rndm();
            if(Quad45Random < std::get<0>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_Simulated"    ) = 1;
            if(Quad45Random < std::get<1>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedUp"  ) = 1;
            if(Quad45Random < std::get<2>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedDown") = 1;
        }

        if(theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_Simulated"    ) || theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Simulated"    )) theOutputTree_->userInt("HLT_Simulated"    ) = 1;
        if(theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedUp"  ) || theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedUp"  )) theOutputTree_->userInt("HLT_SimulatedUp"  ) = 1;
        if(theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedDown") || theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedDown")) theOutputTree_->userInt("HLT_SimulatedDown") = 1;

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
        if((sumPt_>double90Double30_minSumPt_ && pt2_>double90Double30_minPt2_ && pt4_>double90Double30_minPt4_) || !applyTurnOnCut_)
        {
            float Double90Double30Random = gRandom->Rndm();
            if(Double90Double30Random < std::get<0>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMc"    ) = 1;
            if(Double90Double30Random < std::get<1>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcUp"  ) = 1;
            if(Double90Double30Random < std::get<2>(Double90Double30Efficiency)) theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcDown") = 1;
        }

        if((sumPt_>quad45_minSumPt_ && pt4_>quad45_minPt4_) || !applyTurnOnCut_)
        {
            float Quad45Random = gRandom->Rndm();
            if(Quad45Random < std::get<0>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMc"    ) = 1;
            if(Quad45Random < std::get<1>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcUp"  ) = 1;
            if(Quad45Random < std::get<2>(Quad45Efficiency          )) theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcDown") = 1;
        }

        if(theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMc"    ) || theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMc"    )) theOutputTree_->userInt("HLT_SimulatedMc"    ) = 1;
        if(theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcUp"  ) || theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcUp"  )) theOutputTree_->userInt("HLT_SimulatedMcUp"  ) = 1;
        if(theOutputTree_->userInt("HLT_QuadJet45_TripleBTagCSV_p087_SimulatedMcDown") || theOutputTree_->userInt("HLT_DoubleJet90_Double30_TripleBTagCSV_p087_SimulatedMcDown")) theOutputTree_->userInt("HLT_SimulatedMcDown") = 1;

    }


    // std::cout << "MonteCarlo Efficiency -> Double90Double30Efficiency = " << Double90Double30Efficiency << " Quad45Efficiency = " << Quad45Efficiency << " AndEfficiency = " << AndEfficiency << " Double90Double30Efficiency * AndEfficiency = " << (Double90Double30Efficiency * AndEfficiency) << std::endl;
    float efficiencyCentral = std::get<0>(Double90Double30Efficiency) + std::get<0>(Quad45Efficiency) - (std::get<0>(Double90Double30Efficiency) * std::get<0>(AndEfficiency));
    float efficiencyUp      = std::get<1>(Double90Double30Efficiency) + std::get<1>(Quad45Efficiency) - (std::get<1>(Double90Double30Efficiency) * std::get<2>(AndEfficiency));
    float efficiencyDown    = std::get<2>(Double90Double30Efficiency) + std::get<2>(Quad45Efficiency) - (std::get<2>(Double90Double30Efficiency) * std::get<1>(AndEfficiency));
    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataDouble90Double30Efficiency()
{
    #ifdef useFit
        float bTagEffJet0        = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[0]);
        float bTagEffJet1        = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[1]);
        float bTagEffJet2        = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[2]);
        float bTagEffJet3        = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[3]);
        float bTagEffJet0Error        = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[0]);
        float bTagEffJet1Error        = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[1]);
        float bTagEffJet2Error        = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[2]);
        float bTagEffJet3Error        = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[3]);
    #else
        float bTagEffJet0        = std::get<0>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1        = std::get<0>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2        = std::get<0>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3        = std::get<0>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
    #endif
    
    float effL1              = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_L1filterHTPair.first                 ->Eval(sumPt_);
    float effQuad30CaloJet   = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30Pair.first           ->Eval(pt4_  );
    float effDouble90CaloJet = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90Pair.first         ->Eval(pt2_  );
    float effQuad30PFJet     = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.first  ->Eval(pt4_  );
    float effDouble90PFJet   = fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.first->Eval(pt2_  );
    
    float effL1Error              = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_L1filterHTPair.second                 , sumPt_            );
    float effQuad30CaloJetError   = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30Pair.second           , pt4_              );
    float effDouble90CaloJetError = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90Pair.second         , pt2_              );
    float effQuad30PFJetError     = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.second  , pt4_              );
    float effDouble90PFJetError   = getFitError(fTriggerFitCurves.fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.second, pt2_              );


    float efficiencyCentral = computeDouble90Double30Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad30CaloJet, effDouble90CaloJet, effQuad30PFJet, effDouble90PFJet);
    #ifdef useFit
        float efficiencyUp      = computeDouble90Double30Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad30CaloJet + effQuad30CaloJetError, effDouble90CaloJet + effDouble90CaloJetError, effQuad30PFJet + effQuad30PFJetError, effDouble90PFJet + effDouble90PFJetError);
        float efficiencyDown    = computeDouble90Double30Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad30CaloJet - effQuad30CaloJetError, effDouble90CaloJet - effDouble90CaloJetError, effQuad30PFJet - effQuad30PFJetError, effDouble90PFJet - effDouble90PFJetError);
    #else
        float efficiencyUp      = computeDouble90Double30Efficiency(bTagEffJet0Up  , bTagEffJet1Up  , bTagEffJet2Up  , bTagEffJet3Up  , effL1 + effL1Error, effQuad30CaloJet + effQuad30CaloJetError, effDouble90CaloJet + effDouble90CaloJetError, effQuad30PFJet + effQuad30PFJetError, effDouble90PFJet + effDouble90PFJetError);
        float efficiencyDown    = computeDouble90Double30Efficiency(bTagEffJet0Down, bTagEffJet1Down, bTagEffJet2Down, bTagEffJet3Down, effL1 - effL1Error, effQuad30CaloJet - effQuad30CaloJetError, effDouble90CaloJet - effDouble90CaloJetError, effQuad30PFJet - effQuad30PFJetError, effDouble90PFJet - effDouble90PFJetError);
    #endif

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataQuad45Efficiency()
{
    
    #ifdef useFit
        float bTagEffJet0      = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[0]);
        float bTagEffJet1      = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[1]);
        float bTagEffJet2      = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[2]);
        float bTagEffJet3      = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[3]);
        float bTagEffJet0Error      = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[0]);
        float bTagEffJet1Error      = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[1]);
        float bTagEffJet2Error      = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[2]);
        float bTagEffJet3Error      = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[3]);
    #else
        float bTagEffJet0        = std::get<0>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1        = std::get<0>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2        = std::get<0>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3        = std::get<0>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Up      = std::get<1>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Down    = std::get<2>(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
    #endif

    float effL1            = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_L1filterHTPair.first               ->Eval(sumPt_            );
    float effQuad45CaloJet = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_              );
    float effQuad45PFJet   = fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_              );

    float effL1Error            = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_L1filterHTPair.second               , sumPt_            );
    float effQuad45CaloJetError = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_QuadCentralJet45Pair.second         , pt4_              );
    float effQuad45PFJetError   = getFitError(fTriggerFitCurves.fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_              );

    float efficiencyCentral = computeQuad45Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad45CaloJet, effQuad45PFJet);
    #ifdef useFit
        float efficiencyUp      = computeQuad45Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
        float efficiencyDown    = computeQuad45Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);
    #else
        float efficiencyUp      = computeQuad45Efficiency(bTagEffJet0Up  , bTagEffJet1Up  , bTagEffJet2Up  , bTagEffJet3Up  , effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
        float efficiencyDown    = computeQuad45Efficiency(bTagEffJet0Down, bTagEffJet1Down, bTagEffJet2Down, bTagEffJet3Down, effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);
    #endif

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateDataAndEfficiency()
{
    float effL1            = fTriggerFitCurves.fSingleMuon_And_Efficiency_L1filterQuad45HTPair.first         ->Eval(sumPt_);
    float effQuad45CaloJet = fTriggerFitCurves.fSingleMuon_And_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_  );
    float effQuad45PFJet   = fTriggerFitCurves.fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_  );

    float effL1Error            = getFitError(fTriggerFitCurves.fSingleMuon_And_Efficiency_L1filterQuad45HTPair.second         , sumPt_);
    float effQuad45CaloJetError = getFitError(fTriggerFitCurves.fSingleMuon_And_Efficiency_QuadCentralJet45Pair.second         , pt4_  );
    float effQuad45PFJetError   = getFitError(fTriggerFitCurves.fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_  );

    float efficiencyCentral = computeAndEfficiency(effL1, effQuad45CaloJet, effQuad45PFJet);
    float efficiencyUp      = computeAndEfficiency(effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
    float efficiencyDown    = computeAndEfficiency(effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}


std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloDouble90Double30Efficiency()
{
    #ifdef useFit
        float bTagEffJet0        = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[0]);
        float bTagEffJet1        = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[1]);
        float bTagEffJet2        = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[2]);
        float bTagEffJet3        = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.first      ->Eval(deepFlavBVector[3]);
        float bTagEffJet0Error        = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[0]);
        float bTagEffJet1Error        = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[1]);
        float bTagEffJet2Error        = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[2]);
        float bTagEffJet3Error        = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TriplePair.second      , deepFlavBVector[3]);
    #else
        float bTagEffJet0        = std::get<0>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1        = std::get<0>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2        = std::get<0>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3        = std::get<0>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Up      = std::get<1>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Up      = std::get<1>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Up      = std::get<1>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Up      = std::get<1>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Down    = std::get<2>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Down    = std::get<2>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Down    = std::get<2>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Down    = std::get<2>(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
    #endif

    float effL1              = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_L1filterHTPair.first                 ->Eval(sumPt_            );
    float effQuad30CaloJet   = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_QuadCentralJet30Pair.first           ->Eval(pt4_              );
    float effDouble90CaloJet = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90Pair.first         ->Eval(pt2_              );
    float effQuad30PFJet     = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.first  ->Eval(pt4_              );
    float effDouble90PFJet   = fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.first->Eval(pt2_              );

    float effL1Error              = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_L1filterHTPair.second                 , sumPt_            );
    float effQuad30CaloJetError   = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_QuadCentralJet30Pair.second           , pt4_              );
    float effDouble90CaloJetError = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90Pair.second         , pt2_              );
    float effQuad30PFJetError     = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30Pair.second  , pt4_              );
    float effDouble90PFJetError   = getFitError(fTriggerFitCurves.fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90Pair.second, pt2_              );


    float efficiencyCentral = computeDouble90Double30Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad30CaloJet, effDouble90CaloJet, effQuad30PFJet, effDouble90PFJet);
    #ifdef useFit
        float efficiencyUp      = computeDouble90Double30Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad30CaloJet + effQuad30CaloJetError, effDouble90CaloJet + effDouble90CaloJetError, effQuad30PFJet + effQuad30PFJetError, effDouble90PFJet + effDouble90PFJetError);
        float efficiencyDown    = computeDouble90Double30Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad30CaloJet - effQuad30CaloJetError, effDouble90CaloJet - effDouble90CaloJetError, effQuad30PFJet - effQuad30PFJetError, effDouble90PFJet - effDouble90PFJetError);
    #else
        float efficiencyUp      = computeDouble90Double30Efficiency(bTagEffJet0Up  , bTagEffJet1Up  , bTagEffJet2Up  , bTagEffJet3Up  , effL1 + effL1Error, effQuad30CaloJet + effQuad30CaloJetError, effDouble90CaloJet + effDouble90CaloJetError, effQuad30PFJet + effQuad30PFJetError, effDouble90PFJet + effDouble90PFJetError);
        float efficiencyDown    = computeDouble90Double30Efficiency(bTagEffJet0Down, bTagEffJet1Down, bTagEffJet2Down, bTagEffJet3Down, effL1 - effL1Error, effQuad30CaloJet - effQuad30CaloJetError, effDouble90CaloJet - effDouble90CaloJetError, effQuad30PFJet - effQuad30PFJetError, effDouble90PFJet - effDouble90PFJetError);
    #endif

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloQuad45Efficiency()
{
    
    #ifdef useFit
        float bTagEffJet0      = fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[0]);
        float bTagEffJet1      = fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[1]);
        float bTagEffJet2      = fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[2]);
        float bTagEffJet3      = fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.first    ->Eval(deepFlavBVector[3]);
        float bTagEffJet0Error      = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[0]);
        float bTagEffJet1Error      = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[1]);
        float bTagEffJet2Error      = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[2]);
        float bTagEffJet3Error      = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TriplePair.second    , deepFlavBVector[3]);
    #else
        float bTagEffJet0        = std::get<0>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1        = std::get<0>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2        = std::get<0>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3        = std::get<0>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Up      = std::get<1>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Up      = std::get<1>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Up      = std::get<1>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Up      = std::get<1>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
        float bTagEffJet0Down    = std::get<2>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[0], 0, "S");
        float bTagEffJet1Down    = std::get<2>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[1], 0, "S");
        float bTagEffJet2Down    = std::get<2>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[2], 0, "S");
        float bTagEffJet3Down    = std::get<2>(fTriggerFitCurves.fTTbar_Quad45_Efficiency_BTagCaloCSVp087TripleGraphs)  ->Eval(deepFlavBVector[3], 0, "S");
    #endif

    float effL1            = fTriggerFitCurves.fTTbar_Quad45_Efficiency_L1filterHTPair.first               ->Eval(sumPt_            );
    float effQuad45CaloJet = fTriggerFitCurves.fTTbar_Quad45_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_              );
    float effQuad45PFJet   = fTriggerFitCurves.fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_              );

    float effL1Error            = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_L1filterHTPair.second               , sumPt_            );
    float effQuad45CaloJetError = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_QuadCentralJet45Pair.second         , pt4_              );
    float effQuad45PFJetError   = getFitError(fTriggerFitCurves.fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_              );

    float efficiencyCentral = computeQuad45Efficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3, effL1, effQuad45CaloJet, effQuad45PFJet);
    #ifdef useFit
        float efficiencyUp      = computeQuad45Efficiency(bTagEffJet0 + bTagEffJet0Error, bTagEffJet1 + bTagEffJet1Error, bTagEffJet2 + bTagEffJet2Error, bTagEffJet3 + bTagEffJet3Error, effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
        float efficiencyDown    = computeQuad45Efficiency(bTagEffJet0 - bTagEffJet0Error, bTagEffJet1 - bTagEffJet1Error, bTagEffJet2 - bTagEffJet2Error, bTagEffJet3 - bTagEffJet3Error, effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);
    #else
        float efficiencyUp      = computeQuad45Efficiency(bTagEffJet0Up  , bTagEffJet1Up  , bTagEffJet2Up  , bTagEffJet3Up  , effL1 + effL1Error, effQuad45CaloJet + effQuad45CaloJetError, effQuad45PFJet + effQuad45PFJetError);
        float efficiencyDown    = computeQuad45Efficiency(bTagEffJet0Down, bTagEffJet1Down, bTagEffJet2Down, bTagEffJet3Down, effL1 - effL1Error, effQuad45CaloJet - effQuad45CaloJetError, effQuad45PFJet - effQuad45PFJetError);
    #endif

    return {efficiencyCentral, efficiencyUp, efficiencyDown};
}

std::tuple<float, float, float> TriggerEfficiencyCalculator_2016::calculateMonteCarloAndEfficiency()
{
    float effL1            = fTriggerFitCurves.fTTbar_And_Efficiency_L1filterQuad45HTPair.first         ->Eval(sumPt_);
    float effQuad45CaloJet = fTriggerFitCurves.fTTbar_And_Efficiency_QuadCentralJet45Pair.first         ->Eval(pt4_  );
    float effQuad45PFJet   = fTriggerFitCurves.fTTbar_And_Efficiency_QuadPFCentralJetLooseID45Pair.first->Eval(pt4_  );

    float effL1Error            = getFitError(fTriggerFitCurves.fTTbar_And_Efficiency_L1filterQuad45HTPair.second         , sumPt_);
    float effQuad45CaloJetError = getFitError(fTriggerFitCurves.fTTbar_And_Efficiency_QuadCentralJet45Pair.second         , pt4_  );
    float effQuad45PFJetError   = getFitError(fTriggerFitCurves.fTTbar_And_Efficiency_QuadPFCentralJetLooseID45Pair.second, pt4_  );

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

    theOutputTree_->userInt("HLT_Pt2"  ) = pt2_  ;
    theOutputTree_->userInt("HLT_Pt4"  ) = pt4_  ;
    theOutputTree_->userInt("HLT_SumPt") = sumPt_;

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

    theOutputTree_->declareUserIntBranch("HLT_Simulated"                 , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedUp"               , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedDown"             , 0);

    theOutputTree_->declareUserIntBranch("HLT_SimulatedMc"               , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedMcUp"             , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedMcDown"           , 0);

    theOutputTree_->declareUserIntBranch("HLT_Pt2"             , 0);
    theOutputTree_->declareUserIntBranch("HLT_Pt4"             , 0);
    theOutputTree_->declareUserIntBranch("HLT_SumPt"           , 0);

}

void  TriggerEfficiencyCalculator_2017::createTriggerSimulatedBranches()
{
    theOutputTree_->declareUserIntBranch("HLT_Simulated"      , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedUp"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedDown"  , 0);

    theOutputTree_->declareUserIntBranch("HLT_SimulatedMc"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedMcUp"  , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedMcDown", 0);
}

void  TriggerEfficiencyCalculator_2018::createTriggerSimulatedBranches()
{
    theOutputTree_->declareUserIntBranch("HLT_Simulated"      , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedUp"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedDown"  , 0);

    theOutputTree_->declareUserIntBranch("HLT_SimulatedMc"    , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedMcUp"  , 0);
    theOutputTree_->declareUserIntBranch("HLT_SimulatedMcDown", 0);
}

