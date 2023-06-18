#pragma once
#include <glm/glm.hpp>
#include <string>
#include "ColorMap.h"
#include "FileInputData.h"
#include "mathLib.h"

struct IsoParams {
	float paintIsoMin;
	float hapticsIsoMin;
	float paintInValue;
	float hapticsInValue;
	float offsetIsoValue;
	float offsetInValue;
	float tfIsoMin;
	float tfInValue;
	float IsoUpdateRate;

	float isoSurfaceThresh;
	glm::vec4 isoSurfaceColor = glm::vec4(1, 1, 1, 1);

	char paintVisualMode;
	glm::vec3 paintIdxMinNormalized;
	glm::vec3 paintIdxMaxNormalized;

	IsoParams() {
		paintIsoMin = 0;
		hapticsIsoMin = 0;
		paintInValue = 0;
		hapticsInValue = 0;
		offsetIsoValue = 0;
		offsetInValue = 0;
		tfIsoMin = 0;
		tfInValue = 0;
		IsoUpdateRate = 1;

		isoSurfaceThresh = 0;
		paintVisualMode = 0;
		paintIdxMinNormalized = glm::vec3(0, 0, 0);
		paintIdxMaxNormalized = glm::vec3(0, 0, 0);
	}

	void importData(const Assign2::InputTransferFunction& setting) {
		hapticsIsoMin = setting.isoMinHaptics;
		paintIsoMin = setting.isoMinPaint;
	}
	void exportData(Assign2::InputTransferFunction& setting) {
		setting.isoMinHaptics = hapticsIsoMin;
		setting.isoMinPaint = paintIsoMin;
	}
};

struct LightParams {
	int aoMaxSteps;
	float aoStepSize;
	int rayMaxSteps;
	float rayStepSize;

	float specularExponent;
	float intensityAmbient;
	float intensityDiffuse;
	float intensitySpecular;
	glm::vec3 lightColor;

	LightParams() {
		aoMaxSteps = 0;
		aoStepSize = 0;
		rayMaxSteps = 0;
		rayStepSize = 0;

		specularExponent = 0;
		intensityAmbient = 0;
		intensityDiffuse = 0;
		intensitySpecular = 0;
		lightColor = glm::vec3(1, 1, 1);
	}

	void importData(const Assign2::InputLight& setting) {
		rayMaxSteps = setting.raycastMaxSteps;
		rayStepSize = setting.raycastStepSize;
		aoMaxSteps = setting.aoMaxSteps;
		aoStepSize = setting.aoStepSize;

		specularExponent = setting.specularExponent;
		intensityAmbient = setting.intensityAmbient;
		intensityDiffuse = setting.intensityDiffuse;
		intensitySpecular = setting.intensitySpecular;		
	}
	void exportData(Assign2::InputLight& setting) {
		setting.raycastMaxSteps = rayMaxSteps;
		setting.raycastStepSize = rayStepSize;
		setting.aoMaxSteps = aoMaxSteps;
		setting.aoStepSize = aoStepSize;

		setting.specularExponent = specularExponent;
		setting.intensityAmbient = intensityAmbient;
		setting.intensityDiffuse = intensityDiffuse;
		setting.intensitySpecular = intensitySpecular;		
	}
};

struct ScreenshotParams {
	std::string fileDir;
	std::string fileName;	
	bool keepAspectRatio;
	int ssWidth;		
	int ssHeight;	
	int screenWidth;
	int screenHeight;

	ScreenshotParams() {		
		fileDir = "";
		fileName = "image.png";		
		keepAspectRatio = true;
		ssWidth = 0;
		ssHeight = 0;		
		screenWidth = 0;
		screenHeight = 0;
	}

	float getAspectRatio() {
		return (float)screenHeight / (float)screenWidth;
	}

	void updateUndefinedDimensions() {
		if (ssWidth == 0 && ssHeight == 0) {
			// undefined width and height
			ssWidth = screenWidth;
			ssHeight = screenHeight;
		}
		else if (ssWidth != 0 && ssHeight == 0) {
			// undefined height
			ssHeight = ssWidth * getAspectRatio();
		}
		else if (ssWidth == 0 && ssHeight != 0) {
			// undefined width
			ssWidth = ssHeight / getAspectRatio();
		}			
	}

	void importData(const Assign2::InputGeneral& setting) {
		keepAspectRatio = setting.ssKeepAspectRatio;
				
		ssWidth = setting.ssWidth;		
		ssHeight = setting.ssHeight;

		updateUndefinedDimensions();
	}

	void exportData(Assign2::InputGeneral& setting) {
		setting.ssKeepAspectRatio = keepAspectRatio;
		setting.ssWidth = ssWidth;
		setting.ssHeight = ssHeight;
	}	
};

struct ProbeParams {
	//*** TODO: Add offsetType to import and export data

	float probeSize;
	float silhouetteSize;
	float workspaceScale;
	int selectedGroup;
	
	// Haptics 
	bool hapticsEnabled;
	unsigned char hapticType;		//0 = surface sampling, 1 = Object Following, 2 = Proximity Following
	double hapticSpringForce;
	double hapticDampingForce;
	double hapticHitThreshold;

	// Paint
	unsigned char paintType;		//0 = Quick fill, 1 = flood fill, 2 = speckle fill		
	float paintScale;

	// Others
	unsigned char offsetType;		// 0 = DIST2, 1 = Uniform
	float rotateAngleRadians;
	float moveDist;

	ProbeParams() {
		probeSize = 1;
		silhouetteSize = 1;
		workspaceScale = 1;
		selectedGroup = 1;

		// Haptics 
		hapticsEnabled = false;
		hapticType = 0;
		hapticSpringForce = 100;
		hapticDampingForce = 0;
		hapticHitThreshold = 0.5;

		// Paint
		paintType = 0;
		paintScale = 1;
		offsetType = 0;		

		//Others
		rotateAngleRadians = 1.f;
		moveDist = 0.005f;
	}

	void importData(const Assign2::InputGeneral& setting) {
		probeSize = setting.cursorScale;
		silhouetteSize = setting.silhouetteScale;
		workspaceScale = setting.workspaceScale;
		
		hapticType = setting.hapticType;
		hapticSpringForce = setting.hapticSpringForce;
		hapticDampingForce = setting.hapticDampingForce;
		hapticHitThreshold = setting.hapticHitThreshold;
		
		paintType = setting.paintType;
		paintScale = setting.paintScale;
		selectedGroup = setting.selectedGroup;
		offsetType = setting.offsetType;
	}

	void exportData(Assign2::InputGeneral& setting) {
		setting.cursorScale = probeSize;
		setting.silhouetteScale = silhouetteSize;
		setting.workspaceScale = workspaceScale;

		setting.hapticType = hapticType;
		setting.hapticSpringForce = hapticSpringForce;
		setting.hapticDampingForce = hapticDampingForce;
		setting.hapticHitThreshold = hapticHitThreshold;

		setting.paintType = paintType;
		setting.paintScale = paintScale;
		setting.selectedGroup = selectedGroup;
		setting.offsetType = offsetType;
	}

};
struct VisualParams {
	glm::mat4 volumeM;			//Volume Model Matrix (ie. camera position + rotation)
	bool queueCameraSave;
	bool queueCameraLoad;

	// Camera parameters
	unsigned char screenType;	//0 = single screen, 1 = two screens, 2 = quad screen
	float fov;					//field of view
	float nearPlane;
	float farPlane;
	float distY;
	float cameraZ;

	// Visual parameters
	float blendRate;
	float blendSize;

	float clipPlaneNear;
	float clipPlaneFar;	
	glm::vec3 cutMin;
	glm::vec3 cutMax;

	bool hudEnabled;
	bool arrowEnabled;
	bool proxyEnabled;
	bool volumeEnabled;
	bool lineEnabled;
	bool objectEnabled;
	bool forceLineEnabled;

	const float EPSILON = 0.00005;

	void updateClipNear(float clipNear) {
		// Bound clip plane to plane, while avoiding z-fighting with volume
		clipPlaneNear = std::max(nearPlane + EPSILON, clipNear);
	}
	void updateClipFar(float clipFar) {
		// Bound clip plane to plane, while avoiding z-fighting with volume
		clipPlaneFar = std::min(farPlane - EPSILON, clipFar);
	}

	void updateCutMin(const glm::vec3& _cutMin) {
		cutMin = MathLib::max(_cutMin, cutMax);				// no greater than cutMax
		cutMin = MathLib::min(cutMin, glm::vec3(0, 0, 0));	// no less than (0,0,0)
	}
	void updateCutMax(const glm::vec3& _cutMax) {
		cutMax = MathLib::min(_cutMax, cutMin);				// no less than cutMin
		cutMax = MathLib::max(cutMax, glm::vec3(1,1,1));	// no greater than (1,1,1)
	}

	void importData(const Assign2::InputGeneral& setting) {
		// Volume position
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				volumeM[i][j] = setting.volumeM[4 * j + i];
			}
		}
		queueCameraSave = false;
		queueCameraLoad = false;

		// Camera settings
		screenType =setting.screenType;
		fov = setting.fov;
		nearPlane = setting.nearPlane;
		farPlane = setting.farPlane;

		// Visual parameters
		clipPlaneNear = setting.clipPlaneNear;
		clipPlaneFar = setting.clipPlaneFar;
		cutMin = setting.cutMin;
		cutMax = setting.cutMax;

		blendRate = setting.blendRate;
		blendSize = setting.blendSize;

		hudEnabled = setting.hudEnabled;
		arrowEnabled = setting.arrowEnabled;
		proxyEnabled = setting.proxyEnabled;
		volumeEnabled = setting.volumeEnabled;
		lineEnabled = setting.lineEnabled;
		objectEnabled = setting.objectEnabled;
		forceLineEnabled = setting.forceLineEnabled;
	}

	void exportData(Assign2::InputGeneral& setting) {
		// Volume position
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				setting.volumeM[4 * j + i] = volumeM[i][j];
			}
		}

		// Camera settings
		setting.screenType = screenType;
		setting.fov = fov;
		setting.nearPlane = nearPlane;
		setting.farPlane = farPlane;

		// Visual settings
		setting.clipPlaneNear = clipPlaneNear;
		setting.clipPlaneFar = clipPlaneFar;
		setting.cutMin = cutMin;
		setting.cutMax = cutMax;

		setting.blendRate = blendRate;
		setting.blendSize = blendSize;

		setting.hudEnabled = hudEnabled;
		setting.arrowEnabled = arrowEnabled;
		setting.proxyEnabled = proxyEnabled;
		setting.volumeEnabled = volumeEnabled;
		setting.lineEnabled = lineEnabled;
		setting.objectEnabled = objectEnabled;
		setting.forceLineEnabled = forceLineEnabled;
	}

};
struct RenderParams {
	float compressionRatio;
	float fps;			// Frames per second
	int maxNumberOfCommands;


	void importData(const Assign2::InputGeneral& setting) {
		compressionRatio = setting.compressionRatio;
		fps = setting.fps;
		maxNumberOfCommands = setting.maxNumberOfCommands;
	}

	void exportData(Assign2::InputGeneral& setting) {
		setting.compressionRatio = compressionRatio;
		setting.fps = fps;
		setting.maxNumberOfCommands = maxNumberOfCommands;
	}
};

//====

struct ColorGroup {
	bool displayMask;
	bool displayObject;
	unsigned char r, g, b, a;
	float linkScale;
	bool paintOver;

	ColorGroup() {
		displayMask = true;
		displayObject = true;
		r = 0;
		g = 0;
		b = 0;
		a = 0;
		linkScale = 1;
	}

	void setColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 255) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	glm::vec3 getRGB_f() { return glm::vec3((float)r / 255.f, (float)g / 255.f, (float)b / 255.f); }
	glm::vec4 getRGBA_f() { return glm::vec4((float)r / 255.f, (float)g / 255.f, (float)b / 255.f, (float)a / 255.f); }
	glm::ivec3 getRGB_uc() { return glm::ivec3(r, g, b); }
	glm::ivec4 getRGBA_uc() { return glm::ivec4(r, g, b, a); }
};

struct ColorGroups :std::vector<ColorGroup> {

	float maskIntensity;
	ColorGroups() : std::vector<ColorGroup>(10) {
		at(0).displayMask = 0;
	}
	

	void importData(const Assign2::InputGroup& groupFile) {
		maskIntensity = groupFile.maskIntensity;
		for (int i = 0; i < size(); i++) {
			at(i).displayMask = groupFile.displayMask[i];
			at(i).displayObject = groupFile.displayObject[i];
			at(i).r = groupFile.rgba[i][0];
			at(i).g = groupFile.rgba[i][1];
			at(i).b = groupFile.rgba[i][2];
			at(i).a = groupFile.rgba[i][3];
			at(i).linkScale= groupFile.linkScale[i];
			at(i).paintOver = groupFile.paintOver[i];
		}
	}

	void exportData(Assign2::InputGroup& groupFile) {
		groupFile.maskIntensity = maskIntensity;
		for (int i = 0; i < size(); i++) {
			groupFile.displayMask[i] = at(i).displayMask;
			groupFile.displayObject[i] = at(i).displayObject;
			groupFile.rgba[i][0]=at(i).r;
			groupFile.rgba[i][1]=at(i).g;
			groupFile.rgba[i][2]=at(i).b;
			groupFile.rgba[i][3]=at(i).a;
			groupFile.linkScale[i] = at(i).linkScale;
			groupFile.paintOver[i] = at(i).paintOver;
		}
	}

	void initColorMapNoAlpha(ColorMap* colorMap) {
		for (int i = 0; i < size(); ++i)
		{
			ColorMap::RGBuc const& rgb = colorMap->rgb_uc(i);

			//Unsigned char [0,255]
			ColorGroup g;
			g.r = rgb[0];
			g.g = rgb[1];
			g.b = rgb[2];
			g.a = 255;		//Assume max opacity

			at(i) = g;
		}
	}

	void initColorMapWithAlpha(ColorMap* colorMap, std::vector<unsigned char> const& opacityList) {
		initColorMapNoAlpha(colorMap);
		for (int i = 0; i < size(); ++i)
		{
			at(i).a = opacityList[i];
		}
	}

	std::string getDisplayGroupString() {
		std::string s;
		for (int i = 1; i <= 9; i++) {
			if (at(i).displayMask == true) {
				s = s + std::to_string(i);
			}
			else {
				s = s + "_";
			}
		}

		return s;
	}
	std::vector<bool> getDisplayGroup() {
		std::vector<bool> displayGroup;
		for (int i = 0; i < size(); i++) {
			displayGroup.push_back(at(i).displayMask);
		}
		return displayGroup;
	}

	void setDisplayGroup(std::vector<bool> const& displayGroup) {
		for (int i = 0; i < size(); i++) {
			at(i).displayMask = displayGroup[i];
		}
	}

	unsigned int getDisplaySum() {
		int sum = 0;
		for (int i = 0; i < size(); i++) {
			sum = sum + at(i).displayMask;
		}
		return sum;
	}

	unsigned char getMaxOpacityVolume() { return at(0).a; }
	void setMaxOpacityVolume(int _maxOpacityVolume) {
		at(0).a = _maxOpacityVolume;
	}

	unsigned char getOpacityGroup(int groupNum) { return at(groupNum).a; }
	void setOpacityGroup(int opacity, int groupNum) {
		at(groupNum).a = opacity;
	}

	float getMaxLinkScale() { return at(0).linkScale; }
	float getLinkScale(int groupNum) { return at(groupNum).linkScale; }
	void setLinkScale(float value, int groupNum) { at(groupNum).linkScale = value; }

	void setNthMaskGroup(bool _set, int nth_bit) {
		at(nth_bit).displayMask = _set;
	}

	bool isNthMaskGroupSet(int nth_bit) {
		return at(nth_bit).displayMask;
	}

	void setNthObjGroup(bool _set, int nth_bit) {
		at(nth_bit).displayObject = _set;
	}

	bool isNthObjGroupSet(int nth_bit) {
		return at(nth_bit).displayObject;
	}
};
