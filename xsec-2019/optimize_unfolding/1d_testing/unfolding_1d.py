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
if not os.path.exists('output'):
    os.makedirs('output')

# setup input and output file names
if src_iter_chain == '0':
    fdata = TFile('input/data_background.root')
else:
    fdata = TFile('output/unfolded_spectra_{}.root'.format(src_iter_chain))
migration_matrix = pickle.load(open('input/nominal_matrix_1d.pkl', 'r'))
outf_pn = 'output/unfolded_spectra_{}-{}.root'.format(src_iter_chain, niter)

# load data to be unfolded
fdata.cd()
if src_iter_chain == '0':
    hData = fdata.Get('cv_est_sig')
else:
    hData = fdata.Get(src_iter_chain)

# unfold!
unfold = RooUnfoldBayes(migration_matrix, hData, niter)
hUnfold = unfold.Hreco()

# output to file
outf = TFile(outf_pn, 'recreate')
hData.Write(src_iter_chain)
hUnfold.Write('{}-{}'.format(src_iter_chain, niter))