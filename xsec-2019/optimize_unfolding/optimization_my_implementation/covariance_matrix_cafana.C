#ifdef __CINT__
void covariance_matrix_cafana(unsigned int niter = 0, bool grid_run = false)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Unfold/UnfoldIterative.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include "TFile.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TParameter.h"

#include <chrono>

using namespace ana;

void GetCovarianceMatrix(TH1D* hydata, TH1D* hx, TH2D* hresponse, TH2F* hcovmat_eff, TH2F* hcovmat)
{
    // get size of the response matrix
    int nbinsx = hresponse->GetNbinsX();
    int nbinsy = hresponse->GetNbinsY();

    // check relevant dimensions
    assert(hydata->GetNbinsX() == nbinsx);
    // debug info
    // cout << hydata->GetNbinsX() << " " << hx->GetNbinsX() << " " << nbinsx << " " << nbinsy << endl;

    // precalculate epsilon_j
    vector<float> eps;
    for(int j = 1; j <= nbinsy; j++)
    {
        float sum = 0;
        for(int i = 1; i <= nbinsx; i++)
            sum += hresponse->GetBinContent(i,j);
        eps.push_back(sum);
    }

    // precalculate yrec_i
    vector<float> yrec;
    for(int i = 1; i <= nbinsx; i++)
    {
        float sum = 0;
        for(int k = 1; k <= nbinsy; k++)
            sum += hresponse->GetBinContent(i,k)*hx->GetBinContent(k);
        yrec.push_back(sum);
    }

    for(int j = 1; j <= nbinsx; j++)
        for(int k = 1; k <= nbinsy; k++)
        {
            float sum = 0;
            float sum2 = 0;
            for(int i = 1; i <= nbinsx; i++)
            {
                if(eps[j-1] > numeric_limits<float>::min() && eps[k-1] > numeric_limits<float>::min() && yrec[i-1] > numeric_limits<float>::min())
                {
                    sum += hresponse->GetBinContent(i,j)*hresponse->GetBinContent(i,k)/eps[j-1]/eps[k-1]*hydata->GetBinContent(i)/yrec[i-1]/yrec[i-1];
                    sum2 += hresponse->GetBinContent(i,j)*hresponse->GetBinContent(i,k)*hydata->GetBinContent(i)/yrec[i-1]/yrec[i-1];
                }
            }
            float result = hx->GetBinContent(j)*hx->GetBinContent(k)*sum;
            float result2 = hx->GetBinContent(j)*hx->GetBinContent(k)*sum2;
            // debug info
            // cout << hx->GetBinContent(j) << " " << hx->GetBinContent(k) << " " << sum << " " << result << " " << eps[j-1] << endl;
            hcovmat_eff->SetBinContent(j,k,result);
            hcovmat->SetBinContent(j,k,result2);
        }
}

void covariance_matrix_cafana(unsigned int niter = 0, bool grid_run = false)
{
    // store the start time for benchmark
    auto start_time = chrono::steady_clock::now();

    // get the file path if run on grid
    string fpath = "./";
    if(grid_run)
        fpath = string(getenv("CONDOR_DIR_INPUT"));
    
    // file with a precalculated response matrix
    // in this case, nominal
    TFile file((fpath+"/syst_specs_cv.root").c_str());

    // load the response matrix itself
    ReweightableSpectrum response = *ReweightableSpectrum::LoadFrom(file.GetDirectory("cv/RecoTrue"));
    TH2D* hresponse = response.ToTH2(response.POT());

    // retrieve spectrum to be unfolded
    TFile data_file((fpath+"/spec_from_indep_mc.root").c_str());
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH1D* hydata = data_spec.ToTH1(data_spec.POT());

    // unfold!!
    auto bp1 = chrono::steady_clock::now();
    UnfoldIterative unfold(response, niter);
    Spectrum spec_unfolded = unfold.Truth(data_spec);
    TH1D* hspec_unfolded = spec_unfolded.ToTH1(spec_unfolded.POT());
    auto bp2 = chrono::steady_clock::now();
    cout << "***** Doing CAFAna unfold with " << niter << " iterations takes " << chrono::duration_cast<chrono::seconds>(bp2 - bp1).count() << " seconds." << endl;

    // After getting the unfolded spectra, calculate the covariance matrix according to p. 37, 38 of
    // http://www.desy.de/~sschmitt/talks/UnfoldStatSchool2014.pdf
    TH2F* hcovmat_eff = (TH2F*)hresponse->Clone("cov_mat_eff");
    TH2F* hcovmat = (TH2F*)hresponse->Clone("cov_mat");
    GetCovarianceMatrix(hydata, hspec_unfolded, hresponse, hcovmat_eff, hcovmat);

    // save to file
    TFile* outf = new TFile(Form("unfolded_spectra_iter_%d.root", niter),"recreate");
    hspec_unfolded->Write("unfolded_spectrum");
    hcovmat_eff->Write("hcov_mat_eff");
    hcovmat->Write("hcov_mat");

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif