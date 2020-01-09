#!/bin/bash

submit_cafana.py -n 100 -o $DROPBOXCAFANA --user_tarball /pnfs/nova/scratch/users/connorj/numucc_march_rerun.tar.bz2 --numuccinc --memory 3500 --rel S19-01-14 make_spectra_to_unfold.C