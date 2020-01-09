#!/usr/bin/env python

from ROOT import *
import os

orig_iter = '0'
src_iter = '0-10'
inter_iter = '0-5'
total_iter = '0-5-5'

orig_file = TFile('output/unfolded_spectra_{}.root'.format(src_iter))
unfolded_file = TFile('output/unfolded_spectra_{}.root'.format(total_iter))

orig_spec = orig_file.Get(orig_iter)
orig_spec.SetLineColor(kBlack)
orig_spec.SetMarkerColor(kBlack)
orig_spec.SetLineWidth(2)
dest_spec = orig_file.Get(src_iter)
dest_spec.SetLineColor(kBlue)
dest_spec.SetMarkerColor(kBlue)
dest_spec.SetLineWidth(2)
inter_spec = unfolded_file.Get(inter_iter)
inter_spec.SetLineColor(kRed)
inter_spec.SetMarkerColor(kRed)
inter_spec.SetLineWidth(2)
bootstrapped_spec = unfolded_file.Get(total_iter)
bootstrapped_spec.SetLineColor(kGreen)
bootstrapped_spec.SetMarkerColor(kGreen)
bootstrapped_spec.SetLineWidth(2)
bootstrapped_spec.SetStats(0)
bootstrapped_spec.GetXaxis().SetTitle('E_{#nu} (GeV)')
bootstrapped_spec.GetXaxis().CenterTitle()
bootstrapped_spec.SetTitle('')
bootstrapped_spec.Draw()
orig_spec.Draw('same')
dest_spec.Draw('same')
inter_spec.Draw('same')

leg = TLegend(.6,.6,.88,.88)
leg.AddEntry(orig_spec,orig_iter,'L')
leg.AddEntry(dest_spec,src_iter,'L')
leg.AddEntry(inter_spec,inter_iter,'L')
leg.AddEntry(bootstrapped_spec,total_iter,'L')
leg.SetBorderSize(0)
leg.Draw()

if not os.path.exists('output_plots'):
    os.makedirs('output_plots')
gPad.SaveAs('output_plots/{}.png'.format(total_iter))

# raw_input('Press enter to continue.')