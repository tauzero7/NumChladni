/**
    @file   GLShader.cpp

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

#include "GLShader.h"

#include <fstream>
#include <sstream>

GLShader::GLShader(bool haveTessShader) :
    m_haveTessShader(haveTessShader),
    progHandle(0) {
}

GLShader::~GLShader() {
    RemoveAllShaders();
}

void GLShader::SetTessShaderAvailability( bool haveTessShader ) {
    m_haveTessShader = haveTessShader;
}

bool GLShader::CreateProgramFromFile(const char* vShaderName, const char* fShaderName) {
    GLuint vShader = compileShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint fShader = compileShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromFile(const char* vShaderName, 
        const char* gShaderName, const char* fShaderName) {
        
    GLuint vShader = compileShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint gShader = compileShaderFromFile(gShaderName,GL_GEOMETRY_SHADER);
    if (gShader==0)  return false;
    GLuint fShader = compileShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, gShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromFile( const char* vShaderName,
        const char* tcShaderName, const char* teShaderName,
        const char* fShaderName ) {

    GLuint vShader = compileShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint tcShader = compileShaderFromFile(tcShaderName,GL_TESS_CONTROL_SHADER);
    if (tcShader==0)  return false;
    GLuint teShader = compileShaderFromFile(teShaderName,GL_TESS_EVALUATION_SHADER);
    if (teShader==0)  return false;
    GLuint fShader = compileShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, tcShader );
    glAttachShader(progHandle, teShader );
    glAttachShader(progHandle, fShader );
    return Link();
}

bool GLShader::CreateProgramFromFile( const char* vShaderName,
        const char* tcShaderName, const char* teShaderName,
        const char* gShaderName, const char* fShaderName ) {
 
    GLuint vShader = compileShaderFromFile(vShaderName,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint tcShader = compileShaderFromFile(tcShaderName,GL_TESS_CONTROL_SHADER);
    if (tcShader==0)  return false;
    GLuint teShader = compileShaderFromFile(teShaderName,GL_TESS_EVALUATION_SHADER);
    if (teShader==0)  return false;
    GLuint gShader = compileShaderFromFile(gShaderName,GL_GEOMETRY_SHADER);
    if (gShader==0)  return false;
    GLuint fShader = compileShaderFromFile(fShaderName,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader );
    glAttachShader(progHandle, tcShader );
    glAttachShader(progHandle, teShader );
    glAttachShader(progHandle, gShader );
    glAttachShader(progHandle, fShader );
    return Link();
}


bool GLShader::CreateProgramFromString(const char* vShaderText, const unsigned int vShaderLen,
                                             const char* fShaderText, const unsigned int fShaderLen) {
    GLuint vShader = compileShaderFromString(vShaderText,vShaderLen,GL_VERTEX_SHADER);
    if (vShader==0)  return false;
    GLuint fShader = compileShaderFromString(fShaderText,fShaderLen,GL_FRAGMENT_SHADER);
    if (fShader==0)  return false;

    progHandle = glCreateProgram();
    glAttachShader(progHandle, vShader);
    glAttachShader(progHandle, fShader);
    return Link();
}

bool GLShader::AttachShaderFromFile( const char* shaderName, GLenum shaderType, bool linkProg ) {
    if (progHandle==0) {
        return false;  
    }
    GLuint shaderHandle = compileShaderFromFile(shaderName,shaderType);
    if (shaderHandle==0) {
        return false;
    }
    glAttachShader(progHandle,shaderHandle);
    if (linkProg) {
        return Link();
    }
    return true;
}

bool GLShader::AttachShaderFromString( const char* shaderText, const unsigned int shaderLen, GLenum shaderType, bool linkProg ) {
    if (progHandle==0) {
        return false;
    }
    GLuint shaderHandle = compileShaderFromString(shaderText,shaderLen,shaderType);
    if (shaderHandle==0) {
        return false;
    }
    glAttachShader(progHandle,shaderHandle);
    if (linkProg) {
        return Link();
    }
    return true;
}

void GLShader::Bind() {
    glUseProgram(progHandle);
}

bool GLShader::BindAttribLocation( unsigned int attribIndex, const char* attribName, bool linkProg ) {
    glBindAttribLocation( progHandle, attribIndex, attribName ); 
    if (linkProg) {
        return Link();
    }
    return true;    
}

void GLShader::CreateEmptyProgram() {
   if (progHandle!=0) {
       RemoveAllShaders();
   }
   progHandle = glCreateProgram();
}

void GLShader::Release() {
    glUseProgram(0);
}

GLuint GLShader::GetProgHandle() {
    return progHandle;
}

GLint GLShader::GetUniformLocation(const char* name) {
    return glGetUniformLocation(progHandle,name);
}

bool GLShader::Link() {
    glLinkProgram(progHandle);
    bool status = printProgramInfoLog();
    glUseProgram(0);
    return (status==GL_TRUE);
}

GLuint GLShader::readShaderFromFile(const char* shaderFilename, std::string &shaderContent )
{
    std::ifstream in(shaderFilename);  
    if (!in) {
       const unsigned int length = 256;
       char msg[length];
#if defined _WIN32 && !defined __MINGW32__
       sprintf_s(msg, length, "Cannot open file \"%s\"",shaderFilename);
#else
       sprintf(msg,"Cannot open file \"%s\"",shaderFilename);
#endif
       fprintf(stderr,"Error: %s\n",msg);
       return 0;
    }

    std::stringstream shaderData;


    if (m_haveTessShader) {
        shaderData << "#version 420\n";
    } else {
        shaderData << "#version 330\n";
    }
    shaderData << "#define PI     3.14159265\n";
    shaderData << "#define PI_2   1.5707963\n";
    shaderData << "#define SQ2    0.707106781\n";

    shaderData << in.rdbuf();
    in.close();
    shaderContent = shaderData.str();
    return shaderContent.size();
}


 GLuint GLShader::compileShaderFromFile(const char* shaderFilename, GLenum type ) {
    std::string shaderText;
    GLuint iShaderLen = readShaderFromFile(shaderFilename,shaderText);
    if (iShaderLen==0) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    const char *strShaderVar = shaderText.c_str();
    glShaderSource(shader,1,(const GLchar**)&strShaderVar, (GLint*)&iShaderLen);
    glCompileShader(shader);

    if (!printShaderInfoLog(shader)) {
        std::stringstream iss(shaderText);
        std::string sLine;
        int lineCounter = 1;
        while(std::getline(iss,sLine)) {
            fprintf(stderr,"%4d : %s\n",(lineCounter++),sLine.c_str());
        }
    }
    return shader;
}

/*! Create shader from string.
 * \param shaderText : shader text.
 * \param type : shader type.
 * \return shader id.
 */
GLuint GLShader::compileShaderFromString(const char* shaderText, const unsigned int shaderLen, GLenum type) {
    if (shaderLen==0) {
        return 0;
    }

    GLuint shader = glCreateShader(type);
    glShaderSource(shader,1,(const GLchar**)&shaderText, (GLint*)&shaderLen);
    glCompileShader(shader);
   
    if (!printShaderInfoLog(shader)) {
        std::stringstream iss(shaderText);
        std::string sLine;
        int lineCounter = 1;
        while(std::getline(iss,sLine)) {
            fprintf(stderr,"%4d : %s\n",(lineCounter++),sLine.c_str());
        }
    }
    return shader;
}

/*! Print shader information log.
 * 
 */
bool GLShader::printShaderInfoLog( GLuint shader ) {
    int infoLogLen   = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    int compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS,  &compileStatus);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 1)  {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
        printf("InfoLog : %s\n", infoLog);
        delete [] infoLog;
    }
    return (compileStatus == GL_TRUE);
}

/*! Print program information log.
 * 
 */
bool GLShader::printProgramInfoLog() {
    int infoLogLen   = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    int linkStatus = 0;
    glGetProgramiv(progHandle, GL_INFO_LOG_LENGTH, &infoLogLen);
    glGetProgramiv(progHandle, GL_LINK_STATUS, &linkStatus);

    if (infoLogLen > 1)  {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(progHandle, infoLogLen, &charsWritten, infoLog);
        printf("ProgramInfoLog : %s\n", infoLog);
        delete [] infoLog;
    }
    return (linkStatus == GL_TRUE);
}

/*!
 */
void GLShader::RemoveAllShaders() {
    if (progHandle==0) {
        return;
    }
      
    if (!glIsProgram(progHandle)) {
        return;
    }
   
    const GLsizei numShaders = 1024;
    GLsizei numReturned;
    GLuint shaders[numShaders];
    glUseProgram(0);
   
    glGetAttachedShaders(progHandle, numShaders, &numReturned, shaders);
    for (GLsizei i = 0; i < numReturned; i++) {
        glDetachShader(progHandle, shaders[i]);
        glDeleteShader(shaders[i]);
    }
    glDeleteProgram(progHandle);
    progHandle = 0;
}

