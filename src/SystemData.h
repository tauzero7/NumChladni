/**
    @file   SystemData.h

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

#ifndef  NUMCHLADNI_SYSTEM_DATA_H
#define  NUMCHLADNI_SYSTEM_DATA_H


#include <GL3/gl3w.h>
#include <glm/glm.hpp>

#include <QString>
#include <QFile>
#include <QList>
#include <QColor>
#include <QPoint>
#include <QLineEdit>
#include <QLCDNumber>
#include <QTimer>
#include <QTime>

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>

#include "qtdefs.h"
#include "Camera.h"

#ifdef HAVE_GSL                  // HAVE_GSL
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#elif defined HAVE_LAPACK        // HAVE_LAPACK
#include <lapacke.h>

#elif defined HAVE_MAGMA         // HAVE_MAGMA
#include "magma.h"
#include "magma_lapack.h"
#endif


/**
 * @brief The SystemData class
 */
class SystemData : public QObject
{
    Q_OBJECT

public:
    SystemData();
    ~SystemData();

    void ClearAll();

    // --------- public methods -----------
public:
    /** Save triangle data to file
     * \param filename
     */
    bool SavePoly( QString filename );

    void ScreenPosToCoords( const QPoint pos, glm::dvec2 &c );
    void AdjustBorder(glm::dvec2 mmX, glm::dvec2 mmY, glm::dvec2 center );

    /** Call triangle library to calculate triangular mesh
     * \param triswitches  switch parameters for triangle library
     */
    bool DoTriangulation( const char *triswitches );

    /** Call either GSL, Lapack, or Magma routine to solve eigenvalue problem
     */
    void SolveSystem();

#ifdef HAVE_GSL
    gsl_matrix*  deleteElement( gsl_matrix* src, int N, int row, int col );
#elif defined HAVE_LAPACK || defined HAVE_MAGMA
    double* deleteElement( double *src, int N, int row, int col );
#endif

    /** Read node and element data from file
     *   This method is only used if calculation is done outside (deprecated).
     * \param nodeFileName
     * \param eleFileName
     */
    bool ReadNodeAndEleFile( QString nodeFileName, QString eleFileName);

protected:
    /** Initialize basis matrices for linear or quadratic ansatz
     * \param MSize  matrix size
     */
    void initMatrices( int MSize );

    /** Compile stiffness and mass matrices
     */
    void compileMatrices();

    /** Calculate parameters for coordinate transformation to canonical coordinates
     * \param v1
     * \param v2
     * \param v3
     * \param a
     * \param b
     * \param c
     * \param J
     */
    void calc_params( const glm::dvec2 v1, const glm::dvec2 v2, const glm::dvec2 v3,
                      double &a, double &b, double &c, double &J );

    /** Save stiffness and mass matric to file
     * \param filename
     */
    bool exportSMmatrices( QString filename );


    // -------- private attributes --------
public:
    int         m_screenWidth;       //!< Screen width of OrthoView
    int         m_screenHeight;      //!< Screen height of OrthoView
    double      m_aspect;
    bool        m_keepAspectRatio;   //!< Keep aspect ratio of grid
    QLineEdit*  led_status;
    QLCDNumber* lcd_numMeshVertices;
    QLCDNumber* lcd_numTriangles;

    Camera     mCamera;
    bool       m_showBox;
    bool       m_haveTessShader;

    e_viewModus  m_viewModus;
    QColor       m_bgColor;
    QColor       m_pointColor;
    QColor       m_pointColorHighlighted;
    QColor       m_lineColor;
    QColor       m_lineColorHighlighted;

    glm::dvec2  m_gridOrig;
    glm::dvec2  m_gridStep;
    glm::dvec4  m_border;
    bool        m_showGrid;
    bool        m_wireframe;
    e_shading   m_shading;
    bool        m_useDotProd;

    e_spMouseCtrl      m_mouseCtrl;
    //QList<ControlPos>  m_controlPos;
    int                m_activePoint;
    int                m_trackPoint;
    float              m_pointSize;
    glm::dvec2         m_viewScale;
    glm::ivec2         m_activeSegment;


    int numVertices;
    int numDim;
    int numAttribs;
    int numBMarker;
    int numSegments;
    int numSegBMarker;
    int numHoles;
    int numTriangles;
    int numNodesPerTriangle;
    int numTriAttribs;
    int m_idxOffset;

    QList<node_t>     m_vertices;
    QList<segment_t>  m_segments;
    QList<hole_t>     m_holes;

    QList<node_t>   mesh_vertices;
    QList<triIdx_t> mesh_triIndices;
    float*        mMeshVerts;
    unsigned int* mMeshIndices;
    int numMeshVertices;
    int numMeshAttribs;
    int numMeshBMarkers;

    int      m_currEV;
    QTimer*  m_timer;
    QTime    m_time;
    double   m_currTime;
    double   m_freq;
    double   m_scaleFactor;
    float    m_meshOpacity;
    bool     m_useColor;
    bool     m_useMeshOnly;
    bool     m_showIsolines;
    int      m_tessLevelInner;
    int      m_tessLevelOuter;
    int      m_maxPatchVertices;

    double   m_maxArea;
    double   m_minAngle;
    bool     m_useQuad;
    bool     m_useDelaunay;
    bool     m_useConvexHull;
    bool     m_elastSupported;

    int N;
    float *evals;
    double* m_eigenvalues;

#ifdef HAVE_GSL
    gsl_matrix *S1,*S2,*S3,*S4,*Se,*Me,*aS1,*bS2,*cS3,*S5l,*S5q;
    gsl_vector *s1;
    gsl_matrix *Stot;
    gsl_matrix *Mtot;
#elif defined HAVE_LAPACK || defined HAVE_MAGMA
    double *S1,*S2,*S3,*S4,*S5l,*S5q;
    double *s1;
    double *Stot;
    double *Mtot;
#endif
};

#endif // NUMCHLADNI_SYSTEM_DATA_H
