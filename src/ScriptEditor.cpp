/**
    @file   ScriptEditor.cpp

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

#include "ScriptEditor.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTextStream>

ScriptCmdLine::ScriptCmdLine( QScriptEngine* se, QLineEdit* le, QWidget*)
{
    mScriptEngine = se;
    led_scriptResult = le;
    mCurrCommand = 0;
    connect( this, SIGNAL(returnPressed()), this, SLOT(execCmdLine()) );
}


/*! Handle key press event
 * \param e
 */
void ScriptCmdLine::keyPressEvent( QKeyEvent * e )
{
    if (e->key()==Qt::Key_Up) {
        mCurrCommand--;
        if (mCurrCommand<0) {
            mCurrCommand = 0;
        }
        if (mCurrCommand<mLastCommands.size()) {
            this->setText(mLastCommands[mCurrCommand]);
        }
    } else if (e->key()==Qt::Key_Down) {
        mCurrCommand++;
        if (mCurrCommand>=mLastCommands.size()) {
            mCurrCommand = mLastCommands.size();
        }
        if (mCurrCommand==mLastCommands.size()) {
            this->setText("");
        } else {
            this->setText(mLastCommands[mCurrCommand]);
        }
    } else {
        QLineEdit::keyPressEvent(e);
    }
}

void ScriptCmdLine::execCommand( QString cmd )
{
    QScriptValue result = mScriptEngine->evaluate(cmd);
    if (result.isError()) {
        QMessageBox::critical(0, "Executing QtScript",
                              QString::fromLatin1("%1: %2")
                              .arg(result.property("lineNumber").toInt32())
                              .arg(result.toString()));
    } else if (result.isNumber() || result.isString()) {
        led_scriptResult->setText(result.toString());
    } else {
        led_scriptResult->setText("Ok");
    }

    mLastCommands << cmd;
    mCurrCommand = mLastCommands.size();
    if (mLastCommands.size()>MAX_NUM_COMMANS_IN_LIST) {
        mLastCommands.removeFirst();
        mCurrCommand = MAX_NUM_COMMANS_IN_LIST;
    }
}

void ScriptCmdLine::execCmdLine()
{
    QString command = this->text();
    if (command==QString()) {
        return;
    }
    this->setText("");
    execCommand(command);
    emit scriptExecuted();
}

// -------------------------------------------------------------
//
// -------------------------------------------------------------
ScriptEditor::ScriptEditor( QScriptEngine* se, QWidget*) {
    mScriptEngine = se;
    init();
}

void ScriptEditor :: addObjectsToScriptEngine( QScriptEngine* engine ) {
    QScriptValue ogl = engine->newQObject(this);
    engine->globalObject().setProperty("SE",ogl);
}

void ScriptEditor::ExecCommand( QString cmd ) {
    led_scriptCmdLine->execCommand(cmd);
    emit scriptExecuted();
}

void ScriptEditor::loadScript() {
#ifdef _WIN32
    QString appDirPath = QCoreApplication::applicationDirPath() + QString("/../jscripts");
#else
    QString appDirPath = QCoreApplication::applicationDirPath() + QString("/jscripts");
#endif
    QString filename = QFileDialog::getOpenFileName(this,tr("Load script file"),appDirPath,"*.js");

    if (filename==QString()) {
        return;
    }
    this->loadScript(filename);
}

void ScriptEditor::loadScript( QString filename ) {
    QFile scriptFile(filename);
    scriptFile.open(QIODevice::ReadOnly);
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    ted_scriptText->setPlainText(contents);

    QString name = filename.right(filename.length()-filename.lastIndexOf("/")-1);
    led_scriptName->setText(name);
}

void ScriptEditor::saveScript() {
#ifdef _WIN32
    QString appDirPath = QCoreApplication::applicationDirPath() + QString("/../jscripts");
#else
    QString appDirPath = QCoreApplication::applicationDirPath() + QString("/jscripts");
#endif
    //std::cerr << appDirPath.toStdString() << " " << led_scriptName->text().toStdString() << std::endl;
    QString filename = QFileDialog::getSaveFileName(this,tr("Save script file"),appDirPath + "/" + led_scriptName->text(),"*.js");

    if (filename==QString())
        return;

    if (!filename.endsWith(".js"))
        filename.append(".js");

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        fprintf(stderr,"Cannot open file %s for output.\n",filename.toStdString().c_str());
        return;
    }
    QTextStream out(&file);
    out << ted_scriptText->toPlainText();
    file.close();
}

void ScriptEditor::clearScript() {
    led_scriptName->setText("");
    ted_scriptText->clear();
}

void ScriptEditor::execScript() {
    QString comm = ted_scriptText->toPlainText();
    if (comm==QString()) {
        return;
    }

    QScriptValue result = mScriptEngine->evaluate(comm);
    if (result.isError()) {
        QMessageBox::critical(0, "Executing QtScript",
                              QString::fromLatin1("%1: %2")
                              .arg(result.property("lineNumber").toInt32())
                              .arg(result.toString()));
    } else if (result.isNumber() || result.isString()) {
        led_scriptResult->setText(result.toString());
    } else {
        led_scriptResult->setText("Ok");
    }
    emit scriptExecuted();
}

void ScriptEditor::execScript( QString filename ) {
    QFile scriptFile(filename);
    scriptFile.open(QIODevice::ReadOnly);
    QTextStream stream(&scriptFile);
    QString contents = stream.readAll();
    scriptFile.close();

    QScriptValue result = mScriptEngine->evaluate(contents,filename);
    if (result.isError()) {
        QMessageBox::critical(0, "Executing QtScript",
                              QString::fromLatin1("%0:%1: %2")
                              .arg(filename)
                              .arg(result.property("lineNumber").toInt32())
                              .arg(result.toString()));
    } else {
        led_scriptResult->setText("Ok");
    }
    emit scriptExecuted();
}

void ScriptEditor::selectSystem() {
    QObject* obj = sender();
    led_scriptCmdLine->execCommand("System.set(\"" + obj->objectName() + "\");");
    emit scriptExecuted();
}

void ScriptEditor::setFocToCmdLine() {
    led_scriptCmdLine->setFocus();
}

void ScriptEditor::setCmdLineText( QString cmd ) {
    led_scriptCmdLine->setText(cmd);
}

void ScriptEditor::haveNewCmd() {
    emit scriptExecuted();
}

void ScriptEditor::init() {
    initElements();
    initGUI();
    initActions();
    initControl();

    //execScript(":/sprintf.js");
}

void ScriptEditor::initElements() {
    lab_scriptName = new QLabel("Name:");
    led_scriptName = new QLineEdit();
    led_scriptName->setReadOnly(true);

    ted_scriptText = new QTextEdit();
    //ted_scriptText->setStyleSheet("font: 12px \"Monospace\";");
    ted_scriptText->setWordWrapMode(QTextOption::NoWrap);

    mSyntaxHighlighter = new JsHighlighter(ted_scriptText->document());

    lab_scriptResult = new QLabel("Result:");
    led_scriptResult = new QLineEdit();
    led_scriptResult->setReadOnly(true);

    lab_scriptCmdLine = new QLabel("CmdLine");
    led_scriptCmdLine = new ScriptCmdLine(mScriptEngine,led_scriptResult);

    pub_clearScript = new QPushButton(QIcon(":/clear.png"),"");
    pub_clearScript->setMaximumWidth(40);
    pub_clearScript->setToolTip("Clear script text");
    pub_execScript = new QPushButton(QIcon(":/run.png"),"");
    pub_execScript->setMaximumWidth(40);
    pub_execScript->setToolTip("Execute script");

}

void ScriptEditor::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();
    layout_complete->addWidget( lab_scriptName, 0, 0 );
    layout_complete->addWidget( led_scriptName, 0, 1 );

    QHBoxLayout* layout_buttons = new QHBoxLayout();
    layout_buttons->addWidget(pub_clearScript);
    layout_buttons->addWidget(pub_execScript);

    layout_complete->addLayout( layout_buttons, 0, 2 );
    layout_complete->addWidget( ted_scriptText, 1, 0, 1, 3 );
    layout_complete->addWidget( lab_scriptCmdLine, 2, 0 );
    layout_complete->addWidget( led_scriptCmdLine, 2, 1, 1, 2 );
    layout_complete->addWidget( lab_scriptResult, 3, 0 );
    layout_complete->addWidget( led_scriptResult, 3, 1, 1, 2 );

    layout_complete->setRowStretch(1,3);
    layout_complete->setColumnStretch(1,2);

    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout( layout_complete );
    centralWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    setWidget(centralWidget);

    setWindowTitle("ScriptEditor");
    setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetClosable);
}

void ScriptEditor::initActions() {
}

void ScriptEditor::initControl() {
    connect( pub_clearScript, SIGNAL(pressed()), this, SLOT(clearScript()) );
    connect( pub_execScript, SIGNAL(pressed()), this, SLOT(execScript()));
    connect( led_scriptCmdLine, SIGNAL(scriptExecuted()), this, SLOT(haveNewCmd()) );
}

QSize ScriptEditor::sizeHint() const {
   return QSize(100,100);
}

