/**
    @file   GLShader.h

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

#ifndef GL_SHADER_H
#define GL_SHADER_H


#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <cstdio>
#include <cstring>

#include <GL3/gl3w.h>
  
/**
 * @brief The GLShader class
 */
class GLShader
{
public:
    GLShader( bool haveTessShader = false );
    ~GLShader();
    
    /** Activate/deactivate tessellation shader.
     *   Set to false if you don't want to use tessellation shaders
     *   even if your graphics board has tess shader capability.
     * \param haveTessShader
     */
    void    SetTessShaderAvailability( bool haveTessShader );

    /** Bind shader program.
     */
    void    Bind();

    /** Bind shader attribute location.
     * \param attribIndex  index of attribute
     * \param attribName   name of attribute
     * \param linkProg  link program after attaching attribute
     */
    bool    BindAttribLocation( unsigned int attribIndex, const char* attribName, bool linkProg = false );
    
    /** Create empty program.
     *  Shaders must be attached before linking!
     */
    void    CreateEmptyProgram();

    /** Create shader program from file
     * \param vShaderText  vertex shader file name
     * \param fShaderText  fragment shader file name
     */
    bool    CreateProgramFromFile( const char* vShaderName, 
                                   const char* fShaderName );

    /** Create shader program from file
     * \param vShaderText  vertex shader file name
     * \param gShaderText  geometry shader file name
     * \param fShaderText  fragment shader file name
     */
    bool    CreateProgramFromFile( const char* vShaderName, 
                                   const char* gShaderName, 
                                   const char* fShaderName );

    /** Create shader program from file
     * \param vShaderText   vertex shader file name
     * \param tcShaderText  tessellation control shader file name
     * \param teShaderText  tessellation evaluation shader file name
     * \param fShaderText  fragment shader file name
     */
    bool    CreateProgramFromFile( const char* vShaderName,
                                   const char* tcShaderName,
                                   const char* teShaderName,
                                   const char* fShaderName );

    /** Create shader program from file
     * \param vShaderText   vertex shader file name
     * \param tcShaderText  tessellation control shader file name
     * \param teShaderText  tessellation evaluation shader file name
     * \param gShaderText  geometry shader file name
     * \param fShaderText  fragment shader file name
     */
    bool    CreateProgramFromFile( const char* vShaderName,
                                   const char* tcShaderName,
                                   const char* teShaderName,
                                   const char* gShaderName, 
                                   const char* fShaderName );

    /** Create shader program from text strings.
     * \param vShaderText  vertex shader text
     * \param vShaderLen   length of vertex shader text
     * \param fShaderText  fragment shader text
     * \param fShaderLen   length of fragment shader text
     */
    bool    CreateProgramFromString( const char* vShaderText, const unsigned int vShaderLen,
                                     const char* fShaderText, const unsigned int fShaderLen );
    
    /** Attach shader to program from file.
     * \param shaderName  shader file name
     * \param shaderType  type of shader
     * \param linkProg  link program after attaching shader
     */
    bool    AttachShaderFromFile( const char* shaderName, GLenum shaderType, bool linkProg = false );

    /** Attach shader to program from text string.
     * \param shaderText  shader text
     * \param shaderLen   length of shader text
     * \param shaderType  type of shader
     * \param linkProg  link program after attaching shader
     */
    bool    AttachShaderFromString( const char* shaderText, const unsigned int shaderLen, GLenum shaderType, bool linkProg = false );
    
    /** Get program handle id.
     */
    GLuint  GetProgHandle();

    /** Get uniform location.
     * \param name  name of uniform variable
     */
    GLint   GetUniformLocation(const char* name);   

    /** Link shader program.
     */
    bool    Link();
    
    void    PrintInfo(FILE* fptr = stderr);

    /** Remove all shaders from shader program.
     */
    void    RemoveAllShaders();

    /** Release shader program.
     */
    void    Release();   
    
    
protected:      
    /** Compile shader from file.
     * \param shaderFilename  name of shader file
     * \param type : shader type
     */
    GLuint  compileShaderFromFile(const char* shaderFilename, GLenum type);

    /** Compile shader from text string.
     * \param shaderText  shader text
     * \param shaderLen   length of shader text
     * \param type : shader type
     */
    GLuint  compileShaderFromString(const char* shaderText, const unsigned int shaderLen, GLenum type);

    /** Read shader text from file.
     * \param shaderFilename  name of shader file
     * \param shaderContent  shader text read from file
     */
    GLuint  readShaderFromFile(const char* shaderFilename, std::string &shaderContent );
      
    bool    printShaderInfoLog     ( GLuint shader );
    bool    printProgramInfoLog    ();   
    
// ---------- private attributes --------
 private:
    bool    m_haveTessShader;
    GLuint  progHandle;
};
  
#endif  // GL_SHADER_H
