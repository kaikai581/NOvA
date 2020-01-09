#ifdef __CINT__
void calculate_covariance_matrix(unsigned int niter = 0, bool grid_run = false)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include "TFile.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TParameter.h"

using namespace ana;

TH1D* Unravel3D(TH3D* h)
{
    int nbinsx = h->GetNbinsX();
    int nbinsy = h->GetNbinsY();
    int nbinsz = h->GetNbinsZ();
    int nbins = nbinsx*nbinsy*nbinsz;

    TH1D* hres = new TH1D("hunfolded","unraveled unfolded spectrum", nbins, 0, nbins);
    int cnt = 1;
    for(int i = 1; i <= nbinsx; i++)
        for(int j = 1; j <= nbinsy; j++)
            for(int k = 1; k <= nbinsz; k++)
                hres->SetBinContent(cnt++,h->GetBinContent(i,j,k));
    
    return hres;
}

void GetCovarianceMatrix(TH1D* hydata, TH1D* hx, TH2D* hresponse, TH2F* hcovmat_eff, TH2F* hcovmat)
{
    // get size of the response matrix
    int nbinsx = hresponse->GetNbinsX();
    int nbinsy = hresponse->GetNbinsY();

    // check relevant dimensions
    assert(hydata->GetNbinsX() == nbinsx);

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
            hcovmat_eff->SetBinContent(j,k,result);
            hcovmat->SetBinContent(j,k,result2);
        }
}

void calculate_covariance_matrix(unsigned int niter = 0, bool grid_run = false)
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

    // construct empty 3D histograms
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = enuedges.size()-1;
    TH3D* hMeas_for_binning = new TH3D("hMeas_for_binning","hMeas_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);
    TH3D* hTrue_for_binning = new TH3D("hTrue_for_binning","hTrue_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);

    // retrieve spectrum to be unfolded
    TFile data_file((fpath+"/spec_from_indep_mc.root").c_str());
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH3* hdata = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, enubins);
    TH1D* hydata = data_spec.ToTH1(data_spec.POT());

    // construct RooUnfold response matrix
    auto bp1 = chrono::steady_clock::now();
    RooUnfoldResponse Response(hMeas_for_binning, hTrue_for_binning, (TH2*)hresponse);
    auto bp2 = chrono::steady_clock::now();
    cout << "***** Constructing RooUnfold response matrix takes " << chrono::duration_cast<chrono::seconds>(bp2 - bp1).count() << " seconds." << endl;

    // get true spectrum from response matrix
    TH1* hTruth = Response.Htruth();
    RooUnfoldBayes unfold(&Response, hdata, niter);
    auto bp3 = chrono::steady_clock::now();
    cout << "***** " << niter << " iterations of unfolding takes " << chrono::duration_cast<chrono::seconds>(bp3 - bp2).count() << " seconds." << endl;

    // recover the unfolded spectrum
    TH3D* hunfolded3 = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kNoError);
    auto bp5 = chrono::steady_clock::now();
    cout << "***** Retrieving reconstructed spectrum with errors calculated with covariance matrix takes " << chrono::duration_cast<chrono::seconds>(bp5 - bp3).count() << " seconds." << endl;

    // unravel TH3D
    TH1D* hunfolded = Unravel3D(hunfolded3);

    // After getting the unfolded spectra, calculate the covariance matrix according to p. 37, 38 of
    // http://www.desy.de/~sschmitt/talks/UnfoldStatSchool2014.pdf
    TH2F* hcovmat_eff = (TH2F*)hresponse->Clone("cov_mat_eff");
    TH2F* hcovmat = (TH2F*)hresponse->Clone("cov_mat");
    GetCovarianceMatrix(hydata, hunfolded, hresponse, hcovmat_eff, hcovmat);

    // save to file
    TFile* outf = new TFile(Form("unfolded_spectra_roounfold_iter_%d.root", niter),"recreate");
    hunfolded->Write("unfolded_spectrum");
    hcovmat_eff->Write("cov_mat_eff");
    hcovmat->Write("cov_mat");

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif