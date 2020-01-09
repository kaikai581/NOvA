#!/usr/bin/env python

from ROOT import *

# load data to unfold
tinf = TFile('input_files/recasted_input.root')
rresponse = tinf.Get('rresponse')
hdata = tinf.Get('hdata')

# unfold!
unfold = RooUnfoldTUnfold(rresponse, hdata, 10)
hunfold = unfold.Hreco()

# save to file
toutf = TFile('tau10.root', 'recreate')
unfold.Write('roounfoldtunfold')
hunfold.Write('hunfold')