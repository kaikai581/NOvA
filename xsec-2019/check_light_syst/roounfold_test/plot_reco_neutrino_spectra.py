#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import os

# number of files for each datasets for ndrespin:
# 70405 10432 10403 10622 7813 9783 9698
syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']
syst_nfiles = [70405, 10432, 10403, 10622, 7813, 9783, 9698]
# en_names = ['xsec','osc']
input_ds_names = ['roounfold','ndrespin']


def make_plot(hists, histsr, hs, hsr, canv, leg, legr):
    for i in range(len(hists)):
        hists[i].SetLineColor(i+1)
        hists[i].SetMarkerColor(i+1)
        if i == 4: # reset yellow color
            hists[i].SetLineColor(kYellow+1)
            hists[i].SetMarkerColor(kYellow+1)
        hs.Add(hists[i])
        leg.AddEntry(hists[i],syst_names[i],'L')
        if i >= 1:
            histsr.append(hists[i].Clone('histsr{}'.format(i)))
            histsr[i-1].Divide(hists[0])
            hsr.Add(histsr[i-1])
            legr.AddEntry(histsr[i-1],syst_names[i],'L')

    # draw plots

    # make vertical plots
    canv.cd(1)
    hs.Draw('nostack hist')
    leg.SetBorderSize(0)
    leg.Draw()
    canv.cd(2)
    hsr.Draw('nostack')
    hsr.GetHistogram().GetXaxis().SetTitle('Reco Neutrino Energy (GeV)')
    legr.SetBorderSize(0)
    legr.Draw()

if __name__ == '__main__':

    # command line argument parser
    parser = argparse.ArgumentParser()
    # parser.add_argument('-e','--energy',type=int,default=0,choices=range(len(en_names)))
    parser.add_argument('-d','--dataset_index',type=int,default=1)
    args = parser.parse_args()
    # en_idx = args.energy
    ds_idx = args.dataset_index

    ds_name = input_ds_names[ds_idx]

    # create output directory
    out_dir = 'output/plots'
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    
    # open files
    tfiles = [TFile('input/{}_{}.root'.format(ds_name, syst_name)) for syst_name in syst_names]
    trees = [tfile.Get('tr') for tfile in tfiles]
    hxsecs = [TH1F('h{}_xsec'.format(i),'',20,0,5) for i in range(len(syst_names))]
    hxsecsr = []
    hoscs = [TH1F('h{}_osc'.format(i),'',20,0,5) for i in range(len(syst_names))]
    hoscsr = []
    
    # project histograms
    for i in range(len(syst_names)):
        trees[i].Project('h{}_xsec'.format(i),'nue_reco','(weight)*(muke_cut&&muang_cut&&phasespace_cut)')
        trees[i].Project('h{}_osc'.format(i),'nue_osc_reco','(weight)*(muke_cut&&muang_cut&&phasespace_cut)')
    
    # nd respin only has 1/7 nominal stats. Manually scale the nominal to 1/7.
    if ds_name == 'ndrespin':
        for i in range(1,len(syst_nfiles)):
            hxsecs[i].Scale(float(syst_nfiles[0])/float(syst_nfiles[i]))
            hoscs[i].Scale(float(syst_nfiles[0])/float(syst_nfiles[i]))
    
    # use hstack to store histograms
    hs_xsec = THStack('hs_xsec','xsec numu en estimator')
    hsr_xsec = THStack('hsr_xsec','xsec numu en estimator, ratio')
    hs_osc = THStack('hs_osc','osc numu en estimator')
    hsr_osc = THStack('hsr_osc','osc numu en estimator, ratio')

    # make canvas for event counts and ratio plots
    canv_xsec = TCanvas('xsec','xsec',800,800)
    canv_xsec.Divide(1,2)
    canv_osc = TCanvas('osc','osc',800,800)
    canv_osc.Divide(1,2)

    # make legends
    leg_xsec = TLegend(.7,.5,.88,.88)
    leg_xsecr = TLegend(.4,.18,.6,.58)
    leg_osc = TLegend(.7,.5,.88,.88)
    leg_oscr = TLegend(.4,.18,.6,.58)

    # make plots
    make_plot(hxsecs, hxsecsr, hs_xsec, hsr_xsec, canv_xsec, leg_xsec, leg_xsecr)
    make_plot(hoscs, hoscsr, hs_osc, hsr_osc, canv_osc, leg_osc, leg_oscr)

    # save to file
    canv_xsec.SaveAs('output/plots/reco_xsec_enu_spec_{}.pdf'.format(ds_name))
    canv_osc.SaveAs('output/plots/reco_osc_enu_spec_{}.pdf'.format(ds_name))

    raw_input('Press enter to continue.')