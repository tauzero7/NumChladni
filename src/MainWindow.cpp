/**
    @file   MainWindow.cpp

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

#include "MainWindow.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

QScriptValue func_toRadians( QScriptContext *context, QScriptEngine * ) {
    QScriptValue a = context->argument(0);
    return a.toNumber()/180.0*M_PI;
}

QScriptValue func_toDegree( QScriptContext *context, QScriptEngine * ) {
    QScriptValue a = context->argument(0);
    return a.toNumber()/M_PI*180.0;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    init();
#ifdef HAVE_GSL
    setWindowTitle("NumChladni - GSL");
#elif defined HAVE_LAPACK
    setWindowTitle("NumChaldni - LAPACK");
#elif defined HAVE_MAGMA
    setWindowTitle("NumChladni - MAGMA");
#else
    setWindowTitle("NumChladni");
#endif
}

MainWindow::~MainWindow() {
    //std::cerr << "Del MainWindow.\n";
    delete mOpenGL;
}

// ********************************** public methods *****************************
QSize MainWindow::minimumSizeHint() const {
    return QSize( DEF_GUI_WIDTH, DEF_GUI_HEIGHT );
}

QSize MainWindow::sizeHint() const {
    return QSize( DEF_GUI_WIDTH, DEF_GUI_HEIGHT );
}

void MainWindow::IgnoreTessShaderAvailability() {
    fprintf(stderr,"Ignore Tessellation shader availability!\n");
    mData->m_haveTessShader = false;
    mOGLProps->SetOGLParams();
}

// *********************************** public slots ******************************
void MainWindow::quit() {
    close();
}

void MainWindow::about() {
    QMessageBox::about( this, tr("NumChladni") ,
                        tr("NumChladni\n\nCopyright (c) 2013, Thomas Mueller\nVisualization Research Center\nUniversity of Stuttgart, Germany\n\nEmail: Thomas.Mueller@vis.uni-stuttgart.de") );
}

void MainWindow::Reset() {
    newPoly();
}

// ********************************* protected methods ****************************

void MainWindow::init() {
    mScriptEngine = new QScriptEngine();
    mScriptEditor = new ScriptEditor(mScriptEngine);

    initElements();
    initConnects();
    initActions();
    initMenus();


    mCurrViewImgCounter = 0;
    mCurrWinImgCounter  = 0;

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.clear();
    settings.setValue("recentFileList",files);

    initScripting();
    connect( mScriptEditor, SIGNAL(scriptExecuted()), this, SLOT(updateViews()) );

    initGUI();
}

void MainWindow::initElements() {

    QGLFormat format;
#ifdef HAVE_HIGH_END_GPU
    format.setVersion(4,1);
#else
    format.setVersion(3,3);
#endif
    format.setProfile(QGLFormat::CoreProfile);

    mData = new SystemData();
    mOpenGL = new OpenGL(format,mData);
    mControl = new SystemView(mData,mOpenGL);
    mCtrlMesh = new ControlMesh(mData,mOpenGL);
    mOGLProps = new OGLProps(mData,mOpenGL);

    mData->led_status->setReadOnly(true);
    mData->led_status->setEnabled(false);
    mData->led_status->setMaximumWidth(300);
    mData->led_status->setAlignment(Qt::AlignRight);

    lab_numMeshVertices = new QLabel("#Verts:");
    lab_numTriangles    = new QLabel("#Tri:");

    mStatusBar = new QStatusBar(this);
    mStatusBar->addPermanentWidget(lab_numMeshVertices);
    mStatusBar->addPermanentWidget(mData->lcd_numMeshVertices);
    mStatusBar->addPermanentWidget(lab_numTriangles);
    mStatusBar->addPermanentWidget(mData->lcd_numTriangles);
    mStatusBar->addPermanentWidget(mData->led_status);

    mData->m_timer = new QTimer();
    mData->m_timer->setInterval(0);
    connect( mData->m_timer, SIGNAL(timeout()), mOpenGL, SLOT(timeStep()) );
}


void MainWindow::initConnects() {
    connect( mControl, SIGNAL(emitViewModusChanged()), this, SLOT(updateViews()) );
    connect( mCtrlMesh, SIGNAL(borderChanged()), mOGLProps, SLOT(UpdateView()) );

    connect( mOpenGL, SIGNAL(addCtrlPoint(double,double)),  mCtrlMesh, SLOT(addPoint(double,double)) );
    connect( mOpenGL, SIGNAL(moveCtrlPoint(double,double)), mCtrlMesh, SLOT(movePoint(double,double)) );
    connect( mOpenGL, SIGNAL(delCtrlPoint(int)),   mCtrlMesh, SLOT(delPoint(int)) );
    connect( mOpenGL, SIGNAL(checkCtrlPoint(int)), mCtrlMesh, SLOT(checkPoint(int)) );
    connect( mOpenGL, SIGNAL(setActivePoint(int)), mCtrlMesh, SLOT(setActive(int)) );
    connect( mOpenGL, SIGNAL(borderChanged()),     mOGLProps, SLOT(UpdateView()) );
    connect( mOpenGL, SIGNAL(resetBorders()),      mOGLProps, SLOT(SetOrthoReset()) );
    connect( mOpenGL, SIGNAL(emitViewChanged()),   mOGLProps, SLOT(UpdateView()) );

    connect( mOpenGL, SIGNAL(addCtrlSegment(int,int)), mCtrlMesh, SLOT(addSegment(int,int)) );
    connect( mOpenGL, SIGNAL(setActiveSegment(int,int)), mCtrlMesh, SLOT(setActiveSegment(int,int)) );
    connect( mOpenGL, SIGNAL(haveOGLParams()), mOGLProps, SLOT(SetOGLParams()) );    
}


void MainWindow::initMenus() {
    // ------------------
    //  File menu
    // ------------------
    mFileMenu = menuBar()->addMenu("&File");
    mFileMenu->addAction(QIcon(":/new.png"),"New",this,SLOT(newPoly()),Qt::CTRL|Qt::Key_N)->setIconVisibleInMenu(true);
    mFileMenu->addSeparator();
    mFileMenu->addAction(QIcon(":/open.png"),"&Load poly",this,SLOT(loadPoly()),Qt::CTRL|Qt::Key_L)->setIconVisibleInMenu(true);
    mFileMenu->addAction(QIcon(":/save.png"),"&Save poly",this,SLOT(savePoly()),Qt::CTRL|Qt::Key_S)->setIconVisibleInMenu(true);
    mActionSepRecFiles = mFileMenu->addSeparator();
    for(int i=0; i<DEF_MAX_NUM_REC_FILES; ++i) {
       mFileMenu->addAction(mActionRecentFiles[i]);
    }
    mFileMenu->addSeparator();
    mFileMenu->addAction("Grab window",this,SLOT(saveWindow()),Qt::CTRL|Qt::SHIFT|Qt::Key_G);
    mFileMenu->addAction("Grab view",this,SLOT(saveView()),Qt::CTRL|Qt::SHIFT|Qt::Key_V);
    mFileMenu->addAction("Reset counter",this,SLOT(resetCounter()),Qt::CTRL|Qt::SHIFT|Qt::Key_R);
    mFileMenu->addSeparator();
    mFileMenu->addAction(QIcon(":/exit.png"),"Quit",this,SLOT(close()),Qt::CTRL|Qt::Key_Q)->setIconVisibleInMenu(true);


    mViewMenu = menuBar()->addMenu("&View");
    mViewMenu->addAction(mActionShowControl);
    mViewMenu->addAction(mActionShowPoints);
    mViewMenu->addAction(mActionShowOGLProps);
    mViewMenu->addSeparator();
    mViewMenu->addAction("Set &bg color",mOpenGL,SLOT(setBGColor()), Qt::CTRL|Qt::Key_B);


    mScriptMenu = menuBar()->addMenu("&Script");
    mScriptMenu->addAction(mActionShowScriptEditor);
    mScriptMenu->addSeparator();
    mScriptMenu->addAction("&Load script",mScriptEditor,SLOT(loadScript()),Qt::CTRL|Qt::SHIFT|Qt::Key_L);
    mScriptMenu->addAction("&Save script",mScriptEditor,SLOT(saveScript()),Qt::CTRL|Qt::SHIFT|Qt::Key_S);

    mHelpMenu = menuBar()->addMenu("&Help");
    mHelpMenu->addAction(QIcon(":/about.png"),"&About",this,SLOT(about()));
}


void MainWindow::initGUI() {
    setCentralWidget(mOpenGL);
    setStatusBar(mStatusBar);

    addDockWidget(Qt::RightDockWidgetArea,mControl);
    addDockWidget(Qt::RightDockWidgetArea,mCtrlMesh);
    addDockWidget(Qt::LeftDockWidgetArea,mScriptEditor);
    mScriptEditor->hide();
}


void MainWindow :: initActions() {
    mActionPlay = new QAction("Play",this);
    mActionPlay->setShortcut(Qt::CTRL|Qt::Key_P);
    addAction( mActionPlay );
    connect  ( mActionPlay, SIGNAL(triggered()), mControl, SLOT(Play()) );

    for(int i=0; i<DEF_MAX_NUM_REC_FILES; i++) {
        mActionRecentFiles[i] = new QAction(this);
        mActionRecentFiles[i]->setVisible(false);
        connect(mActionRecentFiles[i],SIGNAL(triggered()),this,SLOT(openRecentFile()));
    }

    mActionShowControl = new QAction(QIcon(":/check.png"),"Show &Control",this);
    mActionShowControl->setShortcut(Qt::CTRL|Qt::Key_C);
    mActionShowControl->setIconVisibleInMenu(true);
    connect(mActionShowControl,SIGNAL(triggered()),this,SLOT(showControl()));

    mActionShowPoints = new QAction(QIcon(":/check.png"),"Show &Points",this);
    mActionShowPoints->setShortcut(Qt::CTRL|Qt::Key_M);
    mActionShowPoints->setIconVisibleInMenu(true);
    connect(mActionShowPoints,SIGNAL(triggered()),this,SLOT(showControlPoints()));

    mActionShowOGLProps = new QAction(QIcon(":/check.png"),"&OGLProps",this);
    mActionShowOGLProps->setShortcut(Qt::CTRL|Qt::Key_O);
    mActionShowOGLProps->setIconVisibleInMenu(false);
    connect(mActionShowOGLProps,SIGNAL(triggered()),this,SLOT(showGridProps()));

    mActionExecScript = new QAction(this);
    mActionExecScript->setShortcut(Qt::CTRL|Qt::Key_E);
    addAction(mActionExecScript);
    connect(mActionExecScript,SIGNAL(triggered()),this,SLOT(execScript()));

    mActionShowScriptEditor = new QAction(QIcon(":/check.png"),"Show &Editor",this);
    mActionShowScriptEditor->setShortcut(Qt::CTRL|Qt::Key_Y);
    mActionShowScriptEditor->setIconVisibleInMenu(false);
    connect(mActionShowScriptEditor,SIGNAL(triggered()),this,SLOT(showScriptEditor()));
}


void MainWindow::initScripting() {
    QScriptValue bv = mScriptEngine->newQObject(this);
    mScriptEngine->globalObject().setProperty("Main",bv);

    //QScriptValue ti = mScriptEngine->newQObject(mSysData->m_timer);
    //mScriptEngine->globalObject().setProperty("Timer",ti);

    mOpenGL->AddObjectsToScriptEngine(mScriptEngine);
    mCtrlMesh->AddObjectsToScriptEngine(mScriptEngine);
    mControl->AddObjectsToScriptEngine(mScriptEngine);
    mOGLProps->AddObjectsToScriptEngine(mScriptEngine);

    // ----------------------------
    //   global functions defs
    // ----------------------------
    QScriptValue f_toRadians = mScriptEngine->newFunction(func_toRadians);
    mScriptEngine->globalObject().setProperty("toRadians",f_toRadians);

    QScriptValue f_toDegree = mScriptEngine->newFunction(func_toDegree);
    mScriptEngine->globalObject().setProperty("toDegree",f_toDegree);
}

void MainWindow::closeEvent( QCloseEvent * event ) {
    //fprintf(stderr,"CloseAll\n");
    QApplication::closeAllWindows();
    event->accept();
}

void MainWindow::newPoly() {
    mCtrlMesh->delAllPoints();
    mCtrlMesh->delAllSegments();
    mCtrlMesh->delAllHoles();
    mData->ClearAll();
    mControl->SetTimer(false);
    mControl->ResetParams();
    mOpenGL->DeleteMeshBuffers();
    mOpenGL->updateGL();

    //QKeyEvent* ke = new QKeyEvent(QEvent::KeyPress,Qt::Key_1,Qt::ControlModifier);
    //QApplication::postEvent(this, ke);
    mControl->SetViewModus("Input");
}

void MainWindow::loadPoly() {
#ifdef _WIN32
    QString modelsPath = QCoreApplication::applicationDirPath() + QString("/../models");
#else
    QString modelsPath = QCoreApplication::applicationDirPath() + QString("/models");
#endif

    QFileDialog dialog(this,QString("Load poly file"),modelsPath);
    dialog.setProxyModel(new FileFilterProxyModel());
    dialog.setNameFilter("*.poly");
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.size()>0) {
            mCtrlMesh->LoadPoly(fileNames[0]);

            QSettings settings;
            QStringList files = settings.value("recentFileList").toStringList();
            files.removeAll(fileNames[0]);
            files.prepend(fileNames[0]);
            while(files.size()>DEF_MAX_NUM_REC_FILES) {
               files.removeLast();
            }
            settings.setValue("recentFileList",files);
            updateRecentFileActions();
            mControl->SetViewModus(e_viewInput);
            mControl->ResetParams();
        }
    }
}


void MainWindow::savePoly() {
#ifdef _WIN32
    QString modelsPath = QCoreApplication::applicationDirPath() + QString("/../models");
#else
    QString modelsPath = QCoreApplication::applicationDirPath() + QString("/models");
#endif
    QString filename = QFileDialog::getSaveFileName(this,tr("Save poly file"),modelsPath,"*.poly");

    if (filename!=QString()) {
        if (!filename.endsWith(".poly")) {
            filename.append(".poly");
        }
        mData->SavePoly(filename);
    }

#if 0
    QFileDialog dialog(this,QString("Save poly file"),cdir.absolutePath());
    dialog.setProxyModel(new FileFilterProxyModel());
    dialog.setNameFilter("*.poly");
    dialog.setFileMode(QFileDialog::ExistingFile);
    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.size()>0) {
            if (!filename.endsWith(".poly")) {
                filename.append(".poly");
            }
            mData->SavePoly(filename);
        }
    }
#endif
}

void MainWindow::showControl() {
    if (mControl->isVisible()) {
        mControl->hide();
    } else {
        mControl->show();
    }
    mActionShowControl->setIconVisibleInMenu(mControl->isVisible());
}

void MainWindow::showControlPoints() {
    if (mCtrlMesh->isVisible()) {
        mCtrlMesh->hide();
    } else {
        mCtrlMesh->show();
    }
    mActionShowPoints->setIconVisibleInMenu(mCtrlMesh->isVisible());
}

void MainWindow::showGridProps() {
    if (mOGLProps->isVisible()) {
        mOGLProps->hide();
    } else {
        mOGLProps->show();
    }
    mActionShowOGLProps->setIconVisibleInMenu(mOGLProps->isVisible());
}

void MainWindow::showScriptEditor() {
    if (mScriptEditor->isVisible()) {
        mScriptEditor->hide();
    } else {
        mScriptEditor->show();
    }
    mActionShowScriptEditor->setIconVisibleInMenu(mScriptEditor->isVisible());
}

void MainWindow::execScript() {
    newPoly();
    mScriptEditor->execScript();
}

void MainWindow::updateViews() {
    if (mData->m_viewModus==e_viewInput) {
        mCtrlMesh->setEnabled(true);
    } else {
        mCtrlMesh->setEnabled(false);
    }
    //mCtrlMesh->setActive(-1);
}

void MainWindow::openRecentFile() {
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        mCtrlMesh->LoadPoly(action->data().toString());
    }
}

void MainWindow::updateRecentFileActions() {
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecFiles = qMin(files.size(),(int)DEF_MAX_NUM_REC_FILES);
    for(int i=0; i<numRecFiles;++i) {
        //QString text = tr("&%1 %2").arg(i+1).arg(strippedName(files[i]));
        QString text = tr("&%1 %2").arg(i+1).arg(QFileInfo(files[i]).fileName());
        mActionRecentFiles[i]->setText(text);
        mActionRecentFiles[i]->setData(files[i]);
        mActionRecentFiles[i]->setVisible(true);
    }
    for(int j=numRecFiles;j<DEF_MAX_NUM_REC_FILES;++j) {
        mActionRecentFiles[j]->setVisible(false);
    }
    mActionSepRecFiles->setVisible(numRecFiles>0);
}

void MainWindow::saveWindow() {
    QRect r = this->geometry();
    QPixmap pm = QPixmap::grabWindow(QApplication::desktop()->winId(),r.left(),r.top(),width(),height());
#ifdef _WIN32
    QString filename = QApplication::applicationDirPath() + QString("/../output/numchladni_screenshot_%1.png").arg(mCurrWinImgCounter);
#else
    QString filename = QApplication::applicationDirPath() + QString("/output/numchladni_screenshot_%1.png").arg(mCurrWinImgCounter);
#endif
    fprintf(stderr,"Grab window -> %s\n",filename.toStdString().c_str());
    pm.save(filename);
    mCurrWinImgCounter++;
}

void MainWindow::saveView() {
    QImage img = mOpenGL->grabFrameBuffer();
#ifdef _WIN32
    QString filename = QApplication::applicationDirPath() + QString("/../output/numchladni_framebuffer_%1.png").arg(mCurrViewImgCounter);
#else
    QString filename = QApplication::applicationDirPath() + QString("/output/numchladni_framebuffer_%1.png").arg(mCurrViewImgCounter);
#endif
    fprintf(stderr,"Grab view -> %s\n",filename.toStdString().c_str());
    img.save(filename);
    mCurrViewImgCounter++;
}

void MainWindow::resetCounter() {
    fprintf(stderr,"Reset counters...\n");
    mCurrWinImgCounter = 0;
    mCurrViewImgCounter = 0;
}
