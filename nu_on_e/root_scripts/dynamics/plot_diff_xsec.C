double diff_xsec(double x)
{
  double me = 0.511e-3; // electron rest mass in GeV
  double Enu = 2; // incident neutrino energy in GeV
  double s0 = 88.06e-46;
  double g1 = -0.27;
  double g2 = 0.23;
  return s0*(4*Enu*Enu*(me+Enu)*(me+Enu)*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x)*(g1*g1+g2*g2*(1-(2*me*Enu*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x))*(1-(2*me*Enu*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x))-g1*g2*(2*me*me*x*x)/((me+Enu)*(me+Enu)-Enu*Enu*x*x));
}

void plot_diff_xsec()
{
  TF1* fXsec = new TF1("fXsec", "diff_xsec(x)", 0, 1);
  fXsec->SetLineWidth(3);
  fXsec->Draw();
  fXsec->GetXaxis()->SetTitle("cos#theta");
  fXsec->GetXaxis()->CenterTitle();
}
