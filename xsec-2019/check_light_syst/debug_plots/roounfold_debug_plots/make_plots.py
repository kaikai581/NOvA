#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import os

syst_names = ['cv','lightdown','lightup','ckv','calibneg','calibpos','calibshape']

def plot_1d_background():
    hbkgs3d = [fdata.Get('{}_background_3d'.format(syst_name)) for syst_name in syst_names]
    hbkgs = [h3d.Project3D('z') for h3d in hbkgs3d]

    for i in range(len(hbkgs)):
        hbkgs[i].SetLineColor(i+1)
        hbkgs[i].SetMarkerColor(i+1)
        leg_bkg.AddEntry(hbkgs[i],syst_names[i],'L')
    
    for h in hbkgs:
        hsbkgs.Add(h)
    
    canv_bkg.Divide(2,1)
    canv_bkg.cd(1)

    hsbkgs.Draw('nostack hist')
    hsbkgs.GetHistogram().GetXaxis().SetTitle('Energy (GeV)')
    leg_bkg.SetBorderSize(0)
    leg_bkg.Draw()

    hbkgrs = [hbkgs[i].Clone('hbkgr{}'.format(i)) for i in range(1,len(hbkgs))]
    for i in range(len(hbkgrs)):
        hbkgrs[i].Divide(hbkgs[0])
        hsbkgr.Add(hbkgrs[i])
        leg_bkgr.AddEntry(hbkgrs[i],syst_names[i+1],'L')
    
    canv_bkg.cd(2)
    hsbkgr.Draw('nostack')
    leg_bkgr.SetBorderSize(0)
    leg_bkgr.Draw()

    canv_bkg.SaveAs('output_plots/background.png')
    


if __name__ == '__main__':

    # create output directory
    out_dir = 'output_plots'
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    # background
    hsbkgs = THStack('hsbkg','background')
    hsbkgr = THStack('hsbkgr','background ratios')
    canv_bkg = TCanvas('bkg','bkg',800,350)
    leg_bkg = TLegend(.7,.5,.88,.88)
    leg_bkgr = TLegend(.4,.15,.6,.55)
    fdata = TFile('../../roounfold_test/input/data_background.root')
    plot_1d_background()


    raw_input('Press enter to continue.')