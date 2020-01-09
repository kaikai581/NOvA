#ifdef __CINT__
void make_spectra_from_niter_no_loop(string src_file_history = "0-1", int niter = 1)
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
#include "TROOT.h"

using namespace ana;

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

TH3D* getHist2Unfold(string& history)
{
    vector<string> iter_history = split(history, "-");

    string subfn = history;
    
    string fpn = "spectra_from_reco/spectra_" + subfn + ".root";
    TFile* f = new TFile(fpn.c_str());
    TH3D* src_spec = (TH3D*)f->Get(Form("hunfolded_iter_%s", iter_history[iter_history.size()-1].c_str()));

    return src_spec;
}

void make_spectra_from_niter_no_loop(string src_file_history = "0-1", int niter = 1)
{
    // identify the input spectra to unfold
    TH3D* hdata = getHist2Unfold(src_file_history);
    hdata->Draw();

    // load migration matrix
    TFile file("../syst_specs_cv.root");

    // load the response matrix itself
    ReweightableSpectrum response = *ReweightableSpectrum::LoadFrom(file.GetDirectory("cv/RecoTrue"));
    TH2D* hresponse = response.ToTH2(response.POT());

    // construct empty 3D histograms
    unsigned int nbinsx = angedges.size()-1;
    unsigned int nbinsy = mukeedges.size()-1;
    unsigned int nbinsz = enuedges.size()-1;
    TH3D* hMeas_for_binning = new TH3D("hMeas_for_binning","hMeas_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);
    TH3D* hTrue_for_binning = new TH3D("hTrue_for_binning","hTrue_for_binning",nbinsx,&angedges[0],nbinsy,&mukeedges[0],nbinsz,&enuedges[0]);

    // construct RooUnfold response matrix
    RooUnfoldResponse Response(hMeas_for_binning, hTrue_for_binning, (TH2*)hresponse);

    // output to file
    gROOT->ProcessLine(".! mkdir -p spectra_from_reco");
    TFile* outf = new TFile(Form("spectra_from_reco/spectra_%s-%d.root", src_file_history.c_str(), niter),"recreate");
    hdata->Write("hdata");
    RooUnfoldBayes unfold(&Response, hdata, niter);
    TH3D* hunfold = (TH3D*)unfold.Hreco(RooUnfold::ErrorTreatment::kCovariance);
    hunfold->Write(Form("hunfolded_iter_%d", niter));

    cout << "Unfolding done." << endl;
    outf->Close();
}
#endif