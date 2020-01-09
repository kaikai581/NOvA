#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import os
import pickle
import struct

syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']
input_ds_names = ['roounfold','ndrespin']

if __name__ == '__main__':

    # command line argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument('-f','--file_index',type=int,default=0)
    parser.add_argument('-n','--ntrain',type=int,default=1000000)
    parser.add_argument('-d','--dataset_index',type=int,default=1)
    args = parser.parse_args()
    fidx = args.file_index
    ntrain = args.ntrain
    dsidx = args.dataset_index

    # determine the input datasets
    infpns = [os.path.join('input','{}_{}.root'.format(input_ds_names[dsidx],syst_name)) for syst_name in syst_names]

    # grab training data
    inf = TFile(infpns[fidx])
    tr = inf.Get('tr')

    # initialize response matrix with neutrino energy binning
    response = RooUnfoldResponse(20, 0.0, 5.0)
    response_osc = RooUnfoldResponse(20, 0.0, 5.0)

    # fill the response matrix with specified number of entries
    entry_used = 0
    for entry in tr:
        muke_cut = struct.unpack('B', entry.muke_cut)[0]
        muang_cut = struct.unpack('B', entry.muang_cut)[0]
        phasespace_cut = struct.unpack('B', entry.phasespace_cut)[0]
        truesig = struct.unpack('B', entry.truesig)[0]
        if truesig:
            response.Fill(entry.nue_reco, entry.nue_true, entry.weight)
            response_osc.Fill(entry.nue_osc_reco, entry.nue_true, entry.weight)
            entry_used += 1
        if entry_used >= ntrain:
            break

    # make output directory if not exist
    out_dir = 'trained_response_matrices/{}'.format(input_ds_names[dsidx])
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    # save to file
    with open(os.path.join(out_dir, '{}_matrix_1d.pkl'.format(syst_names[fidx])), 'w') as outf:
        pickle.dump(response, outf)
    with open(os.path.join(out_dir, '{}_osc_matrix_1d.pkl'.format(syst_names[fidx])), 'w') as outf:
        pickle.dump(response_osc, outf)