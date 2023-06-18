#include "VertexObject.h"

void VertexObject::scaleVTX(float scale) {
	for (int i = 0; i < vtx.size(); i++) {
		vtx[i] *= scale;
	}
}
void VertexObject::scaleVTX(glm::vec3 scale) {
	for (int i = 0; i < vtx.size(); i++) {
		vtx[i].x *= scale.x;
		vtx[i].y *= scale.y;
		vtx[i].z *= scale.z;
	}
}
void VertexObject::fillColor(glm::vec3 c) {
	for (int i = 0; i < vtx.size(); i++) {
		clr.push_back(c);
	}
}
void VertexObject::correctBlenderIndex() {
	for (glm::ivec3 idx : elm) {
		qDebug() << "elm.push_back(glm::ivec3(" <<
			idx.x - 1 << "," << idx.y - 1 << "," << idx.z - 1 << "));";
	}
}
void VertexObject::calcNMLFlat() {
	//Calculate the normal of each face
	for (int i = 0; i < vtx.size(); i = i + 3) {
		glm::vec3 n0 = vtx[i];
		glm::vec3 n1 = vtx[i + 1];
		glm::vec3 n2 = vtx[i + 2];
		glm::vec3 normal_face = glm::normalize(glm::cross(n1 - n0, n2 - n0));

		//Store the normal for each vertex
		nml.push_back(normal_face);
		nml.push_back(normal_face);
		nml.push_back(normal_face);
	}
}
void VertexObject::calcNMLSmooth() {
	//Calculate the average normal for each vertex
	std::vector<std::vector<glm::vec3>> nml_list(vtx.size());	//one vertex is 3 floats (f, f, f)
	for (int i = 0; i < elm.size(); i++) {
		glm::vec3 n0 = vtx[elm[i].x];
		glm::vec3 n1 = vtx[elm[i].y];
		glm::vec3 n2 = vtx[elm[i].z];

		glm::vec3 normal_face = glm::normalize(glm::cross(n1 - n0, n2 - n0));

		nml_list[elm[i].x].push_back(normal_face);
		nml_list[elm[i].y].push_back(normal_face);
		nml_list[elm[i].z].push_back(normal_face);
	}

	//Calculate the normal at each vertex
	for (int i = 0; i < nml_list.size(); i++) {

		//take the average of all face normals around one vertex
		glm::vec3 vertex_normal(0, 0, 0);
		for (int j = 0; j < nml_list[i].size(); j++) {
			vertex_normal += nml_list[i][j];
		}
		vertex_normal *= (1.f / (float)nml_list[i].size());

		//only keep direction of normal
		vertex_normal = glm::normalize(vertex_normal);
		nml.push_back(vertex_normal);
	}
}
void VertexObject::calcNML() {
	if (elm.size() > 0) { calcNMLSmooth(); }
	else { calcNMLFlat(); }
}
void VertexObject::flatELM() {
	//convert indices to only vertices
	std::vector<glm::vec3> vtx_temp;
	for (int i = 0; i < elm.size(); i++) {
		vtx_temp.push_back(vtx[elm[i].x]);
		vtx_temp.push_back(vtx[elm[i].y]);
		vtx_temp.push_back(vtx[elm[i].z]);
	}

	//clear existing vertices and indices
	vtx.clear();
	elm.clear();

	//swap with converted vertices
	vtx.swap(vtx_temp);
}
void VertexObject::clear() {
	vtx.clear();
	clr.clear();
	nml.clear();
	elm.clear();
	tex2.clear();
	tex3.clear();
}

//======

void VOBox::genVTX() {
	vtx.push_back(glm::vec3(0.5, 0.5, 0.5));
	vtx.push_back(glm::vec3(0.5, 0.5, -0.5));
	vtx.push_back(glm::vec3(0.5, -0.5, 0.5));
	vtx.push_back(glm::vec3(0.5, -0.5, -0.5));
	vtx.push_back(glm::vec3(-0.5, 0.5, 0.5));
	vtx.push_back(glm::vec3(-0.5, 0.5, -0.5));
	vtx.push_back(glm::vec3(-0.5, -0.5, 0.5));
	vtx.push_back(glm::vec3(-0.5, -0.5, -0.5));
}

void VOBox::genELM() {
	elm.push_back(glm::ivec3(0, 4, 6));
	elm.push_back(glm::ivec3(6, 2, 0));
	elm.push_back(glm::ivec3(1, 0, 2));
	elm.push_back(glm::ivec3(2, 3, 1));
	elm.push_back(glm::ivec3(5, 1, 3));
	elm.push_back(glm::ivec3(3, 7, 5));
	elm.push_back(glm::ivec3(4, 5, 7));
	elm.push_back(glm::ivec3(7, 6, 4));
	elm.push_back(glm::ivec3(4, 0, 1));
	elm.push_back(glm::ivec3(1, 5, 4));
	elm.push_back(glm::ivec3(6, 7, 3));
	elm.push_back(glm::ivec3(3, 2, 6));
}

void VOBox::genTEX3D() {
	tex3.push_back(glm::vec3(1.0, 1.0, 1.0));
	tex3.push_back(glm::vec3(1.0, 1.0, 0.0));
	tex3.push_back(glm::vec3(1.0, 0.0, 1.0));
	tex3.push_back(glm::vec3(1.0, 0.0, 0.0));
	tex3.push_back(glm::vec3(0.0, 1.0, 1.0));
	tex3.push_back(glm::vec3(0.0, 1.0, 0.0));
	tex3.push_back(glm::vec3(0.0, 0.0, 1.0));
	tex3.push_back(glm::vec3(0.0, 0.0, 0.0));
}

void VOBox::generate() {
	clear();	//clear old model
	genVTX();
	genELM();
	genTEX3D();
}


void VOBox::updateVTX(const glm::vec3& vMin, const glm::vec3& vMax) {
	vtx[0] = (glm::vec3(vMax.x, vMax.y, vMax.z));
	vtx[1] = (glm::vec3(vMax.x, vMax.y, vMin.z));
	vtx[2] = (glm::vec3(vMax.x, vMin.y, vMax.z));
	vtx[3] = (glm::vec3(vMax.x, vMin.y, vMin.z));
	vtx[4] = (glm::vec3(vMin.x, vMax.y, vMax.z));
	vtx[5] = (glm::vec3(vMin.x, vMax.y, vMin.z));
	vtx[6] = (glm::vec3(vMin.x, vMin.y, vMax.z));
	vtx[7] = (glm::vec3(vMin.x, vMin.y, vMin.z));
}

//==========
void VOQuad::genVTX() {
	vtx.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
	vtx.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
	vtx.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
	vtx.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
}
void VOQuad::genELM() {
	elm.push_back(glm::ivec3(0, 1, 2));
	elm.push_back(glm::ivec3(2, 1, 3));
}
void VOQuad::genTEX2D() {
	tex2.push_back(glm::vec2(0.f, 0.f)); //0
	tex2.push_back(glm::vec2(1.f, 0.f)); //1
	tex2.push_back(glm::vec2(0.f, 1.f)); //2
	tex2.push_back(glm::vec2(1.f, 1.f)); //3
}

void VOQuad::generate() {
	clear();
	genVTX();
	genELM();
	genTEX2D();
}

void VOQuad::updateVTX(const glm::vec2& lower_left,const glm::vec2& upper_right) {
	vtx[0] = glm::vec3(lower_left.x, lower_left.y, 0.0f);
	vtx[1] = glm::vec3(upper_right.x, lower_left.y, 0.0f);
	vtx[2] = glm::vec3(lower_left.x, upper_right.y, 0.0f);
	vtx[3] = glm::vec3(upper_right.x, upper_right.y, 0.0f);
}
//======
	// ico sphere //
void VOSphere::genVTX() {
	vtx.push_back(glm::vec3(0.000000f, -1.000000f, 0.000000f));
	vtx.push_back(glm::vec3(0.723607f, -0.447220f, 0.525725f));
	vtx.push_back(glm::vec3(-0.276388f, -0.447220f, 0.850649f));
	vtx.push_back(glm::vec3(-0.894426f, -0.447216f, 0.000000f));
	vtx.push_back(glm::vec3(-0.276388f, -0.447220f, -0.850649f));
	vtx.push_back(glm::vec3(0.723607f, -0.447220f, -0.525725f));
	vtx.push_back(glm::vec3(0.276388f, 0.447220f, 0.850649f));
	vtx.push_back(glm::vec3(-0.723607f, 0.447220f, 0.525725f));
	vtx.push_back(glm::vec3(-0.723607f, 0.447220f, -0.525725f));
	vtx.push_back(glm::vec3(0.276388f, 0.447220f, -0.850649f));
	vtx.push_back(glm::vec3(0.894426f, 0.447216f, 0.000000f));
	vtx.push_back(glm::vec3(0.000000f, 1.000000f, 0.000000f));
	vtx.push_back(glm::vec3(-0.162456f, -0.850654f, 0.499995f));
	vtx.push_back(glm::vec3(0.425323f, -0.850654f, 0.309011f));
	vtx.push_back(glm::vec3(0.262869f, -0.525738f, 0.809012f));
	vtx.push_back(glm::vec3(0.850648f, -0.525736f, 0.000000f));
	vtx.push_back(glm::vec3(0.425323f, -0.850654f, -0.309011f));
	vtx.push_back(glm::vec3(-0.525730f, -0.850652f, 0.000000f));
	vtx.push_back(glm::vec3(-0.688189f, -0.525736f, 0.499997f));
	vtx.push_back(glm::vec3(-0.162456f, -0.850654f, -0.499995f));
	vtx.push_back(glm::vec3(-0.688189f, -0.525736f, -0.499997f));
	vtx.push_back(glm::vec3(0.262869f, -0.525738f, -0.809012f));
	vtx.push_back(glm::vec3(0.951058f, 0.000000f, 0.309013f));
	vtx.push_back(glm::vec3(0.951058f, 0.000000f, -0.309013f));
	vtx.push_back(glm::vec3(0.000000f, 0.000000f, 1.000000f));
	vtx.push_back(glm::vec3(0.587786f, 0.000000f, 0.809017f));
	vtx.push_back(glm::vec3(-0.951058f, 0.000000f, 0.309013f));
	vtx.push_back(glm::vec3(-0.587786f, 0.000000f, 0.809017f));
	vtx.push_back(glm::vec3(-0.587786f, 0.000000f, -0.809017f));
	vtx.push_back(glm::vec3(-0.951058f, 0.000000f, -0.309013f));
	vtx.push_back(glm::vec3(0.587786f, 0.000000f, -0.809017f));
	vtx.push_back(glm::vec3(0.000000f, 0.000000f, -1.000000f));
	vtx.push_back(glm::vec3(0.688189f, 0.525736f, 0.499997f));
	vtx.push_back(glm::vec3(-0.262869f, 0.525738f, 0.809012f));
	vtx.push_back(glm::vec3(-0.850648f, 0.525736f, 0.000000f));
	vtx.push_back(glm::vec3(-0.262869f, 0.525738f, -0.809012f));
	vtx.push_back(glm::vec3(0.688189f, 0.525736f, -0.499997f));
	vtx.push_back(glm::vec3(0.162456f, 0.850654f, 0.499995f));
	vtx.push_back(glm::vec3(0.525730f, 0.850652f, 0.000000f));
	vtx.push_back(glm::vec3(-0.425323f, 0.850654f, 0.309011f));
	vtx.push_back(glm::vec3(-0.425323f, 0.850654f, -0.309011f));
	vtx.push_back(glm::vec3(0.162456f, 0.850654f, -0.499995f));
}

void VOSphere::updateVTX(const glm::vec3& vtxMin, const glm::vec3& vtxMax) {
	vtx.clear();
	genVTX();
	for (glm::vec3& v: vtx) {
		v.x = (v.x - -1) / 2 * (vtxMax.x - vtxMin.x) + vtxMin.x;
		v.y = (v.y - -1) / 2 * (vtxMax.y - vtxMin.y) + vtxMin.y;
		v.z = (v.z - -1) / 2 * (vtxMax.z - vtxMin.z) + vtxMin.z;
	}
}

void VOSphere::genELM() {
	elm.push_back(glm::ivec3(0, 13, 12));
	elm.push_back(glm::ivec3(1, 13, 15));
	elm.push_back(glm::ivec3(0, 12, 17));
	elm.push_back(glm::ivec3(0, 17, 19));
	elm.push_back(glm::ivec3(0, 19, 16));
	elm.push_back(glm::ivec3(1, 15, 22));
	elm.push_back(glm::ivec3(2, 14, 24));
	elm.push_back(glm::ivec3(3, 18, 26));
	elm.push_back(glm::ivec3(4, 20, 28));
	elm.push_back(glm::ivec3(5, 21, 30));
	elm.push_back(glm::ivec3(1, 22, 25));
	elm.push_back(glm::ivec3(2, 24, 27));
	elm.push_back(glm::ivec3(3, 26, 29));
	elm.push_back(glm::ivec3(4, 28, 31));
	elm.push_back(glm::ivec3(5, 30, 23));
	elm.push_back(glm::ivec3(6, 32, 37));
	elm.push_back(glm::ivec3(7, 33, 39));
	elm.push_back(glm::ivec3(8, 34, 40));
	elm.push_back(glm::ivec3(9, 35, 41));
	elm.push_back(glm::ivec3(10, 36, 38));
	elm.push_back(glm::ivec3(38, 41, 11));
	elm.push_back(glm::ivec3(38, 36, 41));
	elm.push_back(glm::ivec3(36, 9, 41));
	elm.push_back(glm::ivec3(41, 40, 11));
	elm.push_back(glm::ivec3(41, 35, 40));
	elm.push_back(glm::ivec3(35, 8, 40));
	elm.push_back(glm::ivec3(40, 39, 11));
	elm.push_back(glm::ivec3(40, 34, 39));
	elm.push_back(glm::ivec3(34, 7, 39));
	elm.push_back(glm::ivec3(39, 37, 11));
	elm.push_back(glm::ivec3(39, 33, 37));
	elm.push_back(glm::ivec3(33, 6, 37));
	elm.push_back(glm::ivec3(37, 38, 11));
	elm.push_back(glm::ivec3(37, 32, 38));
	elm.push_back(glm::ivec3(32, 10, 38));
	elm.push_back(glm::ivec3(23, 36, 10));
	elm.push_back(glm::ivec3(23, 30, 36));
	elm.push_back(glm::ivec3(30, 9, 36));
	elm.push_back(glm::ivec3(31, 35, 9));
	elm.push_back(glm::ivec3(31, 28, 35));
	elm.push_back(glm::ivec3(28, 8, 35));
	elm.push_back(glm::ivec3(29, 34, 8));
	elm.push_back(glm::ivec3(29, 26, 34));
	elm.push_back(glm::ivec3(26, 7, 34));
	elm.push_back(glm::ivec3(27, 33, 7));
	elm.push_back(glm::ivec3(27, 24, 33));
	elm.push_back(glm::ivec3(24, 6, 33));
	elm.push_back(glm::ivec3(25, 32, 6));
	elm.push_back(glm::ivec3(25, 22, 32));
	elm.push_back(glm::ivec3(22, 10, 32));
	elm.push_back(glm::ivec3(30, 31, 9));
	elm.push_back(glm::ivec3(30, 21, 31));
	elm.push_back(glm::ivec3(21, 4, 31));
	elm.push_back(glm::ivec3(28, 29, 8));
	elm.push_back(glm::ivec3(28, 20, 29));
	elm.push_back(glm::ivec3(20, 3, 29));
	elm.push_back(glm::ivec3(26, 27, 7));
	elm.push_back(glm::ivec3(26, 18, 27));
	elm.push_back(glm::ivec3(18, 2, 27));
	elm.push_back(glm::ivec3(24, 25, 6));
	elm.push_back(glm::ivec3(24, 14, 25));
	elm.push_back(glm::ivec3(14, 1, 25));
	elm.push_back(glm::ivec3(22, 23, 10));
	elm.push_back(glm::ivec3(22, 15, 23));
	elm.push_back(glm::ivec3(15, 5, 23));
	elm.push_back(glm::ivec3(16, 21, 5));
	elm.push_back(glm::ivec3(16, 19, 21));
	elm.push_back(glm::ivec3(19, 4, 21));
	elm.push_back(glm::ivec3(19, 20, 4));
	elm.push_back(glm::ivec3(19, 17, 20));
	elm.push_back(glm::ivec3(17, 3, 20));
	elm.push_back(glm::ivec3(17, 18, 3));
	elm.push_back(glm::ivec3(17, 12, 18));
	elm.push_back(glm::ivec3(12, 2, 18));
	elm.push_back(glm::ivec3(15, 16, 5));
	elm.push_back(glm::ivec3(15, 13, 16));
	elm.push_back(glm::ivec3(13, 0, 16));
	elm.push_back(glm::ivec3(12, 14, 2));
	elm.push_back(glm::ivec3(12, 13, 14));
	elm.push_back(glm::ivec3(13, 1, 14));
}
void VOSphere::genNML() {
	calcNML();
}
void VOSphere::generate() {
	clear();
	genVTX();
	genELM();
	genNML();
	//Note: ignoring vertex colour b/color it's not used in shader
}

//==================




void VOCylinder::cylinderVTX_ELM(float radius_low, float radius_high, float low_z, float high_z) {
	glm::ivec3 s_idx;
	int n = n_sides;
	float step = 2 * M_PI / n_sides;
	//============== SIDES =================
	//elm
	s_idx = glm::ivec3(vtx.size());
	for (int i = 0; i < n_sides - 1; i++) {
		glm::ivec3 t0(i, i + n, i + 1);
		glm::ivec3 t1(i + 1 + n, i + 1, i + n);

		elm.push_back(s_idx + t0);
		elm.push_back(s_idx + t1);
	}
	elm.push_back(s_idx + glm::ivec3(n - 1, 2 * n - 1, 0));
	elm.push_back(s_idx + glm::ivec3(n, 0, 2 * n - 1));

	//vtx
	for (int i = 0; i < n_sides; i++) {
		float x = radius_high * cos(i * step);
		float y = radius_high * sin(i * step);

		vtx.push_back(glm::vec3(x, y, high_z));
	}
	for (int i = 0; i < n_sides; i++) {
		float x = radius_low * cos(i * step);
		float y = radius_low * sin(i * step);

		vtx.push_back(glm::vec3(x, y, low_z));
	}
	//============= CYLINDER TOP/BOTTOM ===============
	//Create top of cylinder
	//elm
	s_idx = glm::ivec3(vtx.size());
	for (int i = 1; i < n_sides; i++) {
		glm::ivec3 t(i + 1, 0, i);
		elm.push_back(s_idx + t);
	}

	elm.push_back(s_idx + glm::ivec3(1, 0, n));

	//vtx
	vtx.push_back(glm::vec3(0, 0, high_z));

	for (int i = 0; i < n_sides; i++) {
		float x = radius_high * cos(i * step);
		float y = radius_high * sin(i * step);

		vtx.push_back(glm::vec3(x, y, high_z));
	}

	//create bottom of cylinder
	//elm
	s_idx = glm::ivec3(vtx.size());
	for (int i = 1; i < n_sides; i++) {
		glm::ivec3 t(i, 0, i + 1);
		elm.push_back(s_idx + t);
	}

	elm.push_back(s_idx + glm::ivec3(n, 0, 1));

	//vtx
	vtx.push_back(glm::vec3(0, 0, low_z));
	for (int i = 0; i < n_sides; i++) {
		float x = radius_low * cos(i * step);
		float y = radius_low * sin(i * step);

		vtx.push_back(glm::vec3(x, y, low_z));
	}
}

void VOCylinder::cylinderVTX_ELM(float radius, float low_z, float high_z) {
	cylinderVTX_ELM(radius, radius, low_z, high_z);
}

void VOCylinder::genNML() {
	calcNML();
}

void VOCylinder::generate() {
	cylinderVTX_ELM(1, -1, 1);
	genNML();
}

//====================

void VOArrow::coneVTX_ELM(float radius, float ring_z, float point_z) {
	glm::ivec3 s_idx;
	int n = n_sides;
	float step = 2 * M_PI / n_sides;
	//============== SIDES =================
	//elm
	s_idx = glm::ivec3(vtx.size());
	for (int i = 0; i < n_sides - 1; i++) {
		glm::ivec3 t1(i + 1 + n, i + 1, i + n);

		elm.push_back(s_idx + t1);
	}
	elm.push_back(s_idx + glm::ivec3(n, 0, 2 * n - 1));

	//vtx
	for (int i = 0; i < n_sides; i++) {
		vtx.push_back(glm::vec3(0, 0, point_z));
	}
	for (int i = 0; i < n_sides; i++) {
		float x = radius * cos(i * step);
		float y = radius * sin(i * step);

		vtx.push_back(glm::vec3(x, y, ring_z));
	}

	//============= TOP/BOTTOM ===============	
	//elm
	s_idx = glm::ivec3(vtx.size());
	for (int i = 1; i < n_sides; i++) {
		glm::ivec3 t(i, 0, i + 1);
		elm.push_back(s_idx + t);
	}
	elm.push_back(s_idx + glm::ivec3(n, 0, 1));

	//create bottom of cylinder
	vtx.push_back(glm::vec3(0, 0, ring_z));
	for (int i = 0; i < n_sides; i++) {
		float x = radius * cos(i * step);
		float y = radius * sin(i * step);

		vtx.push_back(glm::vec3(x, y, ring_z));
	}
}

void VOArrow::genArrow() {
	//top of cone
	coneVTX_ELM(1, 0.25, 1);

	//bottom of cone
	cylinderVTX_ELM(0.5, -1, 0.25);
}
void VOArrow::genNML() {
	calcNML();
}

void VOArrow::generate() {
	clear();
	genArrow();
	genNML();
}
