#ifdef __CINT__
void extract_hade(unsigned int ds_idx = 0, bool use_common_files = false)
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

const Var kTrueHadE = VarFromNuTruthVar(kTrueHadEST);

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

const Var kExtraHadE([](const caf::SRProxy* sr)
                     {
                         int ibesttrk = kBestTrack(sr);
                         if(sr->trk.kalman.ntracks < 1) return -1000.f;
                         if(ibesttrk < 0 || ibesttrk >= int(sr->trk.kalman.ntracks)) return -1000.f;
                         // Extra energy (hadronic contamination) associated with muon track
                         return float(sr->trk.kalman.tracks[ibesttrk].overlapE);
                     });

const Var kCalhadE([](const caf::SRProxy* sr)
                   {
                       int ibesttrk = kBestTrack(sr);
                       if(sr->trk.kalman.ntracks < 1) return -1000.f;
                       if(ibesttrk < 0 || ibesttrk >= int(sr->trk.kalman.ntracks)) return -1000.f;
                       // Extra energy (hadronic contamination) associated with muon track
                       return float(sr->slc.calE - sr->trk.kalman.tracks[ibesttrk].calE);
                   });

const Var kOscVisHadE([](const caf::SRProxy* sr)
                     {
                          if (sr->trk.kalman.ntracks == 0) return -1000.f;
                          return float(sr->energy.numu.hadtrkE + sr->energy.numu.hadcalE);
                     });

const Var kOscHadTrkE([](const caf::SRProxy* sr)
                     {
                          if (sr->trk.kalman.ntracks == 0) return -1000.f;
                          return float(sr->energy.numu.hadtrkE);
                     });

const Var kOscHadCalE([](const caf::SRProxy* sr)
                     {
                          if (sr->trk.kalman.ntracks == 0) return -1000.f;
                          return float(sr->energy.numu.hadcalE);
                     });


vector<string> syst_names = {"nominal","lightdown","lightup","ckv","calibneg","calibpos","calibshape"};

vector<string> get_file_list(int ds_idx)
{
  vector<string> fns;
  string list_filename = "../lists_common_files/"+syst_names[ds_idx]+".txt";
  ifstream list_file(list_filename);
  string tmpstr;

  while(list_file >> tmpstr) fns.push_back(tmpstr);

  return fns;
}

void extract_hade(unsigned int ds_idx = 0, bool use_common_files = false)
{
    struct DSinfo
    {
        string ds_name;
        string out_fpn;
    };
    // datasets to extract values, including nominal and systematic datasets.
    vector<DSinfo> dsinfo = {
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_v1", "roounfold_study_nominal.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightdown-calibup_v1", "roounfold_study_lightdown.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightup-calibdown_v1", "roounfold_study_lightup.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_ckv-proton-shift-down_v1", "roounfold_study_ckv.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-neg-offset_v1", "roounfold_study_calibneg.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-pos-offset_v1", "roounfold_study_calibpos.txt"},
        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-func_v1", "roounfold_study_calibshape.txt"},

        {"dataset_def_name_newest_snapshot prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_v1", "ndrespin_nominal.txt"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightdown-calibup_v1_good_seventh", "ndrespin_lightdown.txt"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightup-calibdown_v1_good_seventh", "ndrespin_lightup.txt"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_ckv-proton-shift-down_v1_good_seventh", "ndrespin_ckv.txt"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-neg-offset_v1_good_seventh", "ndrespin_calibneg.txt"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-pos-offset_v1_good_seventh", "ndrespin_calibpos.txt"},
        {"prod_caf_R17-11-14-prod4reco.remid-hotfix.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-func_v1_good_seventh", "ndrespin_calibshape.txt"}
    };

    // make sure the user is not crzay
    if(ds_idx >= dsinfo.size()){
        cerr << "Invalid script argument. There are only " << dsinfo.size() << " datasets." << endl;
       return;
    }

    // extract!
    if(!use_common_files)
        MakeTextListFile(dsinfo[ds_idx].ds_name, {kNumuSelected}, {dsinfo[ds_idx].out_fpn},
                        {&kRecoMuCostheta, &kRecoMuKE, &kRecoE, &kNumuE2018ND, &kRecoHadE,
                        &kVisHadE, // xsec visiable hardonic energy
                        &kExtraHadE, // xsec trkhadE
                        &kCalhadE, // xsec trkcalE
                        &kOscVisHadE, // oscillation visiable hardonic energy
                        &kOscHadTrkE, // oscillation trkhadE
                        &kOscHadCalE, // oscillation trkcalE
                        &kTrueMuCostheta, &kTrueMuKE, &kTrueE, &kTrueHadE, &wgt,
                        &kPassRecoMuKECut, &kPassRecoMuCosCut, &kPassRecoMuonPhaseSpaceCut, &kPassIsTrueSig},
                        &kStandardSpillCuts);
    else{
        if(ds_idx >= syst_names.size()){
            cout << "Dataset index out of range." << endl;
            return;
        }
        vector<string> fns = get_file_list(ds_idx);
        MakeTextListFile(fns, {kNumuSelected}, {"common_files_"+syst_names[ds_idx]+".txt"},
                        {&kRecoMuCostheta, &kRecoMuKE, &kRecoE, &kNumuE2018ND, &kRecoHadE,
                        &kVisHadE, // xsec visiable hardonic energy
                        &kExtraHadE, // xsec trkhadE
                        &kCalhadE, // xsec trkcalE
                        &kOscVisHadE, // oscillation visiable hardonic energy
                        &kOscHadTrkE, // oscillation trkhadE
                        &kOscHadCalE, // oscillation trkcalE
                        &kTrueMuCostheta, &kTrueMuKE, &kTrueE, &kTrueHadE, &wgt,
                        &kPassRecoMuKECut, &kPassRecoMuCosCut, &kPassRecoMuonPhaseSpaceCut, &kPassIsTrueSig},
                        &kStandardSpillCuts);
    }
}
#endif