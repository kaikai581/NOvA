/*
 * 
 * This script propagates the errors in Fermi constant, electron mass,
 * and Weinberg angle into total cross section.
 * 
 * The value of the parameters are obtained from the NIST website,
 * whose source is CODATA recommended values.
 * 
 * All units used are powers of GeV.
 * 
*/

// Since the cross-section is proportional to the neutrino energy,
// I am calculating the cross-section at 1 GeV
double xsec_unit_energy(double Fc, double me, double s2thw)
{
  double G_F2 = Fc*Fc;
  double term1 = (1-2*s2thw)*(1-2*s2thw);
  double term2 = 4*s2thw*s2thw/3;
  return G_F2*me/2/TMath::Pi()*(term1+term2);
}

void xsec_err(int ninstances = 10000)
{
  TRandom3 ran_Fc(0);
  TRandom3 ran_me(0);
  TRandom3 ran_s2thw(0);
  
  for(int i = 0; i < ninstances; i++)
  {
    double Fc = ran_Fc.Gaus(1.1663787e-5, 0.0000006e-5);
    double me = ran_me.Gaus(0.5109989461e-3, 0.0000000031e-3);
    double s2thw = ran_s2thw.Gaus(0.2223, 0.0021);
    cout << xsec_unit_energy(Fc, me, s2thw) << endl;
  }
}
