#!/usr/bin/env python

from __future__ import print_function
from matplotlib.offsetbox import AnchoredText
from matplotlib.ticker import AutoMinorLocator, MaxNLocator
import matplotlib.pyplot as plt
import os
import pandas as pd

def make_one_plot(infpn, title, text_loc, iter_lim = -1):
    df = pd.read_csv(infpn, sep=' ', header=None, names = ['niter', 'mse', 'bias2', 'variance', 'bias2+variance'])
    if iter_lim > 0:
        df = df[df['niter'] <= iter_lim]
    miniter = df['niter'][df['mse'].idxmin(axis=0)]
    
    ax = df.plot(kind='scatter',x='niter',y='mse',color='red', label='mean squared error')
    df.plot(kind='scatter',x='niter',y='bias2',color='green', ax=ax, alpha=.6, label=r'bias$^2$')
    df.plot(kind='scatter',x='niter',y='variance',color='blue', ax=ax, alpha=.6, label='variance')
    plt.axvline(x=miniter, ls='--', c='magenta')
    ax.set_ylabel('score')
    ax.set_xlabel('number of iterations')
    ax.legend(loc='best')

    # place a text indicating where minimum happens
    anchored_text = AnchoredText('iteration of minimum = {}'.format(miniter), loc=text_loc, frameon=False, prop=dict(c='magenta', fontweight='bold'))
    ax.add_artist(anchored_text)
    # ax.set_title('{}{}'.format(title, miniter))
    ax.set_title(title)

    # make x ticks whole numbers
    if iter_lim > 0:
        ax.xaxis.set_major_locator(MaxNLocator(integer=True))

    # save to file
    outp = 'plots'
    if not os.path.exists(outp):
        os.makedirs(outp)
    outfpn = os.path.join(outp, os.path.basename(infpn).replace('.txt', '{}.pdf'.format('' if iter_lim < 0 else '_iterlim{}'.format(iter_lim))))
    plt.savefig(outfpn)

    ax.yaxis.set_minor_locator(AutoMinorLocator(2))
    plt.grid(linestyle='dotted', which='minor')
    plt.grid(linestyle='dotted', which='major')
    plt.savefig(outfpn.replace('.pdf', '_grid.pdf'))


if __name__ == '__main__':
    infpns = ['results/mse_vs_iter.txt', 'results/mse_vs_iter_true_weighted_mincnt_0.txt', 'results/mse_vs_iter_true_weighted_mincnt_100.txt']
    # titles = ['unweighted MSE, iteration of minimum = ',
    #           'weighted MSE with minimum count 0, iteration of minimum = ',
    #           'weighted MSE with minimum count 100, iteration of minimum = ']
    titles = ['unweighted MSE',
              'weighted MSE with minimum count 0',
              'weighted MSE with minimum count 100']
    text_locations = [8,9,8]
    text_locations_20 = [9,9,9]
    for i in range(len(infpns)):
        make_one_plot(infpns[i], titles[i], text_locations[i])
        make_one_plot(infpns[i], titles[i], text_locations_20[i], 20)
    