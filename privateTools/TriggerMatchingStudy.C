#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TROOT.h"

void TriggerMatchingStudyOlder(std::string inputFileName)
{

    TFile inputFile(inputFileName.data());

    TTree *tree = (TTree*)inputFile.Get("bbbbTree");

    int entries = tree->Draw("TriggerObjectMatched","","goff");
    std::cout<<entries<<"\t";
    int entriesGen = tree->Draw("TriggerObjectMatched","gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<entriesGen<<" ("<<(float)entriesGen/(float)entries*100.<<"%)"<<"\t";
    int entriesTrg = tree->Draw("TriggerObjectMatched","TriggerObjectMatched==1","goff");
    std::cout<<entriesTrg<<"\t";
    int entriesTrgGen = tree->Draw("TriggerObjectMatched","gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && TriggerObjectMatched==1","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<entriesTrgGen<<" ("<<(float)entriesTrgGen/(float)entriesTrg*100.<<"%)"<<"\t";
    int entriesCut = tree->Draw("TriggerObjectMatched","H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<entriesCut<<"\t";
    int entriesCutGen = tree->Draw("TriggerObjectMatched", "gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<entriesCutGen<<" ("<<(float)entriesCutGen/(float)entriesCut*100.<<"%)"<<"\t";
    int entriesCutTrg = tree->Draw("TriggerObjectMatched","H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && TriggerObjectMatched==1","goff");
    std::cout<<entriesCutTrg<<" ("<<(float)entriesCutTrg/(float)entriesCut*100.<<"%)"<<"\t";
    int entriesCutTrgGen = tree->Draw("TriggerObjectMatched","gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && TriggerObjectMatched==1","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<entriesCutTrgGen<<" ("<<(float)entriesCutTrgGen/(float)entriesCutTrg*100.<<"%)"<<"\n";

}


void TriggerMatchingStudyOld(std::string inputFileName)
{

    TFile inputFile(inputFileName.data());

    TTree *tree = (TTree*)inputFile.Get("bbbbTree");

    int trg1Entries = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1","goff");
    std::cout<<trg1Entries<<"\t";
    int trg1EntriesGen = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg1EntriesGen<<" ("<<(float)trg1EntriesGen/(float)trg1Entries*100.<<"%)"<<"\t";
    int trg1EntriesTrg = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<trg1EntriesTrg<<"\t";
    int trg1EntriesTrgGen = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg1EntriesTrgGen<<" ("<<(float)trg1EntriesTrgGen/(float)trg1EntriesTrg*100.<<"%)"<<"\t";
    int trg1EntriesCut = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<trg1EntriesCut<<"\t";
    int trg1EntriesCutGen = tree->Draw("Event", "HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg1EntriesCutGen<<" ("<<(float)trg1EntriesCutGen/(float)trg1EntriesCut*100.<<"%)"<<"\t";
    int trg1EntriesCutTrg = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<trg1EntriesCutTrg<<" ("<<(float)trg1EntriesCutTrg/(float)trg1EntriesCut*100.<<"%)"<<"\t";
    int trg1EntriesCutTrgGen = tree->Draw("Event","HLT_DoubleJet90_Double30_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg1EntriesCutTrgGen<<" ("<<(float)trg1EntriesCutTrgGen/(float)trg1EntriesCutTrg*100.<<"%)"<<"\n";


    int trg2Entries = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1","goff");
    std::cout<<trg2Entries<<"\t";
    int trg2EntriesGen = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg2EntriesGen<<" ("<<(float)trg2EntriesGen/(float)trg2Entries*100.<<"%)"<<"\t";
    int trg2EntriesTrg = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<trg2EntriesTrg<<"\t";
    int trg2EntriesTrgGen = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg2EntriesTrgGen<<" ("<<(float)trg2EntriesTrgGen/(float)trg2EntriesTrg*100.<<"%)"<<"\t";
    int trg2EntriesCut = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<trg2EntriesCut<<"\t";
    int trg2EntriesCutGen = tree->Draw("Event", "gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg2EntriesCutGen<<" ("<<(float)trg2EntriesCutGen/(float)trg2EntriesCut*100.<<"%)"<<"\t";
    int trg2EntriesCutTrg = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<trg2EntriesCutTrg<<" ("<<(float)trg2EntriesCutTrg/(float)trg2EntriesCut*100.<<"%)"<<"\t";
    int trg2EntriesCutTrgGen = tree->Draw("Event","HLT_QuadJet45_TripleBTagCSV_p087_Fired==1 && gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<trg2EntriesCutTrgGen<<" ("<<(float)trg2EntriesCutTrgGen/(float)trg2EntriesCutTrg*100.<<"%)"<<"\n";


    int orEntries = tree->Draw("Event","","goff");
    std::cout<<orEntries<<"\t";
    int orEntriesGen = tree->Draw("Event","gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<orEntriesGen<<" ("<<(float)orEntriesGen/(float)orEntries*100.<<"%)"<<"\t";
    int orEntriesTrg = tree->Draw("Event","(HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1 || HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1)","goff");
    std::cout<<orEntriesTrg<<"\t";
    int orEntriesTrgGen = tree->Draw("Event","gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && (HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1 || HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1)","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<orEntriesTrgGen<<" ("<<(float)orEntriesTrgGen/(float)orEntriesTrg*100.<<"%)"<<"\t";
    int orEntriesCut = tree->Draw("Event","H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<orEntriesCut<<"\t";
    int orEntriesCutGen = tree->Draw("Event", "gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<orEntriesCutGen<<" ("<<(float)orEntriesCutGen/(float)orEntriesCut*100.<<"%)"<<"\t";
    int orEntriesCutTrg = tree->Draw("Event","H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && (HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1 || HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1)","goff");
    std::cout<<orEntriesCutTrg<<" ("<<(float)orEntriesCutTrg/(float)orEntriesCut*100.<<"%)"<<"\t";
    int orEntriesCutTrgGen = tree->Draw("Event","gen_H1_b1_matchedflag>=0 && gen_H1_b2_matchedflag>=0 && gen_H2_b1_matchedflag>=0 && gen_H2_b2_matchedflag>=0 && H1_b1_pt > 30 && H1_b2_pt > 30 && H2_b1_pt > 30 && H2_b2_pt > 30 && abs(H1_b1_eta)<2.4 && abs(H1_b2_eta)<2.4 && abs(H2_b1_eta)<2.4 && abs(H2_b2_eta)<2.4 && H1_b1_ptRegressed > 30 && H1_b2_ptRegressed > 30 && H2_b1_ptRegressed > 30 && H2_b2_ptRegressed > 30 && (HLT_DoubleJet90_Double30_TripleBTagCSV_p087_ObjectMatched==1 || HLT_QuadJet45_TripleBTagCSV_p087_ObjectMatched==1)","goff");
    std::cout<<std::fixed<<std::setprecision(1)<<orEntriesCutTrgGen<<" ("<<(float)orEntriesCutTrgGen/(float)orEntriesCutTrg*100.<<"%)"<<"\n";

}


void TriggerMatchingStudyPerDataset(std::string fileName, std::string dataset)
{

    TFile inputFile(fileName.data());
    TH1F *hist;
    std::string selection;
    std::string region;
    std::string plot = "NpreCutJets";
    std::map<std::string, float> theEntriesMap;

    float generatedEvents = ((TH1F*)inputFile.Get( (dataset + "/" + dataset).data()))->GetBinContent(1);
    float triggeredEvents = ((TH1F*)inputFile.Get( (dataset + "/" + dataset).data()))->GetBinContent(2);

    std::cout<<dataset<<std::endl;
    
    // {
    //     std::cout<<generatedEvents<<"\t";

    //     selection = "HLTDouble";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100. << "\%)"<<"\t";

    //     selection = "HLTDouble";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTDouble_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTDoubleFiredTrgMatched";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";

    //     selection = "HLTDoubleFiredTrgMatched";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTDoubleFiredTrgMatched_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTDoubleFiredPtAndEtaBaseCuts";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";

    //     selection = "HLTDoubleFiredPtAndEtaBaseCuts";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTDoubleFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTDoubleFiredPtAndEtaBaseCutsTrgMatched";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTDoubleFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTDoubleFiredPtAndEtaBaseCutsTrgMatched";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTDoubleFiredPtAndEtaBaseCutsTrgMatched_Full"]*100. << "\%)"<<"\t";

    //     std::cout<<std::endl;
    // }

    // {
    //     std::cout<<generatedEvents<<"\t";

    //     selection = "HLTQuad";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100. << "\%)"<<"\t";

    //     selection = "HLTQuad";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTQuad_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTQuadFiredTrgMatched";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";

    //     selection = "HLTQuadFiredTrgMatched";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTQuadFiredTrgMatched_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTQuadFiredPtAndEtaBaseCuts";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";

    //     selection = "HLTQuadFiredPtAndEtaBaseCuts";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTQuadFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTQuadFiredPtAndEtaBaseCutsTrgMatched";
    //     region = "Full";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTQuadFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";

    //     selection = "HLTQuadFiredPtAndEtaBaseCutsTrgMatched";
    //     region = "AllBjetsGenMatched";
    //     hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    //     theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    //     std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTQuadFiredPtAndEtaBaseCutsTrgMatched_Full"]*100. << "\%)"<<"\t";

    //     std::cout<<std::endl;
    // }

    std::vector<std::string> binNames = {"Triggered", "Selected", "Trigger matched", "Signal region"}; 
    int binNameSize = binNames.size();
    TH1D *cutFlowPlot = new TH1D ( (dataset + "_Cut_flow").data(), (dataset + " Cut flow").data(), binNameSize, 0, binNameSize);
    TH1D *purityPlot = new TH1D ( (dataset + "_Purity").data(), (dataset + " Purity").data(), binNameSize, 0, binNameSize);

    cutFlowPlot->SetDirectory(0);
    purityPlot->SetDirectory(0);
    
    for(int bin=0; bin<binNameSize; ++bin)
    {
        cutFlowPlot->GetXaxis()->SetBinLabel(bin+1, binNames[bin].data() );
        purityPlot ->GetXaxis()->SetBinLabel(bin+1, binNames[bin].data() );
    }

    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Triggered"),triggeredEvents/generatedEvents*100.);
     
    // float genMatchedAverageNbjets;
    // float genUnMatchedAverageNbjets;
    std::cout<<generatedEvents<<"\t";

    selection = "HLTOr";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100. << "\%)"<<"\t";
    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Selected"),theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100.);

    selection = "HLTOr";
    region = "AllBjetsGenMatched";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOr_Full"]*100. << "\%)"<<"\t";
    purityPlot->SetBinContent(purityPlot->GetXaxis()->FindBin("Selected"),theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOr_Full"]*100.);

    selection = "HLTOrFiredTrgMatched";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";
    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Trigger matched"),theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100.);

    selection = "HLTOrFiredTrgMatched";
    region = "AllBjetsGenMatched";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredTrgMatched_Full"]*100. << "\%)"<<"\t";
    purityPlot->SetBinContent(purityPlot->GetXaxis()->FindBin("Trigger matched"),theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredTrgMatched_Full"]*100.);

    selection = "HLTOrFiredPtAndEtaBaseCuts";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";

    selection = "HLTOrFiredPtAndEtaBaseCuts";
    region = "AllBjetsGenMatched";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";
    // genMatchedAverageNbjets = hist->GetMean();

    selection = "HLTOrFiredPtAndEtaBaseCuts";
    region = "NotAllBjetsGenMatched";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    // genUnMatchedAverageNbjets = hist->GetMean();

    selection = "HLTOrFiredPtAndEtaBaseCutsTrgMatched";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";
    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Signal region"),theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100.);

    selection = "HLTOrFiredPtAndEtaBaseCutsTrgMatched";
    region = "AllBjetsGenMatched";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredPtAndEtaBaseCutsTrgMatched_Full"]*100. << "\%)"<<"\t";
    purityPlot->SetBinContent(purityPlot->GetXaxis()->FindBin("Signal region"),theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredPtAndEtaBaseCutsTrgMatched_Full"]*100.);

    std::cout<<std::endl;

    gStyle->SetPaintTextFormat("  4.1f");
    // purityPlot->Draw("pl");
    // purityPlot->SetLineColor(kRed);
    TCanvas *c1 = new TCanvas("","",840,510);
    cutFlowPlot->SetStats(0);
    cutFlowPlot->SetMarkerStyle(7);
    cutFlowPlot->SetMarkerSize(2.0);
    cutFlowPlot->GetXaxis()->SetLabelFont(62);
    cutFlowPlot->GetXaxis()->SetLabelSize(0.05);
    cutFlowPlot->GetYaxis()->SetLabelFont(62);
    cutFlowPlot->GetYaxis()->SetTitleFont(62);
    cutFlowPlot->Draw("pl text");
    cutFlowPlot->SetMaximum(80);
    cutFlowPlot->SetMinimum(0);
    cutFlowPlot->GetYaxis()->SetTitle("Efficiency [%]");

    c1->Modified();
    c1->Update();
    // purityPlot->Draw("pl same");

    // std::cout << genMatchedAverageNbjets << " " << genUnMatchedAverageNbjets << std::endl;

    //scale hint1 to the pad coordinates
    Float_t rightmax = 80;
    Float_t scale = gPad->GetUymax()/rightmax;

    //draw an axis on the right side
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(),gPad->GetUxmax(), gPad->GetUymax(),0,rightmax,510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->SetLabelFont(62);
    axis->SetTextFont(62);
    axis->SetTitle("Purity against combinatorial BKG [%]");
    axis->SetTitleColor(kRed);
    axis->Draw();
    c1->Modified();
    c1->Update();
    
    purityPlot->SetStats(0);
    purityPlot->SetLineWidth(0);
    purityPlot->SetMarkerStyle(7);
    purityPlot->SetMarkerSize(2.0);
    purityPlot->SetMarkerColor(kRed);
    purityPlot->SetMaximum(rightmax);
    purityPlot->SetMinimum(0);
    purityPlot->Draw("pl text same");
    purityPlot->Scale(scale);
    c1->Modified();
    c1->Update();

    //Adding stupid lines
    TLine *lSelToTrig = new TLine(
        purityPlot->GetBinCenter(cutFlowPlot->GetXaxis()->FindBin("Selected")),
        purityPlot->GetBinContent(cutFlowPlot->GetXaxis()->FindBin("Selected")),
        purityPlot->GetBinCenter(cutFlowPlot->GetXaxis()->FindBin("Trigger matched")),
        purityPlot->GetBinContent(cutFlowPlot->GetXaxis()->FindBin("Trigger matched")));
    lSelToTrig->SetLineColor(kRed);
    lSelToTrig->Draw("same");

    TLine *lTrigToSig = new TLine(
        purityPlot->GetBinCenter(cutFlowPlot->GetXaxis()->FindBin("Trigger matched")),
        purityPlot->GetBinContent(cutFlowPlot->GetXaxis()->FindBin("Trigger matched")),
        purityPlot->GetBinCenter(cutFlowPlot->GetXaxis()->FindBin("Signal region")),
        purityPlot->GetBinContent(cutFlowPlot->GetXaxis()->FindBin("Signal region")));
    lTrigToSig->SetLineColor(kRed);
    lTrigToSig->Draw("same");

    inputFile.Close();

    c1->SaveAs((dataset + ".pdf").data());
    delete c1;
    return;

}


void DoAllTriggerMatchingStudy()
{
    // TriggerMatchingStudyOld("test_BTagCSV_trg1_noCut.root");
    // TriggerMatchingStudyOld("test_BTagCSV_trg2_noCut.root");
    // TriggerMatchingStudyOld("test_BTagCSV_OR_noCut.root");
    // std::cout<<std::endl;
    // TriggerMatchingStudyOld("test_BTagCSV_trg1_cut.root");
    // TriggerMatchingStudyOld("test_BTagCSV_trg2_cut.root");
    // TriggerMatchingStudyOld("test_BTagCSV_OR_cut.root");
    
    // TriggerMatchingStudy("test_NMSSM_XYH_bbbb_MC.root");
    gROOT->SetBatch();

    std::string fileName = "2016DataPlots_NMSSM_XYH_bbbb_MCstudies_SF/outPlotter.root";

    std::cout << std::fixed << std::setprecision(2) << "Gen\tSelected\tSelected+Gen\tTrgMatch\tTrgMatch+Gen\tCut\tCut+Gen\tCut+TrgMatch\tCut+TrgMatch+Gen"<<std::endl;

    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_300_MY_60"   );
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_400_MY_100"  );
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_700_MY_300"  );
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_900_MY_200"  );
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_1000_MY_700" );
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_1600_MY_700" );
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200");
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200");
    TriggerMatchingStudyPerDataset(fileName,"sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800");


}
    // TriggerMatchingStudyPerDataset("2016DataPlots_NMSSM_XYH_bbbb_MCstudies/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_300_MY_60"   );



void TriggerMatchingStudyPerData(std::string fileName, std::string dataset)
{
    TFile inputFile(fileName.data());
    TH1F *hist;
    std::string selection;
    std::string region;
    std::string plot = "HH_m";
    std::map<std::string, float> theEntriesMap;
    float generatedEvents = ((TH1F*)inputFile.Get( (dataset + "/" + dataset).data()))->GetBinContent(1);
    float triggeredEvents = ((TH1F*)inputFile.Get( (dataset + "/" + dataset).data()))->GetBinContent(2);
    std::cout<<dataset<<std::endl;
    std::vector<std::string> binNames = {"Triggered", "Selected", "Trigger matched", "Signal region"}; 
    int binNameSize = binNames.size();
    TH1D *cutFlowPlot = new TH1D ( (dataset + "_Cut_flow").data(), (dataset + " Cut flow").data(), binNameSize, 0, binNameSize);
    cutFlowPlot->SetDirectory(0);
    for(int bin=0; bin<binNameSize; ++bin)
    {
        cutFlowPlot->GetXaxis()->SetBinLabel(bin+1, binNames[bin].data() );
    }
    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Triggered"),triggeredEvents/generatedEvents*100.);
    // float genMatchedAverageNbjets;
    // float genUnMatchedAverageNbjets;
    std::cout<<generatedEvents<<"\t";
    selection = "HLTOr";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100. << "\%)"<<"\t";
    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Selected"),theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100.);
    selection = "HLTOrFiredTrgMatched";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ]<<"\t";
    cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Trigger matched"),theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100.);

    selection = "HLTOrFiredPtAndEtaBaseCutsTrgMatched";
    region = "Full";
    hist = (TH1F*)inputFile.Get( (dataset + "/" + selection + "_" + region + "/" + dataset + "_" + selection + "_" + region + "_" + plot).data() );
    theEntriesMap[ (selection + "_" + region) ] = hist->Integral();
    std::cout<<theEntriesMap[ (selection + "_" + region) ] << "~(" << theEntriesMap[ (selection + "_" + region) ]/theEntriesMap["HLTOrFiredPtAndEtaBaseCuts_Full"]*100. << "\%)"<<"\t";
    // cutFlowPlot->SetBinContent(cutFlowPlot->GetXaxis()->FindBin("Signal region"),theEntriesMap[ (selection + "_" + region) ]/generatedEvents*100.);
    std::cout<<std::endl;
    gStyle->SetPaintTextFormat("  4.3f");
    // purityPlot->Draw("pl");
    // purityPlot->SetLineColor(kRed);
    TCanvas *c1 = new TCanvas("","",840,510);
    cutFlowPlot->SetStats(0);
    cutFlowPlot->SetMarkerStyle(7);
    cutFlowPlot->SetMarkerSize(2.0);
    cutFlowPlot->GetXaxis()->SetLabelFont(62);
    cutFlowPlot->GetXaxis()->SetLabelSize(0.05);
    cutFlowPlot->GetYaxis()->SetLabelFont(62);
    cutFlowPlot->GetYaxis()->SetTitleFont(62);
    cutFlowPlot->Draw("pl text");
    cutFlowPlot->SetMaximum(35);
    cutFlowPlot->SetMinimum(0);
    cutFlowPlot->GetYaxis()->SetTitle("Efficiency [%]");
    // c1->Modified();
    // c1->Update();
    // purityPlot->Draw("pl same");

    // std::cout << genMatchedAverageNbjets << " " << genUnMatchedAverageNbjets << std::endl;

    //scale hint1 to the pad coordinates
    Float_t rightmax = 80;
    Float_t scale = gPad->GetUymax()/rightmax;
    inputFile.Close();
    c1->SaveAs((dataset + ".pdf").data());
    delete c1;
    return;

}
