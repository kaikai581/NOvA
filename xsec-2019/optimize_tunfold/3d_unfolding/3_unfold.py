#!/usr/bin/env python

from ROOT import *


gInterpreter.Declare('''
    const char* make_null_const_char_ptr()
    {
        const char* ret_val = 0;
        return ret_val;
    }
''')


# switch on histogram errors
TH1.SetDefaultSumw2()

# read binning schemes and input histograms
tinf = TFile('tunfold_input/tunfold_histograms.root')
data_binning = tinf.Get('data_binning')
mc_binning = tinf.Get('mc_binning')
hreco = tinf.Get('hreco')
htrue = tinf.Get('htrue')
hmigration = tinf.Get('hmigration')

# unfolding
constraint_mode = TUnfold.kEConstraintArea

# basic choice of regularisation scheme:
#    curvature (second derivative)
reg_mode = TUnfold.kRegModeCurvature

# density flags
density_flags = TUnfoldDensity.kDensityModeBinWidth

# detailed steering for regularisation
REGULARISATION_DISTRIBUTION = ROOT.make_null_const_char_ptr()
REGULARISATION_AXISSTEERING = '*[B]'

# set up matrix of migrations
unfold = TUnfoldDensity(hmigration,TUnfold.kHistMapOutputHoriz,
                        reg_mode,constraint_mode,density_flags,
                        mc_binning,data_binning,
                        REGULARISATION_DISTRIBUTION,
                        REGULARISATION_AXISSTEERING)

# define the input vector (the measured data distribution)
unfold.SetInput(hreco)

# start unfolding
# here, tau is determined by scanning the global correlation coefficients
# nScan = 30
# rhoLogTau = id(TSpline())
# lCurve = id(TGraph())

# for determining tau, scan the correlation coefficients
# correlation coefficients may be probed for all distributions
# or only for selected distributions
# underflow/overflow bins may be included/excluded
# SCAN_DISTRIBUTION = 'signal'
# SCAN_AXISSTEERING = ''

# iBest = unfold.ScanTau(nScan,0.,0.,rhoLogTau,
#                        TUnfoldDensity.kEScanTauRhoMax,
#                        SCAN_DISTRIBUTION,SCAN_AXISSTEERING,
#                        lCurve)