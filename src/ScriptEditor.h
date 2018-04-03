/**
    @file   ScriptEditor.h

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

#ifndef SCRIPT_EDITOR_H
#define SCRIPT_EDITOR_H

#include <SyntaxHighlighter.h>

#include <QDockWidget>
#include <QLabel>
#include <QLineEdit>
#include <QScriptEngine>
#include <QPushButton>
#include <QTextEdit>

#define  MAX_NUM_COMMANS_IN_LIST  100

/**
  * @brief Script command line widget.
  *
  * The script command line is based on the QLineEdit widget.
  */
class ScriptCmdLine : public QLineEdit
{
    Q_OBJECT

public:
    ScriptCmdLine( QScriptEngine* se, QLineEdit* le, QWidget *parent = 0);

    void  execCommand( QString cmd );

protected:
    virtual void  keyPressEvent ( QKeyEvent * e );

public slots:
    void  execCmdLine();

signals:
    void  scriptExecuted();

private:
    QScriptEngine*  mScriptEngine;
    QLineEdit*      led_scriptResult;
    QStringList     mLastCommands;
    int             mCurrCommand;
};

/**
  *  @brief Script editor widget.
  *
  *  The script editor is based on the QDockWidget.
  */
class ScriptEditor : public QDockWidget
{
    Q_OBJECT

public:
    ScriptEditor( QScriptEngine* se, QWidget *parent = 0);

    /** Add this object to the script engine with name 'SE'.
     *  @param engine  main script engine.
     */
    void  addObjectsToScriptEngine ( QScriptEngine* engine );

    void ExecCommand( QString cmd );

public slots:        
    /** Load script slot with QFileDialog.
     */
    void  loadScript();

    /** Save script slot.
     */
    void  saveScript();

    /** Load script slot.
     * @param filename  script file name
     */
    void  loadScript( QString filename );

    /**  Execute script.
     * @\param filename  script file name.
     */
    void  execScript( QString filename );

    /** Execute script from text editor.
     */
    void  execScript();

    /** Clear script editor text field.
     */
    void  clearScript();

    /** Select system via menu.
     */
    void  selectSystem();

    /** Set focus to command line.
     */
    void  setFocToCmdLine();

    /** Set command line text.
     * @param cmd  command line text
     */
    void  setCmdLineText( QString cmd );

signals:
    void  scriptExecuted();

protected slots:
    void  haveNewCmd();

protected:
    /** Initialize view.
     */
    void  init();

    /** Initialize view elements.
     */
    void  initElements();

    /** Construct graphical user interface.
     */
    void  initGUI();

    /** Initialize actions.
     */
    void  initActions();

    /** Connect signals and slots.
     */
    void  initControl();

    virtual QSize  sizeHint () const;

private:
    QScriptEngine*  mScriptEngine;
    JsHighlighter*  mSyntaxHighlighter;

    QLabel*         lab_scriptName;
    QLineEdit*      led_scriptName;
    QTextEdit*      ted_scriptText;
    QLabel*         lab_scriptCmdLine;
    ScriptCmdLine*  led_scriptCmdLine;
    QLabel*         lab_scriptResult;
    QLineEdit*      led_scriptResult;

    QPushButton*    pub_execScript;
    QPushButton*    pub_clearScript;
};

#endif // SCRIPT_EDITOR_H
