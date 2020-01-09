#!/usr/bin/env python
"""
Group root output from grid.
"""

from __future__ import print_function
import ROOT
import os

def flatten_3d(h3d, hres):
    nx = h3d.GetNbinsX()
    ny = h3d.GetNbinsY()
    nz = h3d.GetNbinsZ()

    ibin = 1
    for i in range(1, nx+1):
        for j in range(1, ny+1):
            for k in range(1, nz+1):
                hres.SetBinContent(ibin, h3d.GetBinContent(i, j, k))
                ibin += 1
    return hres


if __name__ == '__main__':
    import argparse
    import glob

    parser = argparse.ArgumentParser('Command line argument parser.')
    parser.add_argument('-i', '--niter', type=int, default=1)
    args = parser.parse_args()
    niter = args.niter
    
    # query all available fluctuation files
    file_path = '/pnfs/nova/users/slin/unfolding/numuccinc_roounfoldbayes'
    file_str = os.path.join(file_path, 'smear*_iter_{}.root'.format(niter))
    experiment_ids = sorted([int(os.path.basename(x).split('_')[0].lstrip('smeared')) for x in glob.glob(file_str)])
    fpathnames = [os.path.join(file_path, 'smeared{}_iter_{}.root'.format(x, niter)) for x in experiment_ids]
    # including original spectra
    orig_fname = '/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/mean_squared_error_metric/poisson_varied/processed_input/fluctuated_smeared.root'

    # original file and fluctuated files
    orig_file = ROOT.TFile(orig_fname)
    fluc_files = [ROOT.TFile(fname) for fname in fpathnames]
    
    # load all histograms
    htrue_nom = orig_file.Get('htrue')
    hreco_nom = orig_file.Get('hsmeared')
    htrues = []
    hrecos = []
    hunfoldeds = []
    for i in range(len(fluc_files)):
        htrues.append(orig_file.Get('fluctuated/hfluc{}'.format(i)))
        hrecos.append(orig_file.Get('smeared/hsmeared{}'.format(i)))
        # hrecos.append(fluc_files[i].Get('hreco'))
        hunfoldeds.append(fluc_files[i].Get('hunfolded'))
    
    # include also connor's unfolded histogram
    tinf_connor = ROOT.TFile('/nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/mean_squared_error_metric/single_difference_bias/input_files_by_connor/rooUnfold{}iter.root'.format(niter))
    hflattened = flatten_3d(tinf_connor.Get('cv/EAvail/UnfoldedFakeData'), htrue_nom.Clone('hunfolded_connor'))
    hreco_connor = tinf_connor.Get('cv/EAvail/FakeDataTimesPur')
    
    # save to file
    outpath = 'processed_grid_output'
    if not os.path.exists(outpath):
        os.makedirs(outpath)
    toutfn = os.path.join(outpath, 'iter{}.root'.format(niter))
    toutf = ROOT.TFile(toutfn, 'recreate')
    htrue_nom.SetLineColor(ROOT.kRed)
    htrue_nom.SetMarkerColor(ROOT.kRed)
    htrue_nom.Write('htrue_nom')
    hreco_nom.SetLineColor(ROOT.kBlue)
    hreco_nom.SetMarkerColor(ROOT.kBlue)
    hreco_nom.Write('hreco_nom')
    hreco_connor.Write('hreco_connor')
    hflattened.Write()
    toutf.mkdir('true_hists')
    toutf.cd('true_hists')
    for i in range(len(htrues)):
        htrues[i].Write('htrue_{}'.format(i))
    toutf.cd()
    toutf.mkdir('reco_hists')
    toutf.cd('reco_hists')
    for i in range(len(hrecos)):
        hrecos[i].Write('hreco_{}'.format(i))
    toutf.cd()
    toutf.mkdir('unfolded_hists')
    toutf.cd('unfolded_hists')
    for i in range(len(hunfoldeds)):
        hunfoldeds[i].Write('hunfolded_{}'.format(i))