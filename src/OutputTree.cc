#include "OutputTree.h"

#include <iostream>

using namespace std;

// helper: creates the pt/eta/phi/p4 branches of a variable OBJ
#define BRANCH_m_pt_eta_phi_p4(OBJ) \
    tree_->Branch(#OBJ "_m",  &OBJ ## _m); \
    tree_->Branch(#OBJ "_pt",  &OBJ ## _pt); \
    tree_->Branch(#OBJ "_eta", &OBJ ## _eta); \
    tree_->Branch(#OBJ "_phi", &OBJ ## _phi); \
    if (savetlv_) tree_->Branch(#OBJ "_p4", &OBJ ## _p4);

#define CLEAR_m_pt_eta_phi_p4(OBJ) \
    OBJ ## _m    = -999.; \
    OBJ ## _pt   = -999.; \
    OBJ ## _eta  = -999.; \
    OBJ ## _phi  = -999.; \
    OBJ ## _p4 . SetPxPyPzE(0,0,0,0);

#define BRANCH_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    tree_->Branch(#OBJ "_m"          ,  &OBJ ## _m); \
    tree_->Branch(#OBJ "_pt"         ,  &OBJ ## _pt); \
    tree_->Branch(#OBJ "_ptRegressed",  &OBJ ## _ptRegressed); \
    tree_->Branch(#OBJ "_eta"        , &OBJ ## _eta); \
    tree_->Branch(#OBJ "_phi"        , &OBJ ## _phi); \
    if (savetlv_) tree_->Branch(#OBJ "_p4", &OBJ ## _p4);

#define CLEAR_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    OBJ ## _m             = -999.; \
    OBJ ## _pt            = -999.; \
    OBJ ## _ptRegressed   = -999.; \
    OBJ ## _eta           = -999.; \
    OBJ ## _phi           = -999.; \
    OBJ ## _p4            . SetPxPyPzE(0,0,0,0);

OutputTree::OutputTree (bool savetlv, string name, string title, bool initAllBranches) :
savetlv_ (savetlv)
{
    tree_ = std::unique_ptr<TTree> (new TTree(name.c_str(), title.c_str()));
    
    init_branches(initAllBranches);
    clear();
}

void OutputTree::init_branches(bool initAllBranches)
{

    //event information
    tree_->Branch("Run", &Run);
    tree_->Branch("LumiSec", &LumiSec);
    tree_->Branch("Event", &Event);
    if(!initAllBranches) return;
    // reco b jets
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    tree_->Branch("H1_b1_deepCSV", &H1_b1_deepCSV);
    tree_->Branch("H1_b1_puId", &H1_b1_puId);
    tree_->Branch("H1_b1_jetId", &H1_b1_jetId);
    tree_->Branch("H1_b1_rawpt", &H1_b1_rawpt);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    tree_->Branch("H1_b2_deepCSV", &H1_b2_deepCSV);
    tree_->Branch("H1_b2_puId", &H1_b2_puId);
    tree_->Branch("H1_b2_jetId", &H1_b2_jetId);
    tree_->Branch("H1_b2_rawpt", &H1_b2_rawpt);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    tree_->Branch("H2_b1_deepCSV", &H2_b1_deepCSV);
    tree_->Branch("H2_b1_puId", &H2_b1_puId);
    tree_->Branch("H2_b1_jetId", &H2_b1_jetId);
    tree_->Branch("H2_b1_rawpt", &H2_b1_rawpt);
    BRANCH_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    tree_->Branch("H2_b2_deepCSV", &H2_b2_deepCSV);
    tree_->Branch("H2_b2_puId", &H2_b2_puId);
    tree_->Branch("H2_b2_jetId", &H2_b2_jetId);
    tree_->Branch("H2_b2_rawpt", &H2_b2_rawpt);
    // reco H1 (H1_b1 + H1_b2)
    // reco H2 (H2_b1 + H2_b2)
    BRANCH_m_pt_eta_phi_p4(H1)
    tree_->Branch("H1_bb_DeltaR", &H1_bb_DeltaR);
    BRANCH_m_pt_eta_phi_p4(H2)
    tree_->Branch("H2_bb_DeltaR", &H2_bb_DeltaR);
    BRANCH_m_pt_eta_phi_p4(HH)
    tree_->Branch("HH_2DdeltaM", &HH_2DdeltaM);
    tree_->Branch("HH_m_kinFit", &HH_m_kinFit);

    tree_->Branch("triggerScaleFactor"       , &triggerScaleFactor       );
    tree_->Branch("triggerDataEfficiency"    , &triggerDataEfficiency    );
    tree_->Branch("triggerMcEfficiency"      , &triggerMcEfficiency      );
    tree_->Branch("triggerScaleFactorUp"     , &triggerScaleFactorUp     );
    tree_->Branch("triggerDataEfficiencyUp"  , &triggerDataEfficiencyUp  );
    tree_->Branch("triggerMcEfficiencyUp"    , &triggerMcEfficiencyUp    );
    tree_->Branch("triggerScaleFactorDown"   , &triggerScaleFactorDown   );
    tree_->Branch("triggerDataEfficiencyDown", &triggerDataEfficiencyDown);
    tree_->Branch("triggerMcEfficiencyDown"  , &triggerMcEfficiencyDown  );

    // // Non-resonant analysis and studies
    // tree_->Branch("H1_b1_quarkID", &H1_b1_quarkID);
    // tree_->Branch("H1_b1_partonFlavour", &H1_b1_partonFlavour);    
    // tree_->Branch("H1_b1_hadronFlavour", &H1_b1_hadronFlavour);
    // tree_->Branch("H1_b2_quarkID", &H1_b2_quarkID);    
    // tree_->Branch("H1_b2_partonFlavour", &H1_b2_partonFlavour);
    // tree_->Branch("H1_b2_hadronFlavour", &H1_b2_hadronFlavour);         
    // tree_->Branch("H2_b1_quarkID", &H2_b1_quarkID);
    // tree_->Branch("H2_b1_partonFlavour", &H2_b1_partonFlavour);
    // tree_->Branch("H2_b1_hadronFlavour", &H2_b1_hadronFlavour); 
    // tree_->Branch("H2_b2_quarkID", &H2_b2_quarkID);
    // tree_->Branch("H2_b2_partonFlavour", &H2_b2_partonFlavour);
    // tree_->Branch("H2_b2_hadronFlavour", &H2_b2_hadronFlavour);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    // tree_->Branch("HH_b1_deepCSV", &HH_b1_deepCSV);
    // tree_->Branch("HH_b1_puId", &HH_b1_puId);
    // tree_->Branch("HH_b1_jetId", &HH_b1_jetId);
    // tree_->Branch("HH_b1_rawpt", &HH_b1_rawpt);
    // tree_->Branch("HH_b1_qgl", &HH_b1_qgl);
    // tree_->Branch("HH_b1_quarkID", &HH_b1_quarkID);    
    // tree_->Branch("HH_b1_genJetIdx", &HH_b1_genJetIdx);
    // tree_->Branch("HH_b1_partonFlavour", &HH_b1_partonFlavour);
    // tree_->Branch("HH_b1_hadronFlavour", &HH_b1_hadronFlavour);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    // tree_->Branch("HH_b2_deepCSV", &HH_b2_deepCSV);
    // tree_->Branch("HH_b2_puId", &HH_b2_puId);
    // tree_->Branch("HH_b2_jetId", &HH_b2_jetId);
    // tree_->Branch("HH_b2_rawpt", &HH_b2_rawpt);
    // tree_->Branch("HH_b2_qgl", &HH_b2_qgl);    
    // tree_->Branch("HH_b2_quarkID", &HH_b2_quarkID);  
    // tree_->Branch("HH_b2_genJetIdx", &HH_b2_genJetIdx);
    // tree_->Branch("HH_b2_partonFlavour", &HH_b2_partonFlavour); 
    // tree_->Branch("HH_b2_hadronFlavour", &HH_b2_hadronFlavour);       
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    // tree_->Branch("HH_b3_deepCSV", &HH_b3_deepCSV);
    // tree_->Branch("HH_b3_puId", &HH_b3_puId);
    // tree_->Branch("HH_b3_jetId", &HH_b3_jetId);
    // tree_->Branch("HH_b3_rawpt", &HH_b3_rawpt);
    // tree_->Branch("HH_b3_qgl", &HH_b3_qgl);
    // tree_->Branch("HH_b3_quarkID", &HH_b3_quarkID);
    // tree_->Branch("HH_b3_genJetIdx", &HH_b3_genJetIdx);
    // tree_->Branch("HH_b3_partonFlavour", &HH_b3_partonFlavour);
    // tree_->Branch("HH_b3_hadronFlavour", &HH_b3_hadronFlavour);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    // tree_->Branch("HH_b4_deepCSV", &HH_b4_deepCSV);
    // tree_->Branch("HH_b4_puId", &HH_b4_puId);
    // tree_->Branch("HH_b4_jetId", &HH_b4_jetId);
    // tree_->Branch("HH_b4_rawpt", &HH_b4_rawpt);
    // tree_->Branch("HH_b4_qgl", &HH_b4_qgl);
    // tree_->Branch("HH_b4_quarkID", &HH_b4_quarkID);
    // tree_->Branch("HH_b4_genJetIdx", &HH_b4_genJetIdx);        
    // tree_->Branch("HH_b4_partonFlavour", &HH_b4_partonFlavour);
    // tree_->Branch("HH_b4_hadronFlavour", &HH_b4_hadronFlavour);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
    // tree_->Branch("HH_btag_b1_deepCSV", &HH_btag_b1_deepCSV);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
    // tree_->Branch("HH_btag_b2_deepCSV", &HH_btag_b2_deepCSV);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
    // tree_->Branch("HH_btag_b3_deepCSV", &HH_btag_b3_deepCSV);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
    // tree_->Branch("HH_btag_b4_deepCSV", &HH_btag_b4_deepCSV);       

    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b1)
    // tree_->Branch("HH_btag_cmva_b1_cmva", &HH_btag_cmva_b1_cmva);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b2)
    // tree_->Branch("HH_btag_cmva_b2_cmva", &HH_btag_cmva_b2_cmva);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b3)
    // tree_->Branch("HH_btag_cmva_b3_cmva", &HH_btag_cmva_b3_cmva);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b4)
    // tree_->Branch("HH_btag_cmva_b4_cmva", &HH_btag_cmva_b4_cmva);

    // BRANCH_m_pt_eta_phi_p4(JJ_j1)
    // tree_->Branch("JJ_j1_deepCSV", &JJ_j1_deepCSV);
    // tree_->Branch("JJ_j1_puId", &JJ_j1_puId);
    // tree_->Branch("JJ_j1_jetId", &JJ_j1_jetId);
    // tree_->Branch("JJ_j1_rawpt", &JJ_j1_rawpt);
    // tree_->Branch("JJ_j1_qgl", &JJ_j1_qgl);    
    // tree_->Branch("JJ_j1_quarkID", &JJ_j1_quarkID);    
    // tree_->Branch("JJ_j1_genJetIdx", &JJ_j1_genJetIdx);
    // tree_->Branch("JJ_j1_partonFlavour", &JJ_j1_partonFlavour);
    // tree_->Branch("JJ_j1_hadronFlavour", &JJ_j1_hadronFlavour);    
    // tree_->Branch("JJ_j1_location", &JJ_j1_location);
    // BRANCH_m_pt_eta_phi_p4(JJ_j2)
    // tree_->Branch("JJ_j2_deepCSV", &JJ_j2_deepCSV);
    // tree_->Branch("JJ_j2_puId", &JJ_j2_puId);
    // tree_->Branch("JJ_j2_jetId", &JJ_j2_jetId);
    // tree_->Branch("JJ_j2_rawpt", &JJ_j2_rawpt);
    // tree_->Branch("JJ_j2_qgl", &JJ_j2_qgl);     
    // tree_->Branch("JJ_j2_quarkID", &JJ_j2_quarkID);  
    // tree_->Branch("JJ_j2_genJetIdx", &JJ_j2_genJetIdx); 
    // tree_->Branch("JJ_j2_partonFlavour", &JJ_j2_partonFlavour);
    // tree_->Branch("JJ_j2_hadronFlavour", &JJ_j2_hadronFlavour);
    // tree_->Branch("JJ_j2_location", &JJ_j2_location);
    // BRANCH_m_pt_eta_phi_p4(JJ)
    // tree_->Branch("b1b2_deltaR",&b1b2_deltaR); 
    // tree_->Branch("b1b3_deltaR",&b1b3_deltaR); 
    // tree_->Branch("b1b4_deltaR",&b1b4_deltaR); 
    // tree_->Branch("b1j1_deltaR",&b1j1_deltaR); 
    // tree_->Branch("b1j2_deltaR",&b1j2_deltaR); 
    // tree_->Branch("b2b3_deltaR",&b2b3_deltaR); 
    // tree_->Branch("b2b4_deltaR",&b2b4_deltaR); 
    // tree_->Branch("b2j1_deltaR",&b2j1_deltaR); 
    // tree_->Branch("b2j2_deltaR",&b2j2_deltaR); 
    // tree_->Branch("b3b4_deltaR",&b3b4_deltaR); 
    // tree_->Branch("b3j1_deltaR",&b3j1_deltaR); 
    // tree_->Branch("b3j2_deltaR",&b3j2_deltaR); 
    // tree_->Branch("b4j1_deltaR",&b4j1_deltaR); 
    // tree_->Branch("b4j2_deltaR",&b4j2_deltaR); 
    // tree_->Branch("j1j2_deltaR",&j1j2_deltaR); 
    // tree_->Branch("h1h2_deltaR",&h1h2_deltaR); 
    // tree_->Branch("h1j1_deltaR",&h1j1_deltaR); 
    // tree_->Branch("h1j2_deltaR",&h1j2_deltaR); 
    // tree_->Branch("h2j1_deltaR",&h2j1_deltaR); 
    // tree_->Branch("h2j2_deltaR",&h2j2_deltaR); 
    // tree_->Branch("h1jj_deltaR",&h1jj_deltaR); 
    // tree_->Branch("h2jj_deltaR",&h2jj_deltaR); 
    // tree_->Branch("hhj1_deltaR",&hhj1_deltaR); 
    // tree_->Branch("hhj2_deltaR",&hhj2_deltaR); 
    // tree_->Branch("hhjj_deltaR",&hhjj_deltaR); 
    // tree_->Branch("b1b2_deltaPhi",&b1b2_deltaPhi); 
    // tree_->Branch("b1b3_deltaPhi",&b1b3_deltaPhi); 
    // tree_->Branch("b1b4_deltaPhi",&b1b4_deltaPhi); 
    // tree_->Branch("b1j1_deltaPhi",&b1j1_deltaPhi); 
    // tree_->Branch("b1j2_deltaPhi",&b1j2_deltaPhi); 
    // tree_->Branch("b2b3_deltaPhi",&b2b3_deltaPhi); 
    // tree_->Branch("b2b4_deltaPhi",&b2b4_deltaPhi); 
    // tree_->Branch("b2j1_deltaPhi",&b2j1_deltaPhi); 
    // tree_->Branch("b2j2_deltaPhi",&b2j2_deltaPhi); 
    // tree_->Branch("b3b4_deltaPhi",&b3b4_deltaPhi); 
    // tree_->Branch("b3j1_deltaPhi",&b3j1_deltaPhi); 
    // tree_->Branch("b3j2_deltaPhi",&b3j2_deltaPhi); 
    // tree_->Branch("b4j1_deltaPhi",&b4j1_deltaPhi); 
    // tree_->Branch("b4j2_deltaPhi",&b4j2_deltaPhi); 
    // tree_->Branch("j1j2_deltaPhi",&j1j2_deltaPhi); 
    // tree_->Branch("b1b2_deltaEta",&b1b2_deltaEta); 
    // tree_->Branch("b1b3_deltaEta",&b1b3_deltaEta); 
    // tree_->Branch("b1b4_deltaEta",&b1b4_deltaEta); 
    // tree_->Branch("b1j1_deltaEta",&b1j1_deltaEta); 
    // tree_->Branch("b1j2_deltaEta",&b1j2_deltaEta); 
    // tree_->Branch("b2b3_deltaEta",&b2b3_deltaEta); 
    // tree_->Branch("b2b4_deltaEta",&b2b4_deltaEta); 
    // tree_->Branch("b2j1_deltaEta",&b2j1_deltaEta); 
    // tree_->Branch("b2j2_deltaEta",&b2j2_deltaEta); 
    // tree_->Branch("b3b4_deltaEta",&b3b4_deltaEta); 
    // tree_->Branch("b3j1_deltaEta",&b3j1_deltaEta); 
    // tree_->Branch("b3j2_deltaEta",&b3j2_deltaEta); 
    // tree_->Branch("b4j1_deltaEta",&b4j1_deltaEta); 
    // tree_->Branch("b4j2_deltaEta",&b4j2_deltaEta); 
    // tree_->Branch("j1j2_deltaEta",&j1j2_deltaEta); 
    // tree_->Branch("h1h2_deltaPhi",&h1h2_deltaPhi); 
    // tree_->Branch("h1j1_deltaPhi",&h1j1_deltaPhi); 
    // tree_->Branch("h1j2_deltaPhi",&h1j2_deltaPhi); 
    // tree_->Branch("h2j1_deltaPhi",&h2j1_deltaPhi); 
    // tree_->Branch("h2j2_deltaPhi",&h2j2_deltaPhi); 
    // tree_->Branch("h1jj_deltaPhi",&h1jj_deltaPhi); 
    // tree_->Branch("h2jj_deltaPhi",&h2jj_deltaPhi); 
    // tree_->Branch("hhj1_deltaPhi",&hhj1_deltaPhi); 
    // tree_->Branch("hhj2_deltaPhi",&hhj2_deltaPhi); 
    // tree_->Branch("hhjj_deltaPhi",&hhjj_deltaPhi); 
    // tree_->Branch("h1h2_deltaEta",&h1h2_deltaEta); 
    // tree_->Branch("h1j1_deltaEta",&h1j1_deltaEta); 
    // tree_->Branch("h1j2_deltaEta",&h1j2_deltaEta); 
    // tree_->Branch("h2j1_deltaEta",&h2j1_deltaEta); 
    // tree_->Branch("h2j2_deltaEta",&h2j2_deltaEta); 
    // tree_->Branch("h1jj_deltaEta",&h1jj_deltaEta); 
    // tree_->Branch("h2jj_deltaEta",&h2jj_deltaEta); 
    // tree_->Branch("hhj1_deltaEta",&hhj1_deltaEta); 
    // tree_->Branch("hhj2_deltaEta",&hhj2_deltaEta); 
    // tree_->Branch("hhjj_deltaEta",&hhjj_deltaEta);
    // tree_->Branch("VBFEvent", &VBFEvent);
    // tree_->Branch("VBFEventLocation", &VBFEventLocation);
    // tree_->Branch("NVBFCandidates", &NVBFCandidates);
    // tree_->Branch("ExtraVBFJetDecision", &ExtraVBFJetDecision);
    // tree_->Branch("ExtraVBFJetPt", &ExtraVBFJetPt);
    // tree_->Branch("ExtraVBFJetEta", &ExtraVBFJetEta);    
    // tree_->Branch("nPVgood", &nPVgood);
    tree_->Branch("nJet", &nJet);
    // tree_->Branch("nJetbarrel", &nJetbarrel);
    // tree_->Branch("nJetendcap", &nJetendcap);
    // tree_->Branch("nJethf", &nJethf);
    // tree_->Branch("j1etaj2eta", &j1etaj2eta); 
    // tree_->Branch("maxj1etaj2eta", &maxj1etaj2eta);    
    // tree_->Branch("BDT1", &BDT1);
    // tree_->Branch("BDT2", &BDT2);
    // tree_->Branch("BDT3", &BDT3);
    // tree_->Branch("NPartonsMatchedToJets", &NPartonsMatchedToJets); 
    // tree_->Branch("NVBFCandidateswithVBFMatch", &NVBFCandidateswithVBFMatch); 
    // tree_->Branch("VBFMatchedCandidatesEtaSign", &VBFMatchedCandidatesEtaSign); 
    // tree_->Branch("VBFMatchedCandidatesDeltaEta", &VBFMatchedCandidatesDeltaEta);
    // tree_->Branch("VBFSelectedDeltaEta", &VBFSelectedDeltaEta);
    // tree_->Branch("VBFSelectedEtaSign", &VBFSelectedEtaSign);
    // tree_->Branch("VBFGeneratedDeltaEta", &VBFGeneratedDeltaEta);
    // tree_->Branch("VBFGeneratedEtaSign", &VBFGeneratedEtaSign);
    // tree_->Branch("H1_quality", &H1_quality);
    // tree_->Branch("H2_quality", &H2_quality);
    // tree_->Branch("H1_wpflag", &H1_wpflag);
    // tree_->Branch("H2_wpflag", &H2_wpflag); 
    // tree_->Branch("H1_ukflag", &H1_ukflag);
    // tree_->Branch("H2_ukflag", &H2_ukflag);
    // tree_->Branch("HH_quality", &HH_quality);        
    // tree_->Branch("HH_1wflag", &HH_1wflag);
    // tree_->Branch("HH_2wflag", &HH_2wflag);
    // tree_->Branch("HH_3wflag", &HH_3wflag);
    // tree_->Branch("HH_4wflag", &HH_4wflag);
    // tree_->Branch("JJ_quality", &JJ_quality); 
    // tree_->Branch("JJ_1wflag", &JJ_1wflag);
    // tree_->Branch("JJ_2wflag", &JJ_2wflag);
    // tree_->Branch("HHJJ_quality", &HHJJ_quality);
    // BRANCH_m_pt_eta_phi_p4(H1rand)
    // BRANCH_m_pt_eta_phi_p4(H2rand)
    BRANCH_m_pt_eta_phi_p4(X_b_1)
    BRANCH_m_pt_eta_phi_p4(X_b_2)
    BRANCH_m_pt_eta_phi_p4(H_b_1)
    BRANCH_m_pt_eta_phi_p4(H_b_2)
    BRANCH_m_pt_eta_phi_p4(RecoMatched_X_b_1)
    tree_->Branch("RecoMatched_X_b_1_deepCSV", &RecoMatched_X_b_1_deepCSV);
    tree_->Branch("RecoMatched_X_b_1_deltaR", &RecoMatched_X_b_1_deltaR);
    BRANCH_m_pt_eta_phi_p4(RecoMatched_X_b_2)
    tree_->Branch("RecoMatched_X_b_2_deepCSV", &RecoMatched_X_b_2_deepCSV);
    tree_->Branch("RecoMatched_X_b_2_deltaR", &RecoMatched_X_b_2_deltaR);
    BRANCH_m_pt_eta_phi_p4(RecoMatched_H_b_1)
    tree_->Branch("RecoMatched_H_b_1_deepCSV", &RecoMatched_H_b_1_deepCSV);
    tree_->Branch("RecoMatched_H_b_1_deltaR", &RecoMatched_H_b_1_deltaR);
    BRANCH_m_pt_eta_phi_p4(RecoMatched_H_b_2)
    tree_->Branch("RecoMatched_H_b_2_deepCSV", &RecoMatched_H_b_2_deepCSV);
    tree_->Branch("RecoMatched_H_b_2_deltaR", &RecoMatched_H_b_2_deltaR);
    tree_->Branch("H1_bb_deltaR", &H1_bb_deltaR);
    tree_->Branch("H2_bb_deltaR", &H2_bb_deltaR);    
    tree_->Branch("H1_bb_deltaPhi", &H1_bb_deltaPhi);
    tree_->Branch("H2_bb_deltaPhi", &H2_bb_deltaPhi);  
    tree_->Branch("H1_bb_deltaEta", &H1_bb_deltaEta);
    tree_->Branch("H2_bb_deltaEta", &H2_bb_deltaEta);  
    tree_->Branch("NbJets", &NbJets);  
    tree_->Branch("TotalFourJetPt", &TotalFourJetPt);  
    tree_->Branch("MinDeltaEtaJJ", &MinDeltaEtaJJ);  
    // //generator level information
    // tree_->Branch("gen_mJJ", &gen_mJJ);
    // tree_->Branch("gen_deltaEtaJJ", &gen_deltaEtaJJ);
    // tree_->Branch("gen_mHH", &gen_mHH);
    BRANCH_m_pt_eta_phi_p4(gen_H1)
    BRANCH_m_pt_eta_phi_p4(gen_H2)
    BRANCH_m_pt_eta_phi_p4(gen_H1_last)
    BRANCH_m_pt_eta_phi_p4(gen_H2_last)
    BRANCH_m_pt_eta_phi_p4(gen_H1_b1)
    tree_->Branch("gen_H1_b1_matchedflag", &gen_H1_b1_matchedflag);
    BRANCH_m_pt_eta_phi_p4(gen_H1_b2)
    tree_->Branch("gen_H1_b2_matchedflag", &gen_H1_b2_matchedflag);
    BRANCH_m_pt_eta_phi_p4(gen_H2_b1)
    tree_->Branch("gen_H2_b1_matchedflag", &gen_H2_b1_matchedflag);
    BRANCH_m_pt_eta_phi_p4(gen_H2_b2)
    tree_->Branch("gen_H2_b2_matchedflag", &gen_H2_b2_matchedflag);

    tree_->Branch("recoJetMatchedToGenJet1", &recoJetMatchedToGenJet1);
    tree_->Branch("recoJetMatchedToGenJet2", &recoJetMatchedToGenJet2);
    tree_->Branch("recoJetMatchedToGenJet3", &recoJetMatchedToGenJet3);
    tree_->Branch("recoJetMatchedToGenJet4", &recoJetMatchedToGenJet4);

    tree_->Branch("gen_jet_pt", &gen_jet_pt);
    tree_->Branch("jet_pt", &jet_pt);
    
    // BRANCH_m_pt_eta_phi_p4(gen_q1_in)
    // BRANCH_m_pt_eta_phi_p4(gen_q2_in)
    // BRANCH_m_pt_eta_phi_p4(gen_q1_out)
    // BRANCH_m_pt_eta_phi_p4(gen_q2_out)
    // tree_->Branch("gen_H1_b1_mindRjet", &gen_H1_b1_mindRjet);
    // tree_->Branch("gen_H1_b2_mindRjet", &gen_H1_b2_mindRjet);
    // tree_->Branch("gen_H2_b1_mindRjet", &gen_H2_b1_mindRjet);
    // tree_->Branch("gen_H2_b2_mindRjet", &gen_H2_b2_mindRjet);
    // tree_->Branch("gen_q1_out_mindRjet", &gen_q1_out_mindRjet);
    // tree_->Branch("gen_q2_out_mindRjet", &gen_q2_out_mindRjet);
    // tree_->Branch("gen_H1_b1_mindRgenjet", &gen_H1_b1_mindRgenjet);
    // tree_->Branch("gen_H1_b2_mindRgenjet", &gen_H1_b2_mindRgenjet);
    // tree_->Branch("gen_H2_b1_mindRgenjet", &gen_H2_b1_mindRgenjet);
    // tree_->Branch("gen_H2_b2_mindRgenjet", &gen_H2_b2_mindRgenjet);
    // tree_->Branch("gen_q1_out_mindRgenjet", &gen_q1_out_mindRgenjet);
    // tree_->Branch("gen_q2_out_mindRgenjet", &gen_q2_out_mindRgenjet);
    // tree_->Branch("gen_H1_b1_matched", &gen_H1_b1_matched);
    // tree_->Branch("gen_H1_b2_matched", &gen_H1_b2_matched);
    // tree_->Branch("gen_H2_b1_matched", &gen_H2_b1_matched);
    // tree_->Branch("gen_H2_b2_matched", &gen_H2_b2_matched);
    // tree_->Branch("gen_q1_out_matched", &gen_q1_out_matched);
    // tree_->Branch("gen_q2_out_matched", &gen_q2_out_matched);
    // tree_->Branch("gen_H1_b1_pdgId", &gen_H1_b1_pdgId);
    // tree_->Branch("gen_H1_b2_pdgId", &gen_H1_b2_pdgId);
    // tree_->Branch("gen_H2_b1_pdgId", &gen_H2_b1_pdgId);
    // tree_->Branch("gen_H2_b2_pdgId", &gen_H2_b2_pdgId);
    // tree_->Branch("gen_q1_out_pdgId", &gen_q1_out_pdgId);
    // tree_->Branch("gen_q2_out_pdgId", &gen_q2_out_pdgId);
    // tree_->Branch("gen_H1_b1_ptratio", &gen_H1_b1_ptratio);
    // tree_->Branch("gen_H1_b2_ptratio", &gen_H1_b2_ptratio);
    // tree_->Branch("gen_H2_b1_ptratio", &gen_H2_b1_ptratio);
    // tree_->Branch("gen_H2_b2_ptratio", &gen_H2_b2_ptratio);
    // tree_->Branch("gen_q1_out_ptratio", &gen_q1_out_ptratio);
    // tree_->Branch("gen_q2_out_ptratio", &gen_q2_out_ptratio);
    // tree_->Branch("gen_H1_b1_pz", &gen_H1_b1_pz);
    // tree_->Branch("gen_H1_b2_pz", &gen_H1_b2_pz);
    // tree_->Branch("gen_H2_b1_pz", &gen_H2_b1_pz);
    // tree_->Branch("gen_H2_b2_pz", &gen_H2_b2_pz);
    // tree_->Branch("gen_q1_out_pz", &gen_q1_out_pz);
    // tree_->Branch("gen_q2_out_pz", &gen_q2_out_pz);
    // //TTEMU EVents Studies
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(TT_b1)
    // tree_->Branch("TT_b1_deepCSV", &TT_b1_deepCSV);
    // BRANCH_m_pt_ptRegressed_eta_phi_p4(TT_b2)
    // tree_->Branch("TT_b2_deepCSV", &TT_b2_deepCSV);
    // BRANCH_m_pt_eta_phi_p4(TT_e)
    // BRANCH_m_pt_eta_phi_p4(TT_mu)
    // tree_->Branch("TT_m", &TT_m);         
    // tree_->Branch("TT_nJet", &TT_nJet); 
    // tree_->Branch("TT_nPV", &TT_nPV);
    // tree_->Branch("TT_nPVgood", &TT_nPVgood); 
    // //COSTHETA IN CM
    // tree_->Branch("costh_H1_cm", &costh_H1_cm);     
    // tree_->Branch("costh_H2_cm", &costh_H2_cm); 
    // tree_->Branch("costh_HH_cm", &costh_HH_cm); 
    // tree_->Branch("costh_JJ_cm", &costh_JJ_cm); 
    // tree_->Branch("costh_HH_b1_cm", &costh_HH_b1_cm);    
    // tree_->Branch("costh_HH_b2_cm", &costh_HH_b2_cm);
    // tree_->Branch("costh_HH_b3_cm", &costh_HH_b3_cm);    
    // tree_->Branch("costh_HH_b4_cm", &costh_HH_b4_cm);
    // tree_->Branch("costh_JJ_j1_cm", &costh_JJ_j1_cm);    
    // tree_->Branch("costh_JJ_j2_cm", &costh_JJ_j2_cm);  
    // note that the initialization of the user branches is made separately when calling declareUser*Branch
}

void OutputTree::clear()
{
    Run=0;
    LumiSec=0;
    Event=0;

    CLEAR_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    H1_b1_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    H1_b2_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    H2_b1_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    H2_b2_deepCSV = 0.;

    CLEAR_m_pt_eta_phi_p4(H1)
    H1_bb_DeltaR = 0.;
    CLEAR_m_pt_eta_phi_p4(H2)
    H2_bb_DeltaR = 0.;
    CLEAR_m_pt_eta_phi_p4(HH)
    HH_2DdeltaM = 0.;
    HH_m_kinFit = 0.;

    ///TTEMU events
    CLEAR_m_pt_ptRegressed_eta_phi_p4(TT_b1)
    TT_b1_deepCSV = 0.;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(TT_b2)
    TT_b2_deepCSV = 0.;    
    CLEAR_m_pt_eta_phi_p4(TT_e)
    CLEAR_m_pt_eta_phi_p4(TT_mu)
    TT_m=0; 
    TT_nPV=0;
    TT_nPVgood=0;
    TT_nJet=0;           
    //Non-resonant analysis and studies
    H1_b1_deepCSV = 0.;
    H1_b1_rawpt = 0.;
    H1_b1_quarkID = -1.;
    H1_b2_deepCSV = 0.;
    H1_b2_rawpt = 0.;
    H1_b2_quarkID = -1.;
    H2_b1_deepCSV = 0.;
    H2_b1_quarkID = -1.;
    H2_b1_rawpt = 0.;
    H2_b2_deepCSV = 0.;
    H2_b2_quarkID = -1.;
    H2_b2_rawpt = 0.;
    H1_b1_genJetIdx = -2;
    H1_b2_genJetIdx = -2;
    H2_b1_genJetIdx = -2;
    H2_b2_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    HH_b1_deepCSV = 0;
    HH_b1_qgl = -1;
    HH_b1_rawpt = 0;
    HH_b1_quarkID = -1;   
    HH_b1_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    HH_b2_deepCSV = 0;
    HH_b2_qgl = -1;   
    HH_b2_rawpt = 0;
    HH_b2_quarkID = -1; 
    HH_b3_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    HH_b3_deepCSV = 0;
    HH_b3_rawpt = 0;
    HH_b3_qgl = -1;
    HH_b3_quarkID = -1; 
    HH_b3_genJetIdx = -2;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    HH_b4_deepCSV = 0;
    HH_b4_rawpt = 0;
    HH_b4_qgl = -1;
    HH_b4_quarkID = -1; 
    HH_b4_genJetIdx = -2;  
    CLEAR_m_pt_eta_phi_p4(JJ_j1)
    JJ_j1_deepCSV = 0.;
    JJ_j1_qgl = -1;
    JJ_j1_rawpt = 0;
    JJ_j1_quarkID = -1;
    JJ_j1_genJetIdx = -2;
    JJ_j1_location = 0;
    CLEAR_m_pt_eta_phi_p4(JJ_j2)
    JJ_j2_deepCSV = 0.;    
    JJ_j2_qgl = -1;  
    JJ_j2_rawpt = 0;
    JJ_j2_quarkID = -1;
    JJ_j2_genJetIdx = -2; 
    JJ_j2_location = 0; 
    CLEAR_m_pt_eta_phi_p4(JJ)
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
    HH_btag_b1_deepCSV = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
    HH_btag_b2_deepCSV = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
    HH_btag_b3_deepCSV = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
    HH_btag_b4_deepCSV = 0;

    triggerScaleFactor        = 1.;
    triggerDataEfficiency     = 1.;
    triggerMcEfficiency       = 1.;
    triggerScaleFactorUp      = 1.;
    triggerDataEfficiencyUp   = 1.;
    triggerMcEfficiencyUp     = 1.;
    triggerScaleFactorDown    = 1.;
    triggerDataEfficiencyDown = 1.;
    triggerMcEfficiencyDown   = 1.;

    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b1)
    HH_btag_cmva_b1_cmva = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b2)
    HH_btag_cmva_b2_cmva = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b3)
    HH_btag_cmva_b3_cmva = 0;
    CLEAR_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b4)
    HH_btag_cmva_b4_cmva = 0;


    nJetbarrel = -1.;
    nJetendcap = -1.;
    nJethf = -1.;
    nJet   = -1.;
    VBFEvent =-1.;
    VBFEventLocation = 0;
    NVBFCandidates = -1.;
    ExtraVBFJetDecision=-1.;
    ExtraVBFJetPt=-11.;
    ExtraVBFJetEta=-11.;    
    nJet = -1;
    nPVgood=0;
    BDT1 =-1.2;
    BDT2 =-1.2;
    BDT3 =-1.2;
    NPartonsMatchedToJets=-1.;
    NVBFCandidateswithVBFMatch=-1;
    VBFMatchedCandidatesEtaSign=0;
    VBFMatchedCandidatesDeltaEta=-1;
    VBFSelectedDeltaEta=-1;
    VBFSelectedEtaSign=0;
    VBFGeneratedDeltaEta=-1;
    VBFGeneratedEtaSign=0;
    maxj1etaj2eta=-99;
    j1etaj2eta=-99;
    H1_quality  = -1;
    H2_quality  = -1;
    H1_ukflag  = -1;
    H2_ukflag  = -1;
    H1_wpflag  = -1;
    H2_wpflag  = -1;
    HH_quality  = -1;
    HH_1wflag  = -1;
    HH_2wflag  = -1;
    HH_3wflag  = -1;    
    HH_4wflag  = -1;
    JJ_quality  = -1;
    JJ_1wflag  = -1;
    JJ_2wflag  = -1;
    HHJJ_quality  = -1;
    CLEAR_m_pt_eta_phi_p4(H1rand)
    CLEAR_m_pt_eta_phi_p4(H2rand)
    CLEAR_m_pt_eta_phi_p4(X_b_1)
    CLEAR_m_pt_eta_phi_p4(X_b_2)
    CLEAR_m_pt_eta_phi_p4(H_b_1)
    CLEAR_m_pt_eta_phi_p4(H_b_2)
    CLEAR_m_pt_eta_phi_p4(RecoMatched_X_b_1)
    RecoMatched_X_b_1_deepCSV = -1.;
    RecoMatched_X_b_1_deltaR = -1.;
    CLEAR_m_pt_eta_phi_p4(RecoMatched_X_b_2)
    RecoMatched_X_b_2_deepCSV = -1.;
    RecoMatched_X_b_2_deltaR = -1.;
    CLEAR_m_pt_eta_phi_p4(RecoMatched_H_b_1)
    RecoMatched_H_b_1_deepCSV = -1.;
    RecoMatched_H_b_1_deltaR = -1.;
    CLEAR_m_pt_eta_phi_p4(RecoMatched_H_b_2)
    RecoMatched_H_b_2_deepCSV = -1.;
    RecoMatched_H_b_2_deltaR = -1.;
    H1_bb_deltaR = 0.;
    H2_bb_deltaR = 0.;
    H1_bb_deltaEta = 0.;
    H2_bb_deltaEta = 0.;
    NbJets = 0;
    TotalFourJetPt = -1.;
    MinDeltaEtaJJ = -1.;
    H1_bb_deltaPhi = 0.;
    H2_bb_deltaPhi = 0.;
    b1b2_deltaR = 0.; 
    b1b3_deltaR = 0.; 
    b1b4_deltaR = 0.; 
    b1j1_deltaR = 0.; 
    b1j2_deltaR = 0.; 
    b2b3_deltaR = 0.; 
    b2b4_deltaR = 0.; 
    b2j1_deltaR = 0.; 
    b2j2_deltaR = 0.; 
    b3b4_deltaR = 0.; 
    b3j1_deltaR = 0.; 
    b3j2_deltaR = 0.; 
    b4j1_deltaR = 0.; 
    b4j2_deltaR = 0.; 
    j1j2_deltaR = 0.; 
    h1h2_deltaR = 0.; 
    h1j1_deltaR = 0.; 
    h1j2_deltaR = 0.; 
    h2j1_deltaR = 0.; 
    h2j2_deltaR = 0.; 
    h1jj_deltaR = 0.; 
    h2jj_deltaR = 0.; 
    hhj1_deltaR = 0.; 
    hhj2_deltaR = 0.; 
    hhjj_deltaR = 0.;
    b1b2_deltaPhi = 0.; 
    b1b3_deltaPhi = 0.; 
    b1b4_deltaPhi = 0.; 
    b1j1_deltaPhi = 0.; 
    b1j2_deltaPhi = 0.; 
    b2b3_deltaPhi = 0.; 
    b2b4_deltaPhi = 0.; 
    b2j1_deltaPhi = 0.; 
    b2j2_deltaPhi = 0.; 
    b3b4_deltaPhi = 0.; 
    b3j1_deltaPhi = 0.; 
    b3j2_deltaPhi = 0.; 
    b4j1_deltaPhi = 0.; 
    b4j2_deltaPhi = 0.; 
    j1j2_deltaPhi = 0.; 
    b1b2_deltaEta = 0.; 
    b1b3_deltaEta = 0.; 
    b1b4_deltaEta = 0.; 
    b1j1_deltaEta = 0.; 
    b1j2_deltaEta = 0.; 
    b2b3_deltaEta = 0.; 
    b2b4_deltaEta = 0.; 
    b2j1_deltaEta = 0.; 
    b2j2_deltaEta = 0.; 
    b3b4_deltaEta = 0.; 
    b3j1_deltaEta = 0.; 
    b3j2_deltaEta = 0.; 
    b4j1_deltaEta = 0.; 
    b4j2_deltaEta = 0.; 
    j1j2_deltaEta = 0.;  
    h1h2_deltaPhi = 0.; 
    h1j1_deltaPhi = 0.; 
    h1j2_deltaPhi = 0.; 
    h2j1_deltaPhi = 0.; 
    h2j2_deltaPhi = 0.; 
    h1jj_deltaPhi = 0.; 
    h2jj_deltaPhi = 0.; 
    hhj1_deltaPhi = 0.; 
    hhj2_deltaPhi = 0.; 
    hhjj_deltaPhi = 0.; 
    h1h2_deltaEta = 0.; 
    h1j1_deltaEta = 0.; 
    h1j2_deltaEta = 0.; 
    h2j1_deltaEta = 0.; 
    h2j2_deltaEta = 0.; 
    h1jj_deltaEta = 0.; 
    h2jj_deltaEta = 0.; 
    hhj1_deltaEta = 0.; 
    hhj2_deltaEta = 0.; 
    hhjj_deltaEta = 0.;
    costh_H1_cm=0;       
    costh_H2_cm=0;   
    costh_HH_cm=0;   
    costh_JJ_cm=0;   
    costh_HH_b1_cm=0;    
    costh_HH_b2_cm=0;
    costh_HH_b3_cm=0;    
    costh_HH_b4_cm=0;
    costh_JJ_j1_cm=0;    
    costh_JJ_j2_cm=0;
    //generator level information
    gen_mHH = 0.;    
    gen_mJJ = 0.;    
    gen_deltaEtaJJ = 0.;
    CLEAR_m_pt_eta_phi_p4(gen_H1)
    CLEAR_m_pt_eta_phi_p4(gen_H2)
    CLEAR_m_pt_eta_phi_p4(gen_H1_last)
    CLEAR_m_pt_eta_phi_p4(gen_H2_last)
    CLEAR_m_pt_eta_phi_p4(gen_H1_b1)
    gen_H1_b1_matchedflag = -1;
    CLEAR_m_pt_eta_phi_p4(gen_H1_b2)
    gen_H1_b2_matchedflag = -1;
    CLEAR_m_pt_eta_phi_p4(gen_H2_b1)
    gen_H2_b1_matchedflag = -1;
    CLEAR_m_pt_eta_phi_p4(gen_H2_b2)
    gen_H2_b2_matchedflag = -1;

    recoJetMatchedToGenJet1 = -1;
    recoJetMatchedToGenJet2 = -1;
    recoJetMatchedToGenJet3 = -1;
    recoJetMatchedToGenJet4 = -1;

    // gen_jet_pt = 0;
    jet_pt = 0;

    CLEAR_m_pt_eta_phi_p4(gen_q1_in)
    CLEAR_m_pt_eta_phi_p4(gen_q2_in)
    CLEAR_m_pt_eta_phi_p4(gen_q1_out)
    CLEAR_m_pt_eta_phi_p4(gen_q2_out)
    gen_q1_out_mindRjet=-1;
    gen_q2_out_mindRjet=-1;
    gen_H1_b1_mindRjet=-1;
    gen_H1_b2_mindRjet=-1;
    gen_H2_b1_mindRjet=-1;
    gen_H2_b2_mindRjet=-1;
    gen_q1_out_matched=-1;
    gen_q2_out_matched=-1;
    gen_H1_b1_matched=-1;
    gen_H1_b2_matched=-1;
    gen_H2_b1_matched=-1;
    gen_H2_b2_matched=-1;
    gen_q1_out_ptratio=-1;
    gen_q2_out_ptratio=-1;
    gen_H1_b1_ptratio=-1;
    gen_H1_b2_ptratio=-1;
    gen_H2_b1_ptratio=-1;
    gen_H2_b2_ptratio=-1;
    gen_q1_out_pz=-1;
    gen_q2_out_pz=-1;
    gen_H1_b1_pz=-1;
    gen_H1_b2_pz=-1;
    gen_H2_b1_pz=-1;
    gen_H2_b2_pz=-1;

    userFloats_.resetAll();
    userInts_.resetAll();

}

bool OutputTree::declareUserIntBranch (std::string name, int defaultClearValue)
{
    // check if the branch exists -- the check in the same collection is done by UserVal internally, but I have to do the cross-checks
    if (userFloats_.hasVal(name)){
        cout << "[WARNING] OutputTree : declareUserIntBranch : branch " << name << " was already found as a userFloat, cannot create it" << endl;
        return false;
    }
    
    if (!userInts_.addVal(name, defaultClearValue)){
        cout << "[WARNING] OutputTree : declareUserIntBranch : branch " << name << " was already found as a userInt, cannot create it" << endl;
        return false;
    }

    cout << "[INFO] OutputTree : creating userIntBranch " << name << " (" << defaultClearValue << ")" << endl;

    // set the branch
    tree_->Branch(name.c_str(), userInts_.getValPtr(name));
    return true;
}

bool OutputTree::declareUserFloatBranch (std::string name, float defaultClearValue)
{
    // check if the branch exists -- the check in the same collection is done by UserVal internally, but I have to do the cross-checks
    if (userInts_.hasVal(name)){
        cout << "[WARNING] OutputTree : declareUserFloatBranch : branch " << name << " was already found as a userInt, cannot create it" << endl;
        return false;
    }
    
    if (!userFloats_.addVal(name, defaultClearValue)){
        cout << "[WARNING] OutputTree : declareUserFloatBranch : branch " << name << " was already found as a userFloat, cannot create it" << endl;
        return false;
    }

    cout << "[INFO] OutputTree : creating userFloatBranch " << name << " (" << defaultClearValue << ")" << endl;

    // set the branch
    tree_->Branch(name.c_str(), userFloats_.getValPtr(name));
    return true;
}

bool OutputTree::declareUserIntBranchList(std::vector<std::string> nameList, int defaultClearValue)
{
    for(const auto& name : nameList)
    {
        bool success = declareUserIntBranch(name, defaultClearValue);
        if(!success) return false;
    }
    return true;
}
