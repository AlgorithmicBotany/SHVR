#include "TextureObject.h"

TextureObject::TextureObject() {
	//Default values	
	id = 0;
	target = 0;
	dim = glm::ivec3(0, 0, 0);
	type = 0;
	format = 0;
	tex_num = -1;
	wrap = GL_CLAMP_TO_EDGE;
	filter = GL_NEAREST;
	align = 4;
}
TextureObject::~TextureObject() { if (id != 0) { glDeleteTextures(1, &id); } }

void TextureObject::init() {
	//Note: This needs to happen in the GL component of OpenGLWindow, or
	//the function will not work.
	initializeOpenGLFunctions();
	glGenTextures(1, &id);
}

void TextureObject::initParameters(int active_texture, GLenum tex_type, float tex_size,
	GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type) {
	initParameters(active_texture, tex_type, glm::ivec3(tex_size, 0, 0),
		gl_internal_format, gl_format, gl_source_type);
}
void TextureObject::initParameters(int active_texture, GLenum tex_type, glm::ivec2 dimensions,
	GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type) {
	initParameters(active_texture, tex_type, glm::ivec3(dimensions, 0),
		gl_internal_format, gl_format, gl_source_type);
}
void TextureObject::initParameters(int active_texture, GLenum tex_type, glm::ivec3 dimensions,
	GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type) {
	tex_num = active_texture;	
	target = tex_type;
	dim = dimensions;
	int_format = gl_internal_format;
	format = gl_format;
	type = gl_source_type;
}
void TextureObject::setting(GLenum _filter, GLenum _wrap, int _align) {
	filter = _filter;
	wrap = _wrap;
	align = _align;
}

void TextureObject::reloadSettings() {
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
	glPixelStorei(GL_UNPACK_ALIGNMENT, align);
}

void TextureObject::update() {
	glActiveTexture(GL_TEXTURE0 + tex_num);
	glBindTexture(target, id);
	reloadSettings();

	if (target == GL_TEXTURE_1D) {
		glTexImage1D(target,
			0,
			int_format,
			dim.x,
			0,
			format,
			type,
			NULL
			);
	}
	else if (target == GL_TEXTURE_2D) {
		glTexImage2D(target,
			0,
			int_format,
			dim.x, dim.y,
			0,
			format,
			type,
			NULL
			);
	}
	else if (target == GL_TEXTURE_3D) {
		glTexImage3D(target,
			0,
			int_format,
			dim.x, dim.y, dim.z,
			0,
			format,
			type,
			NULL
			);
	}
}


void TextureObject::update(void* b_data) {
	glActiveTexture(GL_TEXTURE0 + tex_num);
	glBindTexture(target, id);
	reloadSettings();

	if (target == GL_TEXTURE_1D) {
		glTexImage1D(target,
			0,
			int_format,
			dim.x,
			0,
			format,
			type,
			b_data
			);
	}
	else if (target == GL_TEXTURE_2D) {
		glTexImage2D(target,
			0,
			int_format,
			dim.x, dim.y,
			0,
			format,
			type,
			b_data
			);
	}
	else if (target == GL_TEXTURE_3D) {
		glTexImage3D(target,
			0,
			int_format,
			dim.x, dim.y, dim.z,
			0,
			format,
			type,
			b_data
			);
	}
}

void TextureObject::update(void* b_data, const glm::ivec3& idxMin, const glm::ivec3& idxMax) {
	glm::ivec3 dim_update = idxMax - idxMin;	//dimensions of subsection updated
	glActiveTexture(GL_TEXTURE0 + tex_num);
	glBindTexture(target, id);
	reloadSettings();
	if (target == GL_TEXTURE_1D) {
		glTexSubImage1D(target,
			0,
			idxMin.x,
			dim_update.x,
			format,
			type,
			b_data
			);
	}
	else if (target == GL_TEXTURE_2D) {
		glTexSubImage2D(target,	//target
			0,							//level
			idxMin.x, idxMin.y,
			dim_update.x, dim_update.y,
			format,
			type,
			b_data
			);
	}
	else if (target == GL_TEXTURE_3D) {
		glTexSubImage3D(target,
			0,
			idxMin.x, idxMin.y, idxMin.z,
			dim_update.x, dim_update.y, dim_update.z,
			format,
			type,
			b_data
			);
	}
}