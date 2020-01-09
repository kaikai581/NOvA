// Have to use S19-02-22 for old binning.

#ifdef __CINT__
void eff_pur_genie(unsigned int sidx = 0, unsigned int eidx = 99)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Systs/XSecSystLists.h"
#include "CAFAna/XSec/GenieMultiverseSyst.h"

#include "NDAna/numucc_inc/NumuCCIncAnalysis.h"
#include "NDAna/numucc_inc/NumuCCIncCuts.h"

using namespace ana;

const NuTruthVar wgt_xsST = kXSecCVWgt2018_smallerDISScale_NT;
const Var wgt_xs          = VarFromNuTruthVar(wgt_xsST, 1);
const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
const Var wgt             = VarFromNuTruthVar(wgtST, 1);

void eff_pur_genie(unsigned int sidx = 0, unsigned int eidx = 99)
{
    // dataset to run the script through
    string defname = "dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_v1 with stride 7";

    // make loader and set spill cut
    SpectrumLoader loader(defname);
    loader.SetSpillCut(kStandardSpillCuts);

    // construct multiuniverse physics parameters
    GenieMultiverseParameters verse(1000, getAllXsecSysts_2018_RPAFix());
    vector<SystShifts> genie_shifts = verse.GetSystShifts();

    // output containers
    vector<pair<Spectrum*,Spectrum*>> vseff;
    vector<Spectrum*> vs_reco_sel;
    vector<Spectrum*> vs_reco_sel_true;

    // make spectra
    for(unsigned int i = sidx; i <= eidx; i++)
    {
        vseff.push_back(Efficiency(loader, kTrueMuKEVsCosStandardAxisST, kAllNumuCCCuts, kIsTrueSigST, genie_shifts[i], wgtST));
        vs_reco_sel.push_back(new Spectrum(loader, kRecoMuKEVsCosStandardAxis, kAllNumuCCCuts, genie_shifts[i], wgt));
        vs_reco_sel_true.push_back(new Spectrum(loader, kRecoMuKEVsCosStandardAxis, kAllNumuCCCuts && kIsTrueSig, genie_shifts[i], wgt));
    }

    // let go the loader
    loader.Go();

    // output to file
    TFile fout(Form("eff_pur_genie_%d_%d.root",sidx,eidx), "RECREATE");
    fout.cd();
    // seff.first->SaveTo(fout.mkdir("MCSig"));
    // seff.second->SaveTo(fout.mkdir("MCSigNuTrue"));
    // reco_sel.SaveTo(fout.mkdir("reco_sel"));
    // reco_sel_true.SaveTo(fout.mkdir("reco_sel_true"));
    for(unsigned int i = 0; i < vseff.size(); i++)
    {
        fout.cd();
        TDirectory* duni = fout.mkdir(Form("%d",sidx+i));
        vseff[i].first->SaveTo(duni->mkdir("MCSig"));
        vseff[i].second->SaveTo(duni->mkdir("MCSigNuTrue"));
        vs_reco_sel[i]->SaveTo(duni->mkdir("reco_sel"));
        vs_reco_sel_true[i]->SaveTo(duni->mkdir("reco_sel_true"));
    }
}
#endif