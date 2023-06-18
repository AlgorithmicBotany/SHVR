#pragma once

// Qt libraries
#include <QOpenGLWindow>
#include <qopenglfunctions_4_5_core.h>
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>		// Communication between applications
#include <qmimedata.h>		// Drop event

// C++ libraries
#include <Vector>
#include <chrono>	//sleep function
#include <thread>	//sleep function

#define _USE_MATH_DEFINES
#include <cmath>

// Custom Data types
#include "volumes.h"		//volume data
#include "ShaderProgram.h"	//shader types
#include "NodeNetwork.h"
#include "TransferFunction.h"
#include "World.h"
#include "ParameterStructs.h"
#include "StatusBar.h"
#include "Cameras.h"

// Objects used for rendering
#include "VertexObject.h"
#include "TextureObject.h"
#include "RenderObject.h"

// OpenGL math libraries
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

// Widgets
#include "FileOpenWidget.h"   // Note: needed because of Qt bug in sending drop events


using namespace Assign2;

//====

#include <qobject.h>



class OpenGLWindow : public QOpenGLWindow, protected QOpenGLFunctions_4_0_Core
{
	Q_OBJECT
public:
	World* world;

	float maskIntensity;
	glm::vec4 backgroundColour;		
	std::vector<unsigned char> colorTF_RGBA;
	std::vector<unsigned char> colorGroups_RGBA;
	std::vector<unsigned char> aoIntensity_A;

	float blendRate;
	float blendSize;

	OpenGLWindow(QObject* parent, World* worldObject,
		LightParams* _lightParams, VisualParams* _visualParams,
		ScreenshotParams* _ssParams, RenderParams* _renderParams,
		IsoParams* _isoParams);
	~OpenGLWindow();

	void initVolumeTextureParameters(bool volumeCompressed, DataInput primitiveType);

	int getMaxTexSize() { return maxTexSize; }
	glm::ivec2 getScreenDimensions() { 
		return glm::ivec2(
			this->size().width(), 
			this->size().height()); }
	glm::vec3 getNDCfromSS(const glm::vec2& pointSS) {
		//Note: Top left pixel is (0,0), bottom right is (screenWidth, screenHeight)
		return glm::vec3(2 * ((float)pointSS.x / (float)width()) - 1,
			-(2 * ((float)pointSS.y / (float)height()) - 1),
			0);
	}
	bool isPointOutsideView(const glm::vec3& pointNDC) {
		//Screen space is defined to be within [-1,1] in x and y directions
		if (pointNDC.x < -1 || pointNDC.x > 1 ||
			pointNDC.y < -1 || pointNDC.y > 1) {
			return true;
		}
		else {
			return false;
		}
	}

	// Queue updates
	bool queueInitScene;
	bool queueUpdateROBoxMinMax;
	int queueScreenshotMainCamera;	//0 = no screenshot; 1 = main camera; 2 = two halves

	bool queueUpdateTextureVolume;
	bool queueUpdateTextureMask;
	bool queueUpdateTextureVolumeAO;
	bool queueUpdateTextureVolumeColor;
	bool queueUpdateTextureMaskColor;	
	bool queueUpdateTextureAOIntensity;
	bool queueUpdateTextureScreen;

	bool queueUpdateShaderBoxDimensions;
	bool queueUpdateShaderCutPlane;
	bool queueUpdateShaderScreen;
	bool queueUpdateShaderInvVolume;	
	bool queueUpdateOverviewImages;

	void updateScreen(int screenWidth, int screenHeight);
	void updateSSDimensions(int w, int h);
	void updateMaskEnabled(ColorGroups& groups);
	void updateCompressionRatio(float ratio);
	void updateFPS(float fps);
		
protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void updateScreenSection(unsigned int screenSection);
	void render();
	void setDrawBuffer(GLenum mode);
	void render3DScene(CameraScreen& camScreen);
	void renderOverviewScene(std::vector<GLubyte>& image, CameraScreen& camScreen);
	void renderFinalImageToScreen(unsigned int screenSection);
	void renderFull();
	void renderStereo();
	
	void renderOverview();

	void renderShaderVertexUniform(RenderObject* rObject, const glm::mat4& MVP, const glm::vec3& color);
	void renderShaderRayStart(GLExt::ShaderProgram* shader, RenderObject* rObject, const glm::mat4& MVP, const glm::mat4& M, bool rayEnabled);
	void renderShaderRayEndStart(RenderObject* rObject, const glm::mat4& MVP);
	void renderShaderRayEndObject(RenderObject* rObject, const glm::mat4& MVP, const glm::mat4& M);
	void renderShaderVertexObjectLight(RenderObject* rObject, const glm::mat4& MVP, const glm::mat4& MV, const glm::vec3& color);
	void renderShaderVertexObjectUniform(RenderObject* rObject, const glm::mat4& MVP, const glm::vec3& color);
	void renderShaderVertexObjectDepth(RenderObject* rObject, const glm::mat4& MVP, float depth);
	void renderShaderVertexObjectSilhouette(RenderObject* rObject, const glm::mat4& MVP, const glm::vec3& color, float depth);
	void renderShaderImageTexture(RenderObject* rObject, int tex_num, float opacity);
	void renderShaderImageObject(RenderObject* rObject, const glm::mat4& MVP, int tex_num, float opacity);
	void renderShaderLinesSolid(RenderObject* rObject, const glm::mat4& MVP, float lineWidth);
	void renderShaderLinesStippled(RenderObject* rObject, const glm::mat4& MVP, int pixelGap, float lineWidth);
	void renderShaderRayCast(const glm::vec3& lightPosVS, const glm::vec3& camDirVS);

	void renderTextureRayStartPos(CameraScreen& camScreen);
	void renderTextureRayDirAndDist_Volume(CameraScreen& camScreen);
	void renderTextureRayDirAndDist_VertexObjects(CameraScreen& camScreen);
	void renderTextureVertexObjects(CameraScreen& camScreen);
	void renderTextureRayCast(CameraScreen& camScreen);
	void renderTextureOverlay(CameraScreen& camScreen);

	void saveTextureToImage(std::vector<GLubyte>& image, const glm::ivec2& texDimensions);
	void saveImageToFile(std::vector<GLubyte>& image, const glm::ivec2& texDimensions, std::string filename);
	void loadImageToTexture(std::vector<GLubyte>& image, const glm::ivec2& texDimensions, TextureObject* tObject);

private:
	enum RENDER {	//RENDER OBJECTS
		Box,
		Quad,
		Sphere,
		Cylinder,
		FrusBox,
		FrusLinesNear,
		FrusLinesFar,
		Arrow,
		ForceLine,
		COUNT_RENDER
	};

	enum TEXTURE {	//TEXTURE OBJECTS
		VolumeTex,
		VolumeColorTex,
		MaskTex,
		MaskColorTex,
		AOTex,
		AOIntensityTex,
		RayDirAndDistTex,
		RayStartPosTex,
		OverviewTex,
		FinalTex,
		COUNT_TEXTURE
	};

	enum Shader {
		ObjColorVertex,				//Draws object with vertex colors
		ObjColorUniform,
		ObjColorSilhouette,			//Sets the colour and depth of object
		ObjDepth,					//Sets the depth of object
		ObjStippled,				//Draws lines with stippled effect
		ObjRender,					//Render object with phong lighting

		RayCast,					//Cast ray based on some start position & direction		
		RayLightIntensity,			//Generate volume for light intensity volume for ambient occlusion
		RayCastAO,
		RayDepthTopBot,				//Show intersection with volume data on top & bottom surface of probe
		RayDepthFilled,				//Show % of volume data is filled within probe

		RayStartFromObject,
		RayEndAtStart,
		RayEndAtObject,
				
		ImgTexture,					//Displays the image as a texture
		ImgObject,					//Displays the image on an object (Flat)
		COUNT_SHADER
	};

	// External values
	LightParams* lightParams;
	VisualParams* visualParams;
	ScreenshotParams* ssParams;
	RenderParams* renderParams;
	IsoParams* isoParams;

	// Internal objects
	QObject* parentWidget;
	QOpenGLContext* openGLContext;
	GLuint backFramebuffer;
	GLuint volumeFramebuffer;
	GLuint depthbuffer;
	
	VOBox vo_box;
	VOQuad vo_quad;
	Frustum frus;
	std::vector<TextureObject*> textureObjects;
	std::vector<RenderObject*> renderObjects;
	std::vector<GLExt::ShaderProgram*> shaders;

	GLint maxTexSize;	
	float compressionRatio;				
	int microsecondsPerFrame;
	std::chrono::time_point<std::chrono::high_resolution_clock> timer;

	glm::vec2 textureScreenDimensions;
		
	std::vector<GLubyte> overviewImageTop;
	std::vector<GLubyte> overviewImageFront;
	std::vector<GLubyte> overviewImageSide;
	bool queueUpdateMaskEnabled;
	std::vector<int> maskEnabled;
	
	//Shaders
	void initShaderFiles();
	void initShaderParameters();
	void initShaders();	
	void initScene();

	void updateCameraScreen(int screenWidth, int screenHeight, CameraScreen& camScreen);
	void updateTextureScreenDimensionsScreenshot(int screenWidth, int screenHeight, float scale);
	void updateTextureScreenDimensions(int screenWidth, int screenHeight, int screenType);	
	void updateOverviewImages();
	void updateImageSize();
	void renderImage(int imageWidth, int imageHeight, CameraScreen& camScreen, std::vector<GLubyte>& out_image);
	void updateOthers();

	void initLightIntensityVolume();
	void updateLightIntensityVolume();	

	void updateShaderCuttingPlaneMinMax(glm::vec3 cutMin, glm::vec3 cutMax);
	void updateShaderScreenSize();
	void updateShaderTextureID();
	void updateShaderBoxDimensions(VolumeBase* vData);
	void updateShaderInvVolume(const glm::mat4& invVolume);
	void updateShaders();

	// Textures
	void initVolumeTextureParameters(const glm::ivec3& dimensions, bool volumeCompressionEnabled, DataInput dataType);
	void initMaskTextureParameters(const glm::ivec3& dimensions);
	void initAOTextureParameters(const glm::ivec3& dimensions);
	void initVolumeColorTextureParameters();
	void initMaskColorTextureParameters();
	void initAOIntensityTextureParameters();
	void initScreenTexturesParameters();
	void initFrameAndDepthBuffer();
	
	void initTextureObjects();

	void updateTextures();

	void updateMaxTexSize();
	void updateVolumeTexture();
	void updateVolumeTextureSubsection();
	void updateMaskTexture();
	void updateMaskTextureSubsection();
	void updateTransferFunctionTexture();
	void updateMaskColorsTexture();
	void updateAOIntensityTexture();
	void updateScreenTextures();


	//Render Objects
	void initRO_box();
	void initRO_quad();
	void initRO_sphere(float scale);
	void initRO_cylinder(float scale);
	void initRO_frustum();
	void initRO_arrow(float scale);
	void initRO_forceLine();
	void initRenderObjects();

	void updateRenderObjects();

	void updateROBoxMinMax(const glm::vec3& cutMin, const glm::vec3& cutMax);
	void updateROQuadMinMax(const glm::vec2& coordMin, const glm::vec2& coordMax);
	void updateROFrustumLines();
	void updateROFrustumBox();


signals:
	void initialized();
	void resized();
};
