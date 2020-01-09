#!/usr/bin/env python

# This script compares the condition number of
# the original covariance matrix with the zero suppressed one.

from __future__ import print_function
from ROOT import *

import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-t','--tolerance',type=str,default='1e-15',help='Tolerance for SVD.')
parser.add_argument('-i','--iteration',type=int,default=2,help='Number of iterations to look at.')
args = parser.parse_args()
tolerance = args.tolerance
iteration = args.iteration

infn = 'stored_matrices/{}/zero_suppressed_matrix_iter{}.root'.format(tolerance,iteration)

# open the root file
intf = TFile(infn)
intf.ls()

# retrieve objects
hcov_orginal = intf.Get('hcov_orginal')
hcov_zero_sup = intf.Get('hcov_zero_sup')
cond_num_zero_sup = intf.Get('cond_num_zero_sup')

# make plots
canv = TCanvas('c1','c1',1000,500)
canv.Divide(2,1)
canv.cd(1)
hcov_orginal.SetTitle('condition number: #infty')
hcov_orginal.SetStats(0)
hcov_orginal.SetMinimum(1e-3)
hcov_orginal.Draw('colz')
gPad.SetRightMargin(.15)
gPad.SetLogz(1)
canv.cd(2)
hcov_zero_sup.SetTitle('condition number: {0:.2e}'.format(cond_num_zero_sup.GetVal()))
hcov_zero_sup.SetStats(0)
hcov_zero_sup.SetMinimum(1e-3)
hcov_zero_sup.Draw('colz')
gPad.SetRightMargin(.15)
gPad.SetLogz(1)

# save to file
if not os.path.exists('plots'):
    os.makedirs('plots')
canv.SaveAs('plots/iter{}tol{}.png'.format(iteration,tolerance))

raw_input('press enter to continue')