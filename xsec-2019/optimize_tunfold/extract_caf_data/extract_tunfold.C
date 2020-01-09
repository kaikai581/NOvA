#ifdef __CINT__
void extract_tunfold(unsigned int ds_idx = 0)
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
const Var wgt_ppfx        = VarFromNuTruthVar(kPPFXFluxCVWgtST, 1);
const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
const Var wgt             = VarFromNuTruthVar(wgtST, 1);

const Cut kNumuSelected = kNumuMyQuality && kNumuTightContainND && kIsFiducial && kMuonIDCut;

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

void extract_tunfold(unsigned int ds_idx = 0)
{
    struct DSinfo
    {
        string ds_name;
        string out_fpn;
    };
    // datasets to extract values, including nominal and systematic datasets.
    vector<DSinfo> dsinfo = {
        {"dataset_def_name_newest_snapshot nd_fhc_cvnprongrespin_caf_fake_data", "tunfold_study_fakedata.txt"},
        {"dataset_def_name_newest_snapshot nd_fhc_cvnprongrespin_caf_minus_muonid_training_minus_fakedata", "tunfold_study_nominal.txt"}
    };

    // make sure the user is not crzay
    if(ds_idx >= dsinfo.size()){
        cerr << "Invalid script argument. There are only " << dsinfo.size() << " datasets." << endl;
       return;
    }

    // extract!
    MakeTextListFile(dsinfo[ds_idx].ds_name, {kNumuSelected}, {dsinfo[ds_idx].out_fpn},
                    {&kRecoMuCostheta, &kRecoMuKE, &kRecoE, &kRecoEavail, &kNumuE2018ND, &kRecoHadE, &kTrueMuCostheta, &kTrueMuKE, &kTrueE, &kTrueEavail, &kTrueHadE, &wgt_xs, &wgt_ppfx, &kPassRecoMuKECut, &kPassRecoMuCosCut, &kPassRecoMuonPhaseSpaceCut, &kPassIsTrueSig},
                    &kStandardSpillCuts);
}
#endif