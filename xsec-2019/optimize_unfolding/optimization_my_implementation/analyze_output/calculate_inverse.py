#!/usr/bin/env python

from __future__ import print_function
from root_numpy import matrix
from ROOT import *

import argparse
import numpy as np
import os, pickle

def make_tmatrix(np_mat):
    nrow = np_mat.shape[0]
    ncol = np_mat.shape[1]
    tmatrix = TMatrixF(nrow,ncol)
    for i in range(nrow):
        for j in range(ncol):
            tmatrix[i][j] = np_mat[i,j]
    return tmatrix

if __name__ == '__main__':

    # command line argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument('-i','--iterations',type=int,default=1,help='The iteration you want to look at.')
    parser.add_argument('-t','--tolerance',type=str,default='1e-10',help='Tolerance for TSVD.')
    args = parser.parse_args()
    tolerance = float(args.tolerance)

    # grab the input file with the covariance matrix to play with
    inpn = '../grid_output/unfolded_spectra_roounfold_iter_{}_zero_suppress.root'.format(args.iterations)
    intf = TFile(inpn)
    errmat = intf.Get('cov_zero_sup')
    herrmat = intf.Get('hcov_zero_sup')

    # operate in python
    covmat = matrix(errmat)
    covinv = np.linalg.pinv(covmat,rcond=tolerance)

    # transform back to ROOT
    tcovinv = make_tmatrix(covinv)
    hcovinv = TH2F(tcovinv)

    # save results
    dest_folder = 'stored_matrices/{}'.format(tolerance)
    if not os.path.exists(dest_folder):
        os.makedirs(dest_folder)
    toutf = TFile(os.path.join(dest_folder,'iter{}.root'.format(args.iterations)), 'recreate')
    errmat.Write('cov_zero_sup')
    herrmat.Write('hcov_zero_sup')
    tcovinv.Write('covinv_zero_sup')
    hcovinv.Write('hcovinv_zero_sup')
    toutf.Close()