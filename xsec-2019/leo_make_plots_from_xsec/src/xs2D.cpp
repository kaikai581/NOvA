 
#include "SettingDraw.h"
#include "HandleUncertainties.h"
#include "headers.h"
#include "const.h"

void setUncer1D(TH1D* hIn,int ihist);
void setMC1D(TH1D* hIn);
void setData1D(TGraphAsymmErrors* gIn);
bool databin(int xbin,int ybin);

void xs2D(const char* mode, const char* dirOut, const char* tag){
  
  TGaxis::SetMaxDigits(5);
  AnaPack::HandleUncertainties* hhist = new AnaPack::HandleUncertainties();
  AnaPack::SettingDraw*  plot =  new AnaPack::SettingDraw();
  gStyle->SetFillColor(0); 
  
  string sfileIn_oth = "ppfx_others_v2.root";
  string sfileIn_geA = "genie_0_99_v2.root";
  string sfileIn_geB = "genie_100_299_v2.root";
  string sfileIn_geC = "genie_300_499_v2.root";
  string sfileIn_mc  = "nominal_xsec.root";
  TFile* fileIn      = new TFile(sfileIn_oth.c_str(),"read");
  TFile* fileIn_geA  = new TFile(sfileIn_geA.c_str(),"read");
  TFile* fileIn_geB  = new TFile(sfileIn_geB.c_str(),"read");
  TFile* fileIn_geC  = new TFile(sfileIn_geC.c_str(),"read");
  TFile* fileIn_mc   = new TFile(sfileIn_mc.c_str() ,"read");

  TFile* fileOut = new TFile(Form("%s/numucc_inc_%s.root",dirOut),"recreate");

  TH2D *hdt, *hmc;
  std::vector<TH2D*> vuniv[2];
  TH2D *hsyst[Nsyst];
  TH2D *herr_ppfx, *hfes_ppfx;
  
  hdt = (TH2D*)fileIn->Get(Form("%s/hXS1Dsec_cv",tag));
  hmc = (TH2D*)fileIn_mc->Get("hXS2Dsec_nominal");
  
  const int NbinsX = hmc->GetNbinsX();
  const int NbinsY = hmc->GetNbinsY();
  
  //loading ppfx:
  for(int i=0;i<Nuniv_ppfx;i++){
    vuniv[0].push_back( (TH1D*)fileIn->Get(Form("%s/hXS1Dsec_ppfx%d",tag,i)) );
  }
  //loading genie:
  for(int i=0;i<Nuniv_genie;i++){
    if(i>=0   && i<=99 )vuniv[1].push_back( (TH1D*)fileIn_geA->Get(Form("%s/hXS1Dsec_genie%d",tag,i)) );
    if(i>=100 && i<=299)vuniv[1].push_back( (TH1D*)fileIn_geB->Get(Form("%s/hXS1Dsec_genie%d",tag,i)) );
    if(i>=300 && i<=499)vuniv[1].push_back( (TH1D*)fileIn_geC->Get(Form("%s/hXS1Dsec_genie%d",tag,i)) );
  }
  for(int i=0;i<Nsyst;i++){
    hsyst[i]  = (TH2D*)fileIn->Get(Form("%s/hXS2D_%s",tag,syst_name[i].c_str())); 
  }
  
  //Calculating Up and Dw histograms
  //Currently we have 9: PPFX, GENIE, Light, Calib, Chv, Calib Shape,  MuE Scale, Norm, Foc
  std::vector<TH2D*> hup,hdw,hfe;
  for(int i=0;i<Nfinal_fe;i++){
    hup.push_back( (TH2D*)hmc->Clone(Form("hUp_%d",i)));  
    hdw.push_back( (TH2D*)hmc->Clone(Form("hDw_%d",i)));  
    hfe.push_back( (TH2D*)hmc->Clone(Form("hFE_%d",i)));  
  }
  double dt_cv,err_cv;
  double fe_val,sigma_up,sigma_dw,sigma;
  std::vector<double> genie_up_univ,genie_dw_univ;
  std::vector<double> ppfx_up_univ,ppfx_dw_univ;
  
  int isys = -1;

  for(int i=1;i<=NbinsX;i++){
    for(int j=1;j<=NbinsY;j++){
      dt_cv = hdt->GetBinContent(i,j);
      
      ////////PPFX
      isys = 0;
      if(dt_cv<=0){
        hup[isys]->SetBinContent(i,j,0);
        hdw[isys]->SetBinContent(i,j,0);
        hfe[isys]->SetBinContent(i,j,0);
      }
      else{
        ppfx_up_univ.clear();
        ppfx_dw_univ.clear();
	for(unsigned int iuniv=0; iuniv<vuniv[0].size(); iuniv++){
	  double duniv = vuniv[0][iuniv]->GetBinContent(i,j) - dt_cv;
	  if( duniv>0 )ppfx_up_univ.push_back( abs(duniv) );
	  else         ppfx_dw_univ.push_back( abs(duniv) );
	}
	std::sort(ppfx_up_univ.begin(),ppfx_up_univ.end());
	std::sort(ppfx_dw_univ.begin(),ppfx_dw_univ.end());
	int iuniv_up = int( 0.68*double(ppfx_up_univ.size()) );
	int iuniv_dw = int( 0.68*double(ppfx_dw_univ.size()) );
	sigma_up = abs(ppfx_up_univ[iuniv_up]);
	sigma_dw = abs(ppfx_dw_univ[iuniv_dw]);
	hup[isys]->SetBinContent(i,j,sigma_up);
	hdw[isys]->SetBinContent(i,j,sigma_dw);
	sigma = sigma_dw ;
	if(sigma<sigma_up) sigma = sigma_up;
	fe_val = sigma / dt_cv;
	if(sigma!=sigma)sigma = 0;

	if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);
      }
      
      ///////GENIE  
      isys = 1;
      if(dt_cv<=0){
	hup[isys]->SetBinContent(i,j,0);
	hdw[isys]->SetBinContent(i,j,0);
	hfe[isys]->SetBinContent(i,j,0);
      }
      else{
	genie_up_univ.clear();
	genie_dw_univ.clear();
	for(unsigned int iuniv=1; iuniv<vuniv[1].size(); iuniv++){
	  double duniv = vuniv[1][iuniv]->GetBinContent(i,j) - dt_cv;
	  if( duniv>0 )genie_up_univ.push_back( abs(duniv) );
	  else         genie_dw_univ.push_back( abs(duniv) );
	}
	std::sort(genie_up_univ.begin(),genie_up_univ.end());
	std::sort(genie_dw_univ.begin(),genie_dw_univ.end());
	int iuniv_up = int( 0.68*double(genie_up_univ.size()) );
	int iuniv_dw = int( 0.68*double(genie_dw_univ.size()) );
	sigma_up = abs(genie_up_univ[iuniv_up]);
	sigma_dw = abs(genie_dw_univ[iuniv_dw]);
	hup[isys]->SetBinContent(i,j,sigma_up);
	hdw[isys]->SetBinContent(i,j,sigma_dw);
	sigma = sigma_dw ;
	if(sigma<sigma_up) sigma = sigma_up;
	fe_val = sigma / dt_cv;
	if(sigma!=sigma)sigma = 0;
	if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);
      }
      
      ///////Light
      isys = 2;
      hup[isys]->SetBinContent(i,j,0);
      hdw[isys]->SetBinContent(i,j,0);
      hfe[isys]->SetBinContent(i,j,0);
      
      sigma_dw = dt_cv-hsyst[0]->GetBinContent(i,j);
      sigma_up = dt_cv-hsyst[1]->GetBinContent(i,j);
      sigma = abs(sigma_dw);
      if(sigma<abs(sigma_up)) sigma = abs(sigma_up);

      if(sigma_dw<0 && sigma_up<0)     hup[isys]->SetBinContent(i,j,sigma);
      else if(sigma_dw>0 && sigma_up>0)hdw[isys]->SetBinContent(i,j,sigma);
      else{
	hup[isys]->SetBinContent(i,j,abs(sigma_up));
	hdw[isys]->SetBinContent(i,j,abs(sigma_dw));
      }      
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0; 
      if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);
      
      ///////Calibration
      isys = 3;
      hup[isys]->SetBinContent(i,j,0);
      hdw[isys]->SetBinContent(i,j,0);
      hfe[isys]->SetBinContent(i,j,0);
      
      sigma_dw = dt_cv-hsyst[3]->GetBinContent(i,j);
      sigma_up = dt_cv-hsyst[4]->GetBinContent(i,j);
      sigma = abs(sigma_dw);
      if(sigma<abs(sigma_up)) sigma = abs(sigma_up);

      if(sigma_dw<0 && sigma_up<0)     hup[isys]->SetBinContent(i,j,sigma);
      else if(sigma_dw>0 && sigma_up>0)hdw[isys]->SetBinContent(i,j,sigma);
      else{
	hup[isys]->SetBinContent(i,j,abs(sigma_up));
	hdw[isys]->SetBinContent(i,j,abs(sigma_dw));
      }      
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0; 
      if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);
      
      ////////////////Chv
      isys = 4;
      hup[isys]->SetBinContent(i,j,0);
      hdw[isys]->SetBinContent(i,j,0);
      hfe[isys]->SetBinContent(i,j,0);      
      sigma = dt_cv - hsyst[2]->GetBinContent(i,j);
      if(sigma>0 )hdw[isys]->SetBinContent(i,j,abs(sigma));
      if(sigma<=0)hup[isys]->SetBinContent(i,j,abs(sigma));    
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0;
      if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);
      
      ////////////// Calib Shape
      isys = 5;
      hup[isys]->SetBinContent(i,j,0);
      hdw[isys]->SetBinContent(i,j,0);
      hfe[isys]->SetBinContent(i,j,0);      
      sigma = dt_cv - hsyst[5]->GetBinContent(i,j);      
      if(sigma>0 )hdw[isys]->SetBinContent(i,j,abs(sigma));
      if(sigma<=0)hup[isys]->SetBinContent(i,j,abs(sigma));    
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0;
      if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);

      /////// MuE scaling
      isys = 6;
      hup[isys]->SetBinContent(i,j,0);
      hdw[isys]->SetBinContent(i,j,0);
      hfe[isys]->SetBinContent(i,j,0);
    
      sigma_dw = dt_cv-hsyst[6]->GetBinContent(i,j);
      sigma_up = dt_cv-hsyst[7]->GetBinContent(i,j);
      sigma = abs(sigma_dw);
      if(sigma<abs(sigma_up)) sigma = abs(sigma_up);
      
      if(sigma_dw<0 && sigma_up<0)     hup[isys]->SetBinContent(i,j,sigma);
      else if(sigma_dw>0 && sigma_up>0)hdw[isys]->SetBinContent(i,j,sigma);
      else{
	hup[isys]->SetBinContent(i,j,abs(sigma_up));
	hdw[isys]->SetBinContent(i,j,abs(sigma_dw));
      }
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0;
      if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);

      ///////Normalization
      isys = 7;
      sigma = dt_cv * fe_norm;
      if(sigma!=sigma)sigma = 0;
      hup[isys]->SetBinContent(i,j,dt_cv+sigma);
      hdw[isys]->SetBinContent(i,j,dt_cv-sigma);
      if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_norm);
      

      ///////Focusing
       isys = 8;
       hup[isys]->SetBinContent(i,j,0);
       hdw[isys]->SetBinContent(i,j,0);
       hfe[isys]->SetBinContent(i,j,0);
       
       sigma_dw = dt_cv-hsyst[8]->GetBinContent(i,j);
       sigma_up = dt_cv-hsyst[9]->GetBinContent(i,j);
       sigma = abs(sigma_dw);
       if(sigma<abs(sigma_up)) sigma = abs(sigma_up);
       
       if(sigma_dw<0 && sigma_up<0)     hup[isys]->SetBinContent(i,j,sigma);
       else if(sigma_dw>0 && sigma_up>0)hdw[isys]->SetBinContent(i,j,sigma);
       else{
	 hup[isys]->SetBinContent(i,j,abs(sigma_up));
	 hdw[isys]->SetBinContent(i,j,abs(sigma_dw));
       }
       fe_val = sigma / dt_cv;
       if(sigma!=sigma)sigma = 0;
       if(dt_cv>0)hfe[isys]->SetBinContent(i,j,fe_val);
       
    }
  }
  
  //Data:
  TGraphAsymmErrors* gdt[Nabins2X];
  double count[Nabins2X] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
  for(int i=1;i<=NbinsX;i++){
    gdt[i-1] = new TGraphAsymmErrors();
    for(int j=1;j<=NbinsY;j++){      
      double kin   = hdt->GetYaxis()->GetBinCenter(j);	
      double binw  = hdt->GetYaxis()->GetBinWidth(j);
      double cont  = hdt->GetBinContent(i,j);
      double errUp = 0;
      double errDw = 0;      
      for(int p=0;p<Nfinal_fe;p++){
	double up = hup[p]->GetBinContent(i,j);
	double dw = hdw[p]->GetBinContent(i,j);
	errUp += pow(up-cont,2); 
	errDw += pow(cont-dw,2); 
      }
      errUp = sqrt(errUp);
      errDw = sqrt(errDw);
      if(databin(i,j)){
	count[i-1]++;
	gdt[i-1]->SetPoint(count[i-1], kin, cont);
	gdt[i-1]->SetPointError(count[i-1],binw/2, binw/2, errDw, errUp);
      }//databin
    }//binY
  } //bin X      
  
  
  //MC:
  TH1D* h1MC[Nabins2X];
  TH1D* h1fe[Nabins2X][Nfinal_fe];
  for(int i=1;i<=NbinsX;i++){
    h1MC[i-1] = new TH1D(Form("h1MC_%d",i),"",20,0.5,2.5);
    for(int p=0;p<Nfinal_fe;p++)h1fe[i-1][p] = new TH1D(Form("h1fe_%d_%d",i,p),"",20,0.5,2.5);    
    for(int j=1;j<=NbinsY;j++){   
      if(databin(i,j)){
	double cont = hmc->GetBinContent(i,j);
	h1MC[i-1]->SetBinContent(j,cont);
	for(int p=0;p<Nfinal_fe;p++){
	  double ferr = hfe[p]->GetBinContent(i,j);	
	  h1fe[i-1][p]->SetBinContent(j,ferr);      
	}
      } //databin
    }//binY
  }//binX
  
  
  //XS:
  TCanvas* cxs[Nabins2X];
  TLegend* leg[Nabins2X]; 
  TLatex* lat[Nabins2X];
  double maxY = -1;
  
  for(int i=0;i<NbinsX;i++){
    maxY = h1MC[i]->GetMaximum();
    cxs[i] = new TCanvas();
    lat[i] = new TLatex();
    lat[i]->SetNDC();
    plot->canvas(cxs[i]);
    plot->range(0.5, 2.5-0.0001,0,2.*(maxY));
    plot->axis(h1MC[i],"",tit_ke.c_str(),tit_xs.c_str());
    leg[i] = new TLegend(0.67,0.68,0.87,0.88);
    leg[i]->SetBorderSize(0);
    setMC1D(h1MC[i]);
    setData1D(gdt[i]);
    h1MC[i]->Draw("hist");
    gdt[i]->Draw("p1 same");  
    leg[i]->SetTextSize(0.05);
    leg[i]->AddEntry(h1MC[i],  "MC", "fl");
    leg[i]->AddEntry(gdt[i],"Data (Syst.)", "lp");
    leg[i]->Draw();
    lat[i]->DrawLatex(0.2,0.83,Form("%s",leg_angle[i].c_str()));
    cxs[i]->SaveAs(Form("%s/xs2D_anglebin%d.png",dirOut,i));
  }
  
  //FE:
  TCanvas* cfe[Nabins2X];
  TLatex* latfe[Nabins2X];
  TLegend* lfe;
  for(int i=0;i<NbinsX;i++){
    if(i>6)lfe = new TLegend(0.2,0.5,0.45,0.88);
    else   lfe = new TLegend(0.6,0.4,0.85,0.78);
    lfe->SetBorderSize(0);
    cfe[i] = new TCanvas();
    latfe[i] = new TLatex();
    latfe[i]->SetNDC();
    plot->canvas(cfe[i]);
    plot->canvas(cfe[i]);
    plot->range(0.5, 2.5-0.0001,0,0.4);
    plot->axis(h1fe[i][0],"",tit_ke.c_str(),tit_fe.c_str());
    for(int j=0;j<Nfinal_fe;j++){
      setUncer1D(h1fe[i][j],j);
      if(j==0)h1fe[i][j]->Draw("hist");
      else    h1fe[i][j]->Draw("histsame");
      lfe->AddEntry(h1fe[i][j],final_fe[j].c_str(),"l");
    }
    latfe[i]->DrawLatex(0.63,0.83,Form("%s",leg_angle[i].c_str()));
    lfe->Draw();
    cfe[i]->SaveAs(Form("%s/fe2D_anglebin%d.png",dirOut,i));
  }
  
  //Storing the final histograms:
  fileOut->cd();
  fileOut->mkdir("CrossSections");
  for(int i=0;i<NbinsX;i++){
    fileOut->cd("CrossSections");
    gdt[i]->GetXaxis()->SetTitle(tit_ke.c_str());
    gdt[i]->GetYaxis()->SetTitle(tit_xs.c_str());
    gdt[i]->Write(Form("mock_data_bin%d",i));
    h1MC[i]->Write(Form("mc_bin%d",i));

    fileOut->mkdir(Form("FractUncer_bin%d",i));
    fileOut->cd(Form("FractUncer_bin%d",i));
    for(int j=0;j<Nfinal_fe;j++){
      h1fe[i][j]->Write(Form("FractUncer_bin%d_%s",i,final_fe_name[j].c_str()));   
    }

  }
  
}

/////////////////////////////////////////
void setMC1D(TH1D* hIn){
  hIn->SetLineColor(2);
  hIn->SetLineWidth(2);
}

/////////////////////////////////////////
void setData1D(TGraphAsymmErrors* gIn){
  gIn->SetMarkerColor(kBlack);
  gIn->SetMarkerStyle(20);
  gIn->SetLineColor(kBlack);
  gIn->SetLineWidth(3);  
}

/////////////////////////////////////////
void setUncer1D(TH1D* hIn,int ihist){
  hIn->SetLineColor(col_fe[ihist]);
  hIn->SetLineWidth(3);
  hIn->SetLineStyle(col_st[ihist]);
}

/////////////////////////////////////////
bool databin(int xbin,int ybin){
  //  xbin and ybin  start at 1
  bool answer = false;
  if(xbin==1 && ybin<=4) answer = true;
  if( (xbin==2 || xbin==3) && ybin<=5) answer = true;

  if(xbin==4 && ybin<=7) answer = true;
  if(xbin==5 && ybin<=9) answer = true;
  if(xbin==6 && ybin<=10) answer = true;
  if(xbin==7 && ybin<=12) answer = true;
  if(xbin==8 && ybin<=15) answer = true;
  if(xbin==9 && ybin<=19) answer = true;
  if( (xbin==10 || xbin==11 || xbin==12 || xbin==13) && ybin<=20) answer = true;

  return answer;

}

////////////////////////////////
#ifndef __CINT__
int main(int argc, const char* argv[]){  
  xs2D(argv[1],argv[2],argv[3]);
  return 0;
}
#endif
