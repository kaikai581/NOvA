#ifdef __CINT__
void optimize_iterations()
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

using namespace ana;

void optimize_iterations()
{
    // file with a precalculated response matrix
    // in this case, nominal
    TFile file("syst_specs_cv.root");

    // load the response matrix itself
    ReweightableSpectrum response = *ReweightableSpectrum::LoadFrom(file.GetDirectory("cv/RecoTrue"));
    TH2D* hresponse = response.ToTH2(response.POT());

    // construct empty 3D histograms
    // float angedges[] = {-1.0, 0.5, 0.56, 0.62, 0.68, 0.74, 0.80, 0.85, 0.88, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0};
    // float mukeedges[] = {-10.0,
    //                     0.5, 0.6, 0.7, 0.8, 0.9,
    //                     1.0, 1.1, 1.2, 1.3, 1.4,
    //                     1.5, 1.6, 1.7, 1.8, 1.9,
    //                     2.0, 2.1, 2.2, 2.3, 2.4,
    //                     2.5, 120};
    // float enuedges[] = { -10,
    //                     0.0, 0.25, 0.50, 0.75,
    //                     1.0, 1.25, 1.50, 1.75,
    //                     2.0, 2.25, 2.50, 2.75,
    //                     3.0, 3.25, 3.50, 3.75,
    //                     4.0, 4.25, 4.50, 4.75,
    //                     5.0, 120};
    // int nbinsx = sizeof(angedges)/sizeof(float) - 1;
    // int nbinsy = sizeof(mukeedges)/sizeof(float) - 1;
    // int nbinsz = sizeof(enuedges)/sizeof(float) - 1;
    // TH3D* hMeas = new TH3D("hMeas","hMeas",nbinsx,angedges,nbinsy,mukeedges,nbinsz,enuedges);
    // TH3D* hTrue = new TH3D("hTrue","hTrue",nbinsx,angedges,nbinsy,mukeedges,nbinsz,enuedges);
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = enuedges.size()-1;
    TH3D* hMeas = new TH3D("hMeas","hMeas",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);
    TH3D* hTrue = new TH3D("hTrue","hTrue",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);

    // retrieve spectrum to be unfolded
    TFile data_file("spec_from_indep_mc.root");
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("nominal/MCSig_Reco"));
    TH3* hdata = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, enubins);

    // construct RooUnfold response matrix
    TFile* outf = new TFile("intermediate_spectra.root","recreate");
    RooUnfoldResponse Response(hMeas, hTrue, (TH2*)hresponse);
    for(unsigned int i = 0; i <= 2; i++)
    {
        RooUnfoldBayes unfold(&Response, hdata, i);
        TH3D* hunfold = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
        hunfold->Write(Form("hunfolded_iter_%d", i));
    }
    // RooUnfoldBayes unfold_iter0(&Response, hdata, 0);
    // TH3D* unfolded_iter0 = (TH3D*)unfold_iter0.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    // RooUnfoldBayes unfold_iter1(&Response, hdata, 1);
    // TH3D* unfolded_iter1 = (TH3D*)unfold_iter1.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    // RooUnfoldBayes unfold_iter2(&Response, hdata, 2);
    // TH3D* unfolded_iter2 = (TH3D*)unfold_iter2.Hreco(RooUnfold::ErrorTreatment::kCovariance);

    cout << "Unfolding done." << endl;
    // unfolded_iter0->Write("unfolded_iter0");
    // unfolded_iter1->Write("unfolded_iter1");
    // unfolded_iter2->Write("unfolded_iter2");
    outf->Close();
}
#endif