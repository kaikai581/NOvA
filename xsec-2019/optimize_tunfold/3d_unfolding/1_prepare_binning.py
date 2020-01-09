#!/usr/bin/env python

from ROOT import *
from array import array
import os

#- define bin boundaries
angedges = [-1.0, 0.5, 0.56, 0.62, 0.68, 0.74, 0.80, 0.85, 0.88, 0.91, 0.94, 0.96, 0.98, 0.99, 1.0]
mukeedges =  [-10,.5,.6,.7,.8,.9,1,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,120]
eavailedges = [0.0, 0.25, 0.50, 0.75, 1.0, 1.25, 1.50, 1.75, 2.0, 2.25, 2.50, 120]

#- set up reco axes
data_binning = TUnfoldBinning('data_binning')
reco_axes = data_binning.AddBinning('reco_axes')
reco_axes.AddAxis('muang',len(angedges)-1,array('d',angedges),False,False)
reco_axes.AddAxis('muke',len(mukeedges)-1,array('d',mukeedges),False,False)
reco_axes.AddAxis('hadeavail',len(eavailedges)-1,array('d',eavailedges),False,False)
#- There is this ROOT.cout pythonized!!
data_binning.PrintStream(cout)

#- set up true axes
mc_binning = TUnfoldBinning('mc_binning')
true_axes = mc_binning.AddBinning('true_axes')
true_axes.AddAxis('muang_true',len(angedges)-1,array('d',angedges),False,False)
true_axes.AddAxis('muke_true',len(mukeedges)-1,array('d',mukeedges),False,False)
true_axes.AddAxis('hadeavail_true',len(eavailedges)-1,array('d',eavailedges),False,False)
#- set up background axes
# bkg_axes = mc_binning.AddBinning('bkg_axes')
# bkg_axes.AddAxis('muang_bkg',len(angedges)-1,array('d',angedges),False,False)
# bkg_axes.AddAxis('muke_bkg',len(mukeedges)-1,array('d',mukeedges),False,False)
# bkg_axes.AddAxis('hadeavail_bkg',len(eavailedges)-1,array('d',eavailedges),False,False)
mc_binning.PrintStream(cout)

#- save to file
if not os.path.exists('tunfold_input'):
    os.makedirs('tunfold_input')
outf = TFile('tunfold_input/tunfold_binning.root', 'recreate')
data_binning.Write()
mc_binning.Write()