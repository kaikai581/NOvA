#!/usr/bin/env python

from ROOT import *

# load data to unfold
tinf = TFile('input_files/recasted_input.root')
rresponse = tinf.Get('rresponse')
hdata = tinf.Get('hdata')

# unfold!
unfold = RooUnfoldSvd(rresponse, hdata, 3000)
hunfold = unfold.Hreco()

# save to file
toutf = TFile('kreg3000.root', 'recreate')
unfold.Write('roounfoldsvd')
hunfold.Write('hunfold')