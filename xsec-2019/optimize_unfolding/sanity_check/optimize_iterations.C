#ifdef __CINT__
void optimize_iterations(string infn = "spectra_from_fake_data.root")
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

void optimize_iterations(string infn = "spectra_from_fake_data.root")
{
    // file with a precalculated response matrix
    // in this case, nominal
    cout << "Input file used for spectra to unfold: " << infn << endl;
    TFile file_response("../syst_specs_cv.root");

    // load the response matrix itself
    ReweightableSpectrum response = *ReweightableSpectrum::LoadFrom(file_response.GetDirectory("cv/RecoTrue"));
    TH2D* hresponse = response.ToTH2(response.POT());

    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = enuedges.size()-1;
    TH3D* hMeas = new TH3D("hMeas","hMeas",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);
    TH3D* hTrue = new TH3D("hTrue","hTrue",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);

    // retrieve spectrum to be unfolded
    TFile data_file(infn.c_str());
    TH3* hdata = (TH3*)data_file.Get("hunfolded_iter_1");

    // construct RooUnfold response matrix
    TFile* outf = new TFile("spectra_from_iter_1.root","recreate");
    RooUnfoldResponse Response(hMeas, hTrue, (TH2*)hresponse);
    for(unsigned int i = 0; i <= 2; i++)
    {
        RooUnfoldBayes unfold(&Response, hdata, i);
        TH3D* hunfold = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
        hunfold->Write(Form("hunfolded_iter_%d", i));
    }

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif