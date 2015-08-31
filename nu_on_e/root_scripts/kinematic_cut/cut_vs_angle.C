/*
 * For the formula to relate the electron total energy and the electron
 * scattering angle, see
 * https://github.com/kaikai581/NOvA/blob/master/nu_on_e/documents/kinematics/nue_kinematics.pdf
 * 
 * This script will plot E*theta^2/(2*m) as a function of theta, and
 * see how much it deviates from unity.
 */

double Ee(double x)
{
  double m = 0.511e-3; // electron rest mass in GeV
  double Enu = 2; // incident neutrino energy in GeV
  double A = (1 + m/Enu)*(1 + m/Enu);
  double B = cos(x)*cos(x);
  return (A+B)/(A-B);
}

double cut_function(double x)
{
  return Ee(x)*x*x/2;
}

double cut_function2(double x)
{
  double m = 0.511e-3; // electron rest mass in GeV
  double Enu = 0.1; // incident neutrino energy in GeV
  double A = (1 + m/Enu)*(1 + m/Enu);
  double B = cos(x)*cos(x);
  return (A+B)/(A-B)*x*x/2;
}

double cut_function3(double x)
{
  double m = 0.511e-3; // electron rest mass in GeV
  double Enu = 10; // incident neutrino energy in GeV
  double A = (1 + m/Enu)*(1 + m/Enu);
  double B = cos(x)*cos(x);
  return (A+B)/(A-B)*x*x/2;
}

void cut_vs_angle()
{
  TF1* fEe = new TF1("fEe", "Ee(x)", 0,TMath::Pi());
  fEe->SetLineWidth(3);
  fEe->Draw();
  
  TCanvas* c2 = new TCanvas("c2");
  TF1* fCut = new TF1("fCut", "cut_function(x)", 0,2);
  fCut->SetLineColor(kYellow);
  fCut->SetLineWidth(3);
  fCut->Draw();
  gPad->SetGridy(1);
  fCut->GetXaxis()->SetTitle("#theta_{e}");
  fCut->GetXaxis()->CenterTitle();
  fCut->SetTitle("E_{e}#theta_{e}^{2}/2m_{e} as a function of #theta_{e}");
  
  TF1* fCut2 = new TF1("fCut2", "cut_function2(x)", 0,2);
  fCut2->SetLineColor(kBlue);
  fCut2->SetLineWidth(3);
  fCut2->Draw("same");
  
  TF1* fCut3 = new TF1("fCut3", "cut_function3(x)", 0,2);
  fCut3->SetLineWidth(3);
  fCut3->Draw("same");
  
  TLine* l = new TLine(TMath::Pi()/2,0.035,TMath::Pi()/2,2.9);
  l->Draw();
  
  TLegend* lg = new TLegend(.2,.7,.4,.8);
  lg->AddEntry(fCut2,"E_{#nu} = 0.1 GeV","L");
  lg->AddEntry(fCut,"E_{#nu} = 2 GeV","L");
  lg->AddEntry(fCut3,"E_{#nu} = 10 GeV","L");
  lg->SetBorderSize(0);
  lg->Draw();
}
