#!/usr/bin/bash
# submit_cafana.py --numuccinc -n 1 -r S19-04-15 -o $DROPBOXCAFANA --memory 8000 -i dropbox://spec_from_indep_mc.root -i dropbox://syst_specs_cv.root --lifetime 86400 optimize_iterations.C 100 1
submit_cafana.py --numuccinc -n 1 -r S19-09-12 -o $DROPBOXCAFANA --memory 3000 -i /pnfs/nova/scratch/users/slin/input/fluctuated_smeared.root --lifetime 86400 /nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/mean_squared_error_metric/poisson_varied/unfold_grid.C 0 1 1

# Below is the interactive command.
# time cafe -b -q -nr --numuccinc optimize_iterations.C 100 &>log/iter100.txt