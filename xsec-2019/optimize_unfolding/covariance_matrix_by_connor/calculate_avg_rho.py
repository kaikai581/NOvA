#!/usr/bin/env python

from __future__ import print_function
from array import array
from ROOT import *
import math
import os
import statistics

def process_one_tolerance(tolerance):
    tinf = TFile('input_files/mat_tol_{}.root'.format(tolerance))
    rhos_iter = dict()
    valid_rhok = dict()
    for iter in iters:
        cov = tinf.Get('iter{}/cov'.format(iter))
        invcov = tinf.Get('iter{}/invcov'.format(iter))

        rho_k = []
        M = cov.GetNrows()
        for i in range(M):
            mult = cov[i][i]*invcov[i][i]
            if mult >= 1:
                rho_k.append(math.sqrt(1-1./mult))
        rhos_iter[iter] = statistics.mean(rho_k)
        valid_rhok[iter] = len(rho_k)
    return rhos_iter, valid_rhok


if __name__ == '__main__':
    from glob import glob
    tols = sorted([x.lstrip('input_files/mat_tol_').rstrip('.root') for x in glob('input_files/mat_tol_*.root')])
    # get the number of iterations to run through
    iters = sorted([int(x.lstrip('input_files/rooUnfold').rstrip('iter.root')) for x in glob('input_files/rooUnfold*.root')])

    # choose the tolerance file to process
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('-ti','--tolerance_index',type=int,default=0,help='Tolerance index for opening files.')
    args = parser.parse_args()
    ti = args.tolerance_index
    tol = tols[ti]
    
    # make the output directory
    if not os.path.exists('output'):
        os.makedirs('output')

    rho, valid_rhok = process_one_tolerance(tol)



    # define colors
    colors = [kOrange, kPink, kViolet, kAzure, kTeal, kSpring, kRed, kMagenta, kBlue, kCyan, kGreen, kYellow]
    c_index = ti % len(colors)
    color = colors[c_index] + ti/len(colors)
    # make ROOT graphs
    x = array('d')
    y1 = array('d')
    y2 = array('d')
    n = len(rho)
    # grho = TGraph()
    for iter in iters:
        x.append(iter)
        y1.append(rho[iter])
        y2.append(valid_rhok[iter])
    g1 = TGraph(n,x,y1)
    g1.SetLineColor(color)
    g1.SetMarkerColor(color)
    g1.SetMarkerStyle(21)
    g1.SetLineWidth(2)
    g1.GetXaxis().CenterTitle()
    g1.GetXaxis().SetTitle('iteration')
    g1.SetTitle('average global correlation coefficient')
    g2 = TGraph(n,x,y2)
    g2.SetLineColor(color)
    g2.SetMarkerColor(color)
    g2.SetMarkerStyle(21)
    g2.SetLineWidth(2)
    g2.GetXaxis().CenterTitle()
    g2.GetXaxis().SetTitle('iteration')
    g2.SetTitle('number of real-valued #rho_{k}\'s')

    toutf = TFile('output/results_tol_{}.root'.format(tol), 'recreate')
    g1.Write('avg_rho')
    g2.Write('n_real_rho')