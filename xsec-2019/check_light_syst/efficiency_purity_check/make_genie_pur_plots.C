#ifdef __CINT__
void make_genie_pur_plots(int sidx = 100, int eidx = 799)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/Ratio.h"

#include "NDAna/numucc_inc/NumuCCIncVars.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"
#include "TH3.h"
#include "TPad.h"

using namespace ana;

const double pot = 8.09e20;      //data POT

void get_up_down_sigma(vector<float>& vals, float& up, float& down)
{
    vector<float> upvals, downvals;

    for(unsigned int i = 1; i < vals.size(); i++)
    {
        float diff = vals[i]-vals[0];
        if(diff > 0) upvals.push_back(diff);
        else downvals.push_back(-diff);
    }

    sort(upvals.begin(), upvals.end());
    sort(downvals.begin(), downvals.end());

    up = 0;
    down = 0;

    if(upvals.size())
    {
        int pos = (float)upvals.size()*.68;
        up = upvals[pos];
    }

    if(downvals.size())
    {
        int pos = (float)downvals.size()*.68;
        down = downvals[pos];
    }

    // cout << up << " " << down << endl;
}

void make_genie_pur_plots(int sidx = 100, int eidx = 799)
{

    // retrieve data
    // for now, there are 125 genie files, each one having 4 universes
    vector<TH2*> hpurs;
    for(int i = 1; i <= 7; i++)
    {
        TFile* fgenie = new TFile(Form("grid_output/eff_pur_genie_%d_%d.root",i*100,i*100+99));
        for(int j = 100*i; j < 100*(i+1); j++)
        {
            Spectrum* ssel = LoadFrom<Spectrum>(fgenie->GetDirectory(Form("%d/reco_sel_true",j))).release();
            TH2* hsel = (TH2*)ToTH2(*ssel, pot, kPOT, angbinsCustom, mukebins);
            Spectrum* strue = LoadFrom<Spectrum>(fgenie->GetDirectory(Form("%d/reco_sel",j))).release();
            TH2* htrue = (TH2*)ToTH2(*strue, pot, kPOT, angbinsCustom, mukebins);
            hsel->Divide(htrue);
            hpurs.push_back(hsel);
        }
    }

    // configure histogram
    hpurs[0]->GetXaxis()->CenterTitle();
    hpurs[0]->GetXaxis()->SetTitle("Reco cos#theta_{#mu}");
    hpurs[0]->GetYaxis()->CenterTitle();
    hpurs[0]->GetYaxis()->SetTitle("Reco Muon Kinetic Energy (GeV)");
    hpurs[0]->SetMaximum(.08);

    // results containers
    TH2* hup = (TH2*)hpurs[0]->Clone("hup");
    hup->SetTitle("GENIE (pur_{up}-pur_{nominal})/pur_{nominal}");
    TH2* hdown = (TH2*)hpurs[0]->Clone("hdown");
    hdown->SetTitle("GENIE (pur_{nominal}-pur_{down})/pur_{nominal}");
    int nbinsx = hup->GetNbinsX();
    int nbinsy = hup->GetNbinsY();
    
    for(int i = 1; i <= nbinsx; i++)
        for(int j = 1; j <= nbinsy; j++)
        {
            float up, down;
            vector<float> bincontents;
            for(unsigned int k = 0; k < hpurs.size(); k++)
                bincontents.push_back(hpurs[k]->GetBinContent(i,j));
            get_up_down_sigma(bincontents, up, down);
            hup->SetBinContent(i,j,up);
            hdown->SetBinContent(i,j,down);
        }
    
    // make plots
    double zscaleup = .3;
    TCanvas* canv = new TCanvas("canv","canv", 1000, 450);
    canv->Divide(2,1);
    canv->cd(1);
    gPad->SetRightMargin(0.18);
    hup->Divide(hpurs[0]);
    hup->SetMaximum(zscaleup);
    hup->SetMinimum(-.15);
    hup->Draw("colz");
    canv->cd(2);
    gPad->SetRightMargin(0.18);
    hdown->Divide(hpurs[0]);
    hdown->SetMaximum(zscaleup);
    hdown->SetMinimum(-.15);
    hdown->Draw("colz");
    canv->SaveAs("output_plots/genie_pur.pdf");
}
#endif