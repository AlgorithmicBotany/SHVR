#pragma once

#include "volumes.h"

struct VolumeFill {
	VolumeBase* volumeData;
	std::vector<glm::ivec3> seeds;
	glm::ivec3 idxMin, idxMax;
	float inValue;

	virtual inline bool isVisited(glm::ivec3& idx) { return false; }
	virtual inline bool updateMaskValue(const glm::ivec3& idx) { return false; }

	void updateParameters(const glm::ivec3& indexMin,
		const glm::ivec3& indexMax,
		float minimumFillThreshold);

	void updateSeeds(const std::vector<glm::ivec3>& seedIndexes);

	bool fill();
	bool fillIdx(glm::ivec3 idx);
	bool floodFill_scanline();
	bool floodFill_sixWay();

	struct SpeckleSeed;

	void findSpeckles(std::vector<glm::ivec3>& out_seedPoints);
	bool fill_speckles();
};
