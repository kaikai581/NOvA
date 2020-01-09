#!/usr/bin/env python

from __future__ import print_function
import random

def save2file(flist, outfn):
    sorted_flist = sorted(flist)
    with open(outfn, 'w') as outf:
        for fn in sorted_flist:
            matchstr = fn.split('.')[0]
            for cs in common_str:
                if matchstr in cs:
                    outf.write('{}\n'.format(fn))

file_lists = [
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-func_v1.txt',
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-neg-offset_v1.txt',
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_calib-shift-nd-xyview-pos-offset_v1.txt',
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_ckv-proton-shift-down_v1.txt',
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightdown-calibup_v1.txt',
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_lightmodel-lightup-calibdown_v1.txt',
    'prod_caf_R17-11-14-prod4reco.CVNprong-respin.b_nd_genie_nonswap_fhc_nova_v08_full_v1.txt'
]

ref_fns = []
linecnt = 0
with open(file_lists[6], 'r') as inf:
    for line in inf:
        if linecnt % 500 == 0:
            ref_fns.append(line.rstrip('\n'))
# ref_fns = ref_fns[:110]
ref_fns = random.sample(ref_fns, 110)

common_fns = [[] for i in range(6)]
for i in range(6):
    with open(file_lists[i], 'r') as inf:
        for line in inf:
            matchstr = line.split('.')[0]
            for refline in ref_fns:
                if matchstr in refline:
                    common_fns[i].append(line.rstrip('\n'))

common_strs = []
common_strs.append(set([s.split('.')[0] for s in ref_fns]))
for i in range(len(common_fns)):
    common_strs.append(set([s.split('.')[0] for s in common_fns[i]]))

common_str = common_strs[0] & common_strs[1] & common_strs[2] & common_strs[3] & common_strs[4] & common_strs[5] & common_strs[6]
common_str = list(common_str)[:100]

outfns = ['nominal.txt', 'calibshape.txt', 'calibneg.txt','calibpos.txt','ckv.txt','lightdown.txt','lightup.txt']
save2file(ref_fns, outfns[0])
for i in range(6):
    save2file(common_fns[i], outfns[i+1])