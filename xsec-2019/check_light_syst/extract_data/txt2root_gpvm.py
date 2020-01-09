#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse

infns = [
    'unfolding_study_calibneg.txt', 'unfolding_study_calibshape.txt', 'unfolding_study_lightdown.txt', 'unfolding_study_nominal.txt',
    'unfolding_study_calibpos.txt', 'unfolding_study_ckv.txt', 'unfolding_study_lightup.txt'
]

outfns = [infn.rstrip('.txt')+'.root' for infn in infns]

# determine which file to run through from command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('-f','--file_index',type=int,default=0,choices=range(len(infns)))
args = parser.parse_args()
fidx = args.file_index

infn = infns[fidx]
outfn = outfns[fidx]

outf = TFile(outfn, 'recreate')
tr = TTree('tr','tree with reco and true values')
tr.ReadFile(infn, 'muang_reco/F:muke_reco/F:nue_reco/F:hade_reco/F:muang_true/F:muke_true/F:nue_true/F:hade_true/F:weight/F:quality/b:contain/b:muid/b:fiducial/b:muke/b:muang/b:phasespace/b:truesig/b', ' ')
outf.Write()