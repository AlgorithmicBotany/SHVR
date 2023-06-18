#pragma once
#include "Graph2D.h"
#include "FileInputData.h"
struct AmbientOcclusion : public Graph2D
{
	AmbientOcclusion();
	void generateLinearTF(int maxTexSize, std::vector<unsigned char>& out_texture1DTF);
	void importData(const Assign2::InputLight& lightSettings);
	void exportData(Assign2::InputLight& lightSettings);
};
