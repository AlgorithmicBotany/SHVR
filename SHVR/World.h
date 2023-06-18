#pragma once

#define _USE_MATH_DEFINES
#include <math.h>			//M_PI
#include "volumes.h"
#include "Cameras.h"
#include "NodeNetwork.h"
#include "Commands.h"
#include "mathLib.h"
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

enum HapticType {
	HAPTIC_SURFACE_SAMPLING,
	HAPTIC_OBJECT_FOLLOWING,
	HAPTIC_PROXIMITY_FOLLOWING
};

enum PaintType {
	FILL,
	FLOOD_FILL,
	SPECKLES,
	PAINT_COUNT
};

enum OffsetType {
	DIST2,
	UNIFORM, 
	OFFSET_COUNT
};

struct Point {
	Point();
	void updateModelM(glm::mat4& m);
	void updatePos(glm::vec3& v);
	void updateScaleM(float s);
	void updateSilhouetteScaleM(float s);
	
	const glm::vec3 pos();	//world position
	float scale();
	float silhouetteScale();
	const glm::mat4 M();
	const glm::mat4 M_silhouette();
		
private:	
	glm::mat4 modelM;	//translate & rotate
	glm::mat4 scaleM;	//translate & rotate & scale
	glm::mat4 silhouetteScaleM;	
};

struct World {
	NN::NNState nn;
	Volume volume;
	Point probe;
	Point proxy;
	Cameras cams;
	CommandStack commandStack;
	glm::vec3 lightCS;
	glm::vec3 camDirCS;

	float worldScale;

	World() : worldScale(0) {
		camDirCS = glm::vec3(0, 0, -1);
		lightCS = glm::vec3(5, 5, 5);
	}
	~World() {}	

	void initBasedOnConfig(Assign2::InputConfiguration& configFile) {
		worldScale = configFile.sceneScale;
		volume.importData(configFile);
		cams.init(volume);
	}

	//Matrices
	glm::mat4 getClippingMNear(CameraScreen& camScreen) { return camScreen.cam().invV() * camScreen.clippingMNear(); }
	glm::mat4 getClippingMFar(CameraScreen& camScreen) { return camScreen.cam().invV() * camScreen.clippingMFar(); }

	glm::mat4 getMVP(CameraScreen& camScreen) { return camScreen.P() * camScreen.cam().V() * volume.M(); }
	glm::mat4 getMVP(CameraScreen& camScreen, const glm::mat4& M) { return camScreen.P() * camScreen.cam().V() * M; }
	glm::mat4 getVP(CameraScreen& camScreen) { return camScreen.P() * camScreen.cam().V(); }
	glm::mat4 getMV(CameraScreen& camScreen, const glm::mat4& M) { return camScreen.cam().V() * M; }

	glm::mat4 getMVP_rayStartPos(CameraScreen& camScreen) { return camScreen.clippingP() * camScreen.cam().V() * volume.M(); }
	glm::mat4 getMVP_nearClipping(CameraScreen& camScreen) { return camScreen.P() * camScreen.clippingMNear(); }	//Note: ViewM is cancelled out
	glm::mat4 getMVP_farClipping(CameraScreen& camScreen) { return camScreen.P() * camScreen.clippingMFar(); }		//Note: ViewM is cancelled out

	//Terms: 
	// TS = texture space; volume coordinate is [0,1]^3
	// VS = volume space; volume coordinate is [0, boxWidth], [0, boxHeight], [0, boxDepth]
	// MS = model space; volume is centered at (0,0,0), where range of volume is [-w/2,w/2],[-h/2,h/2],[-d/2,d/2]
	// WS = world space; world is centered at (0,0,0), which is default coordinate space of haptic device
	// CS = camera space; centered at camera postion at (0,0,0), with camera facing -z direction
	// NDC = normalized device coordinate; [-1,1]^2 for screen width and height 
	// SS = screen space [0, pixelWidth], [0, pixelHeight]	

	void rotateView(float angleRadians, const glm::vec3& rotateAxis, CameraScreen& camScreen) {
		// Rotate centered around camera position
		glm::mat4 rotateM = glm::rotate(angleRadians, rotateAxis);		
		glm::mat4 M = camScreen.cam().invV() * rotateM * camScreen.cam().V() * volume.M();
		volume.updateM(M);
	}
};




