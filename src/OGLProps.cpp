/**
    @file   OGLProps.cpp

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

#include <limits>
#include "OGLProps.h"

#include <QAction>
#include <QGridLayout>
#include <QGroupBox>

OGLProps :: OGLProps( SystemData* sd, OpenGL* ogl, QWidget *parent )
    : QWidget(parent)
{    
    mData = sd;
    mOpenGL  = ogl;
    init();
}

OGLProps::~OGLProps() {
}

void OGLProps::AddObjectsToScriptEngine( QScriptEngine* engine ) {
    QScriptValue sv = engine->newQObject(this);
    engine->globalObject().setProperty("OGL",sv);
}

// *********************************** public methods *********************************

void OGLProps::UpdateView() {
    setOrthoBlockStatus(true);
    setProjBlockStatus(true);

    led_stepX->setValue(mData->m_gridStep.x);
    led_stepY->setValue(mData->m_gridStep.y);
    led_left->setValue(mData->m_border.x);
    led_right->setValue(mData->m_border.y);
    led_bottom->setValue(mData->m_border.z);
    led_top->setValue(mData->m_border.w);
    chb_grid->setChecked(mData->m_showGrid);
    chb_keepAspect->setChecked(mData->m_keepAspectRatio);

    led_proj_fov->setValue( mData->mCamera.GetFovY() );
    led_proj_distance->setValue( mData->mCamera.GetDistance() );
    led_proj_theta->setValue( mData->mCamera.GetTheta() );
    led_proj_phi->setValue( mData->mCamera.GetPhi() );
    glm::vec3 poi = mData->mCamera.GetPOI();
    led_proj_poiX->setValue(poi.x);
    led_proj_poiY->setValue(poi.y);
    led_proj_poiZ->setValue(poi.z);

    setOrthoBlockStatus(false);
    setProjBlockStatus(false);
}

void OGLProps::SetOGLParams() {
    spb_tessLevelInner->setRange(1,mData->m_maxPatchVertices);
    spb_tessLevelOuter->setRange(1,mData->m_maxPatchVertices);

    if (!mData->m_haveTessShader) {
        spb_tessLevelInner->setEnabled(false);
        spb_tessLevelOuter->setEnabled(false);
    }
}

// ************************************* public slots ***********************************

void OGLProps::setGridStyle() {
    mData->m_gridStep = glm::dvec2(led_stepX->getValue(),
                                   led_stepY->getValue());
    mData->m_showGrid = chb_grid->isChecked();
    mData->m_keepAspectRatio = chb_keepAspect->isChecked();
    setOrthoBorder();
}

void OGLProps::setOrthoBorder() {
    mData->m_border = glm::dvec4(led_left->getValue(), led_right->getValue(),
                                 led_bottom->getValue(), led_top->getValue());
    if (mData->m_keepAspectRatio) {
        mData->m_aspect = mData->m_screenWidth/static_cast<double>(mData->m_screenHeight);
        double centerX = 0.5*(mData->m_border.x+mData->m_border.y);
        double sizeY   = 0.5*(mData->m_border.w-mData->m_border.z);
        //std::cerr << sizeY << " " << aspect << std::endl;
        mData->m_border.x = centerX - sizeY*mData->m_aspect;
        mData->m_border.y = centerX + sizeY*mData->m_aspect;
        UpdateView();
    }
    mData->m_viewScale = glm::dvec2((mData->m_border.y - mData->m_border.x)/static_cast<double>(mData->m_screenWidth),
                                    (mData->m_border.w - mData->m_border.z)/static_cast<double>(mData->m_screenHeight) );
    mOpenGL->updateGL();
}

void OGLProps::SetOrthoReset() {
    setOrthoBlockStatus(true);
    led_left->setValue(init_ortho_left);
    led_right->setValue(init_ortho_right);
    led_bottom->setValue(init_ortho_bottom);
    led_top->setValue(init_ortho_top);
    led_stepX->setValue(init_grid_stepX);
    led_stepY->setValue(init_grid_stepY);
    setOrthoBlockStatus(false);
    //setOrthoBorder();
    setGridStyle();
}

void OGLProps::setOrthoBlockStatus( bool status ) {
    led_left->blockSignals(status);
    led_right->blockSignals(status);
    led_bottom->blockSignals(status);
    led_top->blockSignals(status);
    led_stepX->blockSignals(status);
    led_stepY->blockSignals(status);
    chb_grid->blockSignals(status);
    chb_keepAspect->blockSignals(status);

    chb_useColor->blockSignals(status);
    chb_useMeshOnly->blockSignals(status);
    chb_showIsolines->blockSignals(status);
}

double OGLProps::GetLeft() {
    return mData->m_border.x;
}

void   OGLProps::SetLeft(double val) {
    mData->m_border.x = val;
    setOrthoBlockStatus(true);
    led_left->setValue(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

double OGLProps::GetRight() {
    return mData->m_border.y;
}

void OGLProps::SetRight(double val) {
    mData->m_border.y = val;
    setOrthoBlockStatus(true);
    led_right->setValue(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

double OGLProps::GetBottom() {
    return mData->m_border.z;
}

void OGLProps::SetBottom(double val) {
    mData->m_border.z = val;
    setOrthoBlockStatus(true);
    led_bottom->setValue(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

double OGLProps::GetTop() {
    return mData->m_border.w;
}

void OGLProps::SetTop(double val) {
    mData->m_border.w = val;
    setOrthoBlockStatus(true);
    led_top->setValue(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

double OGLProps::GetStepX() {
    return mData->m_gridStep.x;
}

void OGLProps::SetStepX(double val) {
    mData->m_gridStep.x = val;
    setOrthoBlockStatus(true);
    led_stepX->setValue(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

double OGLProps::GetStepY() {
    return mData->m_gridStep.y;
}

void OGLProps::SetStepY(double val) {
    mData->m_gridStep.y = val;
    setOrthoBlockStatus(true);
    led_stepY->setValue(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

bool OGLProps::GetGrid() {
    return mData->m_showGrid;
}

void OGLProps::SetGrid(bool val) {
    mData->m_showGrid = val;
    setOrthoBlockStatus(true);
    chb_grid->setChecked(val);
    setOrthoBlockStatus(false);
    mOpenGL->updateGL();
}

bool OGLProps::GetAspect() {
    return mData->m_keepAspectRatio;
}

void OGLProps::SetAspect(bool val) {
    mData->m_keepAspectRatio = val;
    setOrthoBlockStatus(true);
    chb_keepAspect->setChecked(val);
    setOrthoBlockStatus(false);
    setOrthoBorder();
}

bool OGLProps::GetColor() {
    return mData->m_useColor;
}

void OGLProps::SetColor(bool val) {
    chb_useColor->setChecked(val);
}

bool OGLProps::GetMesh() {
    return mData->m_useMeshOnly;
}

void OGLProps::SetMesh(bool val) {
    chb_useMeshOnly->setChecked(val);
}

bool OGLProps::GetIsolines() {
    return mData->m_showIsolines;
}

void OGLProps::SetIsolines(bool val) {
    chb_showIsolines->setChecked(val);
}

double OGLProps::GetOpacity() {
    return static_cast<double>(mData->m_meshOpacity);
}

void OGLProps::SetOpacity(double val) {
    sli_meshOpacity->setValue(static_cast<int>(val*100));
}

float OGLProps::GetFOV() {
    return mData->mCamera.GetFovY();
}

void OGLProps::SetFOV(float val) {
    mData->mCamera.SetFovY(val);
    setProjBlockStatus(true);
    led_proj_fov->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

float OGLProps::GetDist() {
    return mData->mCamera.GetDistance();
}

void OGLProps::SetDist(float val) {
    mData->mCamera.SetDistance(val);
    setProjBlockStatus(true);
    led_proj_distance->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

float OGLProps::GetTheta() {
    return mData->mCamera.GetTheta();
}

void OGLProps::SetTheta(float val) {
    mData->mCamera.SetTheta(val);
    setProjBlockStatus(true);
    led_proj_theta->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

float OGLProps::GetPhi() {
    return mData->mCamera.GetPhi();
}

void OGLProps::SetPhi(float val) {
    mData->mCamera.SetPhi(val);
    setProjBlockStatus(true);
    led_proj_phi->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

float OGLProps::GetPoiX() {
    return mData->mCamera.GetPOI().x;
}

void OGLProps::SetPoiX(float val) {
    glm::vec3 poi = mData->mCamera.GetPOI();
    mData->mCamera.SetPOI(val,poi.y,poi.z);
    setProjBlockStatus(true);
    led_proj_poiX->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

float OGLProps::GetPoiY() {
    return mData->mCamera.GetPOI().y;
}

void OGLProps::SetPoiY(float val) {
    glm::vec3 poi = mData->mCamera.GetPOI();
    mData->mCamera.SetPOI(poi.x,val,poi.z);
    setProjBlockStatus(true);
    led_proj_poiY->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

float OGLProps::GetPoiZ() {
    return mData->mCamera.GetPOI().z;
}

void OGLProps::SetPoiZ(float val) {
    glm::vec3 poi = mData->mCamera.GetPOI();
    mData->mCamera.SetPOI(poi.x,poi.y,val);
    setProjBlockStatus(true);
    led_proj_poiZ->setValue(val);
    setProjBlockStatus(false);
    setCameraParams();
}

bool OGLProps::GetBox() {
    return mData->m_showBox;
}

void OGLProps::SetBox(bool val) {
    chb_show_box->setChecked(val);
}

int OGLProps::GetTessIn() {
    return mData->m_tessLevelInner;
}

void OGLProps::SetTessIn(int val) {
    spb_tessLevelInner->setValue(val);
}

int OGLProps::GetTessOut() {
    return mData->m_tessLevelOuter;
}

void OGLProps::SetTessOut(int val) {
    spb_tessLevelOuter->setValue(val);
}


bool OGLProps::GetWireframe() {
    return mData->m_wireframe;
}

void OGLProps::SetWireframe(bool val) {
    chb_wireframe->setChecked(val);
}

bool OGLProps::GetDotProd() {
    return mData->m_useDotProd;
}

void OGLProps::SetDotProd(bool val) {
    chb_useDotProd->setChecked(val);
}

QString OGLProps::GetShading() {
    return stl_shading[cob_shading->currentIndex()];
}

void OGLProps::SetShading(QString val) {
    for(int i=0; i<stl_shading.size(); i++) {
        if (val.compare(stl_shading[i])==0) {
            cob_shading->setCurrentIndex(i);
            break;
        }
    }
}


void OGLProps::setCameraParams() {
    mData->mCamera.SetPOI(static_cast<float>(led_proj_poiX->getValue()),
                          static_cast<float>(led_proj_poiY->getValue()),
                          static_cast<float>(led_proj_poiZ->getValue()));

    mData->mCamera.SetFovY(static_cast<float>(led_proj_fov->getValue()));
    mData->mCamera.SetDistance(static_cast<float>(led_proj_distance->getValue()));
    mData->mCamera.SetTheta(static_cast<float>(led_proj_theta->getValue()));
    mData->mCamera.SetPhi(static_cast<float>(led_proj_phi->getValue()));
    mOpenGL->updateGL();
}

void OGLProps::setViewParams() {
    mData->m_showBox = chb_show_box->isChecked();
    mData->m_useDotProd = chb_useDotProd->isChecked();
    mData->m_shading = static_cast<e_shading>(cob_shading->currentIndex());
    mOpenGL->updateGL();
}

void OGLProps::SetProjReset() {
    setProjBlockStatus(true);
    led_proj_fov->setValue(init_proj_fov);
    led_proj_distance->setValue(init_proj_dist);
    led_proj_theta->setValue(init_proj_theta);
    led_proj_phi->setValue(init_proj_phi);
    led_proj_poiX->setValue(init_proj_poiX);
    led_proj_poiY->setValue(init_proj_poiY);
    led_proj_poiZ->setValue(init_proj_poiZ);
    setProjBlockStatus(false);
    setCameraParams();
}

void OGLProps::setProjBlockStatus( bool status ) {
    led_proj_fov->blockSignals(status);
    led_proj_distance->blockSignals(status);
    led_proj_theta->blockSignals(status);
    led_proj_phi->blockSignals(status);
    led_proj_poiX->blockSignals(status);
    led_proj_poiY->blockSignals(status);
    led_proj_poiZ->blockSignals(status);
}


void OGLProps::setMeshParams() {
    mData->m_useColor    = chb_useColor->isChecked();
    mData->m_useMeshOnly = chb_useMeshOnly->isChecked();
    mData->m_showIsolines = chb_showIsolines->isChecked();
    mData->m_tessLevelInner = spb_tessLevelInner->value();
    mData->m_tessLevelOuter = spb_tessLevelOuter->value();
    mData->m_wireframe      = chb_wireframe->isChecked();
    mOpenGL->updateGL();
}

void OGLProps::setMeshOpacity( int opac ) {
    mData->m_meshOpacity = opac*0.01f;
    mOpenGL->updateGL();
}

// *********************************** protected methods *********************************

void OGLProps::init() {
    initElements();
    initGUI();
    initActions();
    initConnect();
}

void OGLProps::initElements() {
    pub_close = new QPushButton("close");

    int prec = 2;
    double stepSize = 0.01;
    lab_left   = new QLabel("Left");
    led_left   = new DoubleEdit(prec,init_ortho_left,stepSize);
    lab_right  = new QLabel("Right");
    led_right  = new DoubleEdit(prec,init_ortho_right,stepSize);
    lab_bottom = new QLabel("Bottom");
    led_bottom = new DoubleEdit(prec,init_ortho_bottom,stepSize);
    lab_top    = new QLabel("Top");
    led_top    = new DoubleEdit(prec,init_ortho_top,stepSize);

    lab_stepX = new QLabel("StepX");
    led_stepX = new DoubleEdit(prec,init_grid_stepX,stepSize);
    lab_stepY = new QLabel("StepY");
    led_stepY = new DoubleEdit(prec,init_grid_stepY,stepSize);

    chb_grid = new QCheckBox("Grid");
    chb_grid->setChecked(mData->m_showGrid);

    chb_keepAspect = new QCheckBox("Aspect");
    chb_keepAspect->setChecked(mData->m_keepAspectRatio);
    pub_reset = new QPushButton("Reset");

    lab_meshOpacity = new QLabel("#opacity");
    sli_meshOpacity = new QSlider(Qt::Horizontal);
    sli_meshOpacity->setRange(0,100);
    sli_meshOpacity->setValue(static_cast<int>(mData->m_meshOpacity*100));
    chb_useColor = new QCheckBox("color");
    chb_useColor->setChecked(mData->m_useColor);
    chb_useMeshOnly = new QCheckBox("mesh");
    chb_useMeshOnly->setChecked(mData->m_useMeshOnly);
    chb_showIsolines = new QCheckBox("isolines");
    chb_showIsolines->setChecked(mData->m_showIsolines);

    lab_proj_fov = new QLabel("FoV");
    led_proj_fov = new DoubleEdit(1,init_proj_fov,0.1);
    led_proj_fov->setRange(0.1,180.0);

    lab_proj_distance = new QLabel("Dist");
    led_proj_distance = new DoubleEdit(prec,init_proj_dist,stepSize);
    led_proj_distance->setRange(0.01,std::numeric_limits<double>::max());

    lab_proj_theta = new QLabel("Theta");
    led_proj_theta = new DoubleEdit(prec,init_proj_theta,stepSize);
    led_proj_theta->setRange(-89.999,89.999);
    lab_proj_phi = new QLabel("Phi");
    led_proj_phi = new DoubleEdit(prec,init_proj_phi,stepSize);

    lab_proj_poiX = new QLabel("PoIx");
    led_proj_poiX = new DoubleEdit(prec,init_proj_poiX,stepSize);
    lab_proj_poiY = new QLabel("PoIy");
    led_proj_poiY = new DoubleEdit(prec,init_proj_poiY,stepSize);
    lab_proj_poiZ = new QLabel("PoIz");
    led_proj_poiZ = new DoubleEdit(prec,init_proj_poiZ,stepSize);

    pub_proj_reset = new QPushButton("Reset");
    chb_show_box = new QCheckBox("Box");
    chb_show_box->setChecked(mData->m_showBox);


    // 3d mesh stuff
    lab_tessLevelInOut = new QLabel("TessLevelIn/Out");
    spb_tessLevelInner = new QSpinBox();
    spb_tessLevelInner->setValue(mData->m_tessLevelInner);
    spb_tessLevelOuter = new QSpinBox();
    spb_tessLevelOuter->setValue(mData->m_tessLevelOuter);

    lab_shading = new QLabel("Shading");
    cob_shading = new QComboBox();
    cob_shading->addItems(stl_shading);
    cob_shading->setCurrentIndex(static_cast<int>(mData->m_shading));

    chb_wireframe = new QCheckBox("Wireframe");
    chb_wireframe->setChecked(mData->m_wireframe);
    chb_useDotProd = new QCheckBox("DotProd");
    chb_useDotProd->setChecked(mData->m_useDotProd);
}



void OGLProps::initGUI() {
    QGridLayout* layout_complete = new QGridLayout();

    // ---------------------
    //   view 2d
    // ---------------------
    QGroupBox* grb_view_2d = new QGroupBox("View 2D");
    QGridLayout* layout_view_2d = new QGridLayout();
    layout_view_2d->addWidget( lab_left,  0, 0 );
    layout_view_2d->addWidget( led_left,  0, 1 );
    layout_view_2d->addWidget( lab_right, 1, 0 );
    layout_view_2d->addWidget( led_right, 1, 1 );
    layout_view_2d->addWidget( lab_bottom, 0, 2 );
    layout_view_2d->addWidget( led_bottom, 0, 3 );
    layout_view_2d->addWidget( lab_top,    1, 2 );
    layout_view_2d->addWidget( led_top,    1, 3 );
    layout_view_2d->addWidget( lab_stepX, 2, 0 );
    layout_view_2d->addWidget( led_stepX, 2, 1 );
    layout_view_2d->addWidget( lab_stepY, 2, 2 );
    layout_view_2d->addWidget( led_stepY, 2, 3 );
    layout_view_2d->addWidget( pub_reset, 4,0,1,2);
    layout_view_2d->addWidget( chb_grid, 4, 2);
    layout_view_2d->addWidget( chb_keepAspect, 4, 3);
    grb_view_2d->setLayout(layout_view_2d);

    // ---------------------
    //   mesh 2d
    // ---------------------
    QGroupBox* grb_mesh_2d = new QGroupBox("Mesh 2D");
    QGridLayout* layout_mesh_2d = new QGridLayout();
    layout_mesh_2d->addWidget( lab_meshOpacity, 0, 0 );
    layout_mesh_2d->addWidget( sli_meshOpacity, 0, 1, 1, 2 );
    layout_mesh_2d->addWidget( chb_useColor,    1, 0 );
    layout_mesh_2d->addWidget( chb_useMeshOnly, 1, 1 );
    layout_mesh_2d->addWidget( chb_showIsolines, 1, 2 );
    grb_mesh_2d->setLayout(layout_mesh_2d);


    // ---------------------
    //   view 3d
    // ---------------------
    QGroupBox* grb_view_3d = new QGroupBox("View 3D");
    QGridLayout* layout_view_3d = new QGridLayout();
    layout_view_3d->addWidget( lab_proj_fov, 0, 0 );
    layout_view_3d->addWidget( led_proj_fov, 0, 1 );
    layout_view_3d->addWidget( lab_proj_distance, 1, 0 );
    layout_view_3d->addWidget( led_proj_distance, 1, 1 );
    layout_view_3d->addWidget( lab_proj_theta, 2, 0 );
    layout_view_3d->addWidget( led_proj_theta, 2, 1 );
    layout_view_3d->addWidget( lab_proj_phi, 3, 0 );
    layout_view_3d->addWidget( led_proj_phi, 3, 1 );
    layout_view_3d->addWidget( lab_proj_poiX, 1, 2 );
    layout_view_3d->addWidget( led_proj_poiX, 1, 3 );
    layout_view_3d->addWidget( lab_proj_poiY, 2, 2 );
    layout_view_3d->addWidget( led_proj_poiY, 2, 3 );
    layout_view_3d->addWidget( lab_proj_poiZ, 3, 2 );
    layout_view_3d->addWidget( led_proj_poiZ, 3, 3 );
    layout_view_3d->addWidget( pub_proj_reset, 4, 0, 1, 2);
    layout_view_3d->addWidget( chb_show_box, 4, 3);
    grb_view_3d->setLayout(layout_view_3d);


    // ---------------------
    //   mesh 3d
    // ---------------------
    QGroupBox* grb_mesh_3d = new QGroupBox("Mesh 3D");
    QGridLayout* layout_mesh_3d = new QGridLayout();
    layout_mesh_3d->addWidget( lab_tessLevelInOut, 0, 0 );
    layout_mesh_3d->addWidget( spb_tessLevelInner, 0, 1 );
    layout_mesh_3d->addWidget( spb_tessLevelOuter, 0, 2 );
    layout_mesh_3d->addWidget( lab_shading,        1, 0 );
    layout_mesh_3d->addWidget( cob_shading,        1, 1, 1, 2 );
    layout_mesh_3d->addWidget( chb_wireframe,      2, 0 );
    layout_mesh_3d->addWidget( chb_useDotProd,     2, 1 );
    grb_mesh_3d->setLayout(layout_mesh_3d);


    //layout_complete->addWidget( pub_close, 2, 0, 1, 2 );
    layout_complete->addWidget( grb_view_2d, 0, 0 );
    layout_complete->addWidget( grb_mesh_2d, 1, 0 );
    layout_complete->addWidget( grb_view_3d, 2, 0 );
    layout_complete->addWidget( grb_mesh_3d, 3, 0 );
    layout_complete->setRowStretch(4,2);
    this->setLayout(layout_complete);

    this->setWindowTitle("OpenGL properties");
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

void OGLProps :: initActions() {
    mActionClose = new QAction(this);
    mActionClose->setShortcut(Qt::CTRL|Qt::Key_O);
    addAction(mActionClose);
    connect(mActionClose,SIGNAL(triggered()),this,SLOT(close()));
}

void OGLProps::initConnect() {
    connect( pub_close, SIGNAL(pressed()), this, SLOT(close()) );

    connect( led_left,   SIGNAL(editingFinished()), this, SLOT(setOrthoBorder()) );
    connect( led_right,  SIGNAL(editingFinished()), this, SLOT(setOrthoBorder()) );
    connect( led_bottom, SIGNAL(editingFinished()), this, SLOT(setOrthoBorder()) );
    connect( led_top,    SIGNAL(editingFinished()), this, SLOT(setOrthoBorder()) );
    connect( led_stepX, SIGNAL(editingFinished()), this, SLOT(setGridStyle()) );
    connect( led_stepY, SIGNAL(editingFinished()), this, SLOT(setGridStyle()) );
    connect( pub_reset, SIGNAL(pressed()), this, SLOT(SetOrthoReset()) );
    connect( chb_grid,  SIGNAL(stateChanged(int)), this, SLOT(setGridStyle()) );
    connect( chb_keepAspect,  SIGNAL(stateChanged(int)), this, SLOT(setGridStyle()) );

    connect( chb_useColor, SIGNAL(stateChanged(int)), this, SLOT(setMeshParams()) );
    connect( chb_useMeshOnly, SIGNAL(stateChanged(int)), this, SLOT(setMeshParams()) );
    connect( chb_showIsolines, SIGNAL(stateChanged(int)), this, SLOT(setMeshParams()) );
    connect( sli_meshOpacity, SIGNAL(valueChanged(int)), this, SLOT(setMeshOpacity(int)) );

    connect( led_proj_fov,   SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( led_proj_distance, SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( led_proj_theta, SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( led_proj_phi,   SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( led_proj_poiX,  SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( led_proj_poiY,  SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( led_proj_poiZ,  SIGNAL(editingFinished()), this, SLOT(setCameraParams()) );
    connect( pub_proj_reset, SIGNAL(pressed()), this, SLOT(SetProjReset()) );
    connect( chb_show_box,   SIGNAL(stateChanged(int)), this, SLOT(setViewParams()) );

    connect( spb_tessLevelInner, SIGNAL(valueChanged(int)), this, SLOT(setMeshParams()) );
    connect( spb_tessLevelOuter, SIGNAL(valueChanged(int)), this, SLOT(setMeshParams()) );
    connect( chb_wireframe, SIGNAL(stateChanged(int)), this, SLOT(setMeshParams()) );

    connect( chb_useDotProd, SIGNAL(stateChanged(int)), this, SLOT(setViewParams()) );
    connect( cob_shading,    SIGNAL(currentIndexChanged(int)), this, SLOT(setViewParams()) );
}

QSize OGLProps::sizeHint() const {
    return QSize(300,150);
}
