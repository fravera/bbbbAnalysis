#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"

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


TGraphAsymmErrors* TriggerEfficiency(TFile *inputFile, std::string sampleName, std::string referenceTriggerName, std::string histogramName, 
    std::string selectionName, std::string normalizationSelectionName)
{
    std::string cutTmpName = Form("%s/%s_%s/%s_%s_%s_%s",sampleName.data(),selectionName.data(),referenceTriggerName.data(),sampleName.data(),selectionName.data(),referenceTriggerName.data(),histogramName.data());
    std::cout<<cutTmpName<<std::endl;
    TH1D *cutTmp    = (TH1D*)inputFile->Get(cutTmpName.data());
    TH1D *cutHisto  = (TH1D*)cutTmp->Clone(("cut "+selectionName).data());
    cutHisto->SetDirectory(0);

    std::string normalizationTmpName = Form("%s/%s_%s/%s_%s_%s_%s",sampleName.data(),normalizationSelectionName.data(),referenceTriggerName.data(), sampleName.data(),normalizationSelectionName.data(),referenceTriggerName.data(), histogramName.data());
    // std::cout<<normalizationTmpName<<std::endl;
    TH1D *normalizationTmp    = (TH1D*)inputFile->Get(normalizationTmpName.data());
    TH1D *normalizationHisto  = (TH1D*)normalizationTmp->Clone(("Normalization "+selectionName).data());
    normalizationHisto->SetDirectory(0);
    
    TGraphAsymmErrors *g_efficiency = new TGraphAsymmErrors();
    g_efficiency->Divide(cutHisto,normalizationHisto,"cl=0.683 b(1,1) mode");
    
    return g_efficiency;
}

TGraphAsymmErrors* TriggerScaleFactor(TFile *dataInputFile, std::string dataSampleName, TFile *mcInputFile, std::string mcSampleName, std::string referenceTriggerName, std::string histogramName, 
    std::string selectionName, std::string normalizationSelectionName, TF1 *fittingFunction,
    std::string histogramTitle, std::string xAxisTitle, Color_t lineColor)
{

    TGraphAsymmErrors *dataTriggerEfficiency = TriggerEfficiency(dataInputFile, dataSampleName, referenceTriggerName, histogramName, selectionName, normalizationSelectionName);
    TGraphAsymmErrors *mcTriggerEfficiency   = TriggerEfficiency(mcInputFile  , mcSampleName  , referenceTriggerName, histogramName, selectionName, normalizationSelectionName);

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

    g_scaleFactor->GetXaxis()->SetTitle(xAxisTitle.data());
    g_scaleFactor->GetYaxis()->SetTitle("scale factor");
    g_scaleFactor->SetName(("ScaleFactor_"+selectionName+"_"+histogramName).data());
    g_scaleFactor->SetTitle(("Scale factor "+selectionName+"_"+histogramName).data());
    g_scaleFactor->GetYaxis()->SetRangeUser(0.2, 1.5);
    g_scaleFactor->SetLineColor(lineColor);

    if(fittingFunction != nullptr) g_scaleFactor->Fit(fittingFunction);
   
    return g_scaleFactor;
}

void MeasureTriggerScaleFactor(std::string dataInputFileName, std::string dataSampleName, std::string mcInputFileName, std::string mcSampleName, std::string referenceTriggerName, std::string outputFileName, Color_t lineColor)
{
    TF1 *dummyFormula = nullptr;

    TFile *dataInputFile = new TFile(dataInputFileName.data());
    TFile *mcInputFile = new TFile(mcInputFileName.data());
    TFile *outputFile = new TFile(outputFileName.data(),"RECREATE");
    TGraphAsymmErrors *scaleFactor;


    // HLT_DoubleJet90_Double30_TripleBTagCSV_p087
    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FourHighetJetPtSum", 
        "L1triggerDouble90Double30AndPrevious", "Normalization", dummyFormula,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula,
    //     "Quad Central Jet 30 GeV", "p_{1}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula,
    //     "Quad Central Jet 30 GeV", "p_{2}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula,
    //     "Quad Central Jet 30 GeV", "p_{3}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ForthJetPt", 
        "QuadCentralJet30AndPrevious", "L1triggerDouble90Double30AndPrevious", dummyFormula,
        "Quad Central Jet 30 GeV", "p_{4}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetPt", 
    //     "DoubleCentralJet90AndPrevious", "QuadCentralJet30AndPrevious", dummyFormula,
    //     "Double Central Jet 90 GeV", "p_{1}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "SecondJetPt", 
        "DoubleCentralJet90AndPrevious", "QuadCentralJet30AndPrevious", dummyFormula,
        "Double Central Jet 90 GeV", "p_{2}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetDeepCSV", 
        "BTagCaloCSVp087SingleDouble90Double30AndPrevious", "DoubleCentralJet90AndPrevious", dummyFormula,
        "BTag CaloCSV p087 Triple", "DeepCSV_{1}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula,
    //     "Quad PF Central Jet Loose ID 30 GeV", "p_{1}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula,
    //     "Quad PF Central Jet Loose ID 30 GeV", "p_{2}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula,
    //     "Quad PF Central Jet Loose ID 30 GeV", "p_{3}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ForthJetPt", 
        "QuadPFCentralJetLooseID30AndPrevious", "BTagCaloCSVp087TripleDouble90Double30AndPrevious", dummyFormula,
        "Quad PF Central Jet Loose ID 30 GeV", "p_{4}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetPt", 
    //     "DoublePFCentralJetLooseID90AndPrevious", "QuadPFCentralJetLooseID30AndPrevious", dummyFormula,
    //     "Double PF Central Jet Loose ID 90 GeV", "p_{1}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "SecondJetPt", 
        "DoublePFCentralJetLooseID90AndPrevious", "QuadPFCentralJetLooseID30AndPrevious", dummyFormula,
        "Double PF Central Jet Loose ID 90 GeV", "p_{2}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;



    // _QuadJet45_TripleBTagCSV_p087
    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FourHighetJetPtSum", 
        "L1triggerQuad45AndPrevious", "Normalization", dummyFormula,
        "L1 Trigger", "p_{1}^{T} + p_{2}^{T} + p_{3}^{T} + p_{4}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula,
    //     "Quad Central Jet 45 GeV", "p_{1}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula,
    //     "Quad Central Jet 45 GeV", "p_{2}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula,
    //     "Quad Central Jet 45 GeV", "p_{3}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ForthJetPt", 
        "QuadCentralJet45AndPrevious", "L1triggerQuad45AndPrevious", dummyFormula,
        "Quad Central Jet 45 GeV", "p_{4}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetDeepCSV", 
        "BTagCaloCSVp087SingleQuad45AndPrevious", "QuadCentralJet45AndPrevious", dummyFormula,
        "BTag CaloCSV p087 Triple", "DeepCSV_{1}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "FirstJetPt", 
    //     "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula,
    //     "Quad PF Central Jet Loose ID 45 GeV", "p_{1}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "SecondJetPt", 
    //     "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula,
    //     "Quad PF Central Jet Loose ID 45 GeV", "p_{2}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    // scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ThirdJetPt", 
    //     "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula,
    //     "Quad PF Central Jet Loose ID 45 GeV", "p_{3}^{T}", lineColor);
    // scaleFactor->Write();
    // dummyFormula = nullptr;

    scaleFactor = TriggerScaleFactor(dataInputFile, dataSampleName, mcInputFile, mcSampleName, referenceTriggerName, "ForthJetPt", 
        "QuadPFCentralJetLooseID45AndPrevious", "BTagCaloCSVp087TripleQuad45AndPrevious", dummyFormula,
        "Quad PF Central Jet Loose ID 45 GeV", "p_{4}^{T}", lineColor);
    scaleFactor->Write();
    dummyFormula = nullptr;

    dataInputFile->Close();
    mcInputFile->Close();
    outputFile->Close();

    delete dataInputFile;
    delete mcInputFile;
    delete outputFile;

    return;
}

void ProduceAllScaleFactors()
{
    // MeasureTriggerScaleFactor("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunBF","2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunBF","2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunBF_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunBF","2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunBF_4bSelection_IsoMu24.root",kBlue);

    // MeasureTriggerScaleFactor("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon_RunGH","2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon_RunGH","2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunGH_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_4bSelection/outPlotter.root","SingleMuon_RunGH","2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunGH_4bSelection_IsoMu24.root",kBlue);


    // MeasureTriggerScaleFactor("2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","SingleMuon","2016DataPlots_MuonPt40_ForthJetPt30/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_4bLooseSelection/outPlotter.root","SingleMuon","2016DataPlots_4bLooseSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunBF_4bLooseSelection_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_4bSelection/outPlotter.root","SingleMuon","2016DataPlots_4bSelection/outPlotter.root","TTbar","HLT_IsoMu24","TriggerScaleFactor_RunBF_4bSelection_IsoMu24.root",kBlue);

    MeasureTriggerScaleFactor("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll_2016Analysis/outPlotter.root","SingleMuon_triggerMatched","2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll_2016Analysis/outPlotter.root","TTbar_triggerMatched","HLT_IsoMu24","TriggerScaleFactor_triggerMatched_IsoMu24.root",kBlue);
    // MeasureTriggerScaleFactor("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","SingleMuon_triggerUnMatched","2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll/outPlotter.root","TTbar_triggerUnMatched","HLT_IsoMu24","TriggerScaleFactor_triggerUnMatched_IsoMu24.root",kBlue);
    MeasureTriggerScaleFactor("2016DataPlots_NMSSM_XYH_bbbb_triggerEfficienciesAll_2016Analysis/outPlotter.root","SingleMuon_triggerUnMatched","2016DataPlots_NMSSM_XYH_bbbb_triggerEfficiencies_TTbar/outPlotter.root","TTbar_triggerUnMatched","HLT_IsoMu24","TriggerScaleFactor_triggerUnMatched_IsoMu24.root",kBlue);

}








