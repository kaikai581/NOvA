#!/usr/bin/bash
#submit_cafana.py --numuccinc -n 1 -r S19-04-15 -o $DROPBOXCAFANA --memory 8000 -i /pnfs/nova/scratch/users/slin/input/spec_from_indep_mc.root -i /pnfs/nova/scratch/users/slin/input/syst_specs_cv.root --lifetime 864000 covariance_matrix_cafana.C 900 1
#submit_cafana.py --numuccinc -n 1 -r development -o $DROPBOXCAFANA --memory 8000 -i /pnfs/nova/scratch/users/slin/input/spec_from_indep_mc.root -i /pnfs/nova/scratch/users/slin/input/syst_specs_cv.root --lifetime 864000 covariance_matrix_cafana.C 100 1

# I keep encountering this error:
# Assertion `!IsInitialized(x)' failed.
# That safeguard was added on 3/28/2019. Therefore I will try using an older release.
submit_cafana.py --numuccinc -n 1 -r S19-03-15 -o $DROPBOXCAFANA --memory 6000 -i /pnfs/nova/scratch/users/slin/input/spec_from_indep_mc.root -i /pnfs/nova/scratch/users/slin/input/syst_specs_cv.root --lifetime 864000 covariance_matrix_cafana.C 100 1
# convenient command
# for i in {1..100}; do submit_cafana.py --numuccinc -n 1 -r S19-03-15 -o $DROPBOXCAFANA --memory 6000 -i /pnfs/nova/scratch/users/slin/input/spec_from_indep_mc.root -i /pnfs/nova/scratch/users/slin/input/syst_specs_cv.root --lifetime 864000 /nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/optimization_my_implementation/covariance_matrix_cafana.C $i 1; done
# for i in {1..100}; do submit_cafana.py --numuccinc -n 1 -r S19-03-15 -o $DROPBOXCAFANA --memory 6000 -i /pnfs/nova/scratch/users/slin/input/spec_from_indep_mc.root -i /pnfs/nova/scratch/users/slin/input/syst_specs_cv.root --lifetime 864000 /nova/ana/users/slin/my_git/NOvA/numu/prod4/xsec_2019/optimize_unfolding/optimization_my_implementation/calculate_covariance_matrix.C ${i} 1; done