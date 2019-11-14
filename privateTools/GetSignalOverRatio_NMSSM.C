#include "Riostream.h"
#include "TFile.h"
#include "TH2F.h"
#include "TF2.h"
#include "TCanvas.h"
#include "TEllipse.h"

bool isInEllipse(float x0, float y0, float r1, float r2, float angle, float pointX, float pointY)
{
    float cos=TMath::Cos(angle);
    float sin=TMath::Sin(angle);

    return TMath::Power((cos*(pointX-x0) + sin*(pointY-y0))/r1,2) + TMath::Power((sin*(pointX-x0) - cos*(pointY-y0))/r2,2) <= 1;
}


void GetSignalOverRatio_NMSSM(std::string genMatchedFileName, std::string inputFileName, std::string datasetName, float nSigma)
{
    std::string selectionType = "selectionbJetsAndTrigger_SignalRegion";

    TFile genMatchedFile(genMatchedFileName.data());
    TFile inputFile(inputFileName.data());

    TH2F *genMatchedSignalDistribution = (TH2F*)genMatchedFile.Get( (datasetName + "/" + selectionType + "GenMatched/" + datasetName + "_" + selectionType + "GenMatched_HH_m_H2_m" ).data() );
    genMatchedSignalDistribution->SetDirectory(0);
    float xMean = genMatchedSignalDistribution->GetMean(1);
    float yMean = genMatchedSignalDistribution->GetMean(2);
    float xRMS  = genMatchedSignalDistribution->GetRMS (1);
    float yRMS  = genMatchedSignalDistribution->GetRMS (2);
    // std::cout << xMean << "\t" << yMean << "\t" << xRMS << "\t" << yRMS << std::endl;

    TF2 *bigaussianFunction = new TF2("bigaussianFunction","bigaus", xMean - 3.*xRMS, xMean + 3.*xRMS, yMean - 3.*yRMS, yMean + 3.*yRMS);

    bigaussianFunction->SetParameters(genMatchedSignalDistribution->Integral(),xMean,xRMS,yMean,yRMS,0.8);
    bigaussianFunction->SetParLimits(0, genMatchedSignalDistribution->Integral()*0.75, genMatchedSignalDistribution->Integral()*1.25);
    bigaussianFunction->SetParLimits(1, xMean*0.75, xMean*1.25);
    bigaussianFunction->SetParLimits(2, xRMS*0.75, xRMS*1.25);
    bigaussianFunction->SetParLimits(3, yMean*0.75, yMean*1.25);
    bigaussianFunction->SetParLimits(4, yRMS*0.75, yRMS*1.25);
    // bigaussianFunction->FixParameter(0, genMatchedSignalDistribution->Integral());
    genMatchedSignalDistribution->Fit(bigaussianFunction,"RBMNQ0");

    float conicA = 1/(bigaussianFunction->GetParameter(2)*bigaussianFunction->GetParameter(2));
    float conicB = - 2.*bigaussianFunction->GetParameter(5) / (bigaussianFunction->GetParameter(2)*bigaussianFunction->GetParameter(4));
    float conicC = 1/(bigaussianFunction->GetParameter(4)*bigaussianFunction->GetParameter(4));

    float denominator  = 2. * (1. - bigaussianFunction->GetParameter(5)*bigaussianFunction->GetParameter(5)) ;
    conicA /= denominator;
    conicB /= denominator;
    conicC /= denominator;

    float angle = 0.5*TMath::ATan(conicB / (conicA-conicC) );
    float r1    = conicA*TMath::Cos(angle)*TMath::Cos(angle) + conicB*TMath::Cos(angle)*TMath::Sin(angle) + conicC*TMath::Sin(angle)*TMath::Sin(angle);
    float r2    = conicA*TMath::Sin(angle)*TMath::Sin(angle) - conicB*TMath::Cos(angle)*TMath::Sin(angle) + conicC*TMath::Cos(angle)*TMath::Cos(angle);
    float newB  = 2.* (conicC - conicA) *TMath::Sin(angle)*TMath::Cos(angle) + conicB*(TMath::Cos(angle)*TMath::Cos(angle) - TMath::Sin(angle)*TMath::Sin(angle));
    // std::cout << genMatchedSignalDistribution->Integral() << "\t" << conicA << "\t" << conicB << "\t" << conicC << "\t" << angle << "\t" << r1 << "\t" << r2 << "\t" << newB << std::endl;

    // TEllipse *theEllipse = new TEllipse(bigaussianFunction->GetParameter(1),bigaussianFunction->GetParameter(3),bigaussianFunction->GetParameter(2)*nSigma,bigaussianFunction->GetParameter(4)*nSigma,0,360,angle);
    // TEllipse *theOldEllipse = new TEllipse(bigaussianFunction->GetParameter(1),bigaussianFunction->GetParameter(3),bigaussianFunction->GetParameter(2),bigaussianFunction->GetParameter(4),0,360,0);
    TEllipse *theEllipse = new TEllipse(bigaussianFunction->GetParameter(1),bigaussianFunction->GetParameter(3),1./TMath::Sqrt(r1)*nSigma,1./TMath::Sqrt(r2)*nSigma,0,360,angle * 180./TMath::Pi());
    // TEllipse *theEllipse = new TEllipse(xMean,yMean,xRMS,yRMS,0,360,0);
    // theOldEllipse->SetLineColor(kBlue);
    // theOldEllipse->SetLineWidth(3);
    // theOldEllipse->SetFillStyle(0);	
    // theOldEllipse->Draw("same");

    theEllipse->SetLineColor(kRed);
    theEllipse->SetLineWidth(2);
    theEllipse->SetFillStyle(0);

    TCanvas *theCanvas = new TCanvas();
    theCanvas->DivideSquare(4);

    // TCanvas *cSignalGenMatched = new TCanvas();
    theCanvas->cd(1);
    genMatchedSignalDistribution->SetStats(0);
    genMatchedSignalDistribution->Draw("colz");
    theEllipse->Draw("same");

    TH2F *backgroundDistribution = (TH2F*)inputFile.Get(("data_BTagCSV_Background/" + selectionType + "/data_BTagCSV_Background_" + selectionType + "_HH_m_H2_m").data());
    backgroundDistribution->SetDirectory(0);

    float background = 0.;
    for(int xBin=1; xBin<=backgroundDistribution->GetNbinsX(); ++xBin)
    {
        for(int yBin=1; yBin<=backgroundDistribution->GetNbinsY(); ++yBin)
        {
            if(isInEllipse(bigaussianFunction->GetParameter(1),bigaussianFunction->GetParameter(3), 1./TMath::Sqrt(r1)*nSigma,1./TMath::Sqrt(r2)*nSigma, angle, backgroundDistribution->GetXaxis()->GetBinCenter(xBin), backgroundDistribution->GetYaxis()->GetBinCenter(yBin))) background += backgroundDistribution->GetBinContent(xBin,yBin);
        }
    }

    // TCanvas *cBackground = new TCanvas();
    theCanvas->cd(2);
    backgroundDistribution->SetStats(0);
    backgroundDistribution->Draw("colz");
    theEllipse->Draw("same");


    TH2F *signalDistribution = (TH2F*)inputFile.Get( (datasetName + "/" + selectionType + "/" + datasetName + "_" + selectionType + "_HH_m_H2_m" ).data() );
    signalDistribution->SetDirectory(0);

    float signal = 0.;
    for(int xBin=1; xBin<=signalDistribution->GetNbinsX(); ++xBin)
    {
        for(int yBin=1; yBin<=signalDistribution->GetNbinsY(); ++yBin)
        {
            if(isInEllipse(bigaussianFunction->GetParameter(1),bigaussianFunction->GetParameter(3), 1./TMath::Sqrt(r1)*nSigma,1./TMath::Sqrt(r2)*nSigma, angle, signalDistribution->GetXaxis()->GetBinCenter(xBin), signalDistribution->GetYaxis()->GetBinCenter(yBin))) signal += signalDistribution->GetBinContent(xBin,yBin);
        }
    }

    // TCanvas *cSignal = new TCanvas();
    theCanvas->cd(3);
    signalDistribution->SetStats(0);
    signalDistribution->Draw("colz");
    theEllipse->Draw("same");
    // theCanvas->SaveAs((datasetName+"_ellipse.pdf").data());
    float run2Factor = 300./35.9;

    std::cout << datasetName << "\t" << background << "\t" <<  signal << "\t" <<  signal/TMath::Sqrt(background)
    // << "\t" <<  TMath::Sqrt(background)*5./signal*1000. 
    << "\t" <<  TMath::Sqrt(background)*1.96/signal*1000.
    // << "\t" <<  TMath::Sqrt(background*run2Factor)*1.96/(signal*run2Factor)*1000.
    << std::endl;

    genMatchedFile.Close();
    inputFile.Close();

}

void DoAllSignalOverNoise()
{
    gROOT->SetBatch();

    std::cout << "dataset" << "\t" << "background" << "\t" <<  "signal" << "\t" <<  "S/sqrt(B)" 
    << "\t" <<  "5_sigma_xsec" << "\t" << "95%_CL_UL_xsec"
    << "\t" <<  "95%_CL_UL_xsec_300_fb-1"
    << std::endl;

    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_300_MY_60",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_400_MY_100",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_700_MY_300",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_900_MY_200",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_1000_MY_700",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_1600_MY_700",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200",2);
    GetSignalOverRatio_NMSSM("2016DataPlots_NMSSM_XYH_bbbb_signalOnly/outPlotter.root","2016DataPlots_NMSSM_XYH_bbbb_triggerMatchedBKGNorm/outPlotter.root","sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800",2);

}
