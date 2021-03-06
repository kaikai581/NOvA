import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import quad
from scipy.interpolate import interp1d
from math import *

def diffxsec(x):
  #~ incident neutrino energy in GeV
  Enu = 2.
  #~ electron mass in GeV
  me = .511e-3
  A = me+Enu
  A2 = A*A
  me2 = me*me
  Enu2 = Enu*Enu
  g1 = -0.27
  g12 = g1*g1
  g2 = 0.23
  g22 = g2*g2
  result = Enu2*A2*x/(A2-Enu2*x*x)/(A2-Enu2*x*x)*(g12+g22*(1-(2*me*Enu*x*x)/(A2-Enu2*x*x))*(1-(2*me*Enu*x*x)/(A2-Enu2*x*x))-g1*g2*(2*me2*x*x)/(A2-Enu2*x*x))
  return result

@np.vectorize
def cumulxsec(x):
  return quad(diffxsec, x, 1)

def angeff(x):
  # angs = np.asarray(np.radians(x))
  # cosx = np.cos(angs)
  # vcumulxsec = np.vectorize(cumulxsec)
  cosx = np.cos(np.radians(x))
  return cumulxsec(cosx)

#~ plot cross section
fig1 = plt.figure()
fig1.patch.set_facecolor('white')
plt.yscale('log', nonposy='clip')
t1 = np.arange(0.0, 1.0, 0.001)
y = diffxsec(t1)
plt.xlabel(r'$\cos\theta$', fontsize=16)
plt.ylabel(r'$\frac{d\sigma}{d\cos\theta}/\sigma_0$', fontsize=16)
plt.plot(t1, y)

#~ plot angular cut efficiency
fig2 = plt.figure()
fig2.patch.set_facecolor('white')
ax1 = fig2.add_subplot(211)
plt.yscale('log', nonposy='clip')
plt.xlabel(r'$\cos\theta$', fontsize=16, labelpad=0)
plt.ylabel(r'$\frac{d\sigma}{d\cos\theta}/\sigma_0$', fontsize=16)
plt.title(r'$E_\nu = 2$ GeV')
plt.plot(t1, y, linewidth=2.0)

ax2 = fig2.add_subplot(212)
x2 = np.arange(0., 90., 0.001)
totxsec = quad(diffxsec, 0, 1)
y2 = angeff(x2)
yeff = [x/totxsec[0] for x in y2[0]]
plt.xlabel(r'$\theta$ (degree)', fontsize=16)
plt.ylabel(r'$\epsilon_\theta$', fontsize=16)
plt.plot(x2, yeff, 'r', linewidth=2.0)
plt.savefig('plots/xsec_cut_eff.eps')

# caculate the angles corresponding to 90% and 99% efficiencies
# note that show() will lock the terminal, therefore have to put code in front
eff_interp = interp1d(yeff, x2)
f = open('efficiency_angle.txt', 'w')
f.write('angle of 90% efficiency: ' + str(eff_interp(.9)) + '\n')
f.write('angle of 99% efficiency: ' + str(eff_interp(.99)))
f.close()

plt.show()
