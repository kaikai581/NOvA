double elec_angle(double x)
{
  double xrad = x*TMath::Pi()/180;
  double m = 0.511e-3; // electron rest mass in GeV
  double Enu = 2; // incident neutrino energy in GeV
  double A = 1+Enu/m;
  return atan(1/A/tan(xrad/2))/TMath::Pi()*180;
}

void angle_relation()
{
  TF1* fElectronAngle = new TF1("fElectronAngle", "elec_angle(x)", 0.0001, 180);
  fElectronAngle->Draw();
}
