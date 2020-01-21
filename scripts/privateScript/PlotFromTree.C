#include "Riostream.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCut.h"
#include "TTree.h"
#include "TFile.h"
#include "TTreeFormula.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

void PlotFromTree(std::string inputFileName, std::string inputTreeName, std::string cut , std::string xVariable, int nBins, float xMin, float xMax)
{

    TFile* inputFile = TFile::Open(inputFileName.data());
    if(inputFile == nullptr)
    {
        std::cout << "File " << inputFileName << " does not exist. Aborting..." << std::endl;
        return;
    }

    TTree *inputTree = (TTree*)inputFile->Get(inputTreeName.data());

    TTreeReader theTreeReader(inputTree);
    TTreeReaderValue<float> variableHandler(theTreeReader, xVariable.data());
 

    TH1F *thePlot = new TH1F("thePlot", xVariable.data(), nBins, xMin, xMax);
    thePlot->SetDirectory(0);
    thePlot->Sumw2();
    TTreeFormula theCut("theCut", cut.data(), inputTree);


    for(int it=0; it<inputTree->GetEntries(); ++it)
    {
        inputTree->GetEntry(it);
        theTreeReader.Next();

        if(theCut.EvalInstance())
        {
            float variableValue = *variableHandler.Get();
            // float weightValue   = *weightHandler  .Get();
            thePlot->Fill(variableValue);
        }
    }

    thePlot->Draw();
    inputFile->Close();
    
}