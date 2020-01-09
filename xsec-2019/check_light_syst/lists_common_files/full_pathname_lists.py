#!/usr/bin/env python

from __future__ import print_function
import os

syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']
inlists = [os.path.join('../check_entries/file_lists/', syst_name+'.txt') for syst_name in syst_names]
outlists = [syst_name+'.txt' for syst_name in syst_names]

file_folder_base = '/pnfs/nova/scratch/users/slin/numuccinc/evt_matched_cafs'

for i in range(len(syst_names)):
    with open(inlists[i], 'r') as inf:
        with open(outlists[i], 'w') as outf:
            for infline in inf:
                fullpn = os.path.join(file_folder_base, syst_names[i], infline)
                outf.write(fullpn)