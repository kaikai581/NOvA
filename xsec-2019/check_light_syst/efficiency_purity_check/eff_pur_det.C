// Have to use S19-02-22 for old binning.

#ifdef __CINT__
void eff_pur_det(int ds_idx = 0)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Vars/PPFXWeights.h"
#include "CAFAna/Vars/XsecTunes.h"

#include "NDAna/numucc_inc/NumuCCIncAnalysis.h"
#include "NDAna/numucc_inc/NumuCCIncCuts.h"

using namespace ana;

const NuTruthVar wgt_xsST = kXSecCVWgt2018_smallerDISScale_NT;
const Var wgt_xs          = VarFromNuTruthVar(wgt_xsST, 1);
const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
const Var wgt             = VarFromNuTruthVar(wgtST, 1);

void eff_pur_det(int ds_idx = 0)
{
    struct DSinfo
    {
        string ds_name;
        string out_fn;
    };

    // datasets to run the script through
    vector<DSinfo> dsinfo = {
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_v1 with stride 7", "eff_pur_nominal.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightdown-calibup_v1_good_seventh", "eff_pur_lightdown.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightup-calibdown_v1_good_seventh", "eff_pur_lightup.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_ckv-proton-shift-down_v1_good_seventh", "eff_pur_ckv.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-neg-offset_v1_good_seventh", "eff_pur_calibneg.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-pos-offset_v1_good_seventh", "eff_pur_calibpos.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-func_v1_good_seventh", "eff_pur_calibshape.root"}
    };

    // make loader and set spill cut
    SpectrumLoader loader(dsinfo[ds_idx].ds_name);
    loader.SetSpillCut(kStandardSpillCuts);

    // make spectra
    pair<Spectrum*,Spectrum*> seff = Efficiency(loader, kTrueMuKEVsCosStandardAxisST, kAllNumuCCCuts, kIsTrueSigST, kNoShift, wgtST);
    Spectrum reco_sel(loader, kRecoMuKEVsCosStandardAxis, kAllNumuCCCuts, kNoShift, wgt);
    Spectrum reco_sel_true(loader, kRecoMuKEVsCosStandardAxis, kAllNumuCCCuts && kIsTrueSig, kNoShift, wgt);

    // let go the loader
    loader.Go();

    // output to file
    TFile fout(dsinfo[ds_idx].out_fn.c_str(), "RECREATE");
    fout.cd();
    seff.first->SaveTo(fout.mkdir("MCSig"));
    seff.second->SaveTo(fout.mkdir("MCSigNuTrue"));
    reco_sel.SaveTo(fout.mkdir("reco_sel"));
    reco_sel_true.SaveTo(fout.mkdir("reco_sel_true"));
}
#endif