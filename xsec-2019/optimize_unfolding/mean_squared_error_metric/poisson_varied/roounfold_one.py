#!/usr/bin/env python
"""
Tested with one spectrum, one iteration. Time spent:
real    12m37.394s
user    11m23.842s
sys	    0m11.583s
"""

from __future__ import print_function
from ROOT import *

import os

if __name__ == '__main__':

    # command line parser
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--niter', type=int, default=1)
    args = parser.parse_args()
    niter = args.niter

    # get reco spectrum and response matrix
    tinf = TFile('processed_input/fluctuated_smeared.root')
    hData = tinf.Get('hsmeared')
    hresponse_matrix = tinf.Get('response_matrix')

    # prepare the response matrix for RooUnfold
    nbins = hData.GetNbinsX()
    xmin = hData.GetXaxis().GetXmin()
    xmax = hData.GetXaxis().GetXmax()
    hMeas_binning = TH1D('hMeas', 'hMeas', nbins, xmin, xmax)
    hTrue_binning = TH1D('hTrue', 'hTrue', nbins, xmin, xmax)
    response_matrix = RooUnfoldResponse(hMeas, hTrue, TH2D(hresponse_matrix))

    # unfold!
    unfold = RooUnfoldBayes(response_matrix, hData, niter)
    hUnfold = unfold.Hreco()

    # save the results
    htrue = tinf.Get('htrue')
    if not os.path.exists('output'):
        os.makedirs('output')
    toutf = TFile('output/unfluctuated_unfolded_iter_{}.root'.format(niter), 'recreate')
    htrue.Write('htrue')
    hData.SetLineColor(kGreen)
    hData.Write('hsmeared')
    hUnfold.SetLineWidth(2)
    hUnfold.SetLineColor(kBlue)
    hUnfold.Write('hunfolded')