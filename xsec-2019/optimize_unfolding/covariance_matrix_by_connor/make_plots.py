#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import os

if __name__ == '__main__':

    from glob import glob
    tols = sorted([x.lstrip('output/results_tol_').rstrip('.root') for x in glob('output/results_tol_*.root')])

    tinfs = []
    grhos = []
    grealrhos = []
    for tol in tols:
        tinfs.append(TFile('output/results_tol_{}.root'.format(tol)))
    
    # make output directory if not exist
    ofp = 'resulting_plots'
    if not os.path.exists(ofp):
        os.makedirs(ofp)

    c1 = TCanvas('c1')
    h1 = TH1F('h1','',21,0,21)
    h1.SetMinimum(.5)
    h1.SetMaximum(1)
    h1.SetStats(0)
    h1.GetXaxis().SetTitle('number of iterations')
    h1.GetXaxis().CenterTitle()
    h1.GetYaxis().CenterTitle()
    h1.GetYaxis().SetTitle('#rho_{avg}')
    h1.Draw()
    leg1 = TLegend(.13,.13,.4,.5)
    leg1.SetBorderSize(0)
    for i in range(len(tinfs)):
        grhos.append(tinfs[i].Get('avg_rho'))
        grhos[i].Draw('pl')
        leg1.AddEntry(grhos[i],tols[i],'pl')
    leg1.Draw()
    c1.SaveAs(os.path.join(ofp, 'avg_rho.eps'))

    c2 = TCanvas('c2')
    h2 = TH1F('h2','',21,0,21)
    h2.SetMinimum(1630)
    h2.SetMaximum(2500)
    h2.SetStats(0)
    h2.GetXaxis().SetTitle('number of iterations')
    h2.GetXaxis().CenterTitle()
    h2.GetYaxis().CenterTitle()
    h2.GetYaxis().SetTitle('number of real-valued #rho_{k}')
    h2.Draw()
    leg2 = TLegend(.6,.13,.87,.4)
    leg2.SetBorderSize(0)
    for i in range(len(tinfs)):
        grealrhos.append(tinfs[i].Get('n_real_rho'))
        grealrhos[i].Draw('pl')
        leg2.AddEntry(grealrhos[i],tols[i],'pl')
    leg2.Draw()
    c2.SaveAs(os.path.join(ofp, 'n_real_rho.pdf'))