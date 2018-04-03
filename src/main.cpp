/**
    @file   main.cpp

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

//
//  mesh generation based on triangle.h by Jonathan Richard Shewchuk
//  http://www.cs.cmu.edu/~quake/triangle.html
//

#include <iostream>
#include "MainWindow.h"

#include <QApplication>
#include <QTextStream>

bool ignoreTessShaderAvail = false;

bool testParam( int argc, char* argv[], int n, const char* name, const int numParams ) {
    if (strcmp(argv[n],name)==0 && (n+numParams<argc)) {
        return true;
    }
    return false;
}

bool readCmdLineParams( int argc, char* argv[] ) {

    for(int nArg=1; nArg<argc; nArg++) {
        if (testParam(argc,argv,nArg,"-h",0) ||
                testParam(argc,argv,nArg,"-help",0)) {
            fprintf(stderr,"NumChladni Help\n-------------\n");
            fprintf(stderr," -h / -help : show this help\n");
            fprintf(stderr," -i         : ignore tess shader availability\n");
            fprintf(stderr,"\n");
            return false;
        } else if (testParam(argc,argv,nArg,"-i",0)) {
            ignoreTessShaderAvail = true;
            break;
        }
    }
    return true;
}


// ---------------------------------------------------
//    m a i n
// ---------------------------------------------------
int main( int argc, char *argv[] )
{
    if (!readCmdLineParams(argc,argv)) {
        return 0;
    }

    QApplication app(argc, argv);

    QString cssStyle;
    QFile cssFile(":/numchladni.css");
    if (cssFile.open(QFile::ReadOnly)) {
        QTextStream styleIn(&cssFile);
        while( !styleIn.atEnd()) {
            QString line = styleIn.readLine();
            if (!line.startsWith("#"))
                cssStyle += line;
        }
        cssFile.close();
    } else {
        fprintf(stderr,"Cannot read stylesheet!\n");
    }
    app.setStyleSheet(cssStyle);

    QLocale::setDefault(QLocale::C);
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    MainWindow w;
    w.show();

    w.move(350,150);
    //fprintf(stderr,"Decimal point: %c\n",w.locale().decimalPoint().toAscii());

    if (ignoreTessShaderAvail) {
        w.IgnoreTessShaderAvailability();
    }
    return app.exec();
}

