#pragma once
#include "mathLib.h"
#include "volumes.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

struct Camera {

	//initialization
	Camera();
	~Camera();

	//Updates
	void updateM();
	void updateM(glm::mat4 viewMatrix);
	void update(glm::vec3 pos, glm::vec3 center, glm::vec3 up);
	float intersectDistance(glm::vec3 p_plane, glm::vec3 p_normal) {
		glm::vec3 c_dir = centerWorld - positionWorld;
		return -glm::dot((positionWorld - p_plane), p_normal) / glm::dot(p_normal, c_dir);
	}
	float intersectDistance(glm::vec3 p_plane) {
		//Plane is directly pointing towards the camera
		glm::vec3 c_dir = centerWorld - positionWorld;
		return intersectDistance(p_plane, -c_dir);
	}

	//helper functions
	const glm::mat4& V();
	const glm::mat4& invV();

	glm::vec3 getWSfromCS(const glm::vec3& pointCS);
	glm::vec3 getWSfromCS(const glm::vec4& pointCS);
	glm::vec3 getCSfromWS(const glm::vec3& pointWS);
	glm::vec3 getCSfromWS(const glm::vec4& pointWS);

private:	
	glm::vec3 positionWorld;	//where camera is
	glm::vec3 centerWorld;		//where camera is looking at
	glm::vec3 upWorld;			//some up direction

	glm::mat4 viewM;
	glm::mat4 invViewM;

	//Functions
	glm::mat4 lookAt();
};

struct CameraScreen {
	bool isOrtho;
	bool isObjectEnabled;
	bool isVolumeEnabled;

	CameraScreen(Camera& c);
	~CameraScreen();
	Camera& cam() { return *camera; }

	void updateScreenRatio(int screenWidth, int screenHeight);
	void updatePerspective(float fov);
	void updateOrtho(float distY);
	void updateOrtho(const glm::mat4& orthoMatrix);

	void updatePlaneNear(float p);
	void updatePlaneFar(float p);
	void updateClipNear(float c);
	void updateClipFar(float c);

	float clippingPlaneNear();
	float clippingPlaneFar();
	float nearPlane();
	float farPlane();

	const glm::mat4& P();
	const glm::mat4& invP();
	const glm::mat4& clippingP();
	const glm::mat4& clippingMNear();
	const glm::mat4& clippingMFar();

	glm::vec3 getNDCfromCS(const glm::vec3& pointCS);
	glm::vec3 getCSfromNDC(const glm::vec3& pointNDC);
	glm::vec3 getNDCfromWS(const glm::vec3& pointWS);
	glm::vec3 getWSfromNDC(const glm::vec3& pointNDC);
	

	float screenRatio();
	float fov();

	//Distance from center of screen to top of screen
	float screenHeightNearPerspective();
	float screenHeightFarPerspective();
	float screenHeightOrtho();

	//Distance from center of screen to right of screen
	float screenWidthNearPerspective();
	float screenWidthFarPerspective();
	float screenWidthOrtho();

private:	
	
	float fieldOfView;
	float orthoHeight;	// height of screen
	float aspectRatio;	// aspectRatio = width / height	
	Camera* camera;

	float planeNear;
	float planeFar;
	float clipNear;
	float clipFar;

	glm::mat4 projectionM;
	glm::mat4 invProjectionM;

	glm::mat4 clipModelMNear;
	glm::mat4 clipModelMFar;
	glm::mat4 clipProjectionM;

	//Functions
	glm::mat4 perspective();
	glm::mat4 perspectiveClip();

	glm::mat4 ortho();
	glm::mat4 orthoClip();

	glm::mat4 clippingM(float screenWidth, float screenHeight, float clippingPlaneDist);
	glm::mat4 clippingMNearOrtho();
	glm::mat4 clippingMNearPerspective();
	glm::mat4 clippingMFarOrtho();
	glm::mat4 clippingMFarPerspective();
};

struct Frustum {
	enum F_Colors {
		C_NEAR,
		C_FAR,
		SIDE0,
		SIDE1,
		SIDE2,
		SIDE3,
	};

	std::vector<glm::vec3> color{
		glm::vec3(0.9,0.9,0.9),		//clipping near plane
		glm::vec3(0.3,0.3,0.3),		//clipping far plane
		glm::vec3(1,0,0),			//side 0
		glm::vec3(0,1,0),			//side 1
		glm::vec3(1,1,0),			//side 2
		glm::vec3(0,1,1),			//side 3
	};

	std::vector<glm::vec3> v;	//vertices
	std::vector<glm::ivec2> e;	//edges
	std::vector<char> e_color;		//edge colors
	std::vector<glm::ivec3> f;	//faces

	std::vector<glm::vec3> v_near;
	std::vector<glm::vec3> c_near;

	std::vector<glm::vec3> v_far;
	std::vector<glm::vec3> c_far;

	Frustum();
	glm::vec3 getMidPointBox();
	void init();
	void updateVertices(CameraScreen& camScreen);	
	void updateLines(Camera& cam);
	std::vector<glm::vec3> getLines();
	std::vector<glm::vec3> getLinesColor();
	std::vector<glm::vec3> getBox();
};


struct Cameras
{

	enum ScreenType {
		SCR_FULL,
		SCR_STEREO,
		SCR_OVERVIEW
	};

	enum ScreenSection {
		SEC_FULL,
		SEC_LEFT,
		SEC_RIGHT,
		SEC_TOP_LEFT,
		SEC_TOP_RIGHT,
		SEC_BOT_LEFT,
		SEC_BOT_RIGHT,
		SEC_COUNT
	};

	enum CameraLocation {
		CAM_MAIN,
		CAM_STEREO_LEFT,
		CAM_STEREO_RIGHT,
		CAM_OVERVIEW_TOP,
		CAM_OVERVIEW_FRONT,
		CAM_OVERVIEW_SIDE,
		CAM_COUNT
	};

	unsigned char screenType;
	bool isArrowEnabled;
	bool isHudEnabled;
	bool isLineEnabled;
	bool isProxyEnabled;	
	bool isForceLineEnabled;

	Cameras();
	~Cameras();
	CameraScreen& operator[](unsigned short camLocation);
	unsigned int size();

	void initSections();
	void init(Volume& volume);
	void initMainCamera();
	void initStereoCameras();
	void initOverviewCamera(Volume& volume);
	void updateOverviewCameras(Volume& volume);
	float getScreenRatio();
	
	glm::vec2 getSectionMin(unsigned short screenSection);
	glm::vec2 getSectionMax(unsigned short screenSection);

	bool isPointInSection(const glm::vec3& pointNDC, unsigned short screenSection);
	short getCameraLocation(const glm::vec3& pointNDC);

	bool getObjectEnabled();
	void setObjectEnabled(bool b);

	bool getVolumeEnabled();
	void setVolumeEnabled(bool b);

	void updatePlaneNear(float planeNear);
	void updatePlaneFar(float planeFar);
	void updateClipNear(float clipPlaneNear);
	void updateClipFar(float clipPlaneFar);
	void updatePerspective(float fov);

	unsigned char getScreenType() {
		return screenType;
	}
	void updateScreenType(unsigned char type) {		
		screenType = type;		//0 = full, 1 = stereo, 2 = overview
	}

	void importData(const VisualParams* param) {		
		// Parameter updates
		updateScreenType(param->screenType);		//0 = full, 1 = stereo, 2 = overview
		isHudEnabled = param->hudEnabled;
		isProxyEnabled = param->proxyEnabled;
		isLineEnabled = param->lineEnabled;
		isArrowEnabled = param->arrowEnabled;
		isForceLineEnabled = param->forceLineEnabled;
		setVolumeEnabled(param->volumeEnabled);
		setObjectEnabled(param->objectEnabled);

		updatePlaneNear(param->nearPlane);
		updatePlaneFar(param->farPlane);

		// Matrix update
		if (param->fov != camScreens[Cameras::CAM_MAIN].fov()) {
			updatePerspective(param->fov);
		}
		if (param->clipPlaneNear != camScreens[Cameras::CAM_MAIN].clippingPlaneNear()) {
			updateClipNear(param->clipPlaneNear);
		}
		if (param->clipPlaneFar != camScreens[Cameras::CAM_MAIN].clippingPlaneFar()) {
			updateClipFar(param->clipPlaneFar);
		}
	}

	void resetClippingPlanes(std::vector<float> dists) {
		//Des: Given a set of distances, set the near CP to the closest point of volume box, and
		// far CP to the furthest point in volume box

		//Find the closest and furthest points in array
		float z_min = 99999999;
		float z_max = -99999999;
		for (auto d : dists) {
			if (d > z_max) { z_max = d; }
			if (d < z_min) { z_min = d; }
		}

		//Clamp the results to near and far plane (otherwise, the camera would break)
		float nearPlane = camScreens[Cameras::CAM_MAIN].nearPlane();
		float farPlane = camScreens[Cameras::CAM_MAIN].farPlane();
		z_min = glm::clamp(z_min, nearPlane, farPlane);
		z_max = glm::clamp(z_max, nearPlane, farPlane);

		//Apply changes
		updateClipNear(z_min);
		updateClipFar(z_max);
	}


private:	
	float z;
	std::vector<CameraScreen> camScreens;
	std::vector<Camera> cameras;
	std::vector<glm::vec2> sectionMin;
	std::vector<glm::vec2> sectionMax;

	glm::mat4 topOverviewM;
	glm::mat4 frontOverviewM;
	glm::mat4 sideOverviewM;
	glm::vec3 pointNDC_prev;
};