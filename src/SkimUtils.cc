#include "SkimUtils.h"

#include "TLorentzVector.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// helper: copies the pt/eta/phi/p4 branches from a candidate OBJ to the output tree
// NOTE: requires the matching of the names (and enforces it)
#define COPY_m_pt_eta_phi_p4(OBJ) \
    ot.OBJ ## _m   = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt  = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _eta = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4  = ei. OBJ -> P4();

#define COPY_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    ot.OBJ ## _m            = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt           = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _ptRegressed  = ei. OBJ -> P4Regressed().Pt(); \
    ot.OBJ ## _eta          = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi          = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4           = ei. OBJ -> P4();

//helperM same as above, but encloses the obj (a boost::optional is expected) in a if clause to check whether it is initialized
#define COPY_OPTIONAL_m_pt_eta_phi_p4(OBJ) \
    if (ei.OBJ) { \
        ot.OBJ ## _m   = ei. OBJ -> P4().M(); \
        ot.OBJ ## _pt  = ei. OBJ -> P4().Pt(); \
        ot.OBJ ## _eta = ei. OBJ -> P4().Eta(); \
        ot.OBJ ## _phi = ei. OBJ -> P4().Phi(); \
        ot.OBJ ## _p4  = ei. OBJ -> P4(); \
    }

#define COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(OBJ) \
    if (ei.OBJ) { \
    ot.OBJ ## _m            = ei. OBJ -> P4().M(); \
    ot.OBJ ## _pt           = ei. OBJ -> P4().Pt(); \
    ot.OBJ ## _ptRegressed  = ei. OBJ -> P4Regressed().Pt(); \
    ot.OBJ ## _eta          = ei. OBJ -> P4().Eta(); \
    ot.OBJ ## _phi          = ei. OBJ -> P4().Phi(); \
    ot.OBJ ## _p4           = ei. OBJ -> P4();\
    }
// --- - --- - --- - --- - --- - --- - --- - --- - --- - --- - --- - --- - 

int SkimUtils::appendFromFileList (TChain* chain, string filename)
{
    //cout << "=== inizio parser ===" << endl;
    std::ifstream infile(filename.c_str());
    std::string line;
    int nfiles = 0;
    while (std::getline(infile, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) // skip empty lines
        {
            chain->Add(line.c_str());
            ++nfiles;
        }
     }
    return nfiles;
}


void SkimUtils::fill_output_tree(OutputTree& ot, NanoAODTree& nat, EventInfo& ei)
{

    // set the variables for resonant analysis
    if(ei.Run) ot.Run  = *ei.Run;
    if(ei.LumiSec) ot.LumiSec  = *ei.LumiSec;
    if(ei.Event) ot.Event  = *ei.Event;
    if(ei.pileUp) ot.pileUp  = *ei.pileUp;

    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H1_b1)
    if (ei.H1_b1) ot.H1_b1_deepCSV = ei.H1_b1.get().bTagScore();   
    if (ei.H1_b1) ot.H1_b1_jetId   = get_property(ei.H1_b1.get(),Jet_jetId); 
    if (ei.H1_b1) ot.H1_b1_puId    = get_property(ei.H1_b1.get(),Jet_puId);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H1_b2)
    if (ei.H1_b2) ot.H1_b2_deepCSV = ei.H1_b2.get().bTagScore();
    if (ei.H1_b2) ot.H1_b2_jetId   = get_property(ei.H1_b2.get(),Jet_jetId); 
    if (ei.H1_b2) ot.H1_b2_puId    = get_property(ei.H1_b2.get(),Jet_puId);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H2_b1)
    if (ei.H2_b1) ot.H2_b1_deepCSV = ei.H2_b1.get().bTagScore();
    if (ei.H2_b1) ot.H2_b1_jetId   = get_property(ei.H2_b1.get(),Jet_jetId); 
    if (ei.H2_b1) ot.H2_b1_puId    = get_property(ei.H2_b1.get(),Jet_puId);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H2_b2)
    if (ei.H2_b2) ot.H2_b2_deepCSV = ei.H2_b2.get().bTagScore();
    if (ei.H2_b2) ot.H2_b2_jetId   = get_property(ei.H2_b2.get(),Jet_jetId); 
    if (ei.H2_b2) ot.H2_b2_puId    = get_property(ei.H2_b2.get(),Jet_puId);

    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H1_b1_kinFit)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H1_b2_kinFit)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H2_b1_kinFit)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(H2_b2_kinFit)

    COPY_OPTIONAL_m_pt_eta_phi_p4(H1)
    if(ei.H1_bb_DeltaR) ot.H1_bb_DeltaR  = *ei.H1_bb_DeltaR;
    COPY_OPTIONAL_m_pt_eta_phi_p4(H2)
    if(ei.H2_bb_DeltaR) ot.H2_bb_DeltaR  = *ei.H2_bb_DeltaR;
    COPY_OPTIONAL_m_pt_eta_phi_p4(HH)
    if(ei.HH_2DdeltaM) ot.HH_2DdeltaM  = *ei.HH_2DdeltaM;
    if(ei.H1_H2_sphericity) ot.H1_H2_sphericity  = *ei.H1_H2_sphericity;
    if(ei.FourBjet_sphericity) ot.FourBjet_sphericity  = *ei.FourBjet_sphericity;
 
     COPY_OPTIONAL_m_pt_eta_phi_p4(offShell_H1)
    if(ei.offShell_H1_bb_DeltaR) ot.offShell_H1_bb_DeltaR  = *ei.offShell_H1_bb_DeltaR;
    COPY_OPTIONAL_m_pt_eta_phi_p4(offShell_H2)
    if(ei.offShell_H2_bb_DeltaR) ot.offShell_H2_bb_DeltaR  = *ei.offShell_H2_bb_DeltaR;
    COPY_OPTIONAL_m_pt_eta_phi_p4(offShell_HH)

    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(offShell_H1_b1)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(offShell_H1_b2)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(offShell_H2_b1)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(offShell_H2_b2)

    COPY_OPTIONAL_m_pt_eta_phi_p4(H1_kinFit)
    COPY_OPTIONAL_m_pt_eta_phi_p4(H2_kinFit)
    COPY_OPTIONAL_m_pt_eta_phi_p4(HH_kinFit)
    if(ei.H1_kinFit_bb_DeltaR) ot.H1_kinFit_bb_DeltaR  = *ei.H1_kinFit_bb_DeltaR;
    if(ei.H1_kinFit_bb_DeltaR) ot.H1_kinFit_bb_DeltaR  = *ei.H1_kinFit_bb_DeltaR;



    //set the variables for TTEMU studies
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(TT_b1)
    if (ei.TT_b1) ot.TT_b1_deepCSV = ei.TT_b1.get().bTagScore();   
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(TT_b2)
    if (ei.TT_b2) ot.TT_b2_deepCSV = ei.TT_b2.get().bTagScore();
    COPY_OPTIONAL_m_pt_eta_phi_p4(TT_e)
    COPY_OPTIONAL_m_pt_eta_phi_p4(TT_mu)
    if(ei.TT_m) ot.TT_m  = *ei.TT_m;            
    if(ei.TT_nPV) ot.TT_nPV  = *ei.TT_nPV;
    if(ei.TT_nPVgood) ot.TT_nPVgood  = *ei.TT_nPVgood;
    if(ei.TT_nJet) ot.TT_nJet  = *ei.TT_nJet;    
    //set the variables for non-resonant analysis and studies
    if(ei.H1_b1_quarkID) ot.H1_b1_quarkID  = *ei.H1_b1_quarkID;
    if(ei.H1_b2_quarkID) ot.H1_b2_quarkID  = *ei.H1_b2_quarkID;
    if(ei.H2_b1_quarkID) ot.H2_b1_quarkID  = *ei.H2_b1_quarkID;
    if(ei.H2_b2_quarkID) ot.H2_b2_quarkID  = *ei.H2_b2_quarkID;
    if(ei.H1_b1_rawpt) ot.H1_b1_rawpt = *ei.H1_b1_rawpt;
    if(ei.H1_b2_rawpt) ot.H1_b2_rawpt = *ei.H1_b2_rawpt;
    if(ei.H2_b1_rawpt) ot.H2_b1_rawpt = *ei.H2_b1_rawpt;
    if(ei.H2_b2_rawpt) ot.H2_b2_rawpt = *ei.H2_b2_rawpt;
    if(ei.H1_b1_genJetIdx) ot.H1_b1_genJetIdx  = *ei.H1_b1_genJetIdx;
    if(ei.H1_b2_genJetIdx) ot.H1_b2_genJetIdx  = *ei.H1_b2_genJetIdx;
    if(ei.H2_b1_genJetIdx) ot.H2_b1_genJetIdx  = *ei.H2_b1_genJetIdx;
    if(ei.H2_b2_genJetIdx) ot.H2_b2_genJetIdx  = *ei.H2_b2_genJetIdx;
    if(ei.H1_b1_partonFlavour) ot.H1_b1_partonFlavour  = *ei.H1_b1_partonFlavour;
    if(ei.H1_b2_partonFlavour) ot.H1_b2_partonFlavour  = *ei.H1_b2_partonFlavour;
    if(ei.H2_b1_partonFlavour) ot.H2_b1_partonFlavour  = *ei.H2_b1_partonFlavour;
    if(ei.H2_b2_partonFlavour) ot.H2_b2_partonFlavour  = *ei.H2_b2_partonFlavour;    
    if(ei.H1_b1_hadronFlavour) ot.H1_b1_hadronFlavour  = *ei.H1_b1_hadronFlavour;
    if(ei.H1_b2_hadronFlavour) ot.H1_b2_hadronFlavour  = *ei.H1_b2_hadronFlavour;
    if(ei.H2_b1_hadronFlavour) ot.H2_b1_hadronFlavour  = *ei.H2_b1_hadronFlavour;
    if(ei.H2_b2_hadronFlavour) ot.H2_b2_hadronFlavour  = *ei.H2_b2_hadronFlavour;
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b1)
    if(ei.HH_b1)                ot.HH_b1_deepCSV = ei.HH_b1.get().bTagScore();
    if(ei.HH_b1)                ot.HH_b1_qgl = get_property(ei.HH_b1.get(),Jet_qgl);
    if(ei.HH_b1) ot.HH_b1_jetId   = get_property(ei.HH_b1.get(),Jet_jetId); 
    if(ei.HH_b1) ot.HH_b1_puId    = get_property(ei.HH_b1.get(),Jet_puId);
    if(ei.HH_b1_rawpt) ot.HH_b1_rawpt   = *ei.HH_b1_rawpt;
    if(ei.HH_b1_quarkID)        ot.HH_b1_quarkID = *ei.HH_b1_quarkID;
    if(ei.HH_b1_genJetIdx)        ot.HH_b1_genJetIdx = *ei.HH_b1_genJetIdx;
    if(ei.HH_b1_partonFlavour)        ot.HH_b1_partonFlavour = *ei.HH_b1_partonFlavour;   
    if(ei.HH_b1_hadronFlavour)        ot.HH_b1_hadronFlavour = *ei.HH_b1_hadronFlavour; 
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b2)
    if(ei.HH_b2)                ot.HH_b2_deepCSV = ei.HH_b2.get().bTagScore();
    if(ei.HH_b2)                ot.HH_b2_qgl = get_property(ei.HH_b2.get(),Jet_qgl);
    if(ei.HH_b2) ot.HH_b2_jetId   = get_property(ei.HH_b2.get(),Jet_jetId); 
    if(ei.HH_b2) ot.HH_b2_puId    = get_property(ei.HH_b2.get(),Jet_puId);
    if(ei.HH_b2_rawpt) ot.HH_b2_rawpt   = *ei.HH_b2_rawpt;
    if(ei.HH_b2_quarkID)        ot.HH_b2_quarkID = *ei.HH_b2_quarkID;
    if(ei.HH_b2_genJetIdx)        ot.HH_b2_genJetIdx = *ei.HH_b2_genJetIdx;
    if(ei.HH_b2_partonFlavour)        ot.HH_b2_partonFlavour = *ei.HH_b2_partonFlavour; 
    if(ei.HH_b2_hadronFlavour)        ot.HH_b2_hadronFlavour = *ei.HH_b2_hadronFlavour; 
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b3)
    if(ei.HH_b3)                ot.HH_b3_deepCSV = ei.HH_b3.get().bTagScore();
    if(ei.HH_b3)                ot.HH_b3_qgl = get_property(ei.HH_b3.get(),Jet_qgl);
    if(ei.HH_b3) ot.HH_b3_jetId   = get_property(ei.HH_b3.get(),Jet_jetId); 
    if(ei.HH_b3) ot.HH_b3_puId    = get_property(ei.HH_b3.get(),Jet_puId);
    if(ei.HH_b3_rawpt) ot.HH_b3_rawpt   = *ei.HH_b3_rawpt;
    if(ei.HH_b3_quarkID)        ot.HH_b3_quarkID = *ei.HH_b3_quarkID;
    if(ei.HH_b3_genJetIdx)                ot.HH_b3_genJetIdx = *ei.HH_b3_genJetIdx;
    if(ei.HH_b3_partonFlavour)        ot.HH_b3_partonFlavour = *ei.HH_b3_partonFlavour;
    if(ei.HH_b3_hadronFlavour)        ot.HH_b3_hadronFlavour = *ei.HH_b3_hadronFlavour;
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_b4)
    if(ei.HH_b4)                ot.HH_b4_deepCSV = ei.HH_b4.get().bTagScore();
    if(ei.HH_b4)                ot.HH_b4_qgl = get_property(ei.HH_b4.get(),Jet_qgl);
    if(ei.HH_b4) ot.HH_b4_jetId   = get_property(ei.HH_b4.get(),Jet_jetId); 
    if(ei.HH_b4) ot.HH_b4_puId    = get_property(ei.HH_b4.get(),Jet_puId);
    if(ei.HH_b4_rawpt) ot.HH_b4_rawpt   = *ei.HH_b4_rawpt;
    if(ei.HH_b4_quarkID)        ot.HH_b4_quarkID = *ei.HH_b4_quarkID;
    if(ei.HH_b4_genJetIdx)                ot.HH_b4_genJetIdx = *ei.HH_b4_genJetIdx;    
    if(ei.HH_b4_partonFlavour)        ot.HH_b4_partonFlavour = *ei.HH_b4_partonFlavour;
    if(ei.HH_b4_hadronFlavour)        ot.HH_b4_hadronFlavour = *ei.HH_b4_hadronFlavour;
    COPY_OPTIONAL_m_pt_eta_phi_p4(JJ_j1)
    if(ei.JJ_j1)                ot.JJ_j1_deepCSV = ei.JJ_j1.get().bTagScore();
    if(ei.JJ_j1)                ot.JJ_j1_qgl = get_property(ei.JJ_j1.get(),Jet_qgl);
    if(ei.JJ_j1) ot.JJ_j1_jetId   = get_property(ei.JJ_j1.get(),Jet_jetId); 
    if(ei.JJ_j1) ot.JJ_j1_puId    = get_property(ei.JJ_j1.get(),Jet_puId);
    if(ei.JJ_j1_rawpt) ot.JJ_j1_rawpt   = *ei.JJ_j1_rawpt;
    if(ei.JJ_j1_quarkID)        ot.JJ_j1_quarkID  = *ei.JJ_j1_quarkID;
    if(ei.JJ_j1_genJetIdx)                ot.JJ_j1_genJetIdx = *ei.JJ_j1_genJetIdx;
    if(ei.JJ_j1_partonFlavour)        ot.JJ_j1_partonFlavour = *ei.JJ_j1_partonFlavour;
    if(ei.JJ_j1_hadronFlavour)        ot.JJ_j1_hadronFlavour = *ei.JJ_j1_hadronFlavour;
    if(ei.JJ_j1_location)        ot.JJ_j1_location = *ei.JJ_j1_location;
    COPY_OPTIONAL_m_pt_eta_phi_p4(JJ_j2)
    if(ei.JJ_j2)                ot.JJ_j2_deepCSV = ei.JJ_j2.get().bTagScore();
    if(ei.JJ_j2)                ot.JJ_j2_qgl = get_property(ei.JJ_j2.get(),Jet_qgl);
    if(ei.JJ_j2) ot.JJ_j2_jetId   = get_property(ei.JJ_j2.get(),Jet_jetId); 
    if(ei.JJ_j2) ot.JJ_j2_puId    = get_property(ei.JJ_j2.get(),Jet_puId);
    if(ei.JJ_j2_rawpt) ot.JJ_j2_rawpt   = *ei.JJ_j2_rawpt;
    if(ei.JJ_j2_quarkID)        ot.JJ_j2_quarkID  = *ei.JJ_j2_quarkID;
    if(ei.JJ_j2_genJetIdx)                ot.JJ_j2_genJetIdx = *ei.JJ_j2_genJetIdx;      
    if(ei.JJ_j2_partonFlavour)        ot.JJ_j2_partonFlavour = *ei.JJ_j2_partonFlavour;
    if(ei.JJ_j2_hadronFlavour)        ot.JJ_j2_hadronFlavour = *ei.JJ_j2_hadronFlavour;
    if(ei.JJ_j2_location)        ot.JJ_j2_location = *ei.JJ_j2_location;
    COPY_OPTIONAL_m_pt_eta_phi_p4(JJ)
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_b1)
    if(ei.HH_btag_b1)                ot.HH_btag_b1_deepCSV = ei.HH_btag_b1.get().bTagScore();
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_b2)
    if(ei.HH_btag_b2)                ot.HH_btag_b2_deepCSV = ei.HH_btag_b2.get().bTagScore();
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_b3)
    if(ei.HH_btag_b3)                ot.HH_btag_b3_deepCSV = ei.HH_btag_b3.get().bTagScore();
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_b4)
    if(ei.HH_btag_b4)                ot.HH_btag_b4_deepCSV = ei.HH_btag_b4.get().bTagScore();

    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b1)
    if(ei.HH_btag_cmva_b1)                ot.HH_btag_cmva_b1_cmva = get_property(ei.HH_btag_cmva_b1.get(),Jet_btagCMVA);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b2)
    if(ei.HH_btag_cmva_b2)                ot.HH_btag_cmva_b2_cmva = get_property(ei.HH_btag_cmva_b2.get(),Jet_btagCMVA);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b3)
    if(ei.HH_btag_cmva_b3)                ot.HH_btag_cmva_b3_cmva = get_property(ei.HH_btag_cmva_b3.get(),Jet_btagCMVA);
    COPY_OPTIONAL_m_pt_ptRegressed_eta_phi_p4(HH_btag_cmva_b4)
    if(ei.HH_btag_cmva_b4)                ot.HH_btag_cmva_b4_cmva = get_property(ei.HH_btag_cmva_b4.get(),Jet_btagCMVA);

    if( ei.b1b2_deltaR) ot.b1b2_deltaR = *ei.b1b2_deltaR; 
    if( ei.b1b3_deltaR) ot.b1b3_deltaR = *ei.b1b3_deltaR; 
    if( ei.b1b4_deltaR) ot.b1b4_deltaR = *ei.b1b4_deltaR; 
    if( ei.b1j1_deltaR) ot.b1j1_deltaR = *ei.b1j1_deltaR; 
    if( ei.b1j2_deltaR) ot.b1j2_deltaR = *ei.b1j2_deltaR; 
    if( ei.b2b3_deltaR) ot.b2b3_deltaR = *ei.b2b3_deltaR; 
    if( ei.b2b4_deltaR) ot.b2b4_deltaR = *ei.b2b4_deltaR; 
    if( ei.b2j1_deltaR) ot.b2j1_deltaR = *ei.b2j1_deltaR; 
    if( ei.b2j2_deltaR) ot.b2j2_deltaR = *ei.b2j2_deltaR; 
    if( ei.b3b4_deltaR) ot.b3b4_deltaR = *ei.b3b4_deltaR; 
    if( ei.b3j1_deltaR) ot.b3j1_deltaR = *ei.b3j1_deltaR; 
    if( ei.b3j2_deltaR) ot.b3j2_deltaR = *ei.b3j2_deltaR; 
    if( ei.b4j1_deltaR) ot.b4j1_deltaR = *ei.b4j1_deltaR; 
    if( ei.b4j2_deltaR) ot.b4j2_deltaR = *ei.b4j2_deltaR; 
    if( ei.j1j2_deltaR) ot.j1j2_deltaR = *ei.j1j2_deltaR; 
    if( ei.h1h2_deltaR) ot.h1h2_deltaR = *ei.h1h2_deltaR; 
    if( ei.h1j1_deltaR) ot.h1j1_deltaR = *ei.h1j1_deltaR; 
    if( ei.h1j2_deltaR) ot.h1j2_deltaR = *ei.h1j2_deltaR; 
    if( ei.h2j1_deltaR) ot.h2j1_deltaR = *ei.h2j1_deltaR; 
    if( ei.h2j2_deltaR) ot.h2j2_deltaR = *ei.h2j2_deltaR; 
    if( ei.h1jj_deltaR) ot.h1jj_deltaR = *ei.h1jj_deltaR; 
    if( ei.h2jj_deltaR) ot.h2jj_deltaR = *ei.h2jj_deltaR; 
    if( ei.hhj1_deltaR) ot.hhj1_deltaR = *ei.hhj1_deltaR; 
    if( ei.hhj2_deltaR) ot.hhj2_deltaR = *ei.hhj2_deltaR; 
    if( ei.hhjj_deltaR) ot.hhjj_deltaR = *ei.hhjj_deltaR;
    if( ei.b1b2_deltaPhi) ot.b1b2_deltaPhi = *ei.b1b2_deltaPhi; 
    if( ei.b1b3_deltaPhi) ot.b1b3_deltaPhi = *ei.b1b3_deltaPhi; 
    if( ei.b1b4_deltaPhi) ot.b1b4_deltaPhi = *ei.b1b4_deltaPhi; 
    if( ei.b1j1_deltaPhi) ot.b1j1_deltaPhi = *ei.b1j1_deltaPhi; 
    if( ei.b1j2_deltaPhi) ot.b1j2_deltaPhi = *ei.b1j2_deltaPhi; 
    if( ei.b2b3_deltaPhi) ot.b2b3_deltaPhi = *ei.b2b3_deltaPhi; 
    if( ei.b2b4_deltaPhi) ot.b2b4_deltaPhi = *ei.b2b4_deltaPhi; 
    if( ei.b2j1_deltaPhi) ot.b2j1_deltaPhi = *ei.b2j1_deltaPhi; 
    if( ei.b2j2_deltaPhi) ot.b2j2_deltaPhi = *ei.b2j2_deltaPhi; 
    if( ei.b3b4_deltaPhi) ot.b3b4_deltaPhi = *ei.b3b4_deltaPhi; 
    if( ei.b3j1_deltaPhi) ot.b3j1_deltaPhi = *ei.b3j1_deltaPhi; 
    if( ei.b3j2_deltaPhi) ot.b3j2_deltaPhi = *ei.b3j2_deltaPhi; 
    if( ei.b4j1_deltaPhi) ot.b4j1_deltaPhi = *ei.b4j1_deltaPhi; 
    if( ei.b4j2_deltaPhi) ot.b4j2_deltaPhi = *ei.b4j2_deltaPhi; 
    if( ei.j1j2_deltaPhi) ot.j1j2_deltaPhi = *ei.j1j2_deltaPhi; 
    if( ei.b1b2_deltaEta) ot.b1b2_deltaEta = *ei.b1b2_deltaEta; 
    if( ei.b1b3_deltaEta) ot.b1b3_deltaEta = *ei.b1b3_deltaEta; 
    if( ei.b1b4_deltaEta) ot.b1b4_deltaEta = *ei.b1b4_deltaEta; 
    if( ei.b1j1_deltaEta) ot.b1j1_deltaEta = *ei.b1j1_deltaEta; 
    if( ei.b1j2_deltaEta) ot.b1j2_deltaEta = *ei.b1j2_deltaEta; 
    if( ei.b2b3_deltaEta) ot.b2b3_deltaEta = *ei.b2b3_deltaEta; 
    if( ei.b2b4_deltaEta) ot.b2b4_deltaEta = *ei.b2b4_deltaEta; 
    if( ei.b2j1_deltaEta) ot.b2j1_deltaEta = *ei.b2j1_deltaEta; 
    if( ei.b2j2_deltaEta) ot.b2j2_deltaEta = *ei.b2j2_deltaEta; 
    if( ei.b3b4_deltaEta) ot.b3b4_deltaEta = *ei.b3b4_deltaEta; 
    if( ei.b3j1_deltaEta) ot.b3j1_deltaEta = *ei.b3j1_deltaEta; 
    if( ei.b3j2_deltaEta) ot.b3j2_deltaEta = *ei.b3j2_deltaEta; 
    if( ei.b4j1_deltaEta) ot.b4j1_deltaEta = *ei.b4j1_deltaEta; 
    if( ei.b4j2_deltaEta) ot.b4j2_deltaEta = *ei.b4j2_deltaEta; 
    if( ei.j1j2_deltaEta) ot.j1j2_deltaEta = *ei.j1j2_deltaEta; 
    if( ei.h1h2_deltaPhi) ot.h1h2_deltaPhi = *ei.h1h2_deltaPhi; 
    if( ei.h1j1_deltaPhi) ot.h1j1_deltaPhi = *ei.h1j1_deltaPhi; 
    if( ei.h1j2_deltaPhi) ot.h1j2_deltaPhi = *ei.h1j2_deltaPhi; 
    if( ei.h2j1_deltaPhi) ot.h2j1_deltaPhi = *ei.h2j1_deltaPhi; 
    if( ei.h2j2_deltaPhi) ot.h2j2_deltaPhi = *ei.h2j2_deltaPhi; 
    if( ei.h1jj_deltaPhi) ot.h1jj_deltaPhi = *ei.h1jj_deltaPhi; 
    if( ei.h2jj_deltaPhi) ot.h2jj_deltaPhi = *ei.h2jj_deltaPhi; 
    if( ei.hhj1_deltaPhi) ot.hhj1_deltaPhi = *ei.hhj1_deltaPhi; 
    if( ei.hhj2_deltaPhi) ot.hhj2_deltaPhi = *ei.hhj2_deltaPhi; 
    if( ei.hhjj_deltaPhi) ot.hhjj_deltaPhi = *ei.hhjj_deltaPhi; 
    if( ei.h1h2_deltaEta) ot.h1h2_deltaEta = *ei.h1h2_deltaEta; 
    if( ei.h1j1_deltaEta) ot.h1j1_deltaEta = *ei.h1j1_deltaEta; 
    if( ei.h1j2_deltaEta) ot.h1j2_deltaEta = *ei.h1j2_deltaEta; 
    if( ei.h2j1_deltaEta) ot.h2j1_deltaEta = *ei.h2j1_deltaEta; 
    if( ei.h2j2_deltaEta) ot.h2j2_deltaEta = *ei.h2j2_deltaEta; 
    if( ei.h1jj_deltaEta) ot.h1jj_deltaEta = *ei.h1jj_deltaEta; 
    if( ei.h2jj_deltaEta) ot.h2jj_deltaEta = *ei.h2jj_deltaEta; 
    if( ei.hhj1_deltaEta) ot.hhj1_deltaEta = *ei.hhj1_deltaEta; 
    if( ei.hhj2_deltaEta) ot.hhj2_deltaEta = *ei.hhj2_deltaEta; 
    if( ei.hhjj_deltaEta) ot.hhjj_deltaEta = *ei.hhjj_deltaEta;
    if(ei.VBFEvent) ot.VBFEvent  = *ei.VBFEvent;
    if(ei.VBFEventLocation) ot.VBFEventLocation  = *ei.VBFEventLocation;
    if(ei.H1_ukflag) ot.H1_ukflag  = *ei.H1_ukflag;
    if(ei.H2_ukflag) ot.H2_ukflag  = *ei.H2_ukflag;
    if(ei.H1_wpflag) ot.H1_wpflag  = *ei.H1_wpflag;
    if(ei.H2_wpflag) ot.H2_wpflag  = *ei.H2_wpflag;    
    if(ei.H1_quality) ot.H1_quality  = *ei.H1_quality;
    if(ei.H2_quality) ot.H2_quality  = *ei.H2_quality;
    if(ei.HH_quality) ot.HH_quality  = *ei.HH_quality;
    if(ei.HH_1wflag) ot.HH_1wflag  = *ei.HH_1wflag;
    if(ei.HH_2wflag) ot.HH_2wflag  = *ei.HH_2wflag;
    if(ei.HH_3wflag) ot.HH_3wflag  = *ei.HH_3wflag;
    if(ei.HH_4wflag) ot.HH_4wflag  = *ei.HH_4wflag;
    if(ei.JJ_quality) ot.JJ_quality= *ei.JJ_quality;
    if(ei.JJ_1wflag) ot.JJ_1wflag  = *ei.JJ_1wflag;
    if(ei.JJ_2wflag) ot.JJ_2wflag  = *ei.JJ_2wflag;    
    if(ei.HHJJ_quality) ot.HHJJ_quality  = *ei.HHJJ_quality;
    if(ei.NPartonsMatchedToJets) ot.NPartonsMatchedToJets  = *ei.NPartonsMatchedToJets;
    if(ei.NVBFCandidateswithVBFMatch) ot.NVBFCandidateswithVBFMatch  = *ei.NVBFCandidateswithVBFMatch;
    if(ei.VBFMatchedCandidatesEtaSign) ot.VBFMatchedCandidatesEtaSign  = *ei.VBFMatchedCandidatesEtaSign;
    if(ei.VBFMatchedCandidatesDeltaEta) ot.VBFMatchedCandidatesDeltaEta  = *ei.VBFMatchedCandidatesDeltaEta;
    if(ei.VBFSelectedDeltaEta) ot.VBFSelectedDeltaEta  = *ei.VBFSelectedDeltaEta;
    if(ei.VBFSelectedEtaSign) ot.VBFSelectedEtaSign  = *ei.VBFSelectedEtaSign;
    if(ei.VBFGeneratedDeltaEta) ot.VBFGeneratedDeltaEta  = *ei.VBFGeneratedDeltaEta;
    if(ei.VBFGeneratedEtaSign) ot.VBFGeneratedEtaSign  = *ei.VBFGeneratedEtaSign;

    if(ei.nJet) ot.nJet = *ei.nJet;
    if(ei.nPVgood) ot.nPVgood  = *ei.nPVgood;
    if(ei.NVBFCandidates) ot.NVBFCandidates = *ei.NVBFCandidates;  
    if(ei.nJetbarrel) ot.nJetbarrel = *ei.nJetbarrel;
    if(ei.nJetendcap) ot.nJetendcap = *ei.nJetendcap;
    if(ei.nJethf) ot.nJethf = *ei.nJethf;
    if(ei.maxj1etaj2eta) ot.maxj1etaj2eta = *ei.maxj1etaj2eta;
    if(ei.j1etaj2eta) ot.j1etaj2eta = *ei.j1etaj2eta;
    if(ei.BDT1) ot.BDT1 = *ei.BDT1;
    if(ei.BDT2) ot.BDT2 = *ei.BDT2;
    if(ei.BDT3) ot.BDT3 = *ei.BDT3;
    if(ei.ExtraVBFJetDecision) ot.ExtraVBFJetDecision = *ei.ExtraVBFJetDecision;
    if(ei.ExtraVBFJetPt) ot.ExtraVBFJetPt = *ei.ExtraVBFJetPt;
    if(ei.ExtraVBFJetEta) ot.ExtraVBFJetEta = *ei.ExtraVBFJetEta;
    if(ei.costh_H1_cm   ) ot.costh_H1_cm   = *ei.costh_H1_cm   ;       
    if(ei.costh_H2_cm   ) ot.costh_H2_cm   = *ei.costh_H2_cm   ;   
    if(ei.costh_HH_cm   ) ot.costh_HH_cm   = *ei.costh_HH_cm   ;   
    if(ei.costh_JJ_cm   ) ot.costh_JJ_cm   = *ei.costh_JJ_cm   ;   
    if(ei.costh_HH_b1_cm) ot.costh_HH_b1_cm= *ei.costh_HH_b1_cm;    
    if(ei.costh_HH_b2_cm) ot.costh_HH_b2_cm= *ei.costh_HH_b2_cm;
    if(ei.costh_HH_b3_cm) ot.costh_HH_b3_cm= *ei.costh_HH_b3_cm;    
    if(ei.costh_HH_b4_cm) ot.costh_HH_b4_cm= *ei.costh_HH_b4_cm;
    if(ei.costh_JJ_j1_cm) ot.costh_JJ_j1_cm= *ei.costh_JJ_j1_cm;    
    if(ei.costh_JJ_j2_cm) ot.costh_JJ_j2_cm= *ei.costh_JJ_j2_cm;
    COPY_OPTIONAL_m_pt_eta_phi_p4(H1rand)
    COPY_OPTIONAL_m_pt_eta_phi_p4(H2rand)
    COPY_OPTIONAL_m_pt_eta_phi_p4(X_b_1)
    COPY_OPTIONAL_m_pt_eta_phi_p4(X_b_2)
    COPY_OPTIONAL_m_pt_eta_phi_p4(H_b_1)
    COPY_OPTIONAL_m_pt_eta_phi_p4(H_b_2)
    COPY_OPTIONAL_m_pt_eta_phi_p4(RecoMatched_X_b_1)
    if (ei.RecoMatched_X_b_1) ot.RecoMatched_X_b_1_deepCSV = ei.RecoMatched_X_b_1.get().bTagScore();
    if (ei.RecoMatched_X_b_1_deltaR) ot.RecoMatched_X_b_1_deltaR = *ei.RecoMatched_X_b_1_deltaR; 
    COPY_OPTIONAL_m_pt_eta_phi_p4(RecoMatched_X_b_2)
    if (ei.RecoMatched_X_b_2) ot.RecoMatched_X_b_2_deepCSV = ei.RecoMatched_X_b_2.get().bTagScore();
    if (ei.RecoMatched_X_b_2_deltaR) ot.RecoMatched_X_b_2_deltaR = *ei.RecoMatched_X_b_2_deltaR; 
    COPY_OPTIONAL_m_pt_eta_phi_p4(RecoMatched_H_b_1)
    if (ei.RecoMatched_H_b_1) ot.RecoMatched_H_b_1_deepCSV = ei.RecoMatched_H_b_1.get().bTagScore();
    if (ei.RecoMatched_H_b_1_deltaR) ot.RecoMatched_H_b_1_deltaR = *ei.RecoMatched_H_b_1_deltaR; 
    COPY_OPTIONAL_m_pt_eta_phi_p4(RecoMatched_H_b_2)
    if (ei.RecoMatched_H_b_2) ot.RecoMatched_H_b_2_deepCSV = ei.RecoMatched_H_b_2.get().bTagScore();
    if (ei.RecoMatched_H_b_2_deltaR) ot.RecoMatched_H_b_2_deltaR = *ei.RecoMatched_H_b_2_deltaR; 
    if(ei.H1_bb_deltaR) ot.H1_bb_deltaR= *ei.H1_bb_deltaR;
    if(ei.H2_bb_deltaR) ot.H2_bb_deltaR= *ei.H2_bb_deltaR;
    if(ei.H1_bb_deltaPhi) ot.H1_bb_deltaPhi= *ei.H1_bb_deltaPhi;
    if(ei.H2_bb_deltaPhi) ot.H2_bb_deltaPhi= *ei.H2_bb_deltaPhi;
    if(ei.H1_bb_deltaEta) ot.H1_bb_deltaEta= *ei.H1_bb_deltaEta;
    if(ei.H2_bb_deltaEta) ot.H2_bb_deltaEta= *ei.H2_bb_deltaEta;
    if(ei.NbJets) ot.NbJets= *ei.NbJets;
    if(ei.TotalFourJetPt) ot.TotalFourJetPt= *ei.TotalFourJetPt;
    if(ei.MinDeltaEtaJJ) ot.MinDeltaEtaJJ= *ei.MinDeltaEtaJJ;

    // gen info are not stored for all samples --> set only if initialized (macro checks if object is initialized, else does not set)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1_last)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2_last)

    if (ei.gen_H1 && ei.gen_H2)
    {
        TLorentzVector p4_HH = ei.gen_H1->P4() + ei.gen_H2->P4();
        ot.gen_mHH = p4_HH.M();
    }

    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1_b1)
    if(ei.gen_H1_b1_matchedflag) ot.gen_H1_b1_matchedflag  = *ei.gen_H1_b1_matchedflag;
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H1_b2)
    if(ei.gen_H1_b2_matchedflag) ot.gen_H1_b2_matchedflag  = *ei.gen_H1_b2_matchedflag;
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2_b1)
    if(ei.gen_H2_b1_matchedflag) ot.gen_H2_b1_matchedflag  = *ei.gen_H2_b1_matchedflag;
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_H2_b2)
    if(ei.gen_H2_b2_matchedflag) ot.gen_H2_b2_matchedflag = *ei.gen_H2_b2_matchedflag;
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q1_in)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q2_in)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q1_out)
    COPY_OPTIONAL_m_pt_eta_phi_p4(gen_q2_out) 




    if(ei.recoJetMatchedToGenJet1) ot.recoJetMatchedToGenJet1 = *ei.recoJetMatchedToGenJet1;
    if(ei.recoJetMatchedToGenJet2) ot.recoJetMatchedToGenJet2 = *ei.recoJetMatchedToGenJet2;
    if(ei.recoJetMatchedToGenJet3) ot.recoJetMatchedToGenJet3 = *ei.recoJetMatchedToGenJet3;
    if(ei.recoJetMatchedToGenJet4) ot.recoJetMatchedToGenJet4 = *ei.recoJetMatchedToGenJet4;
  



    if (ei.gen_q1_out && ei.gen_q2_out)
    {
        TLorentzVector p4_JJ = ei.gen_q1_out->P4() + ei.gen_q2_out->P4();
        ot.gen_mJJ = p4_JJ.M();
    }    

    if (ei.gen_q1_out && ei.gen_q2_out)
    {
        ot.gen_deltaEtaJJ = abs( ei.gen_q1_out->P4().Eta() - ei.gen_q2_out->P4().Eta() );
    }    

    if(ei.gen_H1_b1_mindRjet) ot.gen_H1_b1_mindRjet = *ei.gen_H1_b1_mindRjet;
    if(ei.gen_H1_b2_mindRjet) ot.gen_H1_b2_mindRjet = *ei.gen_H1_b2_mindRjet;
    if(ei.gen_H2_b1_mindRjet) ot.gen_H2_b1_mindRjet = *ei.gen_H2_b1_mindRjet;
    if(ei.gen_H2_b2_mindRjet) ot.gen_H2_b2_mindRjet = *ei.gen_H2_b2_mindRjet;
    if(ei.gen_q1_out_mindRjet) ot.gen_q1_out_mindRjet = *ei.gen_q1_out_mindRjet; 
    if(ei.gen_q2_out_mindRjet) ot.gen_q2_out_mindRjet = *ei.gen_q2_out_mindRjet;
     if(ei.gen_H1_b1_matched)  ot.gen_H1_b1_matched =  *ei.gen_H1_b1_matched;
     if(ei.gen_H1_b2_matched)  ot.gen_H1_b2_matched =  *ei.gen_H1_b2_matched;
     if(ei.gen_H2_b1_matched)  ot.gen_H2_b1_matched =  *ei.gen_H2_b1_matched;
     if(ei.gen_H2_b2_matched)  ot.gen_H2_b2_matched =  *ei.gen_H2_b2_matched;
    if(ei.gen_q1_out_matched) ot.gen_q1_out_matched = *ei.gen_q1_out_matched; 
    if(ei.gen_q2_out_matched) ot.gen_q2_out_matched = *ei.gen_q2_out_matched;
     if(ei.gen_H1_b1_pdgId)  ot.gen_H1_b1_pdgId =  *ei.gen_H1_b1_pdgId;
     if(ei.gen_H1_b2_pdgId)  ot.gen_H1_b2_pdgId =  *ei.gen_H1_b2_pdgId;
     if(ei.gen_H2_b1_pdgId)  ot.gen_H2_b1_pdgId =  *ei.gen_H2_b1_pdgId;
     if(ei.gen_H2_b2_pdgId)  ot.gen_H2_b2_pdgId =  *ei.gen_H2_b2_pdgId;
    if(ei.gen_q1_out_pdgId) ot.gen_q1_out_pdgId = *ei.gen_q1_out_pdgId; 
    if(ei.gen_q2_out_pdgId) ot.gen_q2_out_pdgId = *ei.gen_q2_out_pdgId;
     if(ei.gen_H1_b1_ptratio)  ot.gen_H1_b1_ptratio =  *ei.gen_H1_b1_ptratio;
     if(ei.gen_H1_b2_ptratio)  ot.gen_H1_b2_ptratio =  *ei.gen_H1_b2_ptratio;
     if(ei.gen_H2_b1_ptratio)  ot.gen_H2_b1_ptratio =  *ei.gen_H2_b1_ptratio;
     if(ei.gen_H2_b2_ptratio)  ot.gen_H2_b2_ptratio =  *ei.gen_H2_b2_ptratio;
    if(ei.gen_q1_out_ptratio) ot.gen_q1_out_ptratio = *ei.gen_q1_out_ptratio; 
    if(ei.gen_q2_out_ptratio) ot.gen_q2_out_ptratio = *ei.gen_q2_out_ptratio;
     if(ei.gen_H1_b1_pz)  ot.gen_H1_b1_pz =  *ei.gen_H1_b1_pz;
     if(ei.gen_H1_b2_pz)  ot.gen_H1_b2_pz =  *ei.gen_H1_b2_pz;
     if(ei.gen_H2_b1_pz)  ot.gen_H2_b1_pz =  *ei.gen_H2_b1_pz;
     if(ei.gen_H2_b2_pz)  ot.gen_H2_b2_pz =  *ei.gen_H2_b2_pz;
    if(ei.gen_q1_out_pz) ot.gen_q1_out_pz = *ei.gen_q1_out_pz; 
    if(ei.gen_q2_out_pz) ot.gen_q2_out_pz = *ei.gen_q2_out_pz;
    if(ei.gen_H1_b1_mindRgenjet) ot.gen_H1_b1_mindRgenjet = *ei.gen_H1_b1_mindRgenjet;
    if(ei.gen_H1_b2_mindRgenjet) ot.gen_H1_b2_mindRgenjet = *ei.gen_H1_b2_mindRgenjet;
    if(ei.gen_H2_b1_mindRgenjet) ot.gen_H2_b1_mindRgenjet = *ei.gen_H2_b1_mindRgenjet;
    if(ei.gen_H2_b2_mindRgenjet) ot.gen_H2_b2_mindRgenjet = *ei.gen_H2_b2_mindRgenjet;
    if(ei.gen_q1_out_mindRgenjet) ot.gen_q1_out_mindRgenjet = *ei.gen_q1_out_mindRgenjet; 
    if(ei.gen_q2_out_mindRgenjet) ot.gen_q2_out_mindRgenjet = *ei.gen_q2_out_mindRgenjet;
    // fill the tree
    ot.fill();

}