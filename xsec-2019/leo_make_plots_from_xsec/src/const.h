
//const std::string sfileIn = "/nova/app/users/laliaga/ANA_numu_cc_inc/make_xs/current_try2/test_numucc_inc_xs.root";
const std::string sfileIn_oth = "/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/ppfx_others_v2.root";
const std::string sfileIn_geA = "/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/genie_0_99_v2.root";
const std::string sfileIn_geB = "/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/genie_100_299_v2.root";
const std::string sfileIn_geC = "/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/genie_300_499_v2.root";

const std::string sfileIn_mc  = "/nova/ana/users/slin/test_releases/NumuCC_Fix_TestRelease/NDAna/numucc_inc/nominal_xsec.root";

const int Nuniv_ppfx  = 100;
const int Nuniv_genie = 500;
const int Nsyst = 30;
const std::string syst_name[Nsyst] = {"lightdown","lightup","ckv","calibneg","calibpos","calibshape","MuES_onlyMuKEDw","MuES_onlyMuKEUp",
				      "2kA_Dw","2kA_Up","02mmBeamSpotSize_Dw","02mmBeamSpotSize_Up","1mmBeamShiftX_Dw","1mmBeamShiftX_Up",
				      "1mmBeamShiftY_Dw","1mmBeamShiftY_Up","3mmHorn1X_Dw","3mmHorn1X_Up","3mmHorn1Y_Dw","3mmHorn1Y_Up",
				      "3mmHorn2X_Dw","3mmHorn2X_Up","3mmHorn2Y_Dw","3mmHorn2Y_Up","7mmTargetZ_Dw","7mmTargetZ_Up",
				      "MagneticFieldinDecayPipe_Dw","MagneticFieldinDecayPipe_Up","1mmHornWater_Dw","1mmHornWater_Up"};

const int Nfinal_fe = 9;
const int col_fe[Nfinal_fe] = {2,4,2,4,8,8,kOrange-3,kOrange-3,1};
const int col_st[Nfinal_fe] = {1,1,2,2,1,2,1,2,2};
const std::string final_fe[Nfinal_fe] = {"Flux (HP)","GENIE","Light","Calibration","Cherenkov","Calib. Shape","Mu Energy Scale","Normalization","Focusing"};
const std::string final_fe_name[Nfinal_fe] = {"Flux_HP","GENIE","Light","Calibration","Cherenkov","CalibShape","MuEScale","Normalization","Focusing"};

const int Nabins2X = 13; //pre calculated N bins X

const std::string leg_angle[13] = {
  "0.50<Cos#theta_{#mu}<0.56",
  "0.56<Cos#theta_{#mu}<0.62",
  "0.62<Cos#theta_{#mu}<0.68",
  "0.68<Cos#theta_{#mu}<0.74",
  "0.74<Cos#theta_{#mu}<0.80",
  "0.80<Cos#theta_{#mu}<0.85",
  "0.85<Cos#theta_{#mu}<0.88",
  "0.88<Cos#theta_{#mu}<0.91",
  "0.91<Cos#theta_{#mu}<0.94",
  "0.94<Cos#theta_{#mu}<0.96",
  "0.96<Cos#theta_{#mu}<0.98",
  "0.98<Cos#theta_{#mu}<0.99",
  "0.99<Cos#theta_{#mu}<1.00"
};

const std::string tit_ke = "Muon Kinetic Energy (GeV)";
const std::string tit_nu = "Neutrino Energy (GeV)";
const std::string tit_fe = "Fractional Uncertainties";
const std::string tit_xs = "#sigma(cm^{2}/nucleon)";

// Normalization (TN, doc-): 
// POT: 0.28% (Section 9.1)
// Intensity: 2% (Section 9.2)
// Particle propagation: 1-2%, so we take 2% (Section 9.5).
// fe_norm = sqrt(pow(0.0028,2)+pow(0.02,2)+pow(0.02,2));
const double fe_norm = 0.028422526;
