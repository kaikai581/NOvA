# This script takes given root files, invokes the validation framework
# and publish websites.
# This script presupposes that the user defined her/his setup_nova function in
# ~/.bashrc. This can be made configurable if needed.

from ConfigParser import *
import argparse, os

def make_yaml():
  with open('validation_config.yaml','w') as outf:
    outf.write('{\n')
    outf.write('  "validation_name": "'+validation_name+'",\n')
    outf.write('  "histogram_topdirs": {\n')
    for i in range(0, len(root_files)):
      subdir_name = os.path.basename(root_files[i].rstrip('.root'))
      outf.write('    "'+subdir_name+'": "'+root_files[i])
      if i != len(root_files) - 1: outf.write('",\n')
      else: outf.write('"\n')
    outf.write('  },\n')
    outf.write('  "output_dir": "'+output_dir+'"\n')
    outf.write('}\n')

#
# command line argument parsing
#
parser = argparse.ArgumentParser()
parser.add_argument('-c', '--config_file',  type=str, help='configuration file name', default='publish_reco_validation.cfg')
parser.add_argument('--overwrite', dest='overwrite', help='overwrite output directory', action='store_true')
parser.add_argument('--no-overwrite', dest='overwrite', help='not overwrite output directory', action='store_false')
parser.set_defaults(overwrite=True)
parser.add_argument('--cleanup', dest='cleanup', help='remove the yaml file for framwork configuration', action='store_true')
parser.add_argument('--no-cleanup', dest='cleanup', help='keep the yaml file for framwork configuration', action='store_false')
parser.set_defaults(cleanup=True)
args = parser.parse_args()
config_file = args.config_file
overwrite = args.overwrite
cleanup = args.cleanup

#
# read in the configuration file
#
parser = SafeConfigParser()
parser.read(config_file)
root_files = [x.strip() for x in parser.get('cernroot', 'filenames').split(',')]
output_dir = parser.get('output_dir', 'path')
validation_name = parser.get('validation_name', 'name')
# some preparation work...
project_dir = os.path.join(output_dir, validation_name.replace(' ', '\ '))
if overwrite: os.system('rm -rf '+project_dir)
if os.path.exists(os.path.join(output_dir, 'static_content')):
  os.system('rm -rf '+os.path.join(output_dir, 'static_content'))
os.system('mkdir -p '+output_dir)

#
# make the yaml configuration file for the framework to use
#
make_yaml()

#
# Run the validation framewok. Generally the development version will do.
#
cmd = '. ~/.bashrc;setup_nova;sh $SRT_PUBLIC_CONTEXT/Validation/generic/run_validation.sh validation_config.yaml'
cmd += ';cp -r $SRT_PUBLIC_CONTEXT/Validation/generic/static_content '+output_dir
os.system(cmd)
if cleanup: os.system('rm -f validation_config.yaml')
print 'Webpages are published to:'
print output_dir.replace('/nusoft/app/web/htdoc','http://nusoft.fnal.gov')
