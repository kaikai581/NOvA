#ifdef __CINT__
void make_spectra_to_unfold(const string outfile = "fSystSpec.root")
{
    std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Vars/GenieWeights.h"
#include "CAFAna/Vars/PPFXWeights.h"

#include "NDAna/numucc_inc/NumuCCIncAnalysis.h"

using namespace ana;

// Bkgd is defined as !kIsTrueSig
const std::vector<Cut> bkgd_cuts = {!kIsTrueSig};

void make_spectra_to_unfold(const string outfile = "fSystSpec.root")
{
    // structure utility
    struct DSInfo{
        string defname;
        string foldername;
    };
    // define dataset to run through
    // using 40% of the MC which is not used for muon ID and construction of the unfolding matrix
    vector<DSInfo> dsets_info{
        {"dataset_def_name_newest_snapshot nd_fhc_cvnprongrespin_caf_fake_data", "nominal"}
    };

    // container definitions
    vector<pair<Spectrum*,Spectrum*>> veff;
    // vector<Spectrum*> vMCSig;
    // vector<Spectrum*> vMCSigNuTrue;
    vector<Spectrum*> vMCSel_Reco;
    vector<Spectrum*> vMCSig_Reco;
    vector<ReweightableSpectrum*> vRecoTrue;
    vector<TrivialBkgdEstimator*> vbkg;
    // vector<IBkgdEstimator*> vBkgdEst;
    vector<SpectrumLoaderBase*> vloaders;

    // Define the weights. Build off of each other, and default to "1".
    const NuTruthVar wgt_xsST = kXSecCVWgt2018_smallerDISScale_NT;
    const Var wgt_xs          = VarFromNuTruthVar(wgt_xsST, 1);
    const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
    const Var wgt             = VarFromNuTruthVar(wgtST, 1);

    for(unsigned int i = 0; i < dsets_info.size(); i++)
    {
        // cout << dsets_info[i].defname << endl;
        vloaders.push_back(new SpectrumLoader(dsets_info[i].defname));

        // define spectra
        veff.push_back(Efficiency(*vloaders[i], kTrueMuKEVsCosVsEnuStandardAxisST, kAllNumuCCCuts, kIsTrueSigST, kNoShift, wgtST));
        vbkg.push_back(new TrivialBkgdEstimator(*vloaders[i], kRecoMuKEVsCosVsEnuStandardAxis, kAllNumuCCCuts, bkgd_cuts, kNoShift, wgt));
        vMCSel_Reco.push_back(new Spectrum(*vloaders[i], kRecoMuKEVsCosVsEnuStandardAxis, kAllNumuCCCuts, kNoShift, wgt));
        vMCSig_Reco.push_back(new Spectrum(*vloaders[i], kRecoMuKEVsCosVsEnuStandardAxis, kAllNumuCCCuts && kIsTrueSig, kNoShift, wgt));
        vRecoTrue.push_back(new ReweightableSpectrum(*vloaders[i], kRecoMuKEVsCosVsEnuStandardAxis, kTrueMuKEVsCosVsEnuStandardAxis,
                                                     kAllNumuCCCuts && CutFromNuTruthCut(kIsTrueSigST),
                                                     kNoShift, wgt));
    }

    // let go spectrum loaders
    for(unsigned int i = 0; i < vloaders.size(); i++)
        vloaders[i]->Go();
    
    // output to file
    TFile* fOut = new TFile(outfile.c_str(), "RECREATE");
    fOut->cd();
    TDirectory* dir;

    // loop through all datasets to output
    for(unsigned int i = 0; i < vloaders.size(); i++)
    {
        dir = fOut->mkdir(dsets_info[i].foldername.c_str());
        dir->cd();
        veff[i].first->SaveTo(dir->mkdir("MCSig"));
        veff[i].second->SaveTo(dir->mkdir("MCSigNuTrue"));
        vMCSel_Reco[i]->SaveTo(dir->mkdir("MCSel_Reco"));
        vMCSig_Reco[i]->SaveTo(dir->mkdir("MCSig_Reco"));
        vbkg[i]->SaveTo(dir->mkdir("BkgdEst"));
        vRecoTrue[i]->SaveTo(dir->mkdir("RecoTrue"));
    }
    fOut->Close();
}
#endif