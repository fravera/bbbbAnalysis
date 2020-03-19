#include "NanoAODTree.h"
#include "OutputTree.h"
#include "Jet.h"
#include "TFitResult.h"

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

};


class TriggerEfficiencyCalculator_2016 : public TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator_2016(NanoAODTree& nat);
    ~TriggerEfficiencyCalculator_2016();
    
    
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
    
    float pt2_        {0.} ;
    float pt4_        {0.} ;
    float sumPt_      {0.} ;
    std::vector<float> deepFlavBVector {0., 0., 0., 0.} ;
};



class TriggerEfficiencyCalculator_2017 : public TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator_2017(NanoAODTree& nat) : TriggerEfficiencyCalculator(nat) {}
    ~TriggerEfficiencyCalculator_2017() {}
    
    
private:
    void  createTriggerSimulatedBranches()                                    override;
    void  extractInformationFromEvent         (std::vector<Jet> selectedJets) override {}
    std::tuple<float, float, float> calculateMonteCarloTriggerEfficiency() override {return {1., 1., 1.};}
    std::tuple<float, float, float> calculateDataTriggerEfficiency      () override {return {1., 1., 1.};}

    // static TriggerFitCurves2017 fTriggerFitCurves2017;

    float pt1_        {0.} ;
    float pt2_        {0.} ;
    float pt3_        {0.} ;
    float pt4_        {0.} ;
    float sumPt_      {0.} ;
    std::vector<float> deepFlavBVector {0., 0., 0., 0.} ;
};


class TriggerEfficiencyCalculator_2018 : public TriggerEfficiencyCalculator
{
public:
    TriggerEfficiencyCalculator_2018(NanoAODTree& nat) : TriggerEfficiencyCalculator(nat) {}
    ~TriggerEfficiencyCalculator_2018() {}
    
    
private:
    void  createTriggerSimulatedBranches()                                    override;
    void  extractInformationFromEvent         (std::vector<Jet> selectedJets) override {}
    std::tuple<float, float, float> calculateMonteCarloTriggerEfficiency() override {return {1., 1., 1.};}
    std::tuple<float, float, float> calculateDataTriggerEfficiency      () override {return {1., 1., 1.};}

    // static TriggerFitCurves2018 fTriggerFitCurves2018;

    float pt1_        {0.} ;
    float pt2_        {0.} ;
    float pt3_        {0.} ;
    float pt4_        {0.} ;
    float sumPt_      {0.} ;
    std::vector<float> deepFlavBVector {0., 0., 0., 0.} ;
};