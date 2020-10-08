#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TChain.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TF1.h"
#include "TTreeReaderArray.h"
#include "TTreeReader.h"
#include "TMath.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TObjArray.h"
#include "TStyle.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>

#define numberOfThreads 6

/*
cd scripts && g++  -std=c++17 -I `root-config --incdir` -o ResolutionsForKinFit ResolutionsForKinFit.cc `root-config --libs` -O3 -g; cd ..
*/

inline float deltaPhi(float phi1, float phi2)
{
    float highPhi = phi1;
    float lowPhi  = phi2;
    
    float normalDiff = highPhi - lowPhi;
    float piDiff     = highPhi - lowPhi - TMath::Pi();
    if(abs(normalDiff) < abs(piDiff)) return normalDiff;
    else return piDiff;
}


struct ResolutionContainer
{
    ResolutionContainer(int threadNumber = -1)
    : fThreadNumber (threadNumber)
    {
        std::string append = "";
        if(threadNumber>=0) append = "_" + std::to_string(threadNumber);
        fResolutionPt_barrel  = new TH2F(("resolutionPt_barrel"  + append).data(),"resolutionPt_barrel; reco p_{T} [GeV]; reco p_{T} - gen p_{T} [GeV]" ,100 ,20., 1000., 100, -600., 1000.);
        fResolutionPt_barrel ->SetDirectory(0);
        fResolutionPt_barrel ->GetYaxis()->SetTitleOffset(1.3);
        fResolutionEta_barrel = new TH2F(("resolutionEta_barrel" + append).data(),"resolutionEta_barrel; reco p_{T} [GeV]; reco #eta - gen #eta",100 ,20., 1000., 100, -0.4, 0.4);
        fResolutionEta_barrel->SetDirectory(0);
        fResolutionEta_barrel->GetYaxis()->SetTitleOffset(1.3);
        fResolutionPhi_barrel = new TH2F(("resolutionPhi_barrel" + append).data(),"resolutionPhi_barrel; reco p_{T} [GeV]; reco #phi - gen #phi",100 ,20., 1000., 100, -0.4, 0.4);
        fResolutionPhi_barrel->SetDirectory(0);
        fResolutionPhi_barrel->GetYaxis()->SetTitleOffset(1.3);
        fResolutionPt_endcap  = new TH2F(("resolutionPt_endcap"  + append).data(),"resolutionPt_endcap; reco p_{T} [GeV]; reco p_{T} - gen p_{T} [GeV]" ,100 ,20., 1000., 100, -600., 600.);
        fResolutionPt_endcap ->SetDirectory(0);
        fResolutionPt_endcap ->GetYaxis()->SetTitleOffset(1.3);
        fResolutionEta_endcap = new TH2F(("resolutionEta_endcap" + append).data(),"resolutionEta_endcap; reco p_{T} [GeV]; reco #eta - gen #eta",100 ,20., 1000., 100, -0.4, 0.4);
        fResolutionEta_endcap->SetDirectory(0);
        fResolutionEta_endcap->GetYaxis()->SetTitleOffset(1.3);
        fResolutionPhi_endcap = new TH2F(("resolutionPhi_endcap" + append).data(),"resolutionPhi_endcap; reco p_{T} [GeV]; reco #phi - gen #phi",100 ,20., 1000., 100, -0.4, 0.4);
        fResolutionPhi_endcap->SetDirectory(0);
        fResolutionPhi_endcap->GetYaxis()->SetTitleOffset(1.3);

        fResolutionPt_vsPu  = new TH2F(("resolutionPt_vsPu"  + append).data(),"resolutionPt_vsPu; pileUp; reco p_{T} - gen p_{T} [GeV]" ,100 ,0., 100., 100, -600., 1000.);
        fResolutionPt_vsPu ->SetDirectory(0);
        fResolutionPt_vsPu ->GetYaxis()->SetTitleOffset(1.3);
    }

    ResolutionContainer(const ResolutionContainer&) = delete;

    ResolutionContainer(ResolutionContainer&& theOtherContainer)
    {
        fThreadNumber          = theOtherContainer.fThreadNumber         ;
        fResolutionPt_barrel   = theOtherContainer.fResolutionPt_barrel  ;
        fResolutionEta_barrel  = theOtherContainer.fResolutionEta_barrel ;
        fResolutionPhi_barrel  = theOtherContainer.fResolutionPhi_barrel ;
        fResolutionPt_endcap   = theOtherContainer.fResolutionPt_endcap  ;
        fResolutionEta_endcap  = theOtherContainer.fResolutionEta_endcap ;
        fResolutionPhi_endcap  = theOtherContainer.fResolutionPhi_endcap ;
        fResolutionPt_vsPu     = theOtherContainer.fResolutionPt_vsPu    ;
        fPileUp                = theOtherContainer.fPileUp               ;
        fGen_H1_b1_matchedflag = theOtherContainer.fGen_H1_b1_matchedflag;
        fGen_H1_b2_matchedflag = theOtherContainer.fGen_H1_b2_matchedflag;
        fNbJets                = theOtherContainer.fNbJets               ;
        fGen_H1_b1_pt          = theOtherContainer.fGen_H1_b1_pt         ;
        fGen_H1_b2_pt          = theOtherContainer.fGen_H1_b2_pt         ;
        fGen_H1_b1_eta         = theOtherContainer.fGen_H1_b1_eta        ;
        fGen_H1_b2_eta         = theOtherContainer.fGen_H1_b2_eta        ;
        fGen_H1_b1_phi         = theOtherContainer.fGen_H1_b1_phi        ;
        fGen_H1_b2_phi         = theOtherContainer.fGen_H1_b2_phi        ;
        fH1_b1_pt              = theOtherContainer.fH1_b1_pt             ;
        fH1_b2_pt              = theOtherContainer.fH1_b2_pt             ;
        fH1_b1_eta             = theOtherContainer.fH1_b1_eta            ;
        fH1_b2_eta             = theOtherContainer.fH1_b2_eta            ;
        fH1_b1_phi             = theOtherContainer.fH1_b1_phi            ;
        fH1_b2_phi             = theOtherContainer.fH1_b2_phi            ;
  
        theOtherContainer.fThreadNumber          = -1     ;
        theOtherContainer.fResolutionPt_barrel   = nullptr;
        theOtherContainer.fResolutionEta_barrel  = nullptr;
        theOtherContainer.fResolutionPhi_barrel  = nullptr;
        theOtherContainer.fResolutionPt_vsPu   = nullptr;
        theOtherContainer.fResolutionPt_endcap   = nullptr;
        theOtherContainer.fResolutionEta_endcap  = nullptr;
        theOtherContainer.fResolutionPhi_endcap  = nullptr;
        theOtherContainer.fPileUp                = nullptr;
        theOtherContainer.fGen_H1_b1_matchedflag = nullptr;
        theOtherContainer.fGen_H1_b2_matchedflag = nullptr;
        theOtherContainer.fNbJets                = nullptr;
        theOtherContainer.fGen_H1_b1_pt          = nullptr;
        theOtherContainer.fGen_H1_b2_pt          = nullptr;
        theOtherContainer.fGen_H1_b1_eta         = nullptr;
        theOtherContainer.fGen_H1_b2_eta         = nullptr;
        theOtherContainer.fGen_H1_b1_phi         = nullptr;
        theOtherContainer.fGen_H1_b2_phi         = nullptr;
        theOtherContainer.fH1_b1_pt              = nullptr;
        theOtherContainer.fH1_b2_pt              = nullptr;
        theOtherContainer.fH1_b1_eta             = nullptr;
        theOtherContainer.fH1_b2_eta             = nullptr;
        theOtherContainer.fH1_b1_phi             = nullptr;
        theOtherContainer.fH1_b2_phi             = nullptr;
    }
    
    ~ResolutionContainer()
    {
        delete fResolutionPt_barrel   ;
        delete fResolutionEta_barrel  ;
        delete fResolutionPhi_barrel  ;
        delete fResolutionPt_endcap   ;
        delete fResolutionEta_endcap  ;
        delete fResolutionPhi_endcap  ;
        delete fResolutionPt_vsPu     ;

        delete fGen_H1_b1_matchedflag;
        delete fGen_H1_b2_matchedflag;
        delete fNbJets               ;
        delete fGen_H1_b1_pt         ;
        delete fGen_H1_b2_pt         ;
        delete fGen_H1_b1_eta        ;
        delete fGen_H1_b2_eta        ;
        delete fGen_H1_b1_phi        ;
        delete fGen_H1_b2_phi        ;
        delete fH1_b1_pt             ;
        delete fH1_b2_pt             ;
        delete fH1_b1_eta            ;
        delete fH1_b2_eta            ;
        delete fH1_b1_phi            ;
        delete fH1_b2_phi            ;

        fResolutionPt_barrel   = nullptr;
        fResolutionEta_barrel  = nullptr;
        fResolutionPhi_barrel  = nullptr;
        fResolutionPt_vsPu     = nullptr;
        fResolutionPt_endcap   = nullptr;
        fResolutionEta_endcap  = nullptr;
        fResolutionPhi_endcap  = nullptr;
        fPileUp                = nullptr;
        fGen_H1_b1_matchedflag = nullptr;
        fGen_H1_b2_matchedflag = nullptr;
        fNbJets                = nullptr;
        fGen_H1_b1_pt          = nullptr;
        fGen_H1_b2_pt          = nullptr;
        fGen_H1_b1_eta         = nullptr;
        fGen_H1_b2_eta         = nullptr;
        fGen_H1_b1_phi         = nullptr;
        fGen_H1_b2_phi         = nullptr;
        fH1_b1_pt              = nullptr;
        fH1_b2_pt              = nullptr;
        fH1_b1_eta             = nullptr;
        fH1_b2_eta             = nullptr;
        fH1_b1_phi             = nullptr;
        fH1_b2_phi             = nullptr;
    }

    void initializeTreeReaders (TTreeReader *theTreeReader)
    {
        fPileUp                = new TTreeReaderValue<unsigned int>(*theTreeReader, "pileUp"                );
        fGen_H1_b1_matchedflag = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b1_matchedflag" );
        fGen_H1_b2_matchedflag = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b2_matchedflag" );
        fNbJets                = new TTreeReaderValue<int>(*theTreeReader, "NbJets" );
        fGen_H1_b1_pt          = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b1_pt" );
        fGen_H1_b2_pt          = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b2_pt" );
        fGen_H1_b1_eta         = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b1_eta" );
        fGen_H1_b2_eta         = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b2_eta" );
        fGen_H1_b1_phi         = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b1_phi" );
        fGen_H1_b2_phi         = new TTreeReaderValue<float>(*theTreeReader, "gen_H1_b2_phi" );
        fH1_b1_pt              = new TTreeReaderValue<float>(*theTreeReader, "H1_b1_ptRegressed" );
        fH1_b2_pt              = new TTreeReaderValue<float>(*theTreeReader, "H1_b2_ptRegressed" );
        fH1_b1_eta             = new TTreeReaderValue<float>(*theTreeReader, "H1_b1_eta" );
        fH1_b2_eta             = new TTreeReaderValue<float>(*theTreeReader, "H1_b2_eta" );
        fH1_b1_phi             = new TTreeReaderValue<float>(*theTreeReader, "H1_b1_phi" );
        fH1_b2_phi             = new TTreeReaderValue<float>(*theTreeReader, "H1_b2_phi" );
    }

    void fill(TChain *inputTree, TTreeReader *theTreeReader)
    {
        long long int nEvts  = inputTree->GetEntries();
        for(int it=0; it<inputTree->GetEntries(); ++it)
        {
            if (it % 100000 == 0) std::cout << "Thread number = " << fThreadNumber << " -   ... reading " << it << " / " << nEvts << std::endl;
            inputTree->GetEntry(it);
            theTreeReader->Next();

            int gen_H1_b1_Id = *fGen_H1_b1_matchedflag->Get();
            int gen_H1_b2_Id = *fGen_H1_b2_matchedflag->Get();
            int NbJets_value = *fNbJets->Get();
            if(NbJets_value<4 || gen_H1_b1_Id<0 || gen_H1_b2_Id<0) continue;
            float genPt1  = -999;
            float genPt2  = -999;
            float genEta1 = -999;
            float genEta2 = -999;
            float genPhi1 = -999;
            float genPhi2 = -999;
            if(gen_H1_b1_Id==0 && gen_H1_b2_Id==1)
            {
                genPt1  = *fGen_H1_b1_pt ->Get();
                genEta1 = *fGen_H1_b1_eta->Get();
                genPhi1 = *fGen_H1_b1_phi->Get();

                genPt2  = *fGen_H1_b2_pt ->Get();
                genEta2 = *fGen_H1_b2_eta->Get();
                genPhi2 = *fGen_H1_b2_phi->Get();
            }
            else if(gen_H1_b1_Id==1 && gen_H1_b2_Id==0)
            {
                genPt1  = *fGen_H1_b2_pt ->Get();
                genEta1 = *fGen_H1_b2_eta->Get();
                genPhi1 = *fGen_H1_b2_phi->Get();

                genPt2  = *fGen_H1_b1_pt ->Get();
                genEta2 = *fGen_H1_b1_eta->Get();
                genPhi2 = *fGen_H1_b1_phi->Get();
            }
            else std::cout<<"Impossible\n";
            float recoPt1  = *fH1_b1_pt ->Get();
            float recoEta1 = *fH1_b1_eta->Get();
            float recoPhi1 = *fH1_b1_phi->Get();

            float recoPt2  = *fH1_b2_pt ->Get();
            float recoEta2 = *fH1_b2_eta->Get();
            float recoPhi2 = *fH1_b2_phi->Get();
            int   pileUp   = *fPileUp   ->Get();

            if(recoPt1 >= 500) fResolutionPt_vsPu->Fill(pileUp , recoPt1-genPt1);
            if(recoPt2 >= 500) fResolutionPt_vsPu->Fill(pileUp , recoPt2-genPt2);

            if(abs(recoEta1) <= 1.4)
            {
                fResolutionPt_barrel ->Fill(recoPt1, recoPt1-genPt1);
                fResolutionEta_barrel->Fill(recoPt1, recoEta1-genEta1);
                fResolutionPhi_barrel->Fill(recoPt1, deltaPhi(recoPhi1,genPhi1));
            }
            else if(abs(recoEta1) > 1.4 && abs(recoEta1) <=2.4)
            {
                fResolutionPt_endcap ->Fill(recoPt1, recoPt1-genPt1);
                fResolutionEta_endcap->Fill(recoPt1, recoEta1-genEta1);
                fResolutionPhi_endcap->Fill(recoPt1, deltaPhi(recoPhi1,genPhi1));
            }
            else std::cout<<"Impossible\n";
            

            if(abs(recoEta2) <= 1.4)
            {
                fResolutionPt_barrel ->Fill(recoPt2, recoPt2-genPt2);
                fResolutionEta_barrel->Fill(recoPt2, recoEta2-genEta2);
                fResolutionPhi_barrel->Fill(recoPt2, deltaPhi(recoPhi2,genPhi2));
            }
            else if(abs(recoEta2) > 1.4 && abs(recoEta2) <=2.4)
            {
                fResolutionPt_endcap ->Fill(recoPt2, recoPt2-genPt2);
                fResolutionEta_endcap->Fill(recoPt2, recoEta2-genEta2);
                fResolutionPhi_endcap->Fill(recoPt2, deltaPhi(recoPhi2,genPhi2));
            }
            else std::cout<<"Impossible\n";

        }

    }

    ResolutionContainer& operator+=(const ResolutionContainer& theResolutionContainer)
    {
        fResolutionPt_barrel ->Add(theResolutionContainer.fResolutionPt_barrel );
        fResolutionEta_barrel->Add(theResolutionContainer.fResolutionEta_barrel);
        fResolutionPhi_barrel->Add(theResolutionContainer.fResolutionPhi_barrel);
        fResolutionPt_endcap ->Add(theResolutionContainer.fResolutionPt_endcap );
        fResolutionEta_endcap->Add(theResolutionContainer.fResolutionEta_endcap);
        fResolutionPhi_endcap->Add(theResolutionContainer.fResolutionPhi_endcap);
        fResolutionPt_vsPu   ->Add(theResolutionContainer.fResolutionPt_vsPu   );
        return *this;
    }

    void writeInFile(std::string fileName)
    {
        TFile outputFile((fileName + ".root").data(), "RECREATE");
        std::string fitFunction  = "[0] + [1]/x + [2]/sqrt(x) + [3]/pow(x,1./3.) + [4]/pow(x,1./4.)";
        gStyle->SetOptFit();
        
        fResolutionPt_barrel ->Write();
        fResolutionEta_barrel->Write();
        fResolutionPhi_barrel->Write();
        fResolutionPt_endcap ->Write();
        fResolutionEta_endcap->Write();
        fResolutionPhi_endcap->Write();
        fResolutionPt_vsPu   ->Write();

        fResolutionPt_barrel ->SetDirectory(0);
        fResolutionEta_barrel->SetDirectory(0);
        fResolutionPhi_barrel->SetDirectory(0);
        fResolutionPt_endcap ->SetDirectory(0);
        fResolutionEta_endcap->SetDirectory(0);
        fResolutionPhi_endcap->SetDirectory(0);
        fResolutionPt_vsPu   ->SetDirectory(0);


        TCanvas theCanvas_barrel("c1_barrel","c1_barrel", 1400, 800);
        theCanvas_barrel.DivideSquare(6);

        theCanvas_barrel.cd(1);
        fResolutionPt_barrel->SetStats(0);
        fResolutionPt_barrel->Draw("colz");
        TObjArray ptSlices_barrel;
        fResolutionPt_barrel->FitSlicesY(0, 0, -1, 10, "QNR", &ptSlices_barrel);
        theCanvas_barrel.cd(4);
        TF1 *fitFunctionPt_barrel = new TF1("fitFuntionPt_barrel", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(ptSlices_barrel.At(2))->Fit(fitFunctionPt_barrel, "R");
        fitFunctionPt_barrel->Write();
        static_cast<TH1D*>(ptSlices_barrel.At(2))->GetYaxis()->SetRangeUser(0., 150.);
        ptSlices_barrel.At(2)->Draw();
        static_cast<TH1D*>(ptSlices_barrel.At(2))->GetYaxis()->SetTitle(("#sigma(reco p_{T} - gen p_{T}) [GeV]"));
        static_cast<TH1D*>(ptSlices_barrel.At(2))->SetStats(0);
        static_cast<TH1D*>(ptSlices_barrel.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(ptSlices_barrel.At(0), "ptSlices_barrel_Norm");
        static_cast<TH1D*>(ptSlices_barrel.At(0))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_barrel.At(1), "ptSlices_barrel_Mean");
        static_cast<TH1D*>(ptSlices_barrel.At(1))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_barrel.At(2), "ptSlices_barrel_Sigma");
        static_cast<TH1D*>(ptSlices_barrel.At(2))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_barrel.At(3), "ptSlices_barrel_Chi2");
        static_cast<TH1D*>(ptSlices_barrel.At(3))->SetDirectory(0);
        
        theCanvas_barrel.cd(2);
        fResolutionEta_barrel->SetStats(0);
        fResolutionEta_barrel->Draw("colz");
        TObjArray etaSlices_barrel;
        fResolutionEta_barrel->FitSlicesY(0, 0, -1, 10, "QNR", &etaSlices_barrel);
        theCanvas_barrel.cd(5);
        TF1 *fitFunctionEta_barrel = new TF1("fitFuntionEta_barrel", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(etaSlices_barrel.At(2))->Fit(fitFunctionEta_barrel, "R");
        fitFunctionEta_barrel->Write();
        static_cast<TH1D*>(etaSlices_barrel.At(2))->GetYaxis()->SetRangeUser(0., 0.1);
        etaSlices_barrel.At(2)->Draw();
        static_cast<TH1D*>(etaSlices_barrel.At(2))->GetYaxis()->SetTitle(("#sigma(reco #eta - gen #eta)"));
        static_cast<TH1D*>(etaSlices_barrel.At(2))->SetStats(0);
        static_cast<TH1D*>(etaSlices_barrel.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(etaSlices_barrel.At(0), "etaSlices_barrel_Norm");
        static_cast<TH1D*>(etaSlices_barrel.At(0))->SetDirectory(0);
        outputFile.WriteObject(etaSlices_barrel.At(1), "etaSlices_barrel_Mean");
        static_cast<TH1D*>(etaSlices_barrel.At(1))->SetDirectory(0);
        outputFile.WriteObject(etaSlices_barrel.At(2), "etaSlices_barrel_Sigma");
        static_cast<TH1D*>(etaSlices_barrel.At(2))->SetDirectory(0);
        outputFile.WriteObject(etaSlices_barrel.At(3), "etaSlices_barrel_Chi2");
        static_cast<TH1D*>(etaSlices_barrel.At(3))->SetDirectory(0);

        theCanvas_barrel.cd(3);
        fResolutionPhi_barrel->SetStats(0);
        fResolutionPhi_barrel->Draw("colz");
        TObjArray phiSlices_barrel;
        fResolutionPhi_barrel->FitSlicesY(0, 0, -1, 10, "QNR", &phiSlices_barrel);
        theCanvas_barrel.cd(6);
        TF1 *fitFunctionPhi_barrel = new TF1("fitFuntionPhi_barrel", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(phiSlices_barrel.At(2))->Fit(fitFunctionPhi_barrel, "R");
        fitFunctionPhi_barrel->Write();
        static_cast<TH1D*>(phiSlices_barrel.At(2))->GetYaxis()->SetRangeUser(0., 0.1);
        phiSlices_barrel.At(2)->Draw();
        static_cast<TH1D*>(phiSlices_barrel.At(2))->GetYaxis()->SetTitle(("#sigma(reco #phi - gen #phi)"));
        static_cast<TH1D*>(phiSlices_barrel.At(2))->SetStats(0);
        static_cast<TH1D*>(phiSlices_barrel.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(phiSlices_barrel.At(0), "phiSlices_barrel_Norm");
        static_cast<TH1D*>(phiSlices_barrel.At(0))->SetDirectory(0);
        outputFile.WriteObject(phiSlices_barrel.At(1), "phiSlices_barrel_Mean");
        static_cast<TH1D*>(phiSlices_barrel.At(1))->SetDirectory(0);
        outputFile.WriteObject(phiSlices_barrel.At(2), "phiSlices_barrel_Sigma");
        static_cast<TH1D*>(phiSlices_barrel.At(2))->SetDirectory(0);
        outputFile.WriteObject(phiSlices_barrel.At(3), "phiSlices_barrel_Chi2");
        static_cast<TH1D*>(phiSlices_barrel.At(3))->SetDirectory(0);

        theCanvas_barrel.SaveAs((fileName + "_barrel.png").data());
        
        TCanvas theCanvas_endcap("c1_endcap","c1_endcap", 1400, 800);
        theCanvas_endcap.DivideSquare(6);

        theCanvas_endcap.cd(1);
        fResolutionPt_endcap->SetStats(0);
        fResolutionPt_endcap->Draw("colz");
        TObjArray ptSlices_endcap;
        fResolutionPt_endcap->FitSlicesY(0, 0, -1, 10, "QNR", &ptSlices_endcap);
        theCanvas_endcap.cd(4);
        TF1 *fitFunctionPt_endcap = new TF1("fitFuntionPt_endcap", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(ptSlices_endcap.At(2))->Fit(fitFunctionPt_endcap, "R");
        fitFunctionPt_endcap->Write();
        static_cast<TH1D*>(ptSlices_endcap.At(2))->GetYaxis()->SetRangeUser(0., 150.);
        ptSlices_endcap.At(2)->Draw();
        static_cast<TH1D*>(ptSlices_endcap.At(2))->GetYaxis()->SetTitle(("#sigma(reco p_{T} - gen p_{T}) [GeV]"));
        static_cast<TH1D*>(ptSlices_endcap.At(2))->SetStats(0);
        static_cast<TH1D*>(ptSlices_endcap.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(ptSlices_endcap.At(0), "ptSlices_endcap_Norm");
        static_cast<TH1D*>(ptSlices_endcap.At(0))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_endcap.At(1), "ptSlices_endcap_Mean");
        static_cast<TH1D*>(ptSlices_endcap.At(1))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_endcap.At(2), "ptSlices_endcap_Sigma");
        static_cast<TH1D*>(ptSlices_endcap.At(2))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_endcap.At(3), "ptSlices_endcap_Chi2");
        static_cast<TH1D*>(ptSlices_endcap.At(3))->SetDirectory(0);
        
        theCanvas_endcap.cd(2);
        fResolutionEta_endcap->SetStats(0);
        fResolutionEta_endcap->Draw("colz");
        TObjArray etaSlices_endcap;
        fResolutionEta_endcap->FitSlicesY(0, 0, -1, 10, "QNR", &etaSlices_endcap);
        theCanvas_endcap.cd(5);
        TF1 *fitFunctionEta_endcap = new TF1("fitFuntionEta_endcap", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(etaSlices_endcap.At(2))->Fit(fitFunctionEta_endcap, "R");
        fitFunctionEta_endcap->Write();
        static_cast<TH1D*>(etaSlices_endcap.At(2))->GetYaxis()->SetRangeUser(0., 0.1);
        etaSlices_endcap.At(2)->Draw();
        static_cast<TH1D*>(etaSlices_endcap.At(2))->GetYaxis()->SetTitle(("#sigma(reco #eta - gen #eta)"));
        static_cast<TH1D*>(etaSlices_endcap.At(2))->SetStats(0);
        static_cast<TH1D*>(etaSlices_endcap.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(etaSlices_endcap.At(0), "etaSlices_endcap_Norm");
        static_cast<TH1D*>(etaSlices_endcap.At(0))->SetDirectory(0);
        outputFile.WriteObject(etaSlices_endcap.At(1), "etaSlices_endcap_Mean");
        static_cast<TH1D*>(etaSlices_endcap.At(1))->SetDirectory(0);
        outputFile.WriteObject(etaSlices_endcap.At(2), "etaSlices_endcap_Sigma");
        static_cast<TH1D*>(etaSlices_endcap.At(2))->SetDirectory(0);
        outputFile.WriteObject(etaSlices_endcap.At(3), "etaSlices_endcap_Chi2");
        static_cast<TH1D*>(etaSlices_endcap.At(3))->SetDirectory(0);

        theCanvas_endcap.cd(3);
        fResolutionPhi_endcap->SetStats(0);
        fResolutionPhi_endcap->Draw("colz");
        TObjArray phiSlices_endcap;
        fResolutionPhi_endcap->FitSlicesY(0, 0, -1, 10, "QNR", &phiSlices_endcap);
        theCanvas_endcap.cd(6);
        TF1 *fitFunctionPhi_endcap = new TF1("fitFuntionPhi_endcap", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(phiSlices_endcap.At(2))->Fit(fitFunctionPhi_endcap, "R");
        fitFunctionPhi_endcap->Write();
        static_cast<TH1D*>(phiSlices_endcap.At(2))->GetYaxis()->SetRangeUser(0., 0.1);
        phiSlices_endcap.At(2)->Draw();
        static_cast<TH1D*>(phiSlices_endcap.At(2))->GetYaxis()->SetTitle(("#sigma(reco #phi - gen #phi)"));
        static_cast<TH1D*>(phiSlices_endcap.At(2))->SetStats(0);
        static_cast<TH1D*>(phiSlices_endcap.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(phiSlices_endcap.At(0), "phiSlices_endcap_Norm");
        static_cast<TH1D*>(phiSlices_endcap.At(0))->SetDirectory(0);
        outputFile.WriteObject(phiSlices_endcap.At(1), "phiSlices_endcap_Mean");
        static_cast<TH1D*>(phiSlices_endcap.At(1))->SetDirectory(0);
        outputFile.WriteObject(phiSlices_endcap.At(2), "phiSlices_endcap_Sigma");
        static_cast<TH1D*>(phiSlices_endcap.At(2))->SetDirectory(0);
        outputFile.WriteObject(phiSlices_endcap.At(3), "phiSlices_endcap_Chi2");
        static_cast<TH1D*>(phiSlices_endcap.At(3))->SetDirectory(0);

        theCanvas_endcap.SaveAs((fileName + "_endcap.png").data());
        outputFile.Close();
        

        TCanvas theCanvas_vsPu("c1_vsPu","c1_vsPu", 500, 800);
        theCanvas_vsPu.DivideSquare(2);

        theCanvas_vsPu.cd(1);
        fResolutionPt_vsPu->SetStats(0);
        fResolutionPt_vsPu->Draw("colz");
        TObjArray ptSlices_vsPu;
        fResolutionPt_vsPu->FitSlicesY(0, 0, -1, 10, "QNR", &ptSlices_vsPu);
        theCanvas_vsPu.cd(2);
        TF1 *fitFunctionPt_vsPu = new TF1("fitFuntionPt_vsPu", fitFunction.data(), 30., 1000.);
        static_cast<TH1D*>(ptSlices_vsPu.At(2))->Fit(fitFunctionPt_vsPu, "R");
        fitFunctionPt_vsPu->Write();
        static_cast<TH1D*>(ptSlices_vsPu.At(2))->GetYaxis()->SetRangeUser(0., 150.);
        ptSlices_vsPu.At(2)->Draw();
        static_cast<TH1D*>(ptSlices_vsPu.At(2))->GetYaxis()->SetTitle(("#sigma(reco p_{T} - gen p_{T}) [GeV]"));
        static_cast<TH1D*>(ptSlices_vsPu.At(2))->SetStats(0);
        static_cast<TH1D*>(ptSlices_vsPu.At(2))->GetYaxis()->SetTitleOffset(1.3);
        outputFile.WriteObject(ptSlices_vsPu.At(0), "ptSlices_vsPu_Norm");
        static_cast<TH1D*>(ptSlices_vsPu.At(0))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_vsPu.At(1), "ptSlices_vsPu_Mean");
        static_cast<TH1D*>(ptSlices_vsPu.At(1))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_vsPu.At(2), "ptSlices_vsPu_Sigma");
        static_cast<TH1D*>(ptSlices_vsPu.At(2))->SetDirectory(0);
        outputFile.WriteObject(ptSlices_vsPu.At(3), "ptSlices_vsPu_Chi2");
        static_cast<TH1D*>(ptSlices_vsPu.At(3))->SetDirectory(0);
        
        theCanvas_vsPu.SaveAs((fileName + "_vsPu.png").data());
        outputFile.Close();
        
    }

    int fThreadNumber {-1};

    TH2F *fResolutionPt_barrel  {nullptr};
    TH2F *fResolutionEta_barrel {nullptr};
    TH2F *fResolutionPhi_barrel {nullptr};
    TH2F *fResolutionPt_endcap  {nullptr};
    TH2F *fResolutionEta_endcap {nullptr};
    TH2F *fResolutionPhi_endcap {nullptr};
    TH2F *fResolutionPt_vsPu    {nullptr};

    TTreeReaderValue<unsigned int>* fPileUp         {nullptr};
    TTreeReaderValue<float>* fGen_H1_b1_matchedflag {nullptr};
    TTreeReaderValue<float>* fGen_H1_b2_matchedflag {nullptr};
    TTreeReaderValue<int  >* fNbJets                {nullptr};
    TTreeReaderValue<float>* fGen_H1_b1_pt          {nullptr};
    TTreeReaderValue<float>* fGen_H1_b2_pt          {nullptr};
    TTreeReaderValue<float>* fGen_H1_b1_eta         {nullptr};
    TTreeReaderValue<float>* fGen_H1_b2_eta         {nullptr};
    TTreeReaderValue<float>* fGen_H1_b1_phi         {nullptr};
    TTreeReaderValue<float>* fGen_H1_b2_phi         {nullptr};
    TTreeReaderValue<float>* fH1_b1_pt              {nullptr};
    TTreeReaderValue<float>* fH1_b2_pt              {nullptr};
    TTreeReaderValue<float>* fH1_b1_eta             {nullptr};
    TTreeReaderValue<float>* fH1_b2_eta             {nullptr};
    TTreeReaderValue<float>* fH1_b1_phi             {nullptr};
    TTreeReaderValue<float>* fH1_b2_phi             {nullptr};
                 
};


void PlotListResolutions(const std::vector<std::string>& fileNameList, ResolutionContainer& theResolutionContainer)
{
    TChain *inputTree = new TChain("bbbbTree");
    for(const auto& fileName : fileNameList) inputTree->Add(fileName.data());
    
    TTreeReader *theTreeReader  = new TTreeReader(inputTree);
    theResolutionContainer.initializeTreeReaders (theTreeReader);
    theResolutionContainer.fill(inputTree, theTreeReader);
}


int main(int argc, char *argv[])
{
    gROOT->ProcessLine(".x ~/.rootlogon.C");
    gROOT->ForceStyle();
    gStyle->SetOptFit();

    if(argc < 2)
    {
        std::cout << "Usage: ./ResolutionsForKinFit year" << std::endl;
        return EXIT_FAILURE;
    }

    gSystem->ResetSignal(kSigSegmentationViolation, kTRUE);
    ROOT::EnableThreadSafety();
    gROOT->SetBatch(true);

    std::string inputFileName  = "plotterListFiles/" + std::string(argv[1]) + "Resonant_NMSSM_XYH_bbbb/Signal/FileList_NMSSM_XYH_bbbb_All.txt";
    std::string outputFileName = "KinematicFit_" + std::string(argv[1]);

    std::vector<std::vector<std::string>> fileList(numberOfThreads);
    std::ifstream fList (inputFileName);
    if (!fList.good())
    {
        std::cerr << "*** Sample::openFileAndTree : ERROR : could not open file " << inputFileName << std::endl;
        return EXIT_FAILURE;
    }
    std::string line;
    int currentThread = 0;
    while (std::getline(fList, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) fileList[currentThread++].emplace_back(line);
        if (currentThread>=numberOfThreads) currentThread = 0;
    }

    std::vector<ResolutionContainer> theResolutionContainerVector;
    std::vector<std::thread>         theThreadVector             ;
    

    for(int threadNumber=0; threadNumber<numberOfThreads; ++threadNumber)
    {
        theResolutionContainerVector.emplace_back(ResolutionContainer(threadNumber));
    }

    for(int threadNumber=0; threadNumber<numberOfThreads; ++threadNumber)
    {
        theThreadVector.emplace_back( std::thread(PlotListResolutions, std::ref(fileList[threadNumber]), std::ref(theResolutionContainerVector[threadNumber])) );
    }

    for(auto& theThread : theThreadVector) theThread.join();
    
    ResolutionContainer theTotalResolutionContainer;
    for(const auto& theResolutionContainer : theResolutionContainerVector) theTotalResolutionContainer += theResolutionContainer;

    theTotalResolutionContainer.writeInFile(outputFileName);

    return EXIT_SUCCESS;

}
