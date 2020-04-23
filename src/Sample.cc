#include "Sample.h"
#include <fstream>
#include <assert.h>

using namespace std;

// Sample::Sample (string name, string treename)
// {
//     name_ = name;
//     tree_ = new TChain (treename);
//     filelistname_ = "";
//     tree_ = 0;
//     eff_  = 0.;
//     evt_num_ = 0.;
//     evt_den_ = 0.;
//     nentries_ = 0.;
// }

Sample::Sample (string name, string filelistname, string treename, string histoname)
: datasetCut_("1")
// Sample (name, treename)
{
    name_ = name;
    // tree_ = new TChain (treename.c_str());
    tree_ = new TChain(treename.c_str());
    tree_->CanDeleteRefs();
    filelistname_ = filelistname;
    eff_         = 0.;
    evt_num_     = 0.;
    nentries_    = 0.;
    treename_    = treename;
    histoname_   = histoname;
    hCutInSkim_  = unique_ptr<TH1F>(new TH1F());
    hCutInSkim_->SetNameTitle(name_.data(), name_.data());
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Sample::~Sample ()
{
    clean(true);
}

void Sample::clean(bool cleanPlots)
{
    if(cleanPlots)
    {
        selColl plots_;
        for (uint isel = 0; isel < plots_.size(); ++isel)
        {
            string selectionDir = plots_.key(isel);
            for (uint ivar = 0; ivar < plots_.at(isel).size(); ++ivar)
            {
                for (uint isyst = 0; isyst < plots_.at(isel).at(ivar).size(); ++isyst)
                {
                    delete plots_.at(isel).at(ivar).at(isyst);
                    plots_.at(isel).at(ivar).at(isyst) = nullptr;
                }
            }
        }
        selColl2D plots2D_;
        for (uint isel = 0; isel < plots2D_.size(); ++isel)
        {
            string selectionDir = plots2D_.key(isel);
            for (uint ivar = 0; ivar < plots2D_.at(isel).size(); ++ivar)
            {
                for (uint isyst = 0; isyst < plots2D_.at(isel).at(ivar).size(); ++isyst)
                {
                    delete plots2D_.at(isel).at(ivar).at(isyst);
                    plots2D_.at(isel).at(ivar).at(isyst) = nullptr;
                }
            }
        }
    }
    delete tree_;
    tree_ = nullptr;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Sample::Sample(std::string name, std::vector<Sample const *> sampleList)
// {
//     name_ = name;
    
//     // all the following are set to dummy values
//     tree_ = nullptr;
//     filelistname_ = "";
//     eff_         = 0.;
//     evt_num_     = 0.;
//     evt_den_     = 0.;
//     nentries_    = 0.;
//     bin_eff_den_ = -1;
//     treename_    = "";
//     histoname_   = "";    

//     if (sampleList.size() == 0)
//     {
//         cerr << "*** Sample::Sample (ctor from list of samples) : ERROR : the input list is empty, will create and empty sample" << endl;
//         return;
//     }

//     cout << "@ Creating sample " << name_ << " summing the following samples" << endl;    
//     for (auto s : sampleList)
//         cout << "  ---> " << s->getName() << endl;

//     // clone the structure and histos from sample number 1
// }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool Sample::openFileAndTree(const std::vector<Selection> &selections)
{
    cout << "@ Opening sample: " << name_ << endl;
    cout << "  ---> " << filelistname_ << endl;
    
    ifstream fList (filelistname_);
    if (!fList.good())
    {
        cerr << "*** Sample::openFileAndTree : ERROR : could not open file " << filelistname_ << endl;
        return false;
        // cerr << " ===> Terminating execution" << endl;
        // assert(0);
    }
    string line;
    int counter = 0;
    bool cutHistogramSet=false;
    while (std::getline(fList, line))
    {
        line = line.substr(0, line.find("#", 0)); // remove comments introduced by #
        //cout << line << endl;
        while (line.find(" ") != std::string::npos) line = line.erase(line.find(" "), 1); // remove white spaces
        while (line.find("\n") != std::string::npos) line = line.erase(line.find("\n"), 1); // remove new line characters
        while (line.find("\r") != std::string::npos) line = line.erase(line.find("\r"), 1); // remove carriage return characters
        if (!line.empty()) // skip empty lines
        {
            ++counter;
            tree_->Add(line.c_str());
            
            TFile* f = TFile::Open (line.c_str());
            TH1F* hCutTmp = (TH1F*) f->Get(histoname_.c_str());
            evt_num_  += hCutTmp->GetBinContent ( hCutTmp->GetXaxis()->FindBin("Ntot_uw"          ) ) ;
            // evt_den_  += hCutTmp->GetBinContent ( hCutTmp->GetXaxis()->FindBin(bin_eff_den_.data()) ) ;
            nentries_ += hCutTmp->GetBinContent ( hCutTmp->GetXaxis()->FindBin("Nsel_uw"          ) ) ; // NB! rounding errors could make this different from the actual entries in the tree --> better to use TH1D
            for(int iBin = 1; iBin<=hCutTmp->GetNbinsX(); ++iBin)
            {
                std::string binLabel = hCutTmp->GetXaxis()->GetBinLabel(iBin);
                if(evt_den_map_.find(binLabel)==evt_den_map_.end())
                {
                    evt_den_map_[binLabel] = 0.;
                }
                evt_den_map_[binLabel] += hCutTmp->GetBinContent (iBin);
            }
            if (!cutHistogramSet){
                cutHistogramSet=true;
                initCutHisto(selections);
            }

            hCutInSkim_->Fill(hCutInSkim_->GetBinCenter(hCutInSkim_->GetXaxis()->FindBin("Ntot_w")), hCutTmp->GetBinContent(hCutTmp->GetXaxis()->FindBin("Ntot_w")) );
            hCutInSkim_->Fill(hCutInSkim_->GetBinCenter(hCutInSkim_->GetXaxis()->FindBin("Ntrg_w")), hCutTmp->GetBinContent(hCutTmp->GetXaxis()->FindBin("Ntrg_w")) );
            hCutInSkim_->Fill(hCutInSkim_->GetBinCenter(hCutInSkim_->GetXaxis()->FindBin("Nsel_w")), hCutTmp->GetBinContent(hCutTmp->GetXaxis()->FindBin("Nsel_w")) );

            delete f;
        }
    }
    eff_ = evt_den_map_["Ntot_sel"] / evt_den_map_["Ntot_w"] ;
    cout << "  ---> read " << counter << " files, " << nentries_ << " events" << endl;
    cout << "  ---> efficiency is " << eff_ << "(" << evt_den_map_["Ntot_sel"] << "/" << evt_den_map_["Ntot_w"] << ")" << endl;

    return true;
    // fIn_ = TFile::Open(filename.c_str());
    
    // if (!fIn_)
    // {
    //     cerr << "*** Sample: error: could not open file " << filename << endl;
    //     return;
    // }
    // if (!fIn_->GetListOfKeys()->Contains(treename.c_str()))
    // {
    //     cerr << "*** Sample: error: file does not contain tree " << treename << endl;
    //     return;
    // }
    
    // tree_ = (TTree*) fIn_->Get(treename.c_str());
    // nentries_ = tree_->GetEntries();
    // cout << "  ---> success, tree contains " << nentries_ << " entries" << endl;
}

void Sample::scaleAll(double luminosity)
{
    
    hCutInSkim_->Scale(luminosity/evt_den_map_["Ntot_w"]);

    // 1D
    for (uint isel = 0; isel < plots_.size(); ++isel)
    {
        // cout << "isel " << isel << "/" << plots_.size() << endl;
        for (uint ivar = 0; ivar < plots_.at(isel).size(); ++ivar)
        {
            // cout << "ivar " << ivar << "/" << plots_.at(isel).size() << endl;
            for (uint isyst = 0; isyst < plots_.at(isel).at(ivar).size(); ++isyst)
            {
                // cout << "isyst " << isyst << "/" << plots_.at(isel).at(ivar).size() << endl;
                // cout << " >>>>> : >>>>> scaling histo " << plots_.at(isel).at(ivar).at(isyst)->GetName() << " integral = " << plots_.at(isel).at(ivar).at(isyst)->Integral() << " by " << scale << endl;
                double_t scale;
                if(evt_den_map_.find(plots_.at(isel).at(ivar).key(isyst))!=evt_den_map_.end())
                {
                    scale = luminosity/evt_den_map_[plots_.at(isel).at(ivar).key(isyst)];
                }
                else
                {
                    scale = luminosity/evt_den_map_["Ntot_w"];
                }

                plots_.at(isel).at(ivar).at(isyst)->Scale(scale);
                // cout << "DONE" << endl;
            }
        }
    }

    // 2D
    for (uint isel = 0; isel < plots2D_.size(); ++isel)
    {
        // cout << "isel " << isel << "/" << plots_.size() << endl;
        for (uint ivar = 0; ivar < plots2D_.at(isel).size(); ++ivar)
        {
            // cout << "ivar " << ivar << "/" << plots_.at(isel).size() << endl;
            for (uint isyst = 0; isyst < plots2D_.at(isel).at(ivar).size(); ++isyst)
            {
                // cout << "isyst " << isyst << "/" << plots_.at(isel).at(ivar).size() << endl;
                double_t scale;
                if(evt_den_map_.find(plots_.at(isel).at(ivar).key(isyst))!=evt_den_map_.end())
                {
                    scale = luminosity/evt_den_map_[plots_.at(isel).at(ivar).key(isyst)];
                }
                else
                {
                    scale = luminosity/evt_den_map_["Ntot_w"];
                }
                plots2D_.at(isel).at(ivar).at(isyst)->Scale(scale);
                // cout << "DONE" << endl;
            }
        }
    }
}

void Sample::initCutHisto(const std::vector<Selection> &selections)
{
    int skimNBins = 3;
    int selNBins = selections.size();
    int nBins    = skimNBins + selNBins;
    hCutInSkim_->SetBins(nBins, 0, nBins);
    hCutInSkim_->GetXaxis()->SetBinLabel(1, "Ntot_w" );
    hCutInSkim_->GetXaxis()->SetBinLabel(2, "Ntrg_w" );
    hCutInSkim_->GetXaxis()->SetBinLabel(3, "Nsel_w" );
    for(int xBin=1; xBin<=selNBins; ++ xBin)
    {
        hCutInSkim_->GetXaxis()->SetBinLabel(xBin+skimNBins, selections.at(xBin-1).getName().data() );
    }
    return;
}