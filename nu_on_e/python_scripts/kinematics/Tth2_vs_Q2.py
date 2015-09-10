import matplotlib.pyplot as plt
import numpy as np
import matplotlib
from math import *

# limiting condition with E_nu = 0
@np.vectorize
def Tth20(x):
  return 0

@np.vectorize
def Q20(x):
  return 0

# limiting condition with E_nu = infinity
@np.vectorize
def Tth2inf(x):
  return cos(x)*cos(x)*x*x/sin(x)/sin(x)

@np.vectorize
def Q2inf(x):
  return cos(x)*cos(x)/sin(x)/sin(x)


fig1 = plt.figure()
fig1.patch.set_facecolor('white')
#~ plt.ticklabel_format(style='sci', axis='x', scilimits=(0,0))
plt.ticklabel_format(style='sci', axis='y', scilimits=(0,0))
plt.xscale('log', nonposx='clip')
plt.xlabel(r'$Q^2/4m^2$', fontsize=12, labelpad=0)
plt.ylabel(r'$T\theta^2/2m$', fontsize=12)

ang = np.arange(0.1, 90.0, 0.001)
angrad = np.radians(ang)
x0 = Q20(angrad)
y0 = Tth20(angrad)
xinf = Q2inf(angrad)
yinf = Tth2inf(angrad)
plt.plot(xinf, yinf, linewidth=2.0, label=r'$E_\nu=\infty$')
plt.plot(x0, y0, 'r', linewidth=2.0, label=r'$E_\nu=\infty$')

plt.show()
