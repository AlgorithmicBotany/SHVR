#ifndef GL_SHADER_PROGRAM
#define GL_SHADER_PROGRAM

//#include <GL/glew.h>
//#include <GL/gl.h>

#include <QtGui\qopenglfunctions_4_0_core.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <vector>

#include <qdebug.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace GLExt
{
class ShaderProgram : protected QOpenGLFunctions_4_0_Core
{
public:
    ShaderProgram();
    ~ShaderProgram();

private:
    GLuint m_ID;

public:
    void useProgram();
    GLuint createProgram();
    void addShader( const char * file_path, GLenum ShaderType );
    GLuint getUniformID( const char * uniformName );

	void setInt(const char* uniformName, int value);
	void setFloat(const char* uniformName, float value);
	void setVec2(const char* uniformName, const glm::vec2& value);
	void setVec2(const char * uniformName, float v0, float v1);
	void setVec3(const char* uniformName, const glm::vec3& value);
	void setVec3(const char* uniformName, float v0, float v1, float v2);
	void setVec4(const char* uniformName, const glm::vec4& value);
	void setMat4(const char* uniformName, const glm::mat4& value);
	void setIntArray(const char* uniformName, const std::vector<int>& values);
	void setVec3Array(const char* uniformName, const std::vector<glm::vec3>& values);
	void setVec4Array(const char* uniformName, const std::vector<glm::vec4>& values);
};
}


#endif //GL_SHADER_PROGRAM
