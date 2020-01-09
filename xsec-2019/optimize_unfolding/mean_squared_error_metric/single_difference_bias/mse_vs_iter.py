#!/usr/bin/env python

from __future__ import print_function
from array import array
from ROOT import *

import os, sys

def bias_distribution(niter, weighted):
    # open input file
    tinf = TFile('input_files_by_connor/processed_iter_{}.root'.format(niter))

    # retrieve all kinds of data
    true_spec = tinf.Get('true_spec')
    unfolded_spec = tinf.Get('unfolded_spec')
    cov_mat = tinf.Get('cov_mat')

    # check relevant sizes
    nbins_true = true_spec.GetNbinsX()
    nbins_unfold = unfolded_spec.GetNbinsX()
    ncol_cov = cov_mat.GetNrows()
    if nbins_true != nbins_unfold or nbins_unfold != ncol_cov:
        print('Size mismatch in data! Aborting...')
        sys.exit()
    
    if weighted:
        xtitle = 'relative bias'
    else:
        xtitle = 'absolute bias'
    hbias = TH1F('hbias','iteration {}'.format(niter),50,10,5)
    hbias.SetLineWidth(2)
    hbias.GetXaxis().SetTitle(xtitle)
    hbias.GetXaxis().CenterTitle()
    for i in range(nbins_true):
        wei = 1
        if weighted:
            wei = unfolded_spec.GetBinContent(i+1)
        if wei > 0:
            hbias.Fill((unfolded_spec.GetBinContent(i+1)-true_spec.GetBinContent(i+1))/wei)
    hbias.Draw()
    hbias.SetStats(0)

    if weighted:
        fname = 'plots/relative_bias_dist_{}.pdf'.format(niter)
    else:
        fname = 'plots/absolute_bias_dist_{}.pdf'.format(niter)
    gPad.SetLogy(1)
    gPad.SaveAs(fname)




def calculate_bias_variance(niter, weighted):

    # open input file
    tinf = TFile('input_files_by_connor/processed_iter_{}.root'.format(niter))
    
    # retrieve all kinds of data
    true_spec = tinf.Get('true_spec')
    unfolded_spec = tinf.Get('unfolded_spec')
    cov_mat = tinf.Get('cov_mat')

    # check relevant sizes
    nbins_true = true_spec.GetNbinsX()
    nbins_unfold = unfolded_spec.GetNbinsX()
    ncol_cov = cov_mat.GetNrows()
    if nbins_true != nbins_unfold or nbins_unfold != ncol_cov:
        print('Size mismatch in data! Aborting...')
        sys.exit()
    
    b2 = 0
    vari = 0
    for i in range(nbins_true):
        wei = 1
        if weighted:
            wei = unfolded_spec.GetBinContent(i+1)
        if wei > 0:
            b2 += (unfolded_spec.GetBinContent(i+1)-true_spec.GetBinContent(i+1))**2/wei
            vari += cov_mat[i][i]/wei
    b2 /= nbins_true
    vari /= nbins_true

    return b2, vari

def get_niters():
    import glob
    return sorted([int(x.rstrip('.root').split('_')[-1]) for x in glob.glob('input_files_by_connor/processed_iter_*.root')])

if __name__ == '__main__':
    
    # import argparse
    # parser = argparse.ArgumentParser()
    # parser.add_argument('-i','--niter',type=int,default=1)
    # args = parser.parse_args()
    # niter = args.niter

    # open directory for plots
    if not os.path.exists('plots'):
        os.makedirs('plots')

    # grab all available iterations
    niters = get_niters()
    
    # bias and variance as functions of iterations
    bias2s = array('d')
    variances = array('d')
    xiter = array('d')
    for niter in niters:
        xiter.append(niter)
        bias2, variance = calculate_bias_variance(niter, True)
        bias2s.append(bias2)
        variances.append(variance)

        # calculate bias distribution
        bias_distribution(niter, True)
    
    # plot bias
    gbias = TGraph(len(niters),xiter,bias2s)
    gbias.SetLineColor(kBlue)
    gbias.SetLineWidth(2)
    gbias.SetMarkerColor(kBlue)
    gbias.SetMarkerStyle(22)
    gbias.Draw('APL')
    gbias.SetTitle('bin-averaged bias')
    gbias.GetXaxis().SetTitle('iteration')
    gbias.GetXaxis().CenterTitle()
    gPad.SetLogy(1)
    gPad.SaveAs('plots/bias.pdf')

    # plot variance
    gvari = TGraph(len(niters),xiter,variances)
    gvari.SetLineColor(kRed)
    gvari.SetLineWidth(2)
    gvari.SetMarkerColor(kRed)
    gvari.SetMarkerStyle(21)
    gvari.Draw('APL')
    gvari.SetTitle('bin-averaged variance')
    gvari.GetXaxis().SetTitle('iteration')
    gvari.GetXaxis().CenterTitle()
    gPad.SetLogy(0)
    gPad.SaveAs('plots/variance.pdf')

    # raw_input('Press enter to continue.')