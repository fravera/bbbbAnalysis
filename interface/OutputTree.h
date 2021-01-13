#ifndef OUTPUTTREE_H
#define OUTPUTTREE_H

/**
** class  : OutputTree
** author : L. Cadamuro (UF)
** date   : 10/01/2018
** brief  : the output tree of the skims
** NOTE   : for every new variable added:
**        : 1) add the member variable
**        : 2) add the corresponing branch in init_branches()
**        : 3) set the variable clearing in clear()
**        :
**        : remember that the variables are set (from the EventInfo) in the SkimUtils::fill_output_tree function
**        : so put the set instructions of the new variables there
**/

#include "TTree.h"
#include "TLorentzVector.h"
#include "UserValCollection.h"
#include <string>
#include <memory>

// helper: declares the m/pt/eta/phi/p4 of a variable OBJ
#define DECLARE_m_pt_eta_phi_p4(OBJ) \
    float OBJ ## _m; \
    float OBJ ## _pt; \
    float OBJ ## _eta; \
    float OBJ ## _phi; \
    TLorentzVector OBJ ## _p4;

// helper: declares the m/pt/eta/phi/p4 of a variable OBJ
#define DECLARE_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    float OBJ ## _m; \
    float OBJ ## _pt; \
    float OBJ ## _ptRegressed; \
    float OBJ ## _eta; \
    float OBJ ## _phi; \
    TLorentzVector OBJ ## _p4;

class OutputTree {
    
    public:
        OutputTree (bool savetlv = false, std::string name = "bbbbTree", std::string title = "bbbbTree", bool initAllBranches = true);
        ~OutputTree(){};
        
        void clear(); // to be called to reset the var values
        int fill()  {return tree_->Fill();}
        int write() {return tree_->Write();}
    
        // returns false if the branch could not be created, true if all ok
        // thje second optional value specifies what the branch should be reset to at clear()
        bool declareUserIntBranch   (std::string name, int defaultClearValue = 0);
        bool declareUserFloatBranch (std::string name, float defaultClearValue = 0.0);

        bool declareUserIntBranchList(std::vector<std::string> nameList, int defaultClearValue = 0);
        
        // throws an exception if the branch name was not declared
        int&   userInt   (std::string name) {return userInts_   . getVal(name);}
        float& userFloat (std::string name) {return userFloats_ . getVal(name);}

        //////////////////////////
        //// saved variables
        //////////////////////////

        unsigned int           Run;
        unsigned int           LumiSec;
        unsigned long long int Event;
        unsigned int           pileUp;
        //////////////////----- reco H1 and H2
        DECLARE_m_pt_eta_phi_p4(H1)
        float H1_bb_DeltaR;
        DECLARE_m_pt_eta_phi_p4(H2)
        float H2_bb_DeltaR;
        DECLARE_m_pt_eta_phi_p4(HH)
        float HH_2DdeltaM;
        float H1_H2_sphericity;
        float FourBjet_sphericity;

        DECLARE_m_pt_eta_phi_p4(offShell_H1)
        float offShell_H1_bb_DeltaR;
        DECLARE_m_pt_eta_phi_p4(offShell_H2)
        float offShell_H2_bb_DeltaR;
        DECLARE_m_pt_eta_phi_p4(offShell_HH)

        DECLARE_m_pt_eta_phi_p4(H1_kinFit)
        DECLARE_m_pt_eta_phi_p4(H2_kinFit)
        DECLARE_m_pt_eta_phi_p4(HH_kinFit)
        float H1_kinFit_bb_DeltaR;
        float H2_kinFit_bb_DeltaR;
        float kinFit_chi2;
        float abs_costh_H1_ggfcm;
        float abs_costh_H2_ggfcm;

        DECLARE_m_pt_ptRegressed_eta_phi_p4(H1_b1_kinFit)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H1_b2_kinFit)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H2_b1_kinFit)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H2_b2_kinFit)

        DECLARE_m_pt_ptRegressed_eta_phi_p4(FirstBtaggedJet)
        float FirstBtaggedJet_deepCSV  ;
        float FirstBtaggedJet_PtRegRes ;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(SecondBtaggedJet)
        float SecondBtaggedJet_deepCSV ;
        float SecondBtaggedJet_PtRegRes;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(ThirdBtaggedJet)
        float ThirdBtaggedJet_deepCSV  ;
        float ThirdBtaggedJet_PtRegRes ;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(FourthBtaggedJet)
        float FourthBtaggedJet_deepCSV ;
        float FourthBtaggedJet_PtRegRes;

        float minDeltaRbJets  ;
        float maxDeltaRbJets  ;
        float minDeltaEtabJets;
        float maxDeltaEtabJets;
        float minDeltaPhibJets;
        float maxDeltaPhibJets;

        DECLARE_m_pt_ptRegressed_eta_phi_p4(offShell_H1_b1)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(offShell_H1_b2)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(offShell_H2_b1)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(offShell_H2_b2)

        //////////////////----- reco bs
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H1_b1)
        float H1_b1_deepCSV;
        int H1_b1_jetId;
        int H1_b1_puId;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H1_b2)
        float H1_b2_deepCSV;
        int H1_b2_jetId;
        int H1_b2_puId;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H2_b1)
        float H2_b1_deepCSV;
        int H2_b1_jetId;
        int H2_b1_puId;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(H2_b2)
        float H2_b2_deepCSV;
        int H2_b2_jetId;
        int H2_b2_puId;
        //////////////////----- gen level
        float gen_mHH;
        float gen_mJJ;
        float gen_deltaEtaJJ;
        DECLARE_m_pt_eta_phi_p4(gen_H1)
        DECLARE_m_pt_eta_phi_p4(gen_H2)
        DECLARE_m_pt_eta_phi_p4(gen_H1_last)
        DECLARE_m_pt_eta_phi_p4(gen_H2_last)
        DECLARE_m_pt_eta_phi_p4(gen_H1_b1)
        float gen_H1_b1_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_H1_b2)
        float gen_H1_b2_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_H2_b1)
        float gen_H2_b1_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_H2_b2)
        float gen_H2_b2_matchedflag;
        DECLARE_m_pt_eta_phi_p4(gen_q1_in)
        DECLARE_m_pt_eta_phi_p4(gen_q2_in)
        DECLARE_m_pt_eta_phi_p4(gen_q1_out)
        DECLARE_m_pt_eta_phi_p4(gen_q2_out)

        float gen_H1_b1_matchedflag_swapped;
        float gen_H1_b2_matchedflag_swapped;
        float gen_H2_b1_matchedflag_swapped;
        float gen_H2_b2_matchedflag_swapped;


    // Suzanne added these variables

        float recoJetMatchedToGenJet1;
        float recoJetMatchedToGenJet2;
        float recoJetMatchedToGenJet3;
        float recoJetMatchedToGenJet4;

        std::vector<double> gen_jet_pt;
        float jet_pt;




        float triggerScaleFactor       ;
        float triggerDataEfficiency    ;
        float triggerMcEfficiency      ;
        float triggerScaleFactorUp     ;
        float triggerDataEfficiencyUp  ;
        float triggerMcEfficiencyUp    ;
        float triggerScaleFactorDown   ;
        float triggerDataEfficiencyDown;
        float triggerMcEfficiencyDown  ;

        //TTEMU events
        DECLARE_m_pt_ptRegressed_eta_phi_p4(TT_b1)
        float TT_b1_deepCSV;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(TT_b2)
        float TT_b2_deepCSV;
        DECLARE_m_pt_eta_phi_p4(TT_e)
        DECLARE_m_pt_eta_phi_p4(TT_mu)
        float TT_m;
        int TT_nPV;
        int TT_nPVgood;
        int TT_nJet;                        
        /////////////////---Non-resonant analysis and studies
        float H1_b1_genJetIdx;
        float H1_b2_genJetIdx;
        float H2_b1_genJetIdx;
        float H2_b2_genJetIdx;
        float H1_b1_rawpt;
        float H1_b2_rawpt;
        float H2_b1_rawpt;
        float H2_b2_rawpt;
        float gen_H1_b1_mindRgenjet;
        float gen_H1_b2_mindRgenjet;
        float gen_H2_b1_mindRgenjet;
        float gen_H2_b2_mindRgenjet;
        float gen_q1_out_mindRgenjet;
        float gen_q2_out_mindRgenjet;
        float gen_H1_b1_mindRjet;
        float gen_H1_b2_mindRjet;
        float gen_H2_b1_mindRjet;
        float gen_H2_b2_mindRjet;
        float gen_q1_out_mindRjet;
        float gen_q2_out_mindRjet;
        int gen_H1_b1_matched;
        int gen_H1_b2_matched;
        int gen_H2_b1_matched;
        int gen_H2_b2_matched;
        int gen_q1_out_matched;
        int gen_q2_out_matched;
        float gen_H1_b1_ptratio;
        float gen_H1_b2_ptratio;
        float gen_H2_b1_ptratio;
        float gen_H2_b2_ptratio;
        float gen_q1_out_ptratio;
        float gen_q2_out_ptratio;
        float gen_H1_b1_pz;
        float gen_H1_b2_pz;
        float gen_H2_b1_pz;
        float gen_H2_b2_pz;
        float gen_q1_out_pz;
        float gen_q2_out_pz;
        int gen_H1_b1_pdgId;
        int gen_H1_b2_pdgId;
        int gen_H2_b1_pdgId;
        int gen_H2_b2_pdgId;
        int gen_q1_out_pdgId;
        int gen_q2_out_pdgId;
        int H1_b1_quarkID;
        int H1_b2_quarkID;
        int H2_b1_quarkID;
        int H2_b2_quarkID;
        int H1_b1_partonFlavour;
        int H1_b2_partonFlavour;
        int H2_b1_partonFlavour;
        int H2_b2_partonFlavour;
        int H1_b1_hadronFlavour;
        int H1_b2_hadronFlavour;
        int H2_b1_hadronFlavour;
        int H2_b2_hadronFlavour;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b1)
        float HH_b1_deepCSV;
        float HH_b1_qgl;
        float HH_b1_rawpt;
        int HH_b1_jetId;
        int HH_b1_puId;
        int HH_b1_quarkID;
        int HH_b1_genJetIdx;
        int HH_b1_partonFlavour;
        int HH_b1_hadronFlavour;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b2)
        float HH_b2_deepCSV;
        float HH_b2_qgl;        
        float HH_b2_rawpt;
        int HH_b2_jetId;
        int HH_b2_puId;
        int HH_b2_quarkID;      
        int HH_b2_genJetIdx;
        int HH_b2_partonFlavour;
        int HH_b2_hadronFlavour;        
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b3)
        float HH_b3_deepCSV;
        float HH_b3_qgl;
        float HH_b3_rawpt;
        int HH_b3_jetId;
        int HH_b3_puId;
        int HH_b3_quarkID;
        int HH_b3_genJetIdx;
        int HH_b3_partonFlavour; 
        int HH_b3_hadronFlavour; 
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_b4)
        float HH_b4_deepCSV;
        float HH_b4_qgl;
        float HH_b4_rawpt;
        int HH_b4_jetId;
        int HH_b4_puId;
        int HH_b4_quarkID;
        int HH_b4_genJetIdx;
        int HH_b4_partonFlavour;                
        int HH_b4_hadronFlavour; 
        DECLARE_m_pt_eta_phi_p4(JJ_j1)
        float JJ_j1_deepCSV;
        float JJ_j1_qgl;
        float JJ_j1_rawpt;
        int JJ_j1_jetId;
        int JJ_j1_puId;
        int JJ_j1_quarkID;
        int JJ_j1_genJetIdx;        
        int JJ_j1_partonFlavour;
        int JJ_j1_hadronFlavour;
        int JJ_j1_location;
        DECLARE_m_pt_eta_phi_p4(JJ_j2)
        float JJ_j2_deepCSV;
        float JJ_j2_qgl;
        float JJ_j2_rawpt;
        int JJ_j2_jetId;
        int JJ_j2_puId;
        int JJ_j2_quarkID;      
        int JJ_j2_genJetIdx;
        int JJ_j2_partonFlavour;
        int JJ_j2_hadronFlavour;        
        int JJ_j2_location;
        DECLARE_m_pt_eta_phi_p4(JJ)
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
        float HH_btag_b1_deepCSV;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
        float HH_btag_b2_deepCSV;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
        float HH_btag_b3_deepCSV;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
        float HH_btag_b4_deepCSV;

        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b1)
        float HH_btag_cmva_b1_cmva;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b2)
        float HH_btag_cmva_b2_cmva;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b3)
        float HH_btag_cmva_b3_cmva;
        DECLARE_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b4)
        float HH_btag_cmva_b4_cmva;

        int VBFEvent;
        int VBFEventLocation;
        int nPVgood;
        int nJet; 
        int nJetbarrel;
        int nJetendcap;
        int nJethf;
        int NVBFCandidates;
        float j1etaj2eta;
        float maxj1etaj2eta;
        float ptbalance; 
        float ptcentrality;
        float etacentrality;  
        int H1_quality;
        int H2_quality;
        int H1_wpflag;
        int H2_wpflag;
        int H1_ukflag;
        int H2_ukflag;
        int HH_quality;
        int JJ_quality;
        int HH_1wflag;
        int HH_2wflag;
        int HH_3wflag;
        int HH_4wflag; 
        int JJ_1wflag;
        int JJ_2wflag;               
        int HHJJ_quality;
        int NPartonsMatchedToJets;
        int NVBFCandidateswithVBFMatch;
        int VBFMatchedCandidatesEtaSign;
        float VBFMatchedCandidatesDeltaEta;
        float VBFSelectedDeltaEta;
        int VBFSelectedEtaSign;
        float VBFGeneratedDeltaEta;
        int VBFGeneratedEtaSign;
        float BDT1; 
        float BDT2;
        float BDT3;
        int ExtraVBFJetDecision;
        float ExtraVBFJetPt;
        float ExtraVBFJetEta;  
        DECLARE_m_pt_eta_phi_p4(H1rand);
        DECLARE_m_pt_eta_phi_p4(H2rand);
        DECLARE_m_pt_eta_phi_p4(X_b_1)
        DECLARE_m_pt_eta_phi_p4(X_b_2)
        DECLARE_m_pt_eta_phi_p4(H_b_1)
        DECLARE_m_pt_eta_phi_p4(H_b_2)
        DECLARE_m_pt_eta_phi_p4(RecoMatched_X_b_1)
        float RecoMatched_X_b_1_deepCSV;
        float RecoMatched_X_b_1_deltaR;
        DECLARE_m_pt_eta_phi_p4(RecoMatched_X_b_2)
        float RecoMatched_X_b_2_deepCSV;
        float RecoMatched_X_b_2_deltaR;
        DECLARE_m_pt_eta_phi_p4(RecoMatched_H_b_1)
        float RecoMatched_H_b_1_deepCSV;
        float RecoMatched_H_b_1_deltaR;
        DECLARE_m_pt_eta_phi_p4(RecoMatched_H_b_2)
        float RecoMatched_H_b_2_deepCSV;
        float RecoMatched_H_b_2_deltaR;
        float costh_H1_cm   ;
        float costh_H2_cm   ;
        float costh_HH_cm   ;
        float costh_JJ_cm   ;
        float costh_HH_b1_cm;
        float costh_HH_b2_cm;
        float costh_HH_b3_cm;
        float costh_HH_b4_cm;
        float costh_JJ_j1_cm;
        float costh_JJ_j2_cm;        
        float H1_bb_deltaR;
        float H2_bb_deltaR;
        float H1_bb_deltaPhi;
        float H2_bb_deltaPhi;
        float H1_bb_deltaEta;
        float H2_bb_deltaEta;
        int   NbJets;
        float TotalFourJetPt;
        float MinDeltaEtaJJ;
        float b1b2_deltaR; 
        float b1b3_deltaR; 
        float b1b4_deltaR; 
        float b1j1_deltaR; 
        float b1j2_deltaR; 
        float b2b3_deltaR; 
        float b2b4_deltaR; 
        float b2j1_deltaR; 
        float b2j2_deltaR; 
        float b3b4_deltaR; 
        float b3j1_deltaR; 
        float b3j2_deltaR; 
        float b4j1_deltaR; 
        float b4j2_deltaR; 
        float j1j2_deltaR; 
        float h1h2_deltaR; 
        float h1j1_deltaR; 
        float h1j2_deltaR; 
        float h2j1_deltaR; 
        float h2j2_deltaR; 
        float h1jj_deltaR; 
        float h2jj_deltaR; 
        float hhj1_deltaR; 
        float hhj2_deltaR; 
        float hhjj_deltaR;
        float b1b2_deltaPhi; 
        float b1b3_deltaPhi; 
        float b1b4_deltaPhi; 
        float b1j1_deltaPhi; 
        float b1j2_deltaPhi; 
        float b2b3_deltaPhi; 
        float b2b4_deltaPhi; 
        float b2j1_deltaPhi; 
        float b2j2_deltaPhi; 
        float b3b4_deltaPhi; 
        float b3j1_deltaPhi; 
        float b3j2_deltaPhi; 
        float b4j1_deltaPhi; 
        float b4j2_deltaPhi; 
        float j1j2_deltaPhi; 
        float b1b2_deltaEta; 
        float b1b3_deltaEta; 
        float b1b4_deltaEta; 
        float b1j1_deltaEta; 
        float b1j2_deltaEta; 
        float b2b3_deltaEta; 
        float b2b4_deltaEta; 
        float b2j1_deltaEta; 
        float b2j2_deltaEta; 
        float b3b4_deltaEta; 
        float b3j1_deltaEta; 
        float b3j2_deltaEta; 
        float b4j1_deltaEta; 
        float b4j2_deltaEta; 
        float j1j2_deltaEta; 
        float h1h2_deltaPhi; 
        float h1j1_deltaPhi; 
        float h1j2_deltaPhi; 
        float h2j1_deltaPhi; 
        float h2j2_deltaPhi; 
        float h1jj_deltaPhi; 
        float h2jj_deltaPhi; 
        float hhj1_deltaPhi; 
        float hhj2_deltaPhi; 
        float hhjj_deltaPhi; 
        float h1h2_deltaEta; 
        float h1j1_deltaEta; 
        float h1j2_deltaEta; 
        float h2j1_deltaEta; 
        float h2j2_deltaEta; 
        float h1jj_deltaEta; 
        float h2jj_deltaEta; 
        float hhj1_deltaEta; 
        float hhj2_deltaEta; 
        float hhjj_deltaEta;    
    private:
        void init_branches(bool initAllBranches);
        std::unique_ptr<TTree> tree_;
        const bool savetlv_;
        
        // for user declared branches
        UserValCollection<float> userFloats_;
        UserValCollection<int>   userInts_;
};

#endif