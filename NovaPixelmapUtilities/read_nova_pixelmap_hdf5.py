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
label = inf['/label']

predefined_labes = []
predefined_labes.append('kNumuQE')
predefined_labes.append('kNumubarQE')
predefined_labes.append('kNumuRes')
predefined_labes.append('kNumubarRes')
predefined_labes.append('kNumuDIS')
predefined_labes.append('kNumubarDIS')
predefined_labes.append('kNumuOther')
predefined_labes.append('kNumubarOther')
predefined_labes.append('kNueQE')
predefined_labes.append('kNuebarQE')
predefined_labes.append('kNueRes')
predefined_labes.append('kNuebarRes')
predefined_labes.append('kNueDIS')
predefined_labes.append('kNuebarDIS')
predefined_labes.append('kNueOther')
predefined_labes.append('kNuebarOther')
predefined_labes.append('kNutauQE')
predefined_labes.append('kNutaubarQE')
predefined_labes.append('kNutauRes')
predefined_labes.append('kNutaubarRes')
predefined_labes.append('kNutauDIS')
predefined_labes.append('kNutaubarDIS')
predefined_labes.append('kNutauOther')
predefined_labes.append('kNutaubarOther')
predefined_labes.append('kNuElectronElastic')
predefined_labes.append('kNC')
predefined_labes.append('kCosmic')
predefined_labes.append('kOther')
predefined_labes.append('kNIntType')

print('Reading data...')
data_read = dataset[...]
label_read= label[...]
print('Printing data...')

H = np.array(data_read[entry][ch])
fig = plt.figure(figsize=(6, 3.2))
ax = fig.add_subplot(111)
ax.set_title(predefined_labes[label_read[entry][0][0][0]])
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
