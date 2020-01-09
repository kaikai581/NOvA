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
    fdata = TFile('output/my_unfolded_spectra_{}.root'.format(src_iter_chain))
migration_matrix = pickle.load(open('input/nominal_matrix_1d.pkl', 'r'))
if niter >= 1:
    outf_pn = 'output/my_unfolded_spectra_{}-{}.root'.format(src_iter_chain, niter)
else:
    outf_pn = 'output/my_unfolded_spectra_0.root'

# load data to be unfolded
fdata.cd()
if src_iter_chain == '0':
    hData = fdata.Get('cv_est_sig')
else:
    hData = fdata.Get('h{}'.format(src_iter_chain))

# unfold!
unfold1 = RooUnfoldBayes(migration_matrix, hData, niter)
unfold1.SetVerbose(2)
hUnfold = unfold1.Hreco()
hErr = unfold1.Ereco()

# also output the updated prior
v_prior_output = unfold1.PriorDistribution()

# output to file
outf = TFile(outf_pn, 'recreate')
hData.Write('h{}'.format(src_iter_chain))
if niter >= 1:
    hUnfold.Write('h{}-{}'.format(src_iter_chain, niter))
else:
    migration_matrix.Write('response_matrix')
v_prior_output.Write('prior{}-{}'.format(src_iter_chain, niter))
outf.Close()


# bootstrapping!

# second stage
niter2 = 1
unfold2 = RooUnfoldBayes(migration_matrix, hData, v_prior_output, niter2)
unfold2.SetVerbose(2)
hUnfold2 = unfold2.Hreco()
hErr2 = unfold2.Ereco()

# total unfolding
niter_tot = niter + niter2
unfold_tot = RooUnfoldBayes(migration_matrix, hData, niter_tot)
unfold_tot.SetVerbose(2)
hUnfold_tot = unfold_tot.Hreco()
hErr_tot = unfold_tot.Ereco()

# # print counts
# for i in range(1, hUnfold2.GetNbinsX()):
#     print(hUnfold2.GetBinContent(i), hUnfold_tot.GetBinContent(i))

# output all information to file
outf = TFile('output/diagnostic.root', 'recreate')
foldernames = ['iter{}-{}'.format(src_iter_chain, niter), 'iter{}-{}-{}'.format(src_iter_chain, niter, niter2), 'iter{}-{}'.format(src_iter_chain, niter_tot)]
unfold_algs = [unfold1, unfold2, unfold_tot]
unfold_inputs = [hData, hData, hData]
unfolded_specs = [hUnfold, hUnfold2, hUnfold_tot]
unfolded_covmats = [hErr, hErr2, hErr_tot]
migration_matrix.Mresponse().Write('mresponse')
for i in range(len(foldernames)):
    foldername = foldernames[i]
    unfold_input = unfold_inputs[i]
    unfolded_spec = unfolded_specs[i]
    unfolded_covmat = unfolded_covmats[i]
    unfold = unfold_algs[i]
    outf.cd('/')
    outf.mkdir(foldername)
    outf.cd(foldername)
    unfold_input.Write('hinput')
    unfolded_spec.Write('hunfolded')
    unfolded_covmat.Write('hcovmat')
    unfold.UnfoldingMatrix().Write('munfolding')
    unfold.InputPrior().Write('vprior_in')
    unfold.PriorDistribution().Write('vprior_out')
