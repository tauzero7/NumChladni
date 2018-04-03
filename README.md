
# NumChladni  
* Copyright (c) 2013 - 2018, Thomas Mueller
* Email: tauzero7@gmail.com
* Article:  
    T. Mueller,  
    __Numerical Chladni figures__,  
    European Journal of Physics __34__, 1067--1074 (2013),  
    [http://iopscience.iop.org/article/10.1088/0143-0807/34/4/1067]()
* Preprint:  
    [http://xxx.lanl.gov/abs/1308.5523](arXiv:1308.5523)    

---

## Content:  
* Before installation  
* Linux installation with GSL, LAPACK, or MAGMA  
* Windows installation with GSL or LAPACK  
* HowTo  
* The UI elements in detail  
* Scripting  

---


## Before installation:  

NumChladni is based on the Qt framework, the open graphics library (OpenGL), 
and the OpenGL shading language (GLSL). On standard Linux/Windows machines, 
OpenGL is already installed via the graphics board driver. Qt can be downloaded 
from the developer site as a LGPL-licensed version.

Note that you need a graphics board that supports the OpenGL Shading 
language (GLSL version 3 or higher). If you want to use MAGMA, your
graphics board must also support CUDA (the Compute Unified Device 
Architecture).

You need at least one of the following packages for solving linear
algebra problems:

* GSL -- GNU Scientific library, [http://www.gnu.org/software/gsl]()
   
* LAPACK -- Linear Algebra PACKage, [http://www.netlib.org/lapack]()
   
* MAGMA -- Matrix Algebra on GPU and Multicore Architectures  
    [http://icl.cs.utk.edu/magma]()
   
---
   
## Linux installation with GSL, LAPACK, or MAGMA:

* GSL installation example:  
    - download GSL version 1.15 and extract the tar-archive
    - ./configure --prefix=/home/username/local/gsl/1.15
    - make
    - make install
   
* LAPACK installation example:  
    - you need a fortran compiler installed (e.g. gfortran) as well as cmake
    - download LAPACK version 3.8.0 and extract the archive
    - cd lapack-3.8.0 / mkdir build / cd build
    - ccmake ..  
        - set LAPACKE = ON
        - set CMAKE_INSTALL_PREFIX to "path/to/NumChladni/lapack"
    - make install
   
* MAGMA installaton example:  
    - compile LAPACK as before
    - cp make.inc.atlas make.inc
    - adjust GPU_TARGET
    - adjust path to liblapack.a
    - adjust CUDADIR
    - make
   
* Compile NumChladni:  
    - Within the NumChladni folder: run "qmake && make"
    - Now, you can run NumChladni
   
---   
   
## Windows installation with GSL or LAPACK:

* GSL installation example:  
    - download GSL for Windows/Visual Studio 2010
       (a precompiled version can also be found at the project page:
        http://www.vis.uni-stuttgart.de/numchladni)
    

* LAPACK installation example:  
    - mingw with c++ and fortran must be installed in advance !!
       http://www.mingw.org/
    - download LAPACK for Windows/mingw
       http://icl.cs.utk.edu/lapack-for-windows/lapack/  
       
    You need:
          lapacke.h lapacke_mangling.h  -> lapack/include
          libblas.dll libblas.lib liblapack.dll liblapack.lib 
          liblapacke.dll liblapacke.lib -> lapack/lib


* Compile NumChladni via the QtCreator:
    - Load numchladni.win.pro and select the GSL or LAPACK solver
      by uncommenting the corresponding lines. Adjust also the 
      corresponding paths.    
    - Within the project tab, deselect "Shadow-Build" and select
      either mingw or msvc2010 as Qt-version
    - Build -> run qmake
    - Press Ctrl-B or go to Build -> Build project...
    - Now, you can run NumChladni

    If you want to run NumChladni from outside QtCreator, you can 
    use the startNumChladni.. scripts.   

---

## HowTo:

Let us go through a simple example:

* Open NumChladni

* Go to "ControlMesh" and select "Add/Move point"

* Set four points by clicking with the right mouse button
    within the OpenGL window: they should be roughly located
    at (-1,-1), (1,-1), (1,1), (-1,1).

* As you will not exactly hit these point, you can correct them
    within the "ControlMesh" -> "Points" tab. Here, check 'fix'
    for every point.

* Go to "ControlMesh" and select "Add/Del Segment"

* Click on point 1 within the OpenGL window with the right mouse button.
    Then, click immediately on point 2 within the OpenGL window.
    A segement should now be shown (see also "ControlMesh" -> "Segments" tab).

* Repeat this step until you have a quad.     

* Go to "Control" and set "MinAngle" to 30.

* Press "Calc mesh"

* In the status bar, you should read  #Verts: 145,  #Tri: 64

* Go to "Control" and select "2D view" as view modus.

* Go to "Control" -> "Animate" and set "Scale" to 0.5. 
    This will scale the eigenvalues.

* Press the play button and watch :-)

* You can now switch between the eigenmodes by selecting
   "Control" -> "Animate" -> "#EV".

* You can improve the eigenmodes by setting "MaxArea" to 0.01
    and pressing the "Calc mesh" button again.
    Depending on your system, this step could take some time.

* You can save the polygon mesh via "File" -> "Save poly".
    But note that you only save the four points and the four
    segments. If you load the mesh again, you also have to rerun
    the mesh generation and the EV calculation.

---

## The UI elements in detail:

* Control:
    - View Modus:  switch between Input, 2D view, and 3D view
    
    GenMesh:  
        - MaxArea:     maximum triangle area for mesh generation  
        - MinAngle:    minimum triangle angle for mesh generation 
        - quadr:       use linear or quadratic ansatz function  
        - C-Hull:      use convex hull  
        - Del'ay:      use Delaunay triangulation  
        - Elast:       if checkedm then non-fixed edges are   
                       elastically supported  
                    
    If you change any of these parameters, you have to recalculate 
    the triangle mesh by pressing "Calc mesh" !
                    
    Animate:  
        - #EV:         select available eigenmodes - eigenfrequency  
        - freq:        fixed frequency for animation  
        - Scale:       scaling of eigenmodes  


* ControlMesh:  
    You can only change points and segments in the "Input" view modus.

    - Add/Move point:   Add or move points using the right mouse button.
                        To move a point, hover above it, press the right
                        mouse button and move it to the new position.  
                        Then, release the mouse button again.
    - Check point:      Hover above a point and press the right mouse 
                        button to fix/unfix the point.
    - Delete point:     Delete a point by clicking the right mouse button.
    - Add/Del Segment:  Add a segment by clicking both points with the
                        right mouse button.
                        Delete a segment by selecting it in the "Segments" 
                        tab and pressing the "-" button.
    - Add Hole:         Press "+" to add a hole. Then, enter the correct
                        position.


* View Modus: Input / 2D view  
    - Move the point of interest with the left mouse button pressed.
    - Zoom in and out with the left mouse button and the "SHIFT" key pressed.
    - Press 'r' to reset the view.
    - You can modify the view also in the "OpenGL properties" window.


* View Modus: 3D view  
    - Rotate around the point of interest with the left mouse button pressed.
    - Move along the viewing direction with the left mouse button and the
      "SHIFT" key pressed.
    - Move the point of interest along the z-direction with the middle mouse
      button pressed.
    - Move the point of interest along the xy-plane with the middle mouse
      button and the "SHIFT" key pressed.
    - Press 'r' to reset the view.
    - You can modify the view also in the "OpenGL properties" window.

* File  
    - Grab window   : save NumChladni UI to png-file 
                         -> output/numchladni_screenshot_?.png
    - Grab view     : save current OpenGL view to png-file 
                         -> output/numchladni_framebuffer_?.png
    - Reset counter : reset the image counters for grabbing

    Using grabbing only with keyboard shortcuts.
    
---

## Scripting:

NumChladni is also scriptable using QtScript that is javascript like.
An example can be found in "jscripts/"

NumChladni specific commands:

    * CM.addPoint(x,y)          : add point at position (x,y)
    * CM.checkPoint(n)          : fix/unfix point n
    * CM.addSegment(p1,p2)      : add segment between points p1 and p2
    * CM.addHole(x,y)           : add hole at position (x,y)

    * Ctrl.maxArea              : set/get maximum area for triangle meshing  
    * Ctrl.minAngle             : set/get minimum angle for triangle meshing  
    * Ctrl.quadr                : toggle linear/quadratic ansatz function (true/false)  
    * Ctrl.chull                : toggle convex hull usage (true/false)  
    * Ctrl.delay                : toggle Delaunay triangulation (true/false)  
    * Ctrl.elast                : toggle elastic support (true/false)  
    * Ctrl.modus                : set view modus ("Input","2D view","3D view")  
    * Ctrl.scale                : set/get scaling factor  
    * Ctrl.ev                   : select eigenmode (0,...)  
    * Ctrl.Play()               : toggle play/pause  

    * OGL.left                  : set/get left border for 2D view (depends on aspect)
    * OGL.right                 : set/get right border for 2D view (depends on aspect)
    * OGL.bottom                : set/get bottom border for 2D view
    * OGL.top                   : set/get top border for 2D view
    * OGL.grid                  : toggle grid for 2D view (true/false)
    * OGL.aspect                : toggle aspect ratio for 2D view (true/false)
    * OGL.SetOrthoReset()       : reset 2D view parameters

    * OGL.color                 : toggle color for 2D view (true/false)
    * OGL.mesh                  : show only mesh in 2D view (true/false)
    * OGL.isolines              : show isolines in 2D view (true/false)

    * OGL.fov                   : set field of view for 3D view
    * OGL.dist                  : set camera distance for 3D view
    * OGL.theta                 : set camera angle for 3D view
    * OGL.phi                   : set camera angle for 3D view
    * OGL.poiX                  : set point of interest, x-coord  for 3D view
    * OGL.poiY                  : set point of interest, y-coord  for 3D view
    * OGL.poiZ                  : set point of interest, z-coord  for 3D view
    * OGL.box                   : show box for 3D view (true/false)
    * OGL.SetProjReset()        : reset 3D view parameters

    * OGL.wireframe             : show only wireframe for 3D view (true/false)
    * OGL.dotprod               : use dot product for 3D view (true/false)
    * OGL.shading               : shading for 3D view ("Gray","Gray/Sign","Color",
                                  "Color/Sign","barycentric")
    * OGL.tessIn                : inner tessellation for 3D view                              
    * OGL.tessOut               : outer tessellation for 3D view
                              

In the "ScriptEditor" -> "CmdLine" you can enter single command.
To execute a script, you should use the text editor, press "Ctrl+E".

Note that you better should reset NumChladni via "Ctrl+N" before
executing a script.


