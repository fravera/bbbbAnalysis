#include "Riostream.h"
#include "TFile.h"
#include "TH1F.h"
#include "TCanvas.h"

void OverlapPlots(){
	TFile inputFile("2016DataPlots/outPlotter.root");

	std::vector<TH1F*> vectorPlotGraviton;
	std::vector<TH1F*> vectorPlotGravitonKinFit;
	TCanvas *cGraviton = new TCanvas("cGraviton", "canvas", 800, 800);

	vectorPlotGraviton.push_back((TH1F*)inputFile.Get("sig_Graviton_300GeV/selectionLMR_SR/sig_Graviton_300GeV_selectionLMR_SR_HH_m"));	
	vectorPlotGraviton.push_back((TH1F*)inputFile.Get("sig_Graviton_450GeV/selectionLMR_SR/sig_Graviton_450GeV_selectionLMR_SR_HH_m"));	
	vectorPlotGraviton.push_back((TH1F*)inputFile.Get("sig_Graviton_600GeV/selectionLMR_SR/sig_Graviton_600GeV_selectionLMR_SR_HH_m"));	
	vectorPlotGraviton.push_back((TH1F*)inputFile.Get("sig_Graviton_800GeV/selectionLMR_SR/sig_Graviton_800GeV_selectionLMR_SR_HH_m"));	
	vectorPlotGraviton.push_back((TH1F*)inputFile.Get("sig_Graviton_1000GeV/selectionLMR_SR/sig_Graviton_1000GeV_selectionLMR_SR_HH_m"));	
	vectorPlotGraviton.push_back((TH1F*)inputFile.Get("sig_Graviton_1200GeV/selectionLMR_SR/sig_Graviton_1200GeV_selectionLMR_SR_HH_m"));	

	bool first=true;
	for( auto plot : vectorPlotGraviton){
		plot->SetDirectory(0);
		plot->Scale(1./plot->Integral());
		if(first){
			first = false;
			plot->SetAxisRange(200.,1400.);
			plot->SetAxisRange(0.,0.06,"Y");
			plot->SetStats(0);
		}
		plot->Draw("same hist");
	}

	vectorPlotGravitonKinFit.push_back((TH1F*)inputFile.Get("sig_Graviton_300GeV/selectionLMR_SR/sig_Graviton_300GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotGravitonKinFit.push_back((TH1F*)inputFile.Get("sig_Graviton_450GeV/selectionLMR_SR/sig_Graviton_450GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotGravitonKinFit.push_back((TH1F*)inputFile.Get("sig_Graviton_600GeV/selectionLMR_SR/sig_Graviton_600GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotGravitonKinFit.push_back((TH1F*)inputFile.Get("sig_Graviton_800GeV/selectionLMR_SR/sig_Graviton_800GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotGravitonKinFit.push_back((TH1F*)inputFile.Get("sig_Graviton_1000GeV/selectionLMR_SR/sig_Graviton_1000GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotGravitonKinFit.push_back((TH1F*)inputFile.Get("sig_Graviton_1200GeV/selectionLMR_SR/sig_Graviton_1200GeV_selectionLMR_SR_HH_m_kinFit"));	

	for( auto plot : vectorPlotGravitonKinFit){
		plot->SetDirectory(0);
		plot->Scale(1./plot->Integral());
		plot->SetLineColor(kRed);
		plot->Draw("same hist");
	}


	std::vector<TH1F*> vectorPlotRadion;
	std::vector<TH1F*> vectorPlotRadionKinFit;
	TCanvas *cRadion = new TCanvas("cRadion", "canvas", 800, 800);


	vectorPlotRadion.push_back((TH1F*)inputFile.Get("sig_Radion_300GeV/selectionLMR_SR/sig_Radion_300GeV_selectionLMR_SR_HH_m"));	
	vectorPlotRadion.push_back((TH1F*)inputFile.Get("sig_Radion_450GeV/selectionLMR_SR/sig_Radion_450GeV_selectionLMR_SR_HH_m"));	
	vectorPlotRadion.push_back((TH1F*)inputFile.Get("sig_Radion_600GeV/selectionLMR_SR/sig_Radion_600GeV_selectionLMR_SR_HH_m"));	
	vectorPlotRadion.push_back((TH1F*)inputFile.Get("sig_Radion_800GeV/selectionLMR_SR/sig_Radion_800GeV_selectionLMR_SR_HH_m"));	
	vectorPlotRadion.push_back((TH1F*)inputFile.Get("sig_Radion_1000GeV/selectionLMR_SR/sig_Radion_1000GeV_selectionLMR_SR_HH_m"));	
	vectorPlotRadion.push_back((TH1F*)inputFile.Get("sig_Radion_1200GeV/selectionLMR_SR/sig_Radion_1200GeV_selectionLMR_SR_HH_m"));	

	first=true;
	for( auto plot : vectorPlotRadion){
		plot->SetDirectory(0);
		plot->Scale(1./plot->Integral());
		if(first){
			first = false;
			plot->SetAxisRange(200.,1400.);
			plot->SetAxisRange(0.,0.06,"Y");
			plot->SetStats(0);
		}
		plot->Draw("same hist");
	}

	vectorPlotRadionKinFit.push_back((TH1F*)inputFile.Get("sig_Radion_300GeV/selectionLMR_SR/sig_Radion_300GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotRadionKinFit.push_back((TH1F*)inputFile.Get("sig_Radion_450GeV/selectionLMR_SR/sig_Radion_450GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotRadionKinFit.push_back((TH1F*)inputFile.Get("sig_Radion_600GeV/selectionLMR_SR/sig_Radion_600GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotRadionKinFit.push_back((TH1F*)inputFile.Get("sig_Radion_800GeV/selectionLMR_SR/sig_Radion_800GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotRadionKinFit.push_back((TH1F*)inputFile.Get("sig_Radion_1000GeV/selectionLMR_SR/sig_Radion_1000GeV_selectionLMR_SR_HH_m_kinFit"));	
	vectorPlotRadionKinFit.push_back((TH1F*)inputFile.Get("sig_Radion_1200GeV/selectionLMR_SR/sig_Radion_1200GeV_selectionLMR_SR_HH_m_kinFit"));	


	for( auto plot : vectorPlotRadionKinFit){
		plot->SetDirectory(0);
		plot->Scale(1./plot->Integral());
		plot->SetLineColor(kRed);
		plot->Draw("same hist");
	}

	inputFile.Close();

}



