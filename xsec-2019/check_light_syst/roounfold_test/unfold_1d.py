#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import os
import pickle

syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']
hnames_data = ['{}_est_sig'.format(sname) for sname in ['cv','lightdown','lightup','ckv','calibneg','calibpos','calibshape']]
unfold_algs = [RooUnfoldBayes, RooUnfoldSvd, RooUnfoldTUnfold]
unfold_names = ['bayes','svd','tunfold']
input_ds_names = ['roounfold','ndrespin']

def unfold_one_syst(sidx, unfold_alg):

    matrix_fpns = [os.path.join('trained_response_matrices', input_ds_names[dsidx], '{}_matrix_1d.pkl'.format(syst_name)) for syst_name in syst_names]
    matrix_osc_fpns = [os.path.join('trained_response_matrices', input_ds_names[dsidx], '{}_osc_matrix_1d.pkl'.format(syst_name)) for syst_name in syst_names]
    # load response matrix
    response = pickle.load(open(matrix_fpns[sidx], 'r'))
    response_osc = pickle.load(open(matrix_osc_fpns[sidx], 'r'))

    # load data to be unfolded
    fdata.cd()
    hData = fdata.Get(hnames_data[sidx])

    # unfold!
    n_it = 5
    if unfold_alg == RooUnfoldSvd: n_it = 10
    unfold = unfold_alg(response, hData, n_it)
    unfold_osc = unfold_alg(response_osc, hData, n_it)
    hUnfold = unfold.Hreco()
    hUnfold_osc = unfold_osc.Hreco()
    outf.cd()
    hUnfold.Write('unfolded_{}_xsec'.format(syst_names[sidx]))
    hUnfold_osc.Write('unfolded_{}_osc'.format(syst_names[sidx]))


if __name__ == '__main__':

    # command line argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument('-a','--unfold_algorithm',type=int,default=0,choices=range(len(unfold_algs)))
    parser.add_argument('-d','--dataset_index',type=int,default=1)
    # parser.add_argument('--osc',action='store_true')
    args = parser.parse_args()
    unfold_alg_idx = args.unfold_algorithm
    dsidx = args.dataset_index
    # osc_nue = args.osc
    
    # make sure output folder exists
    out_dir = 'output/{}'.format(input_ds_names[dsidx])
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    fdata = TFile('input/data_background.root')
    outf = TFile('{}/unfolded_spectra_{}.root'.format(out_dir,unfold_names[unfold_alg_idx]),'recreate')
    for i in range(len(syst_names)):
        unfold_one_syst(i, unfold_algs[unfold_alg_idx])
    