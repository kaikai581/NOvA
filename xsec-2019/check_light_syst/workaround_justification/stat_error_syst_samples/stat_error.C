#ifdef __CINT__
void stat_error(int ds_idx = 1)
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

void stat_error(int ds_idx = 1)
{
    string syst_name = syst_names[ds_idx];
    TFile* fsys = new TFile(Form("../selected_and_background/justification_selnbkg_%s.root", syst_name.c_str()));
    
    // retrieve spectra
    Spectrum* s2dsys = LoadFrom<Spectrum>(fsys->GetDirectory("signal/all_cuts_strict_sig")).release();

    // extract 2d histograms
    TH2* h2dsys = s2dsys->ToTH2(s2dsys->POT());
    
    // make stat error histogram
    TH2* h2dstst = (TH2*)h2dsys->Clone("h2dstst");
    int nbinsx = h2dstst->GetNbinsX();
    int nbinsy = h2dstst->GetNbinsY();

    // set bin content as the error
    for(int i = 1; i <= nbinsx; i++)
        for(int j = 1; j <= nbinsy; j++)
        {
            double val = (h2dsys->GetBinErrorLow(i,j)+h2dsys->GetBinErrorUp(i,j))/2/h2dsys->GetBinContent(i,j);
            h2dstst->SetBinContent(i,j,val);
        }

    h2dstst->SetTitle(Form("relative stat error for %s", syst_name.c_str()));
    h2dstst->GetXaxis()->CenterTitle();
    h2dstst->GetXaxis()->SetTitle("Reco cos#theta_{#mu}");
    h2dstst->GetYaxis()->CenterTitle();
    h2dstst->GetYaxis()->SetTitle("Reco Muon Kinetic Energy (GeV)");
    h2dstst->SetMaximum(.1);
    h2dstst->Draw("colz");

    // save to file
    gROOT->ProcessLine(".! mkdir -p output_plots");
    gPad->SaveAs(Form("output_plots/%s.pdf", syst_name.c_str()));
}
#endif