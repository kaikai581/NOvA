double diff_xsec(double x)
{
  double me = 0.511e-3; // electron rest mass in GeV
  double Enu = 2; // incident neutrino energy in GeV
  double s0 = 88.06e-46;
  double g1 = -0.27;
  double g2 = 0.23;
  return s0*(4*Enu*Enu*(me+Enu)*(me+Enu)*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x)*(g1*g1+g2*g2*(1-(2*me*Enu*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x))*(1-(2*me*Enu*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x))-g1*g2*(2*me*me*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x));
}

//~ double cumul_xsec(double x) // x in degree
//~ {
  //~ double lim = cos(x/180*TMath::Pi());
  //~ cout << lim << endl;
  //~ TF1 fXsec("fXsec", "diff_xsec(x)", 0, 1);
  //~ double tot_xsec = fXsec.IntegralOneDim(0,1);
  //~ return fXsec.IntegralOneDim(lim,1)/tot_xsec;
//~ }

void plot_diff_xsec()
{
  TF1* fXsec = new TF1("fXsec", "diff_xsec(x)", 0, 1);
  fXsec->SetLineWidth(3);
  fXsec->SetTitle("#nu_{#mu}-e differential cross section (E_{#nu} = 2 GeV)");
  fXsec->Draw();
  fXsec->GetXaxis()->SetTitle("cos#theta");
  fXsec->GetXaxis()->CenterTitle();
  fXsec->GetYaxis()->SetTitle("d#sigma/dcos#theta (cm^{-2})");
  fXsec->GetYaxis()->CenterTitle();
  fXsec->GetYaxis()->SetTitleOffset(1.4);
  gPad->SetLogy(1);
  
  double totXsec = fXsec->Integral(0,1);
  for(int i = 0; i < 100; i++)
  {
    cout << fXsec->Integral(0.01*i,1)/totXsec << endl;
  }
  
  //~ TCanvas* c2 = new TCanvas("c2");
  //~ TF1* fCXsec = new TF1("fCXsec", "cumul_xsec(x)", 0, 90);
  //~ fCXsec->Draw();
  //~ cout << cumul_xsec(45) << endl;
}
