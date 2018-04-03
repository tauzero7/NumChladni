"""
   Create ring poly file
  
    execfile('createCircle.py')
"""

import math

N1 = 50
N2 = 15
radiusOutA = 1.2
radiusOutB = 1.0
radiusIn   = 0.05
innerRingPosX = 0.7
innerRingPosY = 0.0

phiStep1 = 2.0*math.pi/N1
phiStep2 = 2.0*math.pi/N2
bmOut = 1
bmIn  = 99

f = open('../models/ring.poly','w');

f.write("# this file was generated with createCircle.py\n")
f.write("# vertices\n")
f.write("{0} 2 0 1\n".format(N1+N2))

for i in range(0,N1):
    x = radiusOutA * math.cos(phiStep1*i)
    y = radiusOutB * math.sin(phiStep1*i)
    f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(i+1,x,y,bmOut))

for i in range(0,N2):
    x = innerRingPosX + radiusIn * math.cos(phiStep2*i)
    y = innerRingPosY + radiusIn * math.sin(phiStep2*i)
    f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(N1+i+1,x,y,bmIn))

f.write("# segments\n")
f.write("{0} 1\n".format(N1+N2))

for i in range(0,N1):
    if (i<N1-1):
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,i+2,bmOut))
    else:
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,1,bmOut))

for i in range(0,N2):
    if (i<N2-1):
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(N1+i+1,N1+i+1,N1+i+2,bmIn))
    else:
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(N1+i+1,N1+i+1,N1+1,bmIn))


f.write("# holes\n")
f.write("{0}\n".format(1))
f.write("  {0} {1:8.3f} {2:8.3f}\n".format(1,innerRingPosX,innerRingPosY))

f.close()
