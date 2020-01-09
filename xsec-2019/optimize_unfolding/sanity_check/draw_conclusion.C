#ifdef __CINT__
void draw_conclusion()
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH3D.h"
#include "TMatrixDSparse.h"
#include "TParameter.h"

using namespace ana;


void compare_projection(string dir, TH3* horig, TH3* h_orig_it2, TH3* h_it1_it1)
{
    TCanvas* c1 = new TCanvas("c1");
    TH1* h1_orig = (TH1*)horig->Project3D(dir.c_str());
    TH1* h1_orig_it2 = (TH1*)h_orig_it2->Project3D(dir.c_str());
    TH1* h1_it1_it1 = (TH1*)h_it1_it1->Project3D(dir.c_str());
    h1_orig->Draw("hist");
    h1_orig_it2->Draw("histsame");
    h1_it1_it1->Draw("histsame");
}


void draw_conclusion()
{
    // open files
    TFile* orig = new TFile("../sparse_matrix_test/syst_specs_cv.root");
    TFile* from_orig = new TFile("spectra_from_fake_data.root");
    TFile* from_it1 = new TFile("spectra_from_iter_1.root");

    // retrieve original spectrum
    TFile data_file("../sparse_matrix_test/spec_from_indep_mc.root");
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH3* horig = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, enubins);

    TH3D* h_orig_it1 = (TH3D*)from_orig->Get("hunfolded_iter_1");
    TH3D* h_orig_it2 = (TH3D*)from_orig->Get("hunfolded_iter_2");
    TH3D* h_it1_it1 = (TH3D*)from_it1->Get("hunfolded_iter_1");
    TH1D* h1_orig = (TH1D*)horig->Project3D("X");
    TH1D* h1_orig_it2 = (TH1D*)h_orig_it2->Project3D("X");
    TH1D* h1_it1_it1 = (TH1D*)h_it1_it1->Project3D("X");
    h1_orig_it2->Draw("hist");

    // make projection plots
    // compare_projection("X", horig, h_orig_it2, h_it1_it1);
    // compare_projection("Y", horig, h_orig_it2, h_it1_it1);
    // compare_projection("Z", horig, h_orig_it2, h_it1_it1);
}
#endif