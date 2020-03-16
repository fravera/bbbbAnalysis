#ifndef SKIMEFFCOUNTER_H
#define SKIMEFFCOUNTER_H

/**
** class  : SkimEffCounter
** author : L. Cadamuro (UF)
** date   : 30/01/2018
** brief  : logs the numbner of processed events and the skim efficiency
**/

#include "TH1D.h"
#include <memory>
#include <iostream>
#include <map>

class SkimEffCounter {
    public:

        SkimEffCounter();
        ~SkimEffCounter()
        { 
            //Do not delete eff_histo_, ROOT handles it
        };

        // _w  : weighted
        // _uw : unweighted (plain ev count)
        std::map<std::string,int>   binMap_;
        std::map<std::string,float> binEntries_;
        
        void updateProcessed      (double evtW);
        void updateTriggered      (double evtW);
        void updateSelected       (double evtW);

        void  write();

    private:
        TH1D*  eff_histo_;
        void createHisto();


};


#endif