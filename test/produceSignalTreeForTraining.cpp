/*
** class  : skim_ntuple.cpp
** author : L. Cadamuro (UF)
** date   : 31/12/2017
** brief  : transforms a NanoAOD into a bbbb ntuple for the subsequent plots/analysis
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <any>
#include <algorithm>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "CfgParser.h"
#include "NanoAODTree.h"
#include "EventInfo.h"

#include "SkimUtils.h"
namespace su = SkimUtils;

#include "OfflineProducerHelper.h"
// namespace oph = OfflineProducerHelper;

#include "OutputTree.h"
#include "SkimEffCounter.h"
#include "jsonLumiFilter.h"

#include "TFile.h"
#include "TSystem.h"

using namespace std;

bool debug = false;
// skim_ntuple.exe --cfg config/skim.cfg --input inputFiles/Samples_80X/VBF_HH_4b_10gen2018.txt --output test_bbbb_tree.root --xs 10 --is-signal

#define ASSERT_WITH_MESSAGE(condition) do { \
if (!(condition)) { std::cout << __PRETTY_FUNCTION__ << " " << __LINE__ << std::endl; } \
assert ((condition)); } while(false)

int getClosestJetIndex(const GenPart& theGenParticle, std::vector<Jet>& theJetList, float maxDeltaRaccepted, std::vector<bool>& alreadyMatched)
{
    int closestJetIndex = -1;
    int currentJetIndex = 0;
    float minDeltaR = 1024;
    for(const auto & theJet : theJetList)
    {
        if(!alreadyMatched[currentJetIndex])
        {
            float tmpDeltaR = OfflineProducerHelper::deltaPhi(theJet.P4().Phi(),theGenParticle.P4().Phi())*OfflineProducerHelper::deltaPhi(theJet.P4().Phi(),theGenParticle.P4().Phi()) + (theJet.P4().Eta()-theGenParticle.P4().Eta())*(theJet.P4().Eta()-theGenParticle.P4().Eta());
            if( tmpDeltaR<minDeltaR)
            {
                minDeltaR = tmpDeltaR;
                closestJetIndex = currentJetIndex;
            }
        }
        currentJetIndex++;
    }
    if(minDeltaR > (maxDeltaRaccepted*maxDeltaRaccepted))
    {
        if(debug) std::cout<<"minDeltaR = "<<sqrt(minDeltaR)<<std::endl;
        if(debug) std::cout<<"Gen particle (Pt, Eta, Phi) = "<<theGenParticle.P4().Pt() << "\t\t" << theGenParticle.P4().Eta() << "\t\t" << theGenParticle.P4().Phi() <<std::endl;
        if(debug) std::cout<<"Closest Jet  (Pt, Eta, Phi) = "<<theJetList[closestJetIndex].P4().Pt() << "\t\t" << theJetList[closestJetIndex].P4().Eta() << "\t\t" << theJetList[closestJetIndex].P4().Phi() <<std::endl;
        closestJetIndex = -1;
    }

    return closestJetIndex;
}

void updateMatchingList(std::vector<int>& genMatchedJets, const GenPart& theGenParticle, std::vector<Jet>& theJetList, float maxDeltaRaccepted, int& numberOfMatching, std::vector<bool>& alreadyMatched)
{
    int matchedJetIndex = getClosestJetIndex(theGenParticle,theJetList,maxDeltaRaccepted,alreadyMatched);
    if(matchedJetIndex >= 0 && numberOfMatching < 2)
    {
        alreadyMatched[matchedJetIndex] = true;
        genMatchedJets[numberOfMatching] = matchedJetIndex;
    }
    ++numberOfMatching;

}

template<typename T>
void attachToTrees(TTree *signalTree, TTree *backgroundTree, T& variable, const char* branchName)
{
    signalTree    ->Branch(branchName, &variable);
    backgroundTree->Branch(branchName, &variable);
}

bool isDerivingFromMotherWithId(const GenPart& theParticle, int targerMotherId)
{
    int motherIndex = get_property(theParticle, GenPart_genPartIdxMother);
    if(motherIndex<0) return false;
    GenPart mother (motherIndex, theParticle.getNanoAODTree());
    int motherId = get_property(mother, GenPart_pdgId);
    // std::cout<<motherId<<std::endl;

    if(motherId == targerMotherId) return true;
    else return isDerivingFromMotherWithId(mother, targerMotherId);
    
}

void dumpList(std::vector<int> integerList)
{
    std::cout<<"List of iterators: ";
    for(auto i : integerList) std::cout<<i<<" ";
    std::cout<<std::endl;
    return;
};

bool checkUniqueAndFilled(std::vector<int> tmpList)
{
    std::vector<int> copyList = tmpList;

    std::sort(tmpList.begin(), tmpList.end());
    if(std::unique(tmpList.begin(), tmpList.end()) != tmpList.end()) {dumpList(tmpList); return false;};
    if(![&tmpList]() -> bool {for(auto i : tmpList) {if(i == -1) return false; } return true; }()) {dumpList(tmpList); return false;};
    return true;
}

int main(int argc, char** argv)
{
    gSystem->ResetSignal(kSigSegmentationViolation, kTRUE);
    
    cout << "[INFO] ... starting program" << endl;

    ////////////////////////////////////////////////////////////////////////
    // Declare command line options
    ////////////////////////////////////////////////////////////////////////
    
    po::options_description desc("Skim options");
    desc.add_options()
        ("help", "produce help message")
        // required
        ("cfg"   , po::value<string>()->required(), "skim config")
        ("input" , po::value<string>()->required(), "input file list")
        ("output", po::value<string>()->required(), "output file LFN")
        ("yMassSelection", po::value<string>()->default_value("None"), "Y mass selection")
        // optional
        ("maxEvts"  , po::value<int>()->default_value(-1), "max number of events to process")

    ;

    po::variables_map opts;
    try {
        po::store(parse_command_line(argc, argv, desc, po::command_line_style::unix_style ^ po::command_line_style::allow_short), opts);
        if (opts.count("help")) {
            cout << desc << "\n";
            return 1;
        }
        po::notify(opts);
    }    
    catch (po::error& e) {
        cerr << "** [ERROR] " << e.what() << endl;
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////
    // Read config and other cmd line options for skims
    ////////////////////////////////////////////////////////////////////////

    CfgParser config;
    if (!config.init(opts["cfg"].as<string>())) return 1;
    cout << "[INFO] ... using config file " << opts["cfg"].as<string>() << endl;
    
    ////////////////////////////////////////////////////////////////////////
    // Read needed fields from config file and pass them to the oph
    ////////////////////////////////////////////////////////////////////////

    std::map<std::string, std::any> parameterList;

    const string bbbbChoice = config.readStringOpt("parameters::bbbbChoice");
    
    const string preselectionCutStrategy = config.readStringOpt("parameters::PreselectionCut");
    parameterList.emplace("PreselectionCut",preselectionCutStrategy);
    if(preselectionCutStrategy == "bJetCut")
    {
        parameterList.emplace("MinDeepCSV"          ,config.readFloatOpt("parameters::MinDeepCSV"          ));
        parameterList.emplace("MinPt"               ,config.readFloatOpt("parameters::MinPt"               ));
        parameterList.emplace("MaxAbsEta"           ,config.readFloatOpt("parameters::MaxAbsEta"           ));
    }
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize event choice ObjectsForCut " + preselectionCutStrategy);


    // MC only procedures
    //JER 
    const string JERstrategy = config.readStringOpt("parameters::JetEnergyResolution");
    parameterList.emplace("JetEnergyResolution",JERstrategy);
    if(JERstrategy == "StandardJER"){
        parameterList.emplace("JERComputeVariations" ,config.readBoolOpt  ("parameters::JERComputeVariations" ));
        parameterList.emplace("RandomGeneratorSeed"  ,config.readIntOpt   ("parameters::RandomGeneratorSeed"  ));
        parameterList.emplace("JERResolutionFile"    ,config.readStringOpt("parameters::JERResolutionFile"    ));
        parameterList.emplace("JERScaleFactorFile"   ,config.readStringOpt("parameters::JERScaleFactorFile"    ));
    }
    else if(JERstrategy == "None"){
    }  
    // else if(other selection type){
    //     parameters fo be retreived;
    // }  
    else throw std::runtime_error("cannot recognize event choice ObjectsForCut " + JERstrategy);


    OfflineProducerHelper oph;
    oph.initializeOfflineProducerHelper(&parameterList);

    ////////////////////////////////////////////////////////////////////////
    // Prepare event loop
    ////////////////////////////////////////////////////////////////////////

    cout << "[INFO] ... opening file list : " << opts["input"].as<string>().c_str() << endl;
    if ( access( opts["input"].as<string>().c_str(), F_OK ) == -1 )
    {
        throw std::runtime_error("** [ERROR] The input file list does not exist, aborting");
    }

    // Joining all the NANOAD input file in a TChain in order to be used like an unique three
    TChain ch("Events");
    int nfiles = su::appendFromFileList(&ch, opts["input"].as<string>());
    
    if (nfiles == 0)
    {
        throw std::runtime_error("** [ERROR] The input file list contains no files, aborting");
    }
    cout << "[INFO] ... file list contains " << nfiles << " files" << endl;

    cout << "[INFO] ... creating tree reader" << endl;

    // The TChain is passed to the NanoAODTree_SetBranchImpl to parse all the branches
    // NanoAODTree nat (&ch);
    NanoAODTree nat (&ch, false);

    cout << "[INFO] ... loading the following triggers" << endl;

    std::vector<std::string> triggerAndNameVector = config.readStringListOpt("triggers::makeORof");
    std::vector<std::string> triggerVector;
    // <triggerName , < objectBit, minNumber> >
    std::map<std::string, std::map< std::pair<int,int>, int > > triggerObjectAndMinNumberMap;
    
    for (auto & trigger : triggerAndNameVector)
    {
        if(trigger=="") continue;

        std::string delimiter = ":";
        size_t pos = 0;
        std::vector<std::string> triggerTokens;
        while ((pos = trigger.find(delimiter)) != std::string::npos)
        {
            triggerTokens.push_back(trigger.substr(0, pos));
            trigger.erase(0, pos + delimiter.length());
        }
        triggerTokens.push_back(trigger); // last part splitted
        if (triggerTokens.size() != 2)
        {
            throw std::runtime_error("** skim_ntuple : could not parse trigger entry " + trigger + " , aborting");
        }

        triggerVector.push_back(triggerTokens[1]);
        cout << "   - " << triggerTokens[1] << endl;

        if(!config.hasOpt( Form("triggers::%s_ObjectRequirements",triggerTokens[0].data()) ))
        {
            cout<<Form("triggers::%s_ObjectRequirements",triggerTokens[0].data())<<std::endl;
            cout<<"Trigger "<< triggerTokens[1] <<" does not have ObjectRequirements are not defined";
            continue;
        }

        triggerObjectAndMinNumberMap[triggerTokens[1]] = std::map< std::pair<int,int>, int>();   

        std::vector<std::string> triggerObjectMatchingVector = config.readStringListOpt(Form("triggers::%s_ObjectRequirements",triggerTokens[0].data()));

        for (auto & triggerObject : triggerObjectMatchingVector)
        {

            std::vector<std::string> triggerObjectTokens;
            while ((pos = triggerObject.find(delimiter)) != std::string::npos)
            {
                triggerObjectTokens.push_back(triggerObject.substr(0, pos));
                triggerObject.erase(0, pos + delimiter.length());
            }
            triggerObjectTokens.push_back(triggerObject); // last part splitted
            if (triggerObjectTokens.size() != 3)
            {
                throw std::runtime_error("** skim_ntuple : could not parse trigger entry " + triggerObject + " , aborting");
            }

            triggerObjectAndMinNumberMap[triggerTokens[1]][std::pair<int,int>(atoi(triggerObjectTokens[0].data()),atoi(triggerObjectTokens[1].data()))] = atoi(triggerObjectTokens[2].data());
        }

    }

    parameterList.emplace("MaxDeltaR",                config.readFloatOpt("triggers::MaxDeltaR")     );     
    parameterList.emplace("MatchWithSelectedObjects", config.readBoolOpt("triggers::MatchWithSelectedObjects")     ); 
    // parameterList.emplace("TriggerStudies",config.readBoolOpt ("triggers::TriggerStudies"));        
    parameterList.emplace("TriggerObjectAndMinNumberMap", triggerObjectAndMinNumberMap);
    nat.triggerReader().setTriggers(triggerVector);

    ////////////////////////////////////////////////////////////////////////
    // Prepare the output
    ////////////////////////////////////////////////////////////////////////
    
    string outputFileName = opts["output"].as<string>();
    cout << "[INFO] ... saving output to file : " << outputFileName << endl;
    TFile outputFile(outputFileName.c_str(), "recreate");

    OutputTree ot; //dummy;
    oph.initializeJERsmearingAndVariations(ot);


    ////////////////////////////////////////////////////////////////////////
    // Execute event loop
    ////////////////////////////////////////////////////////////////////////

    TTree *signalTree     = new TTree("signalTree"    , "signalTree"    );
    TTree *backgroundTree = new TTree("backgroundTree", "backgroundTree");
    TTree *selectionTree  = new TTree("selectionTree" , "backgroundTree");
    
    // X variables
    float X_pt = -1.;
    attachToTrees(signalTree, backgroundTree, X_pt, "X_pt");
    float HY_deltaEta = -1.;
    attachToTrees(signalTree, backgroundTree, HY_deltaEta, "HY_deltaEta");
    float HY_deltaPhi = -1.;
    attachToTrees(signalTree, backgroundTree, HY_deltaPhi, "HY_deltaPhi");

    // H - Y variables
    float H_mass = -1.;
    attachToTrees(signalTree, backgroundTree, H_mass, "H_mass");
    float Y_mass = -1.;
    attachToTrees(signalTree, backgroundTree, Y_mass, "Y_mass");
    float Hb1b2_deltaR = -99.;
    attachToTrees(signalTree, backgroundTree, Hb1b2_deltaR, "Hb1b2_deltaR");
    float Yb1b2_deltaR = -99.;
    attachToTrees(signalTree, backgroundTree, Yb1b2_deltaR, "Yb1b2_deltaR");

    // b jet variables
    float Hb_highestPt = -1.;
    attachToTrees(signalTree, backgroundTree, Hb_highestPt, "Hb_highestPt");
    float Hb_lowestPt = -1.;
    attachToTrees(signalTree, backgroundTree, Hb_lowestPt, "Hb_lowestPt");
    float Yb_highestPt = -1.;
    attachToTrees(signalTree, backgroundTree, Yb_highestPt, "Yb_highestPt");
    float Yb_lowestPt = -1.;
    attachToTrees(signalTree, backgroundTree, Yb_lowestPt, "Yb_lowestPt");


    float Candidate_Hb1Gen_pt = -1.;
    selectionTree->Branch("Candidate_Hb1Gen_pt", &Candidate_Hb1Gen_pt);
    float Candidate_Hb2Gen_pt = -1.;
    selectionTree->Branch("Candidate_Hb2Gen_pt", &Candidate_Hb2Gen_pt);
    float Candidate_Yb1Gen_pt = -1.;
    selectionTree->Branch("Candidate_Yb1Gen_pt", &Candidate_Yb1Gen_pt);
    float Candidate_Yb2Gen_pt = -1.;
    selectionTree->Branch("Candidate_Yb2Gen_pt", &Candidate_Yb2Gen_pt);

    float Candidate_Hb1Reco_pt = -1.;
    selectionTree->Branch("Candidate_Hb1Reco_pt", &Candidate_Hb1Reco_pt);
    float Candidate_Hb2Reco_pt = -1.;
    selectionTree->Branch("Candidate_Hb2Reco_pt", &Candidate_Hb2Reco_pt);
    float Candidate_Yb1Reco_pt = -1.;
    selectionTree->Branch("Candidate_Yb1Reco_pt", &Candidate_Yb1Reco_pt);
    float Candidate_Yb2Reco_pt = -1.;
    selectionTree->Branch("Candidate_Yb2Reco_pt", &Candidate_Yb2Reco_pt);


    float Candidate_H_mass = -1.;
    selectionTree->Branch("Candidate_H_mass", &Candidate_H_mass);
    float Candidate_Y_mass = -1.;
    selectionTree->Branch("Candidate_Y_mass", &Candidate_Y_mass);
    float Candidate_X_mass = -1.;
    selectionTree->Branch("Candidate_X_mass", &Candidate_X_mass);
    int Candidate_H_b1_genJetId = -1.;
    selectionTree->Branch("Candidate_H_b1_genJetId", &Candidate_H_b1_genJetId);
    int Candidate_H_b2_genJetId = -1.;
    selectionTree->Branch("Candidate_H_b2_genJetId", &Candidate_H_b2_genJetId);
    int Candidate_Y_b1_genJetId = -1.;
    selectionTree->Branch("Candidate_Y_b1_genJetId", &Candidate_Y_b1_genJetId);
    int Candidate_Y_b2_genJetId = -1.;
    selectionTree->Branch("Candidate_Y_b2_genJetId", &Candidate_Y_b2_genJetId);
    int Candidate_numberOfBGen = 0;
    selectionTree->Branch("Candidate_numberOfBGen", &Candidate_numberOfBGen);
    int Candidate_numberOfBGenPassingCuts = 0;
    selectionTree->Branch("Candidate_numberOfBGenPassingCuts", &Candidate_numberOfBGenPassingCuts);
    int Candidate_numberOfHardBGenPassingCuts = 0;
    selectionTree->Branch("Candidate_numberOfHardBGenPassingCuts", &Candidate_numberOfHardBGenPassingCuts);
    int Candidate_numberOfHardBGenPassingCutsMatched = 0;
    selectionTree->Branch("Candidate_numberOfHardBGenPassingCutsMatched", &Candidate_numberOfHardBGenPassingCutsMatched);
    int Candidate_numberOfHardBGenPassingCutsMatchedBtagged = 0;
    selectionTree->Branch("Candidate_numberOfHardBGenPassingCutsMatchedBtagged", &Candidate_numberOfHardBGenPassingCutsMatchedBtagged);
    int Candidate_numberOfBJet = 0;
    selectionTree->Branch("Candidate_numberOfBJet", &Candidate_numberOfBJet);
    bool Candidate_HgenMatched = false;
    selectionTree->Branch("Candidate_HgenMatched", &Candidate_HgenMatched);
    bool Candidate_YgenMatched = false;
    selectionTree->Branch("Candidate_YgenMatched", &Candidate_YgenMatched);
    bool Candidate_genMatched = false;
    selectionTree->Branch("Candidate_genMatched", &Candidate_genMatched);


    std::string yMassSelection = opts["yMassSelection"].as<std::string>();
    if(yMassSelection != "None") nat.attachCustomValueBranch<Bool_t>(yMassSelection);

    int maxEvts = opts["maxEvts"].as<int>();
    if (maxEvts >= 0)
        cout << "[INFO] ... running on : " << maxEvts << " events" << endl;

    for (int iEv = 0; true; ++iEv)
    {
        if (maxEvts >= 0 && iEv >= maxEvts)
            break;

        Candidate_Hb1Gen_pt                                 = -1.;
        Candidate_Hb2Gen_pt                                 = -1.;
        Candidate_Yb1Gen_pt                                 = -1.;
        Candidate_Yb2Gen_pt                                 = -1.;
        Candidate_Hb1Reco_pt                                = -1.;
        Candidate_Hb2Reco_pt                                = -1.;
        Candidate_Yb1Reco_pt                                = -1.;
        Candidate_Yb2Reco_pt                                = -1.;
        Candidate_H_mass                                    = -1.;
        Candidate_Y_mass                                    = -1.;
        Candidate_X_mass                                    = -1.;
        Candidate_numberOfBGen                              =  0 ;
        Candidate_H_b1_genJetId                             = -1.;
        Candidate_H_b2_genJetId                             = -1.;
        Candidate_Y_b1_genJetId                             = -1.;
        Candidate_Y_b2_genJetId                             = -1.;
        Candidate_numberOfBGenPassingCuts                   =  0 ;
        Candidate_numberOfHardBGenPassingCuts               =  0 ;
        Candidate_numberOfHardBGenPassingCutsMatched        =  0 ;
        Candidate_numberOfHardBGenPassingCutsMatchedBtagged =  0 ;
        Candidate_numberOfBJet                              =  0 ;
        Candidate_HgenMatched                               = false;
        Candidate_YgenMatched                               = false;
        Candidate_genMatched                                = false;

        if (!nat.Next()) break;
        if (iEv % 10000 == 0) cout << "... processing event " << iEv << endl;
        // if (iEv % 1 == 0) cout << "... processing event " << iEv << endl;

        //Check for mass selection
        if(yMassSelection != "None") if(!nat.readCustomValueBranch<Bool_t>(yMassSelection)) continue;

        std::vector<std::string> listOfPassedTriggers = nat.getTrgPassed();

        if( listOfPassedTriggers.size() == 0  && triggerVector.size()>0 ) continue;

        if (*(nat.nJet) < 4) continue;

        std::vector<Jet> unsmearedJets;
        unsmearedJets.reserve(*(nat.nJet));

        for (uint ij = 0; ij < *(nat.nJet); ++ij)
        {
            unsmearedJets.emplace_back(Jet(ij, &nat));
        }

        oph.bJets_PreselectionCut(unsmearedJets);

        if(unsmearedJets.size()<4) continue;

        std::stable_sort(unsmearedJets.begin(), unsmearedJets.end(), [](const Jet & a, const Jet & b) -> bool
        {
            return ( a.bTagScore()  > b.bTagScore() );
        });


        std::vector<Jet> smearedJets;
        if(parameterList.find("JetEnergyResolution") != parameterList.end())
        { //is it a MC event
            smearedJets = oph.JERsmearing(nat,unsmearedJets);
        }
        else
        {
            smearedJets = unsmearedJets;
        }
        std::vector<bool> alreadyMatched(smearedJets.size(),false);
        
        std::vector<int> genMatchedH1Jets(2,-1); // (H_b1, H_b2)
        std::vector<int> genMatchedH2Jets(2,-1); // (Y_b1, Y_b2)

        int numberOfYbJets = 0;
        int numberOfHbJets = 0;
        // std::cout<<"Event"<<std::endl;

        for (uint igp = 0; igp < *(nat.nGenPart); ++igp)
        {
            GenPart gp (igp, &nat);
            if (abs(get_property(gp, GenPart_pdgId)) != 5) continue;
            if (!gp.isLastCopy()) continue;

            ++Candidate_numberOfBGen;
            if(gp.P4().Pt() > any_cast<float>(parameterList.at("MinPt")) && abs(gp.P4().Eta()) < any_cast<float>(parameterList.at("MaxAbsEta"))) ++Candidate_numberOfBGenPassingCuts;

            if (!gp.fromHardProcess()) continue;
            // if(gp.P4().Pt() > 50. && abs(gp.P4().Eta()) < 2) ++Candidate_numberOfHardBGenPassingCuts;
            if(gp.P4().Pt() > any_cast<float>(parameterList.at("MinPt")) && abs(gp.P4().Eta()) < any_cast<float>(parameterList.at("MaxAbsEta"))) ++Candidate_numberOfHardBGenPassingCuts;

            // std::cout<<"Part"<<std::endl;

            if (isDerivingFromMotherWithId(gp, 25))
            {
                if(Candidate_Hb1Gen_pt == -1.) Candidate_Hb1Gen_pt = gp.P4().Pt();
                else if(Candidate_Hb2Gen_pt == -1.) Candidate_Hb2Gen_pt = gp.P4().Pt();
                updateMatchingList(genMatchedH1Jets, gp, smearedJets, 0.3, numberOfHbJets, alreadyMatched);
            }
            else if (isDerivingFromMotherWithId(gp, 35))
            {
                if(Candidate_Yb1Gen_pt == -1.) Candidate_Yb1Gen_pt = gp.P4().Pt();
                else if(Candidate_Yb2Gen_pt == -1.) Candidate_Yb2Gen_pt = gp.P4().Pt();
                updateMatchingList(genMatchedH2Jets, gp, smearedJets, 0.3, numberOfYbJets, alreadyMatched);
            }
        }

        if (numberOfHbJets!=2 || numberOfYbJets!=2)
        {
            cout << "** [DEBUG] : select_gen_bb_bb : something went wrong, dumping gen parts" << endl;
            std::cout << "Number of H jets = " << numberOfHbJets << std::endl;
            std::cout << "Number of Y jets = " << numberOfYbJets << std::endl;
            oph.dump_gen_part(nat, true);
            continue;
        }

        std::vector<int> genMatchedJetsCleaned;
        auto removeNotMatched = [&genMatchedJetsCleaned](const std::vector<int> inputList) -> void 
        {
            for(auto i : inputList)
            {
                if(i>=0) genMatchedJetsCleaned.emplace_back(i);
            }
        };
        removeNotMatched(genMatchedH1Jets);
        removeNotMatched(genMatchedH2Jets);
        Candidate_numberOfHardBGenPassingCutsMatched = genMatchedJetsCleaned.size();
    

        std::vector<bool> btagMask(smearedJets.size(),false);
        for(uint jIt=0; jIt<smearedJets.size(); ++jIt)
        {
            if(smearedJets[jIt].bTagScore() >= 0.6324)
            {
                ++Candidate_numberOfBJet;
                btagMask[jIt] = true;

                if(jIt == genMatchedH1Jets[0] || jIt == genMatchedH1Jets[1] || jIt == genMatchedH2Jets[0] || jIt == genMatchedH2Jets[1]) ++Candidate_numberOfHardBGenPassingCutsMatchedBtagged;

            }
        }

        if(!checkUniqueAndFilled(genMatchedJetsCleaned)) std::cout<<"Duplicate!"<<std::endl;
        // ASSERT_WITH_MESSAGE(checkUniqueAndFilled(genMatchedJetsCleaned));

        float mHtarget = 120.;
        float bestDeltaHiggsMass = 9999;
        std::pair<int,int> bestHcandidate{-1,-1};

        std::vector<int> signalList;

        int numberOfJets = unsmearedJets.size();
        int combinations = 0;
        // std::cout<<"New event"<<std::endl;
        // std::cout<<"To be found: " <<genMatchedH1Jets[0]<< " " <<genMatchedH1Jets[1]<< " " <<genMatchedH2Jets[0]<< " " <<genMatchedH2Jets[1]<<std::endl;

        if(Candidate_numberOfBJet>=4)
        {
            for(int h1b1it = 0; h1b1it< numberOfJets-1; ++h1b1it)
            {
                if(!btagMask[h1b1it]) continue;
                bool h1b1Matches = true;
                if(h1b1it != genMatchedH1Jets[0] && h1b1it != genMatchedH1Jets[1]) h1b1Matches = false;
                Jet &Hb1 = smearedJets.at(h1b1it);

                for(int h1b2it = h1b1it+1; h1b2it< numberOfJets; ++h1b2it)
                {
                    if(!btagMask[h1b2it]) continue;
                    bool h1b2Matches = true;
                    if(h1b2it != genMatchedH1Jets[0] && h1b2it != genMatchedH1Jets[1]) h1b2Matches = false;
                    Jet &Hb2 = smearedJets.at(h1b2it);
                    CompositeCandidate H = CompositeCandidate(Hb1, Hb2);
                    H.rebuildP4UsingRegressedPt(true,true);
                    float currentDeltaHiggsMass = abs(mHtarget-H.P4().M());
                    if(currentDeltaHiggsMass < bestDeltaHiggsMass && h1b1it<4 && h1b2it<4)
                    {
                        bestDeltaHiggsMass = currentDeltaHiggsMass;
                        bestHcandidate.first  = h1b1it;
                        bestHcandidate.second = h1b2it;
                    }
                    
                    for(int h2b1it = 0; h2b1it< numberOfJets-1; ++h2b1it)
                    {
                        if(!btagMask[h2b1it]) continue;
                        if(h2b1it == h1b2it || h2b1it == h1b1it) continue;
                        bool h2b1Matches = true;
                        if(h2b1it != genMatchedH2Jets[0] && h2b1it != genMatchedH2Jets[1]) h2b1Matches = false;
                        Jet &Yb1 = smearedJets.at(h2b1it);

                        for(int h2b2it = h2b1it+1; h2b2it< numberOfJets; ++h2b2it)
                        {
                            if(!btagMask[h2b2it]) continue;
                            if(h2b2it == h1b2it || h2b2it == h1b1it) continue;
                            bool h2b2Matches = true;
                            if(h2b2it != genMatchedH2Jets[0] && h2b2it != genMatchedH2Jets[1]) h2b2Matches = false;

                            std::vector<int> listOfJetIndexes = {h1b1it, h1b2it, h2b1it, h2b2it};
                            ASSERT_WITH_MESSAGE(checkUniqueAndFilled(listOfJetIndexes));

                            Jet &Yb2 = smearedJets.at(h2b2it);

                            CompositeCandidate Y = CompositeCandidate(Yb1, Yb2);
                            Y.rebuildP4UsingRegressedPt(true,true);
                            CompositeCandidate X = CompositeCandidate(H, Y);

                            X_pt = X.P4().Pt();
                            HY_deltaEta = abs(H.P4().Eta() - Y.P4().Eta());
                            HY_deltaPhi =  abs(OfflineProducerHelper::deltaPhi(H.P4().Phi(),Y.P4().Phi()));

                            H_mass = H.P4().M();
                            Y_mass = Y.P4().M();

                            Hb1b2_deltaR = sqrt(OfflineProducerHelper::deltaPhi(Hb1.P4().Phi(),Hb2.P4().Phi())*OfflineProducerHelper::deltaPhi(Hb1.P4().Phi(),Hb2.P4().Phi()) + (Hb1.P4().Phi()-Hb2.P4().Phi())*(Hb1.P4().Phi()-Hb2.P4().Phi()));
                            Yb1b2_deltaR = sqrt(OfflineProducerHelper::deltaPhi(Yb1.P4().Phi(),Yb2.P4().Phi())*OfflineProducerHelper::deltaPhi(Yb1.P4().Phi(),Yb2.P4().Phi()) + (Yb1.P4().Phi()-Yb2.P4().Phi())*(Yb1.P4().Phi()-Yb2.P4().Phi()));

                            Hb_highestPt = std::max(Hb1.P4Regressed().Pt(),Hb2.P4Regressed().Pt());
                            Yb_highestPt = std::max(Yb1.P4Regressed().Pt(),Yb2.P4Regressed().Pt());

                            Hb_lowestPt = std::min(Hb1.P4Regressed().Pt(),Hb2.P4Regressed().Pt());
                            Yb_lowestPt = std::min(Yb1.P4Regressed().Pt(),Yb2.P4Regressed().Pt());
                            // std::cout<<h1b1it<< " " <<h1b2it<< " " <<h2b1it<< " " <<h2b2it<<std::endl;

                            if(h1b1Matches && h1b2Matches && h2b1Matches && h2b2Matches)
                            {
                                //save in signal tree
                                signalTree->Fill();
                                signalList = {h1b1it,h1b2it,h2b1it,h2b2it};
                                std::sort(signalList.begin(),signalList.end());
                            }
                            else
                            {
                                //save in background tree
                                backgroundTree->Fill();
                            }
                            combinations++;
                            
                        }
                    }
                }
            }


            std::function<int (int)> factorial = [&factorial] (int i) 
            { 
                return (i == 1 || i == 0) ? 1 : i * factorial(i - 1); 
            };
            if(combinations != 6*factorial(Candidate_numberOfBJet)/(factorial(4)*factorial(Candidate_numberOfBJet-4)))
            {
                std::cout<<"Number of possible jets = " << Candidate_numberOfBJet << std::endl;
                std::cout<<"Number of possible combinations = " << combinations << std::endl;
            }

            

            std::vector<int> numberList = {0, 1, 2, 3};
            numberList.erase(std::remove(numberList.begin(), numberList.end(), bestHcandidate.first ), numberList.end());
            numberList.erase(std::remove(numberList.begin(), numberList.end(), bestHcandidate.second), numberList.end());
            assert(numberList.size()==2);
            std::pair<int,int> bestYcandidate = {numberList[0], numberList[1]};

            std::vector<int> listOfCandidateIndexes = {bestHcandidate.first, bestHcandidate.second, bestYcandidate.first, bestYcandidate.second};
            ASSERT_WITH_MESSAGE(checkUniqueAndFilled(listOfCandidateIndexes));


            Candidate_Hb1Reco_pt = smearedJets.at(bestHcandidate.first ).P4().Pt();
            Candidate_Hb2Reco_pt = smearedJets.at(bestHcandidate.second).P4().Pt();
            Candidate_Yb1Reco_pt = smearedJets.at(bestYcandidate.first ).P4().Pt();
            Candidate_Yb2Reco_pt = smearedJets.at(bestYcandidate.second).P4().Pt();

            CompositeCandidate H = CompositeCandidate(smearedJets.at(bestHcandidate.first), smearedJets.at(bestHcandidate.second));
            H.rebuildP4UsingRegressedPt(true,true);
            CompositeCandidate Y = CompositeCandidate(smearedJets.at(bestYcandidate.first), smearedJets.at(bestYcandidate.second));
            Y.rebuildP4UsingRegressedPt(true,true);
            Candidate_H_mass = H.P4().M();
            Candidate_Y_mass = Y.P4().M();
            CompositeCandidate X = CompositeCandidate(H, Y);
            Candidate_X_mass = X.P4().M();

            Candidate_H_b1_genJetId = get_property(smearedJets.at(bestHcandidate.first ),Jet_genJetIdx);
            if(Candidate_H_b1_genJetId>=0) if(get_property(GenJet(Candidate_H_b1_genJetId, &nat),GenJet_partonFlavour)!=5) Candidate_H_b1_genJetId = -1;
            Candidate_H_b2_genJetId = get_property(smearedJets.at(bestHcandidate.second),Jet_genJetIdx);
            if(Candidate_H_b2_genJetId>=0) if(get_property(GenJet(Candidate_H_b2_genJetId, &nat),GenJet_partonFlavour)!=5) Candidate_H_b2_genJetId = -1;
            Candidate_Y_b1_genJetId = get_property(smearedJets.at(bestYcandidate.first ),Jet_genJetIdx);
            if(Candidate_Y_b1_genJetId>=0) if(get_property(GenJet(Candidate_Y_b1_genJetId, &nat),GenJet_partonFlavour)!=5) Candidate_Y_b1_genJetId = -1;
            Candidate_Y_b2_genJetId = get_property(smearedJets.at(bestYcandidate.second),Jet_genJetIdx);
            if(Candidate_Y_b2_genJetId>=0) if(get_property(GenJet(Candidate_Y_b2_genJetId, &nat),GenJet_partonFlavour)!=5) Candidate_Y_b2_genJetId = -1;

            Candidate_genMatched = true;
            Candidate_HgenMatched = true;
            Candidate_YgenMatched = true;
            
            if(bestHcandidate.first  != genMatchedH1Jets[0] && bestHcandidate.first  != genMatchedH1Jets[1])
            {
                Candidate_genMatched  = false;
                Candidate_HgenMatched = false;

            }
            if(bestHcandidate.second != genMatchedH1Jets[0] && bestHcandidate.second != genMatchedH1Jets[1])
            {
                Candidate_genMatched = false;
                Candidate_HgenMatched = false;
            } 
            if(bestYcandidate.first  != genMatchedH2Jets[0] && bestYcandidate.first  != genMatchedH2Jets[1])
            {
                Candidate_genMatched = false;
                Candidate_YgenMatched = false;
            } 
            if(bestYcandidate.second != genMatchedH2Jets[0] && bestYcandidate.second != genMatchedH2Jets[1])
            {
                Candidate_genMatched = false;
                Candidate_YgenMatched = false;
            } 

            if(!Candidate_genMatched && debug)
            {
                signalList = {genMatchedH1Jets[0],genMatchedH1Jets[1],genMatchedH2Jets[0],genMatchedH2Jets[1]};
                std::sort(signalList.begin(),signalList.end());
                std::sort(listOfCandidateIndexes.begin(),listOfCandidateIndexes.end());
                if(signalList != listOfCandidateIndexes)
                {
                    dumpList(signalList);
                    dumpList(listOfCandidateIndexes);
                }
            }
        }
        selectionTree->Fill();
    }

    outputFile.cd();
    signalTree    ->Write();
    backgroundTree->Write();
    selectionTree ->Write();

}


