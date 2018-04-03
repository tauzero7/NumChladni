/**
    @file   MainWindow.h

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

#ifndef NUMCHLADNI_MAINWINDOW_H
#define NUMCHLADNI_MAINWINDOW_H

#include <ControlMesh.h>
#include <ScriptEditor.h>
#include <SystemData.h>
#include <SystemView.h>
#include <OpenGL.h>
#include <OGLProps.h>

#include <qtdefs.h>

#include <QDockWidget>
#include <QMainWindow>
#include <QStatusBar>


/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

// --------- public methods -----------
public:
   /** This property holds the recommended minimum size for the widget.
    */
    virtual QSize minimumSizeHint() const;

    /** This property holds the recommended size for the widget.
    */
    virtual QSize sizeHint() const;

    void   IgnoreTessShaderAvailability();


// --------- protected methods -----------
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
    void initConnects();

   /** Initialize menus.
    */
    void initMenus();

    /** Initialize scripting.
     */
    void  initScripting  ( );

    virtual void closeEvent(QCloseEvent *event);


    // --------- public slots -----------
public slots:
    /** Quit program.
    */
    void quit();

    /** Show about message.
    */
    void about();

    void Reset();

protected slots:
    void newPoly();
    void loadPoly();
    void savePoly();
    void showControl();
    void showControlPoints();
    void showGridProps();
    void showScriptEditor();
    void execScript();
    void updateViews();
    void openRecentFile();
    void saveWindow();
    void saveView();
    void resetCounter();

protected:
    void updateRecentFileActions();

// -------- private attributes --------
private:
    SystemData*     mData;
    SystemView*     mControl;
    OpenGL*         mOpenGL;
    OGLProps*       mOGLProps;
    ControlMesh*    mCtrlMesh;

    QStatusBar*     mStatusBar;
    QLabel*         lab_numMeshVertices;
    QLabel*         lab_numTriangles;

    // ---- File Menu ----
    QMenu*       mFileMenu;
    QMenu*       mRecentFiles;
    QAction*     mActionSepRecFiles;
    QAction*     mActionRecentFiles[DEF_MAX_NUM_REC_FILES];

    // ---- View Menu ----
    QMenu*       mViewMenu;
    QAction*     mActionShowControl;
    QAction*     mActionShowPoints;
    QAction*     mActionShowOGLProps;

    QMenu*       mScriptMenu;
    QAction*     mActionShowScriptEditor;

    // ---- Help menu ----
    QMenu*       mHelpMenu;
    QAction*     mActionAbout;

    // ---- QtScript ----
    QScriptEngine*  mScriptEngine;
    ScriptEditor*   mScriptEditor;
    QAction*        mActionGoToCmdLine;
    QAction*        mActionExecScript;

    QAction*     mActionPlay;

    int  mCurrViewImgCounter;
    int  mCurrWinImgCounter;
};

#endif // NUMCHLADNI_MAINWINDOW_H
