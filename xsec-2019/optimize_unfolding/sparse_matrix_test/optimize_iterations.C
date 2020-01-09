#ifdef __CINT__
void optimize_iterations(unsigned int niter = 0, bool grid_run = false)
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
#include "TMatrixDSparse.h"
#include "TParameter.h"

#include <chrono>

using namespace ana;

void optimize_iterations(unsigned int niter = 0, bool grid_run = false)
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
    // Try to use sparse histograms. It seem that THnSparse does not support variable sized binning.
    // THnSparse hMeas_sp = new THnSparse("hMeas_sp","hMeas_sp",3,{nbinsx,nbinsy,nbinsz})

    // retrieve spectrum to be unfolded
    TFile data_file((fpath+"/spec_from_indep_mc.root").c_str());
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH3* hdata = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, enubins);

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

    // store the X^2 of the unfolding
    TParameter<float> chi2("chi2", unfold.Chi2(hTruth, RooUnfold::ErrorTreatment::kCovariance));
    auto bp4 = chrono::steady_clock::now();
    cout << "***** Calculating chi-square takes " << chrono::duration_cast<chrono::seconds>(bp4 - bp3).count() << " seconds." << endl;

    // Below code is copied from Matt's. Copying his "double" option for "double differential" measurement.
    TMatrixDSparse covMatrix = unfold.Ereco();
    TMatrixDSparse covMatrix2 = unfold.Ereco();

    // recover the unfolded spectrum
    TH3D* hunfolded = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    auto bp5 = chrono::steady_clock::now();
    cout << "***** Retrieving reconstructed spectrum with errors calculated with covariance matrix takes " << chrono::duration_cast<chrono::seconds>(bp5 - bp4).count() << " seconds." << endl;

    //Inversion requires the diagonal to be non zero
    //Probably should remove the rows and columns but just setting them to some
    //small but nonzero value for now.
    for(int i = 0; i < covMatrix2.GetNcols(); i++)
        if(covMatrix2[i][i] <= 0.001)
            covMatrix2[i][i] = 0.05;

    Double_t det;
    // TMatrix covMatrixInv = (TMatrix)covMatrix2.Invert(&det);
    auto bp6 = chrono::steady_clock::now();
    cout << "***** Inverting covariance matrix takes " << chrono::duration_cast<chrono::seconds>(bp6 - bp5).count() << " seconds." << endl;

    double rho = 0;
    double dof = 0;
    for(int i = 0; i < covMatrix2.GetNcols(); i++){
        double matrix_part = 0;
        // matrix_part = covMatrix[i][i] * covMatrixInv[i][i];
        double rho2 = 1 - (1.0/matrix_part);
        if(rho2 < 0) continue;
        dof++;
        rho += sqrt(rho2);
    }
    if(dof < 1) dof = 1;
    TParameter<float>  AverageCor("avg_corr", rho/dof);
    auto bp7 = chrono::steady_clock::now();
    cout << "***** Calculating global average covariance takes " << chrono::duration_cast<chrono::seconds>(bp7 - bp6).count() << " seconds." << endl;

    // get another kind of covariance matrix
    TMatrixD covMatrix3 = unfold.GetMeasuredCov();

    // save to file
    gROOT->ProcessLine(".! mkdir -p output");
    TFile* outf = new TFile(Form("unfolded_spectra_iter_%d.root", niter),"recreate");
    hunfolded->Write("unfolded_spectrum");
    chi2.Write();
    AverageCor.Write();
    covMatrix.Write("err_reco_1");
    covMatrix2.Write("err_reco_2");
    covMatrix3.Write("get_meas_cov");

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif