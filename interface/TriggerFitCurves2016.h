#include "Riostream.h"
#include "TF1.h"

struct TriggerFitCurves2016
{
    TriggerFitCurves2016()
    : fSingleMuon_Double90Quad30_Efficiency_L1filterHT                  (TF1("SingleMuon_Double90Quad30_Efficiency_L1filterHT","ROOT::Math::crystalball_cdf(x,2.96227,1.00114,103.889,295.848)*(0.5*(1+TMath::Erf((x-295.848)/86.726)))*0.999773+0.000107826",100.000000,1500.000000))
    , fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30            (TF1("SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30","ROOT::Math::crystalball_cdf(x,9.54607,2.12769,7.33354,25.4458)*(0.5*(1+TMath::Erf((x-25.4458)/20.6385)))*0.976692+5.93664e-12",25.000000,300.000000))
    , fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90          (TF1("SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90","ROOT::Math::crystalball_cdf(x,9.89521,1.0145,8.45356,80.1383)*(0.5*(1+TMath::Erf((x-80.1383)/32.8662)))*0.980683+0.0160638",30.000000,500.000000))
    , fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple       (TF1("SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple","(0.0416966+2.54921*x+-3.25547*pow(x,2)+1.60967*pow(x,3))",0.200000,1.000000))
    , fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30   (TF1("SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30","ROOT::Math::crystalball_cdf(x,8.85007,1.0036,7.9301,27.8303)*0.991403+1.99941e-10",25.000000,300.000000))
    , fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90 (TF1("SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90","ROOT::Math::crystalball_cdf(x,3.46867,5.44473,8.61156,91.6683)*0.777159+0.219625",70.000000,500.000000))
    , fSingleMuon_Quad45_Efficiency_L1filterHT                          (TF1("SingleMuon_Quad45_Efficiency_L1filterHT","ROOT::Math::crystalball_cdf(x,2.98916,1.00101,112.51,297.213)*(0.5*(1+TMath::Erf((x-297.213)/89.4935)))*0.997004+1.03565e-11",100.000000,1500.000000))
    , fSingleMuon_Quad45_Efficiency_QuadCentralJet45                    (TF1("SingleMuon_Quad45_Efficiency_QuadCentralJet45","ROOT::Math::crystalball_cdf(x,9.52368,1.11471,7.85323,40.084)*(0.5*(1+TMath::Erf((x-40.084)/23.078)))*0.958955+0.00637715",30.000000,300.000000))
    , fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple               (TF1("SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple","(0.0730663+2.45211*x+-3.15248*pow(x,2)+1.57081*pow(x,3))",0.200000,1.000000))
    , fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45           (TF1("SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45","ROOT::Math::crystalball_cdf(x,3.10964,3.53177,6.46168,45.673)*0.805303+0.184914",30.000000,300.000000))
    , fSingleMuon_And_Efficiency_L1filterQuad45HT                       (TF1("SingleMuon_And_Efficiency_L1filterQuad45HT","ROOT::Math::crystalball_cdf(x,0.163113,2.19583,88.9264,330.731)*(0.5*(1+TMath::Erf((x-330.731)/15.5691)))*0.108772+0.890417",250.000000,1500.000000))
    , fSingleMuon_And_Efficiency_QuadCentralJet45                       (TF1("SingleMuon_And_Efficiency_QuadCentralJet45","ROOT::Math::crystalball_cdf(x,1.8114,1.05234,4.96533,41.4225)*(0.5*(1+TMath::Erf((x-41.4225)/22.5681)))*0.979792+0.0191887",30.000000,300.000000))
    , fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45              (TF1("SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45","ROOT::Math::crystalball_cdf(x,3.29482,3.91065,6.13278,45.8185)*0.764853+0.230565",30.000000,300.000000))
    , fTTbar_Double90Quad30_Efficiency_L1filterHT                       (TF1("TTbar_Double90Quad30_Efficiency_L1filterHT","ROOT::Math::crystalball_cdf(x,3.27321,1.00245,84.9528,261.853)*(0.5*(1+TMath::Erf((x-261.853)/85.3202)))*0.982501+0.0174295",100.000000,1500.000000))
    , fTTbar_Double90Quad30_Efficiency_QuadCentralJet30                 (TF1("TTbar_Double90Quad30_Efficiency_QuadCentralJet30","ROOT::Math::crystalball_cdf(x,8.5876,8.93638,6.50331,25.159)*(0.5*(1+TMath::Erf((x-25.159)/18.2408)))*0.971524+0.0106722",25.000000,300.000000))
    , fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90               (TF1("TTbar_Double90Quad30_Efficiency_DoubleCentralJet90","ROOT::Math::crystalball_cdf(x,7.90176,9.93504,8.35642,79.1509)*(0.5*(1+TMath::Erf((x-79.1509)/35.5726)))*0.98656+0.0107251",50.000000,500.000000))
    , fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple            (TF1("TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple","(0.214623+2.1912*x+-2.89593*pow(x,2)+1.43433*pow(x,3))",0.200000,1.000000))
    , fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30        (TF1("TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30","ROOT::Math::crystalball_cdf(x,9.28171,2.8133,7.66284,27.1963)*0.990363+2.02195e-08",25.000000,300.000000))
    , fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90      (TF1("TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90","ROOT::Math::crystalball_cdf(x,5.2022,9.6722,12.3635,83.7059)*0.998589+0.000301971",70.000000,500.000000))
    , fTTbar_Quad45_Efficiency_L1filterHT                               (TF1("TTbar_Quad45_Efficiency_L1filterHT","ROOT::Math::crystalball_cdf(x,2.42513,1.00102,121.386,287.636)*(0.5*(1+TMath::Erf((x-287.636)/92.0676)))*0.9933+0.00588846",100.000000,1500.000000))
    , fTTbar_Quad45_Efficiency_QuadCentralJet45                         (TF1("TTbar_Quad45_Efficiency_QuadCentralJet45","ROOT::Math::crystalball_cdf(x,6.0779,9.77709,7.84855,39.5498)*(0.5*(1+TMath::Erf((x-39.5498)/21.1253)))*0.966494+0.00310594",30.000000,300.000000))
    , fTTbar_Quad45_Efficiency_BTagCaloCSVp087Triple                    (TF1("TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple","(0.182741+2.3614*x+-3.17037*pow(x,2)+1.57383*pow(x,3))",0.200000,1.000000))
    , fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45                (TF1("TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45","ROOT::Math::crystalball_cdf(x,3.27081,3.84217,6.97708,44.9491)*0.818001+0.172963",30.000000,300.000000))
    , fTTbar_And_Efficiency_L1filterQuad45HT                            (TF1("TTbar_And_Efficiency_L1filterQuad45HT","ROOT::Math::crystalball_cdf(x,2.64372,1.00137,99.7716,178.241)*(0.5*(1+TMath::Erf((x-178.241)/39.0175)))*0.999972+2.79833e-05",250.000000,1500.000000))
    , fTTbar_And_Efficiency_QuadCentralJet45                            (TF1("TTbar_And_Efficiency_QuadCentralJet45","ROOT::Math::crystalball_cdf(x,5.66907,1.9997,7.59649,38.9601)*(0.5*(1+TMath::Erf((x-38.9601)/22.2368)))*0.918461+0.072993",30.000000,300.000000))
    , fTTbar_And_Efficiency_QuadPFCentralJetLooseID45                   (TF1("TTbar_And_Efficiency_QuadPFCentralJetLooseID45","ROOT::Math::crystalball_cdf(x,3.2353,3.6422,6.30335,44.8307)*0.776785+0.220449",30.000000,300.000000))
    {}


    TF1 fSingleMuon_Double90Quad30_Efficiency_L1filterHT                 ;
    TF1 fSingleMuon_Double90Quad30_Efficiency_QuadCentralJet30           ;
    TF1 fSingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90         ;
    TF1 fSingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple      ;
    TF1 fSingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30  ;
    TF1 fSingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90;
    TF1 fSingleMuon_Quad45_Efficiency_L1filterHT                         ;
    TF1 fSingleMuon_Quad45_Efficiency_QuadCentralJet45                   ;
    TF1 fSingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple              ;
    TF1 fSingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45          ;
    TF1 fSingleMuon_And_Efficiency_L1filterQuad45HT                      ;
    TF1 fSingleMuon_And_Efficiency_QuadCentralJet45                      ;
    TF1 fSingleMuon_And_Efficiency_QuadPFCentralJetLooseID45             ;
    TF1 fTTbar_Double90Quad30_Efficiency_L1filterHT                      ;
    TF1 fTTbar_Double90Quad30_Efficiency_QuadCentralJet30                ;
    TF1 fTTbar_Double90Quad30_Efficiency_DoubleCentralJet90              ;
    TF1 fTTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple           ;
    TF1 fTTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30       ;
    TF1 fTTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90     ;
    TF1 fTTbar_Quad45_Efficiency_L1filterHT                              ;
    TF1 fTTbar_Quad45_Efficiency_QuadCentralJet45                        ;
    TF1 fTTbar_Quad45_Efficiency_BTagCaloCSVp087Triple                   ;
    TF1 fTTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45               ;
    TF1 fTTbar_And_Efficiency_L1filterQuad45HT                           ;
    TF1 fTTbar_And_Efficiency_QuadCentralJet45                           ;
    TF1 fTTbar_And_Efficiency_QuadPFCentralJetLooseID45                  ;
    
};
