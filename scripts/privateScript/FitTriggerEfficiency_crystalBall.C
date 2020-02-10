#include "Riostream.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TF1.h"
#include "TF1Convolution.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFitResultPtr.h"
#include "TStyle.h"
#include "TString.h"
#include "TAxis.h"

ofstream outputFile ("fitCurves.h", ios::out);

void doFit(TVirtualPad *theCanvas, TFile &theInputFile, std::string&& plotName, std::string fitFunction, std::vector<double> initialParameters , double xMin, double xMax, bool drawPlot = true, std::string appendToFunctionName = "")
{
    TGraphAsymmErrors* theTriggerEfficiency = (TGraphAsymmErrors*)theInputFile.Get(plotName.data());
    theCanvas->cd();
    
    TF1 *theFunction  = new TF1("cdf",fitFunction.data(), xMin, xMax);
    if(fitFunction.find("crystalball_cdf") != string::npos)
    {
        TF1 *theSCurveFunction = new TF1("fitFunction", "0.5*(1 + TMath::Erf( (x[0]-[0])/[1]) ) * [3] + [2]", xMin, xMax);
        theSCurveFunction->SetParameters(initialParameters.data());
        theSCurveFunction->SetParLimits(2,0,1);
        theSCurveFunction->SetParLimits(3,0,2);
        theTriggerEfficiency->Fit(theSCurveFunction, "Q0RE");

        theFunction->SetParameter(0, theSCurveFunction->GetParameter(0));
        theFunction->SetParameter(1, theSCurveFunction->GetParameter(1));
        theFunction->SetParameter(2, theSCurveFunction->GetParameter(2));
        theFunction->SetParameter(3, theSCurveFunction->GetParameter(3));
        theFunction->SetParameter(4, 3.);
        theFunction->SetParameter(5, 3.);
        theFunction->SetParLimits(2,0,1);
        theFunction->SetParLimits(3,0,1);
        theFunction->SetParLimits(4,1.001,10);
        theFunction->SetParLimits(5,0.001,10.);
        if(fitFunction.find("Erf") != string::npos) theFunction->SetParLimits(6,10.,100.);
        theTriggerEfficiency->Fit(theFunction, "Q0RE");
        
        theFunction->FixParameter(0, theFunction->GetParameter(0));
        theFunction->FixParameter(1, theFunction->GetParameter(1));
        theFunction->FixParameter(2, theFunction->GetParameter(2));
        theFunction->FixParameter(3, theFunction->GetParameter(3));
        theTriggerEfficiency->Fit(theFunction, "Q0R");

        theFunction->ReleaseParameter(0);
        theFunction->ReleaseParameter(1);
        theFunction->FixParameter(4, theFunction->GetParameter(4));
        theFunction->FixParameter(5, theFunction->GetParameter(5));
        theTriggerEfficiency->Fit(theFunction, "Q0R");

        theFunction->ReleaseParameter(4);
        theFunction->ReleaseParameter(5);
        theFunction->SetParLimits(4,1.001,10);
        theFunction->SetParLimits(5,0.001,10.);
        theTriggerEfficiency->Fit(theFunction, "Q0R");


        theFunction->ReleaseParameter(2);
        theFunction->ReleaseParameter(3);
        theFunction->SetParLimits(2,0,1);
        theFunction->SetParLimits(3,0,1);
    }

    theFunction->SetLineColor(kRed);
    TFitResultPtr fitResults = theTriggerEfficiency->Fit(theFunction, "SE0R");
    if(drawPlot) theTriggerEfficiency->Draw("ap");
    theTriggerEfficiency->GetYaxis()->SetRangeUser(0., 1.2);
    theFunction->SetLineWidth(2);
    theFunction->Draw("E3 same");
    theTriggerEfficiency->SetLineColor(kBlack);
    theTriggerEfficiency->SetMarkerColor(kBlack);
    std::cout<< "initialParameters = { " << theFunction->GetParameter(0) << ",  " << theFunction->GetParameter(1) << ",  " << theFunction->GetParameter(2) << ",  " << theFunction->GetParameter(3) << " };"<<std::endl;

    std::string fullFunctionName = plotName + appendToFunctionName;
    outputFile << 
        Form("    TF1* %s = new TF1(\"%s\",\"%s\",%f,%f)",fullFunctionName.data(),fullFunctionName.data(),theFunction->GetExpFormula("P").Data(),xMin,xMax)
        << std::endl;

    return;
}

void doAllFit(std::string inputFileName)
{
    gROOT->SetBatch(true);

    gStyle->SetOptFit();
    gStyle->SetStatY(0.5);
    TFile theInputFile(inputFileName.data());
    std::vector<double> initialParameters;
    std::string crystalBallFunction = "ROOT::Math::crystalball_cdf(x, [5], [4], [1], [0])*[3] + [2]";
    std::string crystalBallAndErrorFunction = "ROOT::Math::crystalball_cdf(x, [5], [4], [1], [0]) * (0.5*(1 + TMath::Erf( (x-[0])/[6]) ) ) *[3] + [2]";

    outputFile << "#include \"Riostream.h\"" << std::endl;
    outputFile << "#include \"TF1.h\"" << std::endl;
    outputFile << std::endl;
    outputFile << "{" << std::endl;

    TCanvas *theCanvasSingleMuonDouble90Quad30Ratio = new TCanvas("SingleMuon_Double90Quad30", "SingleMuon_Double90Quad30", 1400, 800);
    theCanvasSingleMuonDouble90Quad30Ratio->DivideSquare(6,0.005,0.005);
    initialParameters = { 240.731,  77.3069,  0.018671,  0.981187 };
    doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(1),theInputFile, "SingleMuon_Double90Quad30_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 100.,1500.);
    initialParameters = { 31.4358,  15.9878,  2.56566e-10,  0.976469 };
    doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(2),theInputFile, "SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30"           , crystalBallAndErrorFunction, initialParameters,  25., 300.);
    initialParameters = { 91.4671,  23.0038,  3.24185e-13,  0.995492 };
    doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(3),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallAndErrorFunction, initialParameters,  30., 500.);
    // doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(3),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters, 110., 500.);
    // doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(3),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters,  50., 110., false, "Low");
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(4),theInputFile, "SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 26.7507,  12.9058,  8.91746e-08,  0.992066 };
    doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(5),theInputFile, "SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , crystalBallFunction, initialParameters,  25., 300.);
    initialParameters = { 92.9863,  11.7114,  0.26419,  0.734074 };
    doFit(theCanvasSingleMuonDouble90Quad30Ratio->cd(6),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", crystalBallFunction, initialParameters,  70., 500.);
    theCanvasSingleMuonDouble90Quad30Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasSingleMuonDouble90Quad30Ratio->GetName()) + "_Fit.png").data());
    delete theCanvasSingleMuonDouble90Quad30Ratio;

    TCanvas *theCanvasSingleMuonQuad45Ratio = new TCanvas("SingleMuon_Quad45", "SingleMuon_Quad45", 1400, 800);
    theCanvasSingleMuonQuad45Ratio->DivideSquare(4,0.005,0.005);
    initialParameters = { 243.595,  84.877,  1.38778e-15,  0.997779 };
    doFit(theCanvasSingleMuonQuad45Ratio->cd(1),theInputFile, "SingleMuon_Quad45_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 100.,1500.);
    initialParameters = { 47.5867,  16.09,  4.26492e-12,  0.962807 };
    doFit(theCanvasSingleMuonQuad45Ratio->cd(2),theInputFile, "SingleMuon_Quad45_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  30., 300.);
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(theCanvasSingleMuonQuad45Ratio->cd(3),theInputFile, "SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 43.2393,  12.5233,  0.074217,  0.918021 };
    doFit(theCanvasSingleMuonQuad45Ratio->cd(4),theInputFile, "SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  30., 300.);
    theCanvasSingleMuonQuad45Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasSingleMuonQuad45Ratio->GetName()) + "_Fit.png").data());
    delete theCanvasSingleMuonQuad45Ratio;

    TCanvas *theCanvasSingleMuonQuad45AfterDoubleRatio = new TCanvas("SingleMuon_And", "SingleMuon_And", 1400, 800);
    theCanvasSingleMuonQuad45AfterDoubleRatio->DivideSquare(4,0.005,0.005);
    initialParameters = { 121.327,  124.553,  7.53672e-07,  0.999811 };
    doFit(theCanvasSingleMuonQuad45AfterDoubleRatio->cd(1),theInputFile, "SingleMuon_And_Efficiency_L1filterQuad45HT"           , crystalBallAndErrorFunction, initialParameters, 250.,1500.);
    initialParameters = { 44.7711,  16.8627,  1.17955e-09,  0.981758 };
    doFit(theCanvasSingleMuonQuad45AfterDoubleRatio->cd(2),theInputFile, "SingleMuon_And_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  30., 300.);
    // initialParameters = { -499.291,  80,  9.53674e-07,  1 };
    // doFit(theCanvasSingleMuonQuad45AfterDoubleRatio->cd(3),theInputFile, "SingleMuon_And_Efficiency_BTagCaloCSVp087Triple"      , crystalBallFunction, initialParameters,  0.2,   1.);
    initialParameters = { 43.9976,  11.5532,  0.150397,  0.847895 };
    doFit(theCanvasSingleMuonQuad45AfterDoubleRatio->cd(4),theInputFile, "SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  30., 300.);
    theCanvasSingleMuonQuad45AfterDoubleRatio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasSingleMuonQuad45AfterDoubleRatio->GetName()) + "_Fit.png").data());
    delete theCanvasSingleMuonQuad45AfterDoubleRatio;



    TCanvas *theCanvasTTbarDouble90Quad30Ratio = new TCanvas("TTbar_Double90Quad30", "TTbar_Double90Quad30", 1400, 800);
    theCanvasTTbarDouble90Quad30Ratio->DivideSquare(6,0.005,0.005);
    initialParameters = { 240.731,  77.3069,  0.018671,  0.981187 };
    doFit(theCanvasTTbarDouble90Quad30Ratio->cd(1),theInputFile, "TTbar_Double90Quad30_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 100.,1500.);
    initialParameters = { 31.4358,  15.9878,  2.56566e-10,  0.976469 };
    doFit(theCanvasTTbarDouble90Quad30Ratio->cd(2),theInputFile, "TTbar_Double90Quad30_Efficiency_QuadCentralJet30"           , crystalBallAndErrorFunction, initialParameters,  25., 300.);
    initialParameters = { 91.4671,  23.0038,  3.24185e-13,  0.995492 };
    doFit(theCanvasTTbarDouble90Quad30Ratio->cd(3),theInputFile, "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallAndErrorFunction, initialParameters,  50., 500.);
    // doFit(theCanvasTTbarDouble90Quad30Ratio->cd(3),theInputFile, "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters, 110., 500.);
    // doFit(theCanvasTTbarDouble90Quad30Ratio->cd(3),theInputFile, "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters,  50., 110., false, "Low");
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(theCanvasTTbarDouble90Quad30Ratio->cd(4),theInputFile, "TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 26.7507,  12.9058,  8.91746e-08,  0.992066 };
    doFit(theCanvasTTbarDouble90Quad30Ratio->cd(5),theInputFile, "TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , crystalBallFunction, initialParameters,  25., 300.);
    initialParameters = { 92.9863,  11.7114,  0.26419,  0.734074 };
    doFit(theCanvasTTbarDouble90Quad30Ratio->cd(6),theInputFile, "TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", crystalBallFunction, initialParameters,  70., 500.);
    theCanvasTTbarDouble90Quad30Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasTTbarDouble90Quad30Ratio->GetName()) + "_Fit.png").data());
    delete theCanvasTTbarDouble90Quad30Ratio;

    TCanvas *theCanvasTTbarQuad45Ratio = new TCanvas("TTbar_Quad45", "TTbar_Quad45", 1400, 800);
    theCanvasTTbarQuad45Ratio->DivideSquare(4,0.005,0.005);
    initialParameters = { 243.595,  84.877,  1.38778e-15,  0.997779 };
    doFit(theCanvasTTbarQuad45Ratio->cd(1),theInputFile, "TTbar_Quad45_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 100.,1500.);
    initialParameters = { 47.5867,  16.09,  4.26492e-12,  0.962807 };
    doFit(theCanvasTTbarQuad45Ratio->cd(2),theInputFile, "TTbar_Quad45_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  30., 300.);
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(theCanvasTTbarQuad45Ratio->cd(3),theInputFile, "TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 43.2393,  12.5233,  0.074217,  0.918021 };
    doFit(theCanvasTTbarQuad45Ratio->cd(4),theInputFile, "TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  30., 300.);
    theCanvasTTbarQuad45Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasTTbarQuad45Ratio->GetName()) + "_Fit.png").data());
    delete theCanvasTTbarQuad45Ratio;

    TCanvas *theCanvasTTbarQuad45AfterDoubleRatio = new TCanvas("TTbar_And", "TTbar_And", 1400, 800);
    theCanvasTTbarQuad45AfterDoubleRatio->DivideSquare(4,0.005,0.005);
    initialParameters = { 121.327,  124.553,  7.53672e-07,  0.999811 };
    doFit(theCanvasTTbarQuad45AfterDoubleRatio->cd(1),theInputFile, "TTbar_And_Efficiency_L1filterQuad45HT"           , crystalBallAndErrorFunction, initialParameters, 250.,1500.);
    initialParameters = { 44.7711,  16.8627,  1.17955e-09,  0.981758 };
    doFit(theCanvasTTbarQuad45AfterDoubleRatio->cd(2),theInputFile, "TTbar_And_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  30., 300.);
    // initialParameters = { -499.291,  80,  9.53674e-07,  1 };
    // doFit(theCanvasTTbarQuad45AfterDoubleRatio->cd(3),theInputFile, "TTbar_And_Efficiency_BTagCaloCSVp087Triple"      , crystalBallFunction, initialParameters,  0.3,   1.);
    initialParameters = { 43.9976,  11.5532,  0.150397,  0.847895 };
    doFit(theCanvasTTbarQuad45AfterDoubleRatio->cd(4),theInputFile, "TTbar_And_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  30., 300.);
    theCanvasTTbarQuad45AfterDoubleRatio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasTTbarQuad45AfterDoubleRatio->GetName()) + "_Fit.png").data());
    delete theCanvasTTbarQuad45AfterDoubleRatio;

    outputFile << std::endl;
    outputFile << "}" << std::endl;

    gROOT->SetBatch(false);
}

