#ifdef __CINT__
void recast_input()
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/ReweightableSpectrum.h"

#include "NDAna/numucc_inc/NumuCCIncBins.h"

#include "RooUnfoldResponse.h"

#include "TFile.h"
#include "TH2D.h"
#include "TH3D.h"

using namespace ana;

void recast_input()
{
    // load response matrix
    TFile fresponse("input_files/syst_specs_cv.root");
    // load the response matrix itself
    ReweightableSpectrum response = *ReweightableSpectrum::LoadFrom(fresponse.GetDirectory("cv/EAvail/RecoTrue"));
    TH2D* hresponse = response.ToTH2(response.POT());

    // retrieve spectrum to be unfolded
    TFile data_file("input_files/fake_data_fluxes.root");
    Spectrum data_spec = *Spectrum::LoadFrom(data_file.GetDirectory("fakedata/ppfxwgt"));
    TH3* hdata = ana::ToTH3(data_spec, data_spec.POT(), kPOT, angbinsCustom, mukebins, eavailbins);

    // make template histograms
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = eavailedges.size()-1;
    TH3D* hMeas = new TH3D("hMeas","hMeas",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&eavailedges[0]);
    TH3D* hTrue = new TH3D("hTrue","hTrue",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&eavailedges[0]);

    // make RooUnfoldResponse instence
    RooUnfoldResponse Response(hMeas, hTrue, (TH2*)hresponse);

    // save to file
    TFile toutf("input_files/recasted_input.root", "recreate");
    hresponse->Write("hresponse");
    Response.Write("rresponse");
    hdata->Write("hdata");
    hMeas->Write("hmeas");
    hTrue->Write("htrue");
}
#endif