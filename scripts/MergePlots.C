#include "Riostream.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1.h"
#include "TAttLine.h"
#include "TKey.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TStyle.h"

//don't judge me, I've just discovered variadic templates

std::map<int, TH1* > plotMapList;
int canvasNumber = 0;
std::string canvasNameString = "";
// TFile inputFile("plotOverlap.root");
TFile *inputFile;

// std::pair<int,int> CanvasDivision(int numberOfPlots)
// {
//     int xCanvasDivision = 1;
//     int yCanvasDivision = 1;
//     bool found=kTRUE;
    
//     while(found){
//         if(xCanvasDivision*yCanvasDivision >= numberOfPlots) break;
//         ++yCanvasDivision;
//         if(xCanvasDivision*yCanvasDivision >= numberOfPlots) break;
//         ++xCanvasDivision;
//     }
//     return std::make_pair(xCanvasDivision,yCanvasDivision);    
// }

////Generic function
template<typename ... Plot>
void MergePlot(Plot... thePlotList) {;}

////Generic function pack expansion
template<typename P, typename ... Plot>
void MergePlot(P plotName, Plot... thePlotList)
{
    std::cout << "This Should never be used " << std::endl;
}

//Specific pack expansion for string
template<typename ... Files>
void MergePlot(const char* plotName, Files... thePlotList)
{

    std::cout << plotName << std::endl;

    plotMapList[canvasNumber++] = (TH1*)inputFile->Get(plotName);
    
    MergePlot(thePlotList...);   
}

template<> void MergePlot() 
{
    TCanvas *outputCanvas = new TCanvas("","",8000,5000);

    // std::pair<int,int> canvasDivision = CanvasDivision(plotMapList.size());
    // outputCanvas->Divide(canvasDivision.first, canvasDivision.second);
    outputCanvas->DivideSquare(plotMapList.size());

    int currentCanvas = 1;

    for(auto canvas : plotMapList)
    {
        outputCanvas->cd(currentCanvas++);
        canvas.second->DrawClone();
    }
    
    outputCanvas->SaveAs((canvasNameString + ".pdf").data());
    delete outputCanvas;
    plotMapList.clear();

}

template<typename ... Plot>
void MergePlotFromFile(const char* inputFileName, const char* plotName, Plot... thePlotList)
{
    inputFile = new TFile(inputFileName);
    canvasNameString = plotName;
    std::cout<<plotName<< " "<<canvasNameString<<std::endl;
    canvasNumber = 0;

    MergePlot(thePlotList...);
}

void MergeAll()
{
    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_300_MY_60",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_300_MY_60/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_300_MY_60_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_400_MY_100",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_400_MY_100/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_400_MY_100_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_700_MY_300",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_700_MY_300/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_700_MY_300_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_900_MY_200",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_900_MY_200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_900_MY_200_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_1000_MY_700",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1000_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1000_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_1600_MY_700",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_700/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_700_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_1600_MY_1200",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_1600_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_2000_MY_1200",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1200_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );

    MergePlotFromFile<>("2016DataPlots_NMSSM_XYH_bbbb_MCgenStudies/outPlotter.root", "Plots_NMSSM_bbbb_MX_2000_MY_1800",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_X_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_X_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_PtAndEtaBaseCuts_Full_RecoMatched_X_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_X_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_X_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_PtAndEtaBaseCuts_Full_RecoMatched_X_b_2_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_H_b_1_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_H_b_1_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_PtAndEtaBaseCuts_Full_RecoMatched_H_b_1_deepCSV",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_H_b_2_pt",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/NoCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_NoCuts_Full_RecoMatched_H_b_2_eta",
        "sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800/PtAndEtaBaseCuts_Full/sig_privateMC_NMSSM_bbbb_MX_2000_MY_1800_PtAndEtaBaseCuts_Full_RecoMatched_H_b_2_deepCSV"
    );
}




