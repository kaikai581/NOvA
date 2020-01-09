#ifdef __CINT__
void extract_reco_true(unsigned int ds_idx = 0)
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

// CAFAna includes
#include "CAFAna/Core/EventList.h"
#include "CAFAna/Core/MultiVar.h"
#include "CAFAna/Cuts/NumuCuts2018.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Vars/BPFVars.h"
#include "CAFAna/Vars/CVNProngVars.h"
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
const Var kTrueHadE       = VarFromNuTruthVar(kTrueHadEST);

const Var kPassNumuMyQuality([](const caf::SRProxy* sr)
                             {
                                 return kNumuMyQuality(sr);
                             });

const Var kPassNumuTightContainND([](const caf::SRProxy* sr)
                                  {
                                      return kNumuTightContainND(sr);
                                  });

const Var kPassMuonIDCut([](const caf::SRProxy* sr)
                         {
                              return kMuonIDCut(sr);
                         });

const Var kPassIsFiducial([](const caf::SRProxy* sr)
                         {
                              return kIsFiducial(sr);
                         });

const Var kPassRecoMuKECut([](const caf::SRProxy* sr)
                           {
                                return kRecoMuKECut(sr);
                           });

const Var kPassRecoMuCosCut([](const caf::SRProxy* sr)
                            {
                                 return kRecoMuCosCut(sr);
                            });

const Var kPassRecoMuonPhaseSpaceCut([](const caf::SRProxy* sr)
                                     {
                                          return kRecoMuonPhaseSpaceCut(sr);
                                     });

const Var kPassIsTrueSig([](const caf::SRProxy* sr)
                         {
                             return kIsTrueSig(sr);
                         });

void extract_reco_true(unsigned int ds_idx = 0)
{
    struct DSinfo
    {
        string ds_name;
        string out_fpn;
    };
    // datasets to extract values, including nominal and systematic datasets.
    vector<DSinfo> dsinfo = {
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_v1", "unfolding_study_nominal.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightdown-calibup_v1", "unfolding_study_lightup.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightup-calibdown_v1", "unfolding_study_lightdown.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_ckv-proton-shift-down_v1", "unfolding_study_ckv.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-neg-offset_v1", "unfolding_study_calibneg.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-pos-offset_v1", "unfolding_study_calibpos.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-func_v1", "unfolding_study_calibshape.txt"}
    };

    // make sure the user is not crzay
    if(ds_idx >= dsinfo.size()){
        cerr << "Invalid script argument. There are only " << dsinfo.size() << " datasets." << endl;
       return;
    }

    // extract!
    MakeTextListFile(dsinfo[ds_idx].ds_name, {kNoCut}, {dsinfo[ds_idx].out_fpn},
                    {&kRecoMuCostheta, &kRecoMuKE, &kRecoE, &kRecoHadE, &kTrueMuCostheta, &kTrueMuKE, &kTrueE, &kTrueHadE, &wgt,
                     &kPassNumuMyQuality, &kPassNumuTightContainND, &kPassMuonIDCut, &kPassIsFiducial, &kPassRecoMuKECut,
                     &kPassRecoMuCosCut, &kPassRecoMuonPhaseSpaceCut, &kPassIsTrueSig},
                    &kStandardSpillCuts);
}
#endif