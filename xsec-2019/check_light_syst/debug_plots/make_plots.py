#!/usr/bin/env python

from __future__ import print_function

from ROOT import *
import os

def compare_up_down(category):
    i0 = gidx[category][0]
    i1 = gidx[category][1]
    i2 = gidx[category][2]
    sname1 = syst_names[i0]
    sname2 = syst_names[i1]
    sname3 = syst_names[i2]
    hsig1 = hall[sname1]['_est_sig']
    hsig2 = hall[sname2]['_est_sig']
    hsig2.SetLineColor(kRed)
    hsig3 = hall[sname3]['_est_sig']
    hsig3.SetLineColor(kBlue)

    hssig[category].Add(hsig1)
    hssig[category].Add(hsig2)
    hssig[category].Add(hsig3)

    # make canvas
    canvs[category]['_est_sig'] = TCanvas()

    hssig[category].Draw('nostack hist')
    h = hssig[category].GetHistogram()
    h.GetXaxis().CenterTitle()
    h.GetXaxis().SetTitle('Reco Neutrino Energy (GeV)')

    # setup legend
    legsig[category].AddEntry(hsig3, syst_names[i2]+' '+str(int(hsig3.Integral())), 'l')
    legsig[category].AddEntry(hsig1, syst_names[i0]+' '+str(int(hsig1.Integral())), 'l')
    legsig[category].AddEntry(hsig2, syst_names[i1]+' '+str(int(hsig2.Integral())), 'l')
    legsig[category].SetBorderSize(0)
    legsig[category].Draw()

    # unfolded spectra
    hunf1 = hall[sname1]['_unfolded_sig']
    hunf2 = hall[sname2]['_unfolded_sig']
    hunf2.SetLineColor(kRed)
    hunf3 = hall[sname3]['_unfolded_sig']
    hunf3.SetLineColor(kBlue)

    hsunf[category].Add(hunf1)
    hsunf[category].Add(hunf2)
    hsunf[category].Add(hunf3)

    # make canvas
    canvs[category]['_unfolded_sig'] = TCanvas()

    hsunf[category].Draw('nostack hist')
    hu = hsunf[category].GetHistogram()
    hu.GetXaxis().CenterTitle()
    hu.GetXaxis().SetTitle('True Neutrino Energy (GeV)')

    # setup legend
    legunf[category].AddEntry(hunf3, syst_names[i2]+' '+str(int(hunf3.Integral())), 'l')
    legunf[category].AddEntry(hunf1, syst_names[i0]+' '+str(int(hunf1.Integral())), 'l')
    legunf[category].AddEntry(hunf2, syst_names[i1]+' '+str(int(hunf2.Integral())), 'l')
    legunf[category].SetBorderSize(0)
    legunf[category].Draw()

    canvs_sbs[category].Divide(2,1)
    canvs_sbs[category].cd(1)
    hssig[category].Draw('nostack hist')
    legsig[category].Draw()
    gPad.Update()
    canvs_sbs[category].cd(2)
    hsunf[category].Draw('nostack hist')
    legunf[category].Draw()
    gPad.Update()
    canvs_sbs[category].SaveAs('output_plots/{}.pdf'.format(category))


def compare_single_shift(category):
    i0 = gidx[category][0]
    i1 = gidx[category][1]
    sname1 = syst_names[i0]
    sname2 = syst_names[i1]

    # reco spectra
    hsig1 = hall[sname1]['_est_sig']
    hsig2 = hall[sname2]['_est_sig']
    hsig2.SetLineColor(kRed)

    hssig[category].Add(hsig1)
    hssig[category].Add(hsig2)

    # make canvas
    canvs[category]['_est_sig'] = TCanvas()

    hssig[category].Draw('nostack hist')
    h = hssig[category].GetHistogram()
    h.GetXaxis().CenterTitle()
    h.GetXaxis().SetTitle('Reco Neutrino Energy (GeV)')

    # setup legend
    legsig[category].AddEntry(hsig1, syst_names[i0]+' '+str(int(hsig1.Integral())), 'l')
    legsig[category].AddEntry(hsig2, syst_names[i1]+' '+str(int(hsig2.Integral())), 'l')
    legsig[category].SetBorderSize(0)
    legsig[category].Draw()

    # unfolded spectra
    hunf1 = hall[sname1]['_unfolded_sig']
    hunf2 = hall[sname2]['_unfolded_sig']
    hunf2.SetLineColor(kRed)

    hsunf[category].Add(hunf1)
    hsunf[category].Add(hunf2)

    # make canvas
    canvs[category]['_unfolded_sig'] = TCanvas()

    hsunf[category].Draw('nostack hist')
    hu = hsunf[category].GetHistogram()
    hu.GetXaxis().CenterTitle()
    hu.GetXaxis().SetTitle('True Neutrino Energy (GeV)')

    # setup legend
    legunf[category].AddEntry(hunf1, syst_names[i0]+' '+str(int(hunf1.Integral())), 'l')
    legunf[category].AddEntry(hunf2, syst_names[i1]+' '+str(int(hunf2.Integral())), 'l')
    legunf[category].SetBorderSize(0)
    legunf[category].Draw()

    canvs_sbs[category].Divide(2,1)
    canvs_sbs[category].cd(1)
    hssig[category].Draw('nostack hist')
    legsig[category].Draw()
    gPad.Update()
    canvs_sbs[category].cd(2)
    hsunf[category].Draw('nostack hist')
    legunf[category].Draw()
    gPad.Update()
    canvs_sbs[category].SaveAs('output_plots/{}.pdf'.format(category))


tinf = TFile('hack.root')
syst_names = ['cv', 'lightdown', 'lightup', 'ckv', 'calibneg', 'calibpos', 'calibshape']
hstage = ['_est_sig', '_unfolded_sig']

hall = dict()
hssig = dict()
hssig['light'] = THStack('hs','estimated signal, light')
hssig['ckv'] = THStack('hs','estimated signal, Cherenkov')
hssig['calib'] = THStack('hs','estimated signal, calibration')
hssig['calibshape'] = THStack('hs','estimated signal, calibration shape')
legsig = dict()
legsig['light'] = TLegend(.6,.88,.6,.88)
legsig['ckv'] = TLegend(.6,.88,.6,.88)
legsig['calib'] = TLegend(.6,.88,.6,.88)
legsig['calibshape'] = TLegend(.6,.88,.6,.88)
hsunf = dict()
hsunf['light'] = THStack('hs','unfolded, light')
hsunf['ckv'] = THStack('hs','unfolded, Cherenkov')
hsunf['calib'] = THStack('hs','unfolded, calibration')
hsunf['calibshape'] = THStack('hs','unfolded, calibration shape')
legunf = dict()
legunf['light'] = TLegend(.6,.88,.6,.88)
legunf['ckv'] = TLegend(.6,.88,.6,.88)
legunf['calib'] = TLegend(.6,.88,.6,.88)
legunf['calibshape'] = TLegend(.6,.88,.6,.88)
gidx = dict()
gidx['light'] = (0,1,2)
gidx['ckv'] = (0,3)
gidx['calib'] = (0,4,5)
gidx['calibshape'] = (0,6)

canvs = dict()
canvs_sbs = dict()
for name in ['light', 'ckv', 'calib', 'calibshape']:
    canvs[name] = dict()
    canvs_sbs[name] = TCanvas(name,name,1000,500)

for syst_name in syst_names:
    hall[syst_name] = dict()
    for stage in hstage:
        hall[syst_name][stage] = tinf.Get('debug/'+syst_name+stage)
        hall[syst_name][stage].SetLineWidth(2)

# make sure output directory exists
if not os.path.exists('output_plots'):
    os.makedirs('output_plots')

compare_up_down('light')
compare_up_down('calib')
compare_single_shift('ckv')
compare_single_shift('calibshape')

raw_input('Press enter to continue.')