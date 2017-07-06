#
# This script is to make easy the grid submission of RecoValidation jobs.
#

from ConfigParser import *
import argparse, os

def make_grid_config_file(dataset):
  working_subdir = os.path.join(working_dir, dataset)
  with open(os.path.join(working_subdir, dataset+'.cfg')) as outf:
    outf.write('--jobname '+defname+'\n')
    outf.write('--defname '+defname+'\n')
    outf.write('-c '+fcl+'\n')
    outf.write('--dest '+dest+'\n')
    if testrel: outf.write('--testrel '+testrel+'\n')
    if tag: outf.write('--tag '+tag+'\n')
    outf.write('--histTier '+histTier+'\n')
    if maxopt: outf.write('--maxopt\n')
    outf.write('-G nova\n')
    outf.write('--njobs '+njobs+'\n')
    outf.write('--files_per_job '+files_per_job+'\n')
    outf.write('--opportunistic')
    outf.write('--copyOut')
    outf.write('--print_jobsub')
    outf.write('--expected_lifetime '+expected_lifetime+'\n')

def make_fhicl_file(dataset):
  working_subdir = os.path.join(working_dir, dataset)
  # copy a template from development
  os.system('. ~/.bashrc;setup_nova;cp $SRT_PUBLIC_CONTEXT/RecoValidation/prod_reco_validation_job.fcl '+working_subdir)
  inflines = []
  with open(os.path.join(working_subdir, 'prod_reco_validation_job.fcl')) as inf:
    for line in inf: inflines.append(line)
  with open(os.path.join(working_subdir, dataset+'.fcl')) as outf:
    for line in inflines:
      for module in complete_module_list:
        if module in line:
          if not module in enabled_modules:
            line = '#'+line
    outf.write(line)

def record_output_path(datset):
  written_paths = []
  with open(os.path.join(working_dir, 'hadd_reco_validation.cfg')) as inf:
    for line in outf:
      if line.strip('\n') not in written_path: written_paths.append(line.strip('\n'))
  if dest.strip('\n') not in written_paths: written_paths.append(dest.strip('\n'))
  with open(os.path.join(working_dir, 'hadd_reco_validation.cfg')) as outf:
    for line in written_paths: outf.write(line+'\n')

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
# enabled art modules
enabled_modules = [x.strip() for x in parser.get('art modules', 'enabled').split(',')]
complete_module_list = [
                         calhitana,
                         slicerana,
                         multihoughana,
                         elasticarmsana,
                         houghvertexana,
                         fuzzykana,
                         kalmantrackana,
                         breakpointana,
                         bpfenergyana,
                         cosmictrackana,
                         windowtrackana
                       ]

#
# create the working directory
#
os.system('mkdir -p '+working_dir)
# use the sam defnames as the folder names
for dataset in sam_datasets:
  os.system('mkdir -p ' + os.path.join(working_dir, dataset))
  make_grid_config_file(dataset)
  make_fhicl_file(dataset)
  os.system('. ~/.bashrc;setup_nova;submit_nova_art.py -f '+os.path.join(working_dir, dataset, dataset+'.cfg'))
  record_output_path(dataset)
