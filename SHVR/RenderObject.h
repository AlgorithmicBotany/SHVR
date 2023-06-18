#pragma once

#include <glm/glm.hpp>
#include <QOpenGLFunctions_4_0_Core>

struct RenderObject : protected QOpenGLFunctions_4_0_Core {
	enum BufferID {
		//Note: The order corresponds to all the shader locations, so don't change it
		VERTEX,
		COLOR,
		NORMAL,
		TEXCOORD,
		COUNT
	};

	//render object's id					
	GLuint vao;
	GLuint vbo;	//vertex positions (model)
	GLuint clr;	//color
	GLuint nml;	//normal (model)
	GLuint elm;	//element buffer
	GLuint tex;	//texture coordinates (3D)

	int tex_dim;//2D or 3D
	int size;

	RenderObject();
	~RenderObject();

	void init();
	void clear();

	//Generate buffers
	void genVAO();
	void genBuf(GLuint& buffer_obj);
	void genVBO();
	void genCLR();
	void genNML();
	void genELM();
	void genTEX(int tex_dimension);

	template<class T>
	void updateBuf(GLuint buf_id, std::vector<T>& buf);
	void updateVBO(std::vector<glm::vec3>& b);
	void updateCLR(std::vector<glm::vec3>& b);
	void updateNML(std::vector<glm::vec3>& b);
	void updateELM(std::vector<glm::ivec3>& b);
	void updateTEX(std::vector<glm::vec3>& b);
	void updateTEX(std::vector<glm::vec2>& b);

	//Attribute Buffers
	void enableAttribVBO();
	void enableAttribCLR();
	void enableAttribNML();
	void enableAttribTEX();
	void enableAttribs();
};