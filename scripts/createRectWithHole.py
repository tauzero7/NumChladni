"""
   Create circle poly file
  
    execfile('createCircle.py')
"""

import math

N = 15
radius   = 0.05
holePosX = 0.8
holePosY = 0.7
rectLA   = 2.4
rectLB   = 2.0

phiStep = 2.0*math.pi/(N)
bmHole = 99
bmRect = 1

f = open('../models/rectWithHole.poly','w');

f.write("# this file was generated with createCircle.py\n")
f.write("# vertices\n")
f.write("{0} 2 0 1\n".format(N+4))

for i in range(0,N):
    x = holePosX + radius * math.cos(phiStep*i)
    y = holePosY + radius * math.sin(phiStep*i)
    f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(i+1,x,y,bmHole))

f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(N+2,-0.5*rectLA,-0.5*rectLB,bmRect))
f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(N+3, 0.5*rectLA,-0.5*rectLB,bmRect))
f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(N+4, 0.5*rectLA, 0.5*rectLB,bmRect))
f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(N+5,-0.5*rectLA, 0.5*rectLB,bmRect))

f.write("# segments\n")
f.write("{0} 1\n".format(N+4))

for i in range(0,N):
    if (i<N-1):
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,i+2,bmHole))
    else:
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,1,bmHole))

f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(N+1,N+1,N+2,bmRect))
f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(N+2,N+2,N+3,bmRect))
f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(N+3,N+3,N+4,bmRect))
f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(N+4,N+4,N+1,bmRect))

f.write("# holes\n")
f.write("{0}\n".format(1))
f.write("  {0} {1:8.3f} {2:8.3f}\n".format(1,holePosX,holePosY))

f.close()
