#!/usr/bin/env python

# This script takes a covariance matrix, calculates the cosine angles
# of adjacent column vectors, and make plots.

from __future__ import print_function
from ROOT import *
from root_numpy import matrix

import argparse
import matplotlib.pyplot as plt
import numpy as np
import os


def cosine_between(v1, v2):

    v1_len = np.linalg.norm(v1)
    v2_len = np.linalg.norm(v2)
    if v1_len == 0: return -2
    if v2_len == 0: return -2
    
    v1_unit = v1/v1_len
    v2_unit = v2/v2_len

    return np.dot(v1_unit.transpose(), v2_unit)[0,0]


parser = argparse.ArgumentParser()
parser.add_argument('-i','--iteration',type=int,default=2,help='The iteration to look at.')
parser.add_argument('-t','--tolerance',type=str,default='1e-10',help='Choose tolerance.')
args = parser.parse_args()
iteration = args.iteration
tolerance = args.tolerance

# open the input file
infn = 'stored_matrices/{}/zero_suppressed_matrix_iter{}.root'.format(tolerance, iteration)
tinf = TFile(infn)
# tinf.ls()

# retrieve the zero suppressed covariance matrix and cast it into a numpy matrix
tcov_zero_sup = tinf.Get('cov_zero_sup')
cov_mat = matrix(tcov_zero_sup)

# results container
cosines = []
# loop through all columns
for i in range(cov_mat.shape[1]-1):
    # get the adjacent column vectors
    v1 = cov_mat[:,i]
    v2 = cov_mat[:,i+1]
    cosines.append(cosine_between(v1, v2))

# make plot
bins = np.linspace(-1.0, 1.0, 51)
plt.figure(figsize=(5,5))
plt.hist(cosines, bins=bins, histtype='step')
plt.xlabel(r'$\cos(\mathbf{v}_i,\mathbf{v}_{i+1})$')
plt.text(-1, 100, r'$\mathbf{v}_i$ column vectors of'"\n"'the covariance matrix.'"\n"r'$i \in [0,N_{col}-2]$')
plt.text(0.1, 175, r'Need only one entry with'"\n"r'$\cos\approx1$ to render'"\n"'the matrix singular.')

# save the figure
if not os.path.exists('plots'):
    os.makedirs('plots')
plt.savefig('plots/collinearity.pdf')