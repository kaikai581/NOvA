#
# This script is to make easy hadding the grid output of the RecoValidation jobs.
# This script uses the same configuration file as the submit_reco_validation.py script.
#

from ConfigParser import *
import argparse, os

#
# command line argument parsing
#
parser = argparse.ArgumentParser()
parser.add_argument('-c', '--config_file',  type=str, help='configuration file name', default='submit_reco_validation.cfg')
args = parser.parse_args()
config_file = args.config_file

#
# read in the configuration file
#
parser = SafeConfigParser()
parser.read(config_file)
sam_datasets = [x.strip() for x in parser.get('sam', 'defnames').split(',')]
nfiles = parser.get('sam', 'number_of_files')
working_dir = parser.get('working folder', 'path')
# grid config variables
jobname = parser.get('grid configuration', 'jobname')
fcl = parser.get('grid configuration', 'c')
dest = parser.get('grid configuration', 'dest')
testrel = parser.get('grid configuration', 'testrel')
tag = parser.get('grid configuration', 'tag')
histTier = parser.get('grid configuration', 'histTier')
maxopt = parser.get('grid configuration', 'maxopt')
group = parser.get('grid configuration', 'G')
njobs = parser.get('grid configuration', 'njobs')
files_per_job = parser.get('grid configuration', 'files_per_job')
opportunistic = parser.get('grid configuration', 'opportunistic')
copyOut = parser.get('grid configuration', 'copyOut')
print_jobsub = parser.get('grid configuration', 'print_jobsub')
expected_lifetime = parser.get('grid configuration', 'expected_lifetime')

#
# Copy files to the working directory and hadd them.
#
for defname in sam_datasets:
  grid_copy_dir = os.path.join(working_dir, defname, 'hadded_output', 'grid_copy')
  os.system('mkdir -p ' + grid_copy_dir)
  os.system('cp ' + os.path.join(dest, '*.root') + ' ' + grid_copy_dir)
  os.system('. ~/.bashrc;setup_nova;hadd -T -f -k ' + os.path.join(grid_copy_dir,'../hadded_output.root') + ' ' + os.path.join(grid_copy_dir,'*.root'))
