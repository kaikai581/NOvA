#ifdef __CINT__
void unfold_grid(int spec_id = 0, unsigned int niter = 1, bool grid_run = false, bool do_1d = true, bool do_3d = false)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include <iostream>
#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

using namespace ana;

// global variable
map<tuple<int,int,int>,int> index_map;
map<int,tuple<int,int,int>> reverse_index_map;

void fill_index_map()
{
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = eavailedges.size()-1;
    int ibin = 1;
    for(unsigned int i = 1; i <= nbinsx; i++)
        for(unsigned int j = 1; j <= nbinsy; j++)
            for(unsigned int k = 1; k <= nbinsz; k++)
            {
                tuple<int,int,int> idx_3d(i,j,k);
                int idx_1d = ibin++;
                index_map[idx_3d] = idx_1d;
                reverse_index_map[idx_1d] = idx_3d;
            }
}

TH1D* MyToTH1(TH3D* h3d, TH1D* h1d, char const* hname)
{
    TH1D* hres = (TH1D*)h1d->Clone(hname);

    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = eavailedges.size()-1;
    int ibin = 1;
    for(unsigned int i = 1; i <= nbinsx; i++)
        for(unsigned int j = 1; j <= nbinsy; j++)
            for(unsigned int k = 1; k <= nbinsz; k++)
                hres->SetBinContent(ibin++,h3d->GetBinContent(i,j,k));

    return hres;
}

TH3D* MyToTH3(TH1D* h1d)
{
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = eavailedges.size()-1;

    TH3D* hres = new TH3D("hdata_3d","",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&eavailedges[0]);
    for(int i = 1; i <= h1d->GetNbinsX(); i++)
    {
        int ix = (i-1)/(nbinsy*nbinsz)+1;
        int iy = ((i-1)%(nbinsy*nbinsz))/nbinsz+1;
        int iz = ((i-1)%(nbinsy*nbinsz))%nbinsz+1;
        hres->SetBinContent(ix,iy,iz,h1d->GetBinContent(i));
    }

    return hres;
}


void unfold_grid(int spec_id = 0, unsigned int niter = 1, bool grid_run = false, bool do_1d = true, bool do_3d = false)
{
    // get the input file path depending on where the script is run
    string fpath = "./processed_input";
    if(grid_run)
        fpath = string(getenv("CONDOR_DIR_INPUT"));
    
    // define bimap and fill them
    // fill_index_map();

    // open the input file containing the reco spectrum and the
    // response matrix
    TFile* tinf = new TFile((fpath+"/fluctuated_smeared.root").c_str());

    // get the reco spectrum and response matrix
    TH1D* hTrue = (TH1D*)tinf->Get("htrue");
    TH1D* hData = (TH1D*)tinf->Get(Form("smeared/hsmeared%d",spec_id));
    TH3D* hData_3d = MyToTH3(hData);


    // save to file
    string outpath = "output";
    if(grid_run)
        outpath = ".";
    TFile toutf(Form("%s/migration_unnormalized_smeared_%d_iter_%d.root",outpath.c_str(),spec_id,niter), "recreate");
    hTrue->SetLineColor(kRed);
    hTrue->SetMarkerColor(kRed);
    hTrue->Write("htrue");
    hData->SetLineColor(kBlue);
    hData->SetMarkerColor(kBlue);
    hData->Write("hreco");


    if(do_3d)
    {
        //***** 3d unfolding *****
        //* construct empty 3D histograms
        unsigned int nbinsx = angedges.size()-1;
        unsigned int nbinsy = mukeedges.size()-1;
        unsigned int nbinsz = eavailedges.size()-1;
        TH3D* hMeas_binning_3d = new TH3D("hMeas_for_binning","hMeas_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&eavailedges[0]);
        TH3D* hTrue_binning_3d = new TH3D("hTrue_for_binning","hTrue_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&eavailedges[0]);
        TH2D* hunfolding_matrix = (TH2D*)tinf->Get("unfolding_matrix"); // response is NOT "normalized"
        RooUnfoldResponse unfolding_matrix(hMeas_binning_3d, hTrue_binning_3d, (TH2*)hunfolding_matrix);
        //* unfold!
        RooUnfoldBayes unfold_3d(&unfolding_matrix, (TH3*)hData_3d, niter);
        TH3D* hunfold_3d = (TH3D*)unfold_3d.Hreco(RooUnfold::ErrorTreatment::kCovariance);
        TH1D* hunfold_3dto1d = MyToTH1(hunfold_3d, hTrue, "hunfold_3dto1d");
        TMatrixD covMatrix_3d = unfold_3d.Ereco();
        TH2D* covmat_3d = new TH2D(covMatrix_3d);
        //***** 3d unfolding *****
        hData_3d->Write("hreco_3d");
        hunfold_3dto1d->Write("hunfold_3dto1d");
        hunfold_3d->Write("hunfold_3d");
        hunfolding_matrix->Write("hunfolding_matrix");
        covmat_3d->Write("hcovariance_matrix_3d");
    }

    if(do_1d)
    {
        //***** 1d unfolding *****
        //* prepare the response matrix for RooUnfold
        TH1D* hMeas_binning = new TH1D("hmeas_bin", "hmeas_bin", hData->GetNbinsX(), hData->GetXaxis()->GetXmin(), hData->GetXaxis()->GetXmax());
        TH1D* hTrue_binning = new TH1D("htrue_bin", "htrue_bin", hTrue->GetNbinsX(), hTrue->GetXaxis()->GetXmin(), hTrue->GetXaxis()->GetXmax());
        TH2D* hresponse_matrix = (TH2D*)tinf->Get("response_matrix"); // response is "normalized"
        TH2D* hunfolding_matrix = (TH2D*)tinf->Get("unfolding_matrix"); // response is NOT "normalized"

        //* unfold!
        // RooUnfoldResponse response_matrix(hMeas_binning, hTrue_binning, (TH2*)hresponse_matrix);
        // RooUnfoldBayes unfold(&response_matrix, hData, niter);
        RooUnfoldResponse unfolding_matrix(hMeas_binning, hTrue_binning, (TH2*)hunfolding_matrix);
        RooUnfoldBayes unfold(&unfolding_matrix, hData, niter);
        TH1* hunfold = unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
        TMatrixD covMatrix = unfold.Ereco();
        TH2D* covmat = new TH2D(covMatrix);
        //***** 1d unfolding *****

        hunfold->SetLineColor(kGreen);
        hunfold->SetMarkerColor(kGreen);
        hunfold->Write("hunfolded");
        hresponse_matrix->Write("hresponse_matrix");
        if(!do_3d) hunfolding_matrix->Write("hunfolding_matrix");
        covmat->Write("hcovariance_matrix");
    }
}
#endif
