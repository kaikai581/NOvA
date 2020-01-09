#!/usr/bin/env python

from ROOT import *
import os

# switch on histogram errors
TH1.SetDefaultSumw2()

# load prepared binning schemes
fbinning = TFile('tunfold_input/tunfold_binning.root')
data_binning = fbinning.Get('data_binning')
mc_binning = fbinning.Get('mc_binning')
reco_binning = data_binning.FindNode('reco_axes')
true_binning = mc_binning.FindNode('true_axes')

# book and fill fake data spectrum
ffakedata = TFile('tunfold_input/tunfold_fakedata.root')
tfakedata = ffakedata.Get('tr')
# prepare the spectrum container
hreco = data_binning.CreateHistogram('hreco')
htrue = mc_binning.CreateHistogram('htrue')
print('Looping over fake data events...')
for entry in tfakedata:
    if entry.truesig == 0: continue
    bin_num = reco_binning.GetGlobalBinNumber(entry.muang_reco, entry.muke_reco, entry.hadeavail_reco)
    hreco.Fill(bin_num, entry.ppfx_weight)
    bin_num = true_binning.GetGlobalBinNumber(entry.muang_true, entry.muke_true, entry.hadeavail_true)
    htrue.Fill(bin_num, entry.ppfx_weight*entry.xsec_weight)

# book and fill migration matrix
hmigration = TUnfoldBinning.CreateHistogramOfMigrations(mc_binning, data_binning, 'hmigration')
fmc = TFile('tunfold_input/tunfold_nominal.root')
tmc = fmc.Get('tr')
print('Constructing migration matrix...')
for entry in tmc:
    if entry.truesig == 0: continue
    mc_bin = true_binning.GetGlobalBinNumber(entry.muang_true, entry.muke_true, entry.hadeavail_true)
    data_bin = reco_binning.GetGlobalBinNumber(entry.muang_reco, entry.muke_reco, entry.hadeavail_reco)
    hmigration.Fill(mc_bin, data_bin, entry.ppfx_weight*entry.xsec_weight)

# save all to file
tof = TFile('tunfold_input/tunfold_histograms.root','recreate')
data_binning.Write()
mc_binning.Write()
hreco.Write()
htrue.Write()
hmigration.Write()