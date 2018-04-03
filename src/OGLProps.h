/**
    @file   OGLProps.h

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

#ifndef NUMCHLADNI_OGL_PROPS_H
#define NUMCHLADNI_OGL_PROPS_H

#include <DoubleEdit.h>
#include <OpenGL.h>
#include <SystemData.h>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

/**
 * @brief The OpenGL properties class
 */
class OGLProps : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( double  left      READ  GetLeft      WRITE  SetLeft )
    Q_PROPERTY( double  right     READ  GetRight     WRITE  SetRight )
    Q_PROPERTY( double  bottom    READ  GetBottom    WRITE  SetBottom )
    Q_PROPERTY( double  top       READ  GetTop       WRITE  SetTop )
    Q_PROPERTY( double  stepX     READ  GetStepX     WRITE  SetStepX )
    Q_PROPERTY( double  stepY     READ  GetStepY     WRITE  SetStepY )
    Q_PROPERTY( bool    grid      READ  GetGrid      WRITE  SetGrid )
    Q_PROPERTY( bool    aspect    READ  GetAspect    WRITE  SetAspect )

    Q_PROPERTY( bool    color     READ  GetColor     WRITE  SetColor )
    Q_PROPERTY( bool    mesh      READ  GetMesh      WRITE  SetMesh )
    Q_PROPERTY( bool    isolines  READ  GetIsolines  WRITE  SetIsolines )
    Q_PROPERTY( double  opacity   READ  GetOpacity   WRITE  SetOpacity)

    Q_PROPERTY( float   fov       READ  GetFOV       WRITE  SetFOV )
    Q_PROPERTY( float   dist      READ  GetDist      WRITE  SetDist )
    Q_PROPERTY( float   theta     READ  GetTheta     WRITE  SetTheta )
    Q_PROPERTY( float   phi       READ  GetPhi       WRITE  SetPhi )
    Q_PROPERTY( float   poiX      READ  GetPoiX      WRITE  SetPoiX )
    Q_PROPERTY( float   poiY      READ  GetPoiY      WRITE  SetPoiY )
    Q_PROPERTY( float   poiZ      READ  GetPoiZ      WRITE  SetPoiZ )
    Q_PROPERTY( bool    box       READ  GetBox       WRITE  SetBox )
    Q_PROPERTY( int     tessIn    READ  GetTessIn    WRITE  SetTessIn )
    Q_PROPERTY( int     tessOut   READ  GetTessOut   WRITE  SetTessOut )
    Q_PROPERTY( bool    wireframe READ  GetWireframe WRITE  SetWireframe )
    Q_PROPERTY( bool    dotprod   READ  GetDotProd   WRITE  SetDotProd )
    Q_PROPERTY( QString shading   READ  GetShading   WRITE  SetShading )

public:
    /** Standard constructor.
     * @param sd pointer to system data widget.
     * @param ogl pointer to main opengl widget.
     * @param parent  pointer to parent widget.
     */
    OGLProps( SystemData* sd, OpenGL* ogl, QWidget* parent = 0 );

    /** Standard destructor.
     */
    virtual ~OGLProps();

    void AddObjectsToScriptEngine( QScriptEngine* engine );

// ------------ public methods -------------
public:

// ------------ public slots -------------
public slots:
    void UpdateView();
    void SetOGLParams();

    void setOrthoBorder();
    void setGridStyle();

    void SetOrthoReset();
    void SetProjReset();

    double  GetLeft();
    void    SetLeft(double val);
    double  GetRight();
    void    SetRight(double val);
    double  GetBottom();
    void    SetBottom(double val);
    double  GetTop();
    void    SetTop(double val);
    double  GetStepX();
    void    SetStepX(double val);
    double  GetStepY();
    void    SetStepY(double val);
    bool    GetGrid();
    void    SetGrid(bool val);
    bool    GetAspect();
    void    SetAspect(bool val);

    bool    GetColor();
    void    SetColor(bool val);
    bool    GetMesh();
    void    SetMesh(bool val);
    bool    GetIsolines();
    void    SetIsolines(bool val);
    double  GetOpacity();
    void    SetOpacity(double val);

    float   GetFOV();
    void    SetFOV(float val);
    float   GetDist();
    void    SetDist(float val);
    float   GetTheta();
    void    SetTheta(float val);
    float   GetPhi();
    void    SetPhi(float val);
    float   GetPoiX();
    void    SetPoiX(float val);
    float   GetPoiY();
    void    SetPoiY(float val);
    float   GetPoiZ();
    void    SetPoiZ(float val);
    bool    GetBox();
    void    SetBox(bool val);
    int     GetTessIn();
    void    SetTessIn(int val);
    int     GetTessOut();
    void    SetTessOut(int val);
    bool    GetWireframe();
    void    SetWireframe(bool val);
    bool    GetDotProd();
    void    SetDotProd(bool val);
    QString GetShading();
    void    SetShading(QString val);

protected slots:
    void setMeshParams();
    void setMeshOpacity(int);    
    void setCameraParams();
    void setViewParams();

// ------------ signals -------------
signals:

 
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

    void setOrthoBlockStatus( bool status );
    void setProjBlockStatus( bool status );
 
// ----------- private attributes ----------
private:
    SystemData*   mData;
    OpenGL*       mOpenGL;

    QAction*      mActionClose;
    QPushButton*  pub_close;

    QLabel*       lab_left;
    DoubleEdit*   led_left;
    QLabel*       lab_right;
    DoubleEdit*   led_right;
    QLabel*       lab_bottom;
    DoubleEdit*   led_bottom;
    QLabel*       lab_top;
    DoubleEdit*   led_top;
    QLabel*       lab_stepX;
    DoubleEdit*   led_stepX;
    QLabel*       lab_stepY;
    DoubleEdit*   led_stepY;
    QCheckBox*    chb_grid;
    QCheckBox*    chb_keepAspect;
    QPushButton*  pub_reset;

    QLabel*       lab_meshOpacity;
    QSlider*      sli_meshOpacity;
    QCheckBox*    chb_useColor;
    QCheckBox*    chb_useMeshOnly;
    QCheckBox*    chb_showIsolines;

    // camera
    QLabel*       lab_proj_fov;
    DoubleEdit*   led_proj_fov;
    QLabel*       lab_proj_distance;
    DoubleEdit*   led_proj_distance;
    QLabel*       lab_proj_theta;
    DoubleEdit*   led_proj_theta;
    QLabel*       lab_proj_phi;
    DoubleEdit*   led_proj_phi;
    QLabel*       lab_proj_poiX;
    DoubleEdit*   led_proj_poiX;
    QLabel*       lab_proj_poiY;
    DoubleEdit*   led_proj_poiY;
    QLabel*       lab_proj_poiZ;
    DoubleEdit*   led_proj_poiZ;
    QPushButton*  pub_proj_reset;
    QCheckBox*    chb_show_box;


    QLabel*       lab_tessLevelInOut;
    QSpinBox*     spb_tessLevelInner;
    QSpinBox*     spb_tessLevelOuter;
    QLabel*       lab_shading;
    QComboBox*    cob_shading;
    QCheckBox*    chb_wireframe;
    QCheckBox*    chb_useDotProd;
};

#endif // NUMCHLADNI_OGL_PROPS_H
