#pragma once

#define _USE_MATH_DEFINES
#include <math.h>			//M_PI
#include <random>
#include <chrono>	
#include <thread>	

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

#include <QDebug>

//Note: Camera Space is the same as World Space due to the camera being fixed toward the origin

namespace MathLib {	
	void makeOffsets(float radius, int numberOfOffsets, std::vector<glm::vec3>& out_offsets);

	glm::mat4 translateM(const glm::vec3& v);
	glm::mat4 translateM(float x, float y, float z);
	glm::mat4 rotateXAxis(float theta);
	glm::mat4 rotateYAxis(float theta);
	glm::mat4 rotateZAxis(float theta);
	float degreesToRadians(float theta_degrees);
	double angleBtwnVectors(glm::vec3 a, glm::vec3 b);
	glm::quat RotationBtwnVectors(glm::vec3 start, glm::vec3 dest);
	glm::vec3 lerp(glm::vec3 p0, glm::vec3 p1, float t);
	glm::vec3 ceilVec3(const glm::vec3& v);
	glm::vec3 max(const glm::vec3& v, const glm::vec3& max);
	glm::vec3 min(const glm::vec3& v, const glm::vec3& min);

	glm::ivec3 max(const glm::ivec3& v, const glm::ivec3& max);
	glm::ivec3 min(const glm::ivec3& v, const glm::ivec3& max);

	// Time library
	void limitRefreshRate(int microsecondsPerFrame, std::chrono::time_point<std::chrono::steady_clock>& timer);
	double getSecondsPerFrame(std::chrono::time_point<std::chrono::steady_clock>& startTime);
	void printSecondsPerFrame(std::chrono::time_point<std::chrono::steady_clock>& startTime);

	void printIvec3(const glm::ivec3& v);
	void printVec3(const glm::vec3& v);	
	void printVec4(const glm::vec4& v);
	void printMat4(const glm::mat4& m);
};