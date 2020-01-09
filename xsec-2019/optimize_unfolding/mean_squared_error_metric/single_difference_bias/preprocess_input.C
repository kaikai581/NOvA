#ifdef __CINT__
void preprocess_input(int niter = 1)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH3F.h"
#include "TMatrixD.h"

using namespace ana;

TH1D* myToTH1(TH3F* h3d, TH1D* href, char const* spec_name)
{
    TH1D* hres = (TH1D*)href->Clone(spec_name);
    int bincnt = 1;
    for(int i = 1; i <= h3d->GetNbinsX(); i++)
        for(int j = 1; j <= h3d->GetNbinsY(); j++)
            for(int k = 1; k <= h3d->GetNbinsZ(); k++)
                hres->SetBinContent(bincnt++, h3d->GetBinContent(i,j,k));

    return hres;
}

TH1D* myToTH1(TH3* h3d, TH1D* href, char const* spec_name)
{
    TH1D* hres = (TH1D*)href->Clone(spec_name);
    int bincnt = 1;
    for(int i = 1; i <= h3d->GetNbinsX(); i++)
        for(int j = 1; j <= h3d->GetNbinsY(); j++)
            for(int k = 1; k <= h3d->GetNbinsZ(); k++)
                hres->SetBinContent(bincnt++, h3d->GetBinContent(i,j,k));

    return hres;
}

void preprocess_input(int niter = 1)
{
    TFile tinf(Form("input_files_by_connor/rooUnfold%diter.root", niter));
    TH1D* fake_data_bkg = (TH1D*)tinf.Get("cv/EAvail/FakeDataMinusBkgd");
    TH1D* fake_data_pur = (TH1D*)tinf.Get("cv/EAvail/FakeDataTimesPur");
    TH3F* unfolded_spec_3d = (TH3F*)tinf.Get("cv/EAvail/UnfoldedFakeData");
    TH1D* unfolded_spec = myToTH1(unfolded_spec_3d, fake_data_bkg, "unfolded_spec");
    TMatrixD* cov_mat = (TMatrixD*)tinf.Get("cv/EAvail/Covariance");

    TFile tinf_true_spec("input_files_by_connor/fake_data_fluxes_allwghts.root");
    Spectrum strue = *Spectrum::LoadFrom(tinf_true_spec.GetDirectory("fakedata/true/ppfxxsecwgt"));
    TH3* htrue_3d = ana::ToTH3(strue, strue.POT(), kPOT, angbinsCustom, mukebins, eavailbins);
    TH1D* true_spec = myToTH1(htrue_3d, fake_data_bkg, "true_spec");
    
    TFile toutf(Form("input_files_by_connor/processed_iter_%d.root", niter), "recreate");
    fake_data_bkg->Write("fake_data_bkg");
    fake_data_pur->Write("fake_data_pur");
    true_spec->Write("true_spec");
    unfolded_spec->Write("unfolded_spec");
    cov_mat->Write("cov_mat");
}
#endif