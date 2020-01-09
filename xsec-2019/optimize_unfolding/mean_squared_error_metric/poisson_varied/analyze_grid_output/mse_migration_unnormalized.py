#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
from sklearn.metrics import mean_squared_error
import numpy as np
import os

def mse_one_bin(ibin, hists, hnom, weighted, required_minimum_count, weight_by_true):
    # prepare data to calculate
    true_count = hnom.GetBinContent(ibin)
    count_dist = [h.GetBinContent(ibin) for h in hists]
    count_true = [true_count for i in range(len(hists))]
    avg_cnt = np.mean(count_dist)
    # calculate bin weight
    if weighted:
        if weight_by_true: denom = true_count
        else:              denom = avg_cnt
        weight = 1./denom if true_count > required_minimum_count and avg_cnt > required_minimum_count else 0.
    else:
        weight = 1
    # calculate bias squared
    bias2 = (avg_cnt-true_count)**2
    # calculate variance
    variance = np.var(count_dist)
    return mean_squared_error(count_dist, count_true), bias2, variance, weight

if __name__ == '__main__':

    # command line argument parser
    import argparse
    import glob
    parser = argparse.ArgumentParser('Comand line argument parser.')
    parser.add_argument('-i', '--niter', type=int, default=1)
    parser.add_argument('-w', '--weighted', action='store_true')
    parser.add_argument('-l', '--least_bin_count', type=int, default=0)
    parser.add_argument('-t', '--weight_by_true', action='store_false')
    parser.add_argument('-na', '--no_append', action='store_true')
    args = parser.parse_args()
    niter = args.niter
    weighted = args.weighted
    required_minimum_count = args.least_bin_count
    weight_by_true = args.weight_by_true
    no_append = args.no_append
    
    infp = '/pnfs/nova/users/slin/unfolding/numuccinc_roounfoldbayes'

    avail_iters = sorted([int(x.rstrip('.root').split('_')[-3]) for x in glob.glob(os.path.join(infp, 'migration_unnormalized_smeared_*_iter_{}.root'.format(niter)))])
    
    infpns = [os.path.join(infp, 'migration_unnormalized_smeared_{}_iter_{}.root'.format(expid, niter)) for expid in avail_iters]

    # grab all histograms
    tinfs = [TFile(infpn) for infpn in infpns]
    hists = [f.Get('hunfolded') for f in tinfs]
    # true histogram
    tinf_nom = TFile('../processed_input/fluctuated_smeared.root')
    htrue_nom = tinf_nom.Get('htrue')
    
    # get MSE for each bin
    mse_all_bins = []
    weight_all_bins = []
    bias2_all_bins = []
    var_all_bins = []
    for i in range(1, hists[0].GetNbinsX()+1):
        mse, bias2, var, weight = mse_one_bin(i, hists, htrue_nom, weighted, required_minimum_count, weight_by_true)
        mse_all_bins.append(mse)
        weight_all_bins.append(weight)
        bias2_all_bins.append(bias2)
        var_all_bins.append(var)

    # save results
    result_mse = np.mean(mse_all_bins) if not weighted else np.mean(np.multiply(mse_all_bins, weight_all_bins))
    result_bias2 = np.mean(bias2_all_bins) if not weighted else np.mean(np.multiply(bias2_all_bins, weight_all_bins))
    result_var = np.mean(var_all_bins) if not weighted else np.mean(np.multiply(var_all_bins, weight_all_bins))
    ofn = 'mse_vs_iter{}{}{}.txt'.format('' if not weighted else ('_true' if weight_by_true else '_avg'), '_weighted' if weighted else '',  '_mincnt_{}'.format(required_minimum_count) if weighted else '')

    outp = 'results'
    os.system('mkdir -p {}'.format(outp))
    outfpn = os.path.join(outp, ofn)
    os.system('touch {}'.format(outfpn))
    os.system('echo {} {} {} {} {} |& tee {} {}'.format(niter, result_mse, result_bias2, result_var, result_bias2+result_var, '' if no_append else '-a', outfpn))