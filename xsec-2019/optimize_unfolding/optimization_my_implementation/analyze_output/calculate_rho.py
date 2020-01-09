#!/usr/bin/env python

from __future__ import print_function
from root_numpy import matrix

import argparse
import glob
import math
import matplotlib.pyplot as plt
import os
import pandas as pd
import pickle
import statistics

def find_niter(fn):
    bfn = os.path.basename(fn)
    return bfn.lstrip('iter').rstrip('.root')

def process_one_file(infn):

    # load precalculated matrices
    tinf = TFile(infn)
    tcov = tinf.Get('cov_zero_sup')
    cov = matrix(tcov)
    tinvcov = tinf.Get('covinv_zero_sup')
    invcov = matrix(tinvcov)

    # calculate the global correlation coefficient for each bin
    rhos = []
    mat_size = cov.shape[0]
    for i in range(mat_size):
        mult = cov[i,i]*invcov[i,i]
        # if mult < 0:
            # print('*****WARNING*****\nnegative number at {} with value {}'.format(i, mult))
        if mult >= 1:
            rhos.append(math.sqrt(1.-1./mult))
    
    # debugging info
    print('size of the rho vector for {} iterations:\n{}'.format(find_niter(infn), len(rhos)))
    
    # return results
    if len(rhos) == 0: return 0, 0
    return statistics.mean(rhos), len(rhos)

# choose truncation threshold used for TSVD
parser = argparse.ArgumentParser()
parser.add_argument('-t','--tolerance',type=str,help='tolerance threshold for TSVD',default='1e-10')
args = parser.parse_args()
tsvd_thr = args.tolerance

# query available iterations
infns = glob.glob('stored_matrices/{}/*.root'.format(tsvd_thr))

# if results exist, ask to recalculate
outfn = 'stored_matrices/{}/avg_glob_corr_coeff.txt'.format(tsvd_thr)
file_exists = os.path.isfile(outfn)
overwrite = False
if file_exists:
    overwrite_answer = raw_input('File already exists. Overwrite? Y = yes, N = no\n')
    overwrite = (overwrite_answer.lower() == 'y')

# if results do not exist or overwrite flag is true, recalculate
if overwrite or (not file_exists):
    rho_vs_iter = dict()
    rho_size = dict()
    for infn in infns:
        niter = find_niter(infn)
        rho_vs_iter[int(niter)], rho_size[int(niter)] = process_one_file(infn)
    with open(outfn, 'w') as f:
        for key in sorted(rho_vs_iter.keys()):
            f.write('{} {} {}\n'.format(key, rho_vs_iter[key], rho_size[key]))

# at this point, the results file should exit and should be ready for plots
df = pd.read_csv(outfn, delim_whitespace=True, header=None, names=['iterations', 'avg_corr_coeff', 'num_valid_rho'])
x = df['iterations'].values
y = df['avg_corr_coeff'].values
plt.scatter(x,y)
plt.ylim(bottom=.5)
plt.show()