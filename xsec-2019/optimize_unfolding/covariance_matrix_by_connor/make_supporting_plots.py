#!/usr/bin/env python
"""
This script is going to print:
1. covariance matrix
2. distribution of cosine of any pair of columns
"""
from __future__ import print_function
from ROOT import *

if __name__ == '__main__':
    # retrieve the matrix
    infpn = 'input_files/mat_tol_1e-15.root'
    tinf = TFile(infpn)

    # covariance matrix in histogram
    hcov_mat = tinf.Get('iter20/hcov')
    hcov_mat.SetStats(0)
    hcov_mat.SetTitle('covariance matrix of 20 iterations')
    hcov_mat.Draw('colz')
    gPad.SetLogz(1)
    gPad.SaveAs('resulting_plots/cov_mat_iter20.png')

    # covariance matrix in TMatrix
    # cov_mat = tinf.Get('iter20/cov')
    # v = ROOT.TMatrixColumn(cov_mat, 0)
    # print(v)