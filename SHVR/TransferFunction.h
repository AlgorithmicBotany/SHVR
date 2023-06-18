#pragma once

#include <vector>
#include "FileInputData.h"
#include "Graph2D.h"

class TransferFunction: public Graph2D {

public:
	float dataTypeRange;

	TransferFunction();
	float getVolumeInValue(float isoValue);
	float getOffsetInValue(float isoValue);
	void generateLinearTF(int maxTexSize, std::vector<unsigned char>& out_texture1DTF);
	void generateHistogram(const std::vector<unsigned char>& RAWData);
	void generateHistogram(const std::vector<unsigned short>& RAWData);
	void updateTFRange(const std::vector<unsigned char>& RAWData, const Assign2::InputTransferFunction& tfFile);
	void updateTFRange(const std::vector<unsigned short>& RAWData, const Assign2::InputTransferFunction& tfFile);
	void importData(const Assign2::InputTransferFunction& tf_setting);
	void exportData(Assign2::InputTransferFunction& tf_settings);
};
