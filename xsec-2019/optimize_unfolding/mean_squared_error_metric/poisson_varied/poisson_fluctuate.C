#ifdef __CINT__
void poisson_fluctuate(int nensemble = 100)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TMatrixD.h"
#include "TPad.h"
#include "TRandom3.h"
#include "TROOT.h"

using namespace ana;

TH2F* normalize_matrix(TH2F* mat)
{
    TH2F* res_mat = (TH2F*)mat->Clone("response_matrix");

    int nx = mat->GetNbinsX();
    int ny = mat->GetNbinsY();

    for(int j = 1; j <= ny; j++)
    {
        float row_sum = 0;
        for(int i = 1; i <= nx; i++)
            row_sum += mat->GetBinContent(i,j);
        for(int i = 1; i <= nx; i++)
        {
            float cnt = mat->GetBinContent(i,j);
            if(row_sum != 0) cnt /= row_sum;
            res_mat->SetBinContent(i,j,cnt);
        }
    }
    
    return res_mat;
}

TH1F* poisson_fluctuate(int spec_id, TH1* htrue)
{
    TH1F* hfluc = (TH1F*)htrue->Clone(Form("hfluc%d", spec_id));

    // fluctuate
    TRandom3 rand_num(spec_id+1);
    for(int i = 1; i <= htrue->GetNbinsX(); i++)
    {
        double mean = htrue->GetBinContent(i);
        hfluc->SetBinContent(i, rand_num.Poisson(mean));
    }
    
    return hfluc;
}

TH1F* smear_histogram(char const* hname, TH1F* true_spec, TH2F* resp_mat)
{
    TH1F* smeared = (TH1F*)true_spec->Clone(hname);
    
    for(int i = 1; i <= smeared->GetNbinsX(); i++)
    {
        float bin_cnt = 0;
        for(int j = 1; j <= true_spec->GetNbinsX(); j++)
            bin_cnt += resp_mat->GetBinContent(i,j)*true_spec->GetBinContent(j);
        smeared->SetBinContent(i,bin_cnt);
    }
    cout << hname << " done" << endl;
    return smeared;
}

void poisson_fluctuate(int nensemble = 100)
{
    // retrieve the true histogram
    TFile tinf_true_spec("input_from_connor/fake_data_fluxes_allwghts.root");
    Spectrum strue = *Spectrum::LoadFrom(tinf_true_spec.GetDirectory("fakedata/true/ppfxxsecwgt"));
    TH1* htrue = strue.ToTH1(strue.POT());

    // make container and fluctuate true
    vector<TH1F*> true_specs;
    for(int i = 0; i < nensemble; i++)
        true_specs.push_back(poisson_fluctuate(i, htrue));

    // normalize migration matrix
    TFile tinf_unf_mat("input_from_connor/rooUnfold1iter.root");
    TH2F* unf_mat = (TH2F*)tinf_unf_mat.Get("cv/EAvail/UnfoldingMatrix");
    TH2F* resp_mat = normalize_matrix(unf_mat);
    
    // smear all histograms
    vector<TH1F*> reco_specs;
    for(unsigned int i = 0; i < true_specs.size(); i++)
        reco_specs.push_back(smear_histogram(Form("hsmeared%d", i), true_specs[i], resp_mat));
    
    // smear the true spectrum
    TH1F* hsmeared = smear_histogram("hsmeared", (TH1F*)htrue, resp_mat);
    
    // save to file
    gROOT->ProcessLine(".! mkdir -p processed_input");
    TFile toutf("processed_input/fluctuated_smeared.root", "recreate");
    htrue->Write("htrue");
    hsmeared->Write();
    unf_mat->Write("unfolding_matrix");
    resp_mat->Write("response_matrix");
    toutf.mkdir("fluctuated");
    toutf.cd("fluctuated");
    for(unsigned int i = 0; i < true_specs.size(); i++)
        true_specs[i]->Write();
    toutf.cd();
    toutf.mkdir("smeared");
    toutf.cd("smeared");
    for(unsigned int i = 0; i < reco_specs.size(); i++)
        reco_specs[i]->Write();
}
#endif