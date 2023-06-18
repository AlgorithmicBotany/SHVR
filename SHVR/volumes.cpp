#include "volumes.h"


VolumeState::VolumeState(VolumeBase* _vData, VolumeMask* _vMask, VolumeBase* _vOffset) :
	vData(_vData), vMask(_vMask), vOffset(_vOffset) {
	paintOverGroups.resize(10, true);
	keepExistingOffset = false;
	clearMinMax();
}

void VolumeState::clearMinMax() {
	idxMin = glm::ivec3(999999, 999999, 999999);
	idxMax = glm::ivec3(-1, -1, -1);
}

void VolumeState::updateMinMax(const glm::ivec3& _idxMin, const glm::ivec3& _idxMax) {
	idxMin = MathLib::max(idxMin, _idxMin);
	idxMax = MathLib::min(idxMax, _idxMax);
}

float VolumeState::getVoxelOffset(const glm::ivec3& idx) {
	return vOffset->valueAtCell(idx);
}

float VolumeState::getDist2Explicit(const glm::ivec3& idx0, const glm::ivec3& idx1, float maxDist) {
	//Get world distance between cell indices
	// Note: all indices must be valid
	glm::vec3 pos0, pos1;
	vData->getPositionExplicit(idx0, pos0);
	vData->getPositionExplicit(idx1, pos1);
	return glm::distance2(pos0, pos1) / maxDist;
}

void VolumeState::applyVoxelOffset(float offset, const glm::ivec3& idx) {
	vOffset->setValueAtCell(offset, idx);
	vData->applyVoxelIntensityOffset(offset, idx);
}

void VolumeState::removeVoxelOffset(const glm::ivec3& idx) {
	//? Should I also unset the voxel offset, even if I am going to change soon after?
	float offset = vOffset->valueAtCell(idx);
	vData->applyVoxelIntensityOffset(-offset, idx);
}


void VolumeState::applyVolumeOffset() {
	for (int k = 0; k < vData->depth(); k++) {
		for (int j = 0; j < vData->height(); j++) {
			for (int i = 0; i < vData->width(); i++) {
				glm::ivec3 idx(i, j, k);
				applyVoxelOffset(vOffset->valueAtCell(idx), idx);
			}
		}
	}
}

void VolumeState::clearVolumeOffset() {
	vOffset->setValue(0);
}

float VolumeState::getUpdatedOffsetValue(float offset, float maxOffset, float prevOffset, const glm::ivec3& idx) {
	float off = 0;
	float originalVoxelValue = vData->valueAtCell(idx) - prevOffset;

	//Set offsets between maximum group offset, and minimum of previous offset
	if (maxOffset > 0) {
		// Positive offset
		off = std::max(offset, prevOffset);

		// limit the offset to the limit of data type			
		float maxRange = vData->getRangeMax() - originalVoxelValue;
		off = std::min(off, maxOffset);
		off = std::min(off, maxRange);
	}
	else if (maxOffset < 0) {
		// Negative offset
		off = std::min(offset, prevOffset);

		// limit the offset to the negative of voxel value
		// (ie. can't be less than zero)
		float minRange = vData->getRangeMin() - originalVoxelValue;
		off = std::max(off, maxOffset);
		off = std::max(off, minRange);
	}
	else {
		// No offset
		off = 0;
	}

	return off;
}

void VolumeState::updateOffsetValue(float offset, float maxOffset, const glm::ivec3& idx) {
	float prevOffset = getVoxelOffset(idx);
	float off = getUpdatedOffsetValue(offset, maxOffset, prevOffset, idx);

	// Commit difference for undo
	float offsetDiff = std::floor(off - prevOffset);	// Prevents precision loss
	bool isChanged = offsetDiff != 0;
	bool offsetOn = !keepExistingOffset && isChanged;
	if (offsetOn) {
		removeVoxelOffset(idx);
		applyVoxelOffset(off, idx);

		affectedOffset.push_back(idx);
		affectedOffsetDiff.push_back(offsetDiff);
	}
}

void VolumeState::updateMaskValue(int value, const glm::ivec3& idx) {
	//// Paint if existing paint is not considered,
	//// existing paint is considered and the region is painted, or 
	//// the paint value is zero (ie. eraser)
	char paintValue = vMask->valueAtCell(idx);
	bool paintOver = paintOverGroups.at(paintValue);

	char maskDiff = value - paintValue;
	//if (maskDiff != 0 && paintOn) {		//(!) Turned off for now
	if(paintOver){
		vMask->sampleAt(idx) = value;
		affectedPaintTotal.push_back(idx);
		affectedPaintDiff.push_back(maskDiff);
	}
}

float VolumeState::getNeighbouringMaxOffset(const glm::ivec3 &idx) {
	float offset = 0;
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(0, 0, 0)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(0, 0, 1)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(0, 1, 0)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(0, 1, 1)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(1, 0, 0)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(1, 0, 1)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(1, 1, 0)));
	offset = std::max(offset, vOffset->valueAtCell(idx + glm::ivec3(1, 1, 1)));

	return offset;
}

void VolumeState::updateNeighbouringVoxelOffsets(int i, int j, int k, float paintMaxOffset, Math::Array3D<char>& modifiedVoxels) {
	// Something is changing values of existing offsets. I don't know what it could be.
	bool b0 = modifiedVoxels.at(i, j, k);
	bool b1 = modifiedVoxels.at(i, j, k + 1);
	bool b2 = modifiedVoxels.at(i, j + 1, k);
	bool b3 = modifiedVoxels.at(i, j + 1, k + 1);
	bool b4 = modifiedVoxels.at(i + 1, j, k);
	bool b5 = modifiedVoxels.at(i + 1, j, k + 1);
	bool b6 = modifiedVoxels.at(i + 1, j + 1, k);
	bool b7 = modifiedVoxels.at(i + 1, j + 1, k + 1);

	//Return true if there is a mixture of unmodified (== 0)  and modified (!= 0) voxels
	bool valueAND = b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7;
	bool valueOR = b0 || b1 || b2 || b3 || b4 || b5 || b6 || b7;
	if (valueAND == 0 && valueOR == 1) {
		glm::ivec3 idx = idxMin + glm::ivec3(i, j, k);
		float offset = getNeighbouringMaxOffset(idx) * 0.8f;
		// Update only voxels that are adjacent to updated voxels
		updateOffsetValue(offset, paintMaxOffset, idx);
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(0, 0, 1));
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(0, 1, 0));
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(0, 1, 1));
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(1, 0, 0));
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(1, 0, 1));
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(1, 1, 0));
		updateOffsetValue(offset, paintMaxOffset, idx + glm::ivec3(1, 1, 1));
	}
}

void VolumeState::updateNeighbouringVoxelsOffsets(float paintMaxOffset, float minVisibleInValue, Math::Array3D<float>* subvolume) {
	// Produce a volume
	Math::Array3D<char> modifiedVoxels;	//Note: bool doesn't allow retrieving data?
	modifiedVoxels.resize(subvolume->width(), subvolume->height(), subvolume->depth(), false);

	// Mark voxels that have been updated from painting
	for (int k = 0; k < subvolume->depth(); k++) {
		for (int j = 0; j < subvolume->height(); j++) {
			for (int i = 0; i < subvolume->width(); i++) {
				glm::ivec3 idx = idxMin + glm::ivec3(i, j, k);
				modifiedVoxels.at(i, j, k) =
					(subvolume->at(i, j, k) != 0) &&				//Updated from last painting session
					(vData->valueAtCell(idx) > minVisibleInValue);	//Voxel with offset is inside the structure
			}
		}
	}

	//Update neighbouring voxels
	for (int k = 0; k < subvolume->depth() - 1; k++) {
		for (int j = 0; j < subvolume->height() - 1; j++) {
			for (int i = 0; i < subvolume->width() - 1; i++) {
				updateNeighbouringVoxelOffsets(i, j, k, paintMaxOffset, modifiedVoxels);
			}
		}
	}
}

void VolumeState::updateNeighbouringVoxelPaint(int i, int j, int k, float paintValue, Math::Array3D<char>& modifiedVoxels) {
	// Something is changing values of existing offsets. I don't know what it could be.
	bool b0 = modifiedVoxels.at(i, j, k);
	bool b1 = modifiedVoxels.at(i, j, k + 1);
	bool b2 = modifiedVoxels.at(i, j + 1, k);
	bool b3 = modifiedVoxels.at(i, j + 1, k + 1);
	bool b4 = modifiedVoxels.at(i + 1, j, k);
	bool b5 = modifiedVoxels.at(i + 1, j, k + 1);
	bool b6 = modifiedVoxels.at(i + 1, j + 1, k);
	bool b7 = modifiedVoxels.at(i + 1, j + 1, k + 1);

	//Return true if there is a mixture of unmodified (== 0)  and modified (!= 0) voxels
	bool valueAND = b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7;
	bool valueOR = b0 || b1 || b2 || b3 || b4 || b5 || b6 || b7;
	if (valueAND == 0 && valueOR == 1) {
		glm::ivec3 idx = idxMin + glm::ivec3(i, j, k);
		updateMaskValue(paintValue, idx);
		updateMaskValue(paintValue, idx + glm::ivec3(0, 0, 1));
		updateMaskValue(paintValue, idx + glm::ivec3(0, 1, 0));
		updateMaskValue(paintValue, idx + glm::ivec3(0, 1, 1));
		updateMaskValue(paintValue, idx + glm::ivec3(1, 0, 0));
		updateMaskValue(paintValue, idx + glm::ivec3(1, 0, 1));
		updateMaskValue(paintValue, idx + glm::ivec3(1, 1, 0));
		updateMaskValue(paintValue, idx + glm::ivec3(1, 1, 1));
	}
}

void VolumeState::updateNeighbouringVoxelsPaint(float paintValue, Math::Array3D<char>* subvolume) {
	// Produce a volume
	Math::Array3D<char> modifiedVoxels;	//Note: bool doesn't allow retrieving data
	modifiedVoxels.resize(subvolume->width(), subvolume->height(), subvolume->depth(), false);

	for (int k = 0; k < subvolume->depth(); k++) {
		for (int j = 0; j < subvolume->height(); j++) {
			for (int i = 0; i < subvolume->width(); i++) {
				glm::ivec3 idx = idxMin + glm::ivec3(i, j, k);
				modifiedVoxels.at(i, j, k) =
					(subvolume->at(i, j, k) != 0);				//Updated from last painting session
			}
		}
	}

	//Update neighbouring voxels	
	for (int k = 0; k < subvolume->depth() - 1; k++) {
		for (int j = 0; j < subvolume->height() - 1; j++) {
			for (int i = 0; i < subvolume->width() - 1; i++) {
				updateNeighbouringVoxelPaint(i, j, k, paintValue, modifiedVoxels);
			}
		}
	}
}


Volume::Volume() {
	volumeData = 0; volumeMask = 0; volumeState = 0;
	volumeOffset = 0; maskUpdateInfo = 0;
}

Volume::~Volume() {
	clearVolumes();
	if (maskUpdateInfo != 0) { delete maskUpdateInfo; }
	maskUpdateInfo = 0;
}

//Initialization
void Volume::importData(const Assign2::InputConfiguration& configFile) {
	//Volume
	int width = configFile.dimensions[0];
	int height = configFile.dimensions[1];
	int depth = configFile.dimensions[2];

	float xSpacing = configFile.spacings[0] * configFile.sceneScale;
	float ySpacing = configFile.spacings[1] * configFile.sceneScale;
	float zSpacing = configFile.spacings[2] * configFile.sceneScale;

	initData(width, height, depth, xSpacing, ySpacing, zSpacing, configFile.inputType);

	//Mask
	if (configFile.maskEnabled) { 
		initMask(); 
		initOffset(configFile.inputType);
		volumeState = new VolumeState(volumeData, volumeMask, volumeOffset);
		maskUpdateInfo = new MaskUpdateInfo(configFile.inputType);
	}
	else { initMask(0, 0, 0, 0, 0, 0); }
}

void Volume::initData(int width, int height, int depth,
	float xSpacing, float ySpacing, float zSpacing, Assign2::DataInput dataType) {
	if (dataType == Assign2::DataInput::UINT8) {
		volumeData = new VolumeBase_Template<unsigned char>(width, height, depth,
			xSpacing, ySpacing, zSpacing);
		volumeData->updateRange(0, 255);
	}
	else if (dataType == Assign2::DataInput::UINT16) { 
		volumeData = new VolumeBase_Template<unsigned short>(width, height, depth,
			xSpacing, ySpacing, zSpacing);
		volumeData->updateRange(0, 65535);
	}
	else {
		volumeData = new VolumeBase_Template<float>(width, height, depth,
			xSpacing, ySpacing, zSpacing);
		volumeData->updateRange(0, 0);
	}

	updateLongestLength();
	updateTransform();
}

void Volume::initMask() {
	initMask(volumeData->width(), volumeData->height(), volumeData->depth(),
		volumeData->widthSpacing(), volumeData->heightSpacing(), volumeData->depthSpacing());
}

void Volume::initMask(int width, int height, int depth,
	float xSpacing, float ySpacing, float zSpacing) {
	volumeMask = new VolumeMask(width, height, depth,
		xSpacing, ySpacing, zSpacing);	
}


void Volume::initOffset(Assign2::DataInput dataType) {
	initOffset(volumeData->width(), volumeData->height(), volumeData->depth(),
		volumeData->widthSpacing(), volumeData->heightSpacing(), volumeData->depthSpacing(), dataType);
}

void Volume::initOffset(int width, int height, int depth,
	float xSpacing, float ySpacing, float zSpacing, Assign2::DataInput dataType) {

	// Note: need the extra bit (...and a few more) to have negative values
	if (dataType == Assign2::DataInput::UINT8) {
		volumeOffset = new VolumeBase_Template<short>(width, height, depth,
			xSpacing, ySpacing, zSpacing);
		volumeOffset->updateRange(0, 255);
	}
	else if (dataType == Assign2::DataInput::UINT16) {
		volumeOffset = new VolumeBase_Template<int>(width, height, depth,
			xSpacing, ySpacing, zSpacing);
		volumeOffset->updateRange(0, 65535);
	}
	else {
		volumeOffset = new VolumeBase_Template<float>(width, height, depth,
			xSpacing, ySpacing, zSpacing);
		volumeOffset->updateRange(0, 0);
	}
}

//Updates
void Volume::updateM(glm::mat4 m) {
	modelM = m;
	invModelM = glm::inverse(m);
}
void Volume::updateScale(float s) {
	scaleM = glm::scale(s, s, s);
}

void Volume::updateLongestLength() {
	float l = volumeData->boxWidth();
	if (l < volumeData->boxHeight()) { l = volumeData->boxHeight(); }
	if (l < volumeData->boxDepth()) { l = volumeData->boxDepth(); }
	lengthLongest = l;
}

void Volume::updateTransform() {
	translateM = glm::translate(volumeData->boxWidth() * 0.5f,
		volumeData->boxHeight() * 0.5f,
		volumeData->boxDepth() * 0.5f);
	invTranslateM = glm::translate(-volumeData->boxWidth() * 0.5f,
		-volumeData->boxHeight() * 0.5f,
		-volumeData->boxDepth() * 0.5f);
}

//Helpers
VolumeBase* Volume::vData() { return volumeData; }
VolumeMask* Volume::vMask() { return volumeMask; }
VolumeState* Volume::vState() { return volumeState; }

const glm::mat4& Volume::M() { return modelM; }
const glm::mat4& Volume::invM() { return invModelM; }
float Volume::longestLength() { return lengthLongest; }

glm::vec3 Volume::getWSfromVS(const glm::vec3& pointVS) {
	return getWSfromVS(glm::vec4(pointVS, 1.));
}
glm::vec3 Volume::getWSfromVS(const glm::vec4& pointVS) {
	return glm::vec3(modelM * invTranslateM * pointVS);
}

glm::vec3 Volume::getVSfromWS(const glm::vec3& pointWS) {
	return getVSfromWS(glm::vec4(pointWS, 1.));
}
glm::vec3 Volume::getVSfromWS(const glm::vec4& pointWS) {
	return glm::vec3(translateM * invModelM * pointWS);
}

glm::vec3 Volume::getDirVSfromWS(const glm::vec3& dirWS) {
	return getDirVSfromWS(glm::vec4(dirWS, 0.));
}
glm::vec3 Volume::getDirVSfromWS(const glm::vec4& dirWS) {
	return glm::vec3(invModelM * dirWS);
}

glm::vec3 Volume::getSpacings() {
	return glm::vec3(
		volumeData->widthSpacing(),
		volumeData->heightSpacing(),
		volumeData->depthSpacing());
}
glm::vec3 Volume::getBoxDimensions() {
	return glm::vec3(
		volumeData->boxWidth(),
		volumeData->boxHeight(),
		volumeData->boxDepth());
}

glm::vec3 Volume::getCellSize(float lengthWorld) {
	float cellX = lengthWorld / volumeData->widthSpacing();
	float cellY = lengthWorld / volumeData->heightSpacing();
	float cellZ = lengthWorld / volumeData->depthSpacing();
	return MathLib::ceilVec3(glm::vec3(cellX, cellY, cellZ));
}

void Volume::translate(const glm::vec3& translateWS) {
	glm::mat4 M = glm::translate(translateWS) * modelM;
	updateM(M);
}

void Volume::rotate(const glm::vec3& rotateAxis, float angleDegrees) {
	// Resource : http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
	glm::quat q = glm::angleAxis(angleDegrees, rotateAxis);
	glm::mat4 rotateM = glm::toMat4(q);
	glm::mat4 M = modelM * rotateM;
	updateM(M);
}

//====

//===== Loading volume data =====//
template <typename T>
void loadVolumeDataTemplate(
	Math::Array3D<T>& vData,
	const std::vector<T>& RAWData,	
	float densityMin, float densityMax,
	float dataTypeRange) {

	// Get minimum and maximum density values	
	float densityRange = densityMax - densityMin;

	// normalize the density value between 0 and data type range (eg. 8bit = 255; 16bit = 65535)
	std::transform(RAWData.begin(),
		RAWData.end(),
		vData.begin(),
		[&](T num) -> T {
			return  ((static_cast<float>(num) - densityMin) / densityRange) * dataTypeRange;
		});
}

void VolumeIO::loadVolumeData(
	VolumeBase* vData,
	const std::vector<unsigned char>& RAWData,
	int densityMin, int densityMax,
	float dataTypeRange) {
	loadVolumeDataTemplate(((VolumeBase_Template<unsigned char>*)vData)->d, 
		RAWData, densityMin, densityMax, dataTypeRange);
}

void VolumeIO::loadVolumeData(
	VolumeBase* vData,
	const std::vector<unsigned short>& RAWData,
	int densityMin, int densityMax,
	float dataTypeRange) {
	loadVolumeDataTemplate(((VolumeBase_Template<unsigned short>*)vData)->d, 
		RAWData, densityMin, densityMax, dataTypeRange);
}

//==== load mask data ====//
void VolumeIO::saveVolumeMask(VolumeMask* vMask, std::vector<unsigned char>& mask) {
	mask.resize(vMask->width() * vMask->height() * vMask->depth());
	std::copy(vMask->d.begin(), vMask->d.end(), mask.begin());
}

void VolumeIO::loadVolumeMask(VolumeMask* vMask, const std::vector<unsigned char>& mask) {
	std::copy(mask.begin(), mask.end(), vMask->d.begin());
}

void VolumeIO::loadVolume(VolumeBase* vOffset, const std::vector<short>& RAWData) {
	std::copy(RAWData.begin(), RAWData.end(), ((VolumeBase_Template<short>*)vOffset)->d.begin());
}

void VolumeIO::loadVolume(VolumeBase* vOffset, const std::vector<int>& RAWData) {
	std::copy(RAWData.begin(), RAWData.end(), ((VolumeBase_Template<int>*)vOffset)->d.begin());
}

void VolumeIO::saveVolume(VolumeBase* vOffset, std::vector<short>& RAWData) {
	size_t size = vOffset->width() * vOffset->height() * vOffset->depth();
	RAWData.resize(size);
	std::copy(
		((VolumeBase_Template<short>*)vOffset)->d.begin(),
		((VolumeBase_Template<short>*)vOffset)->d.end(),
		RAWData.begin());
}

void VolumeIO::saveVolume(VolumeBase* vOffset, std::vector<int>& RAWData) {
	size_t size = vOffset->width() * vOffset->height() * vOffset->depth();
	RAWData.resize(size);
	std::copy(
		((VolumeBase_Template<int>*)vOffset)->d.begin(),
		((VolumeBase_Template<int>*)vOffset)->d.end(),
		RAWData.begin());
}

void VolumeIO::saveVolume(VolumeBase* vOffset, std::vector<unsigned char>& RAWData) {
	size_t size = vOffset->width() * vOffset->height() * vOffset->depth();
	RAWData.resize(size);
	std::copy(
		((VolumeBase_Template<unsigned char>*)vOffset)->d.begin(),
		((VolumeBase_Template<unsigned char>*)vOffset)->d.end(),
		RAWData.begin());
}

void VolumeIO::saveVolume(VolumeBase* vOffset, std::vector<unsigned short>& RAWData) {
	size_t size = vOffset->width() * vOffset->height() * vOffset->depth();
	RAWData.resize(size);
	std::copy(
		((VolumeBase_Template<unsigned short>*)vOffset)->d.begin(),
		((VolumeBase_Template<unsigned short>*)vOffset)->d.end(),
		RAWData.begin());
}