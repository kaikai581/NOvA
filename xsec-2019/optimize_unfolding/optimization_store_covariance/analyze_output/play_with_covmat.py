#!/usr/bin/env python

from __future__ import print_function
from ROOT import *

import os

def process_one_iteration(fidx):
    errmat = intfs[fidx].Get('TMatrixT<float>')
    herrmats[fidx] = TH2F(errmat)

    # make plots and save to file
    c1 = TCanvas('c1','c1',600,600)
    herrmats[fidx].Draw('colz')
    herrmats[fidx].SetStats(0)
    gPad.SetLogz(1)
    if not os.path.exists('../result_plots'):
        os.makedirs('../result_plots')
    
    niter = avail_iters[fidx]
    if niter <= 1:
        htitle = '{} iteration'.format(niter)
    else:
        htitle = '{} iterations'.format(niter)
    # gPad.SaveAs('../result_plots/cov_mat_iter_{}.pdf'.format(niter))
    herrmats[fidx].SetTitle(htitle)
    gPad.SaveAs('../result_plots/cov_mat_iter_{}.png'.format(niter))
    # gPad.SetLogz(0)
    # gPad.SaveAs('../result_plots/lin_cov_mat_iter_{}.pdf'.format(niter))
    # gPad.SaveAs('../result_plots/lin_cov_mat_iter_{}.png'.format(niter))

avail_iters = ['1','4','6','7','8','9','10','11','14','15']
infns = ['../unfolded_spectra_iter_{}.root'.format(i) for i in avail_iters]
intfs = [TFile(infn) for infn in infns]
herrmats = dict()

for i in range(len(intfs)):
    process_one_iteration(i)
# raw_input('press enter to continue.')