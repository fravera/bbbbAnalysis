#include "Riostream.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TROOT.h"

void OverlapTrees(std::string signalFileName, std::string signalTreeName, std::string backgroundFileName, std::string backgroundTreeName, std::string outputFileName)
{
    gROOT->SetBatch(true);

    TFile *signalFile = new TFile(signalFileName.data());
    TFile *backgroundFile;
    if(signalFileName == backgroundFileName) backgroundFile = signalFile;
    else backgroundFile = new TFile(backgroundFileName.data());

    TFile outputFile(outputFileName.data(),"RECREATE");

    TTree *signalTree     = (TTree*)signalFile    ->Get(signalTreeName    .data());
    TTree *backgroundTree = (TTree*)backgroundFile->Get(backgroundTreeName.data());

    for(const auto branch : *signalTree->GetListOfBranches())
    {
        std::string branchName = branch->GetName();

        signalTree    ->Draw(Form("%s>>signalPlot"    ,branchName.data()));
        backgroundTree->Draw(Form("%s>>backgroundPlot",branchName.data()));

        TH1F* signalPlot     = (TH1F*)gDirectory->Get("signalPlot"    );
        signalPlot    ->SetDirectory(0);
        TH1F* backgroundPlot = (TH1F*)gDirectory->Get("backgroundPlot");
        backgroundPlot->SetDirectory(0);

        TCanvas theCanvas(branchName.data(),branchName.data());
        backgroundPlot->Scale(signalPlot->Integral("width")/backgroundPlot->Integral("width"));
        backgroundPlot->SetLineColor(kBlue);
        backgroundPlot->Draw();
        signalPlot    ->SetLineColor(kRed);
        signalPlot    ->Draw("same");
        backgroundPlot->GetYaxis()->SetRangeUser(0., 1.05*std::max(signalPlot->GetMaximum(),backgroundPlot->GetMaximum()));
        outputFile.WriteObject(&theCanvas,branchName.data());
    }

    signalFile->Close();
    delete signalFile;
    if(signalFileName != backgroundFileName)
    {
        backgroundFile->Close();
        delete backgroundFile;
    }
    outputFile.Close();

    gROOT->SetBatch(false);

    return;

}