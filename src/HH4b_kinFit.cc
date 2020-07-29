#include "HH4b_kinFit.h"
#include "PhysicsTools/KinFitter/interface/TFitConstraintM.h"
#include "PhysicsTools/KinFitter/interface/TFitParticleEtEtaPhi.h"
#include "PhysicsTools/KinFitter/interface/TKinFitter.h"

// double HH4b_kinFit::ErrPt_Signal(double Et, double eta)
// {
//   double sigmaEt=0;
//   if (fabs(eta)<1.4)
//   {
//     if (Et<40) Et=40;
//     if (Et>550) Et=550;
//     sigmaEt = 22.26 - 0.01*Et + 0.00018*Et*Et;
//   }
//   else
//   {
//     if (Et<40) Et=40;
//     if (Et>350) Et=350;
//     sigmaEt = 17.11 + 0.058 * Et;
//   }
//   return sigmaEt*sigmaEt;
// }

// double HH4b_kinFit::ErrEta_Signal(double Et)
// {
//   if (Et<40) Et=40;
//   if (Et>500) Et=500;
//   double sigmaEta = 0.033 + (4.1/Et) + (-0.17/pow(Et,0.5));
//   return sigmaEta*sigmaEta;
// }

// double HH4b_kinFit::ErrPhi_Signal(double Et)
// {
//   if (Et<40) Et=40;
//   if (Et>500) Et=500;
//   double sigmaPhi = 0.038 + (4.1/Et) + (-0.19/pow(Et,0.5));
//   return sigmaPhi*sigmaPhi;
// }


double HH4b_kinFit::constrainHH_signalMeasurement(TLorentzVector *j1, TLorentzVector *j2, TLorentzVector *j3, TLorentzVector *j4, double mass1, double mass2)
{
  TMatrixD m1(3,3);
  TMatrixD m2(3,3);
  TMatrixD m3(3,3);
  TMatrixD m4(3,3);
  m1.Zero();
  m2.Zero();
  m3.Zero();
  m4.Zero();
  
  m1(0,0) = ErrPt_Signal (j1->Et(), j1->Eta());
  m1(1,1) = ErrEta_Signal(j1->Et(), j1->Eta());
  m1(2,2) = ErrPhi_Signal(j1->Et(), j1->Eta());
  
  m2(0,0) = ErrPt_Signal (j2->Et(), j2->Eta());
  m2(1,1) = ErrEta_Signal(j2->Et(), j2->Eta());
  m2(2,2) = ErrPhi_Signal(j2->Et(), j2->Eta());
  
  TFitParticleEtEtaPhi *jet1 = new TFitParticleEtEtaPhi(j1, &m1);
  TFitParticleEtEtaPhi *jet2 = new TFitParticleEtEtaPhi(j2, &m2);
  TFitConstraintM *massConstraint1 = new TFitConstraintM("massConstraint1", "massConstraint1", 0, 0, mass1);
  massConstraint1->addParticles1(jet1, jet2);
  
  TKinFitter *fitter = new TKinFitter("fitter", "fitter");
  fitter->addMeasParticle(jet1);
  fitter->addMeasParticle(jet2);
  fitter->addConstraint(massConstraint1);

  TFitParticleEtEtaPhi *jet3       = nullptr;
  TFitParticleEtEtaPhi *jet4       = nullptr;
  TFitConstraintM *massConstraint2 = nullptr;
  if(mass2>0)
  {
    m3(0,0) = ErrPt_Signal (j3->Et(), j3->Eta());
    m3(1,1) = ErrEta_Signal(j3->Et(), j3->Eta());
    m3(2,2) = ErrPhi_Signal(j3->Et(), j3->Eta());
    m4(0,0) = ErrPt_Signal (j4->Et(), j4->Eta());
    m4(1,1) = ErrEta_Signal(j4->Et(), j4->Eta());
    m4(2,2) = ErrPhi_Signal(j4->Et(), j4->Eta());
    jet3 = new TFitParticleEtEtaPhi(j3, &m3);
    jet4 = new TFitParticleEtEtaPhi(j4, &m4);
    TFitConstraintM *massConstraint2 = new TFitConstraintM("massConstraint2", "massConstraint2", 0, 0, mass2);
    massConstraint2->addParticles1(jet3, jet4);
    fitter->addMeasParticle(jet3);
    fitter->addMeasParticle(jet4);
    fitter->addConstraint(massConstraint2);
  }

  fitter->setMaxNbIter(30);
  fitter->setMaxDeltaS(1e-2);
  fitter->setMaxF(1e-1);
  fitter->setVerbosity(3);
  
  fitter->fit();
  
  double chi2Normalized=fitter->getS()/fitter->getNDF();
  
  *j1 = TLorentzVector(*(fitter->get4Vec(0)));
  *j2 = TLorentzVector(*(fitter->get4Vec(1)));
  
  delete jet1;
  delete jet2;
  delete massConstraint1;

  if(mass2>0)
  {
    *j3 = TLorentzVector(*(fitter->get4Vec(2)));
    *j4 = TLorentzVector(*(fitter->get4Vec(3)));
    delete jet3;
    delete jet4;
    delete massConstraint2;
  }

  delete fitter;
  
  return chi2Normalized;
}

