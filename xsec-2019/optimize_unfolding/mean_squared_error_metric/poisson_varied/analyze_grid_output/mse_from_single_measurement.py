#!/usr/bin/env python

import numpy as np
import os
import ROOT

def mse_one_iter(hunf):
    bin_diff = np.array([hunf.GetBinContent(i)-htrue_nom.GetBinContent(i) for i in range(1, 1+hunf.GetNbinsX())])
    return np.average(bin_diff**2)

if __name__ == '__main__':
    # grab input files
    infpn_nom = '/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/mean_squared_error_metric/poisson_varied/analyze_grid_output/processed_grid_output/iter1.root'
    infpns = ['/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/mean_squared_error_metric/single_difference_bias/input_files_by_connor/processed_iter_{}.root'.format(i) for i in range(1, 21)]
    tinf_nom = ROOT.TFile(infpn_nom)
    tinfs = [ROOT.TFile(fpn) for fpn in infpns]
    
    # grab nominal and unfolded spectra
    htrue_nom = tinf_nom.Get('htrue_nom')
    hunfoldeds = [f.Get('unfolded_spec') for f in tinfs]
    mses = [mse_one_iter(hunfolded) for hunfolded in hunfoldeds]

    print(mses)