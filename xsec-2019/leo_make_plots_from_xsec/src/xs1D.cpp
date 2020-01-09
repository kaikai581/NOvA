 
#include "SettingDraw.h"
#include "HandleUncertainties.h"
#include "headers.h"
#include "const.h"

void setUncer1D(TH1D* hIn,int ihist);
void setMC1D(TH1D* hIn);
void setData1D(TGraphAsymmErrors* gIn);
bool databin(int xbin);

void xs1D(const char* mode, const char* dirOut, const char* tag){
  
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
  
  TH1D *hdt, *hmc;
  std::vector<TH1D*> vuniv[2];
  TH1D *hsyst[Nsyst];
  TH1D *herr_ppfx, *hfes_ppfx;
  
  hdt = (TH1D*)fileIn->Get(Form("%s/hXS1Dsec_cv",tag));
  hmc = (TH1D*)fileIn_mc->Get("hXS2Dsec_nominal");   //look at it
  
  const int NbinsX = hmc->GetNbinsX();
  //loading ppfx:
  for(int i=0;i<Nuniv_ppfx;i++){
    vuniv[0].push_back( (TH1D*)fileIn->Get(Form("%s/hXS1Dsec_ppfx%d",i)) );
  }
  //loading genie:
  for(int i=0;i<Nuniv_genie;i++){
    if(i>=0   && i<=99 )vuniv[1].push_back( (TH1D*)fileIn_geA->Get(Form("%s/hXS1Dsec_genie%d",i)) );
    if(i>=100 && i<=299)vuniv[1].push_back( (TH1D*)fileIn_geB->Get(Form("%s/hXS1Dsec_genie%d",i)) );
    if(i>=300 && i<=499)vuniv[1].push_back( (TH1D*)fileIn_geC->Get(Form("%s/hXS1Dsec_genie%d",i)) );
  }
  for(int i=0;i<Nsyst;i++){
    hsyst[i]  = (TH1D*)fileIn->Get(Form("%s/hXS1Dsec_%s",tag,syst_name[i].c_str())); 
  }
  
  //Calculating Up and Dw histograms
  //Currently we have 9: PPFX, GENIE, Light, Calib, Chv, Calib Shape, MuE Scale. Norm, Foc
  std::vector<TH1D*> hup,hdw,hfe;
  for(int i=0;i<Nfinal_fe;i++){
    hup.push_back( (TH1D*)hmc->Clone(Form("hUp_%d",i)));  
    hdw.push_back( (TH1D*)hmc->Clone(Form("hDw_%d",i)));  
    hfe.push_back( (TH1D*)hmc->Clone(Form("hFE_%d",i)));  
  }
  double dt_cv,err_cv;
  double fe_val,sigma_up,sigma_dw,sigma,sigma_A,sigma_B;
  std::vector<double> genie_up_univ,genie_dw_univ;
  std::vector<double> ppfx_up_univ,ppfx_dw_univ;
  
  int isys = -1;

  for(int i=1;i<=NbinsX;i++){
    dt_cv = hdt->GetBinContent(i);
    
    ////////PPFX
    isys = 0;
    if(dt_cv<=0){
      hup[isys]->SetBinContent(i,0);
      hdw[isys]->SetBinContent(i,0);
      hfe[isys]->SetBinContent(i,0);
    }
    else{
      ppfx_up_univ.clear();
      ppfx_dw_univ.clear();
      for(unsigned int iuniv=0; iuniv<vuniv[0].size(); iuniv++){
	double duniv = vuniv[0][iuniv]->GetBinContent(i) - dt_cv;
	if( duniv>0 )ppfx_up_univ.push_back( abs(duniv) );
	else         ppfx_dw_univ.push_back( abs(duniv) );
      }
      std::sort(ppfx_up_univ.begin(),ppfx_up_univ.end());
      std::sort(ppfx_dw_univ.begin(),ppfx_dw_univ.end());
      int iuniv_up = int( 0.68*double(ppfx_up_univ.size()) );
      int iuniv_dw = int( 0.68*double(ppfx_dw_univ.size()) );
      sigma_up = abs(ppfx_up_univ[iuniv_up]);
      sigma_dw = abs(ppfx_dw_univ[iuniv_dw]);
      hup[isys]->SetBinContent(i,sigma_up);
      hdw[isys]->SetBinContent(i,sigma_dw);
      sigma = sigma_dw ;
      if(sigma<sigma_up) sigma = sigma_up;
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0;     
      if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
    }
    
    ///////GENIE  
    isys = 1;
    if(dt_cv<=0){
      hup[isys]->SetBinContent(i,0);
      hdw[isys]->SetBinContent(i,0);
      hfe[isys]->SetBinContent(i,0);
    }
    else{
      genie_up_univ.clear();
      genie_dw_univ.clear();
      for(unsigned int iuniv=1; iuniv<vuniv[1].size(); iuniv++){
	double duniv = vuniv[1][iuniv]->GetBinContent(i) - dt_cv;
	if( duniv>0 )genie_up_univ.push_back( abs(duniv) );
	else         genie_dw_univ.push_back( abs(duniv) );
      }
      std::sort(genie_up_univ.begin(),genie_up_univ.end());
      std::sort(genie_dw_univ.begin(),genie_dw_univ.end());
      int iuniv_up = int( 0.68*double(genie_up_univ.size()) );
      int iuniv_dw = int( 0.68*double(genie_dw_univ.size()) );
      sigma_up = abs(genie_up_univ[iuniv_up]);
      sigma_dw = abs(genie_dw_univ[iuniv_dw]);
      hup[isys]->SetBinContent(i,sigma_up);
      hdw[isys]->SetBinContent(i,sigma_dw);
      sigma = sigma_dw;
      if(sigma<sigma_up) sigma = sigma_up;
      fe_val = sigma / dt_cv;
      if(sigma!=sigma)sigma = 0;
      if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
    }
      
    ///////Light
    isys = 2;
    hup[isys]->SetBinContent(i,0);
    hdw[isys]->SetBinContent(i,0);
    hfe[isys]->SetBinContent(i,0);
    
    sigma_A = dt_cv-hsyst[0]->GetBinContent(i);
    sigma_B = dt_cv-hsyst[1]->GetBinContent(i);    
    sigma = abs(sigma_A);
    if(sigma<abs(sigma_B)) sigma = abs(sigma_B);
    
    if(sigma_A<0 && sigma_B<0)     hup[isys]->SetBinContent(i,sigma);
    else if(sigma_A>0 && sigma_B>0)hdw[isys]->SetBinContent(i,sigma);
    else if(sigma_A>0 && sigma_B<0){
      hup[isys]->SetBinContent(i,abs(sigma_B));
      hdw[isys]->SetBinContent(i,abs(sigma_A));
    }
    else if(sigma_B>0 && sigma_A<0){
      hup[isys]->SetBinContent(i,abs(sigma_A));
      hdw[isys]->SetBinContent(i,abs(sigma_B));
    }

    fe_val = sigma / dt_cv;
    if(sigma!=sigma)sigma = 0; 
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
      
    ///////Calibration
    isys = 3;
    hup[isys]->SetBinContent(i,0);
    hdw[isys]->SetBinContent(i,0);
    hfe[isys]->SetBinContent(i,0);
    
    sigma_A = dt_cv-hsyst[3]->GetBinContent(i);
    sigma_B = dt_cv-hsyst[4]->GetBinContent(i);
    sigma = abs(sigma_A);
    if(sigma<abs(sigma_B)) sigma = abs(sigma_B);

    if(sigma_A<0 && sigma_B<0)     hup[isys]->SetBinContent(i,sigma);
    else if(sigma_A>0 && sigma_B>0)hdw[isys]->SetBinContent(i,sigma);
    else if(sigma_A>0 && sigma_B<0){
      hup[isys]->SetBinContent(i,abs(sigma_B));
      hdw[isys]->SetBinContent(i,abs(sigma_A));
    }
    else if(sigma_B>0 && sigma_A<0){
      hup[isys]->SetBinContent(i,abs(sigma_A));
      hdw[isys]->SetBinContent(i,abs(sigma_B));
    }

    fe_val = sigma / dt_cv;
    if(sigma!=sigma)sigma = 0;
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
      
    ////////////////Chv
    isys = 4;
    hup[isys]->SetBinContent(i,0);
    hdw[isys]->SetBinContent(i,0);
    hfe[isys]->SetBinContent(i,0);
    
    sigma = dt_cv - hsyst[2]->GetBinContent(i);

    if(sigma>0 )hdw[isys]->SetBinContent(i,abs(sigma));
    if(sigma<=0)hup[isys]->SetBinContent(i,abs(sigma));
    
    fe_val = sigma / dt_cv;
    if(sigma!=sigma)sigma = 0;
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
    
    //////////////Calib Shape
    isys = 5;
    hup[isys]->SetBinContent(i,0);
    hdw[isys]->SetBinContent(i,0);
    hfe[isys]->SetBinContent(i,0);
    
    sigma = dt_cv - hsyst[5]->GetBinContent(i);

    if(sigma>0 )hdw[isys]->SetBinContent(i,abs(sigma));
    if(sigma<=0)hup[isys]->SetBinContent(i,abs(sigma));
    
    fe_val = sigma / dt_cv;
    if(sigma!=sigma)sigma = 0;
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);

    /////// MuE scaling
    isys = 6;
    hup[isys]->SetBinContent(i,0);
    hdw[isys]->SetBinContent(i,0);
    hfe[isys]->SetBinContent(i,0);
    
    sigma_A = dt_cv-hsyst[6]->GetBinContent(i);
    sigma_B = dt_cv-hsyst[7]->GetBinContent(i);
    sigma = abs(sigma_A);
    if(sigma<abs(sigma_B)) sigma = abs(sigma_B);

    if(sigma_A<0 && sigma_B<0)     hup[isys]->SetBinContent(i,sigma);
    else if(sigma_A>0 && sigma_B>0)hdw[isys]->SetBinContent(i,sigma);
    else if(sigma_A>0 && sigma_B<0){
      hup[isys]->SetBinContent(i,abs(sigma_B));
      hdw[isys]->SetBinContent(i,abs(sigma_A));
    }
    else if(sigma_A<0 && sigma_B>0){
      hup[isys]->SetBinContent(i,abs(sigma_A));
      hdw[isys]->SetBinContent(i,abs(sigma_B));
    }
    fe_val = sigma / dt_cv;
    if(sigma!=sigma)sigma = 0;
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
    
    ///////Normalization
    isys = 7;
    sigma = dt_cv * fe_norm;
    if(sigma!=sigma)sigma = 0;
    hup[isys]->SetBinContent(i,dt_cv+sigma);
    hdw[isys]->SetBinContent(i,dt_cv-sigma);
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_norm);
    
    ///////Focusing
    isys = 8;
    hup[isys]->SetBinContent(i,0);
    hdw[isys]->SetBinContent(i,0);
    hfe[isys]->SetBinContent(i,0);
    
    sigma_up = 0.;
    sigma_dw = 0.;
    for(int p=0;p<11;p++){
      sigma_A = dt_cv-hsyst[8+2*p]->GetBinContent(i);
      sigma_B = dt_cv-hsyst[9+2*p]->GetBinContent(i);
      sigma = abs(sigma_A);
      if(sigma<abs(sigma_B)) sigma = abs(sigma_B);
      
      if(sigma_A<0 && sigma_B<0){
	sigma_up += pow(sigma,2);
      }
      else if(sigma_A>0 && sigma_B>0){
	sigma_dw += pow(sigma,2);
      }
      else if(sigma_A>0 && sigma_B<0){
	sigma_up += pow(abs(sigma_B),2);
	sigma_dw += pow(abs(sigma_A),2);
      }
      else if(sigma_A<0 && sigma_B>0){
	sigma_up += pow(abs(sigma_A),2);
	sigma_dw += pow(abs(sigma_B),2);
      }
    }//end of loop over focusing
    
    hup[isys]->SetBinContent(i,sqrt(sigma_up));
    hdw[isys]->SetBinContent(i,sqrt(sigma_dw));
    
    sigma = sigma_up;
    if(sigma<sigma_dw) sigma = abs(sigma_dw);
    fe_val = sigma / dt_cv;
    if(sigma!=sigma)sigma = 0;
    if(dt_cv>0)hfe[isys]->SetBinContent(i,fe_val);
    
  }
  
  //Data:
  TGraphAsymmErrors* gdt = new TGraphAsymmErrors();
  double count = 0;
  for(int i=1;i<=NbinsX;i++){      
    double kin   = hdt->GetXaxis()->GetBinCenter(i);	
    double binw  = hdt->GetXaxis()->GetBinWidth(i);
    double cont  = hdt->GetBinContent(i);
    double errUp = 0;
    double errDw = 0;          
    for(int p=0;p<Nfinal_fe;p++){
	double up = hup[p]->GetBinContent(i);
	double dw = hdw[p]->GetBinContent(i);
	errUp += pow(up-cont,2); 
	errDw += pow(cont-dw,2); 
      }
      errUp = sqrt(errUp);
      errDw = sqrt(errDw);
      if(databin(i)){
	count++;
	gdt->SetPoint(count, kin, cont);
	gdt->SetPointError(count,binw/2, binw/2, errDw, errUp);
      }//databin
  } //bin X      
    
  //XS:
  TCanvas* cxs = new TCanvas();
  TLegend* leg = new TLegend(0.67,0.68,0.87,0.88);;
  double maxY = hmc->GetMaximum();		
  hmc->GetXaxis()->SetNdivisions(510);
  plot->canvas(cxs);
  plot->range(0.5, 5.0-0.0001,0,2.*(maxY));
  plot->axis(hmc,"",tit_nu.c_str(),tit_xs.c_str());
  leg->SetBorderSize(0);
  setMC1D(hmc);
  setData1D(gdt);
  hmc->Draw("hist");
  gdt->Draw("p1 same");  
  leg->SetTextSize(0.05);
  leg->AddEntry(hmc,  "MC", "fl");
  leg->AddEntry(gdt,"Data (Syst.)", "lp");
  leg->Draw();
  cxs->SaveAs(Form("%s/xs1D.png",dirOut));
  
  //FE:
  TCanvas* cfe = new TCanvas();
  TLegend* lfe  = new TLegend(0.2,0.5,0.45,0.88);
  hfe[0]->GetXaxis()->SetNdivisions(510);
  lfe->SetBorderSize(0);
  plot->canvas(cfe);
  plot->range(0.5, 5.0-0.0001,0,0.4);
  plot->axis(hfe[0],"",tit_nu.c_str(),tit_fe.c_str());
  for(int i=0;i<Nfinal_fe;i++){
    setUncer1D(hfe[i],i);
    if(i==0)hfe[i]->Draw("hist");
    else    hfe[i]->Draw("histsame");
    lfe->AddEntry(hfe[i],final_fe[i].c_str(),"l");
  }
  
  lfe->Draw();
  cfe->SaveAs(Form("%s/fe1D.png",dirOut));
 	      
  //Storing the final histograms:
  fileOut->cd();
  fileOut->mkdir("CrossSections");
  fileOut->cd("CrossSections");
  gdt->GetXaxis()->SetTitle(tit_nu.c_str());
  gdt->GetYaxis()->SetTitle(tit_xs.c_str());
  gdt->Write("mock_data");
  hmc->Write("mc");  
  fileOut->mkdir("FractUncer");
  fileOut->cd("FractUncer");
  for(int i=0;i<Nfinal_fe;i++){
    hfe[i]->Write(Form("FractUncer_%s",final_fe_name[i].c_str()));   
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
bool databin(int xbin){
  //  xbin starts at 1
  bool answer = false;
  if(xbin>0) answer = true;

  return answer;

}

////////////////////////////////
#ifndef __CINT__
int main(int argc, const char* argv[]){  
  xs1D(argv[1],argv[2],argv[2]);
  return 0;
}
#endif
