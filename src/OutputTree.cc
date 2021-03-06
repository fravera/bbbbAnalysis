#include "OutputTree.h"

#include <iostream>

using namespace std;

// helper: creates the pt/eta/phi/p4 branches of a variable OBJ
#define BRANCH_pt_eta_phi_p4(OBJ) \
    tree_->Branch(#OBJ "_pt",  &OBJ ## _pt); \
    tree_->Branch(#OBJ "_eta", &OBJ ## _eta); \
    tree_->Branch(#OBJ "_phi", &OBJ ## _phi); \
    if (savetlv_) tree_->Branch(#OBJ "_p4", &OBJ ## _p4);

#define CLEAR_pt_eta_phi_p4(OBJ) \
    OBJ ## _pt   = -999.; \
    OBJ ## _eta  = -999.; \
    OBJ ## _phi  = -999.; \
    OBJ ## _p4 . SetPxPyPzE(0,0,0,0);

OutputTree::OutputTree (bool savetlv, string name, string title) :
savetlv_ (savetlv)
{
    tree_ = std::unique_ptr<TTree> (new TTree(name.c_str(), title.c_str()));
    
    init_branches();
    clear();
}

void OutputTree::init_branches()
{
    // reco b jets
    BRANCH_pt_eta_phi_p4(H1_b1)
    BRANCH_pt_eta_phi_p4(H1_b2)
    BRANCH_pt_eta_phi_p4(H2_b1)
    BRANCH_pt_eta_phi_p4(H2_b2)

    // reco H1 (H1_b1 + H1_b2)
    // reco H2 (H2_b1 + H2_b2)
    BRANCH_pt_eta_phi_p4(H1)
    BRANCH_pt_eta_phi_p4(H2)

    tree_->Branch("gen_mHH", &gen_mHH);

    BRANCH_pt_eta_phi_p4(gen_H1)
    BRANCH_pt_eta_phi_p4(gen_H2)
    BRANCH_pt_eta_phi_p4(gen_H1_last)
    BRANCH_pt_eta_phi_p4(gen_H2_last)
    BRANCH_pt_eta_phi_p4(gen_H1_b1)
    BRANCH_pt_eta_phi_p4(gen_H1_b2)
    BRANCH_pt_eta_phi_p4(gen_H2_b1)
    BRANCH_pt_eta_phi_p4(gen_H2_b2)
    BRANCH_pt_eta_phi_p4(gen_q1_in)
    BRANCH_pt_eta_phi_p4(gen_q2_in)
    BRANCH_pt_eta_phi_p4(gen_q1_out)
    BRANCH_pt_eta_phi_p4(gen_q2_out)

    // note that the initialization of the user branches is made separately when calling declareUser*Branch
}

void OutputTree::clear()
{
    CLEAR_pt_eta_phi_p4(H1_b1)
    CLEAR_pt_eta_phi_p4(H1_b2)
    CLEAR_pt_eta_phi_p4(H2_b1)
    CLEAR_pt_eta_phi_p4(H2_b2)

    CLEAR_pt_eta_phi_p4(H1)
    CLEAR_pt_eta_phi_p4(H2)

    gen_mHH = 0;    

    CLEAR_pt_eta_phi_p4(gen_H1)
    CLEAR_pt_eta_phi_p4(gen_H2)
    CLEAR_pt_eta_phi_p4(gen_H1_last)
    CLEAR_pt_eta_phi_p4(gen_H2_last)
    CLEAR_pt_eta_phi_p4(gen_H1_b1)
    CLEAR_pt_eta_phi_p4(gen_H1_b2)
    CLEAR_pt_eta_phi_p4(gen_H2_b1)
    CLEAR_pt_eta_phi_p4(gen_H2_b2)
    CLEAR_pt_eta_phi_p4(gen_q1_in)
    CLEAR_pt_eta_phi_p4(gen_q2_in)
    CLEAR_pt_eta_phi_p4(gen_q1_out)
    CLEAR_pt_eta_phi_p4(gen_q2_out)

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

    // set the branch
    tree_->Branch(name.c_str(), userFloats_.getValPtr(name));
    return true;
}