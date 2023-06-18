#include "Cameras.h"

//initialization
Camera::Camera() {
	positionWorld = glm::vec3(0, 0, -1);
	centerWorld = glm::vec3(0, 0, 0);
	upWorld = glm::vec3(0, 1, 0);
	updateM();
}

Camera::~Camera() {

}

//Updates
void Camera::updateM() {
	viewM = lookAt();
	invViewM = glm::inverse(viewM);
}

void Camera::updateM(glm::mat4 viewMatrix) {
	viewM = viewMatrix;
	invViewM = glm::inverse(viewM);
}

void Camera::update(glm::vec3 pos, glm::vec3 center, glm::vec3 up) {
	positionWorld = pos;
	centerWorld = center;
	upWorld = up;

	updateM();
}

//helper functions
const glm::mat4& Camera::V() { return viewM; }
const glm::mat4& Camera::invV() { return invViewM; }

glm::vec3 Camera::getWSfromCS(const glm::vec3& pointCS) {
	return getWSfromCS(glm::vec4(pointCS, 1));
}
glm::vec3 Camera::getWSfromCS(const glm::vec4& pointCS) {
	return glm::vec3(invViewM * pointCS);
}

glm::vec3 Camera::getCSfromWS(const glm::vec3& pointWS) {
	return getCSfromWS(glm::vec4(pointWS, 1));
}
glm::vec3 Camera::getCSfromWS(const glm::vec4& pointWS) {
	return glm::vec3(viewM * pointWS);
}

glm::mat4 Camera::lookAt() {
	return glm::lookAt(positionWorld, centerWorld, upWorld);
}

//===================


CameraScreen::CameraScreen(Camera& c) : camera(&c) {
	isOrtho = false;
	isVolumeEnabled = true;
	isObjectEnabled = true;	
	aspectRatio = 1;
	fieldOfView = 0;
	orthoHeight = 1;

	planeNear = 0.01;			//*** Update from settings file
	planeFar = 100;				//*** Update from settings file
	updateClipNear(planeNear + 0.01);
	updateClipFar(planeFar - 0.01);
}
CameraScreen::~CameraScreen() {

}


void CameraScreen::updateScreenRatio(int screenWidth, int screenHeight) {
	aspectRatio = (float)screenWidth / (float)screenHeight;
}

void CameraScreen::updatePerspective(float fov) {
	isOrtho = false;

	//Update parameters
	fieldOfView = fov;

	//Update matrices
	projectionM = perspective();
	invProjectionM = glm::inverse(projectionM);
	clipProjectionM = perspectiveClip();
	clipModelMNear = clippingMNearPerspective();
	clipModelMFar = clippingMFarPerspective();
}

void CameraScreen::updateOrtho(float distY) {
	isOrtho = true;

	//Update parameters
	orthoHeight = distY;	//Note: DistY is usually: (longest length of box / 2)1

	//Update matrices
	projectionM = ortho();
	invProjectionM = glm::inverse(projectionM);
	clipProjectionM = orthoClip();
	clipModelMNear = clippingMNearOrtho();
	clipModelMFar = clippingMFarOrtho();
}

void CameraScreen::updateOrtho(const glm::mat4& orthoMatrix) {
	isOrtho = true;

	//Update parameters
	orthoHeight = -1;	//not guaranteed to be centered at middle

	//Update matrices
	projectionM = orthoMatrix;
	invProjectionM = glm::inverse(projectionM);
}

void CameraScreen::updatePlaneNear(float p) {
	planeNear = p;
}

void CameraScreen::updatePlaneFar(float p) {
	planeFar = p;
}

void CameraScreen::updateClipNear(float c) {
	clipNear = c;
	clipProjectionM = perspectiveClip();
	if (isOrtho) { clipModelMNear = clippingMNearOrtho(); }
	else { clipModelMNear = clippingMNearPerspective(); }
}

void CameraScreen::updateClipFar(float c) {
	clipFar = c;
	if (isOrtho) { clipModelMFar = clippingMFarOrtho(); }
	else { clipModelMFar = clippingMFarPerspective(); }
}

float CameraScreen::clippingPlaneNear() { return clipNear; }
float CameraScreen::clippingPlaneFar() { return clipFar; }
float CameraScreen::nearPlane() { return planeNear; }
float CameraScreen::farPlane() { return planeFar; }

const glm::mat4& CameraScreen::P() { return projectionM; }
const glm::mat4& CameraScreen::invP() { return invProjectionM; }
const glm::mat4& CameraScreen::clippingP() { return clipProjectionM; }
const glm::mat4& CameraScreen::clippingMNear() { return clipModelMNear; }
const glm::mat4& CameraScreen::clippingMFar() { return clipModelMFar; }

glm::vec3 CameraScreen::getNDCfromCS(const glm::vec3& pointCS) {
	glm::vec4 p = projectionM * glm::vec4(pointCS, 1);
	return glm::vec3(p.x, p.y, p.z) / p.w;
}
glm::vec3 CameraScreen::getCSfromNDC(const glm::vec3& pointNDC) {
	return glm::vec3(invProjectionM * glm::vec4(pointNDC, 1));
}
glm::vec3 CameraScreen::getNDCfromWS(const glm::vec3& pointWS) {
	return getNDCfromCS(camera->getCSfromWS(pointWS));
}
glm::vec3 CameraScreen::getWSfromNDC(const glm::vec3& pointNDC) {
	return camera->getWSfromCS(getCSfromNDC(pointNDC));
}

float CameraScreen::screenRatio() { return aspectRatio; }
float CameraScreen::fov() { return fieldOfView; }

//Distance from center of screen to top of screen
float CameraScreen::screenHeightNearPerspective() {
	return std::tan(MathLib::degreesToRadians(fieldOfView / 2.)) * clipNear;
}
float CameraScreen::screenHeightFarPerspective() {
	return std::tan(MathLib::degreesToRadians(fieldOfView / 2.)) * clipFar;
}
float CameraScreen::screenHeightOrtho() {
	return orthoHeight;		//*** Normalized device coordinate?
}

//Distance from center of screen to right of screen
float CameraScreen::screenWidthNearPerspective() {
	return screenHeightNearPerspective() * aspectRatio;
}
float CameraScreen::screenWidthFarPerspective() {
	return screenHeightFarPerspective() * aspectRatio;
}
float CameraScreen::screenWidthOrtho() {
	//Distance from center of screen to right of screen
	return screenHeightOrtho() * aspectRatio;
}


//Functions
glm::mat4 CameraScreen::perspective() {
	return glm::perspective(fieldOfView, aspectRatio, planeNear, planeFar);
}

glm::mat4 CameraScreen::perspectiveClip() {
	return glm::perspective(fieldOfView, aspectRatio, clipNear, planeFar);
}

glm::mat4 CameraScreen::ortho() {
	float distY = screenHeightOrtho();
	float distX = screenWidthOrtho();
	return glm::ortho(-distX, distX, -distY, distY, planeNear, planeFar);
}

glm::mat4 CameraScreen::orthoClip() {
	float distY = screenHeightOrtho();
	float distX = screenWidthOrtho();
	return glm::ortho(-distX, distX, -distY, distY, clipNear, planeFar);
}


glm::mat4 CameraScreen::clippingM(float screenWidth, float screenHeight, float clippingPlaneDist) {
	//Scale and transform of clipping plane from screen space
	glm::mat4 clippingM =
		glm::translate(0.f, 0.f, -clippingPlaneDist) *	//Move plane to clipped region of volume
		glm::scale(screenWidth, screenHeight, 0.f);		//Scale the plane to be as big as screen

	return clippingM;
}

glm::mat4 CameraScreen::clippingMNearOrtho() {
	float screenWidth = screenWidthOrtho();
	float screenHeight = screenHeightOrtho();
	return clippingM(screenWidth, screenHeight, clipNear);
}

glm::mat4 CameraScreen::clippingMNearPerspective() {
	float screenWidth = screenWidthNearPerspective();
	float screenHeight = screenHeightNearPerspective();
	return clippingM(screenWidth, screenHeight, clipNear);
}

glm::mat4 CameraScreen::clippingMFarOrtho() {
	float screenWidth = screenWidthOrtho();
	float screenHeight = screenHeightOrtho();
	return clippingM(screenWidth, screenHeight, clipFar);
}

glm::mat4 CameraScreen::clippingMFarPerspective() {
	float screenWidth = screenWidthFarPerspective();
	float screenHeight = screenHeightFarPerspective();
	return clippingM(screenWidth, screenHeight, clipFar);
}

//====

//=============

Frustum::Frustum() {
	v.resize(8);
	e.resize(12);
	e_color.resize(12);
	f.resize(12); //6 faces * 2 triangles per face

	//==== Edges ====//
	//Side lines
	e[0] = glm::ivec2(0, 1);
	e[1] = glm::ivec2(2, 3);
	e[2] = glm::ivec2(4, 5);
	e[3] = glm::ivec2(6, 7);
	e_color[0] = F_Colors::SIDE0;
	e_color[1] = F_Colors::SIDE1;
	e_color[2] = F_Colors::SIDE2;
	e_color[3] = F_Colors::SIDE3;

	// clipping near plane
	e[4] = glm::ivec2(0, 2);
	e[5] = glm::ivec2(2, 6);
	e[6] = glm::ivec2(6, 4);
	e[7] = glm::ivec2(4, 0);
	e_color[4] = F_Colors::C_NEAR;
	e_color[5] = F_Colors::C_NEAR;
	e_color[6] = F_Colors::C_NEAR;
	e_color[7] = F_Colors::C_NEAR;

	//clipping far plane
	e[8] = glm::ivec2(1, 3);
	e[9] = glm::ivec2(3, 7);
	e[10] = glm::ivec2(7, 5);
	e[11] = glm::ivec2(5, 1);
	e_color[8] = F_Colors::C_FAR;
	e_color[9] = F_Colors::C_FAR;
	e_color[10] = F_Colors::C_FAR;
	e_color[11] = F_Colors::C_FAR;

	//==== Faces ====//
	f[0] = glm::ivec3(0, 1, 2);
	f[1] = glm::ivec3(3, 2, 1);
	f[2] = glm::ivec3(2, 3, 6);
	f[3] = glm::ivec3(7, 6, 3);
	f[4] = glm::ivec3(1, 3, 5);
	f[5] = glm::ivec3(7, 5, 3);
	f[6] = glm::ivec3(0, 1, 4);
	f[7] = glm::ivec3(5, 4, 1);
	f[8] = glm::ivec3(4, 5, 6);
	f[9] = glm::ivec3(7, 6, 5);
	f[10] = glm::ivec3(0, 2, 4);
	f[11] = glm::ivec3(6, 4, 2);

	//==== Vertices ====//
	v_near.resize(12);
	c_near.resize(12);
	v_far.resize(12);
	c_far.resize(12);
}

glm::vec3 Frustum::getMidPointBox() {
	glm::vec3 sum(0, 0, 0);
	for (int i = 0; i < v.size(); i++) {
		sum += v[i];
	}
	return sum /= (float)v.size();
}

void Frustum::updateVertices(CameraScreen& camScreen) {
	float distNearX, distNearY;
	float distFarX, distFarY;

	// Height and width of screen in camera space
	if (!camScreen.isOrtho) {
		distNearY = camScreen.screenHeightNearPerspective();
		distNearX = camScreen.screenWidthNearPerspective();
		distFarY = camScreen.screenHeightFarPerspective();
		distFarX = camScreen.screenWidthFarPerspective();
	}
	else {
		distNearY = camScreen.screenHeightOrtho();
		distNearX = camScreen.screenWidthOrtho();
		distFarY = distNearY;
		distFarX = distNearX;
	}

	// Distance to clipping plane from camera in camera space
	float dist_nearPlane = -camScreen.clippingPlaneNear();
	float dist_farPlane = -camScreen.clippingPlaneFar();

	// Obtain vertices of frustum from camera space 
	v[0] = glm::vec3(distNearX, distNearY, dist_nearPlane);
	v[1] = glm::vec3(distFarX, distFarY, dist_farPlane);
	v[2] = glm::vec3(distNearX, -distNearY, dist_nearPlane);
	v[3] = glm::vec3(distFarX, -distFarY, dist_farPlane);
	v[4] = glm::vec3(-distNearX, distNearY, dist_nearPlane);
	v[5] = glm::vec3(-distFarX, distFarY, dist_farPlane);
	v[6] = glm::vec3(-distNearX, -distNearY, dist_nearPlane);
	v[7] = glm::vec3(-distFarX, -distFarY, dist_farPlane);

	// Convert camera space to model space (overview looks from model coordinates)
	Camera& cam = camScreen.cam();
	for (int i = 0; i < v.size(); i++) {
		v[i] = cam.getWSfromCS(v[i]);
	}
}

void Frustum::updateLines(Camera& cam) {
	// Clear all existing information about proximity of lines
	v_near.clear();
	v_far.clear();
	c_near.clear();
	c_far.clear();
		
	glm::vec3 midFrus = getMidPointBox();	//Midpoint of frustum box (World space)
	glm::vec3 dirCamPosFromFrusMid = cam.getWSfromCS(-midFrus);	//Direction from camera to frustum
	glm::vec3 p0, p1, midLine, dirLineMidFromFrusMid;
	for (int i = 0; i < e.size(); i++) {
		p0 = v[e[i].x];
		p1 = v[e[i].y];
		midLine = (p0 + p1) / 2.f;			// Midpoint of line (World space)
		dirLineMidFromFrusMid = midFrus - midLine;	// direction from midpoint of line to midpoint of frustum

		// dir are opposite := the line is nearer to camera
		if (glm::dot(dirCamPosFromFrusMid, dirLineMidFromFrusMid) <= 0) {
			v_near.push_back(p0);
			v_near.push_back(p1);
			c_near.push_back(color[e_color[i]]);
			c_near.push_back(color[e_color[i]]);
		}

		// dir are similar := the line is further from camera
		else {
			v_far.push_back(p0);
			v_far.push_back(p1);
			c_far.push_back(color[e_color[i]]);
			c_far.push_back(color[e_color[i]]);
		}
	}
}

std::vector<glm::vec3> Frustum::getLines() {
	std::vector<glm::vec3> out;
	for (int i = 0; i < e.size(); i++) {
		out.push_back(v[e[i].x]);
		out.push_back(v[e[i].y]);
	}
	return out;
}

std::vector<glm::vec3> Frustum::getLinesColor() {
	std::vector<glm::vec3> out;
	for (int i = 0; i < e.size(); i++) {
		out.push_back(color[e_color[i]]);
		out.push_back(color[e_color[i]]);
	}
	return out;
}

std::vector<glm::vec3> Frustum::getBox() {
	std::vector<glm::vec3> out;
	for (int i = 0; i < f.size(); i++) {
		out.push_back(v[f[i].x]);
		out.push_back(v[f[i].y]);
		out.push_back(v[f[i].z]);
	}
	return out;
}



//=============

Cameras::Cameras()
{
	// Intialize cameras and its screens
	cameras.resize(CAM_COUNT);
	for (int i = 0; i < cameras.size(); i++) {
		camScreens.push_back(CameraScreen(cameras[i]));
	}

	initSections();
	screenType = Cameras::SCR_FULL;
	z = 0.1;

	isArrowEnabled = false;
	isHudEnabled = false;
	isLineEnabled = false;
	isProxyEnabled = false;
	isForceLineEnabled = false;
}

Cameras::~Cameras()
{
}

CameraScreen& Cameras::operator[] (unsigned short camLocation) {
	return camScreens[camLocation];
}

unsigned int Cameras::size() {
	return camScreens.size();
}

void Cameras::initSections() {
	sectionMin.resize(SEC_COUNT);
	sectionMax.resize(SEC_COUNT);

	sectionMin[SEC_FULL] = glm::vec2(-1, -1);
	sectionMax[SEC_FULL] = glm::vec2(1, 1);

	sectionMin[SEC_LEFT] = glm::vec2(-1, -1);
	sectionMax[SEC_LEFT] = glm::vec2(0, 1);

	sectionMin[SEC_RIGHT] = glm::vec2(0, -1);
	sectionMax[SEC_RIGHT] = glm::vec2(1, 1);

	sectionMin[SEC_TOP_LEFT] = glm::vec2(-1, 0);
	sectionMax[SEC_TOP_LEFT] = glm::vec2(0, 1);

	sectionMin[SEC_TOP_RIGHT] = glm::vec2(0, 0);
	sectionMax[SEC_TOP_RIGHT] = glm::vec2(1, 1);

	sectionMin[SEC_BOT_LEFT] = glm::vec2(-1, -1);
	sectionMax[SEC_BOT_LEFT] = glm::vec2(0, 0);

	sectionMin[SEC_BOT_RIGHT] = glm::vec2(0, -1);
	sectionMax[SEC_BOT_RIGHT] = glm::vec2(1, 0);
}

void Cameras::init(Volume& volume) {
	// Initialize Camera positions	
	initMainCamera();
	initStereoCameras();

	// Update Overview cameras
	initOverviewCamera(volume);
}

void Cameras::initMainCamera() {
	cameras[CAM_MAIN].update(
		glm::vec3(0, 0, z),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0));
}

void Cameras::initStereoCameras() {
	float eye_gap = 0.0015;
	glm::vec3 posLeft = glm::vec3(-eye_gap, 0, z);
	glm::vec3 posRight = glm::vec3(eye_gap, 0, z);

	glm::vec3 dirLeft = glm::normalize(-posLeft);
	glm::vec3 dirRight = glm::normalize(-posRight);

	glm::vec3 up = glm::vec3(0, 1, 0);

	cameras[CAM_STEREO_LEFT].update(
		posLeft,
		posLeft + dirLeft,
		up);
	cameras[CAM_STEREO_RIGHT].update(
		posRight,
		posRight + dirRight,
		up);
}
void Cameras::initOverviewCamera(Volume& volume) 
{
	// Rendering modes
	camScreens[CAM_OVERVIEW_TOP].isOrtho = true;
	camScreens[CAM_OVERVIEW_FRONT].isOrtho = true;
	camScreens[CAM_OVERVIEW_SIDE].isOrtho = true;

	camScreens[CAM_OVERVIEW_TOP].isObjectEnabled = false;
	camScreens[CAM_OVERVIEW_FRONT].isObjectEnabled = false;
	camScreens[CAM_OVERVIEW_SIDE].isObjectEnabled = false;
			
	// Set distance of camera well outside of volume (prevents clipping of frustum)
	float z_overview =  z * 100;
	float distX = volume.vData()->boxWidth() + z_overview;
	float distY = volume.vData()->boxHeight() + z_overview;
	float distZ = volume.vData()->boxDepth() + z_overview;

	// Move clipping plane to near the faces of volume
	camScreens[CAM_OVERVIEW_TOP].updateClipNear(z_overview);
	camScreens[CAM_OVERVIEW_FRONT].updateClipNear(z_overview);
	camScreens[CAM_OVERVIEW_SIDE].updateClipNear(z_overview);

	// Transform to directions of perpendicular basis
	float theta = MathLib::degreesToRadians(-90);
	glm::mat4 rotateM = glm::mat4(1.0);
	topOverviewM = glm::lookAt(
		glm::vec3(rotateM * glm::vec4(0, 0, distX, 1)),
		glm::vec3(rotateM * glm::vec4(0, 0, 0, 1)),
		glm::vec3(rotateM * glm::vec4(0, 1, 0, 1)));
	
	rotateM = MathLib::rotateXAxis(theta);
	frontOverviewM = glm::lookAt(
		glm::vec3(rotateM * glm::vec4(0, 0, distY, 1)),
		glm::vec3(rotateM * glm::vec4(0, 0, 0, 1)),
		glm::vec3(rotateM * glm::vec4(0, 1, 0, 1)));
	
	rotateM = rotateM * MathLib::rotateYAxis(theta);
	sideOverviewM = glm::lookAt(
		glm::vec3(rotateM * glm::vec4(0, 0, distZ, 1)),
		glm::vec3(rotateM * glm::vec4(0, 0, 0, 1)),
		glm::vec3(rotateM * glm::vec4(0, 1, 0, 1)));

	// Update the new position and rotation of cameras
	updateOverviewCameras(volume);
}

void Cameras::updateOverviewCameras(Volume& volume) {
	cameras[CAM_OVERVIEW_TOP].updateM(topOverviewM * volume.invM());
	cameras[CAM_OVERVIEW_FRONT].updateM(frontOverviewM * volume.invM());
	cameras[CAM_OVERVIEW_SIDE].updateM(sideOverviewM * volume.invM());
}

float Cameras::getScreenRatio() {
	//All the screens have the same ratio in overview and full screen view
	//Note: (stereo view has different screen ratio, but I won't worry about that for now)
	return camScreens[CAM_MAIN].screenRatio();
}

glm::vec2 Cameras::getSectionMin(unsigned short screenSection) {
	return sectionMin[screenSection];
}

glm::vec2 Cameras::getSectionMax(unsigned short screenSection) {
	return sectionMax[screenSection];
}

bool Cameras::isPointInSection(const glm::vec3& pointNDC, unsigned short screenSection) {
	glm::vec2 coordMin = sectionMin[screenSection];
	glm::vec2 coordMax = sectionMax[screenSection];

	if (pointNDC.x > coordMin.x &&
		pointNDC.x < coordMax.x &&
		pointNDC.y > coordMin.y &&
		pointNDC.y < coordMax.y) {
		return true;
	}
	return false;
}

short Cameras::getCameraLocation(const glm::vec3& pointNDC) {
	if (screenType == SCR_FULL) {
		return CAM_MAIN;
	}
	else if (screenType == SCR_STEREO) {
		if (isPointInSection(pointNDC, SEC_LEFT)) { return CAM_STEREO_LEFT; }
		else if (isPointInSection(pointNDC, SEC_RIGHT)) { return CAM_STEREO_RIGHT; }
	}
	else if (screenType == SCR_OVERVIEW) {
		if (isPointInSection(pointNDC, SEC_TOP_LEFT)) { return CAM_OVERVIEW_TOP; }
		else if (isPointInSection(pointNDC, SEC_TOP_RIGHT)) { return CAM_OVERVIEW_SIDE; }
		else if (isPointInSection(pointNDC, SEC_BOT_LEFT)) { return CAM_OVERVIEW_FRONT; }
		else if (isPointInSection(pointNDC, SEC_BOT_RIGHT)) { return CAM_MAIN; }
	}
	return -1;
}

bool Cameras::getObjectEnabled() {
	return camScreens[Cameras::CAM_MAIN].isObjectEnabled;
}
void Cameras::setObjectEnabled(bool b) {
	camScreens[Cameras::CAM_MAIN].isObjectEnabled = b;
	camScreens[Cameras::CAM_STEREO_LEFT].isObjectEnabled = b;
	camScreens[Cameras::CAM_STEREO_RIGHT].isObjectEnabled = b;
}

bool Cameras::getVolumeEnabled() {
	return camScreens[Cameras::CAM_MAIN].isVolumeEnabled;
}
void Cameras::setVolumeEnabled(bool b) {
	camScreens[Cameras::CAM_MAIN].isVolumeEnabled = b;
	camScreens[Cameras::CAM_STEREO_LEFT].isVolumeEnabled = b;
	camScreens[Cameras::CAM_STEREO_RIGHT].isVolumeEnabled = b;
}
void Cameras::updatePlaneNear(float planeNear) {
	camScreens[Cameras::CAM_MAIN].updatePlaneNear(planeNear);
	camScreens[Cameras::CAM_STEREO_LEFT].updatePlaneNear(planeNear);
	camScreens[Cameras::CAM_STEREO_RIGHT].updatePlaneNear(planeNear);
}
void Cameras::updatePlaneFar(float planeFar) {
	camScreens[Cameras::CAM_MAIN].updatePlaneFar(planeFar);
	camScreens[Cameras::CAM_STEREO_LEFT].updatePlaneFar(planeFar);
	camScreens[Cameras::CAM_STEREO_RIGHT].updatePlaneFar(planeFar);

	
}
void Cameras::updateClipNear(float clipPlaneNear) {
	camScreens[Cameras::CAM_MAIN].updateClipNear(clipPlaneNear);
	camScreens[Cameras::CAM_STEREO_LEFT].updateClipNear(clipPlaneNear);
	camScreens[Cameras::CAM_STEREO_RIGHT].updateClipNear(clipPlaneNear);
}
void Cameras::updateClipFar(float clipPlaneFar) {
	camScreens[Cameras::CAM_MAIN].updateClipFar(clipPlaneFar);
	camScreens[Cameras::CAM_STEREO_LEFT].updateClipFar(clipPlaneFar);
	camScreens[Cameras::CAM_STEREO_RIGHT].updateClipFar(clipPlaneFar);
}
void Cameras::updatePerspective(float fov) {
	camScreens[Cameras::CAM_MAIN].updatePerspective(fov);
	camScreens[Cameras::CAM_STEREO_LEFT].updatePerspective(fov);
	camScreens[Cameras::CAM_STEREO_RIGHT].updatePerspective(fov);
}