#pragma once

#include <glm/glm.hpp>
#include <qopenglfunctions_4_0_core.h>

struct TextureObject : protected QOpenGLFunctions_4_0_Core {

	// Texture information
	GLuint id;
	GLenum target;		//GL_TEXTURE_1D, GL_TEXTURE_2D, or GL_TEXTURE_3D
	glm::ivec3 dim;	//dimensions (width, height, depth)
	GLenum int_format;	//internal format (slower to update if different from source format)
	GLenum type;		//data in CPU (GL_FLOAT, GL_INT, etc.)
	GLenum format;		//data to GPU (GL_RED, GL_RGBA, etc.)		
	int tex_num;		//allocated active texture

	//Texture settings
	GLenum wrap;		//Setting for wrapping beyond texture dimensions (GL_CLAMP_TO_EDGE, GL_REPEAT)
	GLenum filter;		//Setting for stretching or shrinking texture dimensions (GL_LINEAR, NEAREST)
	int align;			//Alignment for start of each pixel row in memory (1 = byte, 4 = word)

	TextureObject();
	~TextureObject();
	void init();
	void initParameters(int active_texture, GLenum tex_type, float tex_size,
		GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type);
	void initParameters(int active_texture, GLenum tex_type, glm::ivec2 dimensions,
		GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type);
	void initParameters(int active_texture, GLenum tex_type, glm::ivec3 dimensions,
		GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type);
	void setting(GLenum _filter, GLenum _wrap, int _align);
	void reloadSettings();

	void update();
	void update(void* b_data);
	void update(void* b_data, const glm::ivec3& idxMin, const glm::ivec3& idxMax);
};
