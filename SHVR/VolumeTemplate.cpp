#include "VolumeTemplate.h"



//=====
// For the love all things good and right, watch out for initialization list ordering...
VolumeBase::VolumeBase(int samplesWide, int samplesHigh, int samplesDeep,
	float wSpacing, float hSpacing, float dSpacing) :
	m_width(samplesWide),
	m_height(samplesHigh),
	m_depth(samplesDeep),
	m_wSpacing(wSpacing),
	m_hSpacing(hSpacing),
	m_dSpacing(dSpacing),
	m_cellsWide(samplesWide - 1),
	m_cellsHigh(samplesHigh - 1),
	m_cellsDeep(samplesDeep - 1),
	m_boxWidth((samplesWide - 1)* wSpacing),
	m_boxHeight((samplesHigh - 1)* hSpacing),
	m_boxDepth((samplesDeep - 1)* dSpacing)
{}


void VolumeBase::reloadSpacing(float wSpacing, float hSpacing, float dSpacing) {
	m_wSpacing = wSpacing;
	m_hSpacing = hSpacing;
	m_dSpacing = dSpacing;
	m_boxWidth = m_cellsWide * wSpacing;
	m_boxHeight = m_cellsHigh * hSpacing;
	m_boxDepth = m_cellsDeep * dSpacing;
}

bool VolumeBase::getPosition(const glm::ivec3& indices, glm::vec3& out_pos) const
{
	double position_temp;

	position_temp = posVS1D(indices.x, m_boxWidth, m_cellsWide);
	out_pos.x = position_temp;
	if (position_temp == -1)
		return false;

	position_temp = posVS1D(indices.y, m_boxHeight, m_cellsHigh);
	out_pos.y = position_temp;
	if (position_temp == -1)
		return false;

	position_temp = posVS1D(indices.z, m_boxDepth, m_cellsDeep);
	out_pos.z = position_temp;
	if (position_temp == -1)
		return false;

	return true;
}


bool VolumeBase::getPositionExplicit(const glm::ivec3& indices, glm::vec3& out_pos) const
{
	//Note: Only use if all possible indices are valid
	out_pos.x = posVSExplicit1D(indices.x, m_boxWidth, m_cellsWide);
	out_pos.y = posVSExplicit1D(indices.y, m_boxHeight, m_cellsHigh);
	out_pos.z = posVSExplicit1D(indices.z, m_boxDepth, m_cellsDeep);

	return true;
}

bool VolumeBase::getCell(const glm::vec3& pos, glm::ivec3& out_idx) const
{
	int idx;

	idx = index1D(pos.x, m_boxWidth, m_cellsWide);
	out_idx.x = idx;
	if (idx == -1)
		return false;

	idx = index1D(pos.y, m_boxHeight, m_cellsHigh);
	out_idx.y = idx;
	if (idx == -1)
		return false;

	idx = index1D(pos.z, m_boxDepth, m_cellsDeep);
	out_idx.z = idx;
	if (idx == -1)
		return false;

	return true;
}

bool VolumeBase::getCellExplicit(const glm::vec3& pos, glm::ivec3& out_idx) const
{
	int idx;

	idx = indexExplicit1D(pos.x, m_boxWidth, m_cellsWide);
	out_idx.x = idx;

	idx = indexExplicit1D(pos.y, m_boxHeight, m_cellsHigh);
	out_idx.y = idx;

	idx = indexExplicit1D(pos.z, m_boxDepth, m_cellsDeep);
	out_idx.z = idx;

	return true;
}
