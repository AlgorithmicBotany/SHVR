#include "Haptics.h"
#include <GL/glew.h>
#include <chai3d.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

#include <QDebug>
#include <QtWidgets\qwidget.h>

#define HAPTIC_SURFACE_SAMPLING_PROJECTION_THRESHOLD -0.4f

World* world;
ProbeParams* probeParams;
IsoParams* isoParams;
VisualParams* visualParams;
ColorGroups* cGroups;
PaintThread* paintThread;
std::vector<bool> layersEnabled;

glm::vec3 probePosWS_prev;
float probeSize_prev;
float k_weight;
float workspaceScale;

bool simulationRunning = false;
bool simulationFinished = true;
bool isIdle = true;
bool isHit = false;

chai3d::cHapticDeviceHandler* handler;
chai3d::cGenericHapticDevicePtr hapticDevice;
chai3d::cThread* hapticsThread;
chai3d::cFrequencyCounter freqCounterHaptics;

std::vector<glm::vec3> offsets;		//range: [-1,1]^3
std::vector<glm::vec3> samplePointsVS;
ConnectivityVolume* connectFront;
ConnectivityVolume* connectBack;
ConnectivityThread* connectThread;

void updateOffsets(unsigned int numberOfOffsets);
void updateSamplePoints(const glm::vec3& probePosWS);
void updateHaptics();

Haptics::Haptics(World* w, 
	ProbeParams* _probeParams, 
	IsoParams* _isoParams, 
	VisualParams* _visualParams,
	ColorGroups* _cGroups) {
	world = w;
	probeParams = _probeParams;
	isoParams = _isoParams;
	visualParams = _visualParams;
	cGroups = _cGroups;
	bool isDeviceInitialized = initHapticDevice();
	paintThread = new PaintThread();

	connectFront = new ConnectivityVolume();
	connectBack = new ConnectivityVolume();
	connectThread = new ConnectivityThread(connectFront, connectBack);
	hapticsThread = new chai3d::cThread();

	updateOffsets(3000);

	k_weight = 1;
	probeSize_prev = 0;
	workspaceScale = probeParams->workspaceScale;

	layersEnabled.resize(10, true);
}

Haptics::~Haptics() {	
	closeHaptics();
	paintThread->close();
	connectThread->close();
	delete paintThread;
	delete connectThread;	
	delete connectFront;
	delete connectBack;
}

void Haptics::updateVolumes(VolumeBase* vData, VolumeMask* vMask) {
	connectThread->volumeData = world->volume.vData();

	paintThread->volumeData = world->volume.vData();
	paintThread->mask = world->volume.vMask();
	paintThread->volumeState = world->volume.vState();
	paintThread->maskUpdateInfo = world->volume.maskUpdateInfo;
}

bool Haptics::initHapticDevice() {
	using namespace chai3d;
	// create a haptic device handler
	handler = new cHapticDeviceHandler();

	// get a handle to the first haptic device
	handler->getDevice(hapticDevice, 0);

	// open a connection to haptic device
	if (!hapticDevice->open()) {
		return false;
	}

	// calibrate device (if necessary)
	hapticDevice->calibrate();

	// retrieve information about the current haptic device
	cHapticDeviceInfo info = hapticDevice->getSpecifications();

	// if the device has a gripper, enable the gripper to simulate a user switch
	hapticDevice->setEnableGripperUserSwitch(true);

	return true;
}

void Haptics::pause() {
	using namespace chai3d;

	// stop the simulationx
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }
	hapticsThread->stop();
}

void Haptics::updateWorkspaceScale(float _workspaceScale) {
	workspaceScale = _workspaceScale;
}


void updateConnectivityVolumeSize(const glm::ivec3& idxMin, const glm::ivec3& cellRadius) {
	// Note: adding (1,1,1) account for loss of precision with integers
	glm::ivec3 cellLengths = cellRadius * 2 + glm::ivec3(1, 1, 1);
	VolumeBase* vData = world->volume.vData();
	if (cellLengths.x > vData->width()) { cellLengths.x = vData->width(); }
	if (cellLengths.y > vData->height()) { cellLengths.y = vData->height(); }
	if (cellLengths.z > vData->depth()) { cellLengths.z = vData->depth(); }

	connectFront->updateSize(idxMin, cellLengths);
	connectBack->updateSize(idxMin, cellLengths);
}


void getBoxMinMaxIndex(
	const glm::ivec3& idx,
	const glm::ivec3& cellRadius,
	glm::ivec3& idxMin_out,
	glm::ivec3& idxMax_out) {
	////Note: one is added to value to handle data loss from converting from float to int,
	////Note2: idxMax adds two because idxMin uses (>=) while idxMax uses (<)
	VolumeBase* vData = world->volume.vData();
	idxMin_out.x = std::max(0, idx.x - cellRadius.x);
	idxMax_out.x = std::min(idx.x + cellRadius.x + 1, (int)vData->width());

	idxMin_out.y = std::max(0, idx.y - cellRadius.y);
	idxMax_out.y = std::min(idx.y + cellRadius.y + 1, (int)vData->height());

	idxMin_out.z = std::max(0, idx.z - cellRadius.z);
	idxMax_out.z = std::min(idx.z + cellRadius.z + 1, (int)vData->depth());
}

void updateOffsets(unsigned int numberOfOffsets) {
	MathLib::makeOffsets(1, numberOfOffsets, offsets);
	samplePointsVS.resize(offsets.size());
}

void updateSamplePoints(const glm::vec3& probePosWS) {
	glm::vec3 pointVS = world->volume.getVSfromWS(probePosWS);
	glm::vec3 volumeRadius = world->volume.getSpacings() * probeParams->probeSize;

	glm::vec3 offsetVS;
	for (int i = 0; i < samplePointsVS.size(); i++) {
		offsetVS = offsets[i] * volumeRadius;
		samplePointsVS[i] = pointVS + offsetVS;
	}
}

typedef std::pair<float, glm::ivec3> distBox;
struct by_dist {
	bool operator()(distBox const& a, distBox const& b) {
		return a.first < b.first;
	}
};

bool isConnected(const glm::ivec3& aMin, const glm::ivec3& aMax, 
	const glm::ivec3& bMin, const glm::ivec3& bMax,
	glm::ivec3& out_connectedIdx) {

	//No overlap
	if (aMin.x > bMax.x &&
		aMin.y > bMax.y &&
		aMin.z > bMax.z) {
		return false;
	}

	else if (aMax.x <= bMin.x &&
		aMax.y <= bMin.y &&
		aMax.z <= bMin.z) {
		return false;
	}

	// Overlap between two axis-aligned bounding boxes (Box-box intersection)
	else {
		glm::ivec3 idxMin;
		idxMin.x = std::max(aMin.x, bMin.x);
		idxMin.y = std::max(aMin.y, bMin.y);
		idxMin.z = std::max(aMin.z, bMin.z);

		glm::ivec3 idxMax;
		idxMax.x = std::min(aMax.x, bMax.x);
		idxMax.y = std::min(aMax.y, bMax.y);
		idxMax.z = std::min(aMax.z, bMax.z);

		for (int k = idxMin.z; k < idxMax.z; k++) {
			for (int j = idxMin.y; j < idxMax.y; j++) {
				for (int i = idxMin.x; i < idxMax.x; i++) {
					glm::ivec3 idx(i, j, k);
					if (connectBack->at(connectBack->getIterIndex(idx))) {
						out_connectedIdx = idx;						
						return true;
					}
				}
			}
		}
	}
	return false;
}


void updateConnectivityThread(const glm::ivec3& idxCenter, const glm::ivec3& connectivityRadius) {
	glm::ivec3 idxMinConnectivity, idxMaxConnectivity;

	// Update haptic type
	connectThread->hapticType = probeParams->hapticType;

	// Update bounds of next iteration of fill
	getBoxMinMaxIndex(idxCenter, (glm::ivec3)connectivityRadius, idxMinConnectivity, idxMaxConnectivity);
	connectThread->updateParameters(idxMinConnectivity, idxMaxConnectivity, isoParams->hapticsInValue);
	
	// Update seeds
	std::vector<glm::ivec3> seedIndices;
	seedIndices.push_back(idxCenter);
	connectThread->updateSeeds(seedIndices);

	if (connectThread->hapticType == HAPTIC_OBJECT_FOLLOWING) {
		glm::ivec3 idxConnected;
		glm::ivec3 prevMin = connectBack->origin();
		glm::ivec3 prevMax = connectBack->origin() + connectBack->dimensions();
		glm::ivec3 currMin = idxMinConnectivity;
		glm::ivec3 currMax = idxMaxConnectivity;

		if (isConnected(prevMin, prevMax, currMin, currMax, idxConnected)) {
			std::vector<glm::ivec3> seedIndices;
			seedIndices.push_back(idxConnected);
			connectThread->updateSeeds(seedIndices);		
		}
	}

	// Update size of probe
	connectBack->setOrigin(idxMinConnectivity);
	if (probeSize_prev != probeParams->probeSize) {		
		k_weight = glm::min(5.f / probeParams->probeSize, 1.f);	//scale weight by size
		updateConnectivityVolumeSize(idxMinConnectivity, connectivityRadius);
		connectThread->updateConnectivityBack();	// Update one iteration to keep haptics smooth
		probeSize_prev = probeParams->probeSize;
	}	
}

glm::ivec3 getPaintRadius() {
	return glm::ivec3(
		world->volume.getCellSize(
			probeParams->paintScale * probeParams->probeSize * world->worldScale));
}

void updatePaintVisualMinMax(const glm::ivec3& idxCenter) {

	// Update bounds to be rendered visually
	glm::ivec3  idxMinPaint, idxMaxPaint;
	char paintVisualMode = isoParams->paintVisualMode;

	// Show only region around probe
	if (paintVisualMode == 1) {
		glm::ivec3 paintRadius = getPaintRadius();
		getBoxMinMaxIndex(idxCenter, (glm::ivec3)paintRadius, idxMinPaint, idxMaxPaint);
	}

	// Show everything
	else if (paintVisualMode == 2){
		idxMinPaint = glm::ivec3(0, 0, 0);
		idxMaxPaint = world->volume.vData()->dimensions();
	}

	isoParams->paintIdxMinNormalized = (glm::vec3)idxMinPaint / (glm::vec3)world->volume.vData()->dimensions();
	isoParams->paintIdxMaxNormalized = (glm::vec3)idxMaxPaint / (glm::vec3)world->volume.vData()->dimensions();
}

void updatePaintThread(const glm::ivec3& idxCenter, const glm::ivec3& paintRadius) {	
	glm::ivec3  idxMinPaint, idxMaxPaint;

	// Update seeds
	//TODO: Add multiple seeds when paint type is flood fill?
	std::vector<glm::ivec3> seedIndices;
	seedIndices.push_back(idxCenter);	
	paintThread->updateSeeds(seedIndices);

	// Update paint type	
	paintThread->paintType = probeParams->paintType;
	paintThread->paintWorldRadius2 = std::pow(probeParams->paintScale * probeParams->probeSize * world->worldScale,2);
	paintThread->paintMaxOffset = isoParams->offsetInValue;
	paintThread->offsetType = probeParams->offsetType;
	paintThread->updatePaintOver(cGroups);


	// Update bounds of next iteration of fill
	getBoxMinMaxIndex(idxCenter, (glm::ivec3)paintRadius, idxMinPaint, idxMaxPaint);
	paintThread->updateParameters(idxMinPaint, idxMaxPaint, isoParams->paintInValue);
}
bool isPointBeyondNearClippingPlane(const glm::vec3& pointCS) {
	// Check if distance of point is further than near clipping plane
	//Note: Camera points in -Z direction in camera space
	if (-pointCS.z > visualParams->clipPlaneNear) {
		return true;
	}
	return false;
}

bool isPointBetweenCutPlanes(const glm::vec3& pointTS) {
	if (pointTS.x >= visualParams->cutMin.x &&
		pointTS.y >= visualParams->cutMin.y &&
		pointTS.z >= visualParams->cutMin.z &&
		pointTS.x <= visualParams->cutMax.x &&
		pointTS.y <= visualParams->cutMax.y &&
		pointTS.z <= visualParams->cutMax.z) {
		return true;
	}

	return false;
}

bool isPointInVisibleVolume(const glm::vec3& pointVS) {
	glm::vec3 pointCS = 
		world->cams[Cameras::CAM_MAIN].cam().getCSfromWS(
			world->volume.getWSfromVS(pointVS));
	glm::vec3 pointTS = world->volume.getTSfromVS(pointVS);

	bool beyondClipping = isPointBeyondNearClippingPlane(pointCS);
	bool betweenCutting = isPointBetweenCutPlanes(pointTS);
	return beyondClipping && betweenCutting;
}


float getHitRate() {
	VolumeBase* vData = world->volume.vData();
	glm::ivec3 idxVolume;
	size_t countValid = 0;
	for (auto const& pointVS : samplePointsVS){
		if (vData->getCell(pointVS, idxVolume)) {

			// Note1: connectFront->idxOrigin may change while getting hit rate.
			// As a result, there will be small inaccuracies from moving around.
			// By having a bigger connectivity volume, there is less likelihood of 
			// hitting an invalid index

			size_t iterIdx = connectFront->getIterIndex(idxVolume);	//unsigned int
			if (iterIdx < connectFront->size() &&		//valid index
				isPointInVisibleVolume(pointVS)) {
				if (connectFront->at(iterIdx) == true) {
					countValid++;
				}
			}
		}
	}

	return float(countValid) / float(samplePointsVS.size());
}

void surfaceSampling(const glm::vec3& probePosWS, glm::vec3& proxyPosWS) {	
	// Stop the proxy when a surface is found	
	if (!isHit) {
		float hitRate = getHitRate();
		if (hitRate > probeParams->hapticHitThreshold) {
			isHit = true; // Don't change position of proxy			
		}
		else {
			proxyPosWS = probePosWS;
		}
	}

	// Stop haptics when probe is moved toward the camera
	else {
		glm::vec3 cameraDirWS = glm::normalize(
			world->cams[Cameras::CAM_MAIN].cam().getWSfromCS(
				glm::vec4(world->camDirCS, 0)));
		float projection = glm::dot(glm::normalize((probePosWS - proxyPosWS)), cameraDirWS);
		if (projection < HAPTIC_SURFACE_SAMPLING_PROJECTION_THRESHOLD) {
			isHit = false;
 			proxyPosWS = probePosWS;
		}
	}	
}

void arithmeticMean(const glm::vec3& probePosWS, glm::vec3& proxyPosWS) {		
	glm::vec3 mean(0, 0, 0);	
	VolumeBase* vData = world->volume.vData();
	glm::ivec3 idxVolume;
	unsigned int count = 0;
	for (auto const& pointVS : samplePointsVS) {
		if (vData->getCell(pointVS, idxVolume)) {
			size_t iterIdx = connectFront->getIterIndex(idxVolume);	//unsigned int
			if (iterIdx < connectFront->size() &&		//valid index
				isPointInVisibleVolume(pointVS)) {
				if (connectFront->at(iterIdx) == true) {
					mean += pointVS;
					count++;
				}
			}
		}	
	}

	if (count > 0) {
		mean = mean / (float)count;		
		proxyPosWS = world->volume.getWSfromVS(mean);
	}

	else {
		proxyPosWS = probePosWS;
	}
	
}



void updateProxyPos(const glm::vec3& probePosWS, glm::vec3& proxyPosWS) {	
	if (connectThread->hapticType == HAPTIC_SURFACE_SAMPLING) {		
		surfaceSampling(probePosWS, proxyPosWS);
	}
	else if (connectThread->hapticType == HAPTIC_OBJECT_FOLLOWING ||
		connectThread->hapticType == HAPTIC_PROXIMITY_FOLLOWING) {
		arithmeticMean(probePosWS, proxyPosWS);
	}
}

void swapConnectivityVolumes() {
	connectFront->setOrigin(connectBack->origin());
	std::swap(connectFront, connectBack);
	
	connectThread->connectFront = connectFront;
	connectThread->connectBack = connectBack;
}


void updateForce(const glm::vec3& probePosWS, const glm::vec3& proxyPosWS) {
	chai3d::cVector3d force(0, 0, 0);
	chai3d::cVector3d torque(0, 0, 0);
	double gripperForce = 0.0;

	float ks = probeParams->hapticSpringForce;
	float kd = probeParams->hapticDampingForce;
	glm::vec3 forceWS = k_weight * (probePosWS - proxyPosWS);	//***This should be divided by radius of probe?
	glm::vec3 velocityWS = probePosWS - probePosWS_prev;

	force = -ks * chai3d::cVector3d(forceWS.z, forceWS.x, forceWS.y)
		- kd * chai3d::cVector3d(velocityWS.z, velocityWS.x, velocityWS.y);
	force /= probeParams->workspaceScale; //Reduce oscillation from larger movement of probe

	hapticDevice->setForceAndTorqueAndGripperForce(
		force,
		torque,
		gripperForce);
}


void Haptics::run() {
	using namespace chai3d;	
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);
}
void Haptics::stop() {
	using namespace chai3d;

	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }
}



void updateHaptics()
{
	using namespace chai3d;
	
	// Device output
	cVector3d position;
	cMatrix3d rotation;
	bool button0 = false;
	bool button1 = false;
	bool button0_prev = false;

	glm::vec3 probePosWS;
	glm::vec3 proxyPosWS;

	glm::mat4 invProbeM_stored;
	glm::mat4 volumeM_stored;
	simulationRunning = true;
	simulationFinished = false;
	isIdle = true;

	bool queueCommitPaintLayers = false;
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////
		// READ HAPTIC DEVICE
		/////////////////////////////////////////////////////////////////////
		button0_prev = button0;
		hapticDevice->getPosition(position);
		hapticDevice->getRotation(rotation);
		hapticDevice->getUserSwitch(0, button0);
		hapticDevice->getUserSwitch(1, button1);

		/////////////////////////////////////////////////////////////////////
		// UPDATE 3D CURSOR MODEL
		/////////////////////////////////////////////////////////////////////

		//==== Update position and orienation of cursor ====//		
		probePosWS_prev = probePosWS;
		probePosWS = glm::vec3(position.y(), position.z(), position.x());
		probePosWS *= workspaceScale;
		auto c0 = rotation.getCol1();
		auto c1 = rotation.getCol2();
		auto c2 = rotation.getCol0();

		glm::mat4 probeM = glm::mat4(
			c0.y(), c0.z(), c0.x(), 0.,
			c1.y(), c1.z(), c1.x(), 0.,
			c2.y(), c2.z(), c2.x(), 0.,
			probePosWS.x, probePosWS.y, probePosWS.z, 1.);		
		world->probe.updateModelM(probeM);		

		/////////////////////////////////////////////////////////////////////
		// INTERACT WITH VOLUME
		/////////////////////////////////////////////////////////////////////
		glm::vec3 pointVS = world->volume.getVSfromWS(probePosWS);
		glm::ivec3 idxCenter;
		world->volume.vData()->getCellExplicit(pointVS, idxCenter);

		if (isoParams->paintVisualMode > 0) {
			updatePaintVisualMinMax(idxCenter);
		}

		if (queueCommitPaintLayers && paintThread->simulationFinished) {
			if (world->volume.vState()->affectedOffset.size() > 0 ||
				world->volume.vState()->affectedPaintTotal.size() > 0) {
				float paintMaxOffset = paintThread->paintMaxOffset;
				if (paintThread->paintValue == 0) { paintMaxOffset = 0; }		//Eraser mode

				// Create a command group
				VolumeState* vState = world->volume.vState();
				auto cPaint = world->commandStack.getCommand_VolumePaint(vState, paintThread->paintValue);
				auto cOff = world->commandStack.getCommand_VolumeOffset(vState, paintMaxOffset, isoParams->tfInValue);
				auto cGroup = new Command_VolumeGroup(cOff, cPaint);
				world->commandStack.commit(cGroup);

				//Wait until previous update is complete
				while (world->volume.maskUpdateInfo->queueTextureMaskUpdate) {
					chai3d::cSleepMs(10);				
				}

				//Update in render
				world->volume.maskUpdateInfo->queueUpdate(
					world->volume.vState()->idxMin,
					world->volume.vState()->idxMax,
					world->volume.vMask(),
					world->volume.vData());
				world->volume.vState()->clearMinMax();
			}

			queueCommitPaintLayers = false;
		}

		if (!button0 && paintThread->simulationFinished) {
			//Update paint value when the button is no longer being pressed &
			// paint has been updated
			paintThread->paintValue = probeParams->selectedGroup;
		}
		
		if (button0) {
			if (paintThread->simulationFinished && 
				layersEnabled[paintThread->paintValue]) {				
				glm::ivec3 paintRadius = getPaintRadius();
				updatePaintThread(idxCenter, paintRadius);				
				paintThread->start();				
			}
		}
		else if (!button0&& button0_prev) {
			// Wait until paint thread finishes before committing
			queueCommitPaintLayers = true;
		}

		else if (button1) {
			if (isIdle) {
				// Store values for transforming volumeM (rotation + translation)
				invProbeM_stored = glm::inverse(probeM);
				volumeM_stored = world->volume.M();
				isIdle = false;
			}
			else {
				// Obtain difference in rotation and translation of probe
				glm::mat4 transformM = probeM * invProbeM_stored;

				// Apply difference to stored volumeM
				world->volume.updateM(transformM * volumeM_stored);				
			}

			proxyPosWS = probePosWS;
		}
		else {
			isIdle = true;			
		}

	
		/////////////////////////////////////////////////////////////////////
		// COMPUTE FORCES
		/////////////////////////////////////////////////////////////////////
		if (isIdle) {
			if (probeParams->hapticsEnabled) {
				if (connectThread->simulationFinished) {			
					glm::ivec3 cellRadius = glm::ivec3(world->volume.getCellSize(
						probeParams->probeSize * world->worldScale));
					updateConnectivityThread(idxCenter, cellRadius);
					swapConnectivityVolumes();				
					connectThread->start();
				}
				updateSamplePoints(probePosWS);
				updateProxyPos(probePosWS, proxyPosWS);				
			}
			else {
				proxyPosWS = probePosWS;
				isHit = false;
			}
		}

		/////////////////////////////////////////////////////////////////////
		// APPLY FORCES
		/////////////////////////////////////////////////////////////////////

		updateForce(probePosWS, proxyPosWS);
		world->proxy.updatePos(proxyPosWS);
		freqCounterHaptics.signal(1);
		// qDebug() << QString::number((int)freqCounterHaptics.getFrequency()) + " Hz";
	}

	// exit haptics thread
	simulationFinished = true;
}

void Haptics::closeHaptics()
{
	qDebug() << "closingHaptic";
	using namespace chai3d;
	stop();

	// closeHaptics haptic device
	hapticDevice->close();

	// delete resources
	delete hapticsThread;
	delete handler;
}
