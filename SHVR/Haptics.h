#pragma once

#include "World.h"
#include "mathLib.h"
#include "volumes.h"
#include "Commands.h"
#include "VolumeFill.h"

#include "qthread.h"

struct PaintThread : public VolumeFill, public QThread{
	bool simulationFinished;
	VolumeMask* mask;
	VolumeState* volumeState;
	unsigned char paintValue;
	unsigned char paintType; 	
	MaskUpdateInfo* maskUpdateInfo;

	unsigned char offsetType;
	float paintWorldRadius2;
	float paintMaxOffset;	

	PaintThread() {
		maskUpdateInfo = 0;
		volumeData = 0;
		volumeState = 0;
		mask = 0;
		paintValue = 1;
		paintType = FILL;
		simulationFinished = true;
		paintWorldRadius2 = -1;		
	}
	void updatePaintOver(ColorGroups* cGroups) {
		for (int i = 0; i < cGroups->size(); i++) {
			volumeState->paintOverGroups[i] = cGroups->at(i).paintOver;
		}
	}

	bool updateMaskValue(const glm::ivec3& idx) override {
		float originalCellValue = volumeData->valueAtCell(idx) - volumeState->getVoxelOffset(idx);

		if (paintValue != 0 && originalCellValue > inValue) {			
			volumeState->updateMaskValue(paintValue, idx);

			// Update offset
			if (offsetType == DIST2) {
				//Note: Dist2 is more efficient than Euclidean distance
				float weightDist = 1.f - volumeState->getDist2Explicit(seeds[0], idx, paintWorldRadius2);
				weightDist = glm::clamp(weightDist, 0.f, 1.f);
				float offset = paintMaxOffset * weightDist;
				volumeState->updateOffsetValue(offset, paintMaxOffset, idx);
			}

			else if (offsetType == UNIFORM) {
				float offset = paintMaxOffset;
				volumeState->updateOffsetValue(offset, paintMaxOffset, idx);
			}


			return true;
		}

		else if (originalCellValue > inValue) {
			volumeState->updateMaskValue(paintValue, idx);
			volumeState->updateOffsetValue(0, 0, idx);		//erase offset value

			return true;
		}
		
		return false;
	}	

	void pause() {
		std::chrono::time_point<std::chrono::steady_clock> timer = std::chrono::high_resolution_clock::now();
		while (!simulationFinished) {
			MathLib::limitRefreshRate(100000, timer);
		}	//Sleep until loop ends
	}

	void close() {
		pause();
	}

	bool paintMask() {
		if (paintType == FILL) {
			return fill();
		}
		else if (paintType == FLOOD_FILL) {
			return floodFill_sixWay();
		}
		else if (paintType == SPECKLES) {
			return fill_speckles();
		}
		return false;
	}

	void exec() {
		simulationFinished = false;
		if (!maskUpdateInfo->queueTextureMaskUpdate) {
			if (paintMask()) {								
				maskUpdateInfo->queueUpdate(idxMin, idxMax, mask, volumeData);
				volumeState->updateMinMax(idxMin, idxMax);
			}
			else {
				glm::ivec3 zero(0, 0, 0);
				volumeState->updateMinMax(zero,zero);
				//volumeState->updateMinMax(glm::ivec3(0,0,0), glm::ivec3(0, 0, 0));
			}
		}
		simulationFinished = true;
	}

	void run() {		
		exec();		
	}
};

struct ConnectivityVolume: VaryingVolumeTemplate<bool> {};

class ConnectivityThread : public VolumeFill, public QThread
{	
	std::chrono::time_point<std::chrono::high_resolution_clock> timer;

public:
	ConnectivityVolume* connectBack;		//Connections are updated at the back
	ConnectivityVolume* connectFront;		//Connections are used from the front
	int hapticType;
	bool queueUpdateProbeSize;
	bool simulationFinished;
	glm::ivec3 idxConnected;

	ConnectivityThread(ConnectivityVolume* cFront, ConnectivityVolume* cBack) {
		volumeData = 0;
		connectFront = cFront;
		connectBack = cBack;
		simulationFinished = true;		
		queueUpdateProbeSize = false;
		hapticType = HAPTIC_SURFACE_SAMPLING;
	}

	~ConnectivityThread() {}

	void updateConnectivityBack() {
		for (int i = 0; i < connectBack->size(); i++) {
			connectBack->at(i) = false;
		}

		if (hapticType == HAPTIC_SURFACE_SAMPLING ||
			hapticType == HAPTIC_PROXIMITY_FOLLOWING) {
			connectivityFill();
		}
		else if (hapticType == HAPTIC_OBJECT_FOLLOWING) {
			connectivityFloodFill();
		}
	}

	void pause() {
		std::chrono::time_point<std::chrono::steady_clock> timer = std::chrono::high_resolution_clock::now();		
		while (!simulationFinished) { 
			MathLib::limitRefreshRate(100000, timer); }	//Sleep until loop ends
	}

	void close() {
		pause();
	}

	int exec() {
		simulationFinished = false;
		updateConnectivityBack();
		simulationFinished = true;
		return 0;
	}

	void run() {
		exec();
	}

protected:
	bool updateMaskValue(const glm::ivec3& idx) override {
		//Note: Offset is not considered in these calculations to
		// enable touching all visible surfaces
		float cellValue = volumeData->valueAtCell(idx);
		if (cellValue > inValue) {
			size_t iterIdx = connectBack->getIterIndex(idx);
			connectBack->at(iterIdx) = true;
			return true;
		}
		return false;		
	}

	bool connectivityFill() {
		if (fill()) {
			return true;
		}
		return false;
	}

	bool connectivityFloodFill() {
		if (floodFill_sixWay()) {
			return true;
		}
		return false;
	}
};

class Haptics
{
public:
	Haptics(World* w,
		ProbeParams* _probeParams,
		IsoParams* _isoParams,
		VisualParams* _visualParams,
		ColorGroups* _cGroups);

	~Haptics();
	void updateVolumes(VolumeBase* vData, VolumeMask* vMask);
	void updateIsoMins();
	void updateWorkspaceScale(float _workspaceScale);
	bool initHapticDevice();
	void pause();
	void run();
	void stop();
	void closeHaptics();

};
