
######################################################################  RELATIVE PATHS
GLM_DIR      = $$TOP_DIR/glm
GL3W_DIR     = $$TOP_DIR/gl3w
SHD_DIR      = $$TOP_DIR/shaders
SRC_DIR      = $$TOP_DIR/src
JS_DIR       = $$TOP_DIR/jscripts
PROJECT_MAIN = $$SRC_DIR/main.cpp

######################################################################  HEADERS and SOURCES

MY_HEADERS  = $$SRC_DIR/qtdefs.h \
              $$SRC_DIR/MainWindow.h \
              $$SRC_DIR/OpenGL.h \
              $$SRC_DIR/OGLProps.h \
              $$SRC_DIR/Camera.h \
              $$SRC_DIR/ControlMesh.h \
              $$SRC_DIR/DoubleEdit.h \
              $$SRC_DIR/GLShader.h \
              $$SRC_DIR/HoleListModel.h \
              $$SRC_DIR/PointListModel.h \
              $$SRC_DIR/SegmentListModel.h \
              $$SRC_DIR/SystemData.h \
              $$SRC_DIR/SystemView.h \
              $$SRC_DIR/triangle.h \
              $$SRC_DIR/ScriptEditor.h \
              $$SRC_DIR/SyntaxHighlighter.h

MY_SOURCES  = $$SRC_DIR/MainWindow.cpp \
              $$SRC_DIR/OpenGL.cpp \
              $$SRC_DIR/OGLProps.cpp \
              $$SRC_DIR/Camera.cpp \
              $$SRC_DIR/ControlMesh.cpp \
              $$SRC_DIR/DoubleEdit.cpp \
              $$SRC_DIR/GLShader.cpp \
              $$SRC_DIR/HoleListModel.cpp \
              $$SRC_DIR/PointListModel.cpp \
              $$SRC_DIR/SegmentListModel.cpp \
              $$SRC_DIR/SystemData.cpp \
              $$SRC_DIR/SystemView.cpp \
              $$SRC_DIR/triangle.c \
              $$SRC_DIR/ScriptEditor.cpp \
              $$SRC_DIR/SyntaxHighlighter.cpp



######################################################################  INCLUDE and DEPEND
INCLUDEPATH +=  . .. $$SRC_DIR  $$GLM_DIR $$GL3W_DIR

######################################################################  OTHER_FILES: SHADERS
OTHER_FILES += $$SHD_DIR/grid2d.frag \
               $$SHD_DIR/grid2d.vert \
               $$SHD_DIR/ctrlPoints.frag \
               $$SHD_DIR/ctrlPoints.geom \
               $$SHD_DIR/ctrlPoints.vert \
               $$SHD_DIR/ctrlLines.frag  \
               $$SHD_DIR/ctrlLines.geom  \
               $$SHD_DIR/ctrlLines.vert \
               $$SHD_DIR/view2d.frag \
               $$SHD_DIR/view2d.geom \
               $$SHD_DIR/view2d_quad.geom \
               $$SHD_DIR/view2d.vert \
               $$SHD_DIR/view3d.frag \
               $$SHD_DIR/view3d.geom \
               $$SHD_DIR/view3d_quad.geom \
               $$SHD_DIR/view3d.vert \
               $$SHD_DIR/view3d.tc \
               $$SHD_DIR/view3d_quad.tc \
               $$SHD_DIR/view3d.te \
               $$SHD_DIR/object.frag \
               $$SHD_DIR/object.vert

OTHER_FILES += $$JS_DIR/ringBox.js

######################################################################  feste Angaben
CONFIG   += console warn_on
QT       += core gui opengl script
TEMPLATE  = app

DEFINES  += TRILIBRARY ANSI_DECLARATORS # REDUCED


######################################################################  intermediate moc and object files
CONFIG(debug, debug|release) {
        TARGET = ../NumChladni32d
}

CONFIG(release, debug|release) {
        TARGET = ../NumChladni32
}

######################################################################  Input
HEADERS += $$MY_HEADERS
SOURCES += $$MY_SOURCES $$PROJECT_MAIN

RESOURCES += numchladni.qrc

HEADERS += $$GL3W_DIR/GL3/gl3.h $$GL3W_DIR/GL3/gl3w.h
SOURCES += $$GL3W_DIR/gl3w.c

HEADERS += $$GLM_DIR/glm/glm.hpp \
           $$GLM_DIR/glm/detail/*.hpp \
           $$GLM_DIR/glm/gtc/*.hpp \
           $$GLM_DIR/glm/gtx/*.hpp \
           $$GLM_DIR/glm/simd/*.h


win32:HEADERS += wglext.h

unix:!macx {
    QMAKE_CXXFLAGS += -Wall -Wno-comment
    LIBS += -ldl

    USE_GSL {
        LIBS += -L$$GSL_DIR/lib -lgsl -lgslcblas -Wl,-rpath $$GSL_DIR/lib
        INCLUDEPATH += $$GSL_DIR/include
        DEFINES += HAVE_GSL
        CONFIG(release, debug|release) {
            DESTDIR     = $$TOP_DIR/releaseGSL
            OBJECTS_DIR = $$TOP_DIR/releaseGSL/object
            MOC_DIR     = $$TOP_DIR/releaseGSL/moc
        }
        CONFIG(debug, debug|release) {
            DESTDIR     = $$TOP_DIR/debugGSL
            OBJECTS_DIR = $$TOP_DIR/debugGSL/object
            MOC_DIR     = $$TOP_DIR/debugGSL/moc
        }
    } else {
        USE_LAPACK {
            LIBS += -L$$LAPACK_DIR/lib -llapacke -llapack -lblas -lgfortran -lm
            INCLUDEPATH += $$LAPACK_DIR/include
            DEFINES += HAVE_LAPACK
            CONFIG(release, debug|release) {
                DESTDIR     = $$TOP_DIR/releaseLAPACK
                OBJECTS_DIR = $$TOP_DIR/releaseLAPACK/object
                MOC_DIR     = $$TOP_DIR/releaseLAPACK/moc
            }
            CONFIG(debug, debug|release) {
                DESTDIR     = $$TOP_DIR/debugLAPACK
                OBJECTS_DIR = $$TOP_DIR/debugLAPACK/object
                MOC_DIR     = $$TOP_DIR/debugLAPACK/moc
            }
        } else {
            USE_MAGMA {
                LIBS += -L$$MAGMA_DIR/lib -lmagma -lmagmablas \
                        -L$$CUDA_LIBDIR -lcudart -lcublas -L$$LAPACK_DIR -llapack -lrefblas -lgfortran -lcblas
                INCLUDEPATH += $$MAGMA_DIR/include $$CUDA_DIR/include
                DEFINES += HAVE_MAGMA  ADD_
                CONFIG(release, debug|release) {
                    DESTDIR     = $$TOP_DIR/releaseMAGMA
                    OBJECTS_DIR = $$TOP_DIR/releaseMAGMA/object
                    MOC_DIR     = $$TOP_DIR/releaseMAGMA/moc
                }
                CONFIG(debug, debug|release) {
                    DESTDIR     = $$TOP_DIR/debugMAGMA
                    OBJECTS_DIR = $$TOP_DIR/debugMAGMA/object
                    MOC_DIR     = $$TOP_DIR/debugMAGMA/moc
                }
            }
        }
    }
}

win32 {
   USE_GSL {
      LIBS += -L"$$GSL_DIR/lib"
      CONFIG(release, debug|release) {
         LIBS += $$GSL_DIR/lib/Win32/Release/gsl.lib  $$GSL_DIR/lib/Win32/Release/cblas.lib
         DESTDIR     = $$TOP_DIR/releaseGSL
         OBJECTS_DIR = $$TOP_DIR/releaseGSL/object
         MOC_DIR     = $$TOP_DIR/releaseGSL/moc
      }
      CONFIG(debug, debug|release) {
         LIBS += $$GSL_DIR/lib/Win32/Debug/gsl.lib  $$GSL_DIR/lib/Win32/Debug/cblas.lib
         DESTDIR     = $$TOP_DIR/debugGSL
         OBJECTS_DIR = $$TOP_DIR/debugGSL/object
         MOC_DIR     = $$TOP_DIR/debugGSL/moc
      }
      INCLUDEPATH += $$GSL_DIR/include
      DEFINES += HAVE_GSL
   } else {
      USE_LAPACK {
         DEFINES += HAVE_LAPACK
         INCLUDEPATH += $$LAPACK_DIR/include
         #LIBS += $$LAPACK_DIR/lib/liblapack.lib $$LAPACK_DIR/lib/liblapacke.lib $$LAPACK_DIR/lib/libblas.lib
         LIBS += -L"$$LAPACK_DIR/lib/" -llapack -llapacke -lblas
         CONFIG(release, debug|release) {
            DESTDIR     = $$TOP_DIR/releaseLAPACK
            OBJECTS_DIR = $$TOP_DIR/releaseLAPACK/object
            MOC_DIR     = $$TOP_DIR/releaseLAPACK/moc
         }
         CONFIG(debug, debug|release) {
            DESTDIR     = $$TOP_DIR/debugLAPACK
            OBJECTS_DIR = $$TOP_DIR/debugLAPACK/object
            MOC_DIR     = $$TOP_DIR/debugLAPACK/moc
         }
      } else {
         USE_MAGMA {
            DEFINES += HAVE_MAGMA ADD_
            CONFIG(release, debug|release) {
               DESTDIR     = $$TOP_DIR/releaseMAGMA
               OBJECTS_DIR = $$TOP_DIR/releaseMAGMA/object
               MOC_DIR     = $$TOP_DIR/releaseMAGMA/moc
            }
            CONFIG(debug, debug|release) {
               DESTDIR = $$TOP_DIR/debugMAGMA
               OBJECTS_DIR = $$TOP_DIR/debugMAGMA/object
               MOC_DIR     = $$TOP_DIR/debugMAGMA/moc
            }
         }
      }
   }
}
