#include "ShaderProgram.h"

#include <string>
#include <algorithm>
//#include <qdebug.h>

namespace GLExt
{
ShaderProgram::ShaderProgram():
        m_ID( 0 )
{
	
}

ShaderProgram::~ShaderProgram()
{
    if (m_ID != 0) { glDeleteProgram(m_ID); }
}

GLuint ShaderProgram::createProgram()
{
	initializeOpenGLFunctions();	//Needed to use OpenGL functions under Qt Library
    m_ID = glCreateProgram();

    if ( m_ID == 0 )
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    return m_ID;
}

void ShaderProgram::useProgram()
{
    glUseProgram( m_ID );
}

GLuint ShaderProgram::getUniformID( const char * uniformName )
{
    return glGetUniformLocation( m_ID, uniformName );
}

void ShaderProgram::addShader( const char * file_path, GLenum shaderType )
{
    GLuint shaderID = glCreateShader( shaderType );
    if ( shaderID == 0 )
    {
        fprintf(stderr, "Error creating shader type %d\n", shaderType);
        exit(0);
    }

    std::string shaderCode;
    std::ifstream shaderStream( file_path, std::ios::in );
    if ( shaderStream.is_open() )
    {
        std::string line  = "";
        while ( getline( shaderStream, line ) )
            shaderCode += "\n" + line;
        shaderStream.close();
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    printf( "Compiling shader : %s\n", file_path );
    char const * sourcePointer = shaderCode.c_str();
    glShaderSource( shaderID, 1, &sourcePointer , NULL);
    glCompileShader( shaderID );

    // Check Vertex Shader
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
    glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );
	std::vector<char> shaderErrorMessage(std::max(infoLogLength, int(1)) );
    glGetShaderInfoLog( shaderID, infoLogLength, NULL, &shaderErrorMessage[0] );
    fprintf(stdout, "%s\n", &shaderErrorMessage[0]);
	qDebug() << &shaderErrorMessage[0];

    // LinkNT the program
    fprintf( stdout, "Linking program\n" );
    glAttachShader( m_ID, shaderID );
    glLinkProgram( m_ID );

    // Check the program
    glGetProgramiv( m_ID, GL_LINK_STATUS, &result );
    glGetProgramiv( m_ID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> programErrorMessage( std::max( infoLogLength, int(1)) );
    glGetProgramInfoLog( m_ID, infoLogLength, NULL, &programErrorMessage[0] );
    fprintf(stdout, "%s\n", &programErrorMessage[0]);
	qDebug() << &programErrorMessage[0];

    glDeleteShader( shaderID );
}


void ShaderProgram::setInt(const char* uniformName, int value) {
	GLuint uniformID = getUniformID(uniformName);
	glUniform1i(uniformID, value);
}

void ShaderProgram::setFloat(const char* uniformName, float value) {
	GLuint uniformID = getUniformID(uniformName);
	glUniform1f(uniformID, value);
}

void ShaderProgram::setVec2(const char* uniformName, const glm::vec2& value) {
	GLuint uniformID = getUniformID(uniformName);
	glUniform2fv(uniformID, 1, &value[0]);
}

void ShaderProgram::setVec2(const char* uniformName, float v0, float v1) {
	GLuint uniformID = getUniformID(uniformName);
	glUniform2f(uniformID, v0, v1);
}

void ShaderProgram::setVec3(const char* uniformName, const glm::vec3& value) {
	GLuint uniformID = getUniformID(uniformName);
	glUniform3fv(uniformID, 1, &value[0]);
}

void ShaderProgram::setVec3(const char* uniformName, float v0, float v1, float v2) {
	GLuint uniformID = getUniformID(uniformName);
	glUniform3f(uniformID, v0, v1, v2);
}

void ShaderProgram::setVec4(const char* uniformName, const glm::vec4& value) {
    GLuint uniformID = getUniformID(uniformName);
    glUniform4fv(uniformID, 1, &value[0]);
}


void ShaderProgram::setMat4(const char* uniformName, const glm::mat4& value) {
	GLuint uniformID = getUniformID(uniformName);
	glUniformMatrix4fv(uniformID, 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::setIntArray(const char* uniformName, const std::vector<int>& values) {
    GLuint uniformID = getUniformID(uniformName);
    glUniform1iv(uniformID, values.size(), &values[0]);
}

void ShaderProgram::setVec3Array(const char* uniformName, const std::vector<glm::vec3>& values) {
    GLuint uniformID = getUniformID(uniformName);
    glUniform3fv(uniformID, values.size(), &values[0][0]);
}

void ShaderProgram::setVec4Array(const char* uniformName, const std::vector<glm::vec4>& values) {
    GLuint uniformID = getUniformID(uniformName);
    glUniform4fv(uniformID, values.size(), &values[0][0]);
}

}
