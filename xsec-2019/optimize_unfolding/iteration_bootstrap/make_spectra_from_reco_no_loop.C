#ifdef __CINT__
void make_spectra_from_reco_no_loop(int niter = 1, string infn = "../spec_from_indep_mc.root")
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include "TFile.h"
#include "TH3D.h"
#include "TROOT.h"

using namespace ana;

void make_spectra_from_reco_no_loop(int niter = 1, string infn = "../spec_from_indep_mc.root")
{
    // load the source spectrum to be unfolded
    TFile data_file(infn.c_str());
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH3* hdata = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, enubins);

    // load migration matrix
    TFile file("../syst_specs_cv.root");

    // load the response matrix itself
    ReweightableSpectrum response = *ReweightableSpectrum::LoadFrom(file.GetDirectory("cv/RecoTrue"));
    TH2D* hresponse = response.ToTH2(response.POT());

    // construct empty 3D histograms
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = enuedges.size()-1;
    TH3D* hMeas_for_binning = new TH3D("hMeas_for_binning","hMeas_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);
    TH3D* hTrue_for_binning = new TH3D("hTrue_for_binning","hTrue_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);

    // construct RooUnfold response matrix
    RooUnfoldResponse Response(hMeas_for_binning, hTrue_for_binning, (TH2*)hresponse);

    // output to file
    gROOT->ProcessLine(".! mkdir -p spectra_from_reco");
    TFile* outf = new TFile(Form("spectra_from_reco/spectra_from_reco_%diter.root", niter),"recreate");
    hdata->Write("hdata");
    RooUnfoldBayes unfold(&Response, hdata, niter);
    TH3D* hunfold = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    hunfold->Write(Form("hunfolded_iter_%d", niter));

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif