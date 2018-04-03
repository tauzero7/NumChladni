/**
    @file   ControlMesh.cpp

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

#include <limits>
#include "ControlMesh.h"

#include <QGridLayout>
#include <QTextStream>

/*! Standard constructor.
 *  \param parent : pointer to parent widget.
 */ 
ControlMesh::ControlMesh( SystemData* dt, OpenGL* oview, QWidget *parent )
    : QDockWidget(parent),
      mData(dt),
      mOpenGL(oview) {
    init();    
}


ControlMesh::~ControlMesh() {
}

void ControlMesh::AddObjectsToScriptEngine( QScriptEngine* engine ) {
    QScriptValue ogl = engine->newQObject(this);
    engine->globalObject().setProperty("CM",ogl);
}

bool ControlMesh::LoadPoly( QString filename ) {
    setlocale(LC_NUMERIC, "C");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
       fprintf(stderr,"Cannot open file %s for reading.\n",filename.toStdString().c_str());
       return false;
    }

    this->delAllPoints();
    this->delAllSegments();
    this->delAllHoles();

    QTextStream stream(&file);
    QString line;
    QStringList p;

    double x,y;
    int num,bm;
    int p1,p2;


    // ----------------------------
    //  read vertex header
    // ----------------------------
    do {
        line = stream.readLine();
    } while (line.startsWith("#") && !stream.atEnd());

    line = line.trimmed();
    p = line.split(QRegExp("(\\s+)"));
    if (p.size()>=4) {
        mData->numVertices = p[0].toInt();
        mData->numDim = p[1].toInt();
        mData->numAttribs = p[2].toInt();
        mData->numBMarker = p[3].toInt();
    }
    fprintf(stderr,"Vertices: %d %d %d %d\n",mData->numVertices,mData->numDim,mData->numAttribs,mData->numBMarker);

    // ----------------------------
    //  read vertices
    // ----------------------------
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::min();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::min();
    double centerX = 0.0;
    double centerY = 0.0;

    int v = 0;
    while(!stream.atEnd() && v<mData->numVertices) {
        line = stream.readLine();
        if (line==QString() || line.startsWith("#")) {
            continue;
        }

        bm = 0;
        line = line.trimmed();
        p = line.split(QRegExp("(\\s+)"));
        if (p.size()>=3) {
            num = p[0].toInt();
            x = p[1].toDouble();
            y = p[2].toDouble();

            if (x<minX) minX = x;
            if (x>maxX) maxX = x;
            if (y<minY) minY = y;
            if (y>maxY) maxY = y;
            centerX += x;
            centerY += y;

            if (mData->numAttribs>0 && p.size()>=3+mData->numAttribs) {

            }
            if (mData->numBMarker>0 && p.size()>3+mData->numAttribs) {
                bm = p[2+mData->numAttribs+1].toInt();
            }
            mod_points->insertRows(v,1);
            tbw_points->setRowHeight(v,DEF_TBLVIEW_ROW_HEIGHT);

            node_t node = {num,glm::dvec2(x,y),bm,false};
            if (bm==BOUNDARY_FIXED_MARKER) {
                node.isFixed = true;
            }
            mData->m_vertices.replace(v,node);

            fprintf(stderr,"%4d %8.3f %8.3f ",num,x,y);
            for(int i=0; i<mData->numAttribs; i++) {
                //fprintf(stderr,"%e ",)
            }
            if (mData->numBMarker==1) {
                fprintf(stderr,"%d",bm);
            }
            fprintf(stderr,"\n");
        }
        v++;
    }

#if 0
    centerX /= static_cast<double>(mData->m_vertices.size());
    centerY /= static_cast<double>(mData->m_vertices.size());
    mData->AdjustBorder(glm::dvec2(minX,maxX),glm::dvec2(minY,maxY),glm::dvec2(centerX,centerY));
    emit borderChanged();
#endif


    // ----------------------------
    //  read segment header
    // ----------------------------
    do {
        line = stream.readLine();
    } while ((line==QString() || line.startsWith("#")) && !stream.atEnd());

    line = line.trimmed();
    p = line.split(QRegExp("(\\s+)"));
    if (p.size()>=2) {
        mData->numSegments = p[0].toInt();
        mData->numSegBMarker = p[1].toInt();
    }
    fprintf(stderr,"Segments: %d %d\n",mData->numSegments,mData->numSegBMarker);

    // ----------------------------
    //  read segments
    // ----------------------------
    v = 0;
    while(!stream.atEnd() && v<mData->numSegments) {
        line = stream.readLine();
        if (line==QString() || line.startsWith("#")) {
            continue;
        }
        bm = 0;
        line = line.trimmed();
        p = line.split(QRegExp("(\\s+)"));
        if (p.size()>=3) {
            num = p[0].toInt();
            p1 = p[1].toInt();
            p2 = p[2].toInt();
            if (p.size()>=4) {
                bm = p[3].toInt();
            }

            mod_segments->insertRows(v,1);
            tbw_segments->setRowHeight(v,DEF_TBLVIEW_ROW_HEIGHT);

            segment_t seg = {num,p1,p2,bm};
            mData->m_segments.replace(v,seg);

            fprintf(stderr,"%4d %3d %3d ",num,p1,p2);
            if (mData->numSegBMarker==1) {
                fprintf(stderr,"%d",bm);
            }
            fprintf(stderr,"\n");
        }
        v++;
    }

    // ----------------------------
    //  read holes header
    // ----------------------------
    do {
        line = stream.readLine();
    } while ((line==QString() || line.startsWith("#")) && !stream.atEnd());

    line = line.trimmed();
    p = line.split(QRegExp("(\\s+)"));
    if (p.size()>=1) {
        mData->numHoles = p[0].toInt();
    }
    fprintf(stderr,"Holes: %d\n",mData->numHoles);

    // ----------------------------
    //  read holes
    // ----------------------------
    v = 0;
    while(!stream.atEnd() && v<mData->numHoles) {
        line = stream.readLine();
        if (line==QString() || line.startsWith("#")) {
            continue;
        }
        line = line.trimmed();
        p = line.split(QRegExp("(\\s+)"));
        if (p.size()>=3) {
            num = p[0].toInt();
            x = p[1].toDouble();
            y = p[2].toDouble();
            hole_t h = {num,glm::dvec2(x,y)};
            //mData->m_holes.push_back(h);

            mod_holes->insertRows(v,1);
            tbw_holes->setRowHeight(v,DEF_TBLVIEW_ROW_HEIGHT);
            mData->m_holes.replace(v,h);

            fprintf(stderr,"%4d %f %f\n",num,x,y);
        }
        v++;
    }

    file.close();

    fprintf(stderr,"#vertices: %d   #segments: %d\n",mData->m_vertices.size(), mData->m_segments.size());
    dataChanged();
    return true;
}


void ControlMesh::UpdateSegments(int idx) {
    if (idx<0) {

    } else {
        QList<segment_t>::iterator sItr = mData->m_segments.begin();
        int c = 0;
        while(sItr!=mData->m_segments.end()) {
            if (sItr->p1==idx || sItr->p2==idx) {
                sItr = mData->m_segments.erase(sItr);
                c++;
            } else {
                sItr++;
            }
        }
        if (c>0) {
            mod_segments->cleanRows(mData->m_segments.size(),c);
        }

        for(sItr = mData->m_segments.begin(); sItr!=mData->m_segments.end(); sItr++) {
            if (sItr->p1>idx) {
                sItr->p1--;
            }
            if (sItr->p2>idx) {
                sItr->p2--;
            }
        }
    }
}

// ************************************* public slots ***********************************

void ControlMesh::changeMouseStatus () {
    mData->m_mouseCtrl = (e_spMouseCtrl)cob_mouseCtrl->currentIndex();
    sm_points->clearSelection();
    sm_segments->clearSelection();
    mData->m_activePoint = -1;
    mData->m_activeSegment = glm::ivec2(-1,-1);
    mOpenGL->updateGL();

    switch(mData->m_mouseCtrl) {
        case e_spMouse_standard:
        case e_spMouse_checkPoint:
            pub_add->setEnabled(false);
            pub_del->setEnabled(false);
            break;
        case e_spMouse_addMovePoint:
        case e_spMouse_delPoint:
        case e_spMouse_addSegment:
        case e_spMouse_addHole:
            pub_add->setEnabled(true);
            pub_del->setEnabled(true);
            break;
    }
}

void ControlMesh::dataChanged() {
    mOpenGL->setCtrlPoints();
    mOpenGL->setCtrlSegments();
}

void ControlMesh::addPointSegHole() {
    //fprintf(stderr,"add points\n");
    //QModelIndex idx = tbw_points->currentIndex();

    if (cob_mouseCtrl->currentIndex() == (int)e_spMouse_addMovePoint) {
        int rc = mod_points->rowCount();
        if (rc<=0) {
            mod_points->insertRows(0,1);
            tbw_points->setRowHeight(0,DEF_TBLVIEW_ROW_HEIGHT);
        } else {
            mod_points->insertRows(rc,1);
            tbw_points->setRowHeight(rc,DEF_TBLVIEW_ROW_HEIGHT);
        }
        mOpenGL->setCtrlPoints();
    }
    else if (cob_mouseCtrl->currentIndex() == (int)e_spMouse_addSegment) {
        int rc = mod_segments->rowCount();
        if (rc<=0) {
            mod_segments->insertRows(0,1);
            tbw_segments->setRowHeight(0,DEF_TBLVIEW_ROW_HEIGHT);
        } else {
            mod_segments->insertRows(rc,1);
            tbw_segments->setRowHeight(rc,DEF_TBLVIEW_ROW_HEIGHT);
        }
        mOpenGL->setCtrlSegments();
    }
    else if (cob_mouseCtrl->currentIndex() == (int)e_spMouse_addHole) {
        int rc = mod_holes->rowCount();
        if (rc<=0) {
            mod_holes->insertRows(0,1);
            tbw_holes->setRowHeight(0,DEF_TBLVIEW_ROW_HEIGHT);
        } else {
            mod_holes->insertRows(rc,1);
            tbw_holes->setRowHeight(rc,DEF_TBLVIEW_ROW_HEIGHT);
        }
    }
}

void ControlMesh::addPoint(double x, double y) {
    //QModelIndex idx = tbw_points->currentIndex();
    //fprintf(stderr,"Add: %10.8f %10.8f",x,y);

    int rc = mod_points->rowCount();
    if (rc<=0) {
        mod_points->insertRows(0,1);
        tbw_points->setRowHeight(0,DEF_TBLVIEW_ROW_HEIGHT);
    }
    else {
        mod_points->insertRows(rc+1,1);
        tbw_points->setRowHeight(rc,DEF_TBLVIEW_ROW_HEIGHT);
    }

    QModelIndex nidx = mod_points->index( rc,0);
    mod_points->setData(nidx,x);

    nidx = mod_points->index( rc,1);
    mod_points->setData(nidx,y);

    nidx = mod_points->index( rc,2);
    mod_points->setData(nidx,false);

    mData->m_activePoint = rc;
    setActive(mData->m_activePoint);

    mOpenGL->setCtrlPoints();
}


void ControlMesh::delPointSegHole() {
    QModelIndex idx;
    if (cob_mouseCtrl->currentIndex() == (int)e_spMouse_addMovePoint ||
            cob_mouseCtrl->currentIndex() == (int)e_spMouse_delPoint) {
        idx = tbw_points->currentIndex();
        if (idx.row()>=0) {
            mod_points->removeRow(idx.row());
        }
        UpdateSegments(idx.row());
        mOpenGL->setCtrlPoints();
    } else if (cob_mouseCtrl->currentIndex() == (int)e_spMouse_addSegment) {
        idx = tbw_segments->currentIndex();
        if (idx.row()>=0) {
            mod_segments->removeRow(idx.row());
        }
    } else if (cob_mouseCtrl->currentIndex() == (int)e_spMouse_addHole) {
        idx = tbw_holes->currentIndex();
        if (idx.row()>=0) {
            mod_holes->removeRow(idx.row());
        }
    }
    mData->m_activeSegment = glm::ivec2(-1,-1);
    this->setActiveSegment(-1,-1);
    mOpenGL->setCtrlSegments();
}

void ControlMesh::delPoint(int num ) {
    if (num>=0) {
        mod_points->removeRow(num);
    }
    UpdateSegments(num);
    mData->m_activePoint = -1;
    mOpenGL->setCtrlPoints();
    mOpenGL->setCtrlSegments();
}

void ControlMesh::delAllPoints() {
    int numRows = mod_points->rowCount();
    if (numRows>0) {
        mod_points->removeRows(0,numRows);
    }
    delAllSegments();
    mData->m_activePoint = -1;
    mOpenGL->setCtrlPoints();
    mOpenGL->setCtrlSegments();
}

void ControlMesh::checkPoint(int num) {
    if (num<1 || num>mData->m_vertices.size()) {
        return;
    }

    QModelIndex nidx = mod_points->index(num-1,2);
    bool isFixed = mData->m_vertices[num-1].isFixed;
    mod_points->setData(nidx,!isFixed);
    mOpenGL->setCtrlPoints();
}

void ControlMesh::movePoint( double x, double y ) {
    int numRow = mData->m_trackPoint;
    if (numRow>=0)
    {
        QModelIndex nidx = mod_points->index( numRow,0);
        mod_points->setData(nidx,x);
        nidx = mod_points->index( numRow,1);
        mod_points->setData(nidx,y);
    }
    mOpenGL->setCtrlPoints();
}

void ControlMesh::setActive( int actPoint ) {
    tbw_points->clearSelection();
    if (actPoint>0) {
        tbw_points->selectRow(actPoint);
    }
}


void ControlMesh::addSegment(int p1, int p2) {
    //std::cerr << p1 << " " << p2 << std::endl;
    int rc = mod_segments->rowCount();
    if (rc<=0) {
        mod_segments->insertRows(0,1);
        tbw_segments->setRowHeight(0,DEF_TBLVIEW_ROW_HEIGHT);
    }
    else {
        mod_segments->insertRows(rc+1,1);
        tbw_segments->setRowHeight(rc,DEF_TBLVIEW_ROW_HEIGHT);
    }

    QModelIndex nidx = mod_segments->index( rc,0);
    mod_segments->setData(nidx,p1);

    nidx = mod_segments->index( rc,1);
    mod_segments->setData(nidx,p2);

    mOpenGL->setCtrlSegments();
}

void ControlMesh::delAllSegments() {
    int numRows = mod_segments->rowCount();
    if (numRows>0) {
        mod_segments->removeRows(0,numRows);
    }
    mOpenGL->setCtrlSegments();
}

void ControlMesh::setActiveSegment(int p1, int p2) {
    if (p1<0 || p2<0) {
        tbw_segments->clearSelection();
    }
}

void ControlMesh::addHole(double x, double y) {
    int rc = mod_holes->rowCount();
    if (rc<=0) {
        mod_holes->insertRows(0,1);
        tbw_holes->setRowHeight(0,DEF_TBLVIEW_ROW_HEIGHT);
    }
    else {
        mod_holes->insertRows(rc+1,1);
        tbw_holes->setRowHeight(rc,DEF_TBLVIEW_ROW_HEIGHT);
    }

    QModelIndex nidx = mod_holes->index( rc,0);
    mod_holes->setData(nidx,x);

    nidx = mod_holes->index( rc,1);
    mod_holes->setData(nidx,y);
}

void ControlMesh::delAllHoles() {
    int numRows = mod_holes->rowCount();
    if (numRows>0) {
        mod_holes->removeRows(0,numRows);
    }
}

void ControlMesh::highlightPoint(int index) {
    mData->m_activePoint = index;
    //mOpenGL->setCtrlPoints();
}

void ControlMesh::highlightSegment(int) {
    // TODO
}

void ControlMesh::s_close() {
    hide();
}

void ControlMesh::tabChanged() {
    sm_points->clearSelection();
    sm_segments->clearSelection();
    mData->m_activePoint = -1;
    mData->m_activeSegment = glm::ivec2(-1,-1);
    mOpenGL->updateGL();
}

void ControlMesh::points_currentRowChanged(const QModelIndex & current, const QModelIndex &) {
    //std::cerr << current.row() << std::endl;
    mData->m_activePoint = current.row();
    mOpenGL->updateGL();
}

void ControlMesh::segments_currentRowChanged(const QModelIndex & current, const QModelIndex &) {
    //std::cerr << current.row() << std::endl;
    if (current.row()>=0 && current.row()<mData->m_segments.size()) {
        int p1 = mData->m_segments[current.row()].p1;
        int p2 = mData->m_segments[current.row()].p2;
        mData->m_activeSegment = glm::ivec2(p1,p2);
    }
    mOpenGL->updateGL();
}

// *********************************** protected methods *********************************

void ControlMesh::init() {
    initElements();
    initGUI();
    initActions();
    initControl();
}

void ControlMesh::initElements() {
    lab_mouseCtrl = new QLabel("Mouse");
    cob_mouseCtrl = new QComboBox();
    cob_mouseCtrl->addItems(stl_spMouseCtrl);

    pub_add = new QPushButton("+");
    pub_add->setMaximumWidth(20);
    pub_add->setEnabled(false);
    pub_del = new QPushButton("-");
    pub_del->setMaximumWidth(20);
    pub_del->setEnabled(false);
    pub_clearAllPoints = new QPushButton("DelAll");
    pub_clearAllPoints->setMaximumWidth(45);

    mod_points = new PointListModel(mData);
    tbw_points = new TableView(mData);
    tbw_points->setModel(mod_points);
    tbw_points->setItemDelegateForColumn(0,&tbw_delegate);
    tbw_points->setItemDelegateForColumn(1,&tbw_delegate);
    tbw_points->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed|QAbstractItemView::EditKeyPressed);
    tbw_points->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbw_points->setAlternatingRowColors(true);
    tbw_points->setColumnWidth(0,85);
    tbw_points->setColumnWidth(1,85);
    tbw_points->setColumnWidth(2,40);
    tbw_points->setSelectionMode(QAbstractItemView::SingleSelection);
    tbw_points->setSelectionBehavior(QAbstractItemView::SelectRows);
    sm_points = tbw_points->selectionModel();
    connect(sm_points, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
        this, SLOT(points_currentRowChanged(QModelIndex,QModelIndex)));

    tbw_segDelegate = new SegBoxDelegate(mData);
    mod_segments = new SegmentListModel(mData);
    tbw_segments = new QTableView();
    tbw_segments->setModel(mod_segments);
    tbw_segments->setItemDelegateForColumn(0,tbw_segDelegate);
    tbw_segments->setItemDelegateForColumn(1,tbw_segDelegate);
    tbw_segments->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed|QAbstractItemView::EditKeyPressed);
    tbw_segments->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbw_segments->setAlternatingRowColors(true);
    tbw_segments->setColumnWidth(0,60);
    tbw_segments->setColumnWidth(1,60);
    tbw_segments->setSelectionMode(QAbstractItemView::SingleSelection);
    //tbw_segments->setSelectionBehavior(QAbstractItemView::SelectRows);
    sm_segments = tbw_segments->selectionModel();
    connect(sm_segments, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
        this, SLOT(segments_currentRowChanged(QModelIndex,QModelIndex)));


    mod_holes = new HoleListModel(mData);
    tbw_holes = new QTableView();
    tbw_holes->setModel(mod_holes);
    tbw_holes->setItemDelegateForColumn(0,&tbw_holeDelegate);
    tbw_holes->setItemDelegateForColumn(1,&tbw_holeDelegate);
    tbw_holes->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed|QAbstractItemView::EditKeyPressed);
    tbw_holes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbw_holes->setAlternatingRowColors(true);
    tbw_holes->setColumnWidth(0,85);
    tbw_holes->setColumnWidth(1,85);
    tbw_holes->setSelectionMode(QAbstractItemView::SingleSelection);
}


void ControlMesh::initGUI() {
    QGridLayout* layout_data = new QGridLayout();
    //layout_data->addWidget( lab_mouseCtrl, 0, 0 );
    layout_data->addWidget( cob_mouseCtrl, 0, 0 );
    layout_data->addWidget( pub_add,  0, 1 );
    layout_data->addWidget( pub_del,  0, 2 );
    //layout_data->addWidget( pub_clearAllPoints,  0, 4 );

    tabWidget = new QTabWidget();
    tabWidget->addTab(tbw_points,"Points");
    tabWidget->addTab(tbw_segments,"Segments");
    tabWidget->addTab(tbw_holes,"Holes");
    layout_data->addWidget(tabWidget, 1,0,1,3);
    layout_data->setRowStretch(1,2);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout_data);

    setWidget(centralWidget);
    setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
    setAllowedAreas(Qt::BottomDockWidgetArea|Qt::RightDockWidgetArea|Qt::LeftDockWidgetArea);
    setWindowTitle("ControlMesh");
}

void ControlMesh::initActions() {
}


void ControlMesh::initControl() {
    connect( tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged()) );
    connect( cob_mouseCtrl, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMouseStatus()) );
    connect( pub_add, SIGNAL(pressed()), this, SLOT(addPointSegHole()) );
    connect( pub_del, SIGNAL(pressed()), this, SLOT(delPointSegHole()) );
    connect( pub_clearAllPoints, SIGNAL(pressed()), this, SLOT(delAllPoints()) );

    connect( mod_points, SIGNAL(dataChanged(QModelIndex,QModelIndex)),  this, SLOT(dataChanged()) );
    //connect( tbw_points->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(highlightPoint(int)) );
    connect( mod_segments, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged()) );
    //connect( tbw_segments->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(highlightSegment(int)) );
}


