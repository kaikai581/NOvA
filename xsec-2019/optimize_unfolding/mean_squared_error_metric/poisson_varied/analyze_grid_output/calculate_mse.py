#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import glob
import os

def single_iteration_mse(niter, weighted):
    bias2_avg_bin = 0
    variance_avg_bin = 0
    ruf_var_avg_bin = 0

    tinf = TFile('processed_grid_output/iter{}.root'.format(niter))
    htrue_nom = tinf.Get('htrue_nom')
    hunfoldeds = [tinf.Get('unfolded_hists/hunfolded_{}'.format(k)) for k in range(100)]
    
    # pull RooUnfold's covariance matrix directly
    ruf_path = '/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/mean_squared_error_metric/single_difference_bias/input_files_by_connor'
    ruf_inf = TFile(os.path.join(ruf_path, 'processed_iter_{}.root'.format(niter)))
    ruf_cov_mat = ruf_inf.Get('cov_mat')

    # container for the mean histogram
    hunfolded_mean = htrue_nom.Clone('hunfolded_mean')

    # calculate the mean histogram
    for ibin in range(1, hunfolded_mean.GetNbinsX()+1):
        cnt = 0.
        for iexp in range(100):
            cnt += hunfoldeds[iexp].GetBinContent(ibin)
        hunfolded_mean.SetBinContent(ibin, cnt/100.)
    
    # calculate the bias and variance terms
    nbins = hunfolded_mean.GetNbinsX()
    for ibin in range(1, nbins+1):
        wei = 1
        true_bin_cnt = htrue_nom.GetBinContent(ibin)
        mean_bin_cnt = hunfolded_mean.GetBinContent(ibin)
        if true_bin_cnt < 100 or mean_bin_cnt < 100: continue
        if weighted and true_bin_cnt > 0: wei = true_bin_cnt
        
        bi2 = (mean_bin_cnt-true_bin_cnt)**2
        bias2_avg_bin += bi2/wei

        # calculate variance on my own
        avg_cnt2 = 0
        for iexp in range(100):
            avg_cnt2 += (hunfoldeds[iexp].GetBinContent(ibin))**2
        avg_cnt2 /= 100.
        var = avg_cnt2 - mean_bin_cnt**2
        variance_avg_bin += var/wei

        # retrieve variance from RooUnfold
        ruf_var_avg_bin += ruf_cov_mat[ibin-1][ibin-1]/wei


    bias2_avg_bin /= float(nbins)
    variance_avg_bin /= float(nbins)
    ruf_var_avg_bin /= float(nbins)

    return bias2_avg_bin, variance_avg_bin, ruf_var_avg_bin

if __name__ == '__main__':
    parser = argparse.ArgumentParser('Command line argument parser.')
    parser.add_argument('-i', '--niter', type=int, default=1)
    args = parser.parse_args()
    niter = args.niter
    # niters = sorted([int(os.path.basename(x).rstrip('.root').lstrip('iter')) for x in glob.glob('processed_grid_output/*.root')])
    
    bias2_avg_bin, variance_avg_bin, ruf_var_avg_bin = single_iteration_mse(niter, True)
    print(niter, bias2_avg_bin, ruf_var_avg_bin)