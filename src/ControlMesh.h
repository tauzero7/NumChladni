/**
    @file   ControlMesh.h

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

#ifndef NUMCHLADNI_CONTROL_MESH_H
#define NUMCHLADNI_CONTROL_MESH_H

#include <iostream>

#include "SystemData.h"
#include "OpenGL.h"
#include "PointListModel.h"
#include "SegmentListModel.h"
#include "HoleListModel.h"
#include "DoubleEdit.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>


/**
 * @brief The ControlMesh class
 */
class ControlMesh : public QDockWidget
{
    Q_OBJECT

public:
    ControlMesh ( SystemData* dt, OpenGL* oview, QWidget* parent = 0 );
    ~ControlMesh ();

    void AddObjectsToScriptEngine( QScriptEngine* engine );
    bool LoadPoly( QString filename );
    void UpdateSegments( int idx = -1 );

    // ------------ public slots -------------
public slots:
    void  changeMouseStatus ();
    void  dataChanged       ();
    void  addPointSegHole   ();
    void  addPoint          (double x, double y);
    void  delPointSegHole   ();
    void  delPoint          (int);
    void  delAllPoints      ();
    void  checkPoint        (int);
    void  movePoint         (double x, double y);
    void  setActive         (int);

    void  addSegment(int, int);
    void  delAllSegments();
    void  setActiveSegment(int,int);

    void  addHole(double x, double y);
    void  delAllHoles();

    void  highlightPoint(int);
    void  highlightSegment(int);
    void  s_close();

    // script functions

protected slots:
    void tabChanged();
    void points_currentRowChanged(const QModelIndex& , const QModelIndex& );
    void segments_currentRowChanged(const QModelIndex&, const QModelIndex& );

    // ------------ signals -------------
signals:
    void borderChanged();

    // ----------- protected methods -----------
protected:
    void  init         ( );
    void  initElements ( );
    void  initGUI      ( );
    void  initActions  ( );
    void  initControl  ( );


    // ----------- private attributes ----------
private:
    SystemData*    mData;
    OpenGL*        mOpenGL;

    QLabel*        lab_mouseCtrl;
    QComboBox*     cob_mouseCtrl;
    QTabWidget*    tabWidget;

    QPushButton*   pub_add;
    QPushButton*   pub_del;
    QPushButton*   pub_clearAllPoints;

    QModelIndex        nidx;

    PointListModel*    mod_points;
    TableView*         tbw_points;
    DoubleBoxDelegate  tbw_delegate;

    SegmentListModel*  mod_segments;
    QTableView*        tbw_segments;
    SegBoxDelegate*    tbw_segDelegate;

    HoleListModel*     mod_holes;
    QTableView*        tbw_holes;
    DoubleBoxDelegate  tbw_holeDelegate;

    QItemSelectionModel *sm_points;
    QItemSelectionModel *sm_segments;
};

#endif // NUMCHLADNI_CONTROL_MESH_H
