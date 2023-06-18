#include "TransferFunction.h"

TransferFunction::TransferFunction() {
	initGraph2D();	// load default dimensions
	dataTypeRange = 0;
}

float TransferFunction::getVolumeInValue(float isoValue) {
	float densityRange = xValueMax - xValueMin;
	return (isoValue - xValueMin) / densityRange * dataTypeRange;
}

float TransferFunction::getOffsetInValue(float isoValue) {
	float densityRange = xValueMax - xValueMin;
	return isoValue/ densityRange * dataTypeRange;
}

void TransferFunction::generateLinearTF(int maxTexSize, std::vector<unsigned char>& out_texture1DTF) {
	std::vector<glm::vec4> interpolatedValues;
	interpolationLinear(maxTexSize, interpolatedValues);

	int i = 0;
	int j = 0;
	out_texture1DTF.resize(interpolatedValues.size() * 4, 0);
	while (i < interpolatedValues.size()) {
		out_texture1DTF[j] = interpolatedValues[i].r * 255.f;
		out_texture1DTF[j + 1] = interpolatedValues[i].g * 255.f;
		out_texture1DTF[j + 2] = interpolatedValues[i].b * 255.f;
		out_texture1DTF[j + 3] = interpolatedValues[i].a * 255.f;

		i++;
		j += 4;
	}
}

template <typename T>
std::vector<int> findMinMaxValues(const std::vector<T>& RAWData) {
	std::vector<int> list;
	auto minMaxTF = std::minmax_element(RAWData.begin(), RAWData.end());
	list.push_back(*minMaxTF.first);
	list.push_back(*minMaxTF.second);
	return list;
}

template <typename T>
std::vector<int> getMinMaxValues(const std::vector<T>& RAWData, const Assign2::InputTransferFunction& tfFile) {
	if (tfFile.minTF == tfFile.maxTF) {
		return findMinMaxValues(RAWData);
	}
	else {
		return { tfFile.minTF , tfFile.maxTF };
	}
}

void TransferFunction::generateHistogram(const std::vector<unsigned char>& RAWData) {	
	updateHistogramSize();
	updateHistogram(RAWData);
}

void TransferFunction::generateHistogram(const std::vector<unsigned short>& RAWData) {
	updateHistogramSize();
	updateHistogram(RAWData);
}

void TransferFunction::updateTFRange(const std::vector<unsigned char>& RAWData, const Assign2::InputTransferFunction& tfFile) {
	std::vector<int> minMax = getMinMaxValues(RAWData, tfFile);
	xValueMin = minMax[0];
	xValueMax = minMax[1];
	dataTypeRange = 255;
}

void TransferFunction::updateTFRange(const std::vector<unsigned short>& RAWData, const Assign2::InputTransferFunction& tfFile) {
	std::vector<int> minMax = getMinMaxValues(RAWData, tfFile);
	xValueMin = minMax[0];
	xValueMax = minMax[1];	
	dataTypeRange = 65535;
}

void TransferFunction::importData(const Assign2::InputTransferFunction& tf_setting) {
	//Note: minTF and maxTF aren't transferred because it might have been automatically obtained		
	xValueOffset = tf_setting.shiftTF;

	//Insert points from config
	std::vector<ControlPoint> cpList;
	for (int i = 0; i < tf_setting.linearIntensityTF.size(); i++) {
		float r = tf_setting.linearIntensityTF[i].rgba[0] / 255.f;
		float g = tf_setting.linearIntensityTF[i].rgba[1] / 255.f;
		float b = tf_setting.linearIntensityTF[i].rgba[2] / 255.f;

		float x = tf_setting.linearIntensityTF[i].point;
		float y = tf_setting.linearIntensityTF[i].rgba[3] / 255.f;

		ControlPoint cp(r, g, b, x, y);
		boundCPToWindow(cp);
		cpList.push_back(cp);
	}

	controlPoints.swap(cpList);
	sort();
}

void TransferFunction::exportData(Assign2::InputTransferFunction& tf_settings) {
	//Note: minTF and maxTF aren't transferred because it might have been automatically obtained	
	tf_settings.shiftTF = xValueOffset;

	//tf points	
	tf_settings.linearIntensityTF.clear();

	for (int i = 0; i < controlPoints.size(); i++)
	{
		Assign2::ColorPoint1D intensityPoint;
		intensityPoint.point = controlPoints[i].values.x;
		intensityPoint.rgba[0] = controlPoints[i].color.r * 255.f;
		intensityPoint.rgba[1] = controlPoints[i].color.g * 255.f;
		intensityPoint.rgba[2] = controlPoints[i].color.b * 255.f;
		intensityPoint.rgba[3] = controlPoints[i].values.y * 255.f;
		tf_settings.linearIntensityTF.push_back(intensityPoint);
	}
}