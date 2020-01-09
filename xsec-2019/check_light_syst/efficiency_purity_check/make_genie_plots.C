#ifdef __CINT__
void make_genie_plots()
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

void make_genie_plots()
{

    // retrieve data
    // for now, there are 125 genie files, each one having 4 universes
    vector<TH2*> heffs;
    for(int i = 0; i < 125; i++)
    {
        TFile* fgenie = new TFile(Form("/nova/ana/users/slin/temp/numuccinc/genie_univ_%d_to_%d.root",i*4,i*4+3));
        for(int j = 4*i; j < 4*(i+1); j++)
        {
            Spectrum* ssel = LoadFrom<Spectrum>(fgenie->GetDirectory(Form("genie%d/MCSig",j))).release();
            TH2* hsel = (TH2*)(ToTH3(*ssel, pot, kPOT, angbinsCustom, mukebins, enubins)->Project3D("yx"));
            Spectrum* strue = LoadFrom<Spectrum>(fgenie->GetDirectory(Form("genie%d/MCSigNuTre",j))).release();
            TH2* htrue = (TH2*)(ToTH3(*strue, pot, kPOT, angbinsCustom, mukebins, enubins)->Project3D("yx"));
            hsel->Divide(htrue);
            heffs.push_back(hsel);
        }
    }

    // configure histogram
    heffs[0]->GetXaxis()->CenterTitle();
    heffs[0]->GetXaxis()->SetTitle("True cos#theta_{#mu}");
    heffs[0]->GetYaxis()->CenterTitle();
    heffs[0]->GetYaxis()->SetTitle("True Muon Kinetic Energy (GeV)");
    heffs[0]->SetMaximum(.08);

    // results containers
    TH2* hup = (TH2*)heffs[0]->Clone("hup");
    hup->SetTitle("GENIE (eff_{up}-eff_{nominal})/eff_{nominal}");
    TH2* hdown = (TH2*)heffs[0]->Clone("hdown");
    hdown->SetTitle("GENIE (eff_{nominal}-eff_{down})/eff_{nominal}");
    int nbinsx = hup->GetNbinsX();
    int nbinsy = hup->GetNbinsY();
    
    for(int i = 1; i <= nbinsx; i++)
        for(int j = 1; j <= nbinsy; j++)
        {
            float up, down;
            vector<float> bincontents;
            for(unsigned int k = 0; k < heffs.size(); k++)
                bincontents.push_back(heffs[k]->GetBinContent(i,j));
            get_up_down_sigma(bincontents, up, down);
            hup->SetBinContent(i,j,up);
            hdown->SetBinContent(i,j,down);
        }
    
    // make plots
    TCanvas* canv = new TCanvas("canv","canv", 1000, 450);
    canv->Divide(2,1);
    canv->cd(1);
    gPad->SetRightMargin(0.18);
    hup->Divide(heffs[0]);
    hup->SetMaximum(.15);
    hup->SetMinimum(-.15);
    hup->Draw("colz");
    canv->cd(2);
    gPad->SetRightMargin(0.18);
    hdown->Divide(heffs[0]);
    hdown->SetMaximum(.15);
    hdown->SetMinimum(-.15);
    hdown->Draw("colz");
    canv->SaveAs("output_plots/genie.pdf");
}
#endif