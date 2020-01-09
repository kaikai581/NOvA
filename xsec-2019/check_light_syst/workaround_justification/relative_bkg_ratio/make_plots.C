#ifdef __CINT__
void make_plots(int var_idx = 0)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

// CAFAna includes
#include "CAFAna/Core/EventList.h"
#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/MultiVar.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Cuts/NumuCuts2018.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Vars/BPFVars.h"
#include "CAFAna/Vars/CVNProngVars.h"
#include "CAFAna/Vars/NumuEFxs.h"
#include "CAFAna/Vars/NumuVars.h"
#include "CAFAna/Vars/PPFXWeights.h"
#include "CAFAna/Vars/TruthVars.h"
#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Vars/WrongSignBDTVars.h"
#include "CAFAna/Vars/XsecTunes.h" // Only use this include with development
#include "NDAna/numucc_inc/NumuCCIncCuts.h"

#include "TCanvas.h"
#include "TROOT.h"

using namespace ana;

const double pot = 8.09e20;      //data POT

vector<string> var_names = {"bkg_no_ph_cut_loose_sig", "bkg_all_cuts_strict_sig", "bkg_no_ph_cut_strict_sig", "bkg_all_cuts_loose_sig"};

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

void make_plots(int var_idx = 0)
{
    TFile* fnom = new TFile("justification_nominal.root");
    TFile* fup = new TFile("justification_lightup.root");
    TFile* fdown = new TFile("justification_lightdown.root");

    string var_name = var_names[var_idx];

    // retrieve spectra
    Spectrum* snom = LoadFrom<Spectrum>(fnom->GetDirectory(var_name.c_str())).release();
    Spectrum* sup = LoadFrom<Spectrum>(fup->GetDirectory(var_name.c_str())).release();
    Spectrum* sdown = LoadFrom<Spectrum>(fdown->GetDirectory(var_name.c_str())).release();

    // make spectra into histograms
    TH2* hnom = snom->ToTH2(pot);
    TH2* hup = sup->ToTH2(pot);
    TH2* hdown = sdown->ToTH2(pot);

    // make relative error histograms
    TH2* hreup = relative_error(hnom, hup, "background (lightup-nominal)/nominal");
    TH2* hredown = relative_error(hnom, hdown, "background (lightdown-nominal)/nominal");

    // draw!
    TCanvas* cup = new TCanvas();
    hreup->GetXaxis()->SetRangeUser(.5,1);
    hreup->Draw("colz");
    TCanvas* cdown = new TCanvas();
    hredown->GetXaxis()->SetRangeUser(.5,1);
    hredown->Draw("colz");

    // save to file
    gROOT->ProcessLine(".! mkdir -p output_plots");
    cup->SaveAs("output_plots/lightup.pdf");
    cdown->SaveAs("output_plots/lightdown.pdf");
}
#endif