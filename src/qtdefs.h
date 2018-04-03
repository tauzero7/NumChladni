/**
    @file   qtdefs.h

    Copyright (c) 2013, Universitaet Stuttgart, VISUS, Thomas Mueller

    This file is part of NumChladni.

    NumChladni is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NumChladni is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NumChladni.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NUMCHLADNI_DEFS_H
#define NUMCHLADNI_DEFS_H

#include <GL3/gl3w.h>

#include <QList>
#include <QString>
#include <QStringList>
#include <QColor>

#include <glm/glm.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

// -----------------------------------
//   global definitions
// -----------------------------------
#ifndef DEG_TO_RAD
#define DEG_TO_RAD  0.017453292519943295770
#define RAD_TO_DEG  57.295779513082320875
#endif

#ifndef M_PI
#define M_PI   3.1415926535898
#define M_PI_2 1.5707963267949
#endif

#define  DEF_SIGN(x)   ((x)>=0?1:-1)
#define  DEF_SQR(x)    ((x)*(x))

// -----------------------------------
//   GUI definitions
// -----------------------------------

#ifdef  USE_BIG_SCREEN
#define    DEF_GUI_WIDTH     1550
#define    DEF_GUI_HEIGHT    1150
#define    DEF_OGL_WIDTH     1100
#define    DEF_OGL_HEIGHT     700
#define    DEF_TEXT_WIDTH     350
#define    DEF_WIDGET_HEIGHT   20
#define    DEF_DWIDGET_WIDTH   60
#define    DEF_MAX_PER_LINE     8
#define    DEF_MAX_LINES       30
#else
#define    DEF_GUI_WIDTH     1270
#define    DEF_GUI_HEIGHT     700
#define    DEF_OGL_WIDTH      900
#define    DEF_OGL_HEIGHT     500
#define    DEF_TEXT_WIDTH     280
#define    DEF_WIDGET_HEIGHT   20
#define    DEF_DWIDGET_WIDTH   60
#define    DEF_MAX_PER_LINE     8
#define    DEF_MAX_LINES       26
#endif

#define    DEF_TBLVIEW_ROW_HEIGHT  20
#define    DEF_MAX_NUM_REC_FILES   4

#define    BOUNDARY_FIXED_MARKER   99

#ifndef SIGNUM
#define SIGNUM(x) (x>=0 ? 1.0 : -1.0)
#endif

const QColor FLOAT_EDIT_COLOR = QColor(10,10,100);
const int    FLOAT_PREC = 3;

const QColor init_bg_color              = QColor(10,10,10);
const QColor init_point_color           = QColor(255,255,0);
const QColor init_point_color_highlight = QColor(255,0,0);
const QColor init_line_color            = QColor(200,240,200);
const QColor init_line_color_highlight  = QColor(255,100,100);

const double init_ortho_left   = -2.0;
const double init_ortho_right  =  2.0;
const double init_ortho_bottom = -2.0;
const double init_ortho_top    =  2.0;
const double init_grid_origX   =  0.0;
const double init_grid_origY   =  0.0;
const double init_grid_stepX   =  1.0;
const double init_grid_stepY   =  1.0;

const double init_proj_dist    = 4.5;
const double init_proj_fov     = 35.0;
const double init_proj_theta   = 35.0f;
const double init_proj_phi     = -90.0f;
const double init_proj_poiX    = 0.0;
const double init_proj_poiY    = 0.0;
const double init_proj_poiZ    = 0.0;

const double init_max_area     =  0.1;
const double init_min_angle    =  0.0;

const double init_freq  = 1.0;

const int MAX_NUM_CTRL_POINTS  = 1000;

enum  e_viewModus {
    e_viewInput = 0,
    e_view2D,
    e_view3D
};

const QStringList stl_viewModus = QStringList()
        << "Input"
        << "2D view"
        << "3D view";

typedef struct  ControlPos_t
{
    double x;
    double y;
    bool   isFixed;
} ControlPos;

enum  e_spMouseCtrl {
    e_spMouse_standard = 0,
    e_spMouse_addMovePoint,
    e_spMouse_checkPoint,
    e_spMouse_delPoint,
    e_spMouse_addSegment,
    e_spMouse_addHole
};

const QStringList stl_spMouseCtrl = QStringList()
        << "Mouse ctrl"
        << "Add/Move point"
        << "Check point"
        << "Delete point"
        << "Add/Del Segment"
        << "Add Hole";

typedef struct nodeT {
    int  vIdx;
    glm::dvec2 pos;
    int  bmarker;
    bool isFixed;
} node_t;

typedef struct segmentT {
    int num;
    int p1;
    int p2;
    int bmarker;
} segment_t;

typedef struct holeT {
    int num;
    glm::dvec2 pos;
} hole_t;

typedef struct triIdxT {
    glm::ivec3  v;
    glm::ivec3  a;
} triIdx_t;

const double fac_lin[] = {0.5,0.5,0.5,1.0/24.0,1.0/6.0};
const double ms1_lin[] = {1,-1,0,-1,1,0,0,0,0};
const double ms2_lin[] = {2,-1,-1,-1,0,1,-1,1,0};
const double ms3_lin[] = {1,0,-1,0,0,0,-1,0,1};
const double ms4_lin[] = {2,1,1,1,2,1,1,1,2};
const double vs1_lin[] = {1,1,1};

const double fac_quad[] = {1.0/6.0,1.0/6.0,1.0/6.0,1.0/360.0,1.0/6.0};
const double ms1_quad[] = {3,1,0,-4,0,0,1,3,0,-4,0,0,0,0,0,0,0,0,-4,-4,0,8,0,0,0,0,0,0,8,-8,0,0,0,0,-8,8};
const double ms2_quad[] = {6,1,1,-4,0,-4,1,0,-1,-4,4,0,1,-1,0,0,4,-4,-4,-4,0,8,-8,8,0,4,4,-8,8,-8,-4,0,-4,8,-8,8};
const double ms3_quad[] = {3,0,1,0,0,-4,0,0,0,0,0,0,1,0,3,0,0,-4,0,0,0,8,-8,0,0,0,0,-8,8,0,-4,0,-4,0,0,8};
const double ms4_quad[] = {6,-1,-1,0,-4,0,-1,6,-1,0,0,-4,-1,-1,6,-4,0,0,0,0,-4,32,16,16,-4,0,0,16,32,16,0,-4,0,16,16,32};
const double vs1_quad[] = {0,0,0,1,1,1};


const int numBoxVerts = 8;
const float boxVerts[] = {
    0.0f,0.0f,0.0f,
    1.0f,0.0f,0.0f,
    1.0f,1.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f,
    1.0f,0.0f,1.0f,
    1.0f,1.0f,1.0f,
    0.0f,1.0f,1.0f
};

const int numBoxEdges = 12;
const unsigned int boxEdges[] = {
    0,1, 1,2, 2,3, 3,0,
    4,5, 5,6, 6,7, 7,4,
    0,4, 1,5, 2,6, 3,7
};

enum e_shading {
    e_shd_gray = 0,
    e_shd_graysign,
    e_shd_color,
    e_shd_colorsign,
    e_shd_barycentric
};

const QStringList stl_shading = QStringList()
        << "Gray"
        << "Gray/Sign"
        << "Color"
        << "Color/Sign"
        << "barycentric";

#endif // NUMCHLADNI_DEFS_H
