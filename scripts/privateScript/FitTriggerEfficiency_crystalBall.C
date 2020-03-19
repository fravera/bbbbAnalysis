#include "Riostream.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TF1Convolution.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TStyle.h"
#include "TString.h"
#include "TAxis.h"
#include "TVirtualFitter.h"

ofstream outputFile ("fitCurves.h", ios::out);

void doFit(TFile &outputRootFile, TVirtualPad *theCanvas, TFile &theInputFile, std::string&& plotName, std::string fitFunction, std::vector<double> initialParameters , double xMin, double xMax, bool drawPlot = true, std::string appendToFunctionName = "")
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

    theFunction->SetLineColor(kBlack);
    theFunction->SetLineWidth(1);
    // theFunction->SetFillStyle(3002);
    TFitResultPtr fitResults = theTriggerEfficiency->Fit(theFunction, "S0ER");
    if(drawPlot) theTriggerEfficiency->Draw("ap");
    // theTriggerEfficiency->GetFunction("cdf")->Draw("E3 same");
    theTriggerEfficiency->GetYaxis()->SetRangeUser(0., 1.2);
    theTriggerEfficiency->SetLineColor(kBlack);
    theTriggerEfficiency->SetMarkerColor(kBlack);


    //Create a TGraphErrors to hold the confidence intervals
    int nPoints = 1000;
    TGraphErrors *grint = new TGraphErrors(nPoints+1);
    grint->SetTitle("Fitted line with .95 conf. band");
    float histMin = theTriggerEfficiency->GetX()[0] - theTriggerEfficiency->GetErrorXlow(0);
    float histMax = theTriggerEfficiency->GetX()[theTriggerEfficiency->GetN()-1] + theTriggerEfficiency->GetErrorXlow(theTriggerEfficiency->GetN()-1);
    for (int i=0; i<=nPoints; i++)
    {
        // grint->SetPoint(i, histMin + (histMax-histMin)/(float(nPoints)) * float(i) , 0);
        double xValue = histMin + (histMax-histMin)/(double(nPoints)) * double(i);
        double yValue = theFunction->Eval(xValue);
        double yError = 0.;
        // double *stupidXarray      = {xValue};
        // double *stupidErrorXarray = {std::ref(yError)};
        fitResults->GetConfidenceIntervals(1, 1, 1, &xValue, &yError, 0.95);
        grint->SetPoint(i, xValue, yValue);
        grint->SetPointError(i, (histMax-histMin)/(float(nPoints))/2., yError);

        // grint->SetPointError(i,fitResults->GetConfidenceIntervals(1, 1, 1, const double * x, double * ci, double cl, bool norm )

    }

    std::string fullFunctionName = plotName + appendToFunctionName;
    std::string pairObjectName = fullFunctionName + "Pair";
    std::string fitResultName = fullFunctionName + "_FitResult";
    outputRootFile.WriteObject(theTriggerEfficiency, fullFunctionName.data());
    // TFitResult theFitResults(*fitResults.Get());
    outputRootFile.WriteObject(fitResults.Get(), fitResultName.data());
    
    outputFile << "    std::pair<TF1*, KFitResult*> f" << pairObjectName <<" = createPair(" << std::endl;
    outputFile << "        ((TGraphErrors*)triggerFitFile.Get(\"" << fullFunctionName << "\"))->GetFunction(\""<< theFunction->GetName() << "\")," << std::endl;
    outputFile << "        (KFitResult*)triggerFitFile.Get(\"" << fitResultName << "\")"<< std::endl;
    outputFile << "    );" << std::endl;
    outputFile << std::endl;

    
    grint->SetFillColor(kRed);
    grint->SetFillStyle(3001);
    grint->Draw("E3 same");
    theFunction->Draw("same");

    // std::cout<< "initialParameters = { " << theFunction->GetParameter(0) << ",  " << theFunction->GetParameter(1) << ",  " << theFunction->GetParameter(2) << ",  " << theFunction->GetParameter(3) << " };"<<std::endl;

    // outputFile << 
    //     Form("    TF1* %s = new TF1(\"%s\",\"%s\",%f,%f)",fullFunctionName.data(),fullFunctionName.data(),theFunction->GetExpFormula("P").Data(),xMin,xMax)
    //     << std::endl;

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

    std::string outputFileName = "data/" + inputFileName.substr(0, inputFileName.length() -5) + "_fitResults.root";
    TFile outputRootFile(outputFileName.data(), "RECREATE");

    outputFile << "#include \"TFile.h\""                                  << std::endl;
    outputFile << "#include \"TF1.h\""                                    << std::endl;
    outputFile << "#include \"TFitResult.h\""                             << std::endl;
    outputFile << "#include \"TGraphErrors.h\""                           << std::endl;
    outputFile << "#include \"Math/WrappedMultiTF1.h\""                   << std::endl;
    outputFile                                                            << std::endl;
    outputFile << "namespace TriggerFitCurves2016\n{"                     << std::endl;
    outputFile << "    TFile triggerFitFile(\""<< outputFileName <<"\");" << std::endl;


    outputFile << "    class KFitResult : public TFitResult"                                                                                                        << std::endl;
    outputFile << "    {"                                                                                                                                           << std::endl;
	outputFile << "    public:"                                                                                                                                     << std::endl;
	outputFile << "        using TFitResult::TFitResult;"                                                                                                           << std::endl;
	outputFile << "        KFitResult* ResetModelFunction(TF1* func){"                                                                                              << std::endl;
	outputFile << "            this->SetModelFunction(std::shared_ptr<IModelFunction>(dynamic_cast<IModelFunction*>(ROOT::Math::WrappedMultiTF1(*func).Clone())));" << std::endl;
	outputFile << "            return this;"                                                                                                                        << std::endl;
	outputFile << "        }"                                                                                                                                       << std::endl;
    outputFile << "    };"                                                                                                                                          << std::endl;

    outputFile << "    std::pair<TF1*,KFitResult*> createPair(TF1* theFunction, KFitResult* theFitResult )" << std::endl;
    outputFile << "    {"                                                                                   << std::endl;
    outputFile << "        theFitResult->ResetModelFunction(theFunction);"                                  << std::endl;
    outputFile << "        return {theFunction, theFitResult};"                                             << std::endl;
    outputFile << "    }"                                                                                   << std::endl;

    outputFile << std::endl;


    TCanvas *theCanvasSingleMuonDouble90Quad30Ratio = new TCanvas("SingleMuon_Double90Quad30", "SingleMuon_Double90Quad30", 1400, 800);
    theCanvasSingleMuonDouble90Quad30Ratio->DivideSquare(6,0.005,0.005);
    initialParameters = { 240.731,  77.3069,  0.018671,  0.981187 };
    doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(1),theInputFile, "SingleMuon_Double90Quad30_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 300.,1500.);
    initialParameters = { 31.4358,  15.9878,  2.56566e-10,  0.976469 };
    doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(2),theInputFile, "SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30"           , crystalBallAndErrorFunction, initialParameters,  30., 300.);
    initialParameters = { 91.4671,  23.0038,  3.24185e-13,  0.995492 };
    doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(3),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallAndErrorFunction, initialParameters,  90., 500.);
    // doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(3),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters, 110., 500.);
    // doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(3),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters,  50., 110., false, "Low");
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(4),theInputFile, "SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 26.7507,  12.9058,  8.91746e-08,  0.992066 };
    doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(5),theInputFile, "SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , crystalBallFunction, initialParameters,  30., 300.);
    initialParameters = { 92.9863,  11.7114,  0.26419,  0.734074 };
    doFit(outputRootFile, theCanvasSingleMuonDouble90Quad30Ratio->cd(6),theInputFile, "SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", crystalBallFunction, initialParameters,  90., 500.);
    theCanvasSingleMuonDouble90Quad30Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasSingleMuonDouble90Quad30Ratio->GetName()) + "_Fit.png").data());
    outputRootFile.WriteObject(theCanvasSingleMuonDouble90Quad30Ratio, theCanvasSingleMuonDouble90Quad30Ratio->GetName());
    delete theCanvasSingleMuonDouble90Quad30Ratio;

    TCanvas *theCanvasSingleMuonQuad45Ratio = new TCanvas("SingleMuon_Quad45", "SingleMuon_Quad45", 1400, 800);
    theCanvasSingleMuonQuad45Ratio->DivideSquare(4,0.005,0.005);
    initialParameters = { 243.595,  84.877,  1.38778e-15,  0.997779 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45Ratio->cd(1),theInputFile, "SingleMuon_Quad45_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 300.,1500.);
    initialParameters = { 47.5867,  16.09,  4.26492e-12,  0.962807 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45Ratio->cd(2),theInputFile, "SingleMuon_Quad45_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  45., 300.);
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45Ratio->cd(3),theInputFile, "SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 43.2393,  12.5233,  0.074217,  0.918021 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45Ratio->cd(4),theInputFile, "SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  45., 300.);
    theCanvasSingleMuonQuad45Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasSingleMuonQuad45Ratio->GetName()) + "_Fit.png").data());
    outputRootFile.WriteObject(theCanvasSingleMuonQuad45Ratio, theCanvasSingleMuonDouble90Quad30Ratio->GetName());
    delete theCanvasSingleMuonQuad45Ratio;

    TCanvas *theCanvasSingleMuonQuad45AfterDoubleRatio = new TCanvas("SingleMuon_And", "SingleMuon_And", 1400, 800);
    theCanvasSingleMuonQuad45AfterDoubleRatio->DivideSquare(4,0.005,0.005);
    initialParameters = { 121.327,  124.553,  7.53672e-07,  0.999811 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45AfterDoubleRatio->cd(1),theInputFile, "SingleMuon_And_Efficiency_L1filterQuad45HT"           , crystalBallAndErrorFunction, initialParameters, 250.,1500.);
    initialParameters = { 44.7711,  16.8627,  1.17955e-09,  0.981758 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45AfterDoubleRatio->cd(2),theInputFile, "SingleMuon_And_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  45., 300.);
    // initialParameters = { -499.291,  80,  9.53674e-07,  1 };
    // doFit(outputRootFile, theCanvasSingleMuonQuad45AfterDoubleRatio->cd(3),theInputFile, "SingleMuon_And_Efficiency_BTagCaloCSVp087Triple"      , crystalBallFunction, initialParameters,  0.2,   1.);
    initialParameters = { 43.9976,  11.5532,  0.150397,  0.847895 };
    doFit(outputRootFile, theCanvasSingleMuonQuad45AfterDoubleRatio->cd(4),theInputFile, "SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  45., 300.);
    theCanvasSingleMuonQuad45AfterDoubleRatio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasSingleMuonQuad45AfterDoubleRatio->GetName()) + "_Fit.png").data());
    outputRootFile.WriteObject(theCanvasSingleMuonQuad45AfterDoubleRatio, theCanvasSingleMuonQuad45AfterDoubleRatio->GetName());
    delete theCanvasSingleMuonQuad45AfterDoubleRatio;



    TCanvas *theCanvasTTbarDouble90Quad30Ratio = new TCanvas("TTbar_Double90Quad30", "TTbar_Double90Quad30", 1400, 800);
    theCanvasTTbarDouble90Quad30Ratio->DivideSquare(6,0.005,0.005);
    initialParameters = { 240.731,  77.3069,  0.018671,  0.981187 };
    doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(1),theInputFile, "TTbar_Double90Quad30_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 300.,1500.);
    initialParameters = { 31.4358,  15.9878,  2.56566e-10,  0.976469 };
    doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(2),theInputFile, "TTbar_Double90Quad30_Efficiency_QuadCentralJet30"           , crystalBallAndErrorFunction, initialParameters,  30., 300.);
    initialParameters = { 91.4671,  23.0038,  3.24185e-13,  0.995492 };
    doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(3),theInputFile, "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallAndErrorFunction, initialParameters,  90., 500.);
    // doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(3),theInputFile, "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters, 110., 500.);
    // doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(3),theInputFile, "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , crystalBallFunction, initialParameters,  50., 110., false, "Low");
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(4),theInputFile, "TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 26.7507,  12.9058,  8.91746e-08,  0.992066 };
    doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(5),theInputFile, "TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , crystalBallFunction, initialParameters,  30., 300.);
    initialParameters = { 92.9863,  11.7114,  0.26419,  0.734074 };
    doFit(outputRootFile, theCanvasTTbarDouble90Quad30Ratio->cd(6),theInputFile, "TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", crystalBallFunction, initialParameters,  90., 500.);
    theCanvasTTbarDouble90Quad30Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasTTbarDouble90Quad30Ratio->GetName()) + "_Fit.png").data());
    outputRootFile.WriteObject(theCanvasTTbarDouble90Quad30Ratio, theCanvasTTbarDouble90Quad30Ratio->GetName());
    delete theCanvasTTbarDouble90Quad30Ratio;

    TCanvas *theCanvasTTbarQuad45Ratio = new TCanvas("TTbar_Quad45", "TTbar_Quad45", 1400, 800);
    theCanvasTTbarQuad45Ratio->DivideSquare(4,0.005,0.005);
    initialParameters = { 243.595,  84.877,  1.38778e-15,  0.997779 };
    doFit(outputRootFile, theCanvasTTbarQuad45Ratio->cd(1),theInputFile, "TTbar_Quad45_Efficiency_L1filterHT"                 , crystalBallAndErrorFunction, initialParameters, 300.,1500.);
    initialParameters = { 47.5867,  16.09,  4.26492e-12,  0.962807 };
    doFit(outputRootFile, theCanvasTTbarQuad45Ratio->cd(2),theInputFile, "TTbar_Quad45_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  45., 300.);
    initialParameters = { 0.209598,  0.765024,  1.21809e-09,  1.05882 };
    doFit(outputRootFile, theCanvasTTbarQuad45Ratio->cd(3),theInputFile, "TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple"      , "pol3", initialParameters,  0.2,   1.);
    initialParameters = { 43.2393,  12.5233,  0.074217,  0.918021 };
    doFit(outputRootFile, theCanvasTTbarQuad45Ratio->cd(4),theInputFile, "TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  45., 300.);
    theCanvasTTbarQuad45Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasTTbarQuad45Ratio->GetName()) + "_Fit.png").data());
    outputRootFile.WriteObject(theCanvasTTbarQuad45Ratio, theCanvasTTbarQuad45Ratio->GetName());
    delete theCanvasTTbarQuad45Ratio;

    TCanvas *theCanvasTTbarQuad45AfterDoubleRatio = new TCanvas("TTbar_And", "TTbar_And", 1400, 800);
    theCanvasTTbarQuad45AfterDoubleRatio->DivideSquare(4,0.005,0.005);
    initialParameters = { 121.327,  124.553,  7.53672e-07,  0.999811 };
    doFit(outputRootFile, theCanvasTTbarQuad45AfterDoubleRatio->cd(1),theInputFile, "TTbar_And_Efficiency_L1filterQuad45HT"           , crystalBallAndErrorFunction, initialParameters, 250.,1500.);
    initialParameters = { 44.7711,  16.8627,  1.17955e-09,  0.981758 };
    doFit(outputRootFile, theCanvasTTbarQuad45AfterDoubleRatio->cd(2),theInputFile, "TTbar_And_Efficiency_QuadCentralJet45"           , crystalBallAndErrorFunction, initialParameters,  45., 300.);
    // initialParameters = { -499.291,  80,  9.53674e-07,  1 };
    // doFit(outputRootFile, theCanvasTTbarQuad45AfterDoubleRatio->cd(3),theInputFile, "TTbar_And_Efficiency_BTagCaloCSVp087Triple"      , crystalBallFunction, initialParameters,  0.3,   1.);
    initialParameters = { 43.9976,  11.5532,  0.150397,  0.847895 };
    doFit(outputRootFile, theCanvasTTbarQuad45AfterDoubleRatio->cd(4),theInputFile, "TTbar_And_Efficiency_QuadPFCentralJetLooseID45"  , crystalBallFunction, initialParameters,  45., 300.);
    theCanvasTTbarQuad45AfterDoubleRatio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasTTbarQuad45AfterDoubleRatio->GetName()) + "_Fit.png").data());
    outputRootFile.WriteObject(theCanvasTTbarQuad45AfterDoubleRatio, theCanvasTTbarQuad45AfterDoubleRatio->GetName());
    delete theCanvasTTbarQuad45AfterDoubleRatio;

    outputFile << std::endl;
    outputFile << "};" << std::endl;

    outputRootFile.Close();

    gROOT->SetBatch(false);
}

