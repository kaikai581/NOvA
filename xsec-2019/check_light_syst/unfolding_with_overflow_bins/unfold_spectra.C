#ifdef __CINT__

void unfold_spectra()
{
  std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Unfold/UnfoldIterative.h"

#include "NDAna/numucc_inc/NumuCCIncAnalysis.h"

#include "TFile.h"
#include "TH3.h"

using namespace ana;

string inf_fd_name = "input_files/fSpecsFakeData.root";
string inf_nom_name = "input_files/fSystSpec_cv_s100_l100.root";
string inf_ld_name = "input_files/fSystSpec_lightdown_s100_l100.root";
string inf_lu_name = "input_files/fSystSpec_lightup_s100_l100.root";

const double pot    = 8.09e20;      //data POT

void do_unfolding(TDirectory* dirInMC, TDirectory* dirInDt, string sdOut, TH1* hInFlux, TFile* fileOut)
{
  Spectrum* fData = ana::LoadFrom<Spectrum>(dirInDt->GetDirectory("ppfxwgt")).release();
  IBkgdEstimator* fBkgEst  = ana::LoadFrom<IBkgdEstimator>(dirInMC->GetDirectory("BkgdEst")).release();
  Spectrum fBkg            = fBkgEst->Background();
  Spectrum fSignalEst      = (*fData) - fBkg;
  ReweightableSpectrum* fRecoTrue = ana::LoadFrom<ReweightableSpectrum>(dirInMC->GetDirectory("RecoTrue")).release();
  TH3* h3estsig            = ana::ToTH3(fSignalEst      , pot, kPOT, angbinsCustom, mukebins, enubins);
  fileOut->cd();
  fileOut->cd("debug");
  TH1* h1estsig = (TH1*)h3estsig->Project3D("z");
  h1estsig->Write(Form("%s_est_sig",sdOut.c_str()));

  UnfoldIterative unfold(*fRecoTrue,5);
  Spectrum fSigUnfolded = unfold.Truth(fSignalEst);
  TH3* h3unfsig         = ana::ToTH3(fSigUnfolded      , pot, kPOT, angbinsCustom, mukebins, enubins);
  fileOut->cd();
  fileOut->cd("debug");
  TH1* h1unfsig = (TH1*)h3unfsig->Project3D("z");
  h1unfsig->Write(Form("%s_unfolded_sig",sdOut.c_str()));
}


void unfold_spectra()
{
  // open an output file
  TFile* fout = new TFile("output.root", "recreate");
  fout->mkdir("debug");

  // grab flux histogram
  TFile* fflux = new TFile("/nova/ana/users/slin/temp/numuccinc/systs_all.root","read");
  TH1* hFlux_cv = Spectrum::LoadFrom(fflux->GetDirectory("fluxes/CV"))->ToTH1(pot);
  hFlux_cv->Scale(1e-4); // nu/m^2 to nu/cm^2

  TFile* fsys = new TFile(inf_nom_name.c_str());
  TFile* fdata = new TFile(inf_fd_name.c_str());
  TDirectory* dir_mc = fsys->GetDirectory("cv");
  TDirectory* dir_data = fdata->GetDirectory("fakedata");
  do_unfolding(dir_mc, dir_data, "cv", hFlux_cv, fout);

  fsys = new TFile(inf_ld_name.c_str());
  dir_mc = fsys->GetDirectory("lightdown");
  do_unfolding(dir_mc, dir_data, "lightdown", hFlux_cv, fout);

  fsys = new TFile(inf_lu_name.c_str());
  dir_mc = fsys->GetDirectory("lightup");
  do_unfolding(dir_mc, dir_data, "lightup", hFlux_cv, fout);
}
#endif