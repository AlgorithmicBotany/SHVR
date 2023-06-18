#pragma once

#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>
#include <stdlib.h>

#include <map>
#include <utility>
#include <iterator>
#include <vector>
#include <array>
#include <string.h>

#include <qdebug.h>
#include <glm/glm.hpp>		//glm::vec3

namespace Assign2
{
	typedef std::array< unsigned char, 4 > RGBA;
	typedef std::array< float, 2 > Endpoints1D;
	typedef std::array< float, 4 > Endpoints2D;
	enum DataInput { DATA_UNSET, UINT8, UINT16};
	enum TransferFunc { TF1D, TF2D, LINEAR_TF1D, UNSET };

	struct ColorPoint1D
	{
		float point;
		RGBA rgba;
		ColorPoint1D():
			rgba({0,0,0,0}),
			point(0) {}
		ColorPoint1D(unsigned char r, unsigned char g, unsigned char b, unsigned char a, float x):
			rgba({ r,g,b,a }),
			point(x){}
	};

	struct ColorInterval1D
	{
		Endpoints1D endpoint;
		RGBA rgba;
	};

	struct InputConfiguration
	{
		std::string configFile;	//Used as a reference when saving config file, not saved in save file		
		std::string inputFile; // .raw file
		std::string groupFile; //with extension
		std::string transferFuncFile; //with extension
		std::string lightFile;
		std::string screenFile;
		std::string nodeFile;
		std::string maskFile;
		std::string offsetFile;
				
		std::string inputDir;
		std::vector<std::string> inputSlices;
		std::string inputFileSuffix;
		std::string pathRelativeTo;

		DataInput inputType;

		std::vector<float> spacings;
		std::vector<int> dimensions;

		float sceneScale;

		bool maskEnabled;
		bool volumeCompressionEnabled;
		bool convertEndian;
		bool convertBackgroundValue;

		InputConfiguration(){ 
			spacings.resize(3);
			dimensions.resize(3);
			clear(); }
		void clear();
		bool checkInputs() const;
		bool load(const std::vector<std::string >& strings);
		bool save(std::string filename);
	};


	struct InputTransferFunction
	{
		TransferFunc useIntervals;
		std::vector< ColorPoint1D > linearIntensityTF;
		std::vector< ColorInterval1D > intensityTF;

		int shiftTF;
		int minTF;
		int maxTF;

		int isoMinPaint;
		int isoMinHaptics;
		bool autoAdjustIsoMin;
		bool lockIDPositions;
		bool autoApplyUpdates;

		InputTransferFunction() { clear(); }

		void clear();
		bool load(const std::vector<std::string >& lines);
		bool save(std::string filename);
	};

	struct InputLight {
		bool lockIDPositions;
		bool autoApplyUpdates;

		std::vector< ColorPoint1D > linearIntensityTF;
		float raycastStepSize;
		float raycastMaxSteps;
		float aoStepSize;
		float aoMaxSteps;

		float intensityAmbient;
		float intensityDiffuse;
		float intensitySpecular;
		float specularExponent;

		InputLight() { clear(); }
		void clear();
		bool load(const std::vector<std::string >& lines);
		bool save(std::string filename);
	};

	struct InputGeneral {
		// Visuals
		unsigned char screenType;		//0 = single screen, 1 = two screens, 2 = quad screen
		float fov;			//field of view
		float nearPlane;
		float farPlane;
		float clipPlaneNear;
		float clipPlaneFar;

		float volumeM[16]; //Volume Model Matrix (ie. camera position + rotation)
		glm::vec3 cutMin;
		glm::vec3 cutMax;
		
		// Enable settings
		bool hudEnabled;
		bool arrowEnabled;
		bool proxyEnabled;
		bool volumeEnabled;
		bool lineEnabled;
		bool objectEnabled;
		bool forceLineEnabled;

		// Render Settings
		float compressionRatio;
		float fps;			// Frames per second

		// Probe Settings
		float cursorScale;
		float silhouetteScale;
		float workspaceScale;
		
		unsigned char hapticType;		//0 = surface sampling, 1 = region-growth, 2 = solid fill
		double hapticSpringForce;
		double hapticDampingForce;
		double hapticHitThreshold;
				
		unsigned char paintType;		//0 = Quick fill, 1 = flood fill, 2 = speckle fill		
		float paintScale;
		int paintKernelSize;
		int selectedGroup;

		// Screenshot
		bool ssKeepAspectRatio;
		int ssWidth;
		int ssHeight;

		// Others
		int offsetType;
		float blendRate;
		float blendSize;
		int maxNumberOfCommands;
		
		InputGeneral() { clear(); }
		void clear();
		bool load(const std::vector<std::string >& strings);
		bool save(std::string filename);
	};



	struct InputGroup {
		unsigned int size;
		float maskIntensity;
		std::vector<bool> displayMask;
		std::vector<bool> displayObject;
		std::vector<std::array<unsigned char,4>> rgba;
		std::vector<float> linkScale;
		std::vector<float> densityOffset;
		std::vector<bool> paintOver;

		void clear();
		bool load(const std::vector<std::string >& strings);
		bool save(std::string filename);
	};

	struct InputNode {
		std::vector<glm::vec3> vertex_position;	//x,y,z,group
		std::vector<int*> vertex_link;
		std::vector<int> vertex_group;
		std::vector<float> vertex_scale;

		InputNode() {}

		void clear();
		bool load(const std::vector<std::string >& strings);
		bool save(std::string filename);
	};

} //namespace end
