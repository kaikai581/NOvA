#!/usr/bin/env python

from __future__ import print_function

import os
import subprocess

syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']
flists = [syst_name+'.txt' for syst_name in syst_names]

def process_dataset(ds_idx):

    flist = flists[ds_idx]
    fns = []
    with open(flist, 'r') as inf:
        for line in inf:
            fns.append(line.rstrip('\n'))
    
    # create output directory
    out_base = '/pnfs/nova/scratch/users/slin'
    out_path = os.path.join(out_base,'numuccinc','evt_matched_cafs',syst_names[ds_idx])
    if not os.path.exists(out_path):
        os.makedirs(out_path)

    for fn in fns:
        bashout = subprocess.check_output('samweb locate-file {}'.format(fn), shell=True)
        bashout = bashout.split('\n')
        for line in bashout:
            if 'dcache' in line:
                location = line.split(':')[1]
        cmd = 'rsync -v {} {}'.format(os.path.join(location, fn), out_path)
        os.system(cmd)
    

if __name__ == '__main__':

    for i in range(len(syst_names)):
        process_dataset(i)