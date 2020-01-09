#ifdef __CINT__
void make_det_plots(int ds_idx = 1)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/Spectrum.h"

#include "NDAna/numucc_inc/NumuCCIncVars.h"

#include "TPad.h"
#include "TROOT.h"

using namespace ana;

const double pot = 8.09e20;      //data POT

vector<string> syst_names = {"nominal","lightdown","lightup","ckv","calibneg","calibpos","calibshape"};

string syst_name;
int syst_idx;

TH2* get_eff(TFile* tinf)
{
    Spectrum* sselsig = LoadFrom<Spectrum>(tinf->GetDirectory("MCSig")).release();
    Spectrum* struesig = LoadFrom<Spectrum>(tinf->GetDirectory("MCSigNuTrue")).release();
    TH2* hselsig = ana::ToTH2(*sselsig, pot, kPOT, angbinsCustom, mukebins);
    TH2* htruesig = ana::ToTH2(*struesig, pot, kPOT, angbinsCustom, mukebins);

    TH2* heff = (TH2*)hselsig->Clone();
    heff->Divide(htruesig);

    return heff;
}

TH2* get_pur(TFile* tinf)
{
    Spectrum* snum = LoadFrom<Spectrum>(tinf->GetDirectory("reco_sel_true")).release();
    Spectrum* sdenum = LoadFrom<Spectrum>(tinf->GetDirectory("reco_sel")).release();
    TH2* hnum = ana::ToTH2(*snum, pot, kPOT, angbinsCustom, mukebins);
    TH2* hdenum = ana::ToTH2(*sdenum, pot, kPOT, angbinsCustom, mukebins);

    TH2* hpur = (TH2*)hnum->Clone();
    hpur->Divide(hdenum);

    return hpur;
}

void make_relative_error(TH2* hdenum, TH2* hnum, string hname)
{
    TH2* hre = (TH2*)hnum->Clone(hname.c_str());
    hre->SetTitle(Form("(%s_{%s}-%s_{nominal})/%s_{nominal}", hname.c_str(), syst_name.c_str(), hname.c_str(), hname.c_str()));
    hre->Add(hdenum, -1);
    hre->Divide(hdenum);
    hre->SetMaximum(.15);
    hre->SetMinimum(-.15);
    hre->GetXaxis()->CenterTitle();
    hre->GetXaxis()->SetTitle("True cos#theta_{#mu}");
    if(hname == "pur") hre->GetXaxis()->SetTitle("Reco cos#theta_{#mu}");
    hre->GetYaxis()->CenterTitle();
    hre->GetYaxis()->SetTitle("True Muon Kinetic Energy (GeV)");
    if(hname == "pur") hre->GetYaxis()->SetTitle("Reco Muon Kinetic Energy (GeV)");
    hre->Draw("colz");
    gPad->SetRightMargin(0.18);

    // save to file
    gROOT->ProcessLine(".! mkdir -p output_plots");
    gPad->SaveAs(Form("output_plots/%d_rel_err_%s_%s.pdf", syst_idx, hname.c_str(), syst_name.c_str()));
}

void make_det_plots(int ds_idx = 1)
{
    // retrieve data
    syst_idx = ds_idx;
    syst_name = syst_names[ds_idx];
    TFile* fnom = new TFile("grid_output/eff_pur_nominal.root");
    TFile* fsys = new TFile(Form("grid_output/eff_pur_%s.root", syst_name.c_str()));

    // calculate efficiencies
    TH2* heffnom = get_eff(fnom);
    TH2* heffsys = get_eff(fsys);

    // calculate purity
    TH2* hpurnom = get_pur(fnom);
    TH2* hpursys = get_pur(fsys);

    // make relative errors
    make_relative_error(heffnom, heffsys, "eff");
    make_relative_error(hpurnom, hpursys, "pur");
}
#endif