#ifndef HH4B_KINFIT_H
#define HH4B_KINFIT_H

#include "TLorentzVector.h"
#include <iostream>
#include "TF1.h"
#include "TFile.h"

class HH4b_kinFit 
{

  public: 
    HH4b_kinFit(std::string inputFileName)
    {
      TFile resolutionFile(inputFileName.data());
      fPtResolutionFunction_barrel  = (TF1*)resolutionFile.Get("fitFuntionPt_barrel" );
      fEtaResolutionFunction_barrel = (TF1*)resolutionFile.Get("fitFuntionEta_barrel");
      fPhiResolutionFunction_barrel = (TF1*)resolutionFile.Get("fitFuntionPhi_barrel");
      fPtResolutionFunction_endcap  = (TF1*)resolutionFile.Get("fitFuntionPt_endcap" );
      fEtaResolutionFunction_endcap = (TF1*)resolutionFile.Get("fitFuntionEta_endcap");
      fPhiResolutionFunction_endcap = (TF1*)resolutionFile.Get("fitFuntionPhi_endcap");
      resolutionFile.Close();
    }

    double constrainHH_signalMeasurement(TLorentzVector *j1, TLorentzVector *j2, TLorentzVector *j3, TLorentzVector *j4, double mass1=125, double mass2=125);

  // double ErrPt_Signal(double Et, double eta);
  // double ErrEta_Signal(double Et);
  // double ErrPhi_Signal(double Et);

  private:
    TF1* fPtResolutionFunction_barrel  {nullptr}; 
    TF1* fEtaResolutionFunction_barrel {nullptr}; 
    TF1* fPhiResolutionFunction_barrel {nullptr}; 
    TF1* fPtResolutionFunction_endcap  {nullptr}; 
    TF1* fEtaResolutionFunction_endcap {nullptr}; 
    TF1* fPhiResolutionFunction_endcap {nullptr}; 
    inline double ErrPt_Signal (double pt, double eta)
    {
      if(eta <= 1.4) return pow(fPtResolutionFunction_barrel->Eval(pt),2);
      else           return pow(fPtResolutionFunction_endcap->Eval(pt),2);
    }
    inline double ErrEta_Signal(double pt, double eta)
    {
      if(eta <= 1.4) return pow(fEtaResolutionFunction_barrel->Eval(pt),2);
      else           return pow(fEtaResolutionFunction_endcap->Eval(pt),2);
    }
    inline double ErrPhi_Signal(double pt, double eta)
    {
      if(eta <= 1.4) return pow(fPhiResolutionFunction_barrel->Eval(pt),2);
      else           return pow(fPhiResolutionFunction_endcap->Eval(pt),2);
    }

};

#endif