#include "Jet.h"
#include <iostream>
#include "BuildP4.h"

void Jet::buildP4(){
    p4_.BUILDP4(Jet, nat_);
}

void Jet::buildP4Regressed()
{
	if(p4_.Pt()==0.) this->buildP4();

    float bRegCorrection = get_property((*this),Jet_bRegCorr);
	p4Regressed_.SetPtEtaPhiM(
            p4_.Pt()*bRegCorrection,
            p4_.Eta(),
            p4_.Phi(),
            p4_.M()*bRegCorrection
        );
}

