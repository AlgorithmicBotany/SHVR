#pragma once

#include <vector>
#define _USE_MATH_DEFINES	//M_PI
#include <math.h>
#include <Qdebug>
#include <glm/glm.hpp>
#include "RenderObject.h"

struct VertexObject {
	std::vector<glm::vec3> vtx;	//vertex positions (model)
	std::vector<glm::vec3> clr;	//color (***probably unnecessary?)
	std::vector<glm::vec3> nml;	//normal (model)
	std::vector<glm::ivec3> elm;//element buffer
	std::vector<glm::vec2> tex2;//texture coordinates (2D)
	std::vector<glm::vec3> tex3;//texture coordinates (3D)

	void scaleVTX(float scale);
	void scaleVTX(glm::vec3 scale);
	void fillColor(glm::vec3 c);
	void correctBlenderIndex();
	void calcNMLFlat();
	void calcNMLSmooth();
	void calcNML();
	void flatELM();
	void clear();

	void initRO(RenderObject& ro) {	
		if (ro.vao == 0) { ro.genVAO();}
		if (vtx.size() > 0) {
			if (ro.vbo == 0) { 
				ro.genVBO(); 
				ro.updateVBO(vtx);	//Note: Logically, this should be unnecessary.
									//But without it, undesired polygons start popping up.
			}
		}
		if (elm.size() > 0) {
			if (ro.elm == 0) { ro.genELM(); }
		}
		if (clr.size() > 0) {
			if (ro.clr == 0) { ro.genCLR(); }
		}
		if (nml.size() > 0) {
			if (ro.nml == 0) { ro.genNML(); }
		}
		if (tex2.size() > 0) {
			if (ro.tex == 0) { ro.genTEX(2); }
		}
		else if (tex3.size() > 0) {
			if (ro.tex == 0) { ro.genTEX(3); }
		}
		updateRO(ro);
	}

	void updateRO(RenderObject& ro) {
		if (vtx.size() > 0) {
			ro.updateVBO(vtx);
			ro.enableAttribVBO();
			ro.size = vtx.size() * 3;		//*** How many elements are really there?
		}
		if (elm.size() > 0) {
			
			ro.updateELM(elm);

			//replace vertex size if elements are present
			ro.size = elm.size() * 3;		//*** How many elements are really there?
		}
		if (clr.size() > 0) {
			ro.updateCLR(clr);
			ro.enableAttribCLR();
		}
		if (nml.size() > 0) {			
			ro.updateNML(nml);
			ro.enableAttribNML();
		}
		if (tex2.size() > 0) {
			ro.updateTEX(tex2);
			ro.enableAttribTEX();
		}
		else if (tex3.size() > 0) {
			ro.updateTEX(tex3);
			ro.enableAttribTEX();
		}
	}
	//Template function for children classes
	virtual void generate() {}
};

struct VOBox : public VertexObject {
	void genVTX();
	void genELM();
	void genTEX3D();	
	void generate();

	void updateVTX(const glm::vec3& vMin, const glm::vec3& vMax);
};

struct VOQuad : public VertexObject {
	void genVTX();
	void genELM();
	void genTEX2D();
	void generate();
	void updateVTX(const glm::vec2& lower_left, const glm::vec2& upper_right);
};

struct VOSphere : public VertexObject {	// ico sphere //
	void genVTX();
	void updateVTX(const glm::vec3& vtxMin, const glm::vec3& vtxMax);
	void genELM();
	void genNML();
	void generate();
};

struct VOCylinder : public VertexObject {
	//***This needs testing
	int n_sides = 10;

	void cylinderVTX_ELM(float radius_low, float radius_high, float low_z, float high_z);
	void cylinderVTX_ELM(float radius, float low_z, float high_z);
	void genNML();
	void generate();
};

struct VOArrow : public VOCylinder {
	void coneVTX_ELM(float radius, float ring_z, float point_z);
	void genArrow();
	void genNML();
	void generate();
};

struct VOCircle : public VertexObject {
	void genVTX() {
		vtx.push_back(glm::vec3(-1.000000f, 0.000000f, 0.000000f));
		vtx.push_back(glm::vec3(-0.980785f, -0.195090f, 0.000000f));
		vtx.push_back(glm::vec3(-0.923880f, -0.382683f, 0.000000f));
		vtx.push_back(glm::vec3(-0.831470f, -0.555570f, 0.000000f));
		vtx.push_back(glm::vec3(-0.707107f, -0.707107f, 0.000000f));
		vtx.push_back(glm::vec3(-0.555570f, -0.831470f, 0.000000f));
		vtx.push_back(glm::vec3(-0.382683f, -0.923880f, 0.000000f));
		vtx.push_back(glm::vec3(-0.195090f, -0.980785f, 0.000000f));
		vtx.push_back(glm::vec3(-0.000000f, -1.000000f, 0.000000f));
		vtx.push_back(glm::vec3(0.195090f, -0.980785f, 0.000000f));
		vtx.push_back(glm::vec3(0.382683f, -0.923880f, 0.000000f));
		vtx.push_back(glm::vec3(0.555570f, -0.831470f, 0.000000f));
		vtx.push_back(glm::vec3(0.707107f, -0.707107f, 0.000000f));
		vtx.push_back(glm::vec3(0.831470f, -0.555570f, 0.000000f));
		vtx.push_back(glm::vec3(0.923880f, -0.382683f, 0.000000f));
		vtx.push_back(glm::vec3(0.980785f, -0.195090f, 0.000000f));
		vtx.push_back(glm::vec3(1.000000f, 0.000000f, 0.000000f));
		vtx.push_back(glm::vec3(0.980785f, 0.195091f, 0.000000f));
		vtx.push_back(glm::vec3(0.923879f, 0.382684f, 0.000000f));
		vtx.push_back(glm::vec3(0.831469f, 0.555571f, 0.000000f));
		vtx.push_back(glm::vec3(0.707106f, 0.707107f, 0.000000f));
		vtx.push_back(glm::vec3(0.555570f, 0.831470f, 0.000000f));
		vtx.push_back(glm::vec3(0.382683f, 0.923880f, 0.000000f));
		vtx.push_back(glm::vec3(0.195089f, 0.980785f, 0.000000f));
		vtx.push_back(glm::vec3(-0.000001f, 1.000000f, 0.000000f));
		vtx.push_back(glm::vec3(-0.195091f, 0.980785f, 0.000000f));
		vtx.push_back(glm::vec3(-0.382684f, 0.923879f, 0.000000f));
		vtx.push_back(glm::vec3(-0.555571f, 0.831469f, 0.000000f));
		vtx.push_back(glm::vec3(-0.707108f, 0.707106f, 0.000000f));
		vtx.push_back(glm::vec3(-0.831470f, 0.555569f, 0.000000f));
		vtx.push_back(glm::vec3(-0.923880f, 0.382682f, 0.000000f));
		vtx.push_back(glm::vec3(-0.980786f, 0.195089f, 0.000000f));
		vtx.push_back(glm::vec3(-0.000000f, -0.000000f, 0.000000f));
		
	}
	void genELM() {
		elm.push_back(glm::ivec3(11, 10, 32));
		elm.push_back(glm::ivec3(25, 24, 32));
		elm.push_back(glm::ivec3(12, 11, 32));
		elm.push_back(glm::ivec3(26, 25, 32));
		elm.push_back(glm::ivec3(13, 12, 32));
		elm.push_back(glm::ivec3(27, 26, 32));
		elm.push_back(glm::ivec3(14, 13, 32));
		elm.push_back(glm::ivec3(1, 0, 32));
		elm.push_back(glm::ivec3(28, 27, 32));
		elm.push_back(glm::ivec3(15, 14, 32));
		elm.push_back(glm::ivec3(2, 1, 32));
		elm.push_back(glm::ivec3(29, 28, 32));
		elm.push_back(glm::ivec3(16, 15, 32));
		elm.push_back(glm::ivec3(3, 2, 32));
		elm.push_back(glm::ivec3(30, 29, 32));
		elm.push_back(glm::ivec3(17, 16, 32));
		elm.push_back(glm::ivec3(4, 3, 32));
		elm.push_back(glm::ivec3(31, 30, 32));
		elm.push_back(glm::ivec3(18, 17, 32));
		elm.push_back(glm::ivec3(5, 4, 32));
		elm.push_back(glm::ivec3(0, 31, 32));
		elm.push_back(glm::ivec3(19, 18, 32));
		elm.push_back(glm::ivec3(6, 5, 32));
		elm.push_back(glm::ivec3(20, 19, 32));
		elm.push_back(glm::ivec3(7, 6, 32));
		elm.push_back(glm::ivec3(21, 20, 32));
		elm.push_back(glm::ivec3(8, 7, 32));
		elm.push_back(glm::ivec3(22, 21, 32));
		elm.push_back(glm::ivec3(9, 8, 32));
		elm.push_back(glm::ivec3(23, 22, 32));
		elm.push_back(glm::ivec3(10, 9, 32));
		elm.push_back(glm::ivec3(24, 23, 32));	
	}

	void generate() {
		genVTX();
		genELM();
	}
};

