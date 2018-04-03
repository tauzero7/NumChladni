/**
    @file   DoubleEdit.cpp

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

#include <iostream>
#include <limits>

#include "DoubleEdit.h"

#include <QDoubleValidator>
#include <QKeyEvent>

/*! Standard constructor.
 *  \param prec  : precision.
 *  \param value : initial value.
 *  \param step  : stepsize.
 *  \param parent : parent widget.
 */
DoubleEdit :: DoubleEdit ( int prec, double val, double stp, QWidget* parent )
    : QLineEdit(parent)
{
    mPrecision = prec;
    mStep      = stp;
    setValue( val );
    setAlignment( Qt::AlignRight );

    mMin = (std::numeric_limits<double>::min)();
    mMax = (std::numeric_limits<double>::max)();

    //setPalette( QPalette(QColor(FLOAT_EDIT_COLOR)) );
    setValidator( new QDoubleValidator(this) );
}

/*! Standard destructor.
 */
DoubleEdit :: ~DoubleEdit() {
}


// *********************************** public methods ******************************
void DoubleEdit::setValue ( double value ) {
    setText( QString::number(value,'f',mPrecision) );
}

double DoubleEdit::getValue() {
    /*
  double val = atof(text().toStdString().c_str());
  fprintf(stderr,"%15.12f  %15.12f\n",val,text().toDouble());
*/
    return text().toDouble();
}

void DoubleEdit::setStep( double step ) {
    mStep = step;
}

double DoubleEdit::getStep() {
    return mStep;
}

void DoubleEdit::setValueAndStep(double val, double stp ) {
    setValue(val);
    setStep(stp);
}

void DoubleEdit::getValueAndStep( double &val, double &stp ) {
    val = getValue();
    stp = getStep();
}

void DoubleEdit::setRange( double min, double max ) {
    mMin = min;
    mMax = max;
}

void DoubleEdit::getRange( double &min, double &max ) {
    min = mMin;
    max = mMax;
}

// *********************************** public slots ********************************

void DoubleEdit::slot_setValue() {
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();
    setValue(value);
}

void DoubleEdit::slot_setStep() {
    QObject* obj = sender();
    DoubleEdit* led = reinterpret_cast<DoubleEdit*>(obj);
    double value = led->getValue();
    setStep(value);
}

// ********************************* protected methods *****************************

void DoubleEdit::keyPressEvent( QKeyEvent* event ) {
    double val = text().toDouble();
    if (event->key()==Qt::Key_Up) {
        val+=mStep;
    } else if (event->key()==Qt::Key_Down) {
        val-=mStep;
    } else if (event->key()==Qt::Key_PageUp) {
        val+=mStep*10.0;
    } else if (event->key()==Qt::Key_PageDown) {
        val-=mStep*10.0;
    } else {
        QLineEdit::keyPressEvent(event);
        return;
    }

    if (val>mMax) {
        val=mMax;
    }
    if (val<mMin) {
        val=mMin;
    }
    setText( QString("%1").arg(val,0,'f',mPrecision) );
    emit editingFinished();
}

void  DoubleEdit::wheelEvent( QWheelEvent* event ) {
    double val = text().toDouble();

    if (event->delta()>0) {
        val += mStep;
    } else {
        val -= mStep;
    }

    if (val>mMax) {
        val=mMax;
    }
    if (val<mMin) {
        val=mMin;
    }

    setText( QString("%1").arg(val,0,'f',mPrecision) );
    event->accept();

    //emit returnPressed();
    emit editingFinished();
}
