#pragma once
#include "Graph2D.h"
#include "FileInputData.h"

class OffsetVolume: public Graph2D
{
public:
	OffsetVolume() {
		initGraph2D();	// load default dimensions
	}
	//template<typename T>
	//void generateLinearTF(int maxTexSize, std::vector<T>& out_texture1D) {
	//	std::vector<glm::vec4> interpolatedValues;
	//	interpolationLinear(maxTexSize, interpolatedValues);
	//}

	//void generateLinearTF(int maxTexSize, std::vector<unsigned char>& out_texture1DAO) {
	//	std::vector<glm::vec4> interpolatedValues;
	//	interpolationLinear(maxTexSize, interpolatedValues);

	//	int i = 0;
	//	int j = 0;
	//	out_texture1DAO.resize(interpolatedValues.size(), 0);
	//	float a = sizeof(unsigned char);
	//	while (i < interpolatedValues.size()) {
	//		out_texture1DAO[i] = interpolatedValues[i].a * 255.f;
	//		i++;
	//	}
	//}

	//void importData(const Assign2::InputLight& lightSettings) {

	//	//Insert points from config
	//	std::vector<ControlPoint> cpList;
	//	for (int i = 0; i < lightSettings.linearIntensityTF.size(); i++) {
	//		float r = lightSettings.linearIntensityTF[i].rgba[0] / 255.f;
	//		float g = lightSettings.linearIntensityTF[i].rgba[1] / 255.f;
	//		float b = lightSettings.linearIntensityTF[i].rgba[2] / 255.f;

	//		float x = lightSettings.linearIntensityTF[i].point;
	//		float y = lightSettings.linearIntensityTF[i].rgba[3] / 255.f;

	//		ControlPoint cp(r, g, b, x, y);
	//		boundCPToWindow(cp);
	//		cpList.push_back(cp);
	//	}

	//	controlPoints.swap(cpList);
	//	sort();
	//}


	//void exportData(Assign2::InputLight& lightSettings) {
	//	lightSettings.linearIntensityTF.clear();
	//	for (int i = 0; i < controlPoints.size(); i++)
	//	{
	//		Assign2::ColorPoint1D intensityPoint;
	//		intensityPoint.point = controlPoints[i].values.x;
	//		intensityPoint.rgba[0] = controlPoints[i].color.r * 255.f;
	//		intensityPoint.rgba[1] = controlPoints[i].color.g * 255.f;
	//		intensityPoint.rgba[2] = controlPoints[i].color.b * 255.f;
	//		intensityPoint.rgba[3] = controlPoints[i].values.y * 255.f;
	//		lightSettings.linearIntensityTF.push_back(intensityPoint);
	//	}
	//}
};

