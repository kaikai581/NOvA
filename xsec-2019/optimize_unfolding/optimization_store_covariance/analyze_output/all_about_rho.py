#!/usr/bin/env python

from __future__ import print_function
from ROOT import *

import glob
import matplotlib.pyplot as plt
import os
import pandas as pd

# available numbers for TSVD tolerance
thresholds = ['1e-15','1e-10','1e-05']

# read data into dataframes
dfs = dict()
for th in thresholds:
    dfs[th] = pd.read_csv('stored_matrices/{}/avg_glob_corr_coeff.txt'.format(th),sep=" ",header=None,names=['niter','avg_corr_coeff','nvalid'])
    df = dfs[th]
    # append other data from ROOT files
    for iter in df['niter']:
        rootfn = 'stored_matrices/{}/zero_suppressed_matrix_iter{}.root'.format(th, iter)
        if os.path.exists(rootfn):
            rootf = TFile(rootfn)
            trank = rootf.Get('rank_original')
            df.loc[df['niter'] == iter, 'rank'] = trank.GetVal()
    df['nvalid_rank_ratio'] = df['nvalid']/df['rank']

# make plots out of the dataframes
fig = plt.figure(figsize=(8,6))
ax1 = fig.add_subplot(311)
# matrix ranks with different SVD tolerance
for th in thresholds:
    # Markers in legend is not shown correctly.
    # Known issue. See https://github.com/pandas-dev/pandas/issues/14958.
#     dfs[th].plot(x='niter',y='rank',ax=ax1,marker='o',label=th,subplots=True)
    dfs[th].plot(x='niter',y='rank',ax=ax1,label=th,marker='.')
plt.xlabel('number of iterations')
plt.ylabel('rank of matrix')

ax2 = fig.add_subplot(312)
for th in thresholds:
    dfs[th].plot(x='niter',y='nvalid',ax=ax2,label=th,marker='.')
plt.xlabel('number of iterations')
plt.ylabel(r"valid $\rho_j$'s")

ax3 = fig.add_subplot(313)
for th in thresholds:
    dfs[th].plot(x='niter',y='nvalid_rank_ratio',ax=ax3,label=th,marker='.')
plt.xlabel('number of iterations')
plt.ylabel(r"valid $\rho_j$'s/rank")

# show the results
plt.tight_layout()
if not os.path.exists('plots'):
    os.makedirs('plots')
plt.savefig('plots/valid_rank_ratio.pdf')

# the second figure will show the final results of average global correlation coefficient
fig2 = plt.figure(figsize=(8,6))
ax4 = fig2.add_subplot(111)
for th in thresholds:
    dfs[th].plot(x='niter',y='avg_corr_coeff',ax=ax4,label=th,marker='o')
plt.xlabel('number of iterations')
plt.ylabel(r'$\rho_{avg}$')
plt.savefig('plots/average_rho.pdf')
