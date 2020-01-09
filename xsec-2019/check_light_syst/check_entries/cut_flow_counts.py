#!/usr/bin/env python

from __future__ import print_function
from ROOT import *
import os

def do_one_syst(i):

    print('systematics {} starts...'.format(i))

    syst_name = syst_names[i]
    funw.cd()
    funwsyst = funw.mkdir(syst_name)
    inf = TFile(infns[i])
    tr = inf.Get('tr')
    for j in range(len(cut_flow)):
        hname = 'hunw_cut{}'.format(j)
        h = TH1F(hname, '', 20,0,5)
        tr.Project(hname,'nue_reco',cut_flow[j])
        funwsyst.cd()
        h.Write()

    fwei.cd()
    fweisyst = fwei.mkdir(syst_name)
    for j in range(len(cut_flow)):

        print('cut {} starts...'.format(j))
        hname = 'hwei_cut{}'.format(j)
        h = TH1F(hname, '', 20,0,5)
        tr.Draw('nue_reco>>{}'.format(hname),'(weight)*({})'.format(cut_flow[j]))
        fweisyst.cd()
        h.Write()
    
    print('systematics {} finished'.format(i))

    



syst_names = ['nominal','lightdown','lightup','ckv','calibneg','calibpos','calibshape']

infns = ['unfolding_study_{}.root'.format(syst_names[i]) for i in range(len(syst_names))]

infns = [os.path.join('../extract_data', infn) for infn in infns]

cuts = [
    'quality==1', 'contain==1', 'muid==1', 'fiducial==1', 'muke==1', 'muang==1', 'phasespace==1'
]

cut_flow = []

# make cut flow
for i in range(len(cuts)):
    tot_cuts = cuts[0]
    for j in range(1,i+1):
        tot_cuts += '&&'
        tot_cuts += cuts[j]
    cut_flow.append(tot_cuts)

outf = TFile('cut_flow.root', 'recreate')
funw = outf.mkdir('unweighted')
fwei = outf.mkdir('weighted')
for i in range(len(syst_names)):
    do_one_syst(i)