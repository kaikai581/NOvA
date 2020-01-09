#!/usr/bin/env python

from ROOT import *
from array import array
import os

#- define bin boundaries
#- use neutrino energy
enuedges_true = [1./19.*i*5. for i in range(20)]
enuedges_reco = [0.0, 0.25, 0.50, 0.75, 1.0, 1.25, 1.50, 1.75, 2.0, 2.25, 2.50, 2.75, 3.0, 3.25, 3.50, 3.75, 4.0, 4.25, 4.50, 4.75, 5.0]

#- set up reco axes
data_binning = TUnfoldBinning('data_binning')
reco_axes = data_binning.AddBinning('reco_axes')
reco_axes.AddAxis('nue_reco',len(enuedges_reco)-1,array('d',enuedges_reco),True,True)
data_binning.PrintStream(cout)

#- set up true axes
mc_binning = TUnfoldBinning('mc_binning')
true_axes = mc_binning.AddBinning('true_axes')
true_axes.AddAxis('nue_true',len(enuedges_true)-1,array('d',enuedges_true),True,True)
mc_binning.PrintStream(cout)
reco_binning = data_binning.FindNode('reco_axes')
true_binning = mc_binning.FindNode('true_axes')

#- another true histogram with same binning as reco
mc_same_binning = TUnfoldBinning('mc_same_binning')
true_axes_sb = mc_same_binning.AddBinning('true_axes')
true_axes_sb.AddAxis('nue_true_samebin',len(enuedges_reco)-1,array('d',enuedges_reco),True,True)
mc_same_binning.PrintStream(cout)
true_same_binning = mc_same_binning.FindNode('true_axes')

# book and fill fake data spectrum
ffakedata = TFile('tunfold_input/tunfold_fakedata.root')
tfakedata = ffakedata.Get('tr')
# prepare the spectrum container
hreco = data_binning.CreateHistogram('hreco')
htrue = mc_binning.CreateHistogram('htrue')
htrue_same_binning = mc_same_binning.CreateHistogram('htrue_same_binning')
print('Looping over fake data events...')
for entry in tfakedata:
    if entry.truesig == 0: continue
    bin_num = reco_binning.GetGlobalBinNumber(entry.nue_reco)
    hreco.Fill(bin_num, entry.ppfx_weight)
    bin_num = true_binning.GetGlobalBinNumber(entry.nue_true)
    htrue.Fill(bin_num, entry.ppfx_weight*entry.xsec_weight)
    bin_num = true_same_binning.GetGlobalBinNumber(entry.nue_true)
    htrue_same_binning.Fill(bin_num, entry.ppfx_weight*entry.xsec_weight)

# book and fill migration matrix
hmigration = TUnfoldBinning.CreateHistogramOfMigrations(mc_binning, data_binning, 'hmigration')
fmc = TFile('tunfold_input/tunfold_nominal.root')
tmc = fmc.Get('tr')
print('Constructing migration matrix...')
for entry in tmc:
    if entry.truesig == 0: continue
    mc_bin = true_binning.GetGlobalBinNumber(entry.nue_true)
    data_bin = reco_binning.GetGlobalBinNumber(entry.nue_reco)
    hmigration.Fill(mc_bin, data_bin, entry.ppfx_weight*entry.xsec_weight)

# save all to file
tof = TFile('tunfold_input/tunfold_histograms.root','recreate')
data_binning.Write()
mc_binning.Write()
hreco.Write()
htrue.Write()
hmigration.Write()
htrue_same_binning.Write()