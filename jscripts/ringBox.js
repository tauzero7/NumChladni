//
//
//
Main.Reset()

var Num = 16
var dx  = 0.1

CM.addPoint(-dx,-dx)
CM.addPoint(dx,-dx)
CM.addPoint(dx,dx)
CM.addPoint(-dx,dx)
CM.checkPoint(1)
CM.checkPoint(2)
CM.checkPoint(3)
CM.checkPoint(4)
CM.addSegment(1,2)
CM.addSegment(2,3)
CM.addSegment(3,4)
CM.addSegment(4,1)

var n = 5
var phiStep = 360.0/Num

for(var i=0; i<Num; i++) {
  var phi = i*phiStep-phiStep/2
  var x = Math.cos(phi/180*3.141)
  var y = Math.sin(phi/180*3.141)
  CM.addPoint(x,y)
  if (i+1<Num) {
     CM.addSegment(n+i,n+i+1)
  }
}
CM.addSegment(Num-1+n,n)
CM.addHole(0,0)

Ctrl.maxArea = 0.01
Ctrl.minAngle = 30.0
Ctrl.quadr = true
Ctrl.elast = true

//Ctrl.CalcMesh()

//Ctrl.modus = "3D view"
//Ctrl.scale = 0.5
//Ctrl.Play()
//Ctrl.ev = 3