#ifdef __CINT__

void cut_flow(int ds_idx = 0)
{
  std::cout << "Sorry, you must run in compiled mode" << std::endl;
}
#else

#include <fstream>
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Vars/PPFXWeights.h"
#include "CAFAna/Vars/XsecTunes.h" // Only use this include with development

#include "NDAna/numucc_inc/NumuCCIncCuts.h"

using namespace ana;

vector<string> syst_names = {"nominal","lightdown","lightup","ckv","calibneg","calibpos","calibshape"};

const vector<Cut> all_cuts = {kNoCut, kNumuMyQuality, kNumuTightContainND, kIsFiducial, kMuonIDCut, kRecoMuKECut, kRecoMuCosCut, kRecoMuonPhaseSpaceCut};

const NuTruthVar wgt_xsST = kXSecCVWgt2018_smallerDISScale_NT;
const Var wgt_xs          = VarFromNuTruthVar(wgt_xsST, 1);
const NuTruthVar wgtST    = kPPFXFluxCVWgtST*wgt_xsST;
const Var wgt             = VarFromNuTruthVar(wgtST, 1);

// binning
const std::vector<double> angedges_old {0.5, 0.56, 0.62, 0.68, 0.74, 0.80, 0.85, 0.88, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0};
const Binning angbinsCustom_old = Binning::Custom(angedges_old);
const Binning mukebins_old      = Binning::Simple(20,0.5,2.5);
const Binning enubins_old       = Binning::Simple(20, 0.0, 5.0);
const Binning ehadbins          = Binning::Simple(20, 0.0, 3.0);

vector<string> get_file_list(int ds_idx)
{
  vector<string> fns;
  string list_filename = "../lists_common_files/"+syst_names[ds_idx]+".txt";
  ifstream list_file(list_filename);
  string tmpstr;

  while(list_file >> tmpstr) fns.push_back(tmpstr);

  return fns;
}

void cut_flow(int ds_idx = 0)
{
  vector<string> flist = get_file_list(ds_idx);
  
  SpectrumLoader loader(flist);

  struct VarPack
  {
    Var var;
    Binning binning;
    string label;
    string varname;
  };

  vector<VarPack> varpacks = {
    {kRecoMuCostheta, angbinsCustom_old, "Reconstructed cos#{theta}", "muang_reco"},
    {kRecoMuKE, mukebins_old, "Reconstructed Muon Kinetic Energy (GeV)", "muke_reco"},
    {kRecoE, enubins_old, "Reconstructed Neutrino Energy (GeV)", "nue_reco"},
    {kRecoHadE, ehadbins, "Reconstructed Hadronic Energy (GeV)", "hade_reco"},
    {kTrueMuCostheta, angbinsCustom_old, "True cos#{theta}", "muang_true"},
    {kTrueMuKE, mukebins_old, "True Muon Kinetic Energy (GeV)", "muke_true"},
    {kTrueE, enubins_old, "True Neutrino Energy (GeV)", "nue_true"},
    {kTrueHadE, ehadbins, "True Hadronic Energy (GeV)", "hade_true"}
  };

  vector<Cut> cut_flow;
  for(unsigned int i = 0; i < all_cuts.size(); i++)
  {
    Cut tot_cut = all_cuts[0];
    for(unsigned int j = 1; j <= i; j++)
      tot_cut = tot_cut && all_cuts[j];
    cut_flow.push_back(tot_cut);
  }
  vector<string> cut_names = {"NoCut", "Quality", "Containment", "Fiducial", "MuonID", "MuKE", "MuAngle", "PhaseSpace"};

  // set up the loader
  loader.SetSpillCut(kStandardSpillCuts);

  // set up spectra
  vector<vector<Spectrum*>> all_spec;
  for(unsigned int i = 0; i < varpacks.size(); i++)
  {
    vector<Spectrum*> cut_specs;
    for(unsigned int j = 0; j < cut_flow.size(); j++)
      cut_specs.push_back(new Spectrum(varpacks[i].label, varpacks[i].binning, loader, varpacks[i].var, cut_flow[j], kNoShift, wgt));
    all_spec.push_back(cut_specs);
  }

  // let go
  loader.Go();

  // save to file
  TFile fout((syst_names[ds_idx]+".root").c_str(), "RECREATE");
  TDirectory* dir;
  for(unsigned int i = 0; i < varpacks.size(); i++)
  {
    dir = fout.mkdir((varpacks[i].varname).c_str());
    for (unsigned int j = 0; j < cut_flow.size(); j++)
      all_spec[i][j]->SaveTo(dir->mkdir(cut_names[j].c_str()));
  }
}
#endif