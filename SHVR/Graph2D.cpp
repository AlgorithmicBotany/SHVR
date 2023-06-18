#include "Graph2D.h"

Graph2D::Graph2D() {
	initGraph2D();
}
Graph2D::~Graph2D() { histMax = 0; }

void Graph2D::initGraph2D() {
	updateValueMinMaxX(0, 1);
	updateValueMinMaxY(0, 1);
	updateGraphDimensions(-1, 1, -1, 1);
	bucketStepSize = 1;
	xValueOffset = 0;
}

void Graph2D::clear() {
	controlPoints.clear();
	buckets.clear();
	histMax = 0;
}

void Graph2D::updateValueMinMaxX(float xMin, float xMax) {
	xValueMin = xMin;
	xValueMax = xMax;
}
void Graph2D::updateValueMinMaxY(float yMin, float yMax) {
	yValueMin = yMin;
	yValueMax = yMax;
}

void Graph2D::updateGraphDimensions(float xMin, float xMax, float yMin, float yMax) {
	xGraphMin = xMin;
	xGraphMax = xMax;
	yGraphMin = yMin;
	yGraphMax = yMax;
}

// Transformations
glm::vec2 Graph2D::getValueFromGraphPos(const glm::vec2& graphPos) {
	// From [-1,1]^2  to [valueMin, valueMax]^2
	float xValueRange = xValueMax - xValueMin;
	float yValueRange = yValueMax - yValueMin;
	float xGraphRange = xGraphMax - xGraphMin;
	float yGraphRange = yGraphMax - yGraphMin;

	float xValue = (graphPos.x - xGraphMin) / xGraphRange * xValueRange + xValueMin;
	float yValue = (graphPos.y - yGraphMin) / yGraphRange * yValueRange + yValueMin;
	return glm::vec2(xValue, yValue);
}

glm::vec2 Graph2D::getGraphPosFromValue(const glm::vec2& values) {
	// From [valueMin, valueMax]^2 to [-1,1]^2 
	float xValueRange = xValueMax - xValueMin;
	float yValueRange = yValueMax - yValueMin;
	float xGraphRange = xGraphMax - xGraphMin;
	float yGraphRange = yGraphMax - yGraphMin;

	float xGraphPos = (values.x - xValueMin) / xValueRange * xGraphRange + xGraphMin;
	float yGraphPos = (values.y - yValueMin) / yValueRange * yGraphRange + yGraphMin;
	return glm::vec2(xGraphPos, yGraphPos);
}

// Histogram
void Graph2D::updateHistogramSize() {
	// Resize histogram buckets
	float xValueRange = xValueMax - xValueMin;
	float bucketScale = 1.f / bucketStepSize;
	unsigned int bucketSize = xValueRange * bucketScale;
	buckets.resize(bucketSize, 0);
}

template <typename T>
void updateHistogramTemplate(const std::vector<T>& dataValues, float xValueMin, 	
	float bucketStepSize, std::vector<unsigned int>& buckets) {

	// Add values to buckets of histogram
	float bucketScale = 1.f / bucketStepSize;
	for (const T& v : dataValues) {
		//Add values to histogram
		unsigned int idx = (v - xValueMin) * bucketScale;
		if (idx < buckets.size()) {
			buckets[idx]++;
		}
	}
}

void Graph2D::updateHistogram(const std::vector<unsigned char>& dataValues) {
	updateHistogramTemplate(dataValues, xValueMin, bucketStepSize, buckets);
	// Update maximum value of histogram
	histMax = *std::max_element(buckets.begin(), buckets.end());
}
void Graph2D::updateHistogram(const std::vector<unsigned short>& dataValues) {
	updateHistogramTemplate(dataValues, xValueMin, bucketStepSize, buckets);
	// Update maximum value of histogram
	histMax = *std::max_element(buckets.begin(), buckets.end());
}

// Interpolation schemes
void Graph2D::interpolationLinear(int maxTexSize, std::vector<glm::vec4>& out_blendGraph) {
	out_blendGraph.resize(maxTexSize, glm::vec4(0, 0, 0, 0));
	float xValueRange = xValueMax - xValueMin;
	int size = controlPoints.size();
	for (int i = 0; i < size - 1; ++i)
	{
		//Get the range between two TF points
		auto const& a = controlPoints[i];
		auto const& b = controlPoints[i + 1];

		float percentA = (a.values.x - xValueMin) / xValueRange;
		float percentB = (b.values.x - xValueMin) / xValueRange;

		percentA = glm::clamp(percentA, 0.f, 1.f);
		percentB = glm::clamp(percentB, 0.f, 1.f);

		int ia = static_cast<int>(percentA * (maxTexSize - 1));
		int ib = static_cast<int>(percentB * (maxTexSize - 1));

		int interpCount = ib - ia;

		//Linearly blend colour and alpha for all in-between values
		for (int j = 0; j < interpCount; ++j) {
			float t = static_cast<float>(j) / (float)interpCount;

			float blendR = (1. - t) * a.color.r + (t)*b.color.r;
			float blendG = (1. - t) * a.color.g + (t)*b.color.g;
			float blendB = (1. - t) * a.color.b + (t)*b.color.b;
			float blendA = (1. - t) * a.values.y + (t)*b.values.y;
			out_blendGraph[ia + j] = glm::vec4(blendR, blendG, blendB, blendA);
		}
	}
	if (size > 0) {
		// Set the end of interpolation with value
		auto const& b = controlPoints.back();
		float percentB = (b.values.x - xValueMin) / xValueRange;
		percentB = glm::clamp(percentB, 0.f, 1.f);
		int ib = static_cast<int>(percentB * (maxTexSize - 1));
		out_blendGraph[ib] = glm::vec4(b.color.r, b.color.g, b.color.b, b.values.y);
	}
}

// Operations
float Graph2D::getMinimumVisibleValue() {
	int size = controlPoints.size() - 1;
	for (int i = 0; i < size; i++) {
		ControlPoint& cp_next = controlPoints[i + 1];
		if (cp_next.values.y > 0) {
			ControlPoint& cp = controlPoints[i];
			return cp.values.x;
		}
	}
	return 0;
}

void Graph2D::boundCPToWindow(ControlPoint& cp) {
	cp.values.x = glm::clamp(cp.values.x, xValueMin, xValueMax);
	cp.values.y = glm::clamp(cp.values.y, yValueMin, yValueMax);
}

void Graph2D::boundCPToIdx(unsigned int idx) {
	float xMin, xMax;
	idx < controlPoints.size() - 1 ? 
		xMax = controlPoints[idx + 1].values.x : xMax = xValueMax;
	idx > 0 ?
		xMin = controlPoints[idx - 1].values.x : xMin = xValueMin;
	
	ControlPoint& cp = controlPoints[idx];
	cp.values.x = glm::clamp(cp.values.x, xMin, xMax);
	cp.values.y = glm::clamp(cp.values.y, yValueMin, yValueMax);
}

void Graph2D::sort() {
	std::sort(controlPoints.begin(),
		controlPoints.end(),
		[](const ControlPoint& a, const ControlPoint& b)
			{return a.values.x < b.values.x;});
}
int Graph2D::findCPIdx(const ControlPoint& cp) {
	for (int i = 0; i < controlPoints.size(); i++) {
		if (&controlPoints[i] == &cp) {
			return i;
		}
	}
	return -1;
}
int Graph2D::insertCP(const ControlPoint& cp) {
	// Insert new control point in sorted order		
	auto upperCP = std::upper_bound(
		controlPoints.begin(),		//iter start
		controlPoints.end(),		//iter end
		cp,							//element to be inserted
		[](const ControlPoint& a,
			const ControlPoint& b)
		{ return a.values.x < b.values.x; });

	int insertIdx = upperCP - controlPoints.begin();
	controlPoints.insert(upperCP, cp);
	return insertIdx;
}
void Graph2D::removeCP(ControlPoint& cp) {
	// remove control point from list
	int idx = findCPIdx(cp);
	if (idx >= 0) { removeCP(idx);}
}

void Graph2D::removeCP(unsigned int idxCP) {
	// remove control point from list
	auto iterIdx = controlPoints.begin() + idxCP;
	controlPoints.erase(iterIdx);
}

int Graph2D::updateCPIdx(ControlPoint& cp) {
	// update control point by removing, and then inserting to correct position
	ControlPoint cpReplace = cp;
	removeCP(cp);
	return insertCP(cpReplace);
}

void Graph2D::shiftPointsX(float xValueShift) {
	// Shift all points, bound to edge of graph
	for (ControlPoint& cp : controlPoints) {
		cp.values.x += xValueShift;
		cp.values.x = glm::clamp(cp.values.x, xValueMin, xValueMax);
	}
}
