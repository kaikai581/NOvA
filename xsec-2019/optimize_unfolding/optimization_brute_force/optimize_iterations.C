#ifdef __CINT__
void optimize_iterations(unsigned int niter = 0)
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
#include "TParameter.h"

using namespace ana;

void optimize_iterations(unsigned int niter = 0)
{
    // file with a precalculated response matrix
    // in this case, nominal
    TFile file("syst_specs_cv.root");

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
    TFile data_file("spec_from_indep_mc.root");
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH3* hdata = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, enubins);

    // construct RooUnfold response matrix
    RooUnfoldResponse Response(hMeas_for_binning, hTrue_for_binning, (TH2*)hresponse);
    // get true spectrum from response matrix
    TH1* hTruth = Response.Htruth();
    RooUnfoldBayes unfold(&Response, hdata, niter);
    // store the X^2 of the unfolding
    TParameter<float> chi2("chi2", unfold.Chi2(hTruth, RooUnfold::ErrorTreatment::kCovariance));
    // Below code is copied from Matt's. Copying his "double" option for "double differential" measurement.
    TMatrixF covMatrix = unfold.Ereco();
    TMatrixF covMatrix2 = unfold.Ereco();

    // recover the unfolded spectrum
    TH3D* hunfolded = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    //Inversion requires the diagonal to be non zero
    //Probably should remove the rows and columns but just setting them to some
    //small but nonzero value for now.
    for(int i = 0; i < covMatrix2.GetNcols(); i++)
        if(covMatrix2[i][i] <= 0.001)
        {
            covMatrix[i][i] = 0.05;
            covMatrix2[i][i] = 0.05;
        }

    Double_t det;
    TMatrix covMatrixInv = (TMatrix)covMatrix2.Invert(&det);
    double rho = 0;
    double dof = 0;
    for(int i = 0; i < covMatrix2.GetNcols(); i++){
        double matrix_part = 0;
        matrix_part = covMatrix[i][i] * covMatrixInv[i][i];
        double rho2 = 1 - (1.0/matrix_part);
        if(rho2 < 0) continue;
        dof++;
        rho += sqrt(rho2);
    }
    if(dof < 1) dof = 1;

    TParameter<float>  AverageCor("avg_corr", rho/dof);
    TFile* outf = new TFile(Form("unfolded_spectra_iter_%d.root", niter),"recreate");
    hunfolded->Write("unfolded_spectrum");
    chi2.Write();
    AverageCor.Write();

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif