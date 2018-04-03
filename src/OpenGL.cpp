/**
    @file   OpenGL.cpp

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

#include <sstream>
#include <OpenGL.h>

#include <QApplication>
#include <QColorDialog>
#include <QKeyEvent>

OpenGL :: OpenGL(QGLFormat format, SystemData *sd, QWidget* parent )
  : QGLWidget(format, parent),
    va_line(0),
    vbo_line(0),
    va_controlPoints(0),
    vbo_controlPoints(0),
    ido_controlPoints(0),
    m_controlVertices(NULL),
    m_controlVertIdx(NULL),
    m_controlSegments(NULL),
    va_triangles(0)
{

    mData = sd;
    mButtonPressed = Qt::NoButton;
    setFocusPolicy( Qt::ClickFocus );
    mKeyPressed = Qt::Key_No;

    setMinimumSize( 300, 300 );
    setMouseTracking(true);
#ifdef _WIN32
    QString shaderPath = QApplication::applicationDirPath() + QString("/../shaders/");
#else
    QString shaderPath = QApplication::applicationDirPath() + QString("/shaders/");
#endif
    mGridVertShaderName           = shaderPath + QString("grid2d.vert");
    mGridFragShaderName           = shaderPath + QString("grid2d.frag");
    mCtrlPointerVertShaderName    = shaderPath + QString("ctrlPoints.vert");
    mCtrlPointerGeomShaderName    = shaderPath + QString("ctrlPoints.geom");
    mCtrlPointerFragShaderName    = shaderPath + QString("ctrlPoints.frag");
    mCtrlLineVertShaderName       = shaderPath + QString("ctrlLines.vert");
    mCtrlLineGeomShaderName       = shaderPath + QString("ctrlLines.geom");
    mCtrlLineFragShaderName       = shaderPath + QString("ctrlLines.frag");
    mView2DVertShaderName         = shaderPath + QString("view2d.vert");
    mView2DGeomShaderName         = shaderPath + QString("view2d.geom");
    mView2DFragShaderName         = shaderPath + QString("view2d.frag");
    mView2DGeomShaderNameQuad     = shaderPath + QString("view2d_quad.geom");
    mView3DVertShaderName         = shaderPath + QString("view3d.vert");
    mView3DGeomShaderName         = shaderPath + QString("view3d.geom");
    mView3DFragShaderName         = shaderPath + QString("view3d.frag");
    mView3DGeomShaderNameQuad     = shaderPath + QString("view3d_quad.geom");
    mView3DTessCtrlShaderName     = shaderPath + QString("view3d.tc");
    mView3DTessCtrlShaderNameQuad = shaderPath + QString("view3d_quad.tc");
    mView3DTessEvalShaderName     = shaderPath + QString("view3d.te");
    mObjectVertShaderName         = shaderPath + QString("object.vert");
    mObjectFragShaderName         = shaderPath + QString("object.frag");

    mSegP1 = mSegP2 = -1;
    mSegSet = false;    

    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
}


OpenGL::~OpenGL() {
}

void OpenGL::DeleteMeshBuffers() {
    if (vbo_triangles>0) {
        glDeleteBuffers(1,&vbo_triangles);
        vbo_triangles = 0;
    }
    if (ibo_triangles>0) {
        glDeleteBuffers(1,&ibo_triangles);
        ibo_triangles = 0;
    }
    if (va_triangles>0) {
        glDeleteVertexArrays(1,&va_triangles);
        va_triangles = 0;
    }
}

// ************************************ public methods ********************************
void OpenGL::AddObjectsToScriptEngine( QScriptEngine* engine ) {
    QScriptValue ogl = engine->newQObject(this);
    engine->globalObject().setProperty("OGL",ogl);
}

void OpenGL::GenMeshBuffers() {
    DeleteMeshBuffers();

    glGenVertexArrays(1,&va_triangles);
    glGenBuffers(1,&vbo_triangles);
    glGenBuffers(1,&ibo_triangles);

#ifdef BE_VERBOSE
    fprintf(stderr,"GenMeshBuffers...\n#meshVertices %4d\n#triangles: %4d\n#nodesPT: %d\n",mData->numMeshVertices,mData->numTriangles,mData->numNodesPerTriangle);
    for(int i=0; i<mData->numMeshVertices; i++) {
        fprintf(stderr,"%3d  %8.4f %8.4f\n",mData->mMeshVerts[3*i+0],mData->mMeshVerts[3*i+1]);
    }
    fprintf(stderr,"\n");

    for(int i=0; i<mData->numTriangles; i++) {
        fprintf(stderr,"%2d %2d %2d %2d %2d %2d\n",mData->mMeshIndices[6*i+0],
                mData->mMeshIndices[6*i+1],
                mData->mMeshIndices[6*i+2],
                mData->mMeshIndices[6*i+3],
                mData->mMeshIndices[6*i+4],
                mData->mMeshIndices[6*i+5]);
    }
#endif // BE_VERBOSE

    glBindVertexArray(va_triangles);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_triangles);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*3*mData->numMeshVertices,mData->mMeshVerts,GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo_triangles);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*mData->numTriangles*mData->numNodesPerTriangle,mData->mMeshIndices,GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void OpenGL::GenDataTexture() {
    if (texID>0) {
        glDeleteTextures(1,&texID);
        texID = 0;
    }
    if (mData->evals==NULL) {
        return;
    }
#ifdef BE_VERBOSE
    fprintf(stderr,"GenDataTexture...\n#vertices %4d\n#N %4d\n",mData->numMeshVertices,mData->N);
#endif // BE_VERBOSE

    glGenTextures(1,&texID);
    glBindTexture(GL_TEXTURE_2D,texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mData->numMeshVertices,mData->N, 0,GL_RED,GL_FLOAT,mData->evals);
    glBindTexture(GL_TEXTURE_2D,0);
}

void OpenGL::UpdateShaders() {
    removeShaders();
    createShaders();
}

void OpenGL::SetShaderProps() {
    mGridShader.SetTessShaderAvailability(mData->m_haveTessShader);
    mCtrlPointShader.SetTessShaderAvailability(mData->m_haveTessShader);
    mCtrlLineShader.SetTessShaderAvailability(mData->m_haveTessShader);
    mView2DShader.SetTessShaderAvailability(mData->m_haveTessShader);
    mView3DShader.SetTessShaderAvailability(mData->m_haveTessShader);
    mObjectShader.SetTessShaderAvailability(mData->m_haveTessShader);
}


// *********************************** public slots ******************************
void OpenGL::setCtrlPoints() {
    //fprintf(stderr,"Set control points.\n");
    if (m_controlVertices!=NULL) {
        delete [] m_controlVertices;
        m_controlVertices = NULL;
    }

    if (m_controlVertIdx!=NULL) {
        delete [] m_controlVertIdx;
        m_controlVertIdx = NULL;
    }

    int num = mData->m_vertices.size();
    if (num<=0) {
        updateGL();
        return;
    }
    m_controlVertices = new float[num*4];
    m_controlVertIdx  = new int[num*2];
    for(int i=0; i<num; i++) {
        m_controlVertices[4*i+0] = static_cast<float>(mData->m_vertices[i].pos.x);
        m_controlVertices[4*i+1] = static_cast<float>(mData->m_vertices[i].pos.y);
        m_controlVertices[4*i+2] = 0.0f;
        m_controlVertices[4*i+3] = 0.0f;
        m_controlVertIdx[2*i+0] = i;
        m_controlVertIdx[2*i+1] = static_cast<int>(mData->m_vertices[i].isFixed);
        //fprintf(stderr,"%10.0f %10.0f\n", mData->m_controlVertices[2*i+0],mData->m_controlVertices[2*i+1]);
    }
    glBindBuffer(GL_ARRAY_BUFFER,vbo_controlPoints);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*4*num,m_controlVertices);
    glBindBuffer(GL_ARRAY_BUFFER,ido_controlPoints);
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLint)*2*num,m_controlVertIdx);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    updateGL();
}


void OpenGL::setCtrlSegments() {
    if (m_controlSegments!=NULL) {
        delete [] m_controlSegments;
        m_controlSegments = NULL;
    }
    if (mData->m_segments.size()<=0) {
        updateGL();
        return;
    }
    m_controlSegments = new unsigned int[mData->m_segments.size()*2];
    for(int i=0; i<mData->m_segments.size(); i++) {
        m_controlSegments[2*i+0] = static_cast<unsigned int>(mData->m_segments[i].p1-1);
        m_controlSegments[2*i+1] = static_cast<unsigned int>(mData->m_segments[i].p2-1);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo_controlSegments);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,sizeof(GLuint)*2*mData->m_segments.size(),m_controlSegments);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    updateGL();
}

void OpenGL::timeStep() {
    mData->m_currTime += mData->m_time.elapsed()*1e-3;
    mData->m_time.restart();
    updateGL();
}

void OpenGL::setBGColor() {
    QColor bgColor = QColorDialog::getColor(mData->m_bgColor,this,"Set background color");
    if (bgColor.isValid()) {
        mData->m_bgColor = bgColor;
        glClearColor(mData->m_bgColor.redF(),mData->m_bgColor.greenF(),mData->m_bgColor.blueF(),1);
        updateGL();
    }
}

// *********************************** protected methods ******************************
void OpenGL::initializeGL() {
    static int mInitCounter = 0;
    if (mInitCounter>0) {
        return;
    }
    fprintf(stderr,"Initialize OpenGL...\n");
    if (gl3wInit()) {
        fprintf(stderr,"Error: Failed to initialize gl3w.\n");
        exit(1);
    }
    if (!gl3wIsSupported(3,3)) {
        fprintf(stderr,"Error: OpenGL 3.3 or higher is not supported.\n");
        exit(1);
    }

    mInitCounter++;
    /*
    if (!setSyncToVBlank(0)) {
        QMessageBox::warning(this,"Performance warning","SyncToVBlank could not be disabled. This might lead to high performance lost!");
    }
    */


    fprintf(stderr,"Basic information about graphics board:\n");
    fprintf(stderr,"\tVendor:             %s\n",glGetString( GL_VENDOR ));
    fprintf(stderr,"\tGPU:                %s\n",glGetString( GL_RENDERER ));
    fprintf(stderr,"\tOpenGL-Version:     %s\n",glGetString( GL_VERSION ));
    fprintf(stderr,"\tGLSL:               %s\n",glGetString( GL_SHADING_LANGUAGE_VERSION ));


    int numExt;
    glGetIntegerv( GL_NUM_EXTENSIONS, &numExt);
    for(int x=0; x<numExt; x++) {
        const GLubyte* extName = glGetStringi( GL_EXTENSIONS, x );
        //fprintf(stderr,"%s\n",extName);
        const char* ptrToExt = strstr(reinterpret_cast<const char*>(extName), "tessellation_shader");
        if (ptrToExt != NULL) {
            mData->m_haveTessShader = true;
        }
    }


#if 0
    int maxVSOutputComp;
    int maxGSOutputComp;
    glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS,&maxVSOutputComp);
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,&maxGSOutputComp);
    fprintf(stderr,"\tMax VS output components: %d\n",maxVSOutputComp);
    fprintf(stderr,"\tMax GS output components: %d\n",maxGSOutputComp);
#endif


    SetShaderProps();

    glClearColor(mData->m_bgColor.redF(),mData->m_bgColor.greenF(),mData->m_bgColor.blueF(),1);
    //glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ------------------------------------------
    //  generate vertex array for line drawing
    // ------------------------------------------
    float lineVerts[4];
    lineVerts[0] = 0.0; lineVerts[1] = 0.0;
    lineVerts[2] = 1.0; lineVerts[3] = 0.0;
    glGenVertexArrays(1,&va_line);
    glGenBuffers(1,&vbo_line);

    // ------------------------------------------
    //  generate vertex array for grid
    // ------------------------------------------
    glBindVertexArray(va_line);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_line);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*2*2,&lineVerts[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,NULL);
    glBindVertexArray(0);

    // ------------------------------------------
    //  generate vertex array for control points
    // ------------------------------------------
    glGenVertexArrays(1,&va_controlPoints);
    glGenBuffers(1,&vbo_controlPoints);
    glGenBuffers(1,&ido_controlPoints);
    glGenBuffers(1,&ibo_controlSegments);

    glBindVertexArray(va_controlPoints);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_controlPoints);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*4*MAX_NUM_CTRL_POINTS, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,NULL);

    glBindBuffer(GL_ARRAY_BUFFER,ido_controlPoints);
    glEnableVertexAttribArray(1);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLint)*2*MAX_NUM_CTRL_POINTS, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribIPointer(1,2,GL_INT,0,NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo_controlSegments);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*2*MAX_NUM_CTRL_POINTS, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // GL_POINT_SPRITE, otherwise gl_PointCoord does not work
    //glEnable(0x8861);
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

    glGenVertexArrays(1,&va_box);
    glGenBuffers(1,&vbo_box);
    glGenBuffers(1,&ibo_box);

    glBindVertexArray(va_box);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_box);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*3*numBoxVerts,boxVerts,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo_box);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*2*numBoxEdges,boxEdges,GL_STATIC_DRAW);
    glBindVertexArray(0);

    mData->mCamera.SetSizeAndAspect(width(),height());
    mData->mCamera.Set(glm::vec3(init_proj_dist,0,0),glm::vec3(-1,0,0),glm::vec3(0,0,1));
    mData->mCamera.SetPOI(init_proj_poiX,init_proj_poiY,init_proj_poiZ);
    mData->mCamera.SetFovY(init_proj_fov);
    //mData->mCamera.MoveOnSphere(glm::radians(init_proj_theta),glm::radians(init_proj_phi));
    mData->mCamera.SetPhi(static_cast<float>(init_proj_phi));
    mData->mCamera.SetTheta(static_cast<float>(init_proj_theta));

    mData->m_maxPatchVertices = 0;
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &mData->m_maxPatchVertices);

    createShaders();
    emit haveOGLParams();
}

// ----------------------------------------------------------------
void OpenGL::paintGL() {
    glViewport( 0, 0, width(), height() );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    switch (mData->m_viewModus) {
        case e_viewInput: {
            if (mData->m_showGrid) {
                drawGrid();
            }
            if (vbo_controlPoints>0 && mData->m_vertices.size()>0) {
                drawCtrlLines();
                drawCtrlPoints();
            }
            break;
        }
        case e_view2D: {
            if (va_triangles>0 && mData->numMeshVertices>0) {
                draw2DView();
            }
            break;
        }
        case e_view3D: {
            if (va_box>0) {
                draw3DView();
            }
            break;
        }
    }

}

void OpenGL::resizeGL( int w, int h ) {
    glViewport(0,0,w,h);
    mData->mCamera.SetSizeAndAspect(w,h);
    mData->m_screenWidth  = w;
    mData->m_screenHeight = h;
    mData->m_aspect = mData->m_screenWidth/static_cast<double>(mData->m_screenHeight);

    if (mData->m_keepAspectRatio) {
        double centerX = 0.5*(mData->m_border.x+mData->m_border.y);
        double sizeY   = 0.5*(mData->m_border.w-mData->m_border.z);
        //std::cerr << sizeY << " " << aspect << std::endl;
        mData->m_border.x = centerX - sizeY*mData->m_aspect;
        mData->m_border.y = centerX + sizeY*mData->m_aspect;
        emit borderChanged();
    }
    mData->m_viewScale = glm::dvec2((mData->m_border.y - mData->m_border.x)/static_cast<double>(mData->m_screenWidth),
                                    (mData->m_border.w - mData->m_border.z)/static_cast<double>(mData->m_screenHeight) );
}

void OpenGL::keyPressEvent( QKeyEvent* event ) {
    mKeyPressed  = event->key();
    mKeyModifier = event->modifiers();
    switch (mKeyPressed)
    {
        case Qt::Key_Escape: {
            mSegSet = false;
            mSegP1 = mSegP2 = -1;
            mData->led_status->setText(QString());
            break;
        }
        case Qt::Key_R: {
            if (mData->m_viewModus==e_viewInput || mData->m_viewModus==e_view2D) {
                emit resetBorders();
            } else {
                mData->mCamera.SetPOI(0,0,0);
            }
            updateGL();
            break;
        }
        case Qt::Key_S: {
            UpdateShaders();
            updateGL();
            break;
        }
    }
    event->accept();
}

void OpenGL::keyReleaseEvent( QKeyEvent* event ) {
    mKeyPressed  = Qt::Key_No;
    mKeyModifier = Qt::Key_No;
    event->accept();
}

void OpenGL::mousePressEvent( QMouseEvent * event ) {
    mButtonPressed = event->button();
    mLastPos = event->pos();

    if (mButtonPressed==Qt::RightButton)
    {
        switch (mData->m_mouseCtrl) {
            case e_spMouse_standard: {
                break;
            }
            case e_spMouse_addMovePoint: {
                if (mData->m_activePoint<0) {
                    glm::dvec2 c;
                    mData->ScreenPosToCoords(event->pos(),c);
                    //std::cerr << "c: " << c.x << " " << c.y << std::endl;
                    emit addCtrlPoint(c.x,c.y);
                } else {
                    mData->m_trackPoint = mData->m_activePoint;
                }
                break;
            }
            case e_spMouse_delPoint: {
                emit delCtrlPoint(mData->m_activePoint);
                mData->m_activePoint = -1;
                break;
            }
            case e_spMouse_checkPoint: {
                emit checkCtrlPoint(mData->m_activePoint);
                break;
            }
            case e_spMouse_addSegment: {
                //std::cerr << mData->m_
                if (mData->m_activePoint>=0) {
                    if (!mSegSet) {
                        mSegP1 = mData->m_activePoint;
                        mSegSet = true;
                        mData->led_status->setText("Set second point for segment");
                    } else {
                        mSegP2 = mData->m_activePoint;
                        mSegSet = false;
                        if (mSegP1!=mSegP2) {
                            emit addCtrlSegment(mSegP1+1,mSegP2+1);
                        }
                        mData->led_status->setText(QString());
                    }
                }
                break;
            }
            case e_spMouse_addHole: {
                break;
            }
        }
    }

    event->accept();
    updateGL();
}

void OpenGL::mouseReleaseEvent( QMouseEvent * event ) {    
    mData->m_trackPoint = -1;
    mButtonPressed = Qt::NoButton;
    event->accept();
    updateGL();
}


void OpenGL::mouseMoveEvent( QMouseEvent * event ) {
    setFocus();
    QPoint d = event->pos() - mLastPos;

    switch (mButtonPressed)
    {
        default:
            break;
        case Qt::LeftButton: {
            if (mData->m_viewModus==e_viewInput || mData->m_viewModus==e_view2D) {
                if (mKeyModifier==Qt::SHIFT) {
                    double stepFactor = d.y()*0.01;
                    mData->m_border = mData->m_border + stepFactor*glm::dvec4(-mData->m_aspect,mData->m_aspect,-1,1);
                } else {
                    glm::dvec2 m = glm::dvec2(d.x(),d.y())*mData->m_viewScale;
                    mData->m_border.x -= m.x;
                    mData->m_border.y -= m.x;
                    mData->m_border.z += m.y;
                    mData->m_border.w += m.y;
                }
                mData->m_viewScale = glm::dvec2((mData->m_border.y - mData->m_border.x)/static_cast<float>(mData->m_screenWidth),
                                                (mData->m_border.w - mData->m_border.z)/static_cast<float>(mData->m_screenHeight) );
                emit borderChanged();
            } else {
                if (mKeyModifier==Qt::SHIFT) {
                    mData->mCamera.ChangeDistance(d.y()*mData->mCamera.GetDistFactor());
                } else {
                    mData->mCamera.MoveOnSphere(d.y()*mData->mCamera.GetRotFactor(),d.x()*mData->mCamera.GetRotFactor());
                }
                emit emitViewChanged();
            }
            updateGL();
            break;
        }
        case Qt::MidButton: {
            if (mData->m_viewModus==e_view3D) {
                if (mKeyModifier==Qt::SHIFT) {
                    mData->mCamera.MoveOnXY(d.y()*mData->mCamera.GetPanXYFactor(),-d.x()*mData->mCamera.GetPanXYFactor());
                } else {
                    mData->mCamera.MoveOnZ(d.y()*mData->mCamera.GetPanXYFactor());
                }
                emit emitViewChanged();
            }
            updateGL();
            break;
        }
        case Qt::RightButton: {
            break;
        }
    }

    if (mData->m_mouseCtrl == e_spMouse_addMovePoint || mData->m_mouseCtrl == e_spMouse_delPoint
            || mData->m_mouseCtrl == e_spMouse_checkPoint || mData->m_mouseCtrl == e_spMouse_addSegment)
    {
        if (mData->m_trackPoint>=0) {
            glm::dvec2 c;
            mData->ScreenPosToCoords(event->pos(),c);
            emit moveCtrlPoint(c.x,c.y);
        } else {
            mData->m_activePoint = findActivePoint(event->pos());
            emit setActivePoint(mData->m_activePoint);
        }
        updateGL();
    }
    if (mData->m_activeSegment.x>=0) {
        mData->m_activeSegment = glm::ivec2(-1,-1);
        emit setActiveSegment(-1,-1);
        updateGL();
    }

    if (mData->m_viewModus != e_view3D) {
        double mpX = event->pos().x()/static_cast<double>(mData->m_screenWidth)*(mData->m_border.y-mData->m_border.x) + mData->m_border.x;
        double mpY = (mData->m_screenHeight-event->pos().y())/static_cast<double>(mData->m_screenHeight)*(mData->m_border.w-mData->m_border.z) + mData->m_border.z;
        QString mPos = QString("x:%1   y:%2").arg(mpX,7,'f',FLOAT_PREC).arg(mpY,7,'f',FLOAT_PREC);
        mData->led_status->setText(mPos);
    }

    mLastPos = event->pos();
    event->accept();
}


void OpenGL::createShaders() {
    makeCurrent();

    // -----------------------
    //  Grid
    // -----------------------
#ifdef BE_VERBOSE
    fprintf(stderr,"OpenGL: Create grid shader with ...\n\t%s\n\t%s\n",
            mGridVertShaderName.toStdString().c_str(),
            mGridFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
    mGridShader.CreateProgramFromFile(mGridVertShaderName.toStdString().c_str(),
                                      mGridFragShaderName.toStdString().c_str());

    // -----------------------
    //  Control points
    // -----------------------
#ifdef BE_VERBOSE
    fprintf(stderr,"OpenGL: Create ctrlPoint shader with ...\n\t%s\n\t%s\n\t%s\n",
            mCtrlPointerVertShaderName.toStdString().c_str(),
            mCtrlPointerGeomShaderName.toStdString().c_str(),
            mCtrlPointerFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
    mCtrlPointShader.CreateProgramFromFile(mCtrlPointerVertShaderName.toStdString().c_str(),
                                           mCtrlPointerGeomShaderName.toStdString().c_str(),
                                           mCtrlPointerFragShaderName.toStdString().c_str());

    // -----------------------
    //  Control lines
    // -----------------------
#ifdef BE_VERBOSE
    fprintf(stderr,"OpenGL: Create ctrlLine shader with ...\n\t%s\n\t%s\n\t%s\n",
            mCtrlLineVertShaderName.toStdString().c_str(),
            mCtrlLineGeomShaderName.toStdString().c_str(),
            mCtrlLineFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
    mCtrlLineShader.CreateProgramFromFile(mCtrlLineVertShaderName.toStdString().c_str(),
                                          mCtrlLineGeomShaderName.toStdString().c_str(),
                                          mCtrlLineFragShaderName.toStdString().c_str());

    // -----------------------
    //  Mesh for 2D
    // -----------------------
    if (mData->numNodesPerTriangle>3) {
#ifdef BE_VERBOSE
        fprintf(stderr,"OpenGL: Create view2dquad shader with ...\n\t%s\n\t%s\n\t%s\n",
                mView2DVertShaderName.toStdString().c_str(),
                mView2DGeomShaderNameQuad.toStdString().c_str(),
                mView2DFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
        mView2DShader.CreateProgramFromFile(mView2DVertShaderName.toStdString().c_str(),
                                            mView2DGeomShaderNameQuad.toStdString().c_str(),
                                            mView2DFragShaderName.toStdString().c_str());
    } else {
#ifdef BE_VERBOSE
        fprintf(stderr,"OpenGL: Create view2d shader with ...\n\t%s\n\t%s\n\t%s\n",
                mView2DVertShaderName.toStdString().c_str(),
                mView2DGeomShaderName.toStdString().c_str(),
                mView2DFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
        mView2DShader.CreateProgramFromFile(mView2DVertShaderName.toStdString().c_str(),
                                            mView2DGeomShaderName.toStdString().c_str(),
                                            mView2DFragShaderName.toStdString().c_str());
    }

    // -----------------------
    //  Mesh for 3D
    // -----------------------
    if (mData->m_haveTessShader) {
        if (mData->numNodesPerTriangle>3) {
#ifdef BE_VERBOSE
            fprintf(stderr,"OpenGL: Create view3dquad shader with ...\n\t%s\n\t%s\n\t%s\n\t%s\n",
                    mView3DVertShaderName.toStdString().c_str(),
                    mView3DTessCtrlShaderNameQuad.toStdString().c_str(),
                    mView3DTessEvalShaderName.toStdString().c_str(),
                    mView3DFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
            mView3DShader.CreateProgramFromFile(mView3DVertShaderName.toStdString().c_str(),
                                                mView3DTessCtrlShaderNameQuad.toStdString().c_str(),
                                                mView3DTessEvalShaderName.toStdString().c_str(),
                                                mView3DFragShaderName.toStdString().c_str());
        } else {
#ifdef BE_VERBOSE
            fprintf(stderr,"OpenGL: Create view3d shader with ...\n\t%s\n\t%s\n\t%s\n\t%s\n",
                    mView3DVertShaderName.toStdString().c_str(),
                    mView3DTessCtrlShaderName.toStdString().c_str(),
                    mView3DTessEvalShaderName.toStdString().c_str(),
                    mView3DFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
            mView3DShader.CreateProgramFromFile(mView3DVertShaderName.toStdString().c_str(),
                                                mView3DTessCtrlShaderName.toStdString().c_str(),
                                                mView3DTessEvalShaderName.toStdString().c_str(),
                                                mView3DFragShaderName.toStdString().c_str());
        }
    } else {
        if (mData->numNodesPerTriangle>3) {
#ifdef BE_VERBOSE
            fprintf(stderr,"OpenGL: Create view3dquad shader with ...\n\t%s\n\t%s\n\t%s\n",
                    mView3DVertShaderName.toStdString().c_str(),
                    mView3DGeomShaderNameQuad.toStdString().c_str(),
                    mView3DFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
            mView3DShader.CreateProgramFromFile(mView3DVertShaderName.toStdString().c_str(),
                                                mView3DGeomShaderNameQuad.toStdString().c_str(),
                                                mView3DFragShaderName.toStdString().c_str());
        } else {
#ifdef BE_VERBOSE
            fprintf(stderr,"OpenGL: Create view3d shader with ...\n\t%s\n\t%s\n\t%s\n",
                    mView3DVertShaderName.toStdString().c_str(),
                    mView3DGeomShaderName.toStdString().c_str(),
                    mView3DFragShaderName.toStdString().c_str());
#endif // BE_VERBOSE
            mView3DShader.CreateProgramFromFile(mView3DVertShaderName.toStdString().c_str(),
                                                mView3DGeomShaderName.toStdString().c_str(),
                                                mView3DFragShaderName.toStdString().c_str());
        }
    }

    mObjectShader.CreateProgramFromFile(mObjectVertShaderName.toStdString().c_str(),
                                        mObjectFragShaderName.toStdString().c_str());
}


void OpenGL::removeShaders() {
    mGridShader.RemoveAllShaders();
    mCtrlPointShader.RemoveAllShaders();
    mCtrlLineShader.RemoveAllShaders();
    mView2DShader.RemoveAllShaders();
    mView3DShader.RemoveAllShaders();
    mObjectShader.RemoveAllShaders();
}


void OpenGL::drawGrid() {
    mGridShader.Bind();
    projMX = glm::ortho(mData->m_border.x,mData->m_border.y,mData->m_border.z,mData->m_border.w);

    glUniformMatrix4fv( mGridShader.GetUniformLocation("pvm"), 1, GL_FALSE, glm::value_ptr(projMX));
    glUniform4f(mGridShader.GetUniformLocation("border"),mData->m_border.x,mData->m_border.y,mData->m_border.z,mData->m_border.w);
    glUniform2f(mGridShader.GetUniformLocation("gridOrigin"),mData->m_gridOrig.x,mData->m_gridOrig.y);
    glUniform2f(mGridShader.GetUniformLocation("gridStep"),mData->m_gridStep.x,mData->m_gridStep.y);

    glBindVertexArray(va_line);

    glUniform1i(mGridShader.GetUniformLocation("tickType"),1);
    glDrawArrays(GL_LINES,0,2);
    glUniform1i(mGridShader.GetUniformLocation("tickType"),2);
    glDrawArrays(GL_LINES,0,2);

    int numXr = int(ceil((mData->m_border.w-mData->m_gridOrig.y)/mData->m_gridStep.y));
    int numXl = int(ceil((mData->m_gridOrig.y-mData->m_border.z)/mData->m_gridStep.y));
    int numYt = int(ceil((mData->m_border.y-mData->m_gridOrig.x)/mData->m_gridStep.x));
    int numYb = int(ceil((mData->m_gridOrig.x-mData->m_border.x)/mData->m_gridStep.x));

    glUniform1i(mGridShader.GetUniformLocation("tickType"),3);
    glDrawArraysInstanced(GL_LINES,0,2,numXr);
    glUniform1i(mGridShader.GetUniformLocation("tickType"),4);
    glDrawArraysInstanced(GL_LINES,0,2,numXl);
    glUniform1i(mGridShader.GetUniformLocation("tickType"),5);
    glDrawArraysInstanced(GL_LINES,0,2,numYt);
    glUniform1i(mGridShader.GetUniformLocation("tickType"),6);
    glDrawArraysInstanced(GL_LINES,0,2,numYb);

    glBindVertexArray(0);
    mGridShader.Release();
}

void OpenGL::drawCtrlPoints() {
    mCtrlPointShader.Bind();
    glUniformMatrix4fv( mCtrlPointShader.GetUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(projMX) );

    glUniform2f( mCtrlPointShader.GetUniformLocation("scale"), static_cast<float>(mData->m_viewScale.x),
                                                               static_cast<float>(mData->m_viewScale.y) );
    glUniform1f( mCtrlPointShader.GetUniformLocation("pointSize"), mData->m_pointSize );
    glUniform1i( mCtrlPointShader.GetUniformLocation("activePoint"),mData->m_activePoint);

    glUniform3f( mCtrlPointShader.GetUniformLocation("pointColor"),
                 mData->m_pointColor.redF(),
                 mData->m_pointColor.greenF(),
                 mData->m_pointColor.blueF() );

    glUniform3f( mCtrlPointShader.GetUniformLocation("pointColorHighlighted"),
                 mData->m_pointColorHighlighted.redF(),
                 mData->m_pointColorHighlighted.greenF(),
                 mData->m_pointColorHighlighted.blueF() );

    glBindVertexArray(va_controlPoints);
    glDrawArrays( GL_POINTS, 0, mData->m_vertices.size());
    glBindVertexArray(0);
    mCtrlPointShader.Release();
}


void OpenGL::drawCtrlLines() {
    mCtrlLineShader.Bind();
    glUniformMatrix4fv( mCtrlLineShader.GetUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(projMX) );
    glUniform2i( mCtrlLineShader.GetUniformLocation("activeSegment"), mData->m_activeSegment.x-1, mData->m_activeSegment.y-1 );

    glUniform3f( mCtrlLineShader.GetUniformLocation("lineColor"),
                 mData->m_lineColor.redF(),
                 mData->m_lineColor.greenF(),
                 mData->m_lineColor.blueF() );

    glUniform3f( mCtrlLineShader.GetUniformLocation("lineColorHighlighted"),
                 mData->m_lineColorHighlighted.redF(),
                 mData->m_lineColorHighlighted.greenF(),
                 mData->m_lineColorHighlighted.blueF() );

    glBindVertexArray(va_controlPoints);
    glDrawElements(GL_LINES,mData->m_segments.size()*2,GL_UNSIGNED_INT,NULL);
    glBindVertexArray(0);
    mCtrlLineShader.Release();
}


void OpenGL::draw2DView() {
    if (va_triangles<=0) {
        return;
    }

    float cosWT = static_cast<float>(cos(2.0*M_PI*mData->m_freq*mData->m_currTime));

    mView2DShader.Bind();
    projMX = glm::ortho(mData->m_border.x,mData->m_border.y,mData->m_border.z,mData->m_border.w);
    glUniformMatrix4fv(mView2DShader.GetUniformLocation("mvp"),1,GL_FALSE,glm::value_ptr(projMX));
    glUniform2f(mView2DShader.GetUniformLocation("winSize"),width(),height());
    glUniform1i(mView2DShader.GetUniformLocation("currEV"),mData->m_currEV);
    glUniform1i(mView2DShader.GetUniformLocation("numNodesPerTriangle"),mData->numNodesPerTriangle);
    glUniform1f(mView2DShader.GetUniformLocation("cosWT"),cosWT);
    glUniform1f(mView2DShader.GetUniformLocation("meshOpacity"),mData->m_meshOpacity);
    glUniform1i(mView2DShader.GetUniformLocation("useColor"),static_cast<int>(mData->m_useColor));
    glUniform1i(mView2DShader.GetUniformLocation("useGridOnly"),static_cast<int>(mData->m_useMeshOnly));
    glUniform1i(mView2DShader.GetUniformLocation("showIsolines"),static_cast<int>(mData->m_showIsolines));
    glUniform1f(mView2DShader.GetUniformLocation("scaleFactor"),static_cast<float>(mData->m_scaleFactor));

    if (texID>0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texID);
        glUniform1i(mView2DShader.GetUniformLocation("tex"),0);
    }

    glBindVertexArray(va_triangles);
    if (mData->numNodesPerTriangle==3) {
        glDrawElements(GL_TRIANGLES,mData->numTriangles*mData->numNodesPerTriangle,GL_UNSIGNED_INT,NULL);
    } else {
        glDrawElements(GL_TRIANGLES_ADJACENCY,mData->numTriangles*mData->numNodesPerTriangle,GL_UNSIGNED_INT,NULL);
    }
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D,0);
    mView2DShader.Release();
}


void OpenGL::draw3DView() {
    glEnable(GL_DEPTH_TEST);

    if (mData->m_showBox) {
        mObjectShader.Bind();
        glUniformMatrix4fv( mObjectShader.GetUniformLocation("proj_matrix"), 1, GL_FALSE, mData->mCamera.ProjMatrixPtr());
        glUniformMatrix4fv( mObjectShader.GetUniformLocation("view_matrix"), 1, GL_FALSE, mData->mCamera.ViewMatrixPtr());
        glUniform3f( mObjectShader.GetUniformLocation("translate"),-0.5f,-0.5f,-0.5f);
        glBindVertexArray(va_box);
        glDrawElements(GL_LINES,2*numBoxEdges,GL_UNSIGNED_INT,NULL);
        glBindVertexArray(0);
        mObjectShader.Release();
    }

    if (va_triangles<=0) {
        return;
    }

    if (mData->m_wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }

    float cosWT = static_cast<float>(cos(2.0*M_PI*mData->m_freq*mData->m_currTime));

    mView3DShader.Bind();
    glm::vec3 camPos = mData->mCamera.GetEyePos();
    glUniform3f(mView3DShader.GetUniformLocation("camPos"),camPos.x,camPos.y,camPos.z);
    glUniformMatrix4fv( mView3DShader.GetUniformLocation("proj_matrix"), 1, GL_FALSE, mData->mCamera.ProjMatrixPtr());
    glUniformMatrix4fv( mView3DShader.GetUniformLocation("view_matrix"), 1, GL_FALSE, mData->mCamera.ViewMatrixPtr());
    glUniform1i(mView3DShader.GetUniformLocation("numNodesPerTriangle"),mData->numNodesPerTriangle);
    glUniform1i(mView3DShader.GetUniformLocation("currEV"),mData->m_currEV);
    glUniform1f(mView3DShader.GetUniformLocation("cosWT"),cosWT);
    glUniform1f(mView3DShader.GetUniformLocation("scaleFactor"),static_cast<float>(mData->m_scaleFactor));
    glUniform1i(mView3DShader.GetUniformLocation("useDotProd"),static_cast<int>(mData->m_useDotProd));
    glUniform1i(mView3DShader.GetUniformLocation("whichShading"),static_cast<int>(mData->m_shading));
    glUniform1i(mView3DShader.GetUniformLocation("wireframe"),static_cast<int>(mData->m_wireframe));

    if (texID>0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texID);
        glUniform1i(mView3DShader.GetUniformLocation("tex"),0);
    }

    if (mData->m_haveTessShader) {
        glUniform1i(mView3DShader.GetUniformLocation("haveTessShader"),1);
        glUniform1i(mView3DShader.GetUniformLocation("tessLevelInner"),mData->m_tessLevelInner);
        glUniform1i(mView3DShader.GetUniformLocation("tessLevelOuter"),mData->m_tessLevelOuter);
    } else {
        glUniform1i(mView3DShader.GetUniformLocation("haveTessShader"),0);
    }

    glBindVertexArray(va_triangles);
    if (mData->m_haveTessShader) {
        glPatchParameteri(GL_PATCH_VERTICES, mData->numNodesPerTriangle);
        glDrawElements(GL_PATCHES,mData->numTriangles*mData->numNodesPerTriangle,GL_UNSIGNED_INT,NULL);
    } else {
        if (mData->numNodesPerTriangle==3) {
            glDrawElements(GL_TRIANGLES,mData->numTriangles*mData->numNodesPerTriangle,GL_UNSIGNED_INT,NULL);
        } else {
            glDrawElements(GL_TRIANGLES_ADJACENCY,mData->numTriangles*mData->numNodesPerTriangle,GL_UNSIGNED_INT,NULL);
        }
    }
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D,0);
    mView3DShader.Release();

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glDisable(GL_DEPTH_TEST);
}


int OpenGL::findActivePoint( QPoint mousePos ) {
    int num = -1;

    glm::dvec2 pos;
    mData->ScreenPosToCoords(mousePos,pos);

    double val = mData->m_pointSize * glm::length(mData->m_viewScale)*0.5;
    double dist;
    for(int i=0; i<mData->m_vertices.size(); i++) {
        dist = glm::length(pos - mData->m_vertices[i].pos);
        if (dist<val) {
            num = i;
            break;
        }
        //std::cerr << dist << " " << num << std::endl;
    }
    return num;
}

