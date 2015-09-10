import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from math import *

# the independent variable used here is in degree

# limiting condition with E_nu = 0
@np.vectorize
def Q20(x):
  return 0

# limiting condition with E_nu = infinity
@np.vectorize
def Q2inf(x):
  return 2*cos(x)*cos(x)/sin(x)/sin(x)

fig1 = plt.figure()
fig1.patch.set_facecolor('white')
#plt.yscale('log', nonposy='clip')
plt.xscale('log', nonposx='clip')
plt.ticklabel_format(style='sci', axis='y', scilimits=(0,0))
plt.xlabel(r'$\theta$ (degree)', fontsize=12, labelpad=0)
plt.ylabel(r'Q$^2$/2m$^2$', fontsize=12)

x = np.arange(0.001, 90.0, 0.001)
xrad = np.radians(x)
yinf = Q2inf(xrad)
y0 = Q20(xrad)
plt.plot(x, yinf, linewidth=2.0, label=r'$E_\nu=\infty$')
plt.plot(x, y0, 'r', linewidth=2.0, label=r'$E_\nu=0$')
plt.legend(loc='upper right')
x1,x2,y1,y2 = plt.axis()
plt.axis((0.08,x2,0,1e6))
plt.savefig('plots/Q2.eps')

plt.show()
