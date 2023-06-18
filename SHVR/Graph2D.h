#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <algorithm>	//sort, upper_bound, lower_bound

struct ControlPoint {
	glm::vec2 values;
	glm::vec3 color;	

	ControlPoint(float r, float g, float b, float xValue, float yValue) {
		color = glm::vec3(r, g, b);
		values = glm::vec2(xValue, yValue);
	}
};

class Graph2D
{

public:
	std::vector<ControlPoint> controlPoints;	
	std::vector<unsigned int> buckets;	//Histogram buckets

	float xGraphMin, xGraphMax;
	float yGraphMin, yGraphMax;
	float xValueMin, xValueMax;
	float yValueMin, yValueMax;
	float bucketStepSize;
	size_t histMax;
	float xValueOffset;	//***UNIMPLEMENTED

	Graph2D();
	~Graph2D();
	void initGraph2D();
	void clear();

	void updateValueMinMaxX(float xMin, float xMax);
	void updateValueMinMaxY(float yMin, float yMax);	
	void updateGraphDimensions(float xMin, float xMax, float yMin, float yMax);
		
	// Transformations
	glm::vec2 getValueFromGraphPos(const glm::vec2& graphPos);
	glm::vec2 getGraphPosFromValue(const glm::vec2& values);
	
	// Histogram
	void updateHistogramSize();
	void updateHistogram(const std::vector<unsigned char>& dataValues);
	void updateHistogram(const std::vector<unsigned short>& dataValues);

	// Interpolation schemes
	void interpolationLinear(int maxTexSize, std::vector<glm::vec4>& out_blendGraph);
	//void bezierInterp();
	//void bSplineInterp();

	// Operations
	float getMinimumVisibleValue();
	void boundCPToWindow(ControlPoint& cp);
	void boundCPToIdx(unsigned int idx);
	void sort();
	int findCPIdx(const ControlPoint& cp);
	int insertCP(const ControlPoint& cp);
	void removeCP(ControlPoint& cp);
	void removeCP(unsigned int idxCP);
	int updateCPIdx(ControlPoint& cp);
	void shiftPointsX(float xValueShift);
};
