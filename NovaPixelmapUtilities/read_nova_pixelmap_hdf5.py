#!/usr/bin/env python

from PIL import Image
import argparse
import h5py
import io
import itertools
import matplotlib.pyplot as plt
import numpy as np
import sys

parser = argparse.ArgumentParser()
parser.add_argument('-f','--filename', default='training_data.h5', help='Name of the file to open')
parser.add_argument('-n','--entry', type=int, default=0, help='Entry number')
parser.add_argument('-c','--channel', type=int, default=0, help='Channel number')
args = parser.parse_args()

# Open an existing file using default properties.
inf = h5py.File(args.filename, 'r')
entry = args.entry
ch = args.channel

dataset = inf['/data']

print('Reading data...')
data_read = dataset[...]
print('Printing data...')

H = np.array(data_read[entry][ch])
fig = plt.figure(figsize=(6, 3.2))
ax = fig.add_subplot(111)
ax.set_title('colorMap')
plt.imshow(H)
ax.set_aspect('equal')

cax = fig.add_axes([0.12, 0.1, 0.78, 0.8])
cax.get_xaxis().set_visible(False)
cax.get_yaxis().set_visible(False)
cax.patch.set_alpha(0)
cax.set_frame_on(False)
plt.colorbar(orientation='vertical')
plt.show()

inf.close()
