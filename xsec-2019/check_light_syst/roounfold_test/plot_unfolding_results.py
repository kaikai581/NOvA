#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import os

syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']
alg_names = ['bayes','svd']
en_names = ['xsec','osc']
input_ds_names = ['roounfold','ndrespin']


if __name__ == '__main__':

    # command line argument parser
    parser = argparse.ArgumentParser()
    parser.add_argument('-a','--algorithm',type=int,default=0,choices=range(len(alg_names)))
    parser.add_argument('-e','--energy',type=int,default=0,choices=range(len(en_names)))
    parser.add_argument('-d','--dataset_index',type=int,default=1)
    args = parser.parse_args()
    alg_idx = args.algorithm
    en_idx = args.energy
    ds_idx = args.dataset_index

    # create output directory
    out_dir = 'output/plots'
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    # get histograms
    inf = TFile('output/{}/unfolded_spectra_{}.root'.format(input_ds_names[ds_idx], alg_names[alg_idx]))
    hunf = [inf.Get('unfolded_{}_{}'.format(syst_name, en_names[en_idx])) for syst_name in syst_names]
    hunfr = []

    # make canvas for event counts and ratio plots
    canv_unf = TCanvas('unf','unf',1000,450)
    canv_unf.Divide(2,1)

    # make legends
    leg_unf = TLegend(.7,.5,.88,.88)
    leg_unfr = TLegend(.4,.48,.6,.88)

    # use hstack to store histograms
    hs = THStack('hs','{} numu en estimator, {}'.format(en_names[en_idx], alg_names[alg_idx]))
    hsr = THStack('hsr','{} numu en estimator, {}, ratio'.format(en_names[en_idx], alg_names[alg_idx]))

    # fill the stacked histograms
    for i in range(len(hunf)):
        hunf[i].SetLineColor(i+1)
        hunf[i].SetMarkerColor(i+1)
        if i == 4: # reset yellow color
            hunf[i].SetLineColor(kYellow+1)
            hunf[i].SetMarkerColor(kYellow+1)
        hs.Add(hunf[i])
        leg_unf.AddEntry(hunf[i],syst_names[i],'L')
        if i >= 1:
            hunfr.append(hunf[i].Clone('hunfr{}'.format(i)))
            hunfr[i-1].Divide(hunf[0])
            hsr.Add(hunfr[i-1])
            leg_unfr.AddEntry(hunfr[i-1],syst_names[i],'L')

    # draw plots
    canv_unf.cd(1)
    hs.Draw('nostack hist')
    hs.GetHistogram().GetXaxis().SetTitle('Neutrino Energy (GeV)')
    leg_unf.SetBorderSize(0)
    leg_unf.Draw()
    canv_unf.cd(2)
    hsr.SetMaximum(3.5)
    hsr.Draw('nostack')
    hsr.GetHistogram().GetXaxis().SetTitle('Neutrino Energy (GeV)')
    leg_unfr.SetBorderSize(0)
    leg_unfr.Draw()

    canv_unf.SaveAs('output/plots/{}_{}.png'.format(en_names[en_idx], alg_names[alg_idx]))

    # make vertical plots
    canv_unf_vert = TCanvas('unf','unf',800,800)
    canv_unf_vert.Divide(1,2)
    canv_unf_vert.cd(1)
    hs.Draw('nostack hist')
    leg_unf.Draw()
    canv_unf_vert.cd(2)
    hsr.Draw('nostack')
    leg_unfr.Draw()

    canv_unf_vert.SaveAs('output/plots/{}_{}_vert.pdf'.format(en_names[en_idx], alg_names[alg_idx]))

    raw_input('Press enter to continue.')