/**
    @file   Camera.h

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

#ifndef QTCHALDNI_CAMERA_H
#define QTCHALDNI_CAMERA_H

#include <iostream>
#include <cstdio>

#include <GL3/gl3w.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "qtdefs.h"

/**
 * @brief Camera class for 3d view
 */
class Camera
{
public:
    Camera();
    virtual ~Camera();

    // --------- public methods -----------
public:
    void   SetStandardParams();

    void        Set        ( glm::vec3 pos, glm::vec3 dir, glm::vec3 vup );
    void        SetPosPOI  ( glm::vec3 pos, glm::vec3 poi, glm::vec3 vup );

    void        SetEyePos  ( float eye_x, float eye_y, float eye_z );
    void        SetEyePos  ( glm::vec3 pos );
    void        GetEyePos  ( float &eye_x, float &eye_y, float &eye_z );
    glm::vec3   GetEyePos  ( );

    void        SetDir     ( float dir_x, float dir_y, float dir_z );
    void        SetDir     ( glm::vec3 dir );
    void        GetDir     ( float &dir_x, float &dir_y, float &dir_z );
    glm::vec3   GetDir     ( );

    glm::vec3   PixelToDir ( int i, int j );

    void        SetPOI     ( float c_x, float c_y, float c_z );
    void        SetPOI     ( glm::vec3 center );
    void        GetPOI     ( float &c_x, float &c_y, float &c_z );
    glm::vec3   GetPOI     ( );

    void        SetVup     ( float vup_x, float vup_y, float vup_z );
    void        SetVup     ( glm::vec3 vup );
    void        GetVup     ( float &vup_x, float &vup_y, float &vup_z );
    glm::vec3   GetVup     ( );

    glm::vec3   GetRight   ( );

    void        SetFovY      ( float fovy );
    float       GetFovY      ( );

    void        SetAspect    ( float aspect );
    float       GetAspect    ( );
    
    void        SetIntrinsic ( float fovy, float aspect );
    void        SetIntrinsic ( float fovy, float aspect, float near, float far );
    void        GetIntrinsic ( float &fovy, float &aspect, float &near, float &far );
    void        SetNearFar   ( float near, float far );
    void        GetNearFar   ( float &near, float &far );

    void        SetSizeAndAspect  ( int width, int  height );

    void        SetSize           ( int width, int height );
    void        GetSize           ( int &width, int &height );
    int         Width             ( );
    int         Height            ( );

    void        FixRotAroundVup   ( float angle );
    void        FixRotAroundRight ( float angle );
    void        FixRotAroundDir   ( float angle );

    void        RotAroundVup      ( float angle );
    void        RotAroundRight    ( float angle );
    void        RotAroundDir      ( float angle );

    void        MoveOnSphere      ( float delta_theta, float delta_phi );
    void        ChangeDistance    ( float deltaDist );
    void        Panning           ( float dx, float dy );
    void        MoveOnXY          ( float dx, float dy );
    void        MoveOnZ           ( float dz );

    void        SetDistance       ( float distance );
    float       GetDistance       ();
    void        SetTheta          ( float theta );
    float       GetTheta          ();
    void        SetPhi            ( float phi );
    float       GetPhi            ();

    void        SetMinDistance    ( float min_dist );

    void        SetClearColors ( glm::vec4 col );
    glm::vec4   GetClearColors ( );

    void        SetViewMX_lookAt  ( bool upsideDown = false );
    void        SetProjMX_persp   ( );
    void        viewport          ( );

    float*      ProjMatrixPtr  ( );
    float*      ViewMatrixPtr  ( );

    glm::mat4   ProjMatrix     ( );
    glm::mat4   ViewMatrix     ( );

    void        SetPanStep     ( float panStep );
    float       GetPanStep     ( );
    void        SetDistFactor  ( float distFactor );
    float       GetDistFactor  ( );
    void        SetPanXYFactor ( float panXYfactor );
    float       GetPanXYFactor ( );
    void        SetRotFactor   ( float rotFactor );
    float       GetRotFactor   ( );

    void        SetMousePos    ( int x, int y );
    void        GetMousePos    ( int &x, int &y );
    void        GetMouseRelPos ( int x, int y, int &dx, int &dy );

    void        Print ( FILE* ptr = stderr );

    // --------- protected attributes -----------
protected:
    glm::vec3  m_pos;
    glm::vec3  m_dir;
    glm::vec3  m_vup;

    GLfloat    m_zNear;
    GLfloat    m_zFar;
    GLfloat    m_aspect;
    GLint      m_width;
    GLint      m_height;
    GLfloat    m_fovY;

    glm::vec3  m_right;
    glm::vec3  m_poi;

    GLfloat    m_dist;
    GLfloat    m_min_dist;

    glm::vec4  m_clearColor;

    glm::mat4  m_projMX;
    glm::mat4  m_viewMX;

    float      m_panStep;
    float      m_distFactor;
    float      m_panXYfactor;
    float      m_rotFactor;

    int        m_xlast;
    int        m_ylast;
};


#endif  // QTCHALDNI_CAMERA_H
