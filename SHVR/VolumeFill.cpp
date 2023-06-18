#include "VolumeFill.h"

void VolumeFill::updateParameters(const glm::ivec3& indexMin,
	const glm::ivec3& indexMax,
	float minimumFillThreshold) {

	idxMin = indexMin;
	idxMax = indexMax;
	inValue = minimumFillThreshold;
}

void VolumeFill::updateSeeds(const std::vector<glm::ivec3>& seedIndexes) {
	seeds.clear();
	for (int i = 0; i < seedIndexes.size(); i++) {
		seeds.push_back(seedIndexes[i]);
	}
}

bool VolumeFill::fill() {
	//Note: Can handle about ~20000 voxels at a time	

	// Change values in between minimum and maximum index
	for (int k = idxMin.z; k < idxMax.z; k++) {
		for (int j = idxMin.y; j < idxMax.y; j++) {
			for (int i = idxMin.x; i < idxMax.x; i++) {
				glm::ivec3 idx = glm::ivec3(i, j, k);
				updateMaskValue(idx);
			}
		}
	}

	return true;
}

bool VolumeFill::floodFill_scanline() {
	//DESCRIPTION: Grow seed in object space in t_haptic based 
	//on neighbouring voxels that are above a threshold that is
	//within a radius from a center point

	std::queue<glm::ivec3> q;
	using Interval = std::pair<int32_t, int32_t>;
	std::vector<Interval> visited;
	for (auto s : seeds) { q.push(s); }

	while (!q.empty()) {
		auto c = q.front();
		q.pop();

		//Check if index is inside the volume
		if (!volumeData->isValidIndex(c)) {
			continue;
		}

		//Check if cell index is within the list of already-segmented intervals
		int32_t index = volumeData->iterIndex(c.x, c.y, c.z);
		auto iter = std::lower_bound(visited.begin(), visited.end(), index,
			[](Interval i, int32_t index) { return i.second <= index; });
		if (iter != visited.end()) {
			auto i = *iter;
			if (i.first <= index && index < i.second)
				continue;
		}

		Interval i;
		int x = c.x;

		//Region-grow on the left side of x
		while (x >= 0) {
			glm::ivec3 idx = glm::ivec3(x, c.y, c.z);
			//Break if hit voxel outside radius
			if (idx.x < idxMin.x || idx.x > idxMax.x ||
				idx.y < idxMin.y || idx.y > idxMax.y ||
				idx.z < idxMin.z || idx.z > idxMax.z) {
				break;
			}

			//Break if hit voxel below threshold
			if (updateMaskValue(idx)) {
				//Grow up, down, left, and right
				q.push(idx + glm::ivec3(0, -1, 0));
				q.push(idx + glm::ivec3(0, 1, 0));
				q.push(idx + glm::ivec3(0, 0, 1));
				q.push(idx + glm::ivec3(0, 0, -1));
			}
			--x;
		}
		//Set start of interval
		i.first = volumeData->iterIndex((x < 0 ? 0 : x), c.y, c.z);

		//Grow on the right side of x
		x = c.x + 1;
		while (x < volumeData->width()) {
			glm::ivec3 idx = glm::ivec3(x, c.y, c.z);

			//Break if hit voxel outside radius
			if (idx.x < idxMin.x || idx.x >= idxMax.x ||
				idx.y < idxMin.y || idx.y >= idxMax.y ||
				idx.z < idxMin.z || idx.z >= idxMax.z) {
				break;
			}

			//Break if hit voxel below threshold
			if (updateMaskValue(idx)) {
				//Grow up, down, left, and right
				q.push(idx + glm::ivec3(0, -1, 0));
				q.push(idx + glm::ivec3(0, 1, 0));
				q.push(idx + glm::ivec3(0, 0, 1));
				q.push(idx + glm::ivec3(0, 0, -1));
			}
			++x;
		}
		//Set end of interval
		i.second = volumeData->iterIndex((x > volumeData->width() ? volumeData->width() : x), c.y, c.z);

		if (i.first < i.second) {
			iter = std::upper_bound(
				visited.begin(), visited.end(), i.second,
				[](int32_t index, Interval i) { return index < i.second; });
			if (iter == visited.end())
				visited.emplace_back(i);
			else
				visited.insert(iter, i);
		}
	}

	return true;
}

bool VolumeFill::floodFill_sixWay() {
	//Can handle about ~6000 voxels at a time
	glm::ivec3 idx;
	std::queue<glm::ivec3> q;
	for (auto s : seeds) { q.push(s); }

	auto comp = [](glm::ivec3 const& a,
		glm::ivec3 const& b)->bool
	{
		return
			(a.x != b.x ?
				(a.x < b.x) :
				(a.y != b.y ?
					(a.y < b.y) :
					(a.z < b.z)));
	};

	std::set<glm::ivec3, decltype(comp)> visited(comp);

	glm::ivec3 u(0, 1, 0);		//up
	glm::ivec3 d(0, -1, 0);		//down
	glm::ivec3 l(-1, 0, 0);		//left
	glm::ivec3 r(1, 0, 0);		//right
	glm::ivec3 f(0, 0, 1);		//front
	glm::ivec3 b(0, 0, -1);		//back

	while (!q.empty())
	{
		idx = q.front();
		q.pop();

		// outside radius
		if (idx.x < idxMin.x || idx.x >= idxMax.x ||
			idx.y < idxMin.y || idx.y >= idxMax.y ||
			idx.z < idxMin.z || idx.z >= idxMax.z) {
			continue;
		}

		// Voxel is already visited
		if (!(visited.insert(idx).second))
		{
			continue;
		}

		if (updateMaskValue(idx)) {
			q.push(idx + u);
			q.push(idx + d);
			q.push(idx + l);
			q.push(idx + r);
			q.push(idx + f);
			q.push(idx + b);
		}
	}
	return true;
}


struct VolumeFill::SpeckleSeed {
	glm::ivec3 pos;		//Position of seed in volumeData
	bool visited;

	SpeckleSeed() {
		pos = glm::ivec3(-1, -1, -1);
		visited = false;
	}
};

void VolumeFill::findSpeckles(
	std::vector<glm::ivec3>& out_seedPoints) {

	int s_width = idxMax.x - idxMin.x;
	int s_height = idxMax.y - idxMin.y;
	int s_depth = idxMax.z - idxMin.z;

	std::vector<SpeckleSeed> s_seeds;
	std::vector<std::vector<bool>> s_neighbours;
	std::vector<bool> seed_row;
	seed_row.push_back(false);

	s_neighbours.push_back(seed_row);

	SpeckleSeed s_edge;
	s_seeds.push_back(s_edge);
	std::vector<int> box_slice;
	box_slice.resize(s_width * s_height, -1);		//-1 = out of volume; 0 = connected to edge


	//Find volume and position of speckles from a given radius (connected seeds have child-parent relationship)
	for (int k = 0; k < s_depth; k++) {
		for (int j = 0; j < s_height; j++) {
			for (int i = 0; i < s_width; i++) {

				float value = volumeData->valueAtCell(glm::ivec3(idxMin.x + i, idxMin.y + j, idxMin.z + k));
				int sIndex = i + j * s_width;
				int speckleNum = -1;

				//Discard voxels below isoMin
				if (value <= inValue) {
					box_slice[sIndex] = -1;
				}

				//Indicate the voxels at the edge, and indicate the nearby seeds to be part of group
				else if (i == 0 || j == 0 || k == 0 || i == s_width - 1 || j == s_height - 1 || k == s_depth - 1) {
					if (i > 0) {
						speckleNum = box_slice[(i - 1) + j * s_width];
						if (speckleNum > 0) {
							s_neighbours[0][speckleNum] = true;
							s_neighbours[speckleNum][0] = true;
						}
					}

					if (j > 0) {
						speckleNum = box_slice[i + (j - 1) * s_width];
						if (speckleNum > 0) {
							s_neighbours[0][speckleNum] = true;
							s_neighbours[speckleNum][0] = true;
						}
					}

					if (k > 0) {
						speckleNum = box_slice[i + j * s_width];
						if (speckleNum > 0) {
							s_neighbours[0][speckleNum] = true;
							s_neighbours[speckleNum][0] = true;
						}
					}

					box_slice[sIndex] = 0;
				}


				//Seeds not at the edge
				else {
					int currSpeckleNum = -1;
					// ===== LEFT ===== //
					speckleNum = box_slice[(i - 1) + j * s_width];

					if (speckleNum >= 0) {
						if (currSpeckleNum >= 0) {
							s_neighbours[currSpeckleNum][speckleNum] = true;
							s_neighbours[speckleNum][currSpeckleNum] = true;
						}

						currSpeckleNum = speckleNum;
					}

					// ===== POS_BACK ===== //

					speckleNum = box_slice[i + (j - 1) * s_width];

					if (speckleNum >= 0) {
						if (currSpeckleNum >= 0) {
							s_neighbours[currSpeckleNum][speckleNum] = true;
							s_neighbours[speckleNum][currSpeckleNum] = true;
						}
						currSpeckleNum = speckleNum;
					}


					// ===== BOTTOM ===== //
					speckleNum = box_slice[i + j * s_width];

					if (speckleNum >= 0) {
						if (currSpeckleNum >= 0) {
							s_neighbours[currSpeckleNum][speckleNum] = true;
							s_neighbours[speckleNum][currSpeckleNum] = true;
						}
						currSpeckleNum = speckleNum;
					}

					//No connected component
					if (currSpeckleNum < 0) {
						SpeckleSeed s;
						s_seeds.push_back(s);
						currSpeckleNum = s_seeds.size() - 1;
						s_seeds[currSpeckleNum].pos = glm::ivec3(idxMin.x + i, idxMin.y + j, idxMin.z + k);

						//Add a new row in the neighbourhood info
						std::vector<bool> seed_row;
						seed_row.resize(s_neighbours.size(), false);
						s_neighbours.push_back(seed_row);
						for (int i = 0; i < s_neighbours.size(); i++) {
							s_neighbours[i].push_back(false);
						}
					}

					box_slice[sIndex] = currSpeckleNum;
				}
			}
		}
	}


	//Update all adjacent seeds that are touching the edge
	std::queue<int> q_onEdge;
	q_onEdge.push(0);
	while (!q_onEdge.empty()) {
		int next = q_onEdge.front();
		q_onEdge.pop();

		if (s_seeds[next].visited == false) {

			//Seed touches the edge
			s_neighbours[0][next] = true;
			s_neighbours[next][0] = true;

			//Add the next round of seeds
			for (int i = 1; i < s_neighbours.size(); i++) {
				if (s_neighbours[next][i] == true) {
					q_onEdge.push(i);
				}
			}

			//Mark that the seed has been visited
			s_seeds[next].visited = true;
		}
	}

	//Filter out seeds that do not touch the edge
	for (int i = 1; i < s_seeds.size(); i++) {
		if (s_neighbours[0][i] == false) {
			out_seedPoints.push_back(s_seeds[i].pos);
		}
	}
}
bool VolumeFill::fill_speckles() {
	seeds.clear();
	findSpeckles(seeds);
	return floodFill_sixWay();
}
