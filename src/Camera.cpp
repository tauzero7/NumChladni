/**
    @file   Camera.cpp

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

#include "Camera.h"


Camera::Camera() {
    SetStandardParams();
    SetProjMX_persp();
    SetViewMX_lookAt();
}

Camera :: ~Camera ( )
{
}

void Camera::SetStandardParams() {
    m_pos = glm::vec3( 0.0f, 0.0f,  0.0f );
    m_dir = glm::vec3( 0.0f, 0.0f, -1.0f );
    m_vup = glm::vec3( 0.0f, 1.0f,  0.0f );

    m_right = glm::cross(m_dir,m_vup);
    m_poi   = glm::vec3( 0.0f, 0.0f, 0.0f );

    m_zNear = 0.1f;
    m_zFar  = 100.0f;

    m_width  = 100;
    m_height = 100;

    m_aspect = 1.0f;
    m_fovY   = 45.0f;

    m_dist     = glm::length(m_pos-m_poi);
    m_min_dist = 0.0f;

    m_panStep     = 0.1f;
    m_distFactor  = 0.05f;
    m_panXYfactor = 0.01f;
    m_rotFactor   = 0.01f;

    m_xlast = -1;
    m_ylast = -1;
}

/*! Set camera position, view direction and vertical up vector.
 * \param pos : position.
 * \param dir : view direction.
 * \param vup : vertical up vector.
 */
void Camera::Set( glm::vec3 pos, glm::vec3 dir, glm::vec3 vup ) {
    m_pos   = pos;
    m_dir   = glm::normalize(dir);
    m_vup   = glm::normalize(vup);
    m_right = glm::cross(m_dir,m_vup);
    m_dist  = glm::length(m_pos-m_poi);
    SetViewMX_lookAt();
}

/*! Set camera position, point of interest and vertical up vector.
 * \param pos : position.
 * \param poi : point of interest.
 * \param vup : vertical up vector.
 */
void Camera::SetPosPOI( glm::vec3 pos, glm::vec3 poi, glm::vec3 vup ) {
    m_pos   = pos;
    m_poi   = poi;
    m_dir   = glm::normalize(poi-pos);
    m_right = glm::normalize(glm::cross(m_dir,vup));
    m_vup   = glm::cross(m_right,m_dir);
    m_dist  = glm::length(m_pos-m_poi);
    SetViewMX_lookAt();
}


/*! Set position of the camera.
 * \param eye_x : x-position.
 * \param eye_y : y-position.
 * \param eye_z : z-position.
 */
void Camera::SetEyePos( float eye_x, float eye_y, float eye_z ) {
    m_pos = glm::vec3( eye_x, eye_y, eye_z );
    SetViewMX_lookAt();
}

/*! Set position of the camera.
 * \param pos : xyz-position.
 */
void Camera::SetEyePos( glm::vec3 pos ) {
    m_pos = pos;
    SetViewMX_lookAt();
}

/*! Get camera position.
 * \param eye_x : reference to x-position.
 * \param eye_y : reference to y-position.
 * \param eye_z : reference to z-position.
 */
void Camera::GetEyePos( float &eye_x, float &eye_y, float &eye_z ) {
    eye_x = m_pos.x;
    eye_y = m_pos.y;
    eye_z = m_pos.z;
}

/*! Get camera position.
 * \return glm::vec3 : position.
 */
glm::vec3 Camera::GetEyePos() {
    return m_pos;
}

/*! Set camera's viewing direction.
 * \param m_dirx : x-direction.
 * \param m_diry : y-direction.
 * \param m_dirz : z-direction.
 */
void Camera::SetDir( float m_dirx, float m_diry, float m_dirz ) {
    m_dir   = glm::vec3( m_dirx, m_diry, m_dirz );
    m_right = glm::cross(m_dir,m_vup);
    SetViewMX_lookAt();
}

/*! Set camera's viewing direction.
 * \param dir : xyz-direction.
 */
void Camera::SetDir( glm::vec3 dir ) {
    m_dir   = dir;
    m_right = glm::cross(m_dir,m_vup);
    SetViewMX_lookAt();
}

/*! Get direction of camera view.
 *  \param m_dirx  :  x-component of direction.
 *  \param m_diry  :  y-component of direction.
 *  \param m_dirz  :  z-component of direction.
 */
void Camera::GetDir( float &m_dirx, float &m_diry, float &m_dirz ) {
    m_dirx = m_dir.x;
    m_diry = m_dir.y;
    m_dirz = m_dir.z;
}

/*! Get direction of camera view.
 *  \return glm::vec3 : camera's viewing direction.
 */
glm::vec3 Camera::GetDir() {
    return m_dir;
}

/*! Calculate direction for a given pixel.
 *  \param i : pixel in horizontal direction.
 *  \param j : pixel in vertical direction.
 *  \return glm::vec3 : normalized direction of pixel (i,j)
 */
glm::vec3 Camera::PixelToDir( int i, int j ) {
    glm::vec3 dir = float(i-0.5*m_width)*m_right + float(0.5*m_height/tan(0.5*m_fovY))*m_dir + float(j-m_height)*m_vup;
    return glm::normalize(dir);
}

/*! Set point of interest.
 * \param c_x : x-component of the point-of-interest.
 * \param c_y : y-component of the point-of-interest.
 * \param c_z : z-component of the point-of-interest.
 */
void Camera::SetPOI ( float c_x, float c_y, float c_z ) {
    glm::vec3 center = glm::vec3(c_x,c_y,c_z);
    m_dir = center - m_pos;

    if (glm::length(m_dir)!=0)
    {
        m_poi   = center;
        m_dir   = glm::normalize(m_dir);
        m_right = glm::cross(m_dir,m_vup);
    }
    SetViewMX_lookAt();
}

/*! Set point of interest.
 * \param center : xyz-component of the point-of-interest.
 */
void Camera::SetPOI( glm::vec3 center ) {
    m_poi = center;
    m_dir = glm::normalize(m_poi-m_pos);
    SetViewMX_lookAt();
}

/*! Get point of interest.
 * \param c_x : x-component of the point-of-interest.
 * \param c_y : y-component of the point-of-interest.
 * \param c_z : z-component of the point-of-interest.
 */
void Camera::GetPOI( float &c_x, float &c_y, float &c_z ) {
    c_x = m_poi.x;
    c_y = m_poi.y;
    c_z = m_poi.z;
}

/*! Get point of interest.
 * \return glm::vec3 : xyz-components of point of interest.
 */
glm::vec3 Camera::GetPOI() {
    return m_poi;
}

/*! Set up-vector of camera.
 * \param m_vupx : x-component of vertical up-vector.
 * \param m_vupy : y-component of vertical up-vector.
 * \param m_vupz : z-component of vertical up-vector.
 */
void Camera::SetVup( float m_vupx, float m_vupy, float m_vupz ) {
    m_vup   = glm::vec3( m_vupx, m_vupy, m_vupz );
    m_right = glm::cross(m_dir,m_vup);
    SetViewMX_lookAt();
}

/*! Set up-vector of camera.
 * \param vup : xyz-components of vertical up-vector.
 */
void Camera::SetVup( glm::vec3 vup ) {
    m_vup   = vup;
    SetViewMX_lookAt();
}

/*! Get up-vector of camera.
 * \param m_vupx : reference to x-component of vertical up-vector.
 * \param m_vupy : reference to y-component of vertical up-vector.
 * \param m_vupz : reference to z-component of vertical up-vector.
 */
void Camera::GetVup ( float &m_vupx, float &m_vupy, float &m_vupz ) {
    m_vupx = m_vup.x;
    m_vupy = m_vup.y;
    m_vupz = m_vup.z;
}

/*! Get up-vector of camera.
 * \return glm::vec3 : up-vector.
 */
glm::vec3 Camera::GetVup() {
    return m_vup;
}

/*! Get right-vector of camera.
 */
glm::vec3 Camera::GetRight() {
    return m_right;
}

/*! Set field-of-view in y-direction.
 * \param fovy : angle in degree.
 */
void Camera::SetFovY( float fovy ) {
    m_fovY = fovy;
    SetProjMX_persp();
}

/*! Get field-of-view in y-direction.
 */
float Camera::GetFovY() {
    return m_fovY;
}

/*! Set aspect ratio.
 * \param aspect : aspect ratio.
 */
void Camera::SetAspect( float aspect )  {
    m_aspect = aspect;
    SetProjMX_persp();
}

/*! Get aspect ratio.
 */
float Camera::GetAspect() {
    return m_aspect;
}

/*! Set intrinsic parameters.
 * \param fovy : field of view in y-direction in degree.
 * \param aspect : aspect ratio.
 */
void Camera::SetIntrinsic( float fovy, float aspect ) {
    SetIntrinsic(fovy,aspect,0.1f,10000.0f);
    SetProjMX_persp();
}

/*! Set intrinsic parameters.
 * \param fovy : field of view in y-direction in degree.
 * \param aspect : aspect ratio.
 * \param near : near clipping plane.
 * \param far : far clipping plane.
 */
void Camera::SetIntrinsic( float fovy, float aspect, float znear, float zfar ) {
    m_fovY   = fovy;
    m_aspect = aspect;
    m_zNear  = znear;
    m_zFar   = zfar;
    SetProjMX_persp();
}

/*! Get intrinsic parameters.
 * \param fovy : reference to field of view in y-direction in degree.
 * \param aspect : reference to aspect ratio.
 * \param near : reference to near clipping plane.
 * \param far : reference to far clipping plane.
 */
void Camera::GetIntrinsic( float &fovy, float &aspect, float &znear, float &zfar ) {
    fovy    = m_fovY;
    aspect  = m_aspect;
    znear   = m_zNear;
    zfar    = m_zFar;
}

/*! Set near and far field.
 * \param near : near value.
 * \param far : far value.
 */
void Camera::SetNearFar( float znear, float zfar ) {
    m_zNear = znear;
    m_zFar  = zfar;
}

/*! Get near and far field.
 * \param near : reference to near value.
 * \param far : reference to far value.
 */
void Camera::GetNearFar( float &znear, float &zfar ) {
    znear = m_zNear;
    zfar  = m_zFar;
}

/*! Set camera resolution and adjust aspect ratio.
 * \param width : width of camera resolution.
 * \param height : height of camera resolution.
 */
void Camera::SetSizeAndAspect( int width, int  height ) {
    SetSize(width,height);
    SetAspect(width/(float)height);
    SetProjMX_persp();
}

/*! Set camera resolution.
 * \param width : width of camera resolution.
 * \param height : height of camera resolution.
 */
void Camera::SetSize ( int width, int height ) {
    m_width  = width;
    m_height = height;
    SetProjMX_persp();
}

/*! Get camera resolution.
 * \param width : reference to width of camera resolution.
 * \param height : reference to height of camera resolution.
 */
void Camera::GetSize( int &width, int &height ) {
    width  = m_width;
    height = m_height;
}

/*! Get camera width.
 */
int Camera::Width() {
    return m_width;
}

/*! Get camera height.
 */
int Camera::Height() {
    return m_height;
}

/*! Rotate camera around vup with fixed point of interest.
 * \param angle : angle in rad.
 */
void Camera::FixRotAroundVup( float angle ) {
    glm::fquat q_dir(0,m_dir);
    glm::fquat q_right(0,m_right);
    glm::fquat q_eye(0,m_pos);

    q_dir   = glm::rotate(q_dir,angle,m_vup);
    q_right = glm::rotate(q_right,angle,m_vup);
    q_eye   = glm::rotate(q_eye,angle,m_vup);

    m_pos   = glm::vec3(q_eye.x,q_eye.y,q_eye.z);
    //std::cerr << glm::length(m_pos) << std::endl;
    m_dir   = glm::vec3(q_dir.x,q_dir.y,q_dir.z);
    m_right = glm::vec3(q_right.x,q_right.y,q_right.z);
    SetViewMX_lookAt();
}

/*! Rotate camera around right with fixed point of interest.
 * \param angle : angle in rad.
 */
void Camera::FixRotAroundRight( float angle ) {
    glm::fquat q_dir(0,m_dir);
    glm::fquat q_right(0,m_right);
    glm::fquat q_eye(0,m_pos);

    q_dir   = glm::rotate(q_dir,angle,m_right);
    q_right = glm::rotate(q_right,angle,m_right);
    q_eye   = glm::rotate(q_eye,angle,m_right);

    m_pos   = glm::vec3(q_eye.x,q_eye.y,q_eye.z);
    //std::cerr << glm::length(m_pos) << std::endl;
    m_dir   = glm::vec3(q_dir.x,q_dir.y,q_dir.z);
    m_right = glm::vec3(q_right.x,q_right.y,q_right.z);
    SetViewMX_lookAt();
}

/*! Rotate camera around dir with fixed point of interest.
 * \param angle : angle in rad.
 */
void Camera::FixRotAroundDir( float angle ) {
    glm::fquat q_dir(0,m_dir);
    glm::fquat q_right(0,m_right);
    glm::fquat q_eye(0,m_pos);

    q_dir   = glm::rotate(q_dir,angle,m_dir);
    q_right = glm::rotate(q_right,angle,m_dir);
    q_eye   = glm::rotate(q_eye,angle,m_dir);

    m_pos   = glm::vec3(q_eye.x,q_eye.y,q_eye.z);
    m_dir   = glm::vec3(q_dir.x,q_dir.y,q_dir.z);
    m_right = glm::vec3(q_right.x,q_right.y,q_right.z);
    SetViewMX_lookAt();
}

/*! Rotate camera around vup.
 * \param angle : angle in rad.
 */
void Camera::RotAroundVup( float angle ) {
    glm::fquat q_dir(0,m_dir);
    glm::fquat q_right(0,m_right);

    q_dir   = glm::rotate(q_dir,angle,m_vup);
    q_right = glm::rotate(q_right,angle,m_vup);

    m_dir   = glm::vec3(q_dir.x,q_dir.y,q_dir.z);
    m_right = glm::vec3(q_right.x,q_right.y,q_right.z);
    SetViewMX_lookAt();
}

/*! Rotate camera around right.
 * \param angle : angle in rad.
 */
void Camera::RotAroundRight( float angle ) {
    glm::fquat q_dir(0,m_dir);
    glm::fquat q_vup(0,m_vup);

    q_dir = glm::rotate(q_dir,angle,m_right);
    q_vup = glm::rotate(q_vup,angle,m_right);

    m_dir = glm::vec3(q_dir.x,q_dir.y,q_dir.z);
    m_vup = glm::vec3(q_vup.x,q_vup.y,q_vup.z);
    SetViewMX_lookAt();
}

/*! Rotate camera around dir.
 * \param angle : angle in rad.
 */
void Camera::RotAroundDir( float angle ) {
    glm::fquat q_vup(0,m_vup);
    glm::fquat q_right(0,m_right);

    q_vup   = glm::rotate(q_vup,angle,m_dir);
    q_right = glm::rotate(q_right,angle,m_dir);

    m_vup   = glm::vec3(q_vup.x,q_vup.y,q_vup.z);
    m_right = glm::vec3(q_right.x,q_right.y,q_right.z);
    SetViewMX_lookAt();
}

/*! Move on sphere around point of interest.
 * \param delta_theta
 * \param delta_phi
 */
void Camera::MoveOnSphere( float delta_theta, float delta_phi )
{
    glm::vec3 relPos = m_pos-m_poi;
    float theta = atan2f(relPos.z,sqrtf(relPos.x*relPos.x+relPos.y*relPos.y));
    float phi   = atan2f(relPos.y,relPos.x);
    m_dist = glm::length(relPos);

    theta += delta_theta;
    phi   -= delta_phi;

    float eps = 1e-6f;
    if (theta>M_PI_2-eps)
        theta = M_PI_2-eps;
    else if (theta<-M_PI_2+eps)
        theta = -M_PI_2+eps;

    glm::vec3 zDir(0.0,0.0,1.0);
    m_pos   = m_poi + m_dist*glm::vec3( cosf(theta)*cosf(phi), cosf(theta)*sinf(phi), sinf(theta) );
    m_dir   = glm::normalize(m_poi-m_pos);
    m_right = glm::normalize(glm::cross(m_dir,zDir));
    m_vup   = glm::cross(m_right,m_dir);
    SetViewMX_lookAt();
}

void Camera::SetDistance( float distance ) {
    if (distance<m_min_dist)
        m_dist = m_min_dist;
    else
        m_dist = distance;
    m_pos = m_poi - m_dist*m_dir;
    SetViewMX_lookAt();
}

float Camera::GetDistance() {
    glm::vec3 relPos = m_pos-m_poi;
    m_dist = glm::length(relPos);
    return m_dist;
}

void Camera::SetTheta( float theta ) {
    glm::vec3 relPos = m_pos-m_poi;
    float phi = atan2f(relPos.y,relPos.x);
    float th  = glm::radians(theta);

    float eps = 1e-6f;
    if (th>M_PI_2-eps)
        th = M_PI_2-eps;
    else if (th<-M_PI_2+eps)
        th = -M_PI_2+eps;

    glm::vec3 zDir(0.0,0.0,1.0);
    m_pos   = m_poi + m_dist*glm::vec3( cosf(th)*cosf(phi), cosf(th)*sinf(phi), sinf(th) );
    m_dir   = glm::normalize(m_poi-m_pos);
    m_right = glm::normalize(glm::cross(m_dir,zDir));
    m_vup   = glm::cross(m_right,m_dir);
    SetViewMX_lookAt();
}

float Camera::GetTheta() {
    glm::vec3 relPos = m_pos-m_poi;
    return glm::degrees(atan2f(relPos.z,sqrtf(relPos.x*relPos.x+relPos.y*relPos.y)));
}

void Camera::SetPhi( float phi ) {
    glm::vec3 relPos = m_pos-m_poi;
    float theta = atan2f(relPos.z,sqrtf(relPos.x*relPos.x+relPos.y*relPos.y));

    glm::vec3 zDir(0.0,0.0,1.0);
    m_pos   = m_poi + m_dist*glm::vec3( cosf(theta)*cosf(glm::radians(phi)), cosf(theta)*sinf(glm::radians(phi)), sinf(theta) );
    m_dir   = glm::normalize(m_poi-m_pos);
    m_right = glm::normalize(glm::cross(m_dir,zDir));
    m_vup   = glm::cross(m_right,m_dir);
    SetViewMX_lookAt();
}

float Camera::GetPhi() {
    glm::vec3 relPos = m_pos-m_poi;
    return glm::degrees(atan2f(relPos.y,relPos.x));
}


void Camera::ChangeDistance( float deltaDist ) {
    m_dist += deltaDist;
    if (m_dist<m_min_dist)
        m_dist = m_min_dist;
    m_pos = m_poi-m_dist*m_dir;
    SetViewMX_lookAt();
}

void Camera::Panning ( float x, float y ) {
    m_pos = m_pos + x*m_right + y*m_vup;
    m_poi = m_poi + x*m_right + y*m_vup;
    m_dir = glm::normalize(m_poi-m_pos);
    m_dist = glm::length(m_pos-m_poi);
    SetViewMX_lookAt();
}

void Camera::MoveOnXY( float dx, float dy ) {
    glm::vec3 dirRight = glm::normalize(glm::vec3(m_right.x,m_right.y,0.0));
    glm::vec3 dirForw  = glm::normalize(glm::vec3(m_dir.x,m_dir.y,0.0));
    m_poi = m_poi + dy*dirRight + dx*dirForw;
    m_pos = m_pos + dy*dirRight + dx*dirForw;
    SetViewMX_lookAt();
}

/*! Move vertically.
 */
void Camera::MoveOnZ( float dz ) {
    glm::vec3 dirUp = glm::normalize(glm::vec3(0.0,0.0,m_vup.z));
    m_poi = m_poi + dz*dirUp;
    m_pos = m_pos + dz*dirUp;
    SetViewMX_lookAt();
}

void Camera::SetMinDistance( float min_dist ) {
    m_min_dist = min_dist;
}

void Camera::SetClearColors( glm::vec4 color ) {
    m_clearColor = color;
    glClearColor(m_clearColor.x,m_clearColor.y,m_clearColor.z,m_clearColor.w);
}

glm::vec4 Camera::GetClearColors() {
    return m_clearColor;
}

/*! Call gluLookAt method with internal parameters.
 * \param upsideDown : true=mirror vertically.
 */
void Camera::SetViewMX_lookAt( bool upsideDown ) {
    glm::vec3 c = m_pos+m_dir;
    if (!upsideDown) {
        m_viewMX = glm::lookAt(m_pos,c,m_vup);
    } else {
        m_viewMX = glm::lookAt(m_pos,c,-m_vup);
    }
}

/*! Call gluPerspective method with internal parameters.
 */
void Camera::SetProjMX_persp() {
    m_projMX = glm::perspectiveFov(m_fovY,(float)m_width,(float)m_height,m_zNear,m_zFar);

    float aspect = m_width/(float)m_height;
    float range  = (float)(tan(glm::radians(m_fovY)/2.0) * m_zNear);
    float left   = -range * aspect;
    float right  = range * aspect;
    float bottom = -range;
    float top = range;
    m_projMX = glm::frustum(left,right,bottom,top,m_zNear,m_zFar);
}

/*! Call glViewport method with internal parameters;
 */
void  Camera :: viewport() {
    glViewport( 0, 0, GLsizei(m_width), GLsizei(m_height) );
}

float* Camera::ProjMatrixPtr() {
    return glm::value_ptr(m_projMX);
}

float* Camera::ViewMatrixPtr() {
    return glm::value_ptr(m_viewMX);
}

glm::mat4 Camera::ProjMatrix() {
    return m_projMX;
}

glm::mat4 Camera::ViewMatrix() {
    return m_viewMX;
}

void Camera::SetPanStep( float panStep ) {
    m_panStep = panStep;
}

float Camera::GetPanStep() {
    return m_panStep;
}

void  Camera::SetDistFactor( float distFactor ) {
    m_distFactor = distFactor;
}

float Camera::GetDistFactor() {
    return m_distFactor;
}

void  Camera::SetPanXYFactor( float panXYfactor ) {
    m_panXYfactor = panXYfactor;
}

float Camera::GetPanXYFactor() {
    return m_panXYfactor;
}

void  Camera::SetRotFactor( float rotFactor ) {
    m_rotFactor = rotFactor;
}

float Camera::GetRotFactor() {
    return m_rotFactor;
}

void  Camera::SetMousePos( int x, int y ) {
    m_xlast = x;
    m_ylast = y;
}

void  Camera::GetMousePos( int &x, int &y ) {
    x = m_xlast;
    y = m_ylast;
}

void  Camera::GetMouseRelPos( int x, int y, int &dx, int &dy ) {
    dx = x - m_xlast;
    dy = y - m_ylast;
    m_xlast = x;
    m_ylast = y;
}

/*! Print camera parameters.
 * \param ptr : file pointer.
 */
void Camera::Print( FILE* ptr ) {
    fprintf(ptr, "\nCamera parameters:\n");
    fprintf(ptr, "------------------\n");
    fprintf(ptr, "eye    :  %6.3f %6.3f %6.3f\n", m_pos.x, m_pos.y, m_pos.z );
    fprintf(ptr, "dir    :  %6.3f %6.3f %6.3f\n", m_dir.x, m_dir.y, m_dir.z );
    fprintf(ptr, "vup    :  %6.3f %6.3f %6.3f\n", m_vup.x, m_vup.y, m_vup.z );
    fprintf(ptr, "right  :  %6.3f %6.3f %6.3f\n", m_right.x, m_right.y, m_right.z );
    fprintf(ptr, "size   :  %4i %4i\n",m_width,m_height);
    fprintf(ptr, "aspect :  %6.3f\n",m_aspect);
    fprintf(ptr, "fovY   :  %6.3f\n",m_fovY);
    fprintf(ptr, "near   :  %8.2f\n",m_zNear);
    fprintf(ptr, "far    :  %8.2f\n",m_zFar);
}
