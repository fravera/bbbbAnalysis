#ifndef PHOTON_H
#define PHOTON_H

#include "Candidate.h"

class Photon : public Candidate
{
    public:
        Photon () : Candidate(){typeId_=13;}
        Photon (int idx, NanoAODTree* nat) : Candidate(idx, nat){typeId_=13; buildP4();}
        ~Photon(){};
        std::unique_ptr<Candidate> clone() const override{
        	Photon *clonedPhoton = new Photon(this->getIdx(), this->getNanoAODTree());
        	clonedPhoton->setP4(this->P4());
        	return std::unique_ptr<Photon> (clonedPhoton);
        }
    private:
        void buildP4() override; 
};

#endif