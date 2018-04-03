/**
    @file   SystemView.cpp

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

#include "SystemView.h"

#include <QAction>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QGroupBox>

SystemView :: SystemView( SystemData* sd, OpenGL* ogl, QWidget *parent )
    : QDockWidget(parent)
{
    mData = sd;
    mOpenGL  = ogl;
    init();
}

SystemView::~SystemView() {
}

void SystemView::AddObjectsToScriptEngine( QScriptEngine* engine ) {
    QScriptValue sv = engine->newQObject(this);
    engine->globalObject().setProperty("Ctrl",sv);
}

// *********************************** public methods *********************************
void SystemView::SetViewModus( e_viewModus vm ) {
    cob_viewModus->setCurrentIndex((int)vm);
}

void SystemView::ResetParams() {
    led_maxArea->setText(QString::number(init_max_area));
    led_minAngle->setText(QString::number(init_min_angle));
    chb_useConvexHull->setChecked(false);
    chb_useDelaunay->setChecked(false);
    chb_useQuad->setChecked(true);
    chb_elastSupported->setChecked(false);

    mData->m_maxArea  = init_max_area;
    mData->m_minAngle = init_min_angle;
    mData->m_useConvexHull = false;
    mData->m_useDelaunay   = false;
    mData->m_useQuad       = true;
}

// ************************************* public slots ***********************************

void SystemView::CalcMesh() {

#ifndef USE_EXTERN_TRI
    QString cmdSwitches = QString();
    cmdSwitches += QString("zp");
    if (mData->m_useConvexHull) {
        cmdSwitches += QString("c");
    }
    if (mData->m_maxArea>0.0) {
        cmdSwitches += QString("a%1").arg(mData->m_maxArea);
    }
    if (mData->m_minAngle>0.0) {
        cmdSwitches += QString("q%1").arg(mData->m_minAngle);
    }
    if (mData->m_useDelaunay) {
        cmdSwitches += QString("D");
    }
    if (mData->m_useQuad) {
        cmdSwitches += QString("o2");
    }
#ifdef BE_VERBOSE
    std::cerr << cmdSwitches.toStdString() << std::endl;
#endif // BE_VERBOSE
    if (!mData->DoTriangulation(cmdSwitches.toStdString().c_str())) {
        return;
    }

#else
    QString cmdSwitches = QString("-p");
    if (mData->m_useConvexHull) {
        cmdSwitches += QString("c");
    }
    if (mData->m_maxArea>0.0) {
        cmdSwitches += QString("a%1").arg(mData->m_maxArea);
    }
    if (mData->m_minAngle>0.0) {
        cmdSwitches += QString("q%1").arg(mData->m_minAngle);
    }
    if (mData->m_useDelaunay) {
        cmdSwitches += QString(" -D");
    }
    if (mData->m_useQuad) {
        cmdSwitches += QString(" -o2");
    }
    std::cerr << cmdSwitches.toStdString() << std::endl;

    // ---------------------------
    //  save to temporary file
    // ---------------------------
    QString tmpFileName = QString("models/tmp.poly");
    mData->SavePoly(tmpFileName);
    QProcess genMeshOutside;

    // ---------------------------
    //  solve by external program
    // ---------------------------
    QString cmd = QString("../src/Triangle %1 %2").arg(cmdSwitches).arg("models/tmp.poly");
    genMeshOutside.start(cmd);
    QByteArray output = genMeshOutside.readAllStandardError();
    if (!output.isEmpty()) {
        std::cerr << "error: " << output.data() << std::endl;
    }
    output = genMeshOutside.readAllStandardOutput();
    if (!output.isEmpty()) {
        std::cerr << "output: " << output.data() << std::endl;
    }
    genMeshOutside.waitForFinished();
    fprintf(stderr,"finished\n");

    // ---------------------------
    // read back generated mesh
    // ---------------------------
    if (!mData->ReadNodeAndEleFile(QString("models/tmp.1.node"),QString("models/tmp.1.ele"))) {
        return;
    }
#endif // USE_EXTERN_TRI

    mData->lcd_numMeshVertices->display(mData->mesh_vertices.size());
    mData->lcd_numTriangles->display(mData->numTriangles);

    // TODO: wenn der zu lange dauert, dann abbrechen !!!

    QTime time;
    time.start();
    mData->SolveSystem();
    int dt = time.elapsed();
    fprintf(stderr,"Elapsed time for solving system: %d msec\n",dt);

    mOpenGL->GenMeshBuffers();
    mOpenGL->GenDataTexture();
    //    cob_viewModus->setCurrentIndex((int)e_view2D);

    spb_currEV->setRange(0,mData->N);
    if (mData->m_currEV>mData->N) {
        mData->m_currEV = mData->N;
    }
    int ev = mData->m_currEV;
    led_currEV->setText(QString("%1").arg(mData->m_eigenvalues[ev],8,'f',4));

    mOpenGL->UpdateShaders();
    mOpenGL->updateGL();
}

void SystemView::UpdateView() {
    pub_play->blockSignals(true);    
    if (mData->m_timer->isActive()) {
        pub_play->setChecked(true);
        pub_play->setIcon(QIcon(":/pause.png"));
    }
    else {
        pub_play->setChecked(false);
        pub_play->setIcon(QIcon(":/play.png"));
    }
    pub_play->blockSignals(false);
}

void SystemView::SetTimer( bool status ) {
    if (status) {
        mData->m_timer->setSingleShot(false);
        mData->m_time.restart();
        mData->m_timer->start();
        pub_play->setIcon(QIcon(":/pause.png"));
        pub_step->setEnabled(false);
    } else {
        mData->m_timer->stop();
        pub_play->setIcon(QIcon(":/play.png"));
        pub_step->setEnabled(true);
    }
    pub_play->setChecked(status);
}

void SystemView::SingleTimeStep() {
    mData->m_timer->setSingleShot(true);
    mData->m_currTime += 10e-3;
    mOpenGL->updateGL();
}

void SystemView::Reset() {
    mData->m_timer->setSingleShot(false);
    mData->m_time.restart();
    mData->m_currTime = 0.0;
    mOpenGL->updateGL();
}

void SystemView::Play() {
    if (mData->m_timer->isActive()) {
        SetTimer(false);
    } else {
        SetTimer(true);
    }
}

QString SystemView::GetViewModus() {
    return stl_viewModus[cob_viewModus->currentIndex()];
}

void SystemView::SetViewModus(QString modus) {
    for(int i=0; i<stl_viewModus.size(); i++) {
        if (modus.compare(stl_viewModus[i])==0) {
            cob_viewModus->setCurrentIndex(i);
            break;
        }
    }
}

double SystemView::GetMaxArea() {
    return mData->m_maxArea;
}

void SystemView::SetMaxArea(double ma) {
    mData->m_maxArea = ma;
    led_maxArea->blockSignals(true);
    led_maxArea->setValue(ma);
    led_maxArea->blockSignals(false);
}

double SystemView::GetMinAngle() {
    return mData->m_minAngle;
}

void SystemView::SetMinAngle(double ma) {
    mData->m_minAngle = ma;
    led_minAngle->blockSignals(true);
    led_minAngle->setValue(ma);
    led_minAngle->blockSignals(false);
}

bool SystemView::GetQuadr() {
    return mData->m_useQuad;
}

void SystemView::SetQuadr(bool q) {
    mData->m_useQuad = q;
    chb_useQuad->blockSignals(true);
    chb_useQuad->setChecked(q);
    chb_useQuad->blockSignals(false);
}

bool SystemView::GetCHull() {
    return mData->m_useConvexHull;
}

void SystemView::SetCHull(bool c) {
    mData->m_useConvexHull = c;
    chb_useConvexHull->blockSignals(true);
    chb_useConvexHull->setChecked(c);
    chb_useConvexHull->blockSignals(false);
}

bool SystemView::GetDelaunay() {
    return mData->m_useDelaunay;
}

void SystemView::SetDelaunay(bool d) {
    mData->m_useDelaunay = d;
    chb_useDelaunay->blockSignals(true);
    chb_useDelaunay->setChecked(d);
    chb_useDelaunay->blockSignals(false);
}

bool SystemView::GetElast() {
    return mData->m_elastSupported;
}

void SystemView::SetElast(bool e) {
    mData->m_elastSupported = e;
    chb_elastSupported->blockSignals(true);
    chb_elastSupported->setChecked(e);
    chb_elastSupported->blockSignals(false);
}

double SystemView::GetFreq() {
    return mData->m_freq;
}

void SystemView::SetFreq(double freq) {
    mData->m_freq = freq;
    led_freq->blockSignals(true);
    led_freq->setValue(freq);
    led_freq->blockSignals(false);
    mOpenGL->updateGL();
}

double SystemView::GetScaleFactor() {
    return mData->m_scaleFactor;
}

void SystemView::SetScaleFactor(double s) {
    mData->m_scaleFactor = s;
    led_scaleFactor->blockSignals(true);
    led_scaleFactor->setValue(s);
    led_scaleFactor->blockSignals(false);
    mOpenGL->updateGL();
}

int SystemView::GetEV() {
    return spb_currEV->value();
}

void SystemView::SetEV(int ev) {
    spb_currEV->setValue(ev);
}

void SystemView::selectViewModus() {
    QObject* obj = sender();
    QString objName = obj->objectName();
    int modus = objName.right(1).toInt() - 1;

    if (modus>=0 && modus<3) {
        cob_viewModus->setCurrentIndex(modus);
    }
}

void SystemView::selectViewModus(int idx) {
    mData->m_viewModus = static_cast<e_viewModus>(idx);
    if (mData->m_viewModus==e_viewInput) {
        mData->m_timer->stop();
    }
    UpdateView();
    mOpenGL->updateGL();
    emit emitViewModusChanged();
}

void SystemView::setCurrEV(int ev) {
    mData->m_currEV = ev;
    led_currEV->setText(QString("%1").arg(mData->m_eigenvalues[ev],8,'f',4));
    mOpenGL->updateGL();
}

void SystemView::setFreq() {
    double fPrev = mData->m_freq;
    double tPrev = mData->m_currTime;
    mData->m_freq = led_freq->getValue();
    mData->m_currTime = fPrev*tPrev/mData->m_freq;
}

void SystemView::setSwitchParams() {
    mData->m_minAngle = led_minAngle->text().toDouble();
    mData->m_maxArea  = led_maxArea->text().toDouble();
    mData->m_useQuad  = chb_useQuad->isChecked();
    mData->m_useConvexHull = chb_useConvexHull->isChecked();
    mData->m_useDelaunay = chb_useDelaunay->isChecked();
    mData->m_elastSupported = chb_elastSupported->isChecked();
}

void SystemView::setScaleFactor() {
    mData->m_scaleFactor = led_scaleFactor->getValue();
    mOpenGL->updateGL();
}

// *********************************** protected methods *********************************

void SystemView::init() {
    initElements();
    initGUI();
    initActions();
    initConnect();
}


void SystemView::initElements() {
    lab_viewModus = new QLabel("View Modus");
    cob_viewModus = new QComboBox();
    cob_viewModus->addItems(stl_viewModus);

    lab_maxArea = new QLabel("MaxArea");
    led_maxArea = new DoubleEdit(3,init_max_area,0.01);
    led_maxArea->setRange(0,1e4);
    lab_minAngle = new QLabel("MinAngle");
    led_minAngle = new DoubleEdit(1,init_min_angle,0.1);
    led_minAngle->setRange(0,35);
    chb_useQuad = new QCheckBox("quadr");
    chb_useQuad->setChecked(true);
    chb_useConvexHull = new QCheckBox("C-Hull");
    chb_useConvexHull->setChecked(false);
    chb_useDelaunay = new QCheckBox("Del'ay");
    chb_useDelaunay->setChecked(false);
    pub_calcMesh = new QPushButton("Calc mesh");
    chb_elastSupported = new QCheckBox("Elast.");
    chb_elastSupported->setChecked(false);

    pub_reset = new QPushButton(QIcon(":/back.png"),"");
    pub_reset->setMaximumWidth(30);
    pub_play  = new QPushButton(QIcon(":/play.png"),"");
    pub_play->setCheckable(true);    
    pub_step  = new QPushButton(QIcon(":/step.png"),"");
    pub_step->setAutoRepeat(true);
    pub_step->setAutoRepeatInterval(10);

    lab_freq = new QLabel("freq");
    led_freq = new DoubleEdit(3,mData->m_freq,0.1);
    led_freq->setRange(1e-3,1e10);
    lab_currEV = new QLabel("#EV");
    spb_currEV = new QSpinBox();
    spb_currEV->setRange(0,0);
    led_currEV = new QLineEdit();
    led_currEV->setReadOnly(true);
    led_currEV->setAlignment(Qt::AlignRight);

    lab_scaleFactor = new QLabel("Scale");
    led_scaleFactor = new DoubleEdit(2,mData->m_scaleFactor,0.1);
}


void SystemView::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete->addWidget( lab_viewModus, 0, 0 );
    layout_complete->addWidget( cob_viewModus, 0, 1 );


    QGroupBox* grb_gmesh = new QGroupBox("Gen mesh");
    QGridLayout* layout_gmesh = new QGridLayout();
    layout_gmesh->addWidget( lab_maxArea,  0, 0 );
    layout_gmesh->addWidget( led_maxArea,  0, 1 );
    layout_gmesh->addWidget( chb_useConvexHull, 0, 2 );
    layout_gmesh->addWidget( lab_minAngle, 1, 0 );
    layout_gmesh->addWidget( led_minAngle, 1, 1 );
    layout_gmesh->addWidget( chb_useDelaunay, 1, 2 );
    layout_gmesh->addWidget( chb_useQuad,  2, 0 );
    layout_gmesh->addWidget( pub_calcMesh, 2, 1 );
    layout_gmesh->addWidget( chb_elastSupported, 2, 2 );
    grb_gmesh->setLayout(layout_gmesh);


    QGroupBox* grb_anim = new QGroupBox("Animate");
    QGridLayout* layout_anim = new QGridLayout();
    layout_anim->addWidget( lab_currEV, 0, 0 );
    layout_anim->addWidget( spb_currEV, 0, 1 );
    layout_anim->addWidget( led_currEV, 0, 2 );
    layout_anim->addWidget( lab_freq, 1, 0 );
    layout_anim->addWidget( led_freq, 1, 1, 1, 2 );
    layout_anim->addWidget( lab_scaleFactor, 2, 0 );
    layout_anim->addWidget( led_scaleFactor, 2, 1, 1, 2 );
    layout_anim->addWidget( pub_play,  3, 1 );
    layout_anim->addWidget( pub_step,  3, 2 );
    layout_anim->addWidget( pub_reset, 3, 0 );
    grb_anim->setLayout(layout_anim);

    layout_complete->addWidget( grb_gmesh, 1, 0, 1, 2 );
    layout_complete->addWidget( grb_anim, 2, 0, 1, 2);
    layout_complete->setRowStretch(3,2);
    layout_complete->setColumnStretch(1,1);

    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout( layout_complete );
    centralWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setWidget(centralWidget);

    setWindowTitle("Control");
    setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
}

void SystemView :: initActions() {
    QAction*  actSelectInputModus = new QAction(this);
    actSelectInputModus->setShortcut(Qt::CTRL|Qt::Key_1);
    actSelectInputModus->setObjectName("ACT_1");
    addAction(actSelectInputModus);
    connect(actSelectInputModus,SIGNAL(triggered()),this,SLOT(selectViewModus()));

    QAction*  actSelectView2DModus = new QAction(this);
    actSelectView2DModus->setShortcut(Qt::CTRL|Qt::Key_2);
    actSelectView2DModus->setObjectName("ACT_2");
    addAction(actSelectView2DModus);
    connect(actSelectView2DModus,SIGNAL(triggered()),this,SLOT(selectViewModus()));

    QAction*  actSelectView3DModus = new QAction(this);
    actSelectView3DModus->setShortcut(Qt::CTRL|Qt::Key_3);
    actSelectView3DModus->setObjectName("ACT_3");
    addAction(actSelectView3DModus);
    connect(actSelectView3DModus,SIGNAL(triggered()),this,SLOT(selectViewModus()));
}

void SystemView::initConnect() {
    connect( cob_viewModus, SIGNAL(currentIndexChanged(int)), this, SLOT(selectViewModus(int)) );
    connect( led_maxArea,  SIGNAL(editingFinished()), this, SLOT(setSwitchParams()) );
    connect( led_minAngle, SIGNAL(editingFinished()), this, SLOT(setSwitchParams()) );
    connect( chb_useQuad,  SIGNAL(stateChanged(int)), this, SLOT(setSwitchParams()) );
    connect( chb_useConvexHull, SIGNAL(stateChanged(int)), this, SLOT(setSwitchParams()) );
    connect( chb_useDelaunay,   SIGNAL(stateChanged(int)), this, SLOT(setSwitchParams()) );
    connect( chb_elastSupported, SIGNAL(stateChanged(int)), this, SLOT(setSwitchParams()) );
    connect( pub_calcMesh, SIGNAL(pressed()), this,      SLOT(CalcMesh()) );

    connect( spb_currEV, SIGNAL(valueChanged(int)), this, SLOT(setCurrEV(int)) );
    connect( led_freq, SIGNAL(editingFinished()), this, SLOT(setFreq()) );
    connect( pub_play, SIGNAL(toggled(bool)), this,      SLOT(SetTimer(bool)) );
    connect( pub_step, SIGNAL(pressed()),     this,      SLOT(SingleTimeStep()) );
    connect( pub_reset, SIGNAL(pressed()),    this,      SLOT(Reset()) );    
    connect( led_scaleFactor, SIGNAL(editingFinished()), this, SLOT(setScaleFactor()) );
}

QSize SystemView::sizeHint() const {
    return QSize(100,50);
}



bool FileFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    QFileSystemModel* fileModel = qobject_cast<QFileSystemModel*>(sourceModel());
    return fileModel->fileName(index0).indexOf(".1.") < 0;
    // uncomment to call the default implementation
    //return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
