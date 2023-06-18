#pragma once

#include <qdebug.h>
#include "VolumeTemplate.h"
#include "ParameterStructs.h"
#include "mathLib.h"

#include <set>
#include <queue>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

struct VolumeMask : public VolumeBase_Template<unsigned char> {	
	VolumeMask(int samplesWide, int samplesHigh, int samplesDeep,
		float wSpacing, float hSpacing, float dSpacing) :
		VolumeBase_Template<unsigned char>(samplesWide, samplesHigh, samplesDeep,
			wSpacing, hSpacing, dSpacing) {
	}

	void import(const std::vector<unsigned char>& maskValues) {
		std::copy(maskValues.begin(), maskValues.end(), d.begin());
	}
};

struct VolumeState {
	std::vector<bool> paintOverGroups;	// enable overwriting exisiting paint
	bool keepExistingOffset;	// Disable overwriting exisiting offset

	VolumeBase* vData;
	VolumeBase* vOffset;
	VolumeMask* vMask;

	glm::ivec3 idxMin;
	glm::ivec3 idxMax;

	std::vector<glm::ivec3> affectedPaintTotal;	//painted voxels from when the button was pressed, up to when the button is released
	std::vector<char> affectedPaintDiff;

	std::vector<glm::ivec3> affectedOffset;		//idx of adjusted intensity of voxels in this time step
	std::vector<float> affectedOffsetDiff;	//adjusted intensity of voxels (Note: needed to have overlapping difference values)
		
	VolumeState(VolumeBase* _vData, VolumeMask* _vMask, VolumeBase* _vOffset);

	void clearMinMax();
	void updateMinMax(const glm::ivec3& idxMin, const glm::ivec3& idxMax);
	float getVoxelOffset(const glm::ivec3& idx);
	float getDist2Explicit(const glm::ivec3& idx0, const glm::ivec3& idx1, float maxDist);
	void applyVoxelOffset(float offset, const glm::ivec3& idx);
	void removeVoxelOffset(const glm::ivec3& idx);
	void applyVolumeOffset();
	void clearVolumeOffset();
	float getUpdatedOffsetValue(float offset, float maxOffset, float prevOffset, const glm::ivec3& idx);
	void updateOffsetValue(float offset, float maxOffset, const glm::ivec3& idx);
	void updateMaskValue(int value, const glm::ivec3& idx);
	float getNeighbouringMaxOffset(const glm::ivec3& idx);
	void updateNeighbouringVoxelOffsets(int i, int j, int k, float paintMaxOffset, Math::Array3D<char>& modifiedVoxels);
	void updateNeighbouringVoxelsOffsets(float paintMaxOffset, float minVisibleInValue, Math::Array3D<float>* subvolume);
	void updateNeighbouringVoxelPaint(int i, int j, int k, float paintValue, Math::Array3D<char>& modifiedVoxels);
	void updateNeighbouringVoxelsPaint(float paintValue, Math::Array3D<char>* subvolume);
};


//Note: Feels very close to Array3D
template <typename T>
struct VaryingVolumeTemplate : std::vector<T> {
	VaryingVolumeTemplate() {
		idxOrigin = glm::ivec3(0, 0, 0);
		volumeDimensions = glm::ivec3(0, 0, 0);
	}

	VaryingVolumeTemplate(glm::ivec3 origin, glm::ivec3 dimensions) {
		idxOrigin = origin;
		volumeDimensions = dimensions;
	}

	size_t getIterIndex(const glm::ivec3& idxVolume) {
		glm::ivec3 idx = idxVolume - idxOrigin;
		return idx.x + idx.y * volumeDimensions.x + idx.z * volumeDimensions.x * volumeDimensions.y;
	}

	void updateSize(const glm::ivec3& indexOrigin, const glm::ivec3& sideLengths) {
		idxOrigin = indexOrigin;
		volumeDimensions = sideLengths;
		resize(volumeDimensions.x * volumeDimensions.y * volumeDimensions.z);
	}

	bool isValid(const glm::ivec3& idxVolume) {
		glm::ivec3 idxDiff = idxVolume - idxOrigin;
		return (idxDiff.x >= 0 && idxDiff.x < volumeDimensions.x&&
			idxDiff.y >= 0 && idxDiff.y < volumeDimensions.y&&
			idxDiff.z >= 0 && idxDiff.z < volumeDimensions.z);
	}

	glm::ivec3 dimensions() {
		return volumeDimensions;
	}

	glm::ivec3 origin() {
		return idxOrigin;
	}

	void setOrigin(glm::ivec3 origin) {
		idxOrigin = origin;
	}

private:
	glm::ivec3 idxOrigin;
	glm::ivec3 volumeDimensions;
};

struct VaryingVector {
	//Abstract struct
	virtual void resize(int size) {}
	virtual void setValueAtIndex(float value, int idx) {}
	virtual void* data() { return 0; }
	virtual void push_back(float value) {}
	virtual float at(int idx) { return -1; }	//Note: Can't have varying type
};

template <typename T>
struct VaryingVectorTemplate : public VaryingVector {
	std::vector<T> elementList;
	VaryingVectorTemplate<T>() {}
	void resize(int size) override{
		elementList.resize(size);
	}
	virtual void setValueAtIndex(float value, int idx) override {
		elementList[idx] = (T)value;
	}
	void* data() override {
		return (void*)elementList.data();
	}
	void push_back(float value) override {
		elementList.push_back((T)value);
	}
	virtual float at(int idx) { return elementList[idx]; }
};

struct MaskUpdateInfo {
	glm::ivec3 idxMin;
	glm::ivec3 idxMax;
	bool queueTextureMaskUpdate;	// queue mask texture update
	std::vector<unsigned char> maskSubsection;
	VaryingVector* volumeSubsection;

	MaskUpdateInfo(Assign2::DataInput dataType) {
		idxMin = glm::ivec3(0, 0, 0);
		idxMax = glm::ivec3(0, 0, 0);		
		initVolumeSubsection(dataType);
		queueTextureMaskUpdate = false;
	}

	~MaskUpdateInfo() {
		if (volumeSubsection != 0) { delete volumeSubsection; }
	}

	void initVolumeSubsection(Assign2::DataInput dataType) {
		if (dataType == Assign2::DataInput::UINT8) {
			volumeSubsection = new VaryingVectorTemplate<unsigned char>();
		}
		else if (dataType == Assign2::DataInput::UINT16) {
			volumeSubsection = new VaryingVectorTemplate<unsigned short>();
		}
		else {
			volumeSubsection = new VaryingVectorTemplate<float>();
		}		
	}

	void clampMinMax(VolumeMask* vMask) {
		glm::ivec3 minCoord = glm::ivec3(0, 0, 0);
		glm::ivec3 maxCoord = glm::ivec3(vMask->width(), vMask->height(), vMask->depth());
		idxMin = glm::clamp(idxMin, minCoord, maxCoord);
		idxMax = glm::clamp(idxMax, minCoord, maxCoord);
	}

	void updateVolumeSubsection(VolumeBase* vData) {
		glm::ivec3 dim = idxMax - idxMin;
		volumeSubsection->resize(dim.x * dim.y * dim.z);
		unsigned int idx = 0;
		for (int k = idxMin.z; k < idxMax.z; k++) {
			for (int j = idxMin.y; j < idxMax.y; j++) {
				for (int i = idxMin.x; i < idxMax.x; i++) {
					glm::ivec3 cellIdx(i, j, k);
					volumeSubsection->setValueAtIndex(vData->valueAtCell(cellIdx), idx);
					idx++;
				}
			}
		}
	}

	void updateMaskSubsection(VolumeMask* vMask) {
		// Fill with a section of mask that has been updated
		// Note: Updating subsection of texture will read texture from start
		// to end of list, so a new list of values need to be created.

		glm::ivec3 dim = idxMax - idxMin;
		maskSubsection.resize(dim.x * dim.y * dim.z);
		unsigned int idx = 0;
		for (int k = idxMin.z; k < idxMax.z; k++) {
			for (int j = idxMin.y; j < idxMax.y; j++) {
				for (int i = idxMin.x; i < idxMax.x; i++) {
					maskSubsection[idx] = vMask->d.at(i, j, k);
					idx++;
				}
			}
		}
	}

	void queueUpdate(const glm::ivec3& _idxMin, const glm::ivec3& _idxMax, 
			VolumeMask* vMask, VolumeBase* vData) {
		if (queueTextureMaskUpdate == false) {
			idxMin = _idxMin - glm::ivec3(1, 1, 1);						
			idxMax = _idxMax + glm::ivec3(1, 1, 1);
			clampMinMax(vMask);
			updateMaskSubsection(vMask);
			updateVolumeSubsection(vData);
			queueTextureMaskUpdate = true;
		}
	}
};

struct Volume {
	MaskUpdateInfo* maskUpdateInfo;

	Volume();
	~Volume();
	void clearVolumes() {
		if (volumeData != 0) { 
			delete volumeData; 
		}
		if (volumeMask != 0) { delete volumeMask; }
		if (volumeOffset != 0) { delete volumeOffset; }
		if (volumeState != 0) { delete volumeState; }		
		volumeData = 0;
		volumeMask = 0;
		volumeState = 0;
		volumeOffset = 0;
	}

	//Initialization
	void importData(const Assign2::InputConfiguration& configFile);
	void initData(int width, int height, int depth, 
		float xSpacing, float ySpacing, float zSpacing, Assign2::DataInput dataType);
	void initMask();
	void initMask(int width, int height, int depth,
		float xSpacing, float ySpacing, float zSpacing);

	void initOffset(Assign2::DataInput dataType);
	void initOffset(int width, int height, int depth, 
		float xSpacing, float ySpacing, float zSpacing, Assign2::DataInput dataType);

	//Updates
	void updateM(glm::mat4 m);
	void updateScale(float s);
	void updateLongestLength();
	void updateTransform();

	glm::vec3 getWSfromVS(const glm::vec3& pointVS);
	glm::vec3 getWSfromVS(const glm::vec4& pointVS);
	glm::vec3 getVSfromWS(const glm::vec3& pointWS);
	glm::vec3 getVSfromWS(const glm::vec4& pointWS);
	glm::vec3 getDirVSfromWS(const glm::vec3& dirWS);
	glm::vec3 getDirVSfromWS(const glm::vec4& dirWS);
	glm::vec3 getTSfromVS(const glm::vec3& pointVS) {
		return pointVS / getBoxDimensions();
	}
	glm::vec3 getVSfromTS(const glm::vec3& pointTS) {
		return pointTS * getBoxDimensions();
	}
	glm::vec3 getSpacings();
	glm::vec3 getBoxDimensions();
	glm::vec3 getCellSize(float lengthWorld);
	glm::mat4 getTranslateM() {
		return translateM;
	};

	void translate(const glm::vec3& translateWS);
	void rotate(const glm::vec3& rotateAxis, float angleRadians);
	//Helpers
	VolumeBase* vData();
	VolumeMask* vMask();
	VolumeState* vState();

	const glm::mat4& M();
	const glm::mat4& invM();
	
	float longestLength();

	VolumeBase* volumeCopy;
private:
	glm::mat4 scaleM;
	glm::mat4 modelM;
	glm::mat4 invModelM;
	glm::mat4 translateM;		//Translate volume from corner to its center 	
	glm::mat4 invTranslateM;	//Translate volume from center to its corner
	
	VolumeBase* volumeData;
	VolumeBase* volumeOffset;
	VolumeMask* volumeMask;
	VolumeState* volumeState;

	

	float lengthLongest;
};

namespace VolumeIO
{
	void loadVolumeData(VolumeBase* vData, const std::vector<unsigned char>& RAWData, int densityMin, int densityMax, float dataTypeRange);
	void loadVolumeData(VolumeBase* vData, const std::vector<unsigned short>& RAWData, int densityMin, int densityMax, float dataTypeRange);
	void saveVolumeMask(VolumeMask* vMask, std::vector<unsigned char>& mask);
	void loadVolumeMask(VolumeMask* vMask, const std::vector<unsigned char>& mask);

	void loadVolume(VolumeBase* vOffset, const std::vector<short>& RAWData);
	void loadVolume(VolumeBase* vOffset, const std::vector<int>& RAWData);
	void saveVolume(VolumeBase* vOffset, std::vector<short>& RAWData);
	void saveVolume(VolumeBase* vOffset, std::vector<int>& RAWData);

	void saveVolume(VolumeBase* vOffset, std::vector<unsigned char>& RAWData);

	void saveVolume(VolumeBase* vOffset, std::vector<unsigned short>& RAWData);
};
