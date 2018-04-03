/**
    @file   OpenGL.h

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

#ifndef NUMCHLADNI_OPENGL_H
#define NUMCHLADNI_OPENGL_H

#include <iostream>
#include <vector>
#include <cmath>

#include <GL3/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "qtdefs.h"
#include "GLShader.h"
#include "SystemData.h"
#include "Camera.h"


#include <QGLWidget>
#include <QGLFormat>
#include <QMap>
#include <QScriptEngine>


/**
  *  @brief OpenGL render engine.
  *
  *  The OpenGL render engine is based on the QGLWidget
  */
class OpenGL : public QGLWidget
{
    Q_OBJECT


public:
    OpenGL( QGLFormat format, SystemData* sd, QWidget* parent = 0 );
    virtual ~OpenGL ( );

    // --------- public methods -----------
public:
    void AddObjectsToScriptEngine( QScriptEngine* engine );    
    void GenMeshBuffers();
    void GenDataTexture();
    void UpdateShaders();
    void SetShaderProps();
    void DeleteMeshBuffers();    

public slots:
    void setCtrlPoints();
    void setCtrlSegments();
    void timeStep();

    void setBGColor();

   // ------------ signals ---------------
signals:
    void  haveOGLParams();
    void  emitViewChanged();
    void  borderChanged();
    void  resetBorders();
    void  delCtrlPoint    ( int );
    void  checkCtrlPoint  ( int );
    void  addCtrlPoint    ( double, double );
    void  moveCtrlPoint   ( double, double );
    void  setActivePoint  ( int );
    void  addCtrlSegment  ( int, int );
    void  setActiveSegment( int, int );

   // -------- protected methods ---------
protected:
   /** Initialize OpenGL rendering.
    */
    virtual void initializeGL();

   /**  Main rendering method.
    */
    virtual void paintGL();
    virtual void resizeGL( int w, int h );

   /** Evaluate key press event.
    */
    virtual void keyPressEvent( QKeyEvent* event );

   /** Evaluate key release event.
    */
    virtual void keyReleaseEvent( QKeyEvent* event );

   /** Evaluate mouse press event.
    */
    virtual void mousePressEvent( QMouseEvent* event );

   /** Evaluate mouse release event.
    */
    virtual void mouseReleaseEvent( QMouseEvent* event );

   /** Evaluate mouse move event.
    */
    virtual void mouseMoveEvent( QMouseEvent* event );


    void  createShaders();
    void  removeShaders();
    void  drawGrid();
    void  drawCtrlPoints();
    void  drawCtrlLines();
    void  draw2DView();
    void  draw3DView();
    int   findActivePoint( QPoint mousePos );


// -------- private attributes --------
private:
    SystemData*       mData;
    int               mKeyPressed;
    int               mKeyModifier;
    Qt::MouseButton   mButtonPressed;
    QPoint            mLastPos;    

    QString    mGridVertShaderName;
    QString    mGridFragShaderName;
    QString    mCtrlPointerVertShaderName;
    QString    mCtrlPointerGeomShaderName;
    QString    mCtrlPointerFragShaderName;
    QString    mCtrlLineVertShaderName;
    QString    mCtrlLineGeomShaderName;
    QString    mCtrlLineFragShaderName;
    QString    mView2DVertShaderName;
    QString    mView2DGeomShaderName;
    QString    mView2DGeomShaderNameQuad;
    QString    mView2DFragShaderName;
    QString    mView3DVertShaderName;
    QString    mView3DGeomShaderName;
    QString    mView3DGeomShaderNameQuad;
    QString    mView3DFragShaderName;
    QString    mView3DTessCtrlShaderName;
    QString    mView3DTessCtrlShaderNameQuad;
    QString    mView3DTessEvalShaderName;
    QString    mObjectVertShaderName;
    QString    mObjectFragShaderName;

    glm::mat4  projMX;
    GLuint     va_line;
    GLuint     vbo_line;
    GLuint     va_controlPoints;
    GLuint     vbo_controlPoints;
    GLuint     ido_controlPoints;
    GLuint     ibo_controlSegments;
    float*     m_controlVertices;
    int*       m_controlVertIdx;
    unsigned int* m_controlSegments;

    GLShader   mGridShader;
    GLShader   mCtrlPointShader;
    GLShader   mCtrlLineShader;
    GLShader   mView2DShader;
    GLShader   mView3DShader;
    GLShader   mObjectShader;

    int mSegP1, mSegP2;
    bool mSegSet;

    GLuint  va_triangles;
    GLuint  vbo_triangles;
    GLuint  ibo_triangles;
    GLuint  texID;

    GLuint  va_box;
    GLuint  vbo_box;
    GLuint  ibo_box;
};

#endif // NUMCHLADNI_OPENGL_H
