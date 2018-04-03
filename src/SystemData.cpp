/**
    @file   SystemData.cpp

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

#include <locale>
#include <limits>

#include <QTextStream>
#include <QMessageBox>

#include "SystemData.h"

extern "C" {
#include "triangle.h"
}

SystemData::SystemData() {    
    m_screenWidth  = DEF_OGL_WIDTH;
    m_screenHeight = DEF_OGL_HEIGHT;
    m_aspect       = 1.0;
    m_keepAspectRatio = true;
    led_status = new QLineEdit();
    lcd_numMeshVertices = new QLCDNumber();
    lcd_numMeshVertices->setSegmentStyle(QLCDNumber::Flat);
    lcd_numTriangles = new QLCDNumber();
    lcd_numTriangles->setSegmentStyle(QLCDNumber::Flat);

    m_viewModus = e_viewInput;
    m_bgColor               = init_bg_color;
    m_pointColor            = init_point_color;
    m_pointColorHighlighted = init_point_color_highlight;
    m_lineColor             = init_line_color;
    m_lineColorHighlighted  = init_line_color_highlight;

    m_gridOrig = glm::dvec2(init_grid_origX,init_grid_origY);
    m_gridStep = glm::dvec2(init_grid_stepX,init_grid_stepY);
    m_border   = glm::dvec4(init_ortho_left,init_ortho_right,init_ortho_bottom,init_ortho_top);
    m_showGrid = true;
    m_showBox = false;
    m_wireframe = false;
    m_haveTessShader = false;

    m_mouseCtrl   = e_spMouse_standard;
    m_activePoint = -1;
    m_trackPoint  = -1;
    m_pointSize   =  5.0;
    m_viewScale   = glm::dvec2((m_border.y - m_border.x)/static_cast<float>(m_screenWidth),
                               (m_border.w - m_border.z)/static_cast<float>(m_screenHeight) );
    m_activeSegment = glm::ivec2(-1,-1);

    numVertices = 0;
    numDim     = 2;
    numAttribs = 0;    
    numBMarker = 1;
    numSegments = 0;
    numSegBMarker = 1;
    numHoles = 0;
    numTriangles = 0;
    numNodesPerTriangle = 3;
    numTriAttribs = 0;

#ifdef USE_EXTERN_TRI
    m_idxOffset = 1;
#else
    m_idxOffset = 0;
#endif

    m_currEV   = 0;
    m_freq     = init_freq;
    m_currTime = 0.0;
    m_time.start();
    m_scaleFactor = 1.0;
    m_meshOpacity = 0.3f;
    m_useColor    = false;
    m_useMeshOnly = false;
    m_showIsolines = false;
    m_tessLevelInner = 4;
    m_tessLevelOuter = 4;
    m_maxPatchVertices = 0;
    m_shading    = e_shd_graysign;
    m_useDotProd = true;

    m_maxArea  = init_max_area;
    m_minAngle = init_min_angle;
    m_useQuad  = true;
    m_useDelaunay    = false;
    m_useConvexHull  = false;
    m_elastSupported = false;
    m_eigenvalues = NULL;

    N = 0;
    mMeshVerts   = NULL;
    mMeshIndices = NULL;
    evals = NULL;
    S1 = S2 = S3 = S4 = S5l = S5q = NULL;
#ifdef HAVE_GSL
    Me = aS1 = bS2 = cS3 = NULL;
#endif
    s1 = NULL;
    Stot = Mtot = NULL;
}


SystemData::~SystemData() {
    m_timer->stop();
    delete m_timer;
    ClearAll();
}

void SystemData::ClearAll() {
    if (!m_vertices.empty()) {
        m_vertices.clear();
    }
    if (!m_segments.empty()) {
        m_segments.clear();
    }
    if (!m_holes.empty()) {
        m_holes.clear();
    }
    if (!mesh_vertices.empty()) {
        mesh_vertices.clear();
    }
    if (!mesh_triIndices.empty()) {
        mesh_triIndices.clear();
    }

    if (mMeshVerts!=NULL) {
        delete [] mMeshVerts;
        mMeshVerts = NULL;
    }
    if (mMeshIndices!=NULL) {
        delete [] mMeshIndices;
        mMeshIndices = NULL;
    }
    if (evals!=NULL) {
        delete [] evals;
        evals = NULL;
    }
}

// ********************************** public methods *****************************

void freeSpace( void* data ) {
    if (data!=NULL) {
        //std::cerr << data << std::endl;
        trifree(data);
        data = NULL;
    } else {
        //fprintf(stderr,"is null\n");
    }
}


bool SystemData::DoTriangulation(const char *triswitches) {
    if (m_vertices.size()<3) {
        return false;
    }
    fprintf(stderr,"Triangulate...\n");

    //const char *triswitches = "zYYpa0.1";
    const unsigned int length = 256;
    char refparams[length];
#if defined _WIN32 && !defined __MINGW32__
    sprintf_s(refparams, length, "%s", triswitches);
#else
    sprintf(refparams,"%s",triswitches);
#endif

    struct triangulateio in, out;

    in.numberofpoints   = m_vertices.size();
    in.numberofsegments = m_segments.size();
    in.numberofholes    = m_holes.size();
    in.numberofpointattributes = 0;

    in.pointlist          = (REAL*)malloc(in.numberofpoints*2*sizeof(REAL));
    in.pointattributelist = (REAL*)NULL;
    in.pointmarkerlist    = (int*)malloc(in.numberofpoints*sizeof(int));
    in.trianglearealist   = (REAL*)NULL;

    in.segmentlist        = (int*)malloc(in.numberofsegments*2*sizeof(int));
    in.segmentmarkerlist  = (int*)malloc(in.numberofsegments*sizeof(int));
    in.numberoftriangles  = 0;
    in.numberofcorners    = 3;
    in.numberoftriangleattributes = 0;
    in.trianglelist        = (int*)NULL;
    in.triangleattributelist = (REAL*)NULL;
    in.neighborlist        = (int*)NULL;
    in.holelist            = (REAL*)malloc(in.numberofholes*2*sizeof(REAL));
    in.numberofregions     = 0;
    in.regionlist          = (REAL*)NULL;

    out.pointlist = (REAL*)NULL;
    out.pointattributelist = (REAL*)NULL;
    out.pointmarkerlist = (int*)NULL;

    out.trianglelist = (int*)NULL;
    out.triangleattributelist = (REAL*)NULL;
    out.neighborlist = (int*)NULL;
    out.segmentlist = (int*)NULL;
    out.segmentmarkerlist = (int*)NULL;
    out.edgelist = (int*)NULL;
    out.edgemarkerlist = (int*)NULL;


    for(int i=0; i<m_vertices.size(); i++) {
        in.pointlist[2*i+0] = m_vertices[i].pos.x;
        in.pointlist[2*i+1] = m_vertices[i].pos.y;
        in.pointmarkerlist[i] = m_vertices[i].bmarker;
    }

    for(int i=0; i<m_segments.size(); i++) {
        in.segmentlist[2*i+0] = m_segments[i].p1-1;
        in.segmentlist[2*i+1] = m_segments[i].p2-1;
        //in.segmentmarkerlist[i] = m_segments[i].bmarker;
        if (m_vertices[m_segments[i].p1-1].isFixed && m_vertices[m_segments[i].p2-1].isFixed) {
            in.segmentmarkerlist[i] = BOUNDARY_FIXED_MARKER;
        } else {
            in.segmentmarkerlist[i] = 1;
        }
    }

    for(int i=0; i<m_holes.size(); i++) {
        in.holelist[2*i+0] = m_holes[i].pos.x;
        in.holelist[2*i+1] = m_holes[i].pos.y;
    }

    triangulate(refparams,&in,&out,(struct triangulateio*)NULL);

    numMeshVertices = out.numberofpoints;
    numMeshAttribs  = out.numberofpointattributes;
    numMeshBMarkers = 1;

    numTriangles = out.numberoftriangles;
    numNodesPerTriangle = out.numberofcorners;
    numTriAttribs = 0;

    if (!mesh_vertices.empty()) {
        mesh_vertices.clear();
    }
    if (!mesh_triIndices.empty()) {
        mesh_triIndices.clear();
    }

    for(int i=0; i<out.numberofpoints; i++) {
        node_t node = {i,glm::dvec2(out.pointlist[2*i+0],out.pointlist[2*i+1]),out.pointmarkerlist[i],false};
        mesh_vertices.push_back(node);
    }
    assert(out.numberofpoints == mesh_vertices.size());

    // TAKE CARE OF THE ORDER OF THE TRIANGLE VERTICES a !!!
    for(int i=0; i<out.numberoftriangles; i++) {
        triIdx_t triIndex;
        triIndex.v = glm::ivec3( out.trianglelist[numNodesPerTriangle*i+0], out.trianglelist[numNodesPerTriangle*i+1], out.trianglelist[numNodesPerTriangle*i+2] );
        if (numNodesPerTriangle>3) {
            triIndex.a = glm::ivec3( out.trianglelist[numNodesPerTriangle*i+5], out.trianglelist[numNodesPerTriangle*i+3], out.trianglelist[numNodesPerTriangle*i+4] );
        }
        mesh_triIndices.push_back(triIndex);
    }
    assert(out.numberoftriangles == mesh_triIndices.size());

    if (mMeshVerts!=NULL) {
        delete [] mMeshVerts;
    }
    if (mMeshIndices!=NULL) {
        delete [] mMeshIndices;
    }

    mMeshVerts = new float[mesh_vertices.size()*3];
    for(int i=0; i<mesh_vertices.size(); i++) {
        mMeshVerts[3*i+0] = static_cast<float>(mesh_vertices[i].pos.x);
        mMeshVerts[3*i+1] = static_cast<float>(mesh_vertices[i].pos.y);
        mMeshVerts[3*i+2] = static_cast<float>(i);
    }

    mMeshIndices = new unsigned int[mesh_triIndices.size()*numNodesPerTriangle];
    for(int i=0; i<mesh_triIndices.size(); i++) {
        mMeshIndices[numNodesPerTriangle*i+0] = static_cast<unsigned int>(mesh_triIndices[i].v.x - m_idxOffset);
        mMeshIndices[numNodesPerTriangle*i+1] = static_cast<unsigned int>(mesh_triIndices[i].v.y - m_idxOffset);
        mMeshIndices[numNodesPerTriangle*i+2] = static_cast<unsigned int>(mesh_triIndices[i].v.z - m_idxOffset);
        if (numNodesPerTriangle==6) {
            mMeshIndices[numNodesPerTriangle*i+3] = static_cast<unsigned int>(mesh_triIndices[i].a.x - m_idxOffset);
            mMeshIndices[numNodesPerTriangle*i+4] = static_cast<unsigned int>(mesh_triIndices[i].a.y - m_idxOffset);
            mMeshIndices[numNodesPerTriangle*i+5] = static_cast<unsigned int>(mesh_triIndices[i].a.z - m_idxOffset);
        }
    }
    //std::cerr << "hier: " << mesh_vertices.size() << " " << mesh_triIndices.size() << std::endl;

    freeSpace(in.pointlist);
    freeSpace(in.segmentlist);
    freeSpace(in.pointattributelist);
    freeSpace(in.pointmarkerlist);
    freeSpace(in.regionlist);
    freeSpace(in.segmentmarkerlist);
    freeSpace(in.holelist);
    freeSpace(in.neighborlist);
    freeSpace(in.trianglelist);
    freeSpace(in.triangleattributelist);

    freeSpace(out.pointlist);
    freeSpace(out.pointattributelist);
    freeSpace(out.pointmarkerlist);
    freeSpace(out.trianglelist);
    freeSpace(out.triangleattributelist);
    freeSpace(out.neighborlist);
    freeSpace(out.segmentlist);
    freeSpace(out.segmentmarkerlist);
    freeSpace(out.edgelist);
    freeSpace(out.edgemarkerlist);

    fprintf(stderr,"finished\n");
    return true;
}


bool SystemData::SavePoly( QString filename ) {
    setlocale(LC_NUMERIC, "C");

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
       fprintf(stderr,"Cannot open file %s for reading.\n",filename.toStdString().c_str());
       return false;
    }

    QTextStream out(&file);
    QString line;

    // ----------------------------
    //  write vertex header
    // ----------------------------
    out << "# vertices\n";
    out << m_vertices.size() << " " << numDim << " " << numAttribs << " " << numBMarker << endl;

    // ----------------------------
    //  write vertices
    // ----------------------------
    for(int i=0; i<m_vertices.size(); i++) {
        line = QString("%1 %2 %3 %4\n").arg(i+1,5)
                .arg(m_vertices[i].pos.x,8,'f',FLOAT_PREC)
                .arg(m_vertices[i].pos.y,8,'f',FLOAT_PREC)
                .arg(m_vertices[i].bmarker,5);
        out << line;
    }

    // ----------------------------
    //  write segment header
    // ----------------------------
    out << "# segments\n";
    out << m_segments.size() << " " << numSegBMarker << endl;

    // ----------------------------
    //  write segments
    // ----------------------------
    for(int i=0; i<m_segments.size(); i++) {
        int bm = 1; //m_segments[i].bmarker;
        if (m_vertices[m_segments[i].p1-1].isFixed && m_vertices[m_segments[i].p2-1].isFixed) {
            //bm = m_vertices[m_segments[i].p1].bmarker;
            bm = BOUNDARY_FIXED_MARKER;
        }
        line = QString("%1 %2 %3 %4\n").arg(i+1,5)
                .arg(m_segments[i].p1,4)
                .arg(m_segments[i].p2,4)
                .arg(bm,5);
        out << line;
    }

    // ----------------------------
    //  write hole header
    // ----------------------------
    out << "# holes\n";
    out << m_holes.size() << endl;
    for(int i=0; i<m_holes.size(); i++) {
        line = QString("%1 %2 %3\n").arg(i+1,5)
                .arg(m_holes[i].pos.x,8,'f',FLOAT_PREC)
                .arg(m_holes[i].pos.y,8,'f',FLOAT_PREC);
        out << line;
    }

    file.close();
    return true;
}


bool SystemData::ReadNodeAndEleFile( QString nodeFileName, QString eleFileName) {

    int numDim;  // must be 2

    if (!mesh_vertices.empty()) {
        mesh_vertices.clear();
    }

    if (!mesh_triIndices.empty()) {
        mesh_triIndices.clear();
    }

    char line[255];
    // ---------------------
    //   read node file
    // ---------------------
    FILE* fNodePtr;
#if defined(_WIN32) && !defined(__MINGW32__)
    fopen_s(&fNodePtr,nodeFileName.toStdString().c_str(),"rt");
#else
    fNodePtr = fopen(nodeFileName.toStdString().c_str(),"rt");
#endif
    if (fNodePtr==NULL) {
        fprintf(stderr,"Cannot open node file %s.\n",nodeFileName.toStdString().c_str());
        return false;
    }
    if (fgets(line,255,fNodePtr)!=NULL) {
#if defined(_WIN32) && !defined(__MINGW32__)
        sscanf_s(line,"%d %d %d %d",&numMeshVertices,&numDim,&numMeshAttribs,&numMeshBMarkers);
#else
        sscanf(line,"%d %d %d %d",&numMeshVertices,&numDim,&numMeshAttribs,&numMeshBMarkers);
#endif
    }
    fprintf(stderr,"%d %d %d %d\n",numMeshVertices,numDim,numMeshAttribs,numMeshBMarkers);

    while (fgets(line,255,fNodePtr)!=NULL) {
        int id,bm;
        double x,y;
        if (std::string(line).compare(0,1,"#")!=0) {
#if defined(_WIN32) && !defined(__MINGW32__)
            sscanf_s(line,"%d %lf %lf %d",&id,&x,&y,&bm);
#else
            sscanf(line,"%d %lf %lf %d",&id,&x,&y,&bm);
#endif
            node_t node = {id,glm::dvec2(x,y),bm,false};
            mesh_vertices.push_back(node);
        }
    }
    fclose(fNodePtr);
    if (static_cast<int>(mesh_vertices.size()) != numMeshVertices) {
        return false;
    }

    // ---------------------
    //   read ele file
    // ---------------------
    FILE* fElePtr;
#if defined(_WIN32) && !defined(__MINGW32__)
    fopen_s(&fElePtr,eleFileName.toStdString().c_str(),"rt");
#else
    fElePtr = fopen(eleFileName.toStdString().c_str(),"rt");
#endif
    if (fElePtr==NULL) {
        fprintf(stderr,"Cannot open ele file %s.\n",eleFileName.toStdString().c_str());
        return false;
    }
    if (fgets(line,255,fElePtr)!=NULL) {
#if defined(_WIN32) && !defined(__MINGW32__)
        sscanf_s(line,"%d %d %d",&numTriangles,&numNodesPerTriangle,&numTriAttribs);
#else
        sscanf(line,"%d %d %d",&numTriangles,&numNodesPerTriangle,&numTriAttribs);
#endif
    }
    fprintf(stderr,"%d %d %d\n",numTriangles,numNodesPerTriangle,numTriAttribs);
    while (fgets(line,255,fElePtr)!=NULL) {
        int id,p1,p2,p3,p4,p5,p6;
        triIdx_t triIndex;
        if (std::string(line).compare(0,1,"#")!=0) {
            if (numNodesPerTriangle==3) {
#if defined(_WIN32) && !defined(__MINGW32__)
                sscanf_s(line,"%d %d %d %d",&id,&p1,&p2,&p3);
#else
                sscanf(line,"%d %d %d %d",&id,&p1,&p2,&p3);
#endif
                triIndex.v = glm::ivec3(p1,p2,p3);
                triIndex.a = glm::ivec3(0,0,0);
            } else {
#if defined(_WIN32) && !defined(__MINGW32__)
                sscanf_s(line,"%d %d %d %d %d %d %d",&id,&p1,&p2,&p3,&p4,&p5,&p6);
#else
                sscanf(line,"%d %d %d %d %d %d %d",&id,&p1,&p2,&p3,&p4,&p5,&p6);
#endif
                triIndex.v = glm::ivec3(p1,p2,p3);
                triIndex.a = glm::ivec3(p6,p4,p5);
                //triIndex.a = glm::ivec3(p4,p5,p6);
            }
            mesh_triIndices.push_back(triIndex);
        }
    }
    fclose(fElePtr);
    if (static_cast<int>(mesh_triIndices.size()) != numTriangles) {
        return false;
    }


    if (mMeshVerts!=NULL) {
        delete [] mMeshVerts;
    }
    if (mMeshIndices!=NULL) {
        delete [] mMeshIndices;
    }
    mMeshVerts = new float[mesh_vertices.size()*3];
    for(int i=0; i<mesh_vertices.size(); i++) {
        mMeshVerts[3*i+0] = static_cast<float>(mesh_vertices[i].pos.x);
        mMeshVerts[3*i+1] = static_cast<float>(mesh_vertices[i].pos.y);
        mMeshVerts[3*i+2] = static_cast<float>(i);
    }

    mMeshIndices = new unsigned int[mesh_triIndices.size()*numNodesPerTriangle];
    for(int i=0; i<mesh_triIndices.size(); i++) {
        mMeshIndices[numNodesPerTriangle*i+0] = static_cast<unsigned int>(mesh_triIndices[i].v.x - m_idxOffset);
        mMeshIndices[numNodesPerTriangle*i+1] = static_cast<unsigned int>(mesh_triIndices[i].v.y - m_idxOffset);
        mMeshIndices[numNodesPerTriangle*i+2] = static_cast<unsigned int>(mesh_triIndices[i].v.z - m_idxOffset);
        //fprintf(stderr,"%3d %3d %3d  ",mesh_triIndices[i].v.x,mesh_triIndices[i].v.y,mesh_triIndices[i].v.z);
        if (numNodesPerTriangle==6) {
            mMeshIndices[numNodesPerTriangle*i+3] = static_cast<unsigned int>(mesh_triIndices[i].a.x - m_idxOffset);
            mMeshIndices[numNodesPerTriangle*i+4] = static_cast<unsigned int>(mesh_triIndices[i].a.y - m_idxOffset);
            mMeshIndices[numNodesPerTriangle*i+5] = static_cast<unsigned int>(mesh_triIndices[i].a.z - m_idxOffset);
            //fprintf(stderr,"%3d %3d %3d\n",mesh_triIndices[i].a.x,mesh_triIndices[i].a.y,mesh_triIndices[i].a.z);
        } else {
          //  fprintf(stderr,"\n");
        }
    }
    return true;
}


void SystemData::ScreenPosToCoords( const QPoint pos, glm::dvec2 &c ) {
    c = glm::dvec2(pos.x()/static_cast<double>(m_screenWidth)*(m_border.y-m_border.x) + m_border.x,
                   (m_screenHeight-pos.y())/static_cast<double>(m_screenHeight)*(m_border.w-m_border.z) + m_border.z);
}


void SystemData::AdjustBorder(glm::dvec2 mmX, glm::dvec2 mmY, glm::dvec2 center ) {
    double offset = 0.5;

    if (m_keepAspectRatio) {
        mmY += glm::dvec2(-offset,offset);
        m_border = glm::dvec4(mmY[0]*m_aspect,mmY[1]*m_aspect,mmY[0],mmY[1]);
    } else {
        m_border = glm::dvec4(mmX[0]-offset,mmX[1]+offset,mmY[0]-offset,mmY[1]+offset);
    }
    double width  = m_border.y - m_border.x;
    double height = m_border.w - m_border.z;
    m_border.x = center.x - width*0.5;
    m_border.y = center.x + width*0.5;
    m_border.z = center.y - height*0.5;
    m_border.w = center.y + height*0.5;
    m_viewScale = glm::dvec2(width/static_cast<double>(m_screenWidth),
                             height/static_cast<double>(m_screenHeight) );
}


void SystemData::calc_params( const glm::dvec2 v1, const glm::dvec2 v2, const glm::dvec2 v3,
                              double &a, double &b, double &c, double &J ) {

    J = (v2.x-v1.x)*(v3.y-v1.y) - (v3.x-v1.x)*(v2.y-v1.y);
    assert(J!=0.0);

    double edJ = 1.0/J;
    a =  ((v3.x-v1.x)*(v3.x-v1.x) + (v3.y-v1.y)*(v3.y-v1.y))*edJ;
    b = -((v3.x-v1.x)*(v2.x-v1.x) + (v3.y-v1.y)*(v2.y-v1.y))*edJ;
    c =  ((v2.x-v1.x)*(v2.x-v1.x) + (v2.y-v1.y)*(v2.y-v1.y))*edJ;
}


void SystemData::initMatrices( int MSize ) {
    fprintf(stderr,"Initialize %d x %d matrices ... ",MSize,MSize);

#ifdef HAVE_GSL
    if (S1!=NULL) {
        gsl_matrix_free(S1);
        gsl_matrix_free(S2);
        gsl_matrix_free(S3);
        gsl_matrix_free(S4);
        gsl_matrix_free(S5l);
        gsl_matrix_free(S5q);
        gsl_matrix_free(Me);
        gsl_matrix_free(Se);
        gsl_matrix_free(aS1);
        gsl_matrix_free(bS2);
        gsl_matrix_free(cS3);
        gsl_vector_free(s1);
        gsl_matrix_free(Stot);
        gsl_matrix_free(Mtot);
    }

    S1  = gsl_matrix_calloc(MSize,MSize);
    S2  = gsl_matrix_calloc(MSize,MSize);
    S3  = gsl_matrix_calloc(MSize,MSize);
    S4  = gsl_matrix_calloc(MSize,MSize);
    S5l = gsl_matrix_calloc(2,2);
    S5q = gsl_matrix_calloc(3,3);
    Me  = gsl_matrix_calloc(MSize,MSize);
    Se  = gsl_matrix_calloc(MSize,MSize);
    aS1 = gsl_matrix_calloc(MSize,MSize);
    bS2 = gsl_matrix_calloc(MSize,MSize);
    cS3 = gsl_matrix_calloc(MSize,MSize);
    s1  = gsl_vector_calloc(MSize);

    const double *ms1, *ms2, *ms3, *ms4, *vs1, *fac;
    if (MSize==3) {
        ms1 = ms1_lin;
        ms2 = ms2_lin;
        ms3 = ms3_lin;
        ms4 = ms4_lin;
        vs1 = vs1_lin;
        fac = fac_lin;
    } else {
        ms1 = ms1_quad;
        ms2 = ms2_quad;
        ms3 = ms3_quad;
        ms4 = ms4_quad;
        vs1 = vs1_quad;
        fac = fac_quad;
    }

    for(int y=0; y<MSize; y++) {
        for(int x=0; x<MSize; x++) {
            gsl_matrix_set(S1,x,y,ms1[y*MSize+x]);
            gsl_matrix_set(S2,x,y,ms2[y*MSize+x]);
            gsl_matrix_set(S3,x,y,ms3[y*MSize+x]);
            gsl_matrix_set(S4,x,y,ms4[y*MSize+x]);
        }
        gsl_vector_set(s1,y,vs1[y]);
    }

    gsl_matrix_scale(S1,fac[0]);
    gsl_matrix_scale(S2,fac[1]);
    gsl_matrix_scale(S3,fac[2]);
    gsl_matrix_scale(S4,fac[3]);
    gsl_vector_scale(s1,fac[4]);

    gsl_matrix_memcpy(Me,S4);
    Stot = gsl_matrix_calloc(numMeshVertices,numMeshVertices);
    Mtot = gsl_matrix_calloc(numMeshVertices,numMeshVertices);

    gsl_matrix_set(S5l,0,0,1.0/3.0); gsl_matrix_set(S5l,0,1,1.0/6.0);
    gsl_matrix_set(S5l,1,1,1.0/3.0); gsl_matrix_set(S5l,1,0,1.0/6.0);

    gsl_matrix_set(S5q,0,0,2.0);  gsl_matrix_set(S5q,0,1,1.0); gsl_matrix_set(S5q,0,2,-0.5);
    gsl_matrix_set(S5q,1,0,1.0);  gsl_matrix_set(S5q,1,1,8.0); gsl_matrix_set(S5q,1,2,1.0);
    gsl_matrix_set(S5q,2,0,-0.5); gsl_matrix_set(S5q,2,1,1.0); gsl_matrix_set(S5q,2,2,2.0);

    gsl_matrix_scale(S5q,1.0/15.0);

#elif defined HAVE_LAPACK || defined HAVE_MAGMA
    if (S1!=NULL) {
        free(S1);
        free(S2);
        free(S3);
        free(S4);
        free(S5l);
        free(S5q);
        free(s1);
        free(Stot);
        free(Mtot);
    }

    S1  = (double*)calloc(MSize*MSize,sizeof(double));
    S2  = (double*)calloc(MSize*MSize,sizeof(double));
    S3  = (double*)calloc(MSize*MSize,sizeof(double));
    S4  = (double*)calloc(MSize*MSize,sizeof(double));
    S5l = (double*)calloc(2*2,sizeof(double));
    S5q = (double*)calloc(3*3,sizeof(double));
    s1  = (double*)calloc(MSize,sizeof(double));
    Stot = (double*)calloc(numMeshVertices*numMeshVertices,sizeof(double));
    Mtot = (double*)calloc(numMeshVertices*numMeshVertices,sizeof(double));

    const double *ms1, *ms2, *ms3, *ms4, *vs1, *fac;
    if (MSize==3) {
        ms1 = ms1_lin;
        ms2 = ms2_lin;
        ms3 = ms3_lin;
        ms4 = ms4_lin;
        vs1 = vs1_lin;
        fac = fac_lin;
    } else {
        ms1 = ms1_quad;
        ms2 = ms2_quad;
        ms3 = ms3_quad;
        ms4 = ms4_quad;
        vs1 = vs1_quad;
        fac = fac_quad;
    }

    int pos;
    for(int y=0; y<MSize; y++) {
        for(int x=0; x<MSize; x++) {
            pos = y*MSize+x;
            S1[pos] = fac[0]*ms1[pos];
            S2[pos] = fac[1]*ms2[pos];
            S3[pos] = fac[2]*ms3[pos];
            S4[pos] = fac[3]*ms4[pos];
        }
        s1[y] = fac[4]*vs1[y];
    }

    S5l[0] = S5l[3] = 1.0/3.0;  S5l[1] = S5l[2] = 1.0/6.0;
    S5q[1] = S5q[3] = S5q[5] = S5q[7] = 1.0/15.0;
    S5q[0] = S5q[8] = 2.0/15.0; S5q[2] = S5q[6] = -1.0/30.0; S5q[4] = 8.0/15.0;
#endif
    fprintf(stderr,"done.\n");
}

#ifdef HAVE_GSL
gsl_matrix*  SystemData::deleteElement( gsl_matrix* src, int N, int row, int col ) {
    gsl_matrix* dst = gsl_matrix_alloc(N-1,N-1);
    double val;
    int ar,ac;
    for(int r=0; r<N-1; r++) {
        ar = r;
        if (r>=row) ar = r+1;
        for(int c=0; c<N-1; c++) {
            ac = c;
            if (ac>=col) ac = c+1;
            val = gsl_matrix_get(src,ar,ac);
            gsl_matrix_set(dst,r,c,val);
        }
    }
    gsl_matrix_free(src);

#elif defined HAVE_LAPACK || defined HAVE_MAGMA
double* SystemData::deleteElement(double *src, int N, int row, int col ) {
    double *dst = (double*)calloc((N-1)*(N-1),sizeof(double));
    double val;
    int ar,ac;
    for(int r=0; r<N-1; r++) {
        ar = r;
        if (r>=row) ar = r+1;
        for(int c=0; c<N-1; c++) {
            ac = c;
            if (ac>=col) ac = c+1;
            val = src[ar*N+ac];    //gsl_matrix_get(src,ar,ac);
            dst[r*(N-1)+c] = val;  //gsl_matrix_set(dst,r,c,val);
        }
    }
    free(src);
#endif

#if 0
    for(int j=0; j<8; j++) {
        for(int k=0; k<8; k++) {
            fprintf(stderr,"%6.3f ",gsl_matrix_get(dst,j,k));
        }
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
#endif
    return dst;
}


void SystemData::compileMatrices() {
    fprintf(stderr,"Compile matrices...\n");
    double a,b,c,J, l12,l23,l31;
    glm::dvec2 p1,p2,p3;
    int idx[6];

    for(int t=0; t<numTriangles; t++) {
        idx[0] = mesh_triIndices[t].v.x - m_idxOffset;
        idx[1] = mesh_triIndices[t].v.y - m_idxOffset;
        idx[2] = mesh_triIndices[t].v.z - m_idxOffset;
        p1 = mesh_vertices[idx[0]].pos;
        p2 = mesh_vertices[idx[1]].pos;
        p3 = mesh_vertices[idx[2]].pos;
        calc_params(p1,p2,p3,a,b,c,J);

        l12 = glm::length(p2-p1);
        l23 = glm::length(p3-p2);
        l31 = glm::length(p1-p3);

        if (numNodesPerTriangle==6) {
            idx[3] = mesh_triIndices[t].a.x - m_idxOffset;
            idx[4] = mesh_triIndices[t].a.y - m_idxOffset;
            idx[5] = mesh_triIndices[t].a.z - m_idxOffset;
        }

#ifdef HAVE_GSL
        gsl_matrix_memcpy(Me,S4);
        gsl_matrix_scale(Me,J);

        gsl_matrix_memcpy(aS1,S1);
        gsl_matrix_scale(aS1,a);

        gsl_matrix_memcpy(bS2,S2);
        gsl_matrix_scale(bS2,b);

        gsl_matrix_memcpy(cS3,S3);
        gsl_matrix_scale(cS3,c);

        gsl_matrix_memcpy(Se,aS1);
        gsl_matrix_add(Se,bS2);
        gsl_matrix_add(Se,cS3);

        for(int j=0; j<numNodesPerTriangle; j++) {
            for(int k=0; k<numNodesPerTriangle; k++) {
                gsl_matrix_set(Stot,idx[j],idx[k], gsl_matrix_get(Stot,idx[j],idx[k]) + gsl_matrix_get(Se,j,k));
                gsl_matrix_set(Mtot,idx[j],idx[k], gsl_matrix_get(Mtot,idx[j],idx[k]) + gsl_matrix_get(Me,j,k));
            }
        }
#elif defined HAVE_LAPACK || defined HAVE_MAGMA
        int pos;
        int MSize = numNodesPerTriangle;
        for(int j=0; j<MSize; j++) {
            for(int k=0; k<MSize; k++) {
                pos = idx[j]*numMeshVertices + idx[k];
                Stot[pos] += a*S1[j*MSize+k] + b*S2[j*MSize+k] + c*S3[j*MSize+k];
                Mtot[pos] += J*S4[j*MSize+k];
            }
        }

        if (m_elastSupported) {
            // if boundary curves are elastically supported, then also
            // use boundary integral
            if (numNodesPerTriangle==3) {
                if (mesh_vertices[idx[0]].bmarker==1 && mesh_vertices[idx[1]].bmarker==1) {
                    int mi[2] = {0,1};
                    for(int y=0; y<2; y++) {
                        for(int x=0; x<2; x++) {
                            pos = idx[mi[y]]*numMeshVertices + idx[mi[x]];
                            Stot[pos] += l12*S5l[y*2+x];
                        }
                    }
                    //pos = idx[0]*numMeshVertices + idx[0];  Stot[pos] += l12*S5l[0];
                    //pos = idx[0]*numMeshVertices + idx[1];  Stot[pos] += l12*S5l[1];
                    //pos = idx[1]*numMeshVertices + idx[0];  Stot[pos] += l12*S5l[2];
                    //pos = idx[1]*numMeshVertices + idx[1];  Stot[pos] += l12*S5l[3];
                }
                if (mesh_vertices[idx[1]].bmarker==1 && mesh_vertices[idx[2]].bmarker==1) {
                    int mi[2] = {1,2};
                    for(int y=0; y<2; y++) {
                        for(int x=0; x<2; x++) {
                            pos = idx[mi[y]]*numMeshVertices + idx[mi[x]];
                            Stot[pos] += l23*S5l[y*2+x];
                        }
                    }
                }
                if (mesh_vertices[idx[2]].bmarker==1 && mesh_vertices[idx[0]].bmarker==1) {
                    int mi[2] = {2,0};
                    for(int y=0; y<2; y++) {
                        for(int x=0; x<2; x++) {
                            pos = idx[mi[y]]*numMeshVertices + idx[mi[x]];
                            Stot[pos] += l31*S5l[y*2+x];
                        }
                    }
                }
            } else {
                if (mesh_vertices[idx[0]].bmarker==1 &&
                        mesh_vertices[idx[1]].bmarker==1 &&
                        mesh_vertices[idx[3]].bmarker==1) {
                    int mi[3] = {0,3,1};
                    for(int y=0; y<3; y++) {
                        for(int x=0; x<3; x++) {
                            pos = idx[mi[y]]*numMeshVertices + idx[mi[x]];
                            Stot[pos] += l12*S5q[y*3+x];
                        }
                    }
                }
                if (mesh_vertices[idx[1]].bmarker==1 &&
                        mesh_vertices[idx[2]].bmarker==1 &&
                        mesh_vertices[idx[4]].bmarker==1) {
                    int mi[3] = {1,4,2};
                    for(int y=0; y<3; y++) {
                        for(int x=0; x<3; x++) {
                            pos = idx[mi[y]]*numMeshVertices + idx[mi[x]];
                            Stot[pos] += l23*S5q[y*3+x];
                        }
                    }
                }
                if (mesh_vertices[idx[2]].bmarker==1 &&
                        mesh_vertices[idx[0]].bmarker==1 &&
                        mesh_vertices[idx[5]].bmarker==1) {
                    int mi[3] = {2,5,0};
                    for(int y=0; y<3; y++) {
                        for(int x=0; x<3; x++) {
                            pos = idx[mi[y]]*numMeshVertices + idx[mi[x]];
                            Stot[pos] += l31*S5q[y*3+x];
                        }
                    }
                }
            }
        }

#endif
    }
}


// http://www.gnu.org/software/gsl/manual/html_node/Eigensystems.html
//
void SystemData::SolveSystem() {
    initMatrices(numNodesPerTriangle);
    compileMatrices();

    N = mesh_vertices.size();
    for(int i=mesh_vertices.size()-1; i>=0; i--) {
        if (mesh_vertices[i].bmarker==BOUNDARY_FIXED_MARKER) {
            //std::cerr << "del " << i << std::endl;
            Stot = deleteElement(Stot,N,i,i);
            Mtot = deleteElement(Mtot,N,i,i);
            mMeshVerts[3*i+2] = -10.0f;  // fixed point
            N -= 1;
        }
    }
    //exportSMmatrices(QString("sm_matrices.bin"));

    fprintf(stderr,"Solve system...\n");

    if (m_eigenvalues!=NULL) {
        delete [] m_eigenvalues;
    }
    if (evals!=NULL) {
        delete [] evals;
    }

    evals = new float[N*numMeshVertices];
    for(int n=0; n<N; n++) {
        for(int i=0; i<numMeshVertices; i++) {
            evals[n*numMeshVertices+i] = 0.0f;
        }
    }
    m_eigenvalues = new double[N];

    //double min = DBL_MAX;
    //double max = -DBL_MAX;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();

#ifdef HAVE_GSL
    gsl_set_error_handler_off();

    gsl_eigen_gensymmv_workspace *w = gsl_eigen_gensymmv_alloc(N);
    gsl_vector*  eval = gsl_vector_alloc(N);
    gsl_matrix*  evec = gsl_matrix_alloc(N,N);
    int status = gsl_eigen_gensymmv(Stot,Mtot,eval,evec,w);

    if (status>0) {
        //fprintf(stderr,"Error: %d\n\t\%s\n",status,gsl_strerror(status));
        QMessageBox::critical(NULL,tr("GSL error"),QString("Error code: ")+QString(gsl_strerror(status))+QString("\n\nPerhapse you should use convex hull or segments connecting the points."));
    } else {
        gsl_eigen_symmv_sort( eval, evec, GSL_EIGEN_SORT_ABS_ASC );
        // step over all eigenvalues
        for(int n=0; n<N; n++) {
            double           eval_n = gsl_vector_get( eval, n );
            gsl_vector_view  evec_n = gsl_matrix_column(evec, n );
            m_eigenvalues[n] = eval_n;
            fprintf(stderr,"%4d -> %10.5f\n",n,eval_n);
            for(int i=0,j=0; i<numMeshVertices && j<N; i++) {
                if (mesh_vertices[i].bmarker!=BOUNDARY_FIXED_MARKER) {
                    double val = gsl_vector_get(&evec_n.vector,j);
                    if (val>max) max = val;
                    if (val<min) min = val;
                    evals[n*numMeshVertices+i] = static_cast<float>(val);
                    j++;
                }
            }
        }
    }
    gsl_eigen_gensymmv_free(w);
    gsl_vector_free(eval);
    gsl_matrix_free(evec);

#elif defined HAVE_LAPACK

    lapack_int info,n,lda,ldb;
    n   = static_cast<lapack_int>(N);
    lda = static_cast<lapack_int>(N);
    ldb = static_cast<lapack_int>(N);
    info = LAPACKE_dsygv(LAPACK_COL_MAJOR,1,'V','U',n,Stot,lda,Mtot,ldb,m_eigenvalues);
    //std::cerr << "INFO: ---------------" << info << std::endl;

    for(int n=0; n<N; n++) {
        fprintf(stderr,"%4d -> %10.5f\n",n,m_eigenvalues[n]);
        for(int i=0,j=0; i<numMeshVertices && j<N; i++) {
            if (mesh_vertices[i].bmarker!=BOUNDARY_FIXED_MARKER) {
                double val = Stot[n*N+j]; //gsl_vector_get(&evec_n.vector,j);
                if (val>max) max = val;
                if (val<min) min = val;
                evals[n*numMeshVertices+i] = static_cast<float>(val);
                j++;
            }
        }
    }

#elif defined HAVE_MAGMA
    magma_int_t info,n,lda,ldb;

    n = static_cast<magma_int_t>(N);
    lda = static_cast<magma_int_t>(N);
    ldb = static_cast<magma_int_t>(N);

    magma_int_t *iwork;
    double *h_work;

    magma_int_t nb = magma_get_dsytrd_nb(n);
    magma_int_t lwork  = 1 + 6*n*nb + 2* n*n;
    magma_int_t liwork = 3 + 5*n;
    h_work = (double*)calloc(lwork,sizeof(double));
    iwork = (magma_int_t*)calloc(liwork,sizeof(magma_int_t));

    //info = LAPACKE_dsygv(LAPACK_COL_MAJOR,1,'V','U',n,Stot,lda,Mtot,ldb,W);
    magma_dsygvd(1, 'V','U', n, Stot, lda, Mtot, ldb, m_eigenvalues, h_work, lwork, iwork,liwork, &info);

    for(int n=0; n<N; n++) {
        fprintf(stderr,"%4d -> %10.5f\n",n,m_eigenvalues[n]);
        for(int i=0,j=0; i<numMeshVertices && j<N; i++) {
            if (mesh_vertices[i].bmarker!=BOUNDARY_FIXED_MARKER) {
                double val = Stot[n*N+j]; //gsl_vector_get(&evec_n.vector,j);
                if (val>max) max = val;
                if (val<min) min = val;
                evals[n*numMeshVertices+i] = static_cast<float>(val);
                j++;
            }
        }
    }
    free(iwork);
    free(h_work);
#endif

    fprintf(stderr,"Min: %8.4f  Max: %8.4f\n",min,max);
    led_status->setText(QString("Min: %1   Max: %2").arg(min,8,'f',4).arg(max,8,'f',4));
}


bool SystemData::exportSMmatrices( QString filename ) {
    if (Stot==NULL || Mtot==NULL) {
        return false;
    }
    FILE *fptr;
#if defined _WIN32 && !defined __MINGW32__
    fopen_s(&fptr,filename.toStdString().c_str(),"wb");
#else
    fptr = fopen(filename.toStdString().c_str(),"wb");
#endif
    if (fptr==NULL) {
        fprintf(stderr,"Cannot open file %s for output.\n",filename.toStdString().c_str());
        return false;
    }
    fwrite(&N,sizeof(int),1,fptr);
    for(int row=0; row<N; row++) {
        for(int col=0; col<N; col++) {
#ifdef HAVE_GSL
            double val = gsl_matrix_get(Stot,row,col);
#elif defined HAVE_LAPACK || defined HAVE_MAGMA
            double val = Stot[row*N+col];
#endif
            fwrite(&val,sizeof(double),1,fptr);
        }
    }
    for(int row=0; row<N; row++) {
        for(int col=0; col<N; col++) {
#ifdef HAVE_GSL
            double val = gsl_matrix_get(Mtot,row,col);
#elif defined HAVE_LAPACK || defined HAVE_MAGMA
            double val = Mtot[row*N+col];
#endif
            fwrite(&val,sizeof(double),1,fptr);
        }
    }

    fclose(fptr);
    return true;
}

