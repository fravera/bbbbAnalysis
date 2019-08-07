#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <algorithm>
#include "TArrow.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TColor.h"
#include "TF1.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TFractionFitter.h"
#include "TFrame.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TLorentzVector.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TMatrixDSym.h"
#include "TPaveText.h"
#include "TPaveText.h"
#include "TProfile.h"
#include "TRandom3.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TView.h"
#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooExtendPdf.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooCBShape.h"
#include "RooChebychev.h"
#include "RooConstVar.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooRandom.h"
#include "RooIntegralMorph.h"
#include "RooNLLVar.h"
#include "RooPolynomial.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooWorkspace.h"

using namespace RooFit ;

void make_shapes(float range_lo, float range_hi, string signal, string categ, string var, string tag,int seed) {

    //Get File
    TFile *file=new TFile(Form("../MyHistosNominal/Histos%s/outPlotter.root",tag.c_str()));
    //Get Signal histogram and normalization   
    TH1F *h_S=(TH1F*)file->Get( Form("%s/Btag4_%s_SR_110_Histogram/%s_Btag4_%s_SR_110_Histogram_%s",signal.c_str(), categ.c_str(), signal.c_str(),categ.c_str(),var.c_str() ));
    //Get Background histogram and normalization
    TH1F *h_B=(TH1F*)file->Get(Form("MODEL/Btag4_%s_SR_110_Histogram/MODEL_Btag4_%s_SR_110_Histogram_%s",categ.c_str(),categ.c_str(),var.c_str())  );

    //Normalize templates
    double nS= h_S->Integral(h_S->FindBin(range_lo),h_S->FindBin(range_hi)); 
    double nB= h_B->Integral(h_B->FindBin(range_lo),h_B->FindBin(range_hi)); 

    cout<<"S Normalization = "<<nS<<", Background Normalization = "<<nB<<endl;
    //Normalize histograms before creating the pdf
    h_S->Scale(1/h_S->Integral() );
    h_B->Scale(1/h_B->Integral() );
    cout<<"S Normalization = "<<h_S->Integral(h_S->FindBin(range_lo),h_S->FindBin(range_hi))<<", Background Normalization = "<<h_B->Integral(h_B->FindBin(range_lo),h_B->FindBin(range_hi))<<endl;
    RooRandom::randomGenerator()->SetSeed(seed); 
    h_S->Sumw2(0); h_S->Sumw2();

    RooRealVar x("x", "BDT", range_lo, range_hi);
    int nbins = abs(h_S->FindBin(range_lo)-h_S->FindBin(range_hi));
    x.setBins(nbins);
    RooArgSet myVars(x);
    RooDataHist Sigpred("Sigpred", "Signal prediction", myVars, h_S);
    RooHistPdf  Sigpredpdf("Sigpredpdf","Signal hist  pdf",myVars,Sigpred);
    RooDataHist Bkgpred("Bkgpred", "Background prediction", myVars, h_B);
    RooHistPdf  Bkgpredpdf("Bkgpredpdf","Bkg hist  pdf",myVars,Bkgpred);

    RooWorkspace *w = new RooWorkspace();
    w->import(myVars);
    w->import(Sigpred);
    w->import(Sigpredpdf);
    w->import(Bkgpred);
    w->import(Bkgpredpdf);
    w->factory(Form("SUM::model_s(%f*Sigpredpdf,%f*Bkgpredpdf)",nS,nB)  );
    w->factory(Form("SUM::model_b(%f*Bkgpredpdf)",nB));

    RooArgSet obs(*w->var("x"));
    RooDataSet *data_s = w->pdf("model_s")->generate(obs,nS+nB);
    RooDataSet *data_b = w->pdf("model_b")->generate(obs,nB);

    TCanvas *c1 = new TCanvas("c1","c1");
    RooPlot *frame = w->var("x")->frame();
    data_s->plotOn(frame);
    w->pdf("model_s")->plotOn(frame, LineColor(kRed));
    frame->Draw();
    frame = w->var("x")->frame();
    data_b->plotOn(frame);
    w->pdf("model_b")->plotOn(frame);
    frame->Draw();
    RooDataHist *bdata_b = new RooDataHist("data_obs", "", obs, *data_b);
    RooDataHist *bdata_s = new RooDataHist("data_sig", "", obs, *data_s);//

    // ------------ Make histograms ---------------------------
    TFile *allHistFile = new TFile( Form("workspace_%s_%s_%s.root", signal.c_str(),categ.c_str(),tag.c_str() )  , "RECREATE");
    // Signal model
    TH1 *signal_nominal = w->pdf("Sigpredpdf")->createHistogram("x"); 
    signal_nominal->Sumw2(0);    signal_nominal->Sumw2();
    signal_nominal->SetName("signal"); signal_nominal->Scale(nS/signal_nominal->Integral());
    c1->Clear();
    signal_nominal->Draw("H"); signal_nominal->SetLineColor(kBlack); signal_nominal->SetLineWidth(3);
    // background model 
    frame = w->var("x")->frame();
    TH1 *background = w->pdf("Bkgpredpdf")->createHistogram("x"); 
    background->SetName("background"); background->Scale(nB/background->Integral());
    background->Draw("H");             background->SetLineColor(kBlack); background->SetLineWidth(3);

    // data
    TH1 *hdata_b = bdata_b->createHistogram("x"); hdata_b->SetName("data_obs");
    TH1 *hdata_s = bdata_s->createHistogram("x"); hdata_s->SetName("data_sig");
    // write to file
    allHistFile->WriteTObject(signal_nominal); 
    allHistFile->WriteTObject(background); 
    allHistFile->WriteTObject(hdata_b);
    allHistFile->WriteTObject(hdata_s);//
    
}
