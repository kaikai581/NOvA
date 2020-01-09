
#include "CAFAna/Vars/Vars.h"
#include "CAFAna/Core/Binning.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/LoadFromFile.h"
#include "CAFAna/Core/Ratio.h"

#include "CAFAna/Vars/NumuVars.h"
#include "NDAna/numucc_inc/NumuCCIncAnalysis.h"
#include "NDAna/numucc_inc/NumuCCIncBins.h"
#include "NDAna/numucc_inc/NumuCCIncVars.h"
#include "NDAna/numucc_inc/NumuCCIncCuts.h"
#include "NDAna/numucc_inc/NDXSecMuonPID.h"

#include "CAFAna/Unfold/UnfoldIterative.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include <iostream>
#include <cmath>
#include <string>
#include <iomanip>

#include "Utilities/func/MathUtil.h"

// directory utilities
#include <boost/algorithm/string/classification.hpp> // Include boost::is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

using namespace ana;
using namespace boost::filesystem;

//const double ntargs = 3.88998e+31;  //pre-calculated with updated fiducial volume(1e6 interations)
const double ntargs = 3.89137e+31;  //pre-calculated with updated fiducial volume(1e10 interations)
const double pot    = 8.09e20;      //data POT

const std::string fake_xs = "ppfxwgt";

const std::string xs_mode0 = "xs_nominal";
const std::string xs_mode1 = "xs_bkgcons";

const std::string dirIn       = "/nova/ana/users/slin/temp/numuccinc";

const int Nuniv_ppfx  = 100;
const int Nuniv_genie = 1000;
const int Nsys =   8;
const int Nfoc  =   11;
const std::string sys_name[Nsys] = {"lightdown","lightup","ckv","calibneg","calibpos","calibshape","MuES_onlyMuKEDw","MuES_onlyMuKEUp"};
const std::string foc_name[Nfoc] = {"2kA","02mmBeamSpotSize","1mmBeamShiftX","1mmBeamShiftY","3mmHorn1X","3mmHorn1Y","3mmHorn2X","3mmHorn2Y",
				    "7mmTargetZ","MagneticFieldinDecayPipe","1mmHornWater"};

void make_xs(TDirectory* &dirInMC,TDirectory* &dirInDt, std::string sdOut, TH1* hInFlux, TFile* fileOut, Ratio inv_pur){
  
  std::cout<<"=> In make_xs() "<<sdOut<<std::endl;
  
  ///////////////////////////
  ///Getting all:
  //1. Efficiency:
  Spectrum* fMCSig        = ana::LoadFrom<Spectrum> (dirInMC->GetDirectory("MCSig")).release();
  Spectrum* fMCSigNuTree  = ana::LoadFrom<Spectrum> (dirInMC->GetDirectory("MCSigNuTre")).release();  
  TH3* h3Deff             = ana::ToTH3(*fMCSig      , pot, kPOT, angbinsCustom, mukebins, enubins);
  TH3* h3Deff_den         = ana::ToTH3(*fMCSigNuTree, pot, kPOT, angbinsCustom, mukebins, enubins);  
  h3Deff->Divide(h3Deff_den);

  //2. Signal:
  Spectrum* fData          = ana::LoadFrom<Spectrum> (dirInDt->GetDirectory(Form("%s",fake_xs.c_str()))).release();						      
  IBkgdEstimator* fBkgEst  = ana::LoadFrom<IBkgdEstimator>(dirInMC->GetDirectory("BkgdEst")).release();  
  Spectrum fBkg            = fBkgEst->Background();  
  Spectrum fSignalEst      = (*fData) - fBkg;

  //Background constraint
  // Ratio fSignalEst_Bkg_Contraint = fData/inv_pur;
  Ratio fSignalEst_Bkg_Contraint = *fData/Spectrum(inv_pur.ToTH1(), fData->POT(), fData->Livetime());

  //3. Unfolding:
  ReweightableSpectrum* fRecoTrue = ana::LoadFrom<ReweightableSpectrum>(dirInMC->GetDirectory("RecoTrue")).release();  

  UnfoldIterative unfold(*fRecoTrue,5);
  Spectrum fSigUnfolded = unfold.Truth(fSignalEst); 

  UnfoldIterative unfold_bkgc(*fRecoTrue,5);
  Spectrum fSigUnfolded_bkgc = unfold_bkgc.Truth(Spectrum(fSignalEst_Bkg_Contraint.ToTH1(), fData->POT(), fData->Livetime()));

  //4. Calculation:
  double integrated_flux = hInFlux->Integral();
  TH3* h3SigUnfolded[2];
  h3SigUnfolded[0] = ana::ToTH3(fSigUnfolded     , pot, kPOT, angbinsCustom, mukebins, enubins); 
  h3SigUnfolded[1] = ana::ToTH3(fSigUnfolded_bkgc, pot, kPOT, angbinsCustom, mukebins, enubins); 
  
  TH2* h2Dxs[2];
  TH1* h1Dxs[2];
  for(int ii=0;ii<2;ii++){
    h3SigUnfolded[ii]->Divide(h3Deff);
    h2Dxs[ii] = (TH2*)h3SigUnfolded[ii]->Project3D("yx"); 
    h1Dxs[ii] = (TH1*)h3SigUnfolded[ii]->Project3D("z"); 
    
    h2Dxs[ii]->Scale(1./integrated_flux);
    for(int ibin=1;ibin<=h1Dxs[ii]->GetNbinsX();ibin++){
      double cont_num = h1Dxs[ii]->GetBinContent(ibin);
      double cont_den = hInFlux->GetBinContent(ibin);
      if(cont_den>0)h1Dxs[ii]->SetBinContent(ibin,cont_num/cont_den);
      else          h1Dxs[ii]->SetBinContent(ibin,0);
      h1Dxs[ii]->SetBinError(ibin,0);
    }   
    h2Dxs[ii]->Scale(1./ntargs);
    h1Dxs[ii]->Scale(1./ntargs); 
    //Storing 
    //xs_nominal: calcualting the signal estomation by S-B
    //xs_bkgcons: calcualting the signal estomation by P*S
    fileOut->cd();
    string outfolder;
    if(ii == 0) outfolder = xs_mode0;
    else outfolder = xs_mode1;
    fileOut->cd(outfolder.c_str());
    h2Dxs[ii]->Write(Form("hXS2Dsec_%s",sdOut.c_str()));
    h1Dxs[ii]->Write(Form("hXS1Dsec_%s",sdOut.c_str()));
  }
  
  //Releasing memory:
  delete fMCSig;
  delete fMCSigNuTree;
  delete fData;
  delete fBkgEst;
  delete fRecoTrue;
  
}

void Make_NuMuCC_Inc_XS_no_genie(const char* fOutname){
  
  std::cout<<"=> start "<<std::endl;
  //this script does the cv, det, muon energy scale and focusing systematics  
  
  TFile* fpur = new TFile(Form("%s/purity.root"      ,dirIn.c_str()),"read");
  TFile* fsys = new TFile(Form("%s/systs_all.root" ,dirIn.c_str()),"read");
  TFile* fmc;
  //flux:
  TH1* hFlux_cv = Spectrum::LoadFrom(fsys->GetDirectory("fluxes/CV"))->ToTH1(pot);
  hFlux_cv->Scale(1e-4); // nu/m^2 to nu/cm^2

  // Read fake data
  TFile* fflux = new TFile(Form("%s/systs_all.root" ,dirIn.c_str()),"read");
  TH1* hFlux_ppfx[Nuniv_ppfx]; 
  for(int i=0;i<Nuniv_ppfx;i++){
    hFlux_ppfx[i] = Spectrum::LoadFrom( fflux->GetDirectory(Form("fluxes/ppfx%d",i)))->ToTH1(pot);
    hFlux_ppfx[i]->Scale(1e-4); 
  }

  TH1* hFlux_foc[Nfoc][2];  //0:up, 1: dw
  for(int i=0;i<Nfoc;i++){
    hFlux_foc[i][0] = Spectrum::LoadFrom( fflux->GetDirectory(Form("fluxes/%s_Up",foc_name[i].c_str())))->ToTH1(pot);
    hFlux_foc[i][1] = Spectrum::LoadFrom( fflux->GetDirectory(Form("fluxes/%s_Dw",foc_name[i].c_str())))->ToTH1(pot);
    hFlux_foc[i][0]->Scale(1e-4);
    hFlux_foc[i][1]->Scale(1e-4);
  }

  ///Background constraint
  Spectrum* MCSelected     = Spectrum::LoadFrom(fpur->GetDirectory("purity/MCSelected")).release();
  IBkgdEstimator* MCBkgEst = ana::LoadFrom<IBkgdEstimator>(fpur->GetDirectory("purity/BkgdEst")).release();
  Spectrum MCBkg           = MCBkgEst->Background();  
  Spectrum MCSelected_minus_MCBkd = *MCSelected - MCBkg;
  Ratio MC_Inv_Pur         = *MCSelected / MCSelected_minus_MCBkd;
  
  //file Out
  TFile* fOut = new TFile(fOutname, "RECREATE");
  fOut->mkdir(xs_mode0.c_str());
  fOut->mkdir(xs_mode1.c_str());
  
  //void make_xs(TDirectory* &dirInMC,TDirectory* &dirInDt, std::string sdOut, TH1* hInFlux, TFile* fOut, Ratio inv_pur)
  TDirectory* dir_data = fsys->GetDirectory("fakedata");
  std::string namesyst = "";
  TDirectory* dir_mc;
  //CV:
  {
    dir_mc = fsys->GetDirectory("cv");
    make_xs(dir_mc,dir_data,"cv",hFlux_cv,fOut,MC_Inv_Pur);
  }  
  //DET SYST
  for(int i=0;i<Nsys;i++){    
    dir_mc = fsys->GetDirectory(sys_name[i].c_str());
    make_xs(dir_mc,dir_data,sys_name[i],hFlux_cv,fOut,MC_Inv_Pur);
  }
  //FOC
  for(int i=0;i<Nfoc;i++){
    //Up
    dir_mc = fsys->GetDirectory((foc_name[i]+"_Up").c_str());
    make_xs(dir_mc,dir_data,foc_name[i]+"_Up",hFlux_foc[i][0],fOut,MC_Inv_Pur);
    //Down
    dir_mc = fsys->GetDirectory((foc_name[i]+"_Dw").c_str());
    make_xs(dir_mc,dir_data,foc_name[i]+"_Dw",hFlux_foc[i][1],fOut,MC_Inv_Pur);
  }
  //PPFX
  for(int i=0;i<Nuniv_ppfx;i++){
    int idx_file = i%4;
    if(idx_file==0)
    {
      if(boost::filesystem::exists(Form("%s/ppfx_univ_%d_to_%d.root",dirIn.c_str(),i,i+3)))
        fmc = new TFile(Form("%s/ppfx_univ_%d_to_%d.root",dirIn.c_str(),i,i+3),"read");
      else fmc = NULL;
    }
    if(fmc)
    {
      dir_mc = fmc->GetDirectory(Form("ppfx%d",i));    
      make_xs(dir_mc,dir_data,Form("ppfx%d",i),hFlux_ppfx[i],fOut,MC_Inv_Pur);
    }
  }
  //GENIE
  // for(int i=0;i<Nuniv_genie;i++){
  //   int idx_file = i%4;
  //   if(idx_file==0)
  //   {
  //     if(boost::filesystem::exists(Form("%s/genie_univ_%d_to_%d.root",dirIn.c_str(),i,i+3)))
  //       fmc = new TFile(Form("%s/genie_univ_%d_to_%d.root",dirIn.c_str(),i,i+3),"read");
  //     else fmc = NULL;
  //   }
  //   if(fmc)
  //   {
  //     dir_mc = fmc->GetDirectory(Form("genie%d",i));    
  //     make_xs(dir_mc,dir_data,Form("genie%d",i),hFlux_cv,fOut,MC_Inv_Pur);
  //   }
  // }
  
  fOut->Close();

}


