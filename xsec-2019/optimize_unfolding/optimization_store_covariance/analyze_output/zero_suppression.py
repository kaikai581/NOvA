#!/usr/bin/env python

from __future__ import print_function
from ROOT import *

import argparse
import glob
import numpy as np
import os
import pickle

def find_niter(fn):
    bfn = os.path.basename(fn)
    return bfn.lstrip('iter').rstrip('.pkl')

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
    with open(infn, 'rb') as f:
        cov, invcov = pickle.load(f)
    
    # print information
    print('\nshape of the covariance matrix:\n', cov.shape)
    # print('\ncondition number:\n', np.linalg.cond(cov))

    # supress columns and rows with all elements zero
    nonzero_row_indices = [i for i in range(cov.shape[0]) if not np.allclose(cov[i,:],0)]
    nonzero_col_indices = [i for i in range(cov.shape[1]) if not np.allclose(cov[:,i],0)]
    # print(nonzero_row_indices,nonzero_col_indices)
    cov_sup = cov[nonzero_row_indices,:]
    cov_sup = cov_sup[:,nonzero_col_indices]
    cond_num = np.linalg.cond(cov_sup)
    rank_zero_sup = np.linalg.matrix_rank(cov_sup,hermitian=True, tol=float(tsvd_thr))
    rank_original = np.linalg.matrix_rank(cov,hermitian=True, tol=float(tsvd_thr))
    print('\nshape of the zero suppressed covariance matrix:\n', cov_sup.shape)
    print('\ncondition number:\n', cond_num)
    tcond_num = TParameter(float)('cond_num_zero_sup', cond_num)
    trank_zero_sup = TParameter(int)('rank_zero_sup', rank_zero_sup)
    trank_original = TParameter(int)('rank_original', rank_original)
    
    tcov_sup = make_tmatrix(cov_sup)
    hcov_sup = TH2F(tcov_sup)
    tcov = make_tmatrix(cov)
    hcov = TH2F(tcov)
    
    fout = TFile('stored_matrices/{}/zero_suppressed_matrix_iter{}.root'.format(tsvd_thr, find_niter(infn)), 'recreate')
    tcov.Write('cov_orginal')
    hcov.Write('hcov_orginal')
    trank_original.Write()
    tcov_sup.Write('cov_zero_sup')
    hcov_sup.Write('hcov_zero_sup')
    trank_zero_sup.Write()
    tcond_num.Write()
    fout.Close()

# choose truncation threshold used for TSVD
parser = argparse.ArgumentParser()
parser.add_argument('-t','--tolerance',type=str,help='tolerance threshold for TSVD',default='1e-10')
args = parser.parse_args()
tsvd_thr = args.tolerance

# query available iterations
infns = glob.glob('stored_matrices/{}/*.pkl'.format(tsvd_thr))

for infn in infns:
    process_one_file(infn)