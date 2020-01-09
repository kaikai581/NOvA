#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import os
import pickle

parser = argparse.ArgumentParser()
parser.add_argument('-s', '--src_iter_chain', type=str, default='0')
parser.add_argument('-i', '--niter', type=int, default=1)
args = parser.parse_args()
src_iter_chain = args.src_iter_chain
niter = args.niter

# make output directory
outdir = 'output/update_matrix'
if not os.path.exists(outdir):
    os.makedirs(outdir)

# setup input and output file names
if src_iter_chain == '0':
    fdata = TFile('input/data_background.root')
else:
    fdata = TFile('{}/unfolded_spectra_{}.root'.format(outdir, src_iter_chain))
outf_pn = '{}/unfolded_spectra_{}-{}.root'.format(outdir, src_iter_chain, niter)

# load data to be unfolded
fdata.cd()
if src_iter_chain == '0':
    hData = fdata.Get('cv_est_sig')
    migration_matrix = pickle.load(open('input/nominal_matrix_1d.pkl', 'r'))
else:
    hData = fdata.Get(src_iter_chain)
    nbins = hData.GetNbinsX()
    xmin = hData.GetXaxis().GetXmin()
    xmax = hData.GetXaxis().GetXmax()
    hMeas = TH1D('hMeas', 'hMeas', nbins, xmin, xmax)
    hTrue = TH1D('hTrue', 'hTrue', nbins, xmin, xmax)
    tmatrix_unfolding = fdata.Get('M{}'.format(src_iter_chain))
    migration_matrix = RooUnfoldResponse(hMeas, hTrue, TH2D(tmatrix_unfolding))

# unfold!
unfold = RooUnfoldBayes(migration_matrix, hData, niter)
unfold.SetVerbose(2)
hUnfold = unfold.Hreco()
mUnfold = unfold.UnfoldingMatrix()

# output to file
outf = TFile(outf_pn, 'recreate')
hData.Write(src_iter_chain)
if src_iter_chain == '0':
    migration_matrix.Write('M{}'.format(src_iter_chain))
else:
    tmatrix_unfolding.Write('M{}'.format(src_iter_chain))
hUnfold.Write('{}-{}'.format(src_iter_chain, niter))
mUnfold.Write('M{}-{}'.format(src_iter_chain, niter))