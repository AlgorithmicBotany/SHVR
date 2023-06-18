#include "mathLib.h"

void MathLib::makeOffsets(float radius, int numberOfOffsets, std::vector<glm::vec3>& out_offsets)
{
	std::mt19937 rng(1729);
	std::uniform_real_distribution<> dist(0., 1.);

	out_offsets.resize(numberOfOffsets);

	generate(begin(out_offsets), end(out_offsets), [&]()
		{
			float u0 = dist(rng);
			float u1 = dist(rng);
			float u2 = dist(rng);

			float r = radius * std::cbrtf(u0);
			float theta = std::acos(2.f * u1 - 1.f);
			float phi = 360.f * u2 - 180.f;

			float x = r * sin(theta) * cos(phi);
			float y = r * sin(theta) * sin(phi);
			float z = r * cos(theta);

			return glm::vec3(x, y, z);
		}
	);
}

#include <iostream>
#include <fstream>

void saveAsCSV(const std::vector<glm::vec3>& vList) {
	std::ofstream myfile;
	myfile.open("example.txt");
	for (const glm::vec3& v : vList) {
		myfile << v.x << "," << v.y << "," << v.z << "\n";
	}	
	//myfile << "Writing this to a file.\n";
	myfile.close();
}

//void MathLib::makeOffsets(float radius, int numberOfOffsets, std::vector<glm::vec3>& out_offsets)
//{
//	out_offsets.push_back(glm::vec3(0, 0, 0));
//
//	radius = 1;
//	int size_i = 4;
//	int size_j = 18;
//	int size_k = 18;
//
//	for (int k = 0; k <= size_k; k++) {
//		for (int j = 0; j <= size_j; j++) {
//			for (int i = 1; i <= size_i; i++) {
//
//				float r = radius * (float)i / (float)size_i;
//				float theta = 2.f * M_PI * (float)j / (float)size_j;
//				float phi = 2.f * M_PI * (float)k / (float)size_k;
//
//				float x = r * sin(theta) * cos(phi);
//				float y = r * sin(theta) * sin(phi);
//				float z = r * cos(theta);			
//				out_offsets.push_back(glm::vec3(x, y, z));
//			}
//		}
//	}
//
//	glm::vec3 mean(0, 0, 0);
//	for (int i = 0; i < out_offsets.size(); i++) {
//		mean += out_offsets[i];
//	}
//	mean /= (float)out_offsets.size();
//}

glm::mat4 MathLib::translateM(const glm::vec3& v) {
	return translateM(v.x, v.y, v.z);
}

glm::mat4 MathLib::translateM(float x, float y, float z) {
	return glm::mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1);
}

glm::mat4 MathLib::rotateXAxis(float theta) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);

	return glm::mat4(
		1, 0, 0, 0,
		0, cosTheta, -sinTheta, 0,
		0, sinTheta, cosTheta, 0,
		0, 0, 0, 1);
}

glm::mat4 MathLib::rotateYAxis(float theta) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);

	return glm::mat4(
		cosTheta, 0, sinTheta, 0,
		0, 1, 0, 0,
		-sinTheta, 0, cosTheta, 0,
		0, 0, 0, 1);
}

glm::mat4 MathLib::rotateZAxis(float theta) {
	float cosTheta = cos(theta);
	float sinTheta = sin(theta);

	return glm::mat4(
		cosTheta, -sinTheta, 0, 0,
		sinTheta, cosTheta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}


float MathLib::degreesToRadians(float theta_degrees) {
	return (theta_degrees / 180.f) * M_PI;
}


double MathLib::angleBtwnVectors(glm::vec3 a, glm::vec3 b) {
	double theta = std::acos(glm::dot(a, b) / (glm::length(a) * glm::length(b)));
	return theta;
}

glm::quat MathLib::RotationBtwnVectors(glm::vec3 start, glm::vec3 dest) {
	//code modified from:http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis, so guess one;
		//any will do as long as it's perpendicular to start
		cosTheta = 0;
	}

	rotationAxis = glm::cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
		);
}

glm::vec3 MathLib::lerp(glm::vec3 p0, glm::vec3 p1, float t) {
	return (1 - t) * p0 + t * p1;
}

glm::vec3 MathLib::ceilVec3(const glm::vec3& v) {
	return glm::vec3(std::ceilf(v.x), std::ceilf(v.y), std::ceilf(v.z));
}

glm::vec3 MathLib::max(const glm::vec3& v, const glm::vec3& max) {
	glm::vec3 out_v = v;
	if (v.x > max.x) { out_v.x = max.x; }
	if (v.y > max.y) { out_v.y = max.y; }
	if (v.z > max.z) { out_v.z = max.z; }
	return out_v;
}
glm::vec3 MathLib::min(const glm::vec3& v, const glm::vec3& min) {
	glm::vec3 out_v = v;
	if (v.x < min.x) { out_v.x = min.x; }
	if (v.y < min.y) { out_v.y = min.y; }
	if (v.z < min.z) { out_v.z = min.z; }
	return out_v;
}

glm::ivec3 MathLib::max(const glm::ivec3& v, const glm::ivec3& max) {
	glm::ivec3 out_v = v;
	if (v.x > max.x) { out_v.x = max.x; }
	if (v.y > max.y) { out_v.y = max.y; }
	if (v.z > max.z) { out_v.z = max.z; }
	return out_v;
}
glm::ivec3 MathLib::min(const glm::ivec3& v, const glm::ivec3& min) {
	glm::ivec3 out_v = v;
	if (v.x < min.x) { out_v.x = min.x; }
	if (v.y < min.y) { out_v.y = min.y; }
	if (v.z < min.z) { out_v.z = min.z; }
	return out_v;
}

void MathLib::limitRefreshRate(int microsecondsPerFrame, std::chrono::time_point<std::chrono::steady_clock>& timer) {
	// Get the operation time (i.e. time from when the timer was last updated)
	auto currTime = std::chrono::high_resolution_clock::now();
	int operationTime = std::chrono::duration_cast<std::chrono::microseconds>(currTime - timer).count();
	
	// Take difference between desired frame time, and operation time
	int remainingTime = std::max(microsecondsPerFrame - operationTime, 0);

	//Sleep for the remaining length of time
	std::this_thread::sleep_for(std::chrono::microseconds((int)remainingTime));

	//Start timer again for next render cycle
	timer = std::chrono::high_resolution_clock::now();
}

double MathLib::getSecondsPerFrame(std::chrono::time_point<std::chrono::steady_clock>& startTime) {	
	auto currTime = std::chrono::high_resolution_clock::now();
	double operationTime = std::chrono::duration_cast<std::chrono::microseconds>(currTime - startTime).count();
	return operationTime / 1000000;		// seconds	
}

void MathLib::printSecondsPerFrame(std::chrono::time_point<std::chrono::steady_clock>& startTime) {
	qDebug() << "Seconds: " << getSecondsPerFrame(startTime);
}

void MathLib::printIvec3(const glm::ivec3& v) {
	qDebug() << v[0] << " " << v[1] << " " << v[2];
}

void MathLib::printVec3(const glm::vec3& v) {
	qDebug() << v[0] << " " << v[1] << " " << v[2];
}
void MathLib::printVec4(const glm::vec4& v) {
	qDebug() << v[0] << " " << v[1] << " " << v[2] << " " << v[3];
}
void MathLib::printMat4(const glm::mat4& m) {
	for (int i = 0; i < 4; i++) {
		qDebug() << m[i][0] << " " << m[i][1] << " " << m[i][2] << " " << m[i][3];
	}
}



