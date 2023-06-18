#pragma once

//Terminologies:
// Sample: The full resolution of 3D image stack (width, height, depth)
// Cell: One less than the sample length (Note: there are no cells at the edges to interpolate value)

#include <glm/glm.hpp>  //namespace glm
#include "Array3D.h"    //Array3D

class VolumeBase
{
	struct OutOfFieldException : std::exception {
		const char* what() const noexcept { return "Out of Field Query\n"; }
	};

public:
	VolumeBase(int width, int height, int depth, float wSpacing, float hSpacing, float zSpacing);
	virtual ~VolumeBase() {}

	void reloadSpacing(float wSpacing, float hSpacing, float dSpacing);
	bool getCell(const glm::vec3& pos, glm::ivec3& out_idx) const;
	bool getCellExplicit(const glm::vec3& pos, glm::ivec3& out_idx) const;
	bool getPosition(const glm::ivec3& indices, glm::vec3& out_pos) const;
	bool getPositionExplicit(const glm::ivec3& indices, glm::vec3& out_pos) const;

	inline float boxWidth() const;
	inline float boxHeight() const;
	inline float boxDepth() const;
	inline float widthSpacing() const;
	inline float heightSpacing() const;
	inline float depthSpacing() const;
	inline int width() const;
	inline int height() const;
	inline int depth() const;
	inline glm::ivec3 dimensions() const;

	bool isValidXIndex(int index) const;
	bool isValidYIndex(int index) const;
	bool isValidZIndex(int index) const;
	bool isValidIndex(const glm::ivec3& indices) const;
	bool isValidIndex(int x, int y, int z) const;

	inline size_t iterIndex(const size_t x = 0, const size_t y = 0, const size_t z = 0) {
		return x + y * m_width + z * m_width * m_height;
	}

	virtual bool setValue(float value) { return false; }
	virtual bool setValueAtCell(float value, const glm::ivec3& cellIndex) { return false; }
	virtual bool valueAt(const glm::vec3& posVS, float& out_value) const { return false; }
	virtual float valueAtCell(const glm::ivec3& cellIndex) { return 0; }
	virtual void* data() { return 0; }

	void applyVoxelIntensityOffset(float voxelOffset, const glm::ivec3& idx) {
		float cellValue = valueAtCell(idx);
		setValueAtCell(cellValue + voxelOffset, idx);
	}

	void updateRange(size_t minRange, size_t maxRange) {
		m_rangeMin = minRange;
		m_rangeMax = maxRange;

	}
	size_t getRangeMin() { return m_rangeMin; }
	size_t getRangeMax() { return m_rangeMax; }

protected:
	double posVS1D(int index, double boxLength, int cellLength) const;
	double posVSExplicit1D(int idx, double boxLength, int cellLength) const;
	int index1D(double posVS, double boxLength, int cellLength) const;
	int indexExplicit1D(double posVS, double boxLength, int cellLength) const;
	float placeIndex1D(int index, double spacing) const;
	
	float m_wSpacing, m_hSpacing, m_dSpacing;
	int m_width, m_height, m_depth;
	int m_cellsWide, m_cellsHigh, m_cellsDeep;
	float m_boxWidth, m_boxHeight, m_boxDepth;
	size_t m_rangeMin, m_rangeMax;
};


template <typename T>
class VolumeBase_Template : public VolumeBase {
public:	
	VolumeBase_Template<T>(int width, int height, int depth, 
		float wSpacing, float hSpacing, float zSpacing) :
		VolumeBase(width, height, depth, wSpacing, hSpacing, zSpacing),
		d(width, height, depth, 0) {}

	~VolumeBase_Template() {
		d.reset();
	}

	bool setValueAt (T value, const glm::vec3& posVS);
	bool setValueAtCell(float value, const glm::ivec3& cellIndex) override;
	void setValueAroundCellIndex (const glm::ivec3& cellIndex, T value);

	T& sampleAt(const glm::ivec3& index);
	T sampleAt(const glm::ivec3& index) const;	
	bool valueAt(const glm::vec3& posVS, float& out_value) const override;		//needs the value interpolated
	float valueAtCell(const glm::ivec3& cellIndex) override;
	void* data() override;
	bool setValue(float value) override;
	//void import(const std::vector<T>& vList) override {
	//	std::copy(vList.begin(), vList.end(), d.begin());
	//}

	//void export(std::vector<T>& vList) override {
	//	vList.resize(m_width * m_height * m_depth);
	//	std::copy(d.begin(), d.end(), vList.begin());
	//}

	Math::Array3D<T> d;	

private:	
	float interpolateCell(const glm::ivec3& cellIndex, const glm::vec3& rst) const;
};

//====

inline bool VolumeBase::isValidXIndex(int index) const
{
	return index >= 0 && index < m_width;
}

inline bool VolumeBase::isValidYIndex(int index) const
{
	return index >= 0 && index < m_height;
}

inline bool VolumeBase::isValidZIndex(int index) const
{
	return index >= 0 && index < m_depth;
}

inline bool VolumeBase::isValidIndex(const glm::ivec3& indices) const
{
	return isValidXIndex(indices.x) &&
		isValidYIndex(indices.y) &&
		isValidZIndex(indices.z);
}

inline bool VolumeBase::isValidIndex(int x, int y, int z) const
{
	return isValidXIndex(x) &&
		isValidYIndex(y) &&
		isValidZIndex(z);
}

inline double VolumeBase::posVS1D(int idx, double boxLength, int cellLength) const
{
	//Position of index along one axis of the box (width, height or depth)
	double lengthRatio = (double)idx / (double)cellLength;
	double boxPos = -1;

	if (lengthRatio >= 0.0 && lengthRatio <= 1.0) // valid for space
	{
		boxPos = lengthRatio * boxLength;
		if (lengthRatio == 1.0 && boxPos != 0.0)
			boxPos = -1;
	}

	return boxPos;
}

inline double VolumeBase::posVSExplicit1D(int idx, double boxLength, int cellLength) const
{
	//Position of index along one axis of the box (width, height or depth)
	//Note: Only use if all indices are valid
	double lengthRatio = (double)idx / (double)cellLength;
	double boxPos = lengthRatio * boxLength;

	return boxPos;
}

// if valid will give index, if invalid the returns -1
inline int VolumeBase::index1D(double posVS, double boxLength, int cellLength) const
{
	//Index of position along one axis of the box 
	double lengthRatio = (posVS / boxLength);
	int idx = -1;

	if (lengthRatio >= 0.0 && lengthRatio < 1.0) // valid for space
	{
		idx = lengthRatio * (cellLength);
	}
	return idx;
}

inline int VolumeBase::indexExplicit1D(double posVS, double boxLength, int cellLength) const
{
	double ratio = (posVS / boxLength);
	int index = -1;
	index = ratio * (cellLength);

	return index;
}

inline float VolumeBase::placeIndex1D(int index, double spacing) const { return spacing * static_cast<float>(index); }

// === Get/Set/Helper functions === //
inline float VolumeBase::boxWidth() const { return m_boxWidth; }
inline float VolumeBase::boxHeight() const { return m_boxHeight; }
inline float VolumeBase::boxDepth() const { return m_boxDepth; }
inline float VolumeBase::widthSpacing() const { return m_wSpacing; }
inline float VolumeBase::heightSpacing() const { return m_hSpacing; }
inline float VolumeBase::depthSpacing() const { return m_dSpacing; }
inline int VolumeBase::width() const { return m_width; }
inline int VolumeBase::height() const { return m_height; }
inline int VolumeBase::depth() const { return m_depth; }
inline glm::ivec3 VolumeBase::dimensions() const {
	return glm::ivec3(m_width, m_height, m_depth);
}
//====

template <typename T>
void* VolumeBase_Template<T>::data()
{
	return (void*)d.data();
}

template <typename T>
bool VolumeBase_Template<T>::setValue(float value) {
	for (int k = 0; k < m_depth; k++) {
		for (int j = 0; j < m_height; j++) {
			for (int i = 0; i < m_width; i++) {
				d.at(i, j, k) = (T)value;
			}
		}
	}
	return true;
}

template <typename T>
bool VolumeBase_Template<T>::setValueAt(T value,
	const glm::vec3& posVS)
{
	glm::ivec3 indices(0);
	if (!getCell(posVS, indices)) { return false; }
	sampleAt(indices) = value;
	return true;
}

template <typename T>
bool VolumeBase_Template<T>::setValueAtCell(float value,
	glm::ivec3 const& indices)
{
	if (!isValidIndex(indices)) { return false; }
	sampleAt(indices) = value;
	return true;
}

template <typename T>
void VolumeBase_Template<T>::setValueAroundCellIndex(const glm::ivec3& cellIndex,
	T value)
{
	glm::ivec3 offset;
	//Cover at least one cell length
	int reachBack = 1;
	int reachForw = reachBack;

	for (int i = -reachBack; i <= reachForw; ++i) {
		for (int j = -reachBack; j <= reachForw; ++j) {
			for (int k = -reachBack; k <= reachForw; ++k) {
				offset.x = i;
				offset.y = j;
				offset.z = k;
				glm::ivec3 newIndex = cellIndex + offset;
				if (isValidIndex(newIndex))
				{
					sampleAt(newIndex) = value;
				}
			}
		}
	}
}

template <typename T>
T& VolumeBase_Template<T>::sampleAt(const glm::ivec3& index)
{
	return d.at(index.x, index.y, index.z);
}
template <typename T>
T VolumeBase_Template<T>::sampleAt(const glm::ivec3& index) const
{
	return d.at(index.x, index.y, index.z);
}

template <typename T>
float VolumeBase_Template<T>::valueAtCell(const glm::ivec3& cellIndex) {
	return d.at(cellIndex.x, cellIndex.y, cellIndex.z);
}


template <typename T>
float VolumeBase_Template<T>::interpolateCell(const glm::ivec3& cellIndex, const glm::vec3& pos) const
{
	glm::ivec3 xIndex = cellIndex; // eight samples per cell
	glm::ivec3 yIndex = cellIndex;
	glm::ivec3 zIndex = cellIndex;
	glm::ivec3 xyIndex = cellIndex;
	glm::ivec3 xzIndex = cellIndex;
	glm::ivec3 zyIndex = cellIndex;
	glm::ivec3 xyzIndex = cellIndex;

	xIndex.x++;
	yIndex.y++;
	zIndex.z++;

	xyIndex.x++;
	xyIndex.y++;

	xzIndex.x++;
	xzIndex.z++;

	zyIndex.y++;
	zyIndex.z++;

	xyzIndex.x++;
	xyzIndex.y++;
	xyzIndex.z++;

	float inter = 0;
	inter += (float)sampleAt(cellIndex) * (1.0 - pos.x) * (1.0 - pos.y) * (1.0 - pos.z);
	inter += (float)sampleAt(xIndex) * pos.x * (1.0 - pos.y) * (1.0 - pos.z);
	inter += (float)sampleAt(yIndex) * (1.0 - pos.x) * pos.y * (1.0 - pos.z);
	inter += (float)sampleAt(zIndex) * (1.0 - pos.x) * (1.0 - pos.y) * pos.z;
	inter += (float)sampleAt(xyIndex) * pos.x * pos.y * (1.0 - pos.z);
	inter += (float)sampleAt(xzIndex) * pos.x * (1.0 - pos.y) * pos.z;
	inter += (float)sampleAt(zyIndex) * (1.0 - pos.x) * pos.y * pos.z;
	inter += (float)sampleAt(xyzIndex) * pos.x * pos.y * pos.z;

	return inter;
}

template <typename T>
bool VolumeBase_Template<T>::valueAt(const glm::vec3& posVS, float& out_value) const
{
	glm::ivec3 cellIndex(0);
	if (!getCell(posVS, cellIndex))
		return false;

	float originX = placeIndex1D(cellIndex.x, widthSpacing());
	float originY = placeIndex1D(cellIndex.y, heightSpacing());
	float originZ = placeIndex1D(cellIndex.z, depthSpacing());

	glm::vec3 pos = posVS;
	pos.x -= originX;
	pos.y -= originY;
	pos.z -= originZ;

	pos.x /= widthSpacing();
	pos.y /= heightSpacing();
	pos.z /= depthSpacing();

	out_value = interpolateCell(cellIndex, pos);

	return true;
}




