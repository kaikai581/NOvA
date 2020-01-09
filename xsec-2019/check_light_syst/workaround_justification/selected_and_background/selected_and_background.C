// Have to use S19-02-22 for old binning.

#ifdef __CINT__
void selected_and_background(int ds_idx = 0)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

// CAFAna includes
#include "CAFAna/Core/EventList.h"
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

using namespace ana;

const NuTruthVar wgt_xsST = kXSecCVWgt2018_smallerDISScale_NT;
const Var wgt_xs          = VarFromNuTruthVar(wgt_xsST, 1);
const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
const Var wgt             = VarFromNuTruthVar(wgtST, 1);

const Cut kNumuSelected = kNumuMyQuality && kNumuTightContainND && kIsFiducial && kMuonIDCut;

void selected_and_background(int ds_idx = 0)
{
    struct DSinfo
    {
        string ds_name;
        string out_fn;
    };

    // datasets to run the script through
    vector<DSinfo> dsinfo = {
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_v1", "justification_selnbkg_nominal.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightdown-calibup_v1_good_seventh", "justification_selnbkg_lightdown.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightup-calibdown_v1_good_seventh", "justification_selnbkg_lightup.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_ckv-proton-shift-down_v1_good_seventh", "justification_selnbkg_ckv.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-neg-offset_v1_good_seventh", "justification_selnbkg_calibneg.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-pos-offset_v1_good_seventh", "justification_selnbkg_calibpos.root"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-func_v1_good_seventh", "justification_selnbkg_calibshape.root"}
    };

    // make loader and set spill cut
    SpectrumLoader loader(dsinfo[ds_idx].ds_name);
    loader.SetSpillCut(kStandardSpillCuts);

    // make spectrum
    // background
    Spectrum sbkg_no_ph_cut_loose_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kNumuSelected && !kIsNumuCCCut, kNoShift, wgt);
    Spectrum sbkg_all_cuts_strict_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts && !kIsTrueSig, kNoShift, wgt);
    Spectrum sbkg_no_ph_cut_strict_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kNumuSelected && !kIsTrueSig, kNoShift, wgt);
    Spectrum sbkg_all_cuts_loose_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts && !kIsNumuCCCut, kNoShift, wgt);

    // selected
    Spectrum ssel_no_ph_cut = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kNumuSelected, kNoShift, wgt);
    Spectrum ssel_all_cuts = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts, kNoShift, wgt);

    // selected signal
    Spectrum ssig_no_ph_cut_loose_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kNumuSelected && kIsNumuCCCut, kNoShift, wgt);
    Spectrum ssig_all_cuts_strict_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts && kIsTrueSig, kNoShift, wgt);
    Spectrum ssig_no_ph_cut_strict_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kNumuSelected && kIsTrueSig, kNoShift, wgt);
    Spectrum ssig_all_cuts_loose_sig = Spectrum("Reconstructed T_{#mu} vs cos #{theta};T_{#mu} [GeV];cos #{theta}", loader, angbinsCustom, kRecoMuCostheta, mukebins, kRecoMuKE, kAllNumuCCCuts && kIsNumuCCCut, kNoShift, wgt);

    // let go the loader
    loader.Go();

    // output to file
    TFile fout(dsinfo[ds_idx].out_fn.c_str(), "RECREATE");
    fout.cd();
    TDirectory* dbkg = fout.mkdir("background");
    sbkg_no_ph_cut_loose_sig.SaveTo(dbkg->mkdir("no_ph_cut_loose_sig"));
    fout.cd();
    sbkg_all_cuts_strict_sig.SaveTo(dbkg->mkdir("all_cuts_strict_sig"));
    fout.cd();
    sbkg_no_ph_cut_strict_sig.SaveTo(dbkg->mkdir("no_ph_cut_strict_sig"));
    fout.cd();
    sbkg_all_cuts_loose_sig.SaveTo(dbkg->mkdir("all_cuts_loose_sig"));
    fout.cd();
    TDirectory* dsel = fout.mkdir("selected");
    ssel_no_ph_cut.SaveTo(dsel->mkdir("no_ph_cut"));
    ssel_all_cuts.SaveTo(dsel->mkdir("all_cuts"));
    fout.cd();
    TDirectory* dsig = fout.mkdir("signal");
    ssig_no_ph_cut_loose_sig.SaveTo(dsig->mkdir("no_ph_cut_loose_sig"));
    fout.cd();
    ssig_all_cuts_strict_sig.SaveTo(dsig->mkdir("all_cuts_strict_sig"));
    fout.cd();
    ssig_no_ph_cut_strict_sig.SaveTo(dsig->mkdir("no_ph_cut_strict_sig"));
    fout.cd();
    ssig_all_cuts_loose_sig.SaveTo(dsig->mkdir("all_cuts_loose_sig"));
}
#endif