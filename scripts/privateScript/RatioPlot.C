#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TGaxis.h"
#include <string>
#include <TROOT.h>
#include <TStyle.h>


struct AsymErrorPoint
{
    double value {-999.};
    double errUp {-999.};
    double errDown {-999.};

    bool operator<( const AsymErrorPoint& r ) const
    {
        return value < r.value;
    }

};


void RatioEfficiencies(TVirtualPad *theCanvas, TGraphAsymmErrors *dataTriggerEfficiency, TGraphAsymmErrors* mcTriggerEfficiency, std::string xAxis = "", std::string yAxis = "", std::string title = "")
{

    // Upper plot will be in pad1
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.35, 1, 1.0);
    pad1->SetLeftMargin(0.12);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    mcTriggerEfficiency->SetTitle(title.data()); // Remove the ratio title
    mcTriggerEfficiency->Draw("apl");               // Draw mcTriggerEfficiency
    mcTriggerEfficiency->GetYaxis()->SetTitle(yAxis.data()); // Remove the ratio title
    // gROOT->ForceStyle();
    dataTriggerEfficiency->Draw("same");         // Draw dataTriggerEfficiency on top of mcTriggerEfficiency
    if(dataTriggerEfficiency->GetMaximum() > mcTriggerEfficiency->GetMaximum()) mcTriggerEfficiency->SetMaximum(dataTriggerEfficiency->GetMaximum()*1.1);

    // // Define the ratio plot
    // TGraphAsymmErrors *ratio = (TGraphAsymmErrors*)mcTriggerEfficiency->Clone("ratio");
    // ratio->SetLineColor(kBlack);
    // ratio->SetMinimum(0.5);  // Define Y ..
    // ratio->SetMaximum(1.5); // .. range
    // ratio->Divide(dataTriggerEfficiency);
    // ratio->SetMarkerStyle(21);
    // ratio->SetMarkerSize(0.3);
    // ratio->Draw("ep");       // Draw the ratio plot

    // mcTriggerEfficiency settings
    mcTriggerEfficiency->SetLineColor(kBlue+1);
    mcTriggerEfficiency->SetLineWidth(2);

    // Y axis mcTriggerEfficiency plot settings
    mcTriggerEfficiency->GetYaxis()->SetTitle("Efficiency");
    mcTriggerEfficiency->GetYaxis()->SetTitleSize(0.05);
    mcTriggerEfficiency->GetYaxis()->SetTitleFont(62);
    mcTriggerEfficiency->GetYaxis()->SetTitleOffset(0.9);
    mcTriggerEfficiency->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    mcTriggerEfficiency->GetYaxis()->SetLabelSize(0.06);
    


    // dataTriggerEfficiency settings
    dataTriggerEfficiency->SetLineColor(kRed);
    dataTriggerEfficiency->SetLineWidth(2);


    // lower plot will be in pad
    theCanvas->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0., 1, 0.35);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->SetGridy(); // horizontal grid
    pad2->Draw();
    pad2->SetLeftMargin(0.12);
    pad2->SetBottomMargin(0.3);
    pad2->cd();       // pad2 becomes the current pad


    std::map<AsymErrorPoint, std::pair<AsymErrorPoint,AsymErrorPoint> > dataPoints;
    for(int nPoint = 0; nPoint<dataTriggerEfficiency->GetN(); ++nPoint)
    {
        AsymErrorPoint thePointX;
        AsymErrorPoint thePointY;
        dataTriggerEfficiency->GetPoint(nPoint,thePointX.value,thePointY.value);
        thePointX.errUp   = dataTriggerEfficiency->GetErrorXhigh(nPoint);
        thePointX.errDown = dataTriggerEfficiency->GetErrorXlow (nPoint);
        thePointY.errUp   = dataTriggerEfficiency->GetErrorYhigh(nPoint);
        thePointY.errDown = dataTriggerEfficiency->GetErrorYlow (nPoint);

        std::pair<AsymErrorPoint,AsymErrorPoint> thePair = std::make_pair(thePointY,AsymErrorPoint());

        dataPoints[thePointX] = thePair;
    }

    for(int nPoint = 0; nPoint<mcTriggerEfficiency->GetN(); ++nPoint)
    {
        AsymErrorPoint thePointX;
        AsymErrorPoint thePointY;
        mcTriggerEfficiency->GetPoint(nPoint,thePointX.value,thePointY.value);
        thePointX.errUp   = mcTriggerEfficiency->GetErrorXhigh(nPoint);
        thePointX.errDown = mcTriggerEfficiency->GetErrorXlow (nPoint);
        thePointY.errUp   = mcTriggerEfficiency->GetErrorYhigh(nPoint);
        thePointY.errDown = mcTriggerEfficiency->GetErrorYlow (nPoint);

        if(dataPoints.find(thePointX) != dataPoints.end()) dataPoints[thePointX].second = thePointY;
    }
 
    std::vector<AsymErrorPoint> keyToBeErased;
    for(auto &point : dataPoints)
    {
        if(point.second.second.value == -999. || point.second.first.value == 0. || point.second.second.value == 0.) keyToBeErased.push_back(point.first);
    }

    for(auto & key : keyToBeErased) dataPoints.erase(key);

    std::vector<double> pointX(dataPoints.size());
    std::vector<double> pointErrXup(dataPoints.size());
    std::vector<double> pointErrXdown(dataPoints.size());
    std::vector<double> pointY(dataPoints.size());
    std::vector<double> pointErrYup(dataPoints.size());
    std::vector<double> pointErrYdown(dataPoints.size());

    int position = 0;
    for(auto &point : dataPoints)
    {
        pointX[position] = (point.first.value);
        pointErrXup[position] = (point.first.errUp);
        pointErrXdown[position] = (point.first.errDown);
        pointY[position] = (point.second.first.value / point.second.second.value);
        pointErrYup  [position] = (point.second.first.value / point.second.second.value * 
                            sqrt( (point.second.first.errUp/point.second.first.value * point.second.first.errUp/point.second.first.value)
                                + (point.second.second.errDown/point.second.second.value * point.second.second.errDown/point.second.second.value) )
                             );
        pointErrYdown[position++] = (point.second.first.value / point.second.second.value * 
                            sqrt( (point.second.first.errDown/point.second.first.value * point.second.first.errDown/point.second.first.value)
                                + (point.second.second.errUp/point.second.second.value * point.second.second.errUp/point.second.second.value) )
                             );
        // std::cout << pointX[position-1] << " " <<pointY[position-1] << " " <<pointErrXdown[position-1] << " " <<pointErrXup[position-1] << " " <<pointErrYdown[position-1] << " " <<pointErrYup[position-1] << " " << std::endl;
    }

    TGraphAsymmErrors *g_scaleFactor = new TGraphAsymmErrors(dataPoints.size(),&pointX[0], &pointY[0], &pointErrXdown[0], &pointErrXup[0], &pointErrYdown[0], &pointErrYup[0]);

    g_scaleFactor->GetXaxis()->SetTitle(xAxis.data());
    g_scaleFactor->GetYaxis()->SetTitle("scale factor");
    g_scaleFactor->GetYaxis()->SetRangeUser(0.4, 1.6);
    g_scaleFactor->SetLineColor(kBlack);
    g_scaleFactor->Draw("apl");


    // Ratio plot (ratio) settings
    g_scaleFactor->SetTitle(""); // Remove the ratio title

    // Y axis ratio plot settings
    // ratio->GetYaxis()->SetTitle("ratio");
    g_scaleFactor->GetYaxis()->SetNdivisions(505);
    g_scaleFactor->GetYaxis()->SetTitleSize(0.1);
    g_scaleFactor->GetYaxis()->SetTitleFont(62);
    g_scaleFactor->GetYaxis()->SetTitleOffset(0.5);
    g_scaleFactor->GetYaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    g_scaleFactor->GetYaxis()->SetLabelSize(0.1);

    // X axis ratio plot settings
    g_scaleFactor->GetXaxis()->SetTitle(xAxis.data());
    g_scaleFactor->GetXaxis()->SetTitleSize(0.15);
    g_scaleFactor->GetXaxis()->SetTitleFont(62);
    g_scaleFactor->GetXaxis()->SetTitleOffset(0.85);
    g_scaleFactor->GetXaxis()->SetLabelFont(62); // Absolute font size in pixel (precision 3)
    g_scaleFactor->GetXaxis()->SetLabelSize(0.09);

    // theCanvas->SaveAs((std::string(dataTriggerEfficiency->GetName()) + ".png").data());


    theCanvas->cd();

}

void RatioEfficienciesFromFile(TVirtualPad *theCanvas, std::string referenceFileName, std::string dataTriggerEfficiencyName, std::string inputFileName, std::string mcTriggerEfficiencyName, std::string xAxis = "", std::string yAxis = "") 
{

    TFile referenceFile(referenceFileName.data());
    TGraphAsymmErrors *dataTriggerEfficiency = (TGraphAsymmErrors*)referenceFile.Get(dataTriggerEfficiencyName.data());
    if(dataTriggerEfficiency == NULL)
    {
        std::cerr<<"dataTriggerEfficiency does not exist\n";
        return;
    }

    TFile inputFile(inputFileName.data());
    TGraphAsymmErrors *mcTriggerEfficiency = (TGraphAsymmErrors*)inputFile.Get(mcTriggerEfficiencyName.data());
    if(mcTriggerEfficiency == NULL)
    {
        std::cerr<<"mcTriggerEfficiency does not exist\n";
        return;
    }

    referenceFile.Close();
    inputFile.Close();
    
    RatioEfficiencies(theCanvas, dataTriggerEfficiency, mcTriggerEfficiency, xAxis, yAxis);
    
    return;
}

void RatioAllEfficienciesInFile(std::string inputFileName)
{
    gROOT->SetBatch(true);

    TCanvas *theCanvasQuad90Double90Ratio = new TCanvas("Quad90Double90Ratio", "Quad90Double90Ratio", 1400, 800);
    theCanvasQuad90Double90Ratio->DivideSquare(6,0.005,0.005);
    RatioEfficienciesFromFile(theCanvasQuad90Double90Ratio->cd(1),inputFileName.data(),"SingleMuon_Double90Quad30_Efficiency_L1filter"                   , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_L1filter"                   , "Sum of four highest pt [GeV]");
    RatioEfficienciesFromFile(theCanvasQuad90Double90Ratio->cd(2),inputFileName.data(),"SingleMuon_Double90Quad30_Efficiency_QuadCentralJet30"           , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_QuadCentralJet30"           , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad90Double90Ratio->cd(3),inputFileName.data(),"SingleMuon_Double90Quad30_Efficiency_DoubleCentralJet90"         , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , "pT_{2} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad90Double90Ratio->cd(4),inputFileName.data(),"SingleMuon_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , "DeepCSV_{1}"                );
    RatioEfficienciesFromFile(theCanvasQuad90Double90Ratio->cd(5),inputFileName.data(),"SingleMuon_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad90Double90Ratio->cd(6),inputFileName.data(),"SingleMuon_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", inputFileName.data(), "TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", "pT_{2} [GeV]"               );
    theCanvasQuad90Double90Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad90Double90Ratio->GetName()) + ".png").data());
    delete theCanvasQuad90Double90Ratio;

    TCanvas *theCanvasQuad45Ratio = new TCanvas("Quad45Ratio", "Quad45Ratio", 1400, 800);
    theCanvasQuad45Ratio->DivideSquare(4,0.005,0.005);
    RatioEfficienciesFromFile(theCanvasQuad45Ratio->cd(1),inputFileName.data(),"SingleMuon_Quad45_Efficiency_L1filter"                 , inputFileName.data(), "TTbar_Quad45_Efficiency_L1filter"                 , "Sum of four highest pt [GeV]");
    RatioEfficienciesFromFile(theCanvasQuad45Ratio->cd(2),inputFileName.data(),"SingleMuon_Quad45_Efficiency_QuadCentralJet45"         , inputFileName.data(), "TTbar_Quad45_Efficiency_QuadCentralJet45"         , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad45Ratio->cd(3),inputFileName.data(),"SingleMuon_Quad45_Efficiency_BTagCaloCSVp087Triple"    , inputFileName.data(), "TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple"    , "DeepCSV_{1}"                );
    RatioEfficienciesFromFile(theCanvasQuad45Ratio->cd(4),inputFileName.data(),"SingleMuon_Quad45_Efficiency_QuadPFCentralJetLooseID45", inputFileName.data(), "TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45", "pT_{4} [GeV]"               );
    theCanvasQuad45Ratio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad45Ratio->GetName()) + ".png").data());
    delete theCanvasQuad45Ratio;


    TCanvas *theCanvasQuad45AfterDoubleRatio = new TCanvas("Quad45AfterDoubleRatio", "Quad45AfterDoubleRatio", 1400, 800);
    theCanvasQuad45AfterDoubleRatio->DivideSquare(4,0.005,0.005);
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatio->cd(1),inputFileName.data(),"SingleMuon_And_Efficiency_L1filterQuad45"           , inputFileName.data(), "TTbar_And_Efficiency_L1filterQuad45"           , "Sum of four highest pt [GeV]");
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatio->cd(2),inputFileName.data(),"SingleMuon_And_Efficiency_QuadCentralJet45"         , inputFileName.data(), "TTbar_And_Efficiency_QuadCentralJet45"         , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatio->cd(3),inputFileName.data(),"SingleMuon_And_Efficiency_BTagCaloCSVp087Triple"    , inputFileName.data(), "TTbar_And_Efficiency_BTagCaloCSVp087Triple"    , "DeepCSV_{1}"                );
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatio->cd(4),inputFileName.data(),"SingleMuon_And_Efficiency_QuadPFCentralJetLooseID45", inputFileName.data(), "TTbar_And_Efficiency_QuadPFCentralJetLooseID45", "pT_{4} [GeV]"               );
    theCanvasQuad45AfterDoubleRatio->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad45AfterDoubleRatio->GetName()) + ".png").data());
    delete theCanvasQuad45AfterDoubleRatio;



    TCanvas *theCanvasQuad90Double90RatioWJets = new TCanvas("Quad90Double90RatioWJets", "Quad90Double90RatioWJets", 1400, 800);
    theCanvasQuad90Double90RatioWJets->DivideSquare(6,0.005,0.005);
    RatioEfficienciesFromFile(theCanvasQuad90Double90RatioWJets->cd(1),inputFileName.data(),"WJetsToLNu_Double90Quad30_Efficiency_L1filter"                   , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_L1filter"                   , "Sum of four highest pt [GeV]");
    RatioEfficienciesFromFile(theCanvasQuad90Double90RatioWJets->cd(2),inputFileName.data(),"WJetsToLNu_Double90Quad30_Efficiency_QuadCentralJet30"           , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_QuadCentralJet30"           , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad90Double90RatioWJets->cd(3),inputFileName.data(),"WJetsToLNu_Double90Quad30_Efficiency_DoubleCentralJet90"         , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_DoubleCentralJet90"         , "pT_{2} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad90Double90RatioWJets->cd(4),inputFileName.data(),"WJetsToLNu_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_BTagCaloCSVp087Triple"      , "DeepCSV_{1}"                );
    RatioEfficienciesFromFile(theCanvasQuad90Double90RatioWJets->cd(5),inputFileName.data(),"WJetsToLNu_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , inputFileName.data(), "TTbar_Double90Quad30_Efficiency_QuadPFCentralJetLooseID30"  , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad90Double90RatioWJets->cd(6),inputFileName.data(),"WJetsToLNu_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", inputFileName.data(), "TTbar_Double90Quad30_Efficiency_DoublePFCentralJetLooseID90", "pT_{2} [GeV]"               );
    theCanvasQuad90Double90RatioWJets->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad90Double90RatioWJets->GetName()) + ".png").data());
    delete theCanvasQuad90Double90RatioWJets;

    TCanvas *theCanvasQuad45RatioWJets = new TCanvas("Quad45RatioWJets", "Quad45RatioWJets", 1400, 800);
    theCanvasQuad45RatioWJets->DivideSquare(4,0.005,0.005);
    RatioEfficienciesFromFile(theCanvasQuad45RatioWJets->cd(1),inputFileName.data(),"WJetsToLNu_Quad45_Efficiency_L1filter"                 , inputFileName.data(), "TTbar_Quad45_Efficiency_L1filter"                 , "Sum of four highest pt [GeV]");
    RatioEfficienciesFromFile(theCanvasQuad45RatioWJets->cd(2),inputFileName.data(),"WJetsToLNu_Quad45_Efficiency_QuadCentralJet45"         , inputFileName.data(), "TTbar_Quad45_Efficiency_QuadCentralJet45"         , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad45RatioWJets->cd(3),inputFileName.data(),"WJetsToLNu_Quad45_Efficiency_BTagCaloCSVp087Triple"    , inputFileName.data(), "TTbar_Quad45_Efficiency_BTagCaloCSVp087Triple"    , "DeepCSV_{1}"                );
    RatioEfficienciesFromFile(theCanvasQuad45RatioWJets->cd(4),inputFileName.data(),"WJetsToLNu_Quad45_Efficiency_QuadPFCentralJetLooseID45", inputFileName.data(), "TTbar_Quad45_Efficiency_QuadPFCentralJetLooseID45", "pT_{4} [GeV]"               );
    theCanvasQuad45RatioWJets->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad45RatioWJets->GetName()) + ".png").data());
    delete theCanvasQuad45RatioWJets;


    TCanvas *theCanvasQuad45AfterDoubleRatioWJets = new TCanvas("Quad45AfterDoubleRatioWJets", "Quad45AfterDoubleRatioWJets", 1400, 800);
    theCanvasQuad45AfterDoubleRatioWJets->DivideSquare(4,0.005,0.005);
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatioWJets->cd(1),inputFileName.data(),"WJetsToLNu_And_Efficiency_L1filterQuad45"           , inputFileName.data(), "TTbar_And_Efficiency_L1filterQuad45"           , "Sum of four highest pt [GeV]");
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatioWJets->cd(2),inputFileName.data(),"WJetsToLNu_And_Efficiency_QuadCentralJet45"         , inputFileName.data(), "TTbar_And_Efficiency_QuadCentralJet45"         , "pT_{4} [GeV]"               );
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatioWJets->cd(3),inputFileName.data(),"WJetsToLNu_And_Efficiency_BTagCaloCSVp087Triple"    , inputFileName.data(), "TTbar_And_Efficiency_BTagCaloCSVp087Triple"    , "DeepCSV_{1}"                );
    RatioEfficienciesFromFile(theCanvasQuad45AfterDoubleRatioWJets->cd(4),inputFileName.data(),"WJetsToLNu_And_Efficiency_QuadPFCentralJetLooseID45", inputFileName.data(), "TTbar_And_Efficiency_QuadPFCentralJetLooseID45", "pT_{4} [GeV]"               );
    theCanvasQuad45AfterDoubleRatioWJets->SaveAs((std::string(inputFileName.substr(0,inputFileName.length()-5) + "_" + theCanvasQuad45AfterDoubleRatioWJets->GetName()) + ".png").data());
    delete theCanvasQuad45AfterDoubleRatioWJets;



    gROOT->SetBatch(false);

}

void RatioAllEfficiencies()
{
    // RatioAllEfficienciesInFile("TriggerEfficiencies_MuonPt40_unMatched.root");
    RatioAllEfficienciesInFile("TriggerEfficiencies_MuonPt30_unMatched.root");

    // RatioAllEfficienciesInFile("TriggerEfficiencies_MuonPt40_matched.root");
    RatioAllEfficienciesInFile("TriggerEfficiencies_MuonPt30_matched.root");

}
