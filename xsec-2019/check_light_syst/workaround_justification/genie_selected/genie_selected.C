// Use S19-02-22 for old binning.

#ifdef __CINT__
void genie_selected(int sidx = 0, int eidx = 99, int stride = 1)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Systs/Systs.h"
#include "CAFAna/Systs/XSecSystLists.h"
#include "CAFAna/Vars/PPFXWeights.h"
#include "CAFAna/Vars/XsecTunes.h"
#include "CAFAna/XSec/GenieMultiverseSyst.h"

#include "NDAna/numucc_inc/NumuCCIncCuts.h"

using namespace ana;

const NuTruthVar wgt_xsST = kXSecCVWgt2018_smallerDISScale_NT;
const Var wgt_xs          = VarFromNuTruthVar(wgt_xsST, 1);
const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
const Var wgt             = VarFromNuTruthVar(wgtST, 1);

void genie_selected(int sidx = 0, int eidx = 99, int stride = 1)
{
    string ds_defname = "dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_v1 with stride " + to_string(stride);

    GenieMultiverseParameters verse(1000, getAllXsecSysts_2018_RPAFix());
    vector<SystShifts> genie_shifts = verse.GetSystShifts();

    // make loader and set spill cut
    SpectrumLoader loader(ds_defname);
    loader.SetSpillCut(kStandardSpillCuts);

    // spectra container
    map<int, Spectrum*> uni_spec_selsig;
    map<int, Spectrum*> uni_spec_selbkg;

    // set up spectra
    for(int i = sidx; i <= eidx; i++){
        uni_spec_selsig[i] = new Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts && kIsNumuCCCut, genie_shifts[i], wgt);
        uni_spec_selbkg[i] = new Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts && !kIsNumuCCCut, genie_shifts[i], wgt);
    }

    // let go the loader
    loader.Go();

    // output to file
    TFile fout(Form("genie_univ_%d_%d_stride%d.root", sidx, eidx, stride), "recreate");
    fout.cd();
    TDirectory* dsig = fout.mkdir("signal");
    TDirectory* dbkg = fout.mkdir("background");
    for(int i = sidx; i <= eidx; i++){
        uni_spec_selsig[i]->SaveTo(dsig->mkdir(Form("univ%d", i)));
        uni_spec_selbkg[i]->SaveTo(dbkg->mkdir(Form("univ%d", i)));
    }
}
#endif