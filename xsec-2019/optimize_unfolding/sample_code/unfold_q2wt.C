
#include "OscLib/func/OscCalculator.h"

#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Cuts/NueCutsFirstAna.h"
#include "CAFAna/Core/Utilities.h"
#include "CAFAna/Analysis/Fit.h"
#include "CAFAna/Vars/FitVars.h"
#include "CAFAna/Analysis/Plots.h"
#include "CAFAna/Prediction/PredictionNoExtrap.h"
#include "CAFAna/Experiment/SingleSampleExperiment.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/Ratio.h"
#include "CAFAna/Analysis/Surface.h"
#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Vars/GenieWeights.h"
#include "CAFAna/Unfold/UnfoldIterative.h"
#include "CAFAna/Unfold/UnfoldSVD.h"
#include "CAFAna/Unfold/UnfoldTikhonov.h"
#include "CAFAna/Analysis/Style.h"

#include "NDAna/numucc_inc/NumuCCIncCuts.h"
#include "NDAna/numucc_inc/NumuCCIncAnalysis.h"
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
//#include "Fit.h"
#include "TLine.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>

#include "Utilities/func/MathUtil.h"
//gSystem->Load("/nova/app/users/jpaley/offline/RooUnfold/RooUnfold-master/libRooUnfold");
#include "Utilities/rootlogon.C"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldDagostini.h"
#include "RooUnfoldTUnfold.h"
#include "RooUnfoldErrors.h"

#include "TDecompSVD.h"

using namespace ana;

//
//----------------------------------------------------------------------

void unfold_q2wt()
{


  //  TFile file("/nova/ana/users/bbehera/ThesisAnalysis/Unfolding/fout_unfolding_weight_nov30.root");
  TFile file("/nova/ana/users/bbehera/IncXsec/xsec3D/fxsec3D.root");
  
  double pot = 8.09e20;
  
  //  NumuCCIncAnalysis xsec_E_wtdata  = *NumuCCIncAnalysis :: LoadFrom(file.GetDirectory("xsec_E_wtdata"), ana::kSVD, false, false, 1);
  NumuCCIncAnalysis xsec_E_wtdata  = *NumuCCIncAnalysis :: LoadFrom(file.GetDirectory("xsec_E_wtdata"), ana::kIterative, false, false, 1);

  //NumuCCIncAnalysis xsec_E_wtdata  = *NumuCCIncAnalysis :: LoadFrom(file.GetDirectory("xsec_E_inout"), ana::kIterative, false, false, 1);
  
  ReweightableSpectrum response    = *ReweightableSpectrum::LoadFrom(file.GetDirectory("xsec_E_wtdata/fRecoTrue"));
  //ReweightableSpectrum response    = *ReweightableSpectrum::LoadFrom(file.GetDirectory("xsec_E_inout/fRecoTrue"));

  TH2D* hresponse = response.ToTH2(pot);
  //new TCanvas;
  //hresponse->Draw("colz");
  //  gPad->SaveAs("2dhist.png");
  
  //  Spectrum measured_spec= *Spectrum::LoadFrom(file.GetDirectory("xsec_E_wtdata/fMC"));

  // TH1D* measured        = Spectrum::LoadFrom(file.GetDirectory("xsec_E_wtdata/fMC"))->ToTH1(pot);
  // TH1D* truth           = Spectrum::LoadFrom(file.GetDirectory("xsec_E_wtdata/fMCSig"))->ToTH1(pot);

  Spectrum data_spec    = xsec_E_wtdata.SignalEst();
  TH1D* data            = data_spec.ToTH1(pot);

  TH1D* data_true       = Spectrum::LoadFrom(file.GetDirectory("true_data_E_wt"))->ToTH1(pot);
  //TH1D* data_true       = Spectrum::LoadFrom(file.GetDirectory("true_data_E_nowt"))->ToTH1(pot);

  TH1* truedata_q2_nutree    = Spectrum :: LoadFrom(file.GetDirectory("truedata_q2_nutree"))->ToTH1(pot);
  TH1* truedata_q2_wt_nutree = Spectrum :: LoadFrom(file.GetDirectory("truedata_q2_wt_nutree"))->ToTH1(pot);

  TH1* truedata_E_nutree   = Spectrum :: LoadFrom(file.GetDirectory("truedata_E_nutree"))->ToTH1(pot);
  TH1* truedata_E_wt_nutree= Spectrum :: LoadFrom(file.GetDirectory("truedata_E_wt_nutree"))->ToTH1(pot);

  // ------------------------------------------------------------------------
  int iter = 100;
  // for RooUnfold, iterations in Bayes unfold should be +1 of what's used in CAFAna.
   
  double max = data->GetMaximum();
  if (data_true->GetMaximum() > max ) max = data_true->GetMaximum();

  TH1D* kstest = new TH1D("kstest", "; Iterations; Residual (True Fake Data/Roo-Unfold)", 100, 0.5, 100.5);
  // TCanvas *can   = new TCanvas();
  TH1* hRatio[iter];  
  TH1* hunfold[iter];  
  //double value[iter] = {0.0};
  //double error[iter] = {0.0};

  float fChiSq[iter];
  float fCor[iter];
  //float fBias[iter];
  //float fCoverage[iter];

  float fChiSqErr[iter];
  float fCorErr[iter];
  float fBiasErr[iter];
  float fCoverageErr[iter];


  //TGraph* hChiSqCov;
  //TGraph* hAverageCor;
  //TGraph* hBias;
  //TGraph* hCoverage;

  TString outDir = "plots";
  /*
  // TUnfold --------------------------------------
  RooUnfoldResponse ResponseforTunfold (0, 0, hresponse);
  RooUnfoldTUnfold tunfold (&ResponseforTunfold, data);
  TH1D* unfold_roounfold = (TH1D*) tunfold.Hreco();
  if (unfold_roounfold->GetMaximum() > max ) max = unfold_roounfold->GetMaximum();

  UnfoldTikhonov unfold_t(response, 0.0457425);
  TH1D* unfold_cafana = unfold_t.Truth(data_spec).ToTH1(pot);
  if (unfold_cafana->GetMaximum() > max ) max = unfold_cafana->GetMaximum();
  unfold_cafana->SetTitle("Tikhonov Unfolding");
  unfold_cafana->GetXaxis()->SetTitle("Neutrino Energy (GeV)");

  unfold_cafana->SetLineColor(kGreen+1);
  unfold_cafana->Draw();

  data_true->SetLineColor(kRed);
  data_true->SetMarkerColor(kRed);
  data_true->SetMarkerStyle(20);

  unfold_roounfold->SetMarkerColor(kBlue);
  unfold_roounfold->SetLineColor(kBlue);
  unfold_roounfold->SetMarkerStyle(20);

  TCanvas* ct = new TCanvas("ct","ct");
  ct->cd();
  unfold_cafana->GetYaxis()->CenterTitle();
  unfold_cafana->GetXaxis()->CenterTitle();
  unfold_cafana->Draw();
  data_true->Draw("same hist");
  unfold_roounfold->Draw("pe same");

  TLegend *leg4 = AutoPlaceLegend(0.3, 0.3);
  leg4->AddEntry(data_true,"Truth Fake Data", "l");
  leg4->AddEntry(unfold_roounfold,"Unfolded, RooUnfold", "lp");
  leg4->AddEntry(unfold_cafana,"Unfolded, CAFAna", "l");
  leg4->Draw("same");

  ct->SaveAs(outDir+"/"+"Tikhonov.png");

  //TH1 *data_true_R = (TH1*) data_true->Clone();
  TCanvas* can = new TCanvas("can","can");
  data_true->Divide(unfold_roounfold);
  data_true->Draw();
  data_true->GetYaxis()->CenterTitle();
  data_true->GetXaxis()->CenterTitle();
  //  unfolded_cafana->GetYaxis()->SetRangeUser(0.98,1.02);
  data_true->GetYaxis()->SetTitle("True Fake Data/Roo-Unfold");
  gPad->SaveAs(outDir+"/"+"ratio_Tikhonov.png");
  //  ct->Close();
  //delete ct;
  //-------------------------------------------------------------------------
  */
  TH1F* hUnfoldHolder[iter];
  TH1F* hUnfoldHolderSubtraction[iter];

  float ChiSqCov[iter];
  float AverageCor[iter];
  //  TFile* outfile = new TFile("UnfoldingMetrics.root", "update");

  TFile fout("UnfoldingMetrics_q2wt.root", "RECREATE");
  for(int it = 1; it <= iter; ++it){
    RooUnfoldResponse Response (0, 0, hresponse);
        
    // RooUnfoldSvd    unfold (&Response, data, iter);
    RooUnfoldBayes unfold (&Response, data, it);
    TH1D* unfolded_roounfold = (TH1D*) unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    

    char unfold_name[50];
    sprintf(unfold_name,"%s_%i", "unfolded_hist", it-1);

    hunfold[it-1] = (TH1D*)unfolded_roounfold->Clone(unfold_name);
    if (unfolded_roounfold->GetMaximum() > max ) max = unfolded_roounfold->GetMaximum();
   

    char unfold_name2[50];
    sprintf(unfold_name2,"unfolded_sub_%i",it-1);

    hUnfoldHolder[it-1] = (TH1F*)unfolded_roounfold->Clone(unfold_name);

    hUnfoldHolderSubtraction[it-1] = (TH1F*)hUnfoldHolder[it-1]->Clone(unfold_name2);

    hUnfoldHolderSubtraction[it-1]->Add(data_true, -1);

    double chi2 = unfold.Chi2(data_true, RooUnfold::ErrorTreatment::kErrors);
    ChiSqCov[it-1] = chi2;

    TMatrixD covMatrix  = unfold.Ereco();
    TMatrixD covMatrix2 = unfold.Ereco();

    if(it == 1){
      TCanvas *c1 = new TCanvas("c1","c1");
      c1->SetRightMargin(0.15);
      gStyle->SetPalette(kCherry);
      covMatrix.Draw("COLZ");
      c1->SaveAs("covariance_q2_wt.png");
      Simulation();
    }

    TMatrixD H_nsquare = covMatrix2;
    TDecompSVD svd(H_nsquare);
    TMatrixD H4 = svd.Invert();

    double rho = 0;
    double dof = 0;
    
    for( int i = 1; i < unfolded_roounfold->GetXaxis()->GetNbins(); ++i){

      std::cout << "*********For Bin:******** " 
		<< "\t" 
		<< i << "\t" 
		<< "=" << "\t" 
		<< unfolded_roounfold->GetXaxis()->GetBinLowEdge(i) << "\t"
		<< "<" << "\t" 
		<< unfolded_roounfold->GetXaxis()->GetBinUpEdge(i)
		<< std::endl;

      double matrix_part = covMatrix[i][i] * H4[i][i];

      // if(iSpect == 0)  // the index for var e.g. 0 is for KE
      //matrix_part = covMatrix[i][i] * H4[i][i];
     
      double rho2        = 1 - (1/matrix_part);
      
      if(rho2 < 0) continue;
      dof++;
      rho += sqrt(rho2);
    }

    AverageCor[it-1] = rho/dof;
    
  }// end for number of iterations.

  //Initialize x-axis array for the tgraphs
  float xArray[100] = {0};
  for(int i = 0; i < iter; i++){
    xArray[i] = i+1;
  }

  float fBias[iter];
  float fStatUnc[iter];
  float fCoverage[iter];

  for(int itr = 0; itr < iter; itr++){
    //iter counts from 0 but this is the first iteration

    float bias            = 0;
    float bias_weighted   = 0;
    float statistical_unc = 0;
    int   notcovered      = 0;
    float dof             = 0;

    for(int i = 1; i < hUnfoldHolder[itr]->GetXaxis()->GetNbins(); i++){
      float reco_i  = hUnfoldHolder[itr]->GetBinContent(i);
      float error_i = hUnfoldHolder[itr]->GetBinError(i);
      float truth_i = data_true->GetBinContent(i);

      if(truth_i == 0) continue;
      dof++;

      bias += reco_i/truth_i * (reco_i - truth_i);
      bias_weighted += reco_i;

      if( !((reco_i + error_i >= truth_i) &&
	    (reco_i - error_i <= truth_i)) ) notcovered++;
      
    }

    bias /= bias_weighted;
    //  std::cout << "Dof in coverage is : " <<  dof << std::endl;
    float coverage = (dof - notcovered)/dof;

    fBias[itr]     = bias;
    fCoverage[itr] = coverage;
  }

  TGraph* hBias               = new TGraph(100, xArray, fBias);
  TGraph* hCoverage           = new TGraph(100, xArray, fCoverage);
  TGraph* hChiSqCov           = new TGraph(100, xArray, ChiSqCov);
  TGraph* hAverageCorrelation = new TGraph(100, xArray, AverageCor);
  /*
  char biasname[50];
  char coveragename[50];
  char chi2name[50];
  char correlationname[50];



  sprintf(biasname, "%s_%i_bias",
          analysis_vars_unf[iSpect].name.c_str(),
          iSyst);
  sprintf(coveragename, "%s_%i_coverage",
          analysis_vars_unf[iSpect].name.c_str(),
          iSyst);
  sprintf(chi2name, "%s_%i_chi",
          analysis_vars_unf[iSpect].name.c_str(),
          iSyst);
  sprintf(correlationname, "%s_%i_correlation",
          analysis_vars_unf[iSpect].name.c_str(),
          iSyst);
  */
  hBias->SetName("bias");
  hCoverage->SetName("coverage");
  hChiSqCov->SetName("chi2");
  hAverageCorrelation->SetName("correlation");

  fout.cd();
 
  hBias->Write();
  hCoverage->Write();
  hChiSqCov->Write();
  hAverageCorrelation->Write();
 
  fout.Close();

  //----------------------------------------------------------------------
  new TCanvas();
  truedata_q2_wt_nutree->Divide(truedata_q2_nutree);
  truedata_q2_wt_nutree->Draw();
  truedata_q2_wt_nutree->GetYaxis()->SetRangeUser(0.75, 1.25);
  truedata_q2_wt_nutree->GetYaxis()->SetTitle("Weight");
  truedata_q2_wt_nutree->GetYaxis()->CenterTitle();
  truedata_q2_wt_nutree->GetXaxis()->CenterTitle();
  TLine *line = new TLine(0., 1., 5., 1);
  line->SetLineStyle(2);
  line->SetLineColor(kRed);
  line->SetLineWidth(3);
  line->Draw("same");
  gPad->SaveAs(outDir+"/"+"q2_wt.png");

  TCanvas *c =   new TCanvas("c","c");
  //  TH1* truedata_wt_nutree_clone = (TH1*)truedata_wt_nutree->Clone("truedata_wt_nutree_clone");
  truedata_E_wt_nutree->Divide(truedata_E_nutree);
  truedata_E_wt_nutree->Draw();
  truedata_E_wt_nutree->GetXaxis()->SetTitle("True Neutrino Energy (GeV)");
  truedata_E_wt_nutree->GetYaxis()->SetRangeUser(0.75, 0.95);
  truedata_E_wt_nutree->GetYaxis()->SetTitle("Weighted / Unweighted");
  truedata_E_wt_nutree->GetYaxis()->CenterTitle();
  truedata_E_wt_nutree->GetXaxis()->CenterTitle();
  gPad->SaveAs(outDir+"/"+"E_wt.png");

  //----------------------------------------------------------------------


}
//file->Close();
//}

/*
    //UnfoldSVD unfold_it(response, iter);
    UnfoldIterative unfold_it(response, iter);

    
    TH1D* unfolded_cafana = unfold_it.Truth(data_spec).ToTH1(pot);
    if (unfolded_cafana->GetMaximum() > max ) max = unfolded_cafana->GetMaximum();
    
    unfolded_cafana->GetYaxis()->SetRangeUser(0.001, max*1.1);
    //unfolded_cafana->SetTitle(("SVD Unfolding, Iteration = "+std::to_string(iter)).c_str());  
    unfolded_cafana->SetTitle(("Iterative Unfolding, Iteration = "+std::to_string(iter)).c_str());  
    unfolded_cafana->GetXaxis()->SetTitle("Neutrino Energy (GeV)");
    
    unfolded_cafana->SetLineColor(kGreen+1);
    unfolded_cafana->Draw();
    
    data_true->SetLineColor(kRed);
    data_true->SetMarkerColor(kRed);
    data_true->SetMarkerStyle(20);
    
    unfolded_roounfold->SetMarkerColor(kBlue);
    unfolded_roounfold->SetLineColor(kBlue);
    unfolded_roounfold->SetMarkerStyle(20);

    TCanvas* c = new TCanvas("c","c");
    c->cd();    
    unfolded_cafana->GetYaxis()->CenterTitle();
    unfolded_cafana->GetXaxis()->CenterTitle();
    // can->cd();
    unfolded_cafana->Draw();
    data_true->Draw("same hist");
    unfolded_roounfold->Draw("pe same");

    data->SetMarkerColor(kBlack);
    data->SetLineColor(kBlack);
    data->SetMarkerStyle(20);
    data->Draw("pe same");
    
    TLegend *leg = AutoPlaceLegend(0.3, 0.3);
    leg->AddEntry(data, "Reco Fake Data", "lp");
    leg->AddEntry(data_true,"Truth Fake Data", "l");
    leg->AddEntry(unfolded_roounfold,"Unfolded, RooUnfold", "lp");
    leg->AddEntry(unfolded_cafana,"Unfolded, CAFAna", "l");
    leg->Draw("same");

    //std::string outName = "svd_" + std::to_string(iter) + ".png";
    std::string outName = "iterative_" + std::to_string(iter) + ".png";

    c->SaveAs(outDir+"/"+outName.c_str());
    c->Close();
    delete c;
  }
     //    gPad->SaveAs("unfolding_comparison.pdf");
  for(int iter = 0; iter < iterations; ++iter){
    //int iter = 0;  
    char ratio_name[50];
    sprintf(ratio_name,"%s_%i", "ratio_hist", iter);
    hRatio[iter]=(TH1*)data_true->Clone(ratio_name);    
    //TH1 *data_true_R = (TH1*) data_true->Clone();
    //data_true_R->Divide(unfolded_roounfold);
    // hunfold[iter]->Draw();
     hRatio[iter]->Divide(hunfold[iter]);
    // data_true->Draw();
    //    data_true_R->GetYaxis()->SetRangeUser(0.8,1.1);
    //data_true_R->GetYaxis()->SetTitle("true data/roo-unfold");
    //data_true_R->Fit("pol0");
    //data_true_R->SetTitle(("SVD Unfolding, Iteration = "+std::to_string(iter)).c_str());
     TCanvas* c1 = new TCanvas("c1","c1");
     c1->cd();
     hRatio[iter]->GetYaxis()->CenterTitle();
     hRatio[iter]->GetXaxis()->CenterTitle();
     hRatio[iter]->GetYaxis()->SetRangeUser(0.9,1.5);
     hRatio[0]->GetYaxis()->SetRangeUser(0.5,2.5);
     hRatio[iter]->GetYaxis()->SetTitle("True Fake Data/Roo-Unfold");
     hRatio[iter]->Fit("pol0");
     hRatio[iter]->SetTitle(("SVD Unfolding, Iteration = "+std::to_string(iter+1)).c_str());
     //hRatio[iter]->SetTitle(("Iterative Unfolding, Iteration = "+std::to_string(iter+1)).c_str());
     hRatio[iter]->Draw();

     //    TF1 *fit      = data_true_R->GetFunction("pol0");
     TF1 *fit      = hRatio[iter]->GetFunction("pol0");
     //Double_t chi2 = fit->GetChisquare();
     value[iter]   = fit->GetParameter(0);
     error[iter]   = fit->GetParError(0);  
     // kstest->SetBinContent(kstest->FindBin(iter+1), value[iter]);
     //     kstest->SetBinError(kstest->FindBin(iter+1), error[iter]);
     kstest->SetBinContent(iter+1, value[iter]);
     kstest->SetBinError(iter+1,   error[iter]);
     //cout<< iter+1 << "\t" << value[iter] << "\t" << error[iter] << "\t" << kstest->GetBinContent(iter)<< std::endl;
     // std::string outName = "ratio_svd" + std::to_string(iter+1) + ".png";
     std::string outName = "ratio_iterative" + std::to_string(iter+1) + ".png";
     c1->SaveAs(outDir+"/"+outName.c_str());
     c1->Close();
     delete c1;
     delete fit;
  }
  TCanvas* c2 = new TCanvas("c2","c2");     
  kstest->SetMarkerColor(kBlack);
  kstest->SetLineColor(kBlack);
  kstest->SetMarkerStyle(20);
  kstest->Draw("p e");
  kstest->GetYaxis()->SetRangeUser(0.98,1.06);
  kstest->GetYaxis()->CenterTitle();
  kstest->GetXaxis()->CenterTitle();
  c2->SaveAs("plots/iterative_optimum.png");
  //c2->SaveAs("plots/svd_optimum.png");
  c2->Close();
  //  delete c2;



}

//#endif
*/
