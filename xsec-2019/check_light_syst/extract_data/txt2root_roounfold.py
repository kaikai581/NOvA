#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import argparse
import glob
import os

syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']

src_dir = '/pnfs/nova/scratch/users/slin/cafana_on_grid_output/dropbox'

outfns = ['roounfold_'+syst_name+'.root' for syst_name in syst_names]

# determine which file to run through from command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('-f','--file_index',type=int,default=0,choices=range(len(syst_names)))
args = parser.parse_args()
fidx = args.file_index

systname = syst_names[fidx]
outfn = outfns[fidx]

# make sure output directory exists
if not os.path.exists('output_root_files'):
    os.makedirs('output_root_files')

outf = TFile(os.path.join('output_root_files',outfn), 'recreate')
tr = TTree('tr','tree with reco and true values')
matchstr = os.path.join(src_dir, 'roounfold_study_'+systname+'*.txt')

# collect all files and loop through
for infn in glob.glob(matchstr):
    tr.ReadFile(infn, 'muang_reco/F:muke_reco/F:nue_reco/F:nue_osc_reco/F:hade_reco/F:muang_true/F:muke_true/F:nue_true/F:hade_true/F:weight/F:muke_cut/b:muang_cut/b:phasespace_cut/b:truesig/b', ' ')
outf.Write()