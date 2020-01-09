#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
from root_numpy import matrix
import copy
import numpy as np

def make_tmatrix(np_mat):
    nrow = np_mat.shape[0]
    ncol = np_mat.shape[1]
    tmatrix = TMatrixF(nrow,ncol)
    for i in range(nrow):
        for j in range(ncol):
            tmatrix[i][j] = np_mat[i,j]
    return tmatrix

def preprocess_covmat(iter, tolerance):
    tinf = TFile('input_files/rooUnfold{}iter.root'.format(iter))
    covmat = tinf.Get('cv/EAvail/Covariance')
    cov = matrix(covmat)
    # zero row/column supression
    # supress columns and rows with all elements zero
    nonzero_row_indices = [i for i in range(cov.shape[0]) if not np.allclose(cov[i,:],0)]
    nonzero_col_indices = [i for i in range(cov.shape[1]) if not np.allclose(cov[:,i],0)]
    cov_sup = cov[nonzero_row_indices,:]
    cov_sup = cov_sup[:,nonzero_col_indices]
    invcov = np.linalg.pinv(cov_sup,rcond=tolerance)
    return cov_sup, invcov

if __name__=='__main__':
    from glob import glob
    import argparse

    # input the truncation tolerance
    parser = argparse.ArgumentParser()
    parser.add_argument('-t','--tolerance',type=str,default='1e-15',help='Tolerance for TSVD.')
    args = parser.parse_args()
    tolerance = float(args.tolerance)
    iters = sorted([int(x.lstrip('input_files/rooUnfold').rstrip('iter.root')) for x in glob('input_files/rooUnfold*.root')])
    
    # save to root files
    toutf = TFile('input_files/mat_tol_{}.root'.format(tolerance), 'recreate')
    for i in iters:
        toutf.cd()
        toutf.mkdir('iter{}'.format(i))
        pycov, pyinvcov = preprocess_covmat(i, tolerance)
        cov_condnum = np.linalg.cond(pycov)
        invcov_condnum = np.linalg.cond(pyinvcov)

        cov = make_tmatrix(pycov)
        invcov = make_tmatrix(pyinvcov)
        hcov = TH2F(cov)
        hinvcov = TH2F(invcov)
        tcov_condnum = TParameter(float)('cov_condnum', cov_condnum)
        tinvcov_condnum = TParameter(float)('invcov_condnum', invcov_condnum)

        toutf.cd('iter{}'.format(i))
        cov.Write('cov')
        invcov.Write('invcov')
        hcov.Write('hcov')
        hinvcov.Write('hinvcov')
        tcov_condnum.Write()
        tinvcov_condnum.Write()
        print('iteration {} finished'.format(i))
