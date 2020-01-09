#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
from root_numpy import matrix

import argparse
import glob
import numpy as np
import os
import pickle

def find_outfn(fn):
    bfn = os.path.basename(fn)
    return bfn.rstrip('.root')+'_zero_suppress.root'

def make_tmatrix(np_mat):
    nrow = np_mat.shape[0]
    ncol = np_mat.shape[1]
    tmatrix = TMatrixF(nrow,ncol)
    for i in range(nrow):
        for j in range(ncol):
            tmatrix[i][j] = np_mat[i,j]
    return tmatrix

def process_one_file(infn):

    # load precalculated matrices
    tinf = TFile(infn)
    # herrmat = tinf.Get('cov_mat_eff')
    herrmat = tinf.Get('cov_mat')

    # operate in python
    nrow = herrmat.GetNbinsX()
    ncol = herrmat.GetNbinsY()
    cov = np.empty([nrow, ncol])
    for i in range(nrow):
        for j in range(ncol):
            cov[i,j] = herrmat.GetBinContent(i+1, j+1)
    
    # supress columns and rows with all elements zero
    nonzero_row_indices = [i for i in range(cov.shape[0]) if not np.allclose(cov[i,:],0,equal_nan=True)]
    nonzero_col_indices = [i for i in range(cov.shape[1]) if not np.allclose(cov[:,i],0,equal_nan=True)]
    cov_sup = cov[nonzero_row_indices,:]
    cov_sup = cov_sup[:,nonzero_col_indices]

    # make reduced matrices
    tcov_sup = make_tmatrix(cov_sup)
    hcov_sup = TH2F(tcov_sup)
    
    # write out results
    fout = TFile('../grid_output/{}'.format(find_outfn(infn)), 'recreate')
    tcov_sup.Write('cov_zero_sup')
    hcov_sup.Write('hcov_zero_sup')
    fout.Close()

    # write the array to file
    np.savetxt('array50.txt', cov_sup)

# query available iterations
# infns = glob.glob('../grid_output/unfolded_spectra_roounfold_iter_*.root')
infns = glob.glob('../grid_output/unfolded_spectra_roounfold_iter_50.root')

for infn in infns:
    process_one_file(infn)