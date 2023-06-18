#pragma once

#include <qopenglfunctions_4_0_core.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include "Volume.h"
//#include "hapticmanager.h"
//#include "volumemanager.h"
//#include "nodemanager.h"
//#include "tfmanager.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace utility {

	struct Frustum {
		enum F_Colors {
			C_NEAR,
			C_FAR,
			SIDE0,
			SIDE1,
			SIDE2,
			SIDE3,
		};

		std::vector<glm::vec3> c{
			glm::vec3(0.9,0.9,0.9),		//clipping near plane
			glm::vec3(0.3,0.3,0.3),		//clipping far plane
			glm::vec3(1,0,0),			//side 0
			glm::vec3(0,1,0),			//side 1
			glm::vec3(1,1,0),			//side 2
			glm::vec3(0,1,1),			//side 3
		};

		std::vector<glm::vec3> v;	//vertices
		std::vector<glm::ivec2> e;	//edges
		std::vector<char> e_c;		//edge colors
		std::vector<glm::ivec3> f;	//faces

		std::vector<glm::vec3> v_near;
		std::vector<glm::vec3> c_near;

		std::vector<glm::vec3> v_far;
		std::vector<float> uv_far;
		std::vector<glm::vec3> c_far;

		Frustum() {
			v.resize(8);
			e.resize(12);
			e_c.resize(12);
			f.resize(12); //6 faces * 2 triangles per face

						  //==== Edges ====//
						  //Side lines
			e[0] = glm::ivec2(0, 1);
			e[1] = glm::ivec2(2, 3);
			e[2] = glm::ivec2(4, 5);
			e[3] = glm::ivec2(6, 7);
			e_c[0] = F_Colors::SIDE0;
			e_c[1] = F_Colors::SIDE1;
			e_c[2] = F_Colors::SIDE2;
			e_c[3] = F_Colors::SIDE3;

			// clipping near plane
			e[4] = glm::ivec2(0, 2);
			e[5] = glm::ivec2(2, 6);
			e[6] = glm::ivec2(6, 4);
			e[7] = glm::ivec2(4, 0);
			e_c[4] = F_Colors::C_NEAR;
			e_c[5] = F_Colors::C_NEAR;
			e_c[6] = F_Colors::C_NEAR;
			e_c[7] = F_Colors::C_NEAR;

			//clipping far plane
			e[8] = glm::ivec2(1, 3);
			e[9] = glm::ivec2(3, 7);
			e[10] = glm::ivec2(7, 5);
			e[11] = glm::ivec2(5, 1);
			e_c[8] = F_Colors::C_FAR;
			e_c[9] = F_Colors::C_FAR;
			e_c[10] = F_Colors::C_FAR;
			e_c[11] = F_Colors::C_FAR;

			//==== Faces ====//
			f[0] = glm::ivec3(0, 1, 2);
			f[1] = glm::ivec3(3, 2, 1);
			f[2] = glm::ivec3(2, 3, 6);
			f[3] = glm::ivec3(7, 6, 3);
			f[4] = glm::ivec3(1, 3, 5);
			f[5] = glm::ivec3(7, 5, 3);
			f[6] = glm::ivec3(0, 1, 4);
			f[7] = glm::ivec3(5, 4, 1);
			f[8] = glm::ivec3(4, 5, 6);
			f[9] = glm::ivec3(7, 6, 5);
			f[10] = glm::ivec3(0, 2, 4);
			f[11] = glm::ivec3(6, 4, 2);
		}

		glm::vec3 getMidPoint() {
			glm::vec3 sum(0, 0, 0);
			for (int i = 0; i < v.size(); i++) {
				sum += v[i];
			}
			return sum /= (float)v.size();
		}

		void updateLines(glm::mat4 viewMatrix) {
			v_near.clear();
			v_far.clear();
			c_near.clear();
			c_far.clear();
			uv_far.clear();

			glm::vec3 m_frus = getMidPoint();
			glm::vec3 v_frus = glm::vec3(glm::inverse(viewMatrix) * glm::vec4(0, 0, 1, 1));
			glm::vec3 p0, p1, m_line, v_line;
			for (int i = 0; i < e.size(); i++) {
				p0 = v[e[i].x];
				p1 = v[e[i].y];
				m_line = (p0 + p1) / 2.f;
				v_line = m_frus - m_line;

				if (glm::dot(v_frus, v_line) <= 0) {
					v_near.push_back(p0);
					v_near.push_back(p1);
					c_near.push_back(c[e_c[i]]);
					c_near.push_back(c[e_c[i]]);
				}

				else {
					v_far.push_back(p0);
					v_far.push_back(p1);
					c_far.push_back(c[e_c[i]] * 0.8f);
					c_far.push_back(c[e_c[i]] * 0.8f);
					uv_far.push_back(0);
					uv_far.push_back(1);
				}
			}
		}

		std::vector<glm::vec3> getLines() {
			std::vector<glm::vec3> out;
			for (int i = 0; i < e.size(); i++) {
				out.push_back(v[e[i].x]);
				out.push_back(v[e[i].y]);
			}
			return out;
		}

		std::vector<glm::vec3> getLinesColor() {
			std::vector<glm::vec3> out;
			for (int i = 0; i < e.size(); i++) {
				out.push_back(c[e_c[i]]);
				out.push_back(c[e_c[i]]);
			}
			return out;
		}

		std::vector<glm::vec3> getBox() {
			std::vector<glm::vec3> out;
			for (int i = 0; i < f.size(); i++) {
				out.push_back(v[f[i].x]);
				out.push_back(v[f[i].y]);
				out.push_back(v[f[i].z]);
			}
			return out;
		}
	};
	
	struct Model {
		std::vector<glm::vec3> vtx;	//vertex positions (model)
		std::vector<glm::vec3> clr;	//color (***probably unnecessary?)
		std::vector<glm::vec3> nml;	//normal (model)
		std::vector<glm::ivec3> elm;//element buffer
		std::vector<glm::vec2> tex2;//texture coordinates (2D)
		std::vector<glm::vec3> tex3;//texture coordinates (3D)
		
		void scaleVTX(float scale) {
			for (int i = 0; i < vtx.size(); i++) {
				vtx[i] *= scale;
			}
		}
		void scaleVTX(glm::vec3 scale) {
			for (int i = 0; i < vtx.size(); i++) {
				vtx[i].x *= scale.x;
				vtx[i].y *= scale.y;
				vtx[i].z *= scale.z;
			}
		}
		void fillColor(glm::vec3 c) {
			for (int i = 0; i < vtx.size(); i++) {
				clr.push_back(c);
			}
		}
		void correctBlenderIndex() {
			for (glm::ivec3 idx: elm) {
				qDebug() << "elm.push_back(glm::ivec3(" <<
					idx.x - 1 << "," << idx.y - 1 << "," << idx.z - 1 << "));";
			}
		}
		void calcNMLFlat() {
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
		void calcNMLSmooth() {
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
		void calcNML() {
			if (elm.size() > 0) { calcNMLSmooth(); }
			else { calcNMLFlat(); }	
		}
		void flatELM() {
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
		void clear() {
			vtx.clear();
			clr.clear();
			nml.clear();
			elm.clear();
			tex2.clear();
			tex3.clear();
		}
		virtual void generate() {}
	};

	struct MBox: public Model {
		void genVTX() {
			vtx.push_back(glm::vec3(1.0, 1.0, 1.0));
			vtx.push_back(glm::vec3(1.0, 1.0, -1.0));
			vtx.push_back(glm::vec3(1.0, -1.0, 1.0));
			vtx.push_back(glm::vec3(1.0, -1.0, -1.0));
			vtx.push_back(glm::vec3(-1.0, 1.0, 1.0));
			vtx.push_back(glm::vec3(-1.0, 1.0, -1.0));
			vtx.push_back(glm::vec3(-1.0, -1.0, 1.0));
			vtx.push_back(glm::vec3(-1.0, -1.0, -1.0));
		}

		void genELM() {
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

		void genTEX3() {
			tex3.push_back(glm::vec3(1.0, 1.0, 1.0));
			tex3.push_back(glm::vec3(1.0, 1.0, 0.0));
			tex3.push_back(glm::vec3(1.0, 0.0, 1.0));
			tex3.push_back(glm::vec3(1.0, 0.0, 0.0));
			tex3.push_back(glm::vec3(0.0, 1.0, 1.0));
			tex3.push_back(glm::vec3(0.0, 1.0, 0.0));
			tex3.push_back(glm::vec3(0.0, 0.0, 1.0));
			tex3.push_back(glm::vec3(0.0, 0.0, 0.0));
		}

		void generate() {
			clear();	//clear old model
			genVTX();
			genELM();
			genTEX3();
		}

		void updateScale(float scale) {	
			//scale = boxWidth * boxScale			
			for (int i = 0; i < vtx.size(); ++i) {
				vtx[i] *= scale;		//***Should I scale in the matrix?
			}
		}
	};

	struct MQuad : public Model {
		void genVTX() {
			//Note: Originally rendered as array
			//vtx.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
			//vtx.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
			//vtx.push_back(glm::vec3(-1.0f,  1.0f, 0.0f));
			//vtx.push_back(glm::vec3(-1.0f,  1.0f, 0.0f));
			//vtx.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
			//vtx.push_back(glm::vec3(1.0f,  1.0f, 0.0f));

			vtx.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
			vtx.push_back(glm::vec3(1.0f, -1.0f, 0.0f)); 
			vtx.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
			vtx.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
		}
		void genELM() {
			elm.push_back(glm::ivec3(0, 1, 2));
			elm.push_back(glm::ivec3(2, 1, 3));
		}
		void genTEX2() {
			tex2.push_back(glm::vec2(0.f, 0.f)); //0
			tex2.push_back(glm::vec2(1.f, 0.f)); //1
			tex2.push_back(glm::vec2(0.f, 1.f)); //2
			tex2.push_back(glm::vec2(1.f, 1.f)); //3
		}

		void generate() {
			clear();
			genVTX();
			genELM();
			genTEX2();
		}

		void updateQuad(glm::vec2 lower_left, glm::vec2 upper_right) {
			vtx[0] = glm::vec3(lower_left.x, lower_left.y, 0.0f);
			vtx[1] = glm::vec3(upper_right.x, lower_left.y, 0.0f);
			vtx[2] = glm::vec3(lower_left.x, upper_right.y, 0.0f);
			vtx[3] = glm::vec3(upper_right.x, upper_right.y, 0.0f);
		}
	};

	struct MSphere : public Model {

		// ico sphere //
		void genVTX() {
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
		void genELM() {
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
		void genNML() {			
			calcNML();
		}
		void generate() {
			clear();
			genVTX();
			genELM();
			genNML();
			//Note: ignoring vertex colour b/c it's not used in shader
		}
	};

	struct MCylinder : public Model {
		//***This needs testing
		int n_sides = 10;		

		void cylinderVTX_ELM(float radius_low, float radius_high, float low_z, float high_z) {
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
				GLfloat x = radius_high * cos(i * step);
				GLfloat y = radius_high * sin(i * step);

				vtx.push_back(glm::vec3(x, y, high_z));
			}
			for (int i = 0; i < n_sides; i++) {
				GLfloat x = radius_low * cos(i * step);
				GLfloat y = radius_low * sin(i * step);

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
				GLfloat x = radius_high * cos(i * step);
				GLfloat y = radius_high * sin(i * step);

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
				GLfloat x = radius_low * cos(i * step);
				GLfloat y = radius_low * sin(i * step);

				vtx.push_back(glm::vec3(x, y, low_z));
			}
		}

		void cylinderVTX_ELM(float radius, float low_z, float high_z) {
			cylinderVTX_ELM(radius, radius, low_z, high_z);
		}

		void genNML() {
			calcNML();
		}

		void generate() {
			cylinderVTX_ELM(1, -1, 1);
			genNML();
		}
	};

	struct MArrow : public MCylinder {
		void coneVTX_ELM(float radius, float ring_z, float point_z) {
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
				GLfloat x = radius * cos(i * step);
				GLfloat y = radius * sin(i * step);

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
				GLfloat x = radius * cos(i * step);
				GLfloat y = radius * sin(i * step);

				vtx.push_back(glm::vec3(x, y, ring_z));
			}
		}
		
		void genArrow() {
			//top of cone
			coneVTX_ELM(1, 0.25, 1);

			//bottom of cone
			cylinderVTX_ELM(0.5, -1, 0.25);
		}
		void genNML() {
			calcNML();
		}

		void generate() {
			clear();
			genArrow();			
			genNML();
		}
	};

	struct RSetting {
		//Settings to keep consistent through the render??

		float step;		//step sizes
	};

	struct Shaders {

	};

	struct RObject: protected QOpenGLFunctions_4_0_Core {
		enum BufferID {
			//Note: Don't change the order, unless you update all the shader locations too
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

		RObject() {
			initializeOpenGLFunctions();

			// 0 == uninitialized buffer_id
			vao = 0;
			vbo	= 0;
			clr	= 0;
			nml	= 0;
			elm	= 0;
			tex	= 0;	
			int tex_dim = 0;
		}
		~RObject() {
			if (vao != 0) { glDeleteVertexArrays(1, &vao); }
			if (vbo != 0) { glDeleteBuffers(1, &vbo); }
			if (clr != 0) { glDeleteBuffers(1, &clr); }
			if (nml != 0) { glDeleteBuffers(1, &nml); }
			if (elm != 0) { glDeleteBuffers(1, &elm); }
			if (tex != 0) { glDeleteBuffers(1, &tex); }
		}

		//Generate buffers
		void genVAO() { glGenVertexArrays(1, &vao); }
		void genBuf(GLuint &buffer_obj) { glGenBuffers(1, &buffer_obj); }
		void genVBO() { genBuf(vbo); }
		void genCLR() { genBuf(clr); }
		void genNML() { genBuf(nml); }
		void genELM() { genBuf(elm); }
		void genTEX(int tex_dimension) { genBuf(tex); tex_dim = tex_dimension; }
		
		//Update buffers
		template <class T>
		void updateBuf(GLuint buf_id, std::vector<T> &buf) {
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, buf_id);
			glBufferData(GL_ARRAY_BUFFER, 
				sizeof(T) * buf.size(), 
				buf.data(), 
				GL_STATIC_DRAW);		
		}
		void updateVBO(std::vector<glm::vec3> &b) { updateBuf(vbo, b); }
		void updateCLR(std::vector<glm::vec3> &b) { updateBuf(clr, b); }
		void updateNML(std::vector<glm::vec3> &b) { updateBuf(nml, b); }
		void updateELM(std::vector<glm::ivec3> &b) {
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
		void updateTEX(std::vector<glm::vec3> &b) { updateBuf(tex, b); }
		void updateTEX(std::vector<glm::vec2> &b) { updateBuf(tex, b); }

		//Attribute Buffers
		void enableAttribVBO() {
			int attrib_idx = BufferID::VERTEX;		//loc 0: vertices
			glEnableVertexAttribArray(attrib_idx);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(
				attrib_idx,         // attribute intex
				3,                  // size
				GL_FLOAT,           // state
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
		}
		void enableAttribCLR() {
			int attrib_idx = BufferID::COLOR;		//loc 1: vertex colour
			glEnableVertexAttribArray(attrib_idx);
			glBindBuffer(GL_ARRAY_BUFFER, clr);
			glVertexAttribPointer(
				attrib_idx,         // attribute intex
				3,                  // size
				GL_FLOAT,           // state
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
		}
		void enableAttribNML() {
			int attrib_idx = BufferID::NORMAL;		//loc 2: vertex normals
			glEnableVertexAttribArray(attrib_idx);
			glBindBuffer(GL_ARRAY_BUFFER, nml);
			glVertexAttribPointer(
				attrib_idx,         // attribute intex
				3,                  // size
				GL_FLOAT,           // state
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
		}
		void enableAttribTEX() {
			int attrib_idx = BufferID::TEXCOORD;	//loc 3: texture coordinates (3D)
			glEnableVertexAttribArray(attrib_idx);
			glBindBuffer(GL_ARRAY_BUFFER, tex);
			glVertexAttribPointer(
				attrib_idx,         // attribute intex
				tex_dim,            // size
				GL_FLOAT,           // state
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
		}
		
		void enableAttribs() {
			if (vbo != 0) { enableAttribVBO(); }
			if (clr != 0) { enableAttribCLR(); }
			if (nml != 0) { enableAttribNML(); }
			if (tex != 0) { enableAttribTEX(); }
		}

		void generate(Model &m) {
			genVAO();
			if (m.vtx.size() > 0) {
				genVBO();
				updateVBO(m.vtx);
				enableAttribVBO();
			}
			if (m.elm.size() > 0) {
				genELM();
				updateELM(m.elm);
			}
			if (m.clr.size() > 0) {
				genCLR();
				updateCLR(m.clr);
				enableAttribCLR();
			}
			if (m.nml.size() > 0) {
				genNML();
				updateNML(m.nml);
				enableAttribNML();
			}
			if (m.tex2.size() > 0) {
				genTEX(2);
				updateTEX(m.tex2);
				enableAttribTEX();
			}
			if (m.tex3.size() > 0) {
				genTEX(3);
				updateTEX(m.tex3);
				enableAttribTEX();
			}
		}
	};

	struct RTexture: protected QOpenGLFunctions_4_0_Core {		
		GLuint id;
		GLenum target;		//GL_TEXTURE_1D, GL_TEXTURE_2D, or GL_TEXTURE_3D
		glm::ivec3 dim;		//dimensions (width, height, depth)
		GLenum int_format;	//internal format (slower to update if different from source format)
		GLenum type;		//data in CPU (GL_FLOAT, GL_INT, etc.)
		GLenum format;		//data to GPU (GL_RED, GL_RGBA, etc.)		
		int tex_num;		//allocated active texture

		//Texture settings
		GLenum wrap;		//Setting for wrapping beyond texture dimensions (GL_CLAMP_TO_EDGE, GL_REPEAT)
		GLenum filter;		//Setting for stretching or shrinking texture dimensions (GL_LINEAR, NEAREST)
		int align;			//Alignment for start of each pixel row in memory (1 = byte, 4 = word)

		RTexture() {
			initializeOpenGLFunctions();

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
		~RTexture() { if (id = !0) { glDeleteTextures(1, &id); } }
		void generate(int active_texture, GLenum tex_type, float tex_size,
			GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type) {
			generate(active_texture, tex_type, glm::ivec3(tex_size,0, 0),
				gl_internal_format, gl_format, gl_source_type);
		}
		void generate(int active_texture, GLenum tex_type, glm::ivec2 dimensions,
			GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type) {
			generate(active_texture, tex_type, glm::ivec3(dimensions,0),
				gl_internal_format, gl_format, gl_source_type);
		}
		void generate(int active_texture, GLenum tex_type, glm::ivec3 dimensions, 
			GLenum gl_internal_format, GLenum gl_format, GLenum gl_source_type) {
			tex_num = active_texture;
			glGenTextures(1, &id); 
			target = tex_type;
			dim = dimensions;
			int_format = gl_internal_format;
			format = gl_format;
			type = gl_source_type;			
		}
		void setting(GLenum _filter, GLenum _wrap, int _align) {
			filter = _filter;
			wrap = _wrap;
			align = _align;
		}

		void reloadSettings() {
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);
			glPixelStorei(GL_UNPACK_ALIGNMENT, align);
		}		

		void update() {
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

		template <class T> 
		void update(T* b_data) {
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

		template <class T> 
		void update(T* b_data, glm::ivec3 idx_start, glm::ivec3 idx_end) {
			glm::ivec3 dim_update = idx_end - idx_start;	//dimensions of subsection updated
			glActiveTexture(GL_TEXTURE0 + tex_num);
			glBindTexture(target, id);
			reloadSettings();
			if (target == GL_TEXTURE_1D) { 
				glTexSubImage1D(target,
					0,
					idx_start.x,
					dim_update.x,
					format,
					type,
					b_data
				);
			}
			else if (target == GL_TEXTURE_2D) { 
				glTexSubImage2D(target,	//target
					0,							//level
					idx_start.x, idx_start.y,
					dim_update.x, dim_update.y,
					format,
					type,
					b_data
				);
			}
			else if (target == GL_TEXTURE_3D) { 
				glTexSubImage3D(target,
					0,
					idx_start.x, idx_start.y, idx_start.z,
					dim_update.x, dim_update.y, dim_update.z,
					format,
					type,
					b_data
				);
			}
		}
	};

	struct RVolume {
		enum ULength{
			nm,			//nanometre (10^-9m)
			micro_m,	//micrometre (10^-6m)
			mm,			//millimetre (10^-3m)
			cm,			//centimetre (10^-2m)
			m			//metre (1m)
		};
		
		//dimensions
		glm::ivec3 dim;	//total pixel dimensions (width, height, depth)

		//distance per voxel
		float scale;		//base volume scale
		glm::vec3 spacing;	//space distance per pixel in x,y,z dir
		float uLength;	//unit length per unit of space	
		ULength uType;	//Unit Length type
		
		//data storage
		Data::Volume* base;	//visible | visualized volume
		Data::Volume* mask;	//colour on top of visible volume
		Math::Array3D<std::vector<unsigned char>>* lyrs;	//layers
		
		void updateMask() {
			//update mask to show only active groups 
			//***TODO
		}
	};

	struct Screen {
		//Whatever that is laid on top of window
		enum ScreenType {
			FULL,
			STEREO,
			QUAD,
			COUNT
		};
		//
		int type;	//One of screen types

					//Screen dimensions
		int width;		//pixel width
		int height;		//pixel height
		int viewWidth;	//Currently drawing pixel width
		int viewHeight;	//Currently drawing pixel height

		float aspectRatio() { return (float)height / (float)width; }

		glm::vec2 screen2NDC(glm::ivec2 screen_coord) {
			//normalize window screen position in between [-1, 1]
			return glm::vec2(
				2 * ((float)screen_coord.x / (float)viewWidth) - 1,
				-(2 * ((float)screen_coord.y / (float)viewHeight) - 1)	//we want bottom left corner to be -1, not +1
			);
		}
		glm::ivec2 NDC2Screen(glm::vec2 ndc_coord) {
			return glm::ivec2(
				(float)viewWidth*ndc_coord.x / 2.f + 1.f,
				-(float)viewHeight*ndc_coord.y / 2.f + 1
			);
		}

		int clickedScreen(glm::ivec3 screen_coord) {
			/*
			Dual:
			|---------------|
			|	0	|	1	|
			|---------------|

			Quad :
			|---------------|
			|	0	|	1	|
			|---------------|
			|	2	|	3	|
			|---------------|
			*/

			//Quad Screen
			if (type == ScreenType::QUAD) {
				if (screen_coord.y < viewHeight) {
					if (screen_coord.x < viewWidth) { return 0; }
					else { return 1; }
				}

				else {
					if (screen_coord.x < viewWidth) { return 2; }
					else { return 3; }
				}
			}

			//Two Screens
			else if (type == ScreenType::STEREO) {
				if (screen_coord.x < viewWidth) { return 0; }
				else { return 1; }
			}

			//Single Screen
			else if (type == ScreenType::FULL) { return 0; }

			else {
				return -1;	//Not valid screen state
			}
		}
	};

	struct Camera {
		//Rendering within 3D world
		//Camera Settings
		float planeN;	//near plane
		float planeF;	//far plane
		float clipN;	//clipping near
		float clipF;	//clipping far		
		glm::vec3 cutN;	//cutting near (n>=0)
		glm::vec3 cutF;	//cutting far (n<=1)		

		glm::vec3 prev_p;	//previous point of interaction

		glm::vec3 pos;	//position
		glm::vec3 cen;	//center
		glm::vec3 up;	//up direction

		Screen* scr;

		glm::mat4 P;	//Projection matrix
		glm::mat4 V;	//View Matrix (
		glm::mat4 M;	//Model Matrix (camera pos relative to volume pos)

		void updateSettings(
			float plane_near,
			float plane_far,
			float clip_near,
			float clip_far,
			glm::vec3 cut_near,
			glm::vec3 cut_far) {

			planeN = plane_near;
			planeF = plane_far;
			clipN = clip_near;
			clipF = clip_far;
			cutN = cut_near;
			cutF = cut_far;
		}

		void updateCamera(
			glm::vec3 position,
			glm::vec3 center,
			glm::vec3 up_dir) {

			pos = position;
			cen = center;
			up = up_dir;
		}

		// Update Matrices
		void updateV() { V = glm::lookAt(pos, cen, up); }
		void updateP() {}

		float distY() { return -1; }	//distance from center of screen (from planeN) to the top of screen in world position	
		float distX() { return -1; }	//distance from center of screen (from planeN) to the right of screen in world position	
		float distZ() { return pos.z - (planeN + clipN); }	//distance from camera position to the near clipping plane

		glm::vec2 Camera::world2NDC(glm::vec3 world_coord) {
			return glm::vec2(
				world_coord.x / distX(),
				world_coord.y / distY()
			);
		}
		glm::vec3 Camera::NDC2World(glm::vec2 ndc_coord) {
			return glm::vec3(
				distX() * ndc_coord.x,
				distY() * ndc_coord.y,
				distZ());
		}

		bool probeInsideScreen(glm::vec3 probe) { return false; }

		//Mouse activities (click, drag, scroll wheel)
		glm::vec3 clickPoint(glm::ivec2 screen_coord) {
			return NDC2World(scr->screen2NDC(screen_coord));
		}

		glm::vec3 dragPoint(glm::ivec2 screen_coord) {
			glm::vec3 old_wpos = prev_p;
			glm::vec3 new_wpos = NDC2World(scr->screen2NDC(screen_coord));
			prev_p = new_wpos;						//update old screen point
			return new_wpos - old_wpos;
		}

		glm::vec3 scroll() {
			//***TODO
		}
	};

	struct CamOrtho :public Camera {
		float dist_y;

		CamOrtho(Screen* screen) {
			scr = screen;
		}
				
		void updateP() {
			float orthoY = distY();
			float orthoX = distX();
			P = glm::ortho(-orthoX, orthoX, -orthoY, orthoY, planeN, planeF);
		}

		//distance from center of screen (from planeN) to the top of screen in world position
		float distY() {
			//dist_y = (vManager->getLongestBoxLength() / 2);
			return dist_y;
		}

		//distance from center of screen (from planeN) to the right of screen in world position
		float distX() { return distY() * scr->aspectRatio(); }

		bool probeInsideScreen(glm::vec3 probe) {
			//Return if cursor is outside of field of view in both x and y direction 
			return ((distX() < probe.x) || distY() < probe.y);
		}
	};

	struct CamPerspective :public Camera {
		float fov;		//Field of view

		CamPerspective(Screen* screen) {
			scr = screen;
		}

		//distance from center of screen (from planeN) to the top of screen in world position
		float distY() { return (planeN + clipN) * std::tan((fov / 2.) * (M_PI / 180.)); }

		//distance from center of screen (from planeN) to the right of screen in world position
		float distX() { return distY() * scr->aspectRatio(); }

		bool probeInsideScreen(glm::vec3 probe) {
			//Note: Assumes camera is in z-direction (Camera space?)
			glm::vec3 probeX(probe.x, 0.0, probe.z);
			float cosThetaX = glm::dot(
				glm::normalize(glm::vec3(0, 0, 1)),	//Direction of camera 
				glm::normalize(probeX - pos));		//Direction toward the cursor from camera
			float thetaX = acos(cosThetaX);

			glm::vec3 probeY(0.0, probe.y, probe.z);
			float cosThetaY = glm::dot(
				glm::normalize(glm::vec3(0, 0, 1)),	//Direction of camera
				glm::normalize(probeY - pos));		//Direction toward the cursor from camera
			float thetaY = acos(cosThetaY);

			//Return if cursor is outside of field of view in both x and y direction 
			return ((180 - (thetaX / M_PI * 180.0)) > fov * scr->aspectRatio() / 2.0) ||
				((180 - (thetaY / M_PI * 180.0)) > fov / 2.0);
		}
	};

	
}

using namespace utility;

class RenderManager {

public:
	RenderManager();
	~RenderManager();
	
	//***seems really similiar to vrcw
	void init();
	void update();
	void draw();

	////World
	//Camera* cam;
	//Screen* scr;

	//Shaders
};

