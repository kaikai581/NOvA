#ifdef __CINT__
void make_plots()
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/Spectrum.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"

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

void make_plots()
{
    // open files
    vector<TFile*> infs;
    for(int i = 0; i < 10; i++)
        infs.push_back(new TFile(Form("genie_univ_%d_%d_stride7.root", i*100, i*100+99)));
    
    // retrieve spectra
    vector<Spectrum*> s2d_selsigs;
    vector<TH2*> h2d_selsigs;
    for(int i = 0; i < 10; i++)
    {
        int sidx = i*100;
        int eidx = i*100+99;
        for(int j = sidx; j <= eidx; j++)
        {
            Spectrum* stemp = LoadFrom<Spectrum>(infs[i]->GetDirectory(Form("signal/univ%d", j))).release();
            s2d_selsigs.push_back(stemp);
            TH2* htemp = stemp->ToTH2(pot);
            h2d_selsigs.push_back(htemp);
        }
    }

    // configure histogram
    h2d_selsigs[0]->GetXaxis()->CenterTitle();
    h2d_selsigs[0]->GetXaxis()->SetTitle("Reco cos#theta_{#mu}");
    h2d_selsigs[0]->GetYaxis()->CenterTitle();
    h2d_selsigs[0]->GetYaxis()->SetTitle("Reco Muon Kinetic Energy (GeV)");
    h2d_selsigs[0]->SetMaximum(.3);

    // results containers
    TH2* hup = (TH2*)h2d_selsigs[0]->Clone("hup");
    hup->SetTitle("GENIE relative #sigma_{up}");
    TH2* hdown = (TH2*)h2d_selsigs[0]->Clone("hdown");
    hdown->SetTitle("GENIE relative #sigma_{down}");
    int nbinsx = hup->GetNbinsX();
    int nbinsy = hup->GetNbinsY();
    
    for(int i = 1; i <= nbinsx; i++)
        for(int j = 1; j <= nbinsy; j++)
        {
            float up, down;
            vector<float> bincontents;
            for(unsigned int k = 0; k < h2d_selsigs.size(); k++)
                bincontents.push_back(h2d_selsigs[k]->GetBinContent(i,j));
            get_up_down_sigma(bincontents, up, down);
            hup->SetBinContent(i,j,up);
            hdown->SetBinContent(i,j,down);
        }
    
    // make plots
    TCanvas* canv = new TCanvas("canv","canv", 1000, 450);
    canv->Divide(2,1);
    canv->cd(1);
    gPad->SetRightMargin(0.18);
    hup->Divide(h2d_selsigs[0]);
    hup->Draw("colz");
    canv->cd(2);
    gPad->SetRightMargin(0.18);
    hdown->Divide(h2d_selsigs[0]);
    hdown->Draw("colz");
    canv->SaveAs("genie.pdf");
}
#endif