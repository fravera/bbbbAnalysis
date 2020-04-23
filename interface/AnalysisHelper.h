/*
    ** class: AnalysisHelper
    ** author: L. Cadamuro (LLR)
    ** date: 26/05/2016
    ** description: class handling and coordinating all the part of the histo fillling analysis step.
                    It is interfaced with the parser to keep all the analysis relevant info
    ** note about cfg organization:
                    information is supposed to be stored in different configs according to the information
                    that must be shared or not between the different channels.
                    Only main cfg is passed to the program, and it must contain the names of the other secondary cfgs
                    Configs handled:
                    - main cfg: specific to the analysis, contains selection set etc..
                    - sample cfg: list of samples and their pahts (supposed to be common)
                    - cut cfg: contains the definition of the selections
*/

#ifndef ANALYSISHELPER_H
#define ANALYSISHELPER_H

#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <iostream>
#include <future>
#include <chrono>

#include <boost/variant.hpp>

#include "CfgParser.h"
#include "Sample.h"
#include "Weight.h"
#include "Selection.h"
#include "ordered_map.h"
#include <mutex>

#include "TCut.h"
#include "TH1F.h"

class AnalysisHelper
{

    public:

        typedef ordered_map <std::string, std::shared_ptr<Sample>> sampleColl; // the type of a collection of samples

        AnalysisHelper(std::string cfgname);
        ~AnalysisHelper();

        CfgParser& mainCfg() {return *mainCfg_;}
        bool readMainInfo(); // all analysis info read here

        std::shared_ptr<Sample> openSample(std::string sampleName);
        void prepareHistos(std::shared_ptr<Sample>& theSample);
        // void prepareHistos();
        
        void readSamples(); // inits the samles
        void readSelections();
        void readVariables();

        void fillHistos();

        void mergeSamples();

        void printSelections(bool printWeights=false, bool printSysts=false);
        void printSamples(bool printWeights=false, bool printSysts=false);

        void saveSampleOutputToFile(std::shared_ptr<Sample>& theSample);
        void closeOutputFile();
        bool hasToBeMerged(std::shared_ptr<Sample>& theSample);

        // void saveOutputsToFile();

        void setVerbosity (int v) {verbosity_ = v;}
        void setSplitting (int idxsplit, int nsplit);
        void dump(int detail=0);

        void setSampleTreeName(std::string name) {sample_tree_name_ = name;}
        void setSampleHistoName(std::string name) {sample_heff_name_ = name;}

    private:

        std::string formHistoName (std::string sample, std::string sel, std::string var, std::string syst);
        std::string formHisto2DName (std::string sample, std::string sel, std::string var1, std::string var2, std::string syst);
        Selection readSingleSelection (std::string name);
        std::vector<std::pair<std::string, std::string> > readWeightSysts(std::string name, std::string section); // read w list of section::name
        void fillHistosSample(Sample& sample, std::promise<void> thePromise);
        void activateBranches(Sample& sample);
        std::pair<std::string, std::string> unpack2DName(std::string packedName);
        std::string pack2DName (std::string name1, std::string name2);
        // void prepareSamplesHistos();
        // void prepareSamples2DHistos();
        void prepareSamplesHistos(std::shared_ptr<Sample>& theSample);
        void prepareSamples2DHistos(std::shared_ptr<Sample>& theSample);
        // std::vector<const Weight*> getWeightsWithSyst (const Selection& sel, const Sample& sample); // pointers to all weights that have at least one syst defined

        // check if a weight value is declared as default
        bool   hasDefaultWeight(std::string weightName);
        double getDefaultWeight(std::string weightName);

        std::unique_ptr<CfgParser> mainCfg_;
        std::unique_ptr<CfgParser> cutCfg_;
        std::unique_ptr<CfgParser> sampleCfg_;
        // FIXME: sytematics cfg?

        uint8_t numberOfThreads_;
        int nsplit_;
        int idxsplit_;

        float lumi_;
        std::string outputFolder_;
        std::string outputFileName_;
        std::string outFileFullName_;
        // parameters configurable for the access of samples
        std::string sample_tree_name_;
        std::string sample_heff_name_;

        // ordered_map<std::string, TCut> selections_;
        std::vector<Selection> selections_;
        std::vector<std::string> variables_;
        std::vector<std::pair<std::string, std::string>> variables2D_;
        
        ordered_map <std::string, std::shared_ptr<Sample>> data_samples_;
        ordered_map <std::string, std::shared_ptr<Sample>> sig_samples_;
        ordered_map <std::string, std::shared_ptr<Sample>> bkg_samples_;
        ordered_map <std::string, std::shared_ptr<Sample>> datadriven_samples_;

        ordered_map <std::string, std::vector<std::string>> sample_merge_list_;

        std::string nominal_name_;
        int verbosity_;
        std::mutex mutex_;
        // TFile* outputFile_;
};

// used to access the variant that stores weights and variables
// and to return always a double to be used for TH1.Fill
class get_variant_as_double : public boost::static_visitor<double>
{
    public:
        double operator()(int& x) const
        {
            return (double) x;
        }

        double operator()(float& x)  const
        {
            return (double) x;
        }

        double operator()(double& x) const
        {
            return x;
        }

        double operator()(bool& x) const
        {
            return (x ? 1.0 : 0.0);
        }
}; 

#endif