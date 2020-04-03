#include "NanoAODTree.h"
#include "OutputTree.h"
#include "Jet.h"
#include "TFitResult.h"
#include "TriggerFitCurves2016.h"
// #include "TriggerFitCurves2017.h"
// #include "TriggerFitCurves2018.h"

class TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator(NanoAODTree& nat);
    virtual ~TriggerEfficiencyCalculator();

    virtual std::tuple<float, float, float> getMonteCarloTriggerEfficiency(const std::vector<Jet>& selectedJets);
    virtual std::tuple<float, float, float> getDataTriggerEfficiency      (const std::vector<Jet>& selectedJets);
    virtual std::tuple<float, float, float> getTriggerScaleFactor         (const std::vector<Jet>& selectedJets);
    virtual std::tuple<std::tuple<float, float, float>, std::tuple<float, float, float>, std::tuple<float, float, float>> getScaleFactorDataAndMonteCarloEfficiency(const std::vector<Jet>& selectedJets);
    void simulateTrigger(OutputTree* theOutputTree);
    void applyTurnOnCut(bool applyCuts) {applyTurnOnCut_ = applyCuts;}
    virtual bool isPassingTurnOnCuts(std::vector<std::string> listOfPassedTriggers, const std::vector<Jet>& selectedJets) = 0;

protected:
    virtual void   createTriggerSimulatedBranches()                              = 0;
    virtual void   extractInformationFromEvent   (std::vector<Jet> selectedJets) = 0;
    inline  float  fixInLimits                   (float efficiency)
    {
        if(efficiency > 1.) return 1.;
        if(efficiency < 0.) return 0.;
        return efficiency;
    }

    inline float getFitError(TFitResult* theFitResult, float xValue)
    {
        double xValueDouble = xValue;
        double yError = 0.;
        theFitResult->GetConfidenceIntervals(1, 1, 1, &xValueDouble, &yError, 0.68);
        return yError;
    }

    virtual std::tuple<float, float, float>  calculateDataTriggerEfficiency      () = 0;
    virtual std::tuple<float, float, float>  calculateMonteCarloTriggerEfficiency() = 0;
    NanoAODTree &theNanoAODTree_;
    OutputTree* theOutputTree_ {nullptr};
    bool simulateTrigger_ {false};
    bool applyTurnOnCut_ {false};

};


class TriggerEfficiencyCalculator_2016 : public TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator_2016(std::string inputFileName, NanoAODTree& nat);
    ~TriggerEfficiencyCalculator_2016();
    bool isPassingTurnOnCuts(std::vector<std::string> listOfPassedTriggers, const std::vector<Jet>& selectedJets) override;
    void setTurnOnCuts(float double90Double30_minSumPt, float double90Double30_minPt2, float double90Double30_minPt4, float quad45_minSumPt, float quad45_minPt4)
    {
        double90Double30_minSumPt_ = double90Double30_minSumPt;
        double90Double30_minPt2_   = double90Double30_minPt2  ;
        double90Double30_minPt4_   = double90Double30_minPt4  ;
        quad45_minSumPt_           = quad45_minSumPt          ;
        quad45_minPt4_             = quad45_minPt4            ;
    }
    
    
private:
    void  createTriggerSimulatedBranches()                                    override;
    void  extractInformationFromEvent         (std::vector<Jet> selectedJets) override;
    std::tuple<float, float, float> calculateMonteCarloTriggerEfficiency() override;
    std::tuple<float, float, float> calculateDataTriggerEfficiency      () override;

    std::tuple<float, float, float> calculateDataDouble90Double30Efficiency ();
    std::tuple<float, float, float> calculateDataQuad45Efficiency           ();
    std::tuple<float, float, float> calculateDataAndEfficiency              ();

    std::tuple<float, float, float> calculateMonteCarloDouble90Double30Efficiency ();
    std::tuple<float, float, float> calculateMonteCarloQuad45Efficiency           ();
    std::tuple<float, float, float> calculateMonteCarloAndEfficiency              ();

    inline float computeThreeBtagEfficiency(float bTagEffJet0, float bTagEffJet1, float bTagEffJet2, float bTagEffJet3)
    {
        float effJet0 = fixInLimits(bTagEffJet0);
        float effJet1 = fixInLimits(bTagEffJet1);
        float effJet2 = fixInLimits(bTagEffJet2);
        float effJet3 = fixInLimits(bTagEffJet3);

        return   effJet0   *   effJet1   *   effJet2   *   effJet3   +
               (1-effJet0) *   effJet1   *   effJet2   *   effJet3   +
                 effJet0   * (1-effJet1) *   effJet2   *   effJet3   +
                 effJet0   *   effJet1   * (1-effJet2) *   effJet3   +
                 effJet0   *   effJet1   *   effJet2   * (1-effJet3) ;
    }

    inline float computeDouble90Double30Efficiency(float bTagEffJet0, float bTagEffJet1, float bTagEffJet2, float bTagEffJet3, float effL1, float effQuad30CaloJet, float effDouble90CaloJet, float effQuad30PFJet, float effDouble90PFJet)
    {
        float bTagEfficiency = computeThreeBtagEfficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3);
        return fixInLimits(effL1) * fixInLimits(effQuad30CaloJet) * fixInLimits(effDouble90CaloJet) * bTagEfficiency * fixInLimits(effQuad30PFJet) * fixInLimits(effDouble90PFJet);
    }

    inline float computeQuad45Efficiency(float bTagEffJet0, float bTagEffJet1, float bTagEffJet2, float bTagEffJet3, float effL1, float effQuad45CaloJet, float effQuad45PFJet)
    {
        float bTagEfficiency = computeThreeBtagEfficiency(bTagEffJet0, bTagEffJet1, bTagEffJet2, bTagEffJet3);
        return fixInLimits(effL1) * fixInLimits(effQuad45CaloJet) * bTagEfficiency * fixInLimits(effQuad45PFJet);
    }

    inline float computeAndEfficiency(float effL1, float effQuad45CaloJet, float effQuad45PFJet)
    {
        return fixInLimits(effL1) * fixInLimits(effQuad45CaloJet) *fixInLimits(effQuad45PFJet);
    }

    
    TriggerFitCurves2016 fTriggerFitCurves;
    float double90Double30_minSumPt_  {0.};
    float double90Double30_minPt2_    {0.};
    float double90Double30_minPt4_    {0.};
    float quad45_minSumPt_            {0.};
    float quad45_minPt4_              {0.};
    
    float pt2_        {0.};
    float pt4_        {0.};
    float sumPt_      {0.};
    std::vector<float> deepFlavBVector {0., 0., 0., 0.} ;
};



class TriggerEfficiencyCalculator_2017 : public TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator_2017(std::string inputFileName, NanoAODTree& nat) : TriggerEfficiencyCalculator(nat) {}
    ~TriggerEfficiencyCalculator_2017() {}
    bool isPassingTurnOnCuts(std::vector<std::string> listOfPassedTriggers, const std::vector<Jet>& selectedJets) override {return false;}
    
    
private:
    void  createTriggerSimulatedBranches()                                    override;
    void  extractInformationFromEvent         (std::vector<Jet> selectedJets) override {}
    std::tuple<float, float, float> calculateMonteCarloTriggerEfficiency() override {return {1., 1., 1.};}
    std::tuple<float, float, float> calculateDataTriggerEfficiency      () override {return {1., 1., 1.};}

    // TriggerFitCurves2017 fTriggerFitCurves;
    float pt1_        {0.};
    float pt2_        {0.};
    float pt3_        {0.};
    float pt4_        {0.};
    float sumPt_      {0.};
    std::vector<float> deepFlavBVector {0., 0., 0., 0.} ;
};


class TriggerEfficiencyCalculator_2018 : public TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator_2018(std::string inputFileName, NanoAODTree& nat) : TriggerEfficiencyCalculator(nat) {}
    ~TriggerEfficiencyCalculator_2018() {}
    bool isPassingTurnOnCuts(std::vector<std::string> listOfPassedTriggers, const std::vector<Jet>& selectedJets) override {return false;}
    
    
private:
    void  createTriggerSimulatedBranches()                                    override;
    void  extractInformationFromEvent         (std::vector<Jet> selectedJets) override {}
    std::tuple<float, float, float> calculateMonteCarloTriggerEfficiency() override {return {1., 1., 1.};}
    std::tuple<float, float, float> calculateDataTriggerEfficiency      () override {return {1., 1., 1.};}

    // TriggerFitCurves2018 fTriggerFitCurves;
    float pt1_        {0.} ;
    float pt2_        {0.} ;
    float pt3_        {0.} ;
    float pt4_        {0.} ;
    float sumPt_      {0.} ;
    std::vector<float> deepFlavBVector {0., 0., 0., 0.} ;
};