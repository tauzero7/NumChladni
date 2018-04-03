"""
   Create octagon with circular hole
  
    execfile('createOctagon.py')
"""

import math

Noct = 8
octRadius = 1.0
octStep   = 2.0*math.pi/Noct
bmOctagon = 1

Nhole = 20
holeRadius = 0.5
holePosX   = 0.0
holePosY   = 0.0
holeStep = 2.0*math.pi/Nhole
bmHole = 99




f = open('../models/octagonHole.poly','w');

f.write("# this file was generated with createOctagon.py\n")
f.write("# vertices\n")
f.write("{0} 2 0 1\n".format(Noct+Nhole))

# create octagon
for i in range(0,Noct):
    x = octRadius * math.cos(octStep*i - math.radians(22.5))
    y = octRadius * math.sin(octStep*i - math.radians(22.5))
    f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(i+1,x,y,bmOctagon))

# create hole
for i in range(0,Nhole):
    x = holePosX + holeRadius * math.cos(holeStep*i)
    y = holePosY + holeRadius * math.sin(holeStep*i)
    f.write("{0:3d} {1:8.3f} {2:8.3f}  {3:2d}\n".format(Noct+i+1,x,y,bmHole))


f.write("# segments\n")
f.write("{0} 1\n".format(Noct+Nhole))

for i in range(0,Noct):
    if (i<Noct-1):
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,i+2,bmOctagon))
    else:
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(i+1,i+1,1,bmOctagon))

for i in range(0,Nhole):
    if (i<Nhole-1):
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(Noct+i+1,Noct+i+1,Noct+i+2,bmHole))
    else:
        f.write("{0:3d} {1:4d} {2:4d}  {3:2d}\n".format(Noct+i+1,Noct+i+1,Noct+1,bmHole))


f.write("# holes\n")
f.write("{0}\n".format(1))
f.write("  {0} {1:8.3f} {2:8.3f}\n".format(1,holePosX,holePosY))

f.close()
