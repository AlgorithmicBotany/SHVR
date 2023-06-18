#include "RenderObject.h"

RenderObject::RenderObject() {
	// 0 == uninitialized buffer_id
	vao = 0;
	vbo = 0;
	clr = 0;
	nml = 0;
	elm = 0;
	tex = 0;
	int tex_dim = 0;
}
RenderObject::~RenderObject() {
	clear();
}

void RenderObject::init() { initializeOpenGLFunctions(); }
void RenderObject::clear() {
	if (vao != 0) { glDeleteVertexArrays(1, &vao); }
	if (vbo != 0) { glDeleteBuffers(1, &vbo); }
	if (clr != 0) { glDeleteBuffers(1, &clr); }
	if (nml != 0) { glDeleteBuffers(1, &nml); }
	if (elm != 0) { glDeleteBuffers(1, &elm); }
	if (tex != 0) { glDeleteBuffers(1, &tex); }
}

//Generate buffers
void RenderObject::genVAO() { glGenVertexArrays(1, &vao); }
void RenderObject::genBuf(GLuint& buffer_obj) { glGenBuffers(1, &buffer_obj); }
void RenderObject::genVBO() { genBuf(vbo); }
void RenderObject::genCLR() { genBuf(clr); }
void RenderObject::genNML() { genBuf(nml); }
void RenderObject::genELM() { genBuf(elm); }
void RenderObject::genTEX(int tex_dimension) { genBuf(tex); tex_dim = tex_dimension; }

//Update buffers
template <class T>
void RenderObject::updateBuf(GLuint buf_id, std::vector<T>& buf) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buf_id);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(T) * buf.size(),
		buf.data(),
		GL_STATIC_DRAW);
}
void RenderObject::updateVBO(std::vector<glm::vec3>& b) { updateBuf(vbo, b); }
void RenderObject::updateCLR(std::vector<glm::vec3>& b) { updateBuf(clr, b); }
void RenderObject::updateNML(std::vector<glm::vec3>& b) { updateBuf(nml, b); }
void RenderObject::updateELM(std::vector<glm::ivec3>& b) {
	//Note: Indices are buggy as int, so I am converting to ushort
	std::vector<GLushort> shorts;
	for (int i = 0; i < b.size(); i++) {
		shorts.push_back(b[i].x);
		shorts.push_back(b[i].y);
		shorts.push_back(b[i].z);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elm);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(GLushort) * shorts.size(),
		shorts.data(),
		GL_STATIC_DRAW);
}
void RenderObject::updateTEX(std::vector<glm::vec3>& b) { updateBuf(tex, b); }
void RenderObject::updateTEX(std::vector<glm::vec2>& b) { updateBuf(tex, b); }

//Attribute Buffers
void RenderObject::enableAttribVBO() {
	int attrib_idx = BufferID::VERTEX;		//loc 0: vertices
	glEnableVertexAttribArray(attrib_idx);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(
		attrib_idx,         // attribute intex
		3,                  // size
		GL_FLOAT,           // state
		GL_FALSE,           // normalized?
		0,                  // stride (0 := data is tightly packed)
		(void*)0            // array buffer offset
		);
}
void RenderObject::enableAttribCLR() {
	int attrib_idx = BufferID::COLOR;		//loc 1: vertex colour
	glEnableVertexAttribArray(attrib_idx);
	glBindBuffer(GL_ARRAY_BUFFER, clr);
	glVertexAttribPointer(
		attrib_idx,         // attribute intex
		3,                  // size
		GL_FLOAT,           // state
		GL_FALSE,           // normalized?
		0,                  // stride (0 := data is tightly packed)
		(void*)0            // array buffer offset
		);
}
void RenderObject::enableAttribNML() {
	int attrib_idx = BufferID::NORMAL;		//loc 2: vertex normals
	glEnableVertexAttribArray(attrib_idx);
	glBindBuffer(GL_ARRAY_BUFFER, nml);
	glVertexAttribPointer(
		attrib_idx,         // attribute intex
		3,                  // size
		GL_FLOAT,           // state
		GL_FALSE,           // normalized?
		0,                  // stride (0 := data is tightly packed)
		(void*)0            // array buffer offset
		);
}
void RenderObject::enableAttribTEX() {
	int attrib_idx = BufferID::TEXCOORD;	//loc 3: texture coordinates (3D)
	glEnableVertexAttribArray(attrib_idx);
	glBindBuffer(GL_ARRAY_BUFFER, tex);
	glVertexAttribPointer(
		attrib_idx,         // attribute intex
		tex_dim,            // size
		GL_FLOAT,           // state
		GL_FALSE,           // normalized?
		0,                  // stride (0 := data is tightly packed)
		(void*)0            // array buffer offset
		);
}

void RenderObject::enableAttribs() {
	if (vbo != 0) { enableAttribVBO(); }
	if (clr != 0) { enableAttribCLR(); }
	if (nml != 0) { enableAttribNML(); }
	if (tex != 0) { enableAttribTEX(); }
}


