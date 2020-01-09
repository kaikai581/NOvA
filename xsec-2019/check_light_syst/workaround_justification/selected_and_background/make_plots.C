#ifdef __CINT__
void make_plots(int ds_idx = 1)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/Spectrum.h"

#include "TFile.h"
#include "TH2.h"
#include "TPad.h"
#include "TROOT.h"

using namespace ana;

const double pot = 8.09e20;      //data POT

vector<string> syst_names = {"nominal", "lightdown", "lightup", "ckv", "calibneg", "calibpos", "calibshape"};

TH2* relative_error(TH2* href, TH2* hint, string htitle)
{
    TH2D* hres = (TH2D*)hint->Clone("hres");
    hres->Add(href, -1);
    hres->Divide(href);
    hres->SetTitle(htitle.c_str());
    hres->GetXaxis()->CenterTitle();
    hres->GetXaxis()->SetTitle("Reco cos#theta_{#mu}");
    hres->GetYaxis()->CenterTitle();
    hres->GetYaxis()->SetTitle("Reco Muon Kinetic Energy (GeV)");

    return hres;
}

void make_plots(int ds_idx = 1)
{
    string syst_name = syst_names[ds_idx];
    TFile* fnom = new TFile("justification_selnbkg_nominal.root");
    TFile* fsys = new TFile(Form("justification_selnbkg_%s.root", syst_name.c_str()));
    
    // retrieve spectra
    Spectrum* s2dnom = LoadFrom<Spectrum>(fnom->GetDirectory("signal/all_cuts_strict_sig")).release();
    Spectrum* s2dsys = LoadFrom<Spectrum>(fsys->GetDirectory("signal/all_cuts_strict_sig")).release();

    // extract 2d histograms
    TH2* h2dnom = s2dnom->ToTH2(pot);
    TH2* h2dsys = s2dsys->ToTH2(pot);
    TH2* h2dre = relative_error(h2dnom, h2dsys, Form("selected signal (%s - nominal)/nominal", syst_name.c_str()));

    h2dre->SetMaximum(.1);
    h2dre->SetMinimum(-.1);
    h2dre->Draw("colz");

    // save to file
    gROOT->ProcessLine(".! mkdir -p output_plots");
    gPad->SaveAs(Form("output_plots/%s.pdf", syst_name.c_str()));
}
#endif