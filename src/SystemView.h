/**
    @file   SystemView.h

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

#ifndef NUMCHLADNI_SYSTEM_VIEW_H
#define NUMCHLADNI_SYSTEM_VIEW_H

#include "DoubleEdit.h"
#include "OpenGL.h"
#include "SystemData.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QSortFilterProxyModel>

/**
 * @brief The SystemView class
 */
class SystemView : public QDockWidget
{
    Q_OBJECT

    Q_PROPERTY( double   maxArea   READ GetMaxArea      WRITE  SetMaxArea )
    Q_PROPERTY( double   minAngle  READ GetMinAngle     WRITE  SetMinAngle )
    Q_PROPERTY( bool     quadr     READ GetQuadr        WRITE  SetQuadr )
    Q_PROPERTY( bool     chull     READ GetCHull        WRITE  SetCHull )
    Q_PROPERTY( bool     delay     READ GetDelaunay     WRITE  SetDelaunay )
    Q_PROPERTY( bool     elast     READ GetElast        WRITE  SetElast )
    Q_PROPERTY( double   freq      READ GetFreq         WRITE  SetFreq )
    Q_PROPERTY( double   scale     READ GetScaleFactor  WRITE  SetScaleFactor)
    Q_PROPERTY( QString  modus     READ GetViewModus    WRITE  SetViewModus)
    Q_PROPERTY( int      ev        READ GetEV           WRITE  SetEV )

public:
    /** Standard constructor.
     * @param sd pointer to system data widget.
     * @param ogl pointer to main opengl widget.
     * @param parent  pointer to parent widget.
     */
    SystemView ( SystemData* sd, OpenGL* ogl, QWidget* parent = 0 );

    /** Standard destructor.
     */
    virtual ~SystemView ();

    void AddObjectsToScriptEngine( QScriptEngine* engine );

// ------------ public methods -------------
public:
    void SetViewModus( e_viewModus vm );
    void ResetParams();

// ------------ public slots -------------
public slots:
    void  CalcMesh();
    void  UpdateView();
    void  SetTimer(bool);
    void  SingleTimeStep();
    void  Reset();
    void  Play();

    QString GetViewModus();
    void    SetViewModus(QString modus);
    double GetMaxArea();
    void   SetMaxArea(double ma);
    double GetMinAngle();
    void   SetMinAngle(double ma);
    bool   GetQuadr();
    void   SetQuadr(bool q);
    bool   GetCHull();
    void   SetCHull(bool c);
    bool   GetDelaunay();
    void   SetDelaunay(bool d);
    bool   GetElast();
    void   SetElast(bool e);
    double GetFreq();
    void   SetFreq(double freq);
    double GetScaleFactor();
    void   SetScaleFactor(double s);
    int    GetEV();
    void   SetEV(int ev);

protected slots:
    void  selectViewModus();
    void  selectViewModus(int);
    void  setCurrEV(int);
    void  setFreq();
    void  setSwitchParams();
    void  setScaleFactor();

// ------------ signals -------------
signals:
    void  emitViewModusChanged();
 
// ----------- protected methods -----------   
protected:
    /** Initialize view.
     */
    void init();

    /** Initialize view elements.
     */
    void initElements();

    /** Construct graphical user interface.
     */
    void initGUI();

    /** Initialize actions.
     */
    void initActions();

    /** Connect signals and slots.
     */
    void initConnect();

    virtual QSize  sizeHint () const;
 
// ----------- private attributes ----------
private:
    SystemData*   mData;
    OpenGL*       mOpenGL;

    QLabel*       lab_viewModus;
    QComboBox*    cob_viewModus;
    QLabel*       lab_maxArea;
    DoubleEdit*   led_maxArea;
    QLabel*       lab_minAngle;
    DoubleEdit*   led_minAngle;
    QCheckBox*    chb_useQuad;
    QCheckBox*    chb_useConvexHull;
    QCheckBox*    chb_useDelaunay;
    QCheckBox*    chb_elastSupported;
    QPushButton*  pub_calcMesh;

    QLabel*       lab_freq;
    DoubleEdit*   led_freq;
    QLabel*       lab_currEV;
    QSpinBox*     spb_currEV;
    QLineEdit*    led_currEV;
    QLabel*       lab_scaleFactor;
    DoubleEdit*   led_scaleFactor;
    QPushButton*  pub_reset;
    QPushButton*  pub_play;
    QPushButton*  pub_step;
};


class FileFilterProxyModel : public QSortFilterProxyModel
{
protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
};


#endif // NUMCHLADNI_SYSTEM_VIEW_H
