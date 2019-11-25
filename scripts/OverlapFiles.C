#include "Riostream.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TAttLine.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"

//don't judge me, I've just discovered variadic templates


std::map<std::string, std::pair<TCanvas*,TLegend*> > canvasMap;
////Generic function
template<typename ... Files>
void OverlapFile(Files... theFileList);

////Generic function pack expansion
template<typename F, typename L, typename C, typename ... Files>
void OverlapFile(F fileName, F legend, C color, Files... theFileList)
{
    std::cout << "This Should never be used " << std::endl;
}

//Specific pack expansion for string
template<typename ... Files>
void OverlapFile(const char* fileName, const char* legendName, EColor color, Files... theFileList)
{

    std::cout << fileName << " " << legendName << " " << color << std::endl;

    TFile inputFile(fileName);
    gROOT->SetBatch(true);

    TIter keyList(inputFile.GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)keyList())) {
        TObject *h = key->ReadObj();
        TH1* histogram = dynamic_cast<TH1*>(h);
        if(histogram!=nullptr) 
        {
            histogram->SetLineColor(color);
            histogram->SetDirectory(0);
        }
        TGraph* graph = dynamic_cast<TGraph*>(h);
        if(graph!=nullptr) graph->SetLineColor(color);
        
        if(canvasMap.find(h->GetName()) == canvasMap.end())
        {
            canvasMap[h->GetName()] = std::pair<TCanvas*,TLegend*>(new TCanvas(h->GetName(),h->GetName()), new TLegend(0.75,0.15,0.95,0.30));
            canvasMap[h->GetName()].first->cd();
            canvasMap[h->GetName()].second->SetFillColor(kWhite);
            h->Draw("");
        }
        else
        {
            canvasMap[h->GetName()].first->cd();
            h->Draw("same");
        }
        canvasMap[h->GetName()].second->AddEntry(h,legendName,"apl");
    }


    inputFile.Close();

    OverlapFile(theFileList...);   
}

template<> void OverlapFile(const char* outputFileName) 
{
    TFile outputFile(outputFileName, "RECREATE");
    for(auto canvas : canvasMap)
    {
        canvas.second.first->cd();
        canvas.second.second->Draw();
        canvas.second.first->Write();
        std::string canvasFileName = canvas.second.first->GetName();
        canvasFileName += ".pdf";
        canvas.second.first->SaveAs(canvasFileName.data());
        delete canvas.second.first;
        delete canvas.second.second;
    }
    outputFile.Close();
    canvasMap.clear();
    gROOT->SetBatch(false);
}

void OverlapAll()
{
    // //SingleMuon_RunBF
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunBF_4bLooseSelection.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunBF_4bSelection.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","2016QS",kOrange,"TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kBlue,"TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24.root","MQS",kGreen,"TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24.root","HQS",kRed,"OverlapTriggerEfficiency_SingleMuon_RunBF.root");

    // //SingleMuon_RunGH
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunGH_4bLooseSelection.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_RunGH_4bSelection.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","2016QS",kOrange,"TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kBlue,"TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24.root","MQS",kGreen,"TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24.root","HQS",kRed,"OverlapTriggerEfficiency_SingleMuon_RunGH.root");

    // //TTbar
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_MuonPt40_ForthJetPt30.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_4bLooseSelection.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_4bSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_4bSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_4bSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_4bSelection.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","2016QS",kOrange,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kBlue,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","MQS",kGreen,"TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","HQS",kRed,"OverlapTriggerEfficiency_TTbar.root");

    // //SingleMuon_RunBF vs TTbar
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","TTbar 2016QS",kMagenta, "TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","TTbar LQS",kCyan, "TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","TTbar MQS",kGreen, "TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","TTbar HQS",kRed, "TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","SingleMuon_RunBF 2016QS",EColor(kMagenta+3), "TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root","SingleMuon_RunBF LQS",kBlue, "TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24.root","SingleMuon_RunBF MQS",EColor(kGreen+3), "TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24.root","SingleMuon_RunBF HQS",EColor(kRed+2),"OverlapTriggerEfficiency_SingleMuon_RunBF_vs_TTbar.root");

    // //SingleMuon_RunGH vs TTbar
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","TTbar 2016QS",kMagenta, "TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","TTbar LQS",kCyan, "TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","TTbar MQS",kGreen, "TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","TTbar HQS",kRed, "TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","SingleMuon_RunGH 2016QS",EColor(kMagenta+3), "TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root","SingleMuon_RunGH LQS",kBlue, "TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24.root","SingleMuon_RunGH MQS",EColor(kGreen+3), "TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24.root","SingleMuon_RunGH HQS",EColor(kRed+2),"OverlapTriggerEfficiency_SingleMuon_RunGH_vs_TTbar.root");

    // //SingleMuon_RunBF vs SingleMuon_RunGH
    // OverlapFile<>("TriggerEfficiency_SingleMuon_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root","SingleMuon_RunBF LQS",kCyan, "TriggerEfficiency_SingleMuon_RunBF_4bLooseSelection_IsoMu24.root","SingleMuon_RunBF MQS",kGreen, "TriggerEfficiency_SingleMuon_RunBF_4bSelection_IsoMu24.root","SingleMuon_RunBF HQS",kRed, "TriggerEfficiency_SingleMuon_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root","SingleMuon_RunGH LQS",kBlue, "TriggerEfficiency_SingleMuon_RunGH_4bLooseSelection_IsoMu24.root","SingleMuon_RunGH MQS",EColor(kGreen+3), "TriggerEfficiency_SingleMuon_RunGH_4bSelection_IsoMu24.root","SingleMuon_RunGH HQS",EColor(kRed+2),"OverlapTriggerEfficiency_SingleMuon_RunBF_vs_SingleMuon_RunGH.root");


    // //ScaleFactors RunBF
    // OverlapFile<>("TriggerScaleFactor_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kRed, "TriggerScaleFactor_RunBF_4bLooseSelection_IsoMu24.root","MQS",kBlue, "TriggerScaleFactor_RunBF_4bSelection_IsoMu24.root","HQS",kGreen,"OverlapTriggerScaleFactor_RunBF.root");
    // //ScaleFactors RunGH
    // OverlapFile<>("TriggerScaleFactor_RunGH_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kRed, "TriggerScaleFactor_RunGH_4bLooseSelection_IsoMu24.root","MQS",kBlue, "TriggerScaleFactor_RunGH_4bSelection_IsoMu24.root","HQS",kGreen,"OverlapTriggerScaleFactor_RunGH.root");



    //SingleMuon_RunBF
    // OverlapFile<>("TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_4bLooseSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_4bLooseSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_4bLooseSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_4bLooseSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_4bLooseSelection.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_4bSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_4bSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_4bSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_4bSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_4bSelection.root");
    // OverlapFile<>("TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","2016QS",kOrange,"TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kBlue,"TriggerEfficiency_SingleMuon_4bLooseSelection_IsoMu24.root","MQS",kGreen,"TriggerEfficiency_SingleMuon_4bSelection_IsoMu24.root","HQS",kRed,"OverlapTriggerEfficiency_SingleMuon.root");

    // //TTbar
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_MuonPt40_ForthJetPt30.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_4bLooseSelection.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_4bSelection_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_4bSelection_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_4bSelection_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_4bSelection.root");
    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","2016QS",kOrange,"TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kBlue,"TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","MQS",kGreen,"TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","HQS",kRed,"OverlapTriggerEfficiency_TTbar.root");


    //SingleMuon
    OverlapFile<>("TriggerEfficiency_SingleMuon_triggerMatched_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_triggerMatched_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_triggerMatched_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_triggerMatched_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_triggerMatched.root");
    OverlapFile<>("TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_SingleMuon_triggerUnMatched.root");

    //TTbar
    OverlapFile<>("TriggerEfficiency_TTbar_triggerMatched_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_triggerMatched_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_triggerMatched_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_triggerMatched_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_triggerMatched.root");
    OverlapFile<>("TriggerEfficiency_TTbar_triggerUnMatched_IsoMu24.root","IsoMu24",kBlue,"TriggerEfficiency_TTbar_triggerUnMatched_IsoMu24_Matched.root","IsoMu24_Matched",kRed,"TriggerEfficiency_TTbar_triggerUnMatched_IsoMu20.root","IsoMu20",kGreen,"TriggerEfficiency_TTbar_triggerUnMatched_IsoMu20_Matched.root","IsoMu20_Matched",kOrange,"OverlapTriggerEfficiency_TTbar_triggerUnMatched.root");

    //SingleMuon vs TTbar
    OverlapFile<>("TriggerEfficiency_TTbar_triggerMatched_IsoMu24.root","TTbar trg matched",kRed, "TriggerEfficiency_TTbar_triggerUnMatched_IsoMu24.root","TTbar trg unmatched",kCyan, "TriggerEfficiency_SingleMuon_triggerMatched_IsoMu24.root","SingleMuon trg matched",EColor(kRed+2), "TriggerEfficiency_SingleMuon_triggerUnMatched_IsoMu24.root","SingleMuon trg unmatched",kBlue,"OverlapTriggerEfficiency_SingleMuon_vs_TTbar.root");

    // OverlapFile<>("TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","TTbar 2016QS",kMagenta, "TriggerEfficiency_TTbar_MuonPt40_ForthJetPt30_IsoMu24.root","TTbar LQS",kCyan, "TriggerEfficiency_TTbar_4bLooseSelection_IsoMu24.root","TTbar MQS",kGreen, "TriggerEfficiency_TTbar_4bSelection_IsoMu24.root","TTbar HQS",kRed, "TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_CMVAM_IsoMu24.root","SingleMuon 2016QS",EColor(kMagenta+3), "TriggerEfficiency_SingleMuon_MuonPt40_ForthJetPt30_IsoMu24.root","SingleMuon LQS",kBlue, "TriggerEfficiency_SingleMuon_4bLooseSelection_IsoMu24.root","SingleMuon MQS",EColor(kGreen+3), "TriggerEfficiency_SingleMuon_4bSelection_IsoMu24.root","SingleMuon HQS",EColor(kRed+2),"OverlapTriggerEfficiency_SingleMuon_vs_TTbar.root");

    // //ScaleFactors RunBF
    // OverlapFile<>("TriggerScaleFactor_RunBF_MuonPt40_ForthJetPt30_IsoMu24.root","LQS",kRed, "TriggerScaleFactor_RunBF_4bLooseSelection_IsoMu24.root","MQS",kBlue, "TriggerScaleFactor_RunBF_4bSelection_IsoMu24.root","HQS",kGreen,"OverlapTriggerScaleFactor.root");


    //ScaleFactors
    OverlapFile<>("TriggerScaleFactor_triggerMatched_IsoMu24.root","trg matched",kRed, "TriggerScaleFactor_triggerUnMatched_IsoMu24.root","trg unmatched",kBlue,"OverlapTriggerScaleFactor.root");

}

