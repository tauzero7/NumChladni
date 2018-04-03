/**
    @file   DoubleEdit.h

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

#ifndef DOUBLE_EDIT_H
#define DOUBLE_EDIT_H

#include <qtdefs.h>

#include <QLineEdit>

/**
 * @brief Specialization of QLineEdit for double values only
 */
class DoubleEdit : public QLineEdit
{
    Q_OBJECT

public:
    DoubleEdit ( int prec, double val = 0.0, double stp = 0.01, QWidget* parent = 0 );
    ~DoubleEdit ( );

    // --------- public methods -----------
public:
    /** Set value.
     * @param value new value.
     */
    void    setValue( double value );

    /** Get value.
     * @return  value.
     */
    double  getValue();

    /** Set stepsize.
     * @param step  new stepsize.
     */
    void    setStep( double step );

    /** Get stepsize.
     * @return  stepsize.
     */
    double  getStep();

    /** Set value and stepsize.
     * @param value  new value.
     * @param step   new step.
     */
    void    setValueAndStep ( double val, double stp );

    /** Get value and stepsize.
     * @param value  reference to value.
     * @param step   reference to step.
     */
    void    getValueAndStep ( double &val, double &stp );

    /** Set range for input.
     * @param min  range minimum.
     * @param max  range maximum.
     */
    void    setRange ( double min, double max );

    /** Get range for input.
     * @param min : reference to range minimum.
     * @param max : reference to range maximum.
     */
    void    getRange ( double &min, double &max );

    // --------- public slots -----------
public slots:
    void    slot_setValue ( );
    void    slot_setStep  ( );


    // --------- protected methods -----------
protected:
    /** Handle key press event.
     *  @param event  key event.
     */
    virtual void  keyPressEvent       ( QKeyEvent* event );

    /** Wheel event handling.
     * @param event  pointer to QWheelEvent.
     */
    virtual void  wheelEvent          ( QWheelEvent* event );
    
    // ------ protected attributes --------
protected:
    int    mPrecision;
    double mStep;
    double mMin;
    double mMax;
};

#endif // DOUBLE_EDIT_H
