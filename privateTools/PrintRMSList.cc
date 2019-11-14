#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TClass.h"
#include "TCollection.h"
#include "TKey.h"
#include "TF1.h"

int main(int argc, char** argv)
{
    if(argc != 4)
    {
        std::cout << "Usage: ./Unroll2Dplots <fileName> <selection> <variable>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string selection   = argv[2];
    std::string variable    = argv[3];

    TFile theInputFile(argv[1]);

    std::cout<< "Printing RMS for variable " << variable << std::endl;

    TIter next(theInputFile.GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next())) 
    {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (!cl->InheritsFrom("TDirectoryFile")) continue;
        std::string theCurrentDataDataset = key->ReadObj()->GetName();
        std::cout << theCurrentDataDataset ;
        theInputFile.cd();
        std::string theCurrentDirectory = theCurrentDataDataset + "/" + selection + "/";
        std::string theCurrentHistogramName = theCurrentDataDataset + "_" + selection + "_"  + variable;
        TH1F *theCurrent2Dplot = (TH1F*)theInputFile.Get((theCurrentDirectory+theCurrentHistogramName).data());
        TF1 theGaus("gaus","gaus");
        theCurrent2Dplot->Fit(&theGaus,"NQ0");
        std::cout<<" - " << theGaus.GetParameter(2) << " +o- " <<  theGaus.GetParError(2) <<std::endl;
    }

    return EXIT_SUCCESS;
}