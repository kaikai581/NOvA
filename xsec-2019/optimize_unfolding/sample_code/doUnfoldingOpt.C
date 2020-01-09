#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/ISyst.h"
#include "CAFAna/Core/SystShifts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Cuts/TruthCuts.h"
#include "CAFAna/XSec/Flux.h"
#include "CAFAna/Systs/XSecSystLists.h"
#include "CAFAna/Cuts/NueCutsSecondAna.h"
#include "CAFAna/Vars/NueVars.h"
#include "CAFAna/Vars/NueVarsExtra.h"
#include "CAFAna/Vars/GenieWeights.h"
#include "CAFAna/Vars/PPFXWeights.h"
#include "CAFAna/XSec/GenieMultiverseSyst.h"
#include "CAFAna/XSec/FluxMultiverseSyst.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldDagostini.h"
#include "RooUnfoldTUnfold.h"
#include "RooUnfoldErrors.h"
#include "TDecompSVD.h"

#include "NDAna/nuecc_inc/NueCCIncVars.h"
#include "NDAna/nuecc_inc/NueCCIncCuts.h"
#include "NDAna/nuecc_inc/NueCCIncExtra.h"
#include "NDAna/nuecc_inc/NueCCIncTemplateFit.h"
#include "NDAna/nuecc_inc/NueCCIncCrossSectionFunctions.h"

using namespace ana;

const std::string dirName = "/nova/ana/users/mjudah/CrossSection_November2018/";
const std::string file    = "fUnfoldingStudy.root";

const std::vector<double> enuedges_unf{0,1.0,1.25,1.50,1.75,2.0,2.25,
    2.5,2.75,3.0,3.25,3.5,3.75,4,4.5,5,5.5,6,10,100};
const Binning enubins_unf   = Binning::Custom(enuedges_unf);

const std::vector<double> eelecedges_unf{0,0.75,1.0,1.4,1.65,2.0,
    2.5,3.0,3.5,4.1,4.7,6.0,10,100};
const Binning eelecbins_unf = Binning::Custom(eelecedges_unf);

const Binning double_diff_bins_unf = Binning::Simple(eelecbins_unf.NBins()*
						 costhetabins.NBins(),
						 0,
						 eelecbins_unf.NBins()*
						 costhetabins.NBins());

const Binning three_d_bins_unf = Binning::Simple(eelecbins_unf.NBins()*
					     costhetabins.NBins()*
					     enubins_unf.NBins(),
					     0,
					     eelecbins_unf.NBins()*
					     costhetabins.NBins()*
					     enubins_unf.NBins());


const Var kRecoElecEVsCos_unf = Var2D(kvRecoCos,
				      costhetabins,
				      kvElecE, 
				      eelecbins_unf);

const Var kRecoElecEVsCosVsEnu_unf = Var3D(kvRecoCos,
					   costhetabins, 
					   kvElecE, 
					   eelecbins_unf, 
					   kvRecoE,
					   enubins_unf);

const NuTruthVar kTrueElecEVsCosST_unf = Var2D(kTrueElecCosthetaST,
					       costhetabins,
					       kTrueElecEST, 
					       eelecbins_unf);

const NuTruthVar kTrueElecEVsCosVsEnuST_unf = Var3D(kTrueElecCosthetaST,
						    costhetabins,
						    kTrueElecEST, 
						    eelecbins_unf,
						    kTrueEST,
						    enubins_unf);


const mHistAxisDef analysis_vars_unf[5] = {
    {"recoE_2017", {"E_{#nu} (GeV)", enubins_unf, kvRecoE}},
    {"costheta",   {"cos #theta_{e}", costhetabins, kvRecoCos}},
    {"electronE",  {"E_{e} (GeV)",   eelecbins_unf, kvElecE}},
    {"double_diff", {"E_{e} vs cos #theta; Elec_{e} (GeV); cos #{theta}",
		     double_diff_bins_unf, kRecoElecEVsCos_unf}},
     {"triple",     {"",three_d_bins_unf,kRecoElecEVsCosVsEnu_unf}},
  };

  const mHistAxisSTDef analysis_vars_truth_unf[5] = {
    {"st_nu_e", {"True Neutrino Energy (GeV)", enubins_unf, kTrueEST}},
    {"st_costheta_e", {"True cos #theta_{e}", 
		       costhetabins, kTrueElecCosthetaST}},  
    {"st_elec_e", {"True Electron Energy (GeV)",eelecbins_unf, kTrueElecEST}},
    {"st_double_diff", {"True E_{e} vs cos #theta; Elec_{e} (GeV); cos #{theta}", double_diff_bins_unf, kTrueElecEVsCosST_unf}},
    {"st_triple", {"", three_d_bins_unf, kTrueElecEVsCosVsEnuST_unf}},

  };

const double pot = 8.09e20;


void doUnfoldingOpt(std::string dataName = "nominal",
		    std::string varName  = "energy",
		    std::string dimName  = "1d", const int iIter = 100)
{
  TFile* fin = new TFile((dirName+file).c_str(),"read");

  char name[50];
  sprintf(name, "%s_%s_%s_%s","mc", dataName.c_str(), varName.c_str(),
	  "sig");
  Spectrum sReco = *Spectrum::LoadFrom(fin->GetDirectory(name));

  sprintf(name, "%s_%s_%s_%s","mc", dataName.c_str(),(varName+"_truth").c_str(),
	  "sig");  
  Spectrum sTruth = *Spectrum::LoadFrom(fin->GetDirectory(name));




  sprintf(name, "%s_%s_%s_%s","mc", "nominal","unfolding",dimName.c_str());  
  Spectrum sResponse = *Spectrum::LoadFrom(fin->GetDirectory(name));

  TH1F* hReco  = (TH1F*)sReco.ToTH1(pot);
  TH1F* hTruth = (TH1F*)sTruth.ToTH1(pot);
  TH2F* hResponse = (TH2F*)sResponse.ToTH2(pot);

  TCanvas* cResponse = new TCanvas("cResponse","cResponse");
  if(dimName == "1d"){
    hResponse->GetXaxis()->SetRangeUser(1,10.0);
    hResponse->GetYaxis()->SetRangeUser(1,10.0);
  }
  hResponse->GetZaxis()->SetTitle("Signal Events");
  hResponse->GetXaxis()->CenterTitle();
  hResponse->GetYaxis()->CenterTitle();
  hResponse->GetZaxis()->CenterTitle();
  cResponse->SetLeftMargin(0.15);
  cResponse->SetRightMargin(0.15);
  hResponse->Draw("COLZ");
  sprintf(name, "response_%s_%s_%s.png", dataName.c_str(), varName.c_str(),
	  dimName.c_str());
  cResponse->SaveAs(name);


  RooUnfoldResponse ResponseMatrix(0,0,hResponse, "ResponseMatrix");

  //Begin Unfolding
  TH1F* hUnfoldHolder[iIter];

  float ChiSqCov[iIter];
  float AverageCor[iIter];


  float chi_min = 0;
  float chi_max = 0;
  float cor_min = 0;
  float cor_max = 0;

  for(int iter = 0; iter < iIter; ++iter){
    RooUnfoldBayes unfold(&ResponseMatrix,hReco,iter+1);
    TH1F* hunfolded = 
      (TH1F*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);

    hUnfoldHolder[iter] = (TH1F*)hunfolded->Clone(ana::UniqueName().c_str());

    double chi2 = 0;
    if(varName == "energy") 
      chi2 = unfold.Chi2(hTruth, RooUnfold::ErrorTreatment::kCovariance);
    if(varName == "double") 
      chi2 = unfold.Chi2(hTruth, RooUnfold::ErrorTreatment::kNoError);
      
    ChiSqCov[iter] = chi2;

    TMatrixD covMatrix = unfold.Ereco();
    TMatrixD covMatrix2 = unfold.Ereco();

    //Inversion requires the diagonal to be non zero
    //Probably should remove the rows and columns but just setting them to some
    //small but nonzero value for now.
    for(int i = 0; i < hunfolded->GetXaxis()->GetNbins(); i++){
      std::cout << covMatrix[i][i] << std::endl;
      if(covMatrix[i][i] <= 0.001){
	covMatrix[i][i] = 0.05;
	covMatrix2[i][i] = 0.05;
      }
    }


    TMatrixD H_nsquare = covMatrix2;
    TDecompSVD svd(H_nsquare);
    TMatrixD H4 = svd.Invert();

    Double_t det;
    TMatrix   covMatrixInv = (TMatrix)covMatrix2.Invert(&det);

    double rho = 0;
    double dof = 0;
    for(int i = 0; i <hunfolded->GetXaxis()->GetNbins(); i++){
      double matrix_part = 0;
      if(varName == "energy")matrix_part= covMatrix[i][i] * H4[i][i];
      if(varName == "double") matrix_part= covMatrix[i][i] * covMatrixInv[i][i];
      double rho2 = 1 - (1.0/matrix_part);
      if(rho2 < 0) continue;
      dof++;
      rho += sqrt(rho2);
    }
    if(dof < 1) dof = 1;

    AverageCor[iter] = rho/dof;

    if(rho/dof > cor_max) cor_max = rho/dof;
    if(rho/dof < cor_min) cor_min = rho/dof;
    if(chi2 > chi_max) chi_max = chi2;
    if(chi2 < chi_min) chi_min = chi2;
  }//loop over iteractions

  std::cout << cor_min << "\t" << cor_max << std::endl;
  std::cout << chi_min << "\t" << chi_max << std::endl;

  float xArray[iIter];
  for(int i = 0; i < iIter; i++){
    xArray[i] = i+1;
  }

  TGraph *hChiSqCov = new TGraph(iIter, xArray, ChiSqCov);
  TGraph *hAverageCorrelation = new TGraph(iIter, xArray, AverageCor);

  TH1F* hAxes = new TH1F("hAxes",
			 "D'Agostini Unfolding;Number of Iterations;#chi^{2}",
			 iIter,0,iIter);

  for(int i = 0; i < hAxes->GetXaxis()->GetNbins();i++){
    hAxes->SetBinContent(i,0);
  }

  TCanvas *c1 = new TCanvas("c1","c1");
  c1->SetLogy(1);
  hAxes->GetYaxis()->SetRangeUser(1,chi_max);
  hAxes->Draw();
  hChiSqCov->Draw("same");
  sprintf(name, "chi_2_bayes_%s_%s.png", dataName.c_str(), varName.c_str());
  c1->SaveAs(name);

  hAxes->GetYaxis()->SetTitle("Average Correlation");
  
  TCanvas *c2 = new TCanvas("c2","c2");
  hAxes->GetYaxis()->SetRangeUser(cor_min,1.3);
  hAxes->Draw();
  hAverageCorrelation->Draw("same");
  sprintf(name, "cov_bayes_%s_%s.png", dataName.c_str(), varName.c_str());
  c2->SaveAs(name);  

  if(dimName == "1d"){
    TCanvas *c3 = new TCanvas("c3","c3");
    c3->SetLeftMargin(0.10);
    c3->SetRightMargin(0.10);
    TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
    TPad *pad2 = new TPad("pad2","pad2",0,0,1,1);
    pad1->SetFillStyle(0);
    pad2->SetFillStyle(0);
    pad1->SetBottomMargin(0.30);
    pad2->SetTopMargin(1-0.30);
    pad2->SetGridx();
    pad2->SetGridy();
    c3->cd();
    pad1->Draw();
    pad1->cd();
    hTruth->Scale(1,"width");
    hReco->Scale(1,"width");
    hUnfoldHolder[7]->Scale(1,"width");
    hTruth->SetLineColor(kRed);
    hReco->SetLineColor(kBlack);
    hUnfoldHolder[7]->SetLineColor(kBlue);
    hUnfoldHolder[7]->SetMarkerColor(kBlue);
    hUnfoldHolder[7]->SetMarkerStyle(20);
    hReco->GetXaxis()->SetTitle("Neutrino Energy, E_{#nu} (GeV)");
    hReco->GetYaxis()->SetTitle("Events/GeV * 8.09 #times 10^{20} POT");
    hReco->GetXaxis()->CenterTitle();
    hReco->GetYaxis()->CenterTitle();
    hReco->GetXaxis()->SetRangeUser(1.0,10.0);
    hReco->GetYaxis()->SetRangeUser(0,hUnfoldHolder[7]->GetMaximum()*1.3);
    hReco->GetXaxis()->SetLabelColor(kWhite);
    hReco->GetYaxis()->SetLabelSize(0.03);
    hReco->GetYaxis()->SetTitleSize(0.035);
    hReco->Draw("hist");
    hTruth->Draw("hist same");
    hUnfoldHolder[7]->Draw("same");
    TLegend *leg = ana::AutoPlaceLegend(0.3,0.3);
    leg->AddEntry(hUnfoldHolder[7], "Unfolded", "pl");
    leg->AddEntry(hReco, "Reconstructed", "l");
    leg->AddEntry(hTruth, "Truth from Fake Data", "l");
    leg->Draw();
    Simulation();
    c3->cd();
    pad2->Draw();
    pad2->cd();
    hReco->GetXaxis()->SetRangeUser(1.0,10.0);
    hTruth->GetXaxis()->SetRangeUser(1.0,10.0);
    hUnfoldHolder[7]->GetXaxis()->SetRangeUser(1.0,10.0);
    TH1F* hratio = (TH1F*)hReco->Clone(ana::UniqueName().c_str());
    TH1F* hratio2 = (TH1F*)hUnfoldHolder[7]->Clone(ana::UniqueName().c_str());
    hratio->GetYaxis()->SetTitle("Ratio To Truth");
    hratio->GetXaxis()->SetTitle("Neutrino Energy, E_{#nu} (GeV)");
    hratio->GetXaxis()->SetLabelColor(kBlack);
    hratio->GetXaxis()->CenterTitle();
    hratio->Divide(hTruth);
    hratio2->Divide(hTruth);
    hratio->GetYaxis()->SetLabelSize(0.03);
    hratio->GetYaxis()->SetTitleSize(0.035); 
    hratio->GetYaxis()->SetRangeUser(0.4,1.5);
    hratio->Draw("hist same");
    hratio2->Draw("same");
    pad2->Draw("same");
    gPad->RedrawAxis();
    c3->cd();
    c3->Update();
    sprintf(name, "unfolded_%s_%s.png", dataName.c_str(), varName.c_str());
    c3->SaveAs(name);
    delete c3;
  }

  if(dimName == "2d"){

    TCanvas *c3 = new TCanvas("c3","c3");
    c3->SetLeftMargin(0.10);
    c3->SetRightMargin(0.10);
    TPad *pad1 = new TPad("pad1","pad1",0,0,1,1);
    TPad *pad2 = new TPad("pad2","pad2",0,0,1,1);
    pad1->SetFillStyle(0);
    pad2->SetFillStyle(0);
    pad1->SetBottomMargin(0.30);
    pad2->SetTopMargin(1-0.30);
    pad2->SetGridx();
    pad2->SetGridy();
    c3->cd();
    pad1->Draw();
    pad1->cd();
    hTruth->Scale(1,"width");
    hReco->Scale(1,"width");
    hUnfoldHolder[7]->Scale(1,"width");
    hTruth->SetLineColor(kRed);
    hReco->SetLineColor(kBlack);
    hUnfoldHolder[7]->SetLineColor(kBlue);
    hUnfoldHolder[7]->SetMarkerColor(kBlue);
    hUnfoldHolder[7]->SetMarkerStyle(20);
    hReco->GetXaxis()->SetTitle("Electron Kinematic Bins");
    hReco->GetYaxis()->SetTitle("Events/GeV*cos#theta*8.09 #times 10^{20} POT");
    hReco->GetXaxis()->SetLabelColor(kWhite);
    hReco->GetYaxis()->SetLabelSize(0.03);
    hReco->GetYaxis()->SetTitleSize(0.035);
    hReco->GetXaxis()->CenterTitle();
    hReco->GetYaxis()->CenterTitle();
    hReco->GetYaxis()->SetRangeUser(0,hUnfoldHolder[7]->GetMaximum()*1.3);
    hReco->Draw("hist");
    hTruth->Draw("hist same");
    hUnfoldHolder[7]->Draw("same");
    TLegend *leg = ana::AutoPlaceLegend(0.3,0.3);
    leg->AddEntry(hUnfoldHolder[7], "Unfolded", "pl");
    leg->AddEntry(hReco, "Reconstructed", "l");
    leg->AddEntry(hTruth, "Truth from Fake Data", "l");
    leg->Draw();
    Simulation();
    c3->cd();
    pad2->Draw();
    pad2->cd();
    TH1F* hratio = (TH1F*)hReco->Clone(ana::UniqueName().c_str());
    TH1F* hratio2 = (TH1F*)hUnfoldHolder[7]->Clone(ana::UniqueName().c_str());
    hratio->GetYaxis()->SetTitle("Ratio To Truth");
    hratio->GetXaxis()->SetTitle("Electron Kinematic Bins");
    hratio->GetXaxis()->SetLabelColor(kBlack);
    hratio->GetXaxis()->CenterTitle();
    hratio->Divide(hTruth);
    hratio2->Divide(hTruth);
    hratio->GetYaxis()->SetLabelSize(0.03);
    hratio->GetYaxis()->SetTitleSize(0.035); 
    hratio->GetYaxis()->SetRangeUser(0.4,1.5);
    hratio->Draw("hist same");
    hratio2->Draw("same");
    pad2->Draw("same");
    gPad->RedrawAxis();
    c3->cd();
    c3->Update();
    sprintf(name, "unfolded_%s_%s.png", dataName.c_str(), varName.c_str());
    c3->SaveAs(name);
    delete c3;

  }

				   
				   



  



  fin->Close();


}
