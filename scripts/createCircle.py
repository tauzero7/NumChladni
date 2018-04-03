"""
   Create circle poly file
  
    execfile('createCircle.py')
"""

import math

N = 8
radius = 1.0


phiStep = 2.0*math.pi/(N)
bm = 99

f = open('../models/regOctagon.poly','w');

f.write("# this file was generated with createCircle.py\n")
f.write("# vertices\n")
f.write("{0} 2 0 1\n".format(N))

for i in range(0,N):
    x = radius * math.cos(phiStep*i)
    y = radius * math.sin(phiStep*i)
    f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(i+1,x,y,bm))

f.write("# segments\n")
f.write("{0} 1\n".format(N))

for i in range(0,N):
    if (i<N-1):
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,i+2,bm))
    else:
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,1,bm))

f.write("# holes\n")
f.write("{0}\n".format(0))

f.close()
