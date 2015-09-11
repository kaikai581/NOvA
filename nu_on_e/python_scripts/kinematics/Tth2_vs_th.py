

# the independent variable used here is in degree

# limiting condition with E_nu = 0
def Tth20(x):
  return 0

# limiting condition with E_nu = infinity
# function is normalized with 2m, i.e. T*th^2/2m
def Tth2inf(x):
  return cos(x)*cos(x)*x*x/sin(x)/sin(x)

# 
