#!/usr/bin/env python

from __future__ import print_function
from root_numpy import matrix
from ROOT import *

import argparse
import numpy as np
import os, pickle

if __name__ == '__main__':

    # command line argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument('-i','--iterations',type=int,default=1,help='The iteration you want to look at.')
    parser.add_argument('-t','--tolerance',type=str,default='1e-15',help='Tolerance for TSVD.')
    args = parser.parse_args()
    tolerance = float(args.tolerance)
    
    # grab the input file with the covariance matrix to play with
    intf = TFile('../unfolded_spectra_iter_{}.root'.format(args.iterations))
    errmat = intf.Get('err_reco_1')

    # operate in python
    covmat = matrix(errmat)
    covinv = np.linalg.pinv(covmat,rcond=tolerance)
    # leftmult = covinv * covmat
    # rightmult = covmat * covinv
    
    # save results
    dest_folder = 'stored_matrices/{}'.format(tolerance)
    if not os.path.exists(dest_folder):
        os.makedirs(dest_folder)
    with open(os.path.join(dest_folder,'iter{}.pkl'.format(args.iterations)),'wb') as resf:
        # pickle.dump([covmat, covinv, leftmult, rightmult], resf)
        pickle.dump([covmat, covinv], resf)