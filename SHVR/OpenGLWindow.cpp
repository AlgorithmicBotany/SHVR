#include "OpenGLWindow.h"



//=========
// Settings to move
#define OVERVIEW_BKGD_OPACITY 0.5
#define OVERVIEW_LINE_WIDTH 2
#define OVERVIEW_PIXEL_GAP 5

void OpenGLWindow::initShaderFiles() {
	//==== Object shaders ====//
	//	GLExt::ShaderProgram basicProgram;
	shaders[ObjColorVertex]->addShader("shaders/colorVertex.vert", GL_VERTEX_SHADER);
	shaders[ObjColorVertex]->addShader("shaders/colorVertex.frag", GL_FRAGMENT_SHADER);

	shaders[ObjColorUniform]->addShader("shaders/colorUniform.vert", GL_VERTEX_SHADER);
	shaders[ObjColorUniform]->addShader("shaders/colorVertex.frag", GL_FRAGMENT_SHADER);

	shaders[ObjDepth]->addShader("shaders/basic.vert", GL_VERTEX_SHADER);
	shaders[ObjDepth]->addShader("shaders/depthUniform.frag", GL_FRAGMENT_SHADER);

	shaders[ObjColorSilhouette]->addShader("shaders/colorUniform.vert", GL_VERTEX_SHADER);
	shaders[ObjColorSilhouette]->addShader("shaders/colorVertex_depthUniform.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram stippledProgram;
	shaders[ObjStippled]->addShader("shaders/lineStippled.vert", GL_VERTEX_SHADER);
	shaders[ObjStippled]->addShader("shaders/lineStippled.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram objRenderProgram;
	shaders[ObjRender]->addShader("shaders/objRender.vert", GL_VERTEX_SHADER);
	shaders[ObjRender]->addShader("shaders/objRender.frag", GL_FRAGMENT_SHADER);

	//==== Ray-casting shaders ====//
	//	GLExt::ShaderProgram rayCastProgram;
	shaders[RayCast]->addShader("shaders/vertexUV.vert", GL_VERTEX_SHADER);
	shaders[RayCast]->addShader("shaders/RayCast.frag", GL_FRAGMENT_SHADER);

	shaders[RayCastAO]->addShader("shaders/vertexUV.vert", GL_VERTEX_SHADER);
	shaders[RayCastAO]->addShader("shaders/RayCastAO.frag", GL_FRAGMENT_SHADER);

	shaders[RayLightIntensity]->addShader("shaders/vertexUV.vert", GL_VERTEX_SHADER);
	shaders[RayLightIntensity]->addShader("shaders/RayLightIntensity.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram rayTopBotProgram;
	shaders[RayDepthTopBot]->addShader("shaders/vertexBasic.vert", GL_VERTEX_SHADER);
	shaders[RayDepthTopBot]->addShader("shaders/RayDepthTopBot.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram rayFilledProgram;
	shaders[RayDepthFilled]->addShader("shaders/vertexBasic.vert", GL_VERTEX_SHADER);
	shaders[RayDepthFilled]->addShader("shaders/RayDepthFilled.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram objPosProgram;
	shaders[RayStartFromObject]->addShader("shaders/objPosInVolume.vert", GL_VERTEX_SHADER);
	shaders[RayStartFromObject]->addShader("shaders/RayStartFromObject.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram silhouetteVolumeProgram;
	shaders[RayEndAtStart]->addShader("shaders/vertexBasic.vert", GL_VERTEX_SHADER);
	shaders[RayEndAtStart]->addShader("shaders/RayEndAtStart.frag", GL_FRAGMENT_SHADER);

	//	GLExt::ShaderProgram objVolumeProgram;
	shaders[RayEndAtObject]->addShader("shaders/objPosInVolume.vert", GL_VERTEX_SHADER);
	shaders[RayEndAtObject]->addShader("shaders/RayEndAtObject.frag", GL_FRAGMENT_SHADER);

	//==== Image Shaders ====//
	//	GLExt::ShaderProgram finalImageProgram;
	shaders[ImgTexture]->addShader("shaders/vertexUV.vert", GL_VERTEX_SHADER);
	shaders[ImgTexture]->addShader("shaders/imageTexture.frag", GL_FRAGMENT_SHADER);

	shaders[ImgObject]->addShader("shaders/vertexBasic.vert", GL_VERTEX_SHADER);
	shaders[ImgObject]->addShader("shaders/imageObject.frag", GL_FRAGMENT_SHADER);
}

void OpenGLWindow::initShaderParameters() {
	updateShaderCuttingPlaneMinMax(visualParams->cutMin, visualParams->cutMax);
	updateShaderTextureID();
	updateShaderScreenSize();
	updateShaderBoxDimensions(world->volume.vData());
}

void OpenGLWindow::initShaders() {
	for (int i = 0; i < shaders.size(); i++) {
		shaders[i]->createProgram();
	}

	initShaderFiles();
	initShaderParameters();
}

void OpenGLWindow::updateShaderCuttingPlaneMinMax(glm::vec3 cutMin, glm::vec3 cutMax) {
	shaders[RayLightIntensity]->useProgram();
	shaders[RayLightIntensity]->setVec3("cutMin", cutMin);
	shaders[RayLightIntensity]->setVec3("cutMax", cutMax);

	shaders[RayCast]->useProgram();
	shaders[RayCast]->setVec3("cutMin", cutMin);
	shaders[RayCast]->setVec3("cutMax", cutMax);

	shaders[RayCastAO]->useProgram();
	shaders[RayCastAO]->setVec3("cutMin", cutMin);
	shaders[RayCastAO]->setVec3("cutMax", cutMax);

	shaders[RayDepthTopBot]->useProgram();
	shaders[RayDepthTopBot]->setVec3("cutMin", cutMin);
	shaders[RayDepthTopBot]->setVec3("cutMax", cutMax);

	shaders[RayDepthFilled]->useProgram();
	shaders[RayDepthFilled]->setVec3("cutMin", cutMin);
	shaders[RayDepthFilled]->setVec3("cutMax", cutMax);
}

void OpenGLWindow::updateShaderScreenSize() {
	//reload all the data to shader with screen width and height
	shaders[RayEndAtStart]->useProgram();
	shaders[RayEndAtStart]->setVec2("textureSize", textureScreenDimensions);

	shaders[RayEndAtObject]->useProgram();
	shaders[RayEndAtObject]->setVec2("textureSize", textureScreenDimensions);

	shaders[RayDepthTopBot]->useProgram();
	shaders[RayDepthTopBot]->setVec2("textureSize", textureScreenDimensions);

	shaders[RayDepthFilled]->useProgram();
	shaders[RayDepthFilled]->setVec2("textureSize", textureScreenDimensions);

	shaders[ObjStippled]->useProgram();
	shaders[ObjStippled]->setVec2("textureSize", textureScreenDimensions);

	shaders[ImgObject]->useProgram();
	shaders[ImgObject]->setVec2("textureSize", textureScreenDimensions);
}

void OpenGLWindow::updateShaderTextureID() {
	shaders[RayEndAtStart]->useProgram();
	shaders[RayEndAtStart]->setInt("startingPositionTexture", RayStartPosTex);

	shaders[RayEndAtObject]->useProgram();
	shaders[RayEndAtObject]->setInt("startingPositionTexture", RayStartPosTex);

	shaders[RayCast]->useProgram();
	shaders[RayCast]->setInt("volumeTexture", VolumeTex);
	shaders[RayCast]->setInt("maskTexture", MaskTex);
	shaders[RayCast]->setInt("backTexture", RayDirAndDistTex);
	shaders[RayCast]->setInt("frontTexture", RayStartPosTex);
	shaders[RayCast]->setInt("volumeColorTexture", VolumeColorTex);
	shaders[RayCast]->setInt("maskColorTexture", MaskColorTex);

	shaders[RayLightIntensity]->useProgram();
	shaders[RayLightIntensity]->setInt("volumeTexture", VolumeTex);
	shaders[RayLightIntensity]->setInt("maskTexture", MaskTex);
	shaders[RayLightIntensity]->setInt("volumeColorTexture", VolumeColorTex);
	shaders[RayLightIntensity]->setInt("maskColorTexture", MaskColorTex);
	shaders[RayLightIntensity]->setInt("aoIntensityTexture", AOIntensityTex);

	shaders[RayCastAO]->useProgram();
	shaders[RayCastAO]->setInt("volumeTexture", VolumeTex);
	shaders[RayCastAO]->setInt("maskTexture", MaskTex);
	shaders[RayCastAO]->setInt("AOTexture", AOTex);
	shaders[RayCastAO]->setInt("backTexture", RayDirAndDistTex);
	shaders[RayCastAO]->setInt("frontTexture", RayStartPosTex);
	shaders[RayCastAO]->setInt("volumeColorTexture", VolumeColorTex);
	shaders[RayCastAO]->setInt("maskColorTexture", MaskColorTex);

	shaders[RayDepthTopBot]->useProgram();
	shaders[RayDepthTopBot]->setInt("volumeTexture", VolumeTex);
	shaders[RayDepthTopBot]->setInt("backTexture", RayDirAndDistTex);
	shaders[RayDepthTopBot]->setInt("frontTexture", RayStartPosTex);
	shaders[RayDepthTopBot]->setInt("volumeColorTexture", VolumeColorTex);

	shaders[RayDepthFilled]->useProgram();
	shaders[RayDepthFilled]->setInt("volumeTexture", VolumeTex);
	shaders[RayDepthFilled]->setInt("backTexture", RayDirAndDistTex);
	shaders[RayDepthFilled]->setInt("frontTexture", RayStartPosTex);
	shaders[RayDepthFilled]->setInt("volumeColorTexture", VolumeColorTex);
}

void OpenGLWindow::updateShaderBoxDimensions(VolumeBase* vData) {
	glm::vec3 dimensions = glm::vec3(vData->width(), vData->height(), vData->depth());
	
	shaders[RayCast]->useProgram();
	shaders[RayCast]->setVec3("dimensions", dimensions);

	shaders[RayCastAO]->useProgram();
	shaders[RayCastAO]->setVec3("dimensions", dimensions);

	//====
	glm::vec3 bounds = glm::vec3(vData->boxWidth(), vData->boxHeight(), vData->boxDepth());

	shaders[RayStartFromObject]->useProgram();
	shaders[RayStartFromObject]->setVec3("bounds", bounds);

	shaders[RayEndAtObject]->useProgram();
	shaders[RayEndAtObject]->setVec3("bounds", bounds);

	shaders[RayDepthFilled]->useProgram();
	shaders[RayDepthFilled]->setVec3("bounds", bounds);
}

void OpenGLWindow::updateShaderInvVolume(const glm::mat4& invVolumeM) {
	shaders[RayStartFromObject]->useProgram();
	shaders[RayStartFromObject]->setMat4("invVolume", invVolumeM);

	shaders[RayEndAtObject]->useProgram();
	shaders[RayEndAtObject]->setMat4("invVolume", invVolumeM);
}

void OpenGLWindow::updateShaders() {
	if (queueUpdateShaderCutPlane) { 
		updateShaderCuttingPlaneMinMax(visualParams->cutMin, visualParams->cutMax);
		queueUpdateShaderCutPlane = false;
	}

	if (queueUpdateShaderBoxDimensions) {
		updateShaderBoxDimensions(world->volume.vData());
		queueUpdateShaderBoxDimensions = false;
	}

	if (queueUpdateShaderScreen) { 
		updateShaderScreenSize();
		queueUpdateShaderScreen = false;
	}	

	if (queueUpdateShaderInvVolume) {
		updateShaderInvVolume(world->volume.invM());		
		queueUpdateShaderInvVolume = false;
	}
}


void OpenGLWindow::initVolumeTextureParameters(const glm::ivec3& dimensions, 
	bool volumeCompressionEnabled, DataInput dataType)
{
	GLenum internal_format, source_type;
	if (volumeCompressionEnabled) { internal_format = GL_R8; }
	else { internal_format = GL_R32F; }

	if (dataType == DataInput::UINT8) { source_type = GL_UNSIGNED_BYTE; }
	else if (dataType == DataInput::UINT16) { source_type = GL_UNSIGNED_SHORT; }
	else { source_type = GL_FLOAT; }

	textureObjects[TEXTURE::VolumeTex]->initParameters(
		VolumeTex,
		GL_TEXTURE_3D,
		dimensions,
		internal_format,
		GL_RED,
		source_type);

	textureObjects[TEXTURE::VolumeTex]->setting(GL_LINEAR, GL_CLAMP_TO_BORDER, 1);
}

void OpenGLWindow::initMaskTextureParameters(const glm::ivec3& dimensions)
{
	textureObjects[TEXTURE::MaskTex]->initParameters(
		MaskTex,
		GL_TEXTURE_3D,
		dimensions,
		GL_R8,				// Converts unsigned int [0,255] to float [0,1]
		GL_RED,
		GL_UNSIGNED_BYTE);		

	textureObjects[TEXTURE::MaskTex]->setting(GL_NEAREST, GL_CLAMP_TO_EDGE, 1);
}

void OpenGLWindow::initAOTextureParameters(const glm::ivec3& dimensions)
{
	textureObjects[TEXTURE::AOTex]->initParameters(
		AOTex,
		GL_TEXTURE_3D,
		dimensions,
		GL_R8,				// Converts unsigned int [0,255] to float [0,1]
		GL_RED,
		GL_UNSIGNED_BYTE);
	textureObjects[TEXTURE::AOTex]->setting(GL_LINEAR, GL_CLAMP_TO_BORDER, 1);	
}

void OpenGLWindow::initVolumeColorTextureParameters() {
	textureObjects[VolumeColorTex]->initParameters(
		VolumeColorTex, 
		GL_TEXTURE_1D, 
		maxTexSize,
		GL_RGBA8,  
		GL_RGBA, 
		GL_UNSIGNED_BYTE);
	textureObjects[VolumeColorTex]->setting(GL_NEAREST, GL_CLAMP_TO_EDGE, 1);
}

void OpenGLWindow::initMaskColorTextureParameters() {
	textureObjects[MaskColorTex]->initParameters(
		MaskColorTex, 
		GL_TEXTURE_1D, 
		256,	//Note: size of unsigned char for easy texture coordinate conversion
		GL_RGBA8, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE);
	textureObjects[MaskColorTex]->setting(GL_NEAREST, GL_CLAMP_TO_EDGE, 1);
}

void OpenGLWindow::initAOIntensityTextureParameters() {
	textureObjects[AOIntensityTex]->initParameters(
		AOIntensityTex,
		GL_TEXTURE_1D,
		maxTexSize,
		GL_R8,
		GL_RED,
		GL_UNSIGNED_BYTE);
	textureObjects[AOIntensityTex]->setting(GL_LINEAR, GL_CLAMP_TO_EDGE, 1);
}

void OpenGLWindow::initScreenTexturesParameters()
{
	glm::ivec2 texDimensions = (glm::ivec2)textureScreenDimensions;
	textureObjects[RayStartPosTex]->initParameters(
		RayStartPosTex,
		GL_TEXTURE_2D, 
		texDimensions,
		GL_RGBA32F, 
		GL_RGBA, 
		GL_FLOAT);
	textureObjects[RayStartPosTex]->setting(GL_NEAREST, GL_CLAMP_TO_BORDER, 4);

	textureObjects[RayDirAndDistTex]->initParameters(
		RayDirAndDistTex,
		GL_TEXTURE_2D,
		texDimensions,
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT);
	textureObjects[RayDirAndDistTex]->setting(GL_NEAREST, GL_CLAMP_TO_BORDER, 4);

	textureObjects[FinalTex]->initParameters(
		FinalTex,
		GL_TEXTURE_2D, 
		texDimensions,
		GL_RGBA, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE);
	textureObjects[FinalTex]->setting(GL_LINEAR, GL_CLAMP_TO_BORDER, 4);

	textureObjects[OverviewTex]->initParameters(
		OverviewTex,
		GL_TEXTURE_2D, 
		texDimensions,
		GL_RGBA, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE);
	textureObjects[OverviewTex]->setting(GL_LINEAR, GL_CLAMP_TO_BORDER, 4);	
}

void OpenGLWindow::initFrameAndDepthBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, backFramebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureObjects[RayStartPosTex]->id, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, textureObjects[RayDirAndDistTex]->id, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, textureObjects[FinalTex]->id, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, textureObjects[OverviewTex]->id, 0);

	//Set depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, backFramebuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (int)textureScreenDimensions.x, (int)textureScreenDimensions.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

	//Bind current context to framebuffer
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void OpenGLWindow::updateMaxTexSize() {
	GLint maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	maxTexSize = maxTextureSize;
}

void OpenGLWindow::initVolumeTextureParameters(bool volumeCompressed, DataInput dataType) {
	int width, height, depth;

	// initialize volume texture parameter
	width = world->volume.vData()->width();
	height = world->volume.vData()->height();
	depth = world->volume.vData()->depth();
	glm::ivec3 volumeDimensions(width, height, depth);

	initVolumeTextureParameters(volumeDimensions, volumeCompressed, dataType);
	initAOTextureParameters(volumeDimensions);

	// initialize mask texture parameter
	width = world->volume.vMask()->width();
	height = world->volume.vMask()->height();
	depth = world->volume.vMask()->depth();
	glm::ivec3 maskDimensions(width, height, depth);
	initMaskTextureParameters(maskDimensions);
}

void OpenGLWindow::updateVolumeTexture() {
	textureObjects[VolumeTex]->update(world->volume.vData()->data());	
}

void OpenGLWindow::updateVolumeTextureSubsection() {
	textureObjects[VolumeTex]->update(
		world->volume.maskUpdateInfo->volumeSubsection->data(),
		world->volume.maskUpdateInfo->idxMin,
		world->volume.maskUpdateInfo->idxMax);
}

void OpenGLWindow::updateMaskTexture() {
	textureObjects[MaskTex]->update(world->volume.vMask()->data());
}

void OpenGLWindow::updateMaskTextureSubsection() {	
	textureObjects[MaskTex]->update(
		world->volume.maskUpdateInfo->maskSubsection.data(),
		world->volume.maskUpdateInfo->idxMin,
		world->volume.maskUpdateInfo->idxMax);

	updateVolumeTextureSubsection();
}

void OpenGLWindow::updateTransferFunctionTexture() {
	textureObjects[VolumeColorTex]->update(colorTF_RGBA.data());
	colorTF_RGBA.clear();
}

void OpenGLWindow::updateMaskColorsTexture() {
	textureObjects[MaskColorTex]->update(colorGroups_RGBA.data());
	colorGroups_RGBA.clear();
}

void OpenGLWindow::updateAOIntensityTexture() {
	textureObjects[AOIntensityTex]->update(aoIntensity_A.data());
	aoIntensity_A.clear();
}

void OpenGLWindow::updateScreenTextures() {	
	textureObjects[RayDirAndDistTex]->update();	
	textureObjects[RayStartPosTex]->update();
	textureObjects[FinalTex]->update();
	textureObjects[OverviewTex]->update();
}

void OpenGLWindow::initTextureObjects() {
	for (int i = 0; i < textureObjects.size(); i++) {
		textureObjects[i]->init();
	}
	updateMaxTexSize();
	initVolumeColorTextureParameters();
	initMaskColorTextureParameters();
	initAOIntensityTextureParameters();
	initScreenTexturesParameters();	
}

void OpenGLWindow::updateTextures() {
	if (queueUpdateTextureVolumeColor) {
		updateTransferFunctionTexture();
		queueUpdateTextureVolumeColor = false;
		if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
			queueUpdateOverviewImages = true;
		}
	}

	if (queueUpdateTextureMaskColor) {
		updateMaskColorsTexture();
		queueUpdateTextureMaskColor = false;
		if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
			queueUpdateOverviewImages = true;
		}
	}

	if (world->volume.maskUpdateInfo->queueTextureMaskUpdate) {
		// Update portion of mask
		updateMaskTextureSubsection();
		world->volume.maskUpdateInfo->queueTextureMaskUpdate = false;
		if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
			queueUpdateOverviewImages = true;
		}
	}	

	if (queueUpdateTextureMask) { 
		// Update entire mask
		updateMaskTexture(); 
		queueUpdateTextureMask = false;
		if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
			queueUpdateOverviewImages = true;
		}	
	}

	if (queueUpdateTextureVolume) {
		updateVolumeTexture();
		queueUpdateTextureVolume = false;
	}
	
	if (queueUpdateTextureAOIntensity) {
		updateAOIntensityTexture();
		queueUpdateTextureAOIntensity = false;		
		if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
			queueUpdateOverviewImages = true;
		}
	}

	//***Note: Rendering directly into GPU may cause problem
	// for computers with small GPU memory?
	if (queueUpdateTextureVolumeAO) {
		updateLightIntensityVolume();		
		queueUpdateTextureVolumeAO = false;
		if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
			queueUpdateOverviewImages = true;
		}
	}
	if (queueUpdateTextureScreen) { 		
		initScreenTexturesParameters();
		updateScreenTextures(); 
		initFrameAndDepthBuffer();
		queueUpdateTextureScreen = false;
	}
}

//===

void OpenGLWindow::initRO_box() {	
	//Note: box has unit length for each side ([-0.5,0.5])
	float boxWidth = world->volume.vData()->boxWidth();
	float boxHeight = world->volume.vData()->boxHeight();
	float boxDepth = world->volume.vData()->boxDepth();	

	vo_box.generate();	
	vo_box.scaleVTX(glm::vec3(boxWidth, boxHeight, boxDepth));
	vo_box.initRO(*renderObjects[Box]);
//====
	//VOSphere vo_sphere;
	//vo_sphere.generate();
	//vo_sphere.initRO(*renderObjects[Box]);
	//glm::vec3 vMin = glm::vec3(-0.5,-0.5,-0.5);
	//glm::vec3 vMax = glm::vec3(0.5, 0.5, 0.5);
	//vo_sphere.updateVTX(vMin, vMax);

	//// Scale the vertices by dimension of box
	//float boxWidth = world->volume.vData()->boxWidth();
	//float boxHeight = world->volume.vData()->boxHeight();
	//float boxDepth = world->volume.vData()->boxDepth();
	//vo_sphere.scaleVTX(glm::vec3(boxWidth, boxHeight, boxDepth));
	//renderObjects[Box]->updateVBO(vo_sphere.vtx);
}

void OpenGLWindow::initRO_quad() {
	vo_quad.generate();
	vo_quad.initRO(*renderObjects[Quad]);
}

void OpenGLWindow::initRO_sphere(float scale) {
	VOSphere vo_sphere;
	vo_sphere.generate();
	vo_sphere.scaleVTX(scale);
	vo_sphere.initRO(*renderObjects[Sphere]);
}

void OpenGLWindow::initRO_cylinder(float scale) {
	VOCylinder vo_cylinder;
	vo_cylinder.generate();
	vo_cylinder.scaleVTX(scale);
	vo_cylinder.initRO(*renderObjects[Cylinder]);
}

void OpenGLWindow::initRO_frustum() {
	VertexObject frusLinesNearVO;
	frusLinesNearVO.vtx = frus.v_near;
	frusLinesNearVO.clr = frus.c_near;
	frusLinesNearVO.initRO(*renderObjects[FrusLinesNear]);
	renderObjects[FrusLinesNear]->size = frus.v_near.size();
	
	VertexObject frusLinesFarVO;
	frusLinesFarVO.vtx = frus.v_far;
	frusLinesFarVO.clr = frus.c_far;
	frusLinesFarVO.initRO(*renderObjects[FrusLinesFar]);
	renderObjects[FrusLinesFar]->size = frus.v_far.size();

	VertexObject frusBoxVO;
	auto box = frus.getBox();
	frusBoxVO.vtx.swap(box);
	frusBoxVO.initRO(*renderObjects[FrusBox]);
}

void OpenGLWindow::initRO_arrow(float scale) {
	VOArrow vo_arrow;
	vo_arrow.genArrow();
	vo_arrow.scaleVTX(scale * glm::vec3(0.1f, 0.1f, 0.2f));
	vo_arrow.genNML();
	vo_arrow.initRO(*renderObjects[Arrow]);
}

void OpenGLWindow::initRO_forceLine() {	
	VertexObject forceLineVO;
	forceLineVO.vtx = { world->probe.pos(), world->proxy.pos()};
	forceLineVO.clr = { glm::vec3(1,0,0),glm::vec3(1,0,0) };
	forceLineVO.initRO(*renderObjects[ForceLine]);
	renderObjects[ForceLine]->size = 2;
}

void OpenGLWindow::initRenderObjects() {	
	for (int i = 0; i < renderObjects.size(); i++) {
		renderObjects[i]->init();
	}		
	initRO_quad();
	initRO_frustum();
	initRO_arrow(0.05);		//*** This should be in reference to something
}



//void OpenGLWindow::updateROWorldScale() {
//	initRO_box();
//	initRO_sphere(world->worldScale);
//	initRO_cylinder(world->worldScale);
//}

void OpenGLWindow::updateROBoxMinMax(const glm::vec3& cutMin, const glm::vec3& cutMax) {	
	// Center the coordinates from [0,1] to [-0.5,0.5]
	glm::vec3 half(0.5, 0.5, 0.5);
	glm::vec3 vMin = cutMin - half;
	glm::vec3 vMax = cutMax - half;
	vo_box.updateVTX(vMin, vMax);
	
	// Scale the vertices by dimension of box
	float boxWidth = world->volume.vData()->boxWidth();
	float boxHeight = world->volume.vData()->boxHeight();
	float boxDepth = world->volume.vData()->boxDepth();
	vo_box.scaleVTX(glm::vec3(boxWidth, boxHeight, boxDepth));
	renderObjects[Box]->updateVBO(vo_box.vtx);
	//=========
	//VOSphere vo_sphere;
	//vo_sphere.generate();
	//vo_sphere.initRO(*renderObjects[Box]);
	//glm::vec3 vMin = glm::vec3(-0.5,-0.5,-0.5);
	//glm::vec3 vMax = glm::vec3(0.5, 0.5, 0.5);
	//vo_sphere.updateVTX(vMin, vMax);

	//// Scale the vertices by dimension of box
	//float boxWidth = world->volume.vData()->boxWidth();
	//float boxHeight = world->volume.vData()->boxHeight();
	//float boxDepth = world->volume.vData()->boxDepth();
	//vo_sphere.scaleVTX(glm::vec3(boxWidth, boxHeight, boxDepth));
	//renderObjects[Box]->updateVBO(vo_sphere.vtx);
}

void OpenGLWindow::updateROQuadMinMax(const glm::vec2& coordMin, const glm::vec2& coordMax) {
	vo_quad.updateVTX(coordMin, coordMax);
	renderObjects[Quad]->updateVBO(vo_quad.vtx);
}

void OpenGLWindow::updateROFrustumLines() {
	renderObjects[FrusLinesNear]->updateVBO(frus.v_near);
	renderObjects[FrusLinesNear]->updateCLR(frus.c_near);
	renderObjects[FrusLinesNear]->size = frus.v_near.size();
	
	renderObjects[FrusLinesFar]->updateVBO(frus.v_far);
	renderObjects[FrusLinesFar]->updateCLR(frus.c_far);
	renderObjects[FrusLinesFar]->size = frus.v_far.size();
}

void OpenGLWindow::updateROFrustumBox() {
	std::vector<glm::vec3> frus_v = frus.getBox();
	renderObjects[FrusBox]->updateVBO(frus_v);
	renderObjects[FrusBox]->size = frus_v.size();
}

void OpenGLWindow::updateRenderObjects() {
	if (queueUpdateROBoxMinMax) { 
		updateROBoxMinMax(visualParams->cutMin, visualParams->cutMax);
		queueUpdateROBoxMinMax = false;
	}
}

//====================

void OpenGLWindow::initScene() {
	// Render Objects
	initRO_box();
	initRO_sphere(world->worldScale);
	initRO_cylinder(world->worldScale);

	// AO rays
	initLightIntensityVolume();

	// Camera Screen	
	int screenWidth = this->width();
	int screenHeight = this->height();
	updateScreen(screenWidth, screenHeight);

	// Textures & shader
	queueUpdateTextureVolume = true;
	queueUpdateTextureMask = true;
	queueUpdateTextureScreen = true;
	
	queueUpdateShaderBoxDimensions = true;
	queueUpdateShaderCutPlane = true;
	queueUpdateShaderScreen = true;

	queueUpdateROBoxMinMax = true;

	queueUpdateOverviewImages = true;
	queueUpdateMaskEnabled = true;

	//Update timer
	timer = std::chrono::high_resolution_clock::now();

	// Update texture maps (maskColor, volumeColor, AO)
	emit initialized();
}

void OpenGLWindow::updateCameraScreen(int screenWidth, int screenHeight, CameraScreen& camScreen) {
	camScreen.updateScreenRatio(screenWidth, screenHeight);
	if (camScreen.isOrtho) {
		float distY = world->volume.longestLength() / 2.f;
		camScreen.updateOrtho(distY);
	}
	else {
		camScreen.updatePerspective(camScreen.fov());
	}
}


void OpenGLWindow::updateTextureScreenDimensionsScreenshot(int screenWidth, int screenHeight, float scale) {	
	// Update texture screen dimensions
	textureScreenDimensions = glm::vec2(
		screenWidth,
		screenHeight);
	textureScreenDimensions *= scale;
}


void OpenGLWindow::updateTextureScreenDimensions(int screenWidth, int screenHeight, int screenType) {
	// Set scale of screen based on compression and screen type
	glm::vec2 scale;	
	if (screenType == Cameras::SCR_FULL) { scale = glm::vec2(1, 1) * compressionRatio; }
	else if (screenType == Cameras::SCR_STEREO) { scale = glm::vec2(0.5, 1) * compressionRatio; }
	else if (screenType == Cameras::SCR_OVERVIEW) { scale = glm::vec2(0.5, 0.5) * compressionRatio; }

	// Update texture screen dimensions
	textureScreenDimensions = glm::vec2(
		screenWidth,
		screenHeight);
	textureScreenDimensions *= scale;
}

void OpenGLWindow::updateScreen(int screenWidth, int screenHeight) {
	// Update projection for all cameras
	for (int i = 0; i < world->cams.size(); i++) {
		updateCameraScreen(screenWidth, screenHeight, world->cams[i]);
	}

	// Update texture to screen width and height
	updateTextureScreenDimensions(screenWidth, screenHeight, world->cams.getScreenType());

	// Queue updates screen size on textures and shaders
	queueUpdateShaderScreen = true;
	queueUpdateTextureScreen = true;
	if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) {
		queueUpdateOverviewImages = true;
	}
}

void OpenGLWindow::updateSSDimensions(int w, int h) {
	ssParams->screenWidth = width();
	ssParams->screenHeight = height();
	if (ssParams->keepAspectRatio) {
		ssParams->ssHeight = 0;
		ssParams->updateUndefinedDimensions();
	}
}

void OpenGLWindow::updateOverviewImages() {	
	// Draw scenes on images
	glm::ivec2 texDimensions = (glm::ivec2)textureScreenDimensions;

	glBindTexture(GL_TEXTURE_2D, textureObjects[FinalTex]->id);
	render3DScene(world->cams[Cameras::CAM_OVERVIEW_TOP]);
	saveTextureToImage(overviewImageTop, texDimensions);

	render3DScene(world->cams[Cameras::CAM_OVERVIEW_SIDE]);
	saveTextureToImage(overviewImageSide, texDimensions);

	render3DScene(world->cams[Cameras::CAM_OVERVIEW_FRONT]);
	saveTextureToImage(overviewImageFront, texDimensions);	
}

void OpenGLWindow::updateImageSize() {
	// Update texture and shader parameters that changes with texture parameter
	initScreenTexturesParameters();
	updateScreenTextures();
	initFrameAndDepthBuffer();
	updateShaderScreenSize();
}

void OpenGLWindow::renderImage(int imageWidth, int imageHeight, CameraScreen& camScreen, std::vector<GLubyte>& out_image) {
	// Modify the current texture sizes to desired image output size		
	updateTextureScreenDimensionsScreenshot(imageWidth, imageHeight, 1.f);
	updateCameraScreen(imageWidth, imageHeight, camScreen);

	// Update the dimensions of texture to the size of image	
	updateImageSize();
	
	// Render scene	to texture & save RGBA values to list	
	glBindTexture(GL_TEXTURE_2D, textureObjects[FinalTex]->id);
	render3DScene(camScreen);
	saveTextureToImage(out_image, glm::ivec2(imageWidth, imageHeight));

	// Reset to screen size
	int screenWidth = this->width();
	int screenHeight = this->height();	
	updateTextureScreenDimensions(screenWidth, screenHeight, world->cams.getScreenType());
	updateCameraScreen(textureScreenDimensions.x, textureScreenDimensions.y, camScreen);
	updateImageSize();
}

void OpenGLWindow::updateOthers() {
	if (queueUpdateOverviewImages) {		
		updateOverviewImages();
		queueUpdateOverviewImages = false;
	}
	if (queueScreenshotMainCamera > 0) {	
		std::vector<GLubyte> image;
		if (queueScreenshotMainCamera == 1) {
			renderImage(ssParams->ssWidth, ssParams->ssHeight, world->cams[Cameras::CAM_MAIN], image);
			saveImageToFile(image, glm::ivec2(ssParams->ssWidth, ssParams->ssHeight),
				ssParams->fileDir + ssParams->fileName);
		}

		//====
		else if (queueScreenshotMainCamera == 2) {
			//Make two part image using 2 separate cameras
			CameraScreen cam1 = world->cams[Cameras::CAM_MAIN];
			CameraScreen cam2 = world->cams[Cameras::CAM_MAIN];
			cam2.updateClipNear(cam1.clippingPlaneFar());
			cam2.updateClipFar(cam1.farPlane());

			renderImage(ssParams->ssWidth, ssParams->ssHeight, cam1, image);
			saveImageToFile(image, glm::ivec2(ssParams->ssWidth, ssParams->ssHeight),
				ssParams->fileDir + ssParams->fileName);

			renderImage(ssParams->ssWidth, ssParams->ssHeight, cam2, image);
			saveImageToFile(image, glm::ivec2(ssParams->ssWidth, ssParams->ssHeight),
				ssParams->fileDir + ssParams->fileName + "-rear.png");
		}

		queueScreenshotMainCamera = 0;
		QString str = "Screenshot finished: " + QString::fromStdString(ssParams->fileName);
		statusWriteTimedMessage(str);
		qDebug() << str;
	}

	if (queueUpdateMaskEnabled) {
		shaders[RayCastAO]->useProgram();
		shaders[RayCastAO]->setIntArray("maskEnabled", maskEnabled);
		queueUpdateMaskEnabled = false;
	}
}

void initRayList(VolumeBase* vData, std::vector<glm::vec3>& out_rays) {
	std::vector<glm::vec3> rays;
		
	rays.push_back(glm::vec3(0.000000, -1.000000, 0.000000));
	rays.push_back(glm::vec3(0.723607, -0.447220, 0.525725));
	rays.push_back(glm::vec3(-0.276388, -0.447220, 0.850649));
	rays.push_back(glm::vec3(-0.894426, -0.447216, 0.000000));
	rays.push_back(glm::vec3(-0.276388, -0.447220, -0.850649));
	rays.push_back(glm::vec3(0.723607, -0.447220, -0.525725));
	rays.push_back(glm::vec3(0.276388, 0.447220, 0.850649));
	rays.push_back(glm::vec3(-0.723607, 0.447220, 0.525725));
	rays.push_back(glm::vec3(-0.723607, 0.447220, -0.525725));
	rays.push_back(glm::vec3(0.276388, 0.447220, -0.850649));
	rays.push_back(glm::vec3(0.894426, 0.447216, 0.000000));
	rays.push_back(glm::vec3(0.000000, 1.000000, 0.000000));
	rays.push_back(glm::vec3(-0.162456, -0.850654, 0.499995));
	rays.push_back(glm::vec3(0.425323, -0.850654, 0.309011));
	rays.push_back(glm::vec3(0.262869, -0.525738, 0.809012));
	rays.push_back(glm::vec3(0.850648, -0.525736, 0.000000));
	rays.push_back(glm::vec3(0.425323, -0.850654, -0.309011));
	rays.push_back(glm::vec3(-0.525730, -0.850652, 0.000000));
	rays.push_back(glm::vec3(-0.688189, -0.525736, 0.499997));
	rays.push_back(glm::vec3(-0.162456, -0.850654, -0.499995));
	rays.push_back(glm::vec3(-0.688189, -0.525736, -0.499997));
	rays.push_back(glm::vec3(0.262869, -0.525738, -0.809012));
	rays.push_back(glm::vec3(0.951058, 0.000000, 0.309013));
	rays.push_back(glm::vec3(0.951058, 0.000000, -0.309013));
	rays.push_back(glm::vec3(0.000000, 0.000000, 1.000000));
	rays.push_back(glm::vec3(0.587786, 0.000000, 0.809017));
	rays.push_back(glm::vec3(-0.951058, 0.000000, 0.309013));
	rays.push_back(glm::vec3(-0.587786, 0.000000, 0.809017));
	rays.push_back(glm::vec3(-0.587786, 0.000000, -0.809017));
	rays.push_back(glm::vec3(-0.951058, 0.000000, -0.309013));
	rays.push_back(glm::vec3(0.587786, 0.000000, -0.809017));
	rays.push_back(glm::vec3(0.000000, 0.000000, -1.000000));
	rays.push_back(glm::vec3(0.688189, 0.525736, 0.499997));
	rays.push_back(glm::vec3(-0.262869, 0.525738, 0.809012));
	rays.push_back(glm::vec3(-0.850648, 0.525736, 0.000000));
	rays.push_back(glm::vec3(-0.262869, 0.525738, -0.809012));
	rays.push_back(glm::vec3(0.688189, 0.525736, -0.499997));
	rays.push_back(glm::vec3(0.162456, 0.850654, 0.499995));
	rays.push_back(glm::vec3(0.525730, 0.850652, 0.000000));
	rays.push_back(glm::vec3(-0.425323, 0.850654, 0.309011));
	rays.push_back(glm::vec3(-0.425323, 0.850654, -0.309011));
	rays.push_back(glm::vec3(0.162456, 0.850654, -0.499995));

	// Scale ray by length in world space
	float scaleX = 1.f / vData->boxWidth();
	float scaleY = 1.f / vData->boxHeight();
	float scaleZ = 1.f / vData->boxDepth();
	float biggestScale = 0;
	if (biggestScale < scaleX) { biggestScale = scaleX; }
	if (biggestScale < scaleY) { biggestScale = scaleY; }
	if (biggestScale < scaleZ) { biggestScale = scaleZ; }
	scaleX /= biggestScale;
	scaleY /= biggestScale;
	scaleZ /= biggestScale;

	for (int i = 0; i < rays.size(); i++) {
		glm::vec3 ray = glm::normalize(rays[i]);		
		ray = glm::vec3(ray.x * scaleX, ray.y * scaleY, ray.z * scaleZ);
		out_rays.push_back(ray);
	}
}

void OpenGLWindow::initLightIntensityVolume() {	
	VolumeBase* vData = world->volume.vData();
	GLExt::ShaderProgram* shader = shaders[RayLightIntensity];

	// Update settings of VolumeAO in GPU
	textureObjects[AOTex]->update();

	// Set uniform variables in shader
	shader->useProgram();
	
	std::vector<glm::vec3> rays;
	initRayList(vData, rays);
	shader->setVec3Array("rays", rays);

	glm::vec3 dimensions = glm::vec3(vData->width(), vData->height(), vData->depth());
	shader->setVec3("dimensions", dimensions);
}

void OpenGLWindow::updateLightIntensityVolume() {
	VolumeBase* vData = world->volume.vData();
	
	GLExt::ShaderProgram* shader = shaders[RayLightIntensity];
	shader->useProgram();
	shader->setFloat("stepSize", lightParams->aoStepSize);
	shader->setInt("maxSteps", lightParams->aoMaxSteps);
	shader->setFloat("isoSurfaceThresh", isoParams->isoSurfaceThresh);

	glBindFramebuffer(GL_FRAMEBUFFER, volumeFramebuffer);	
	RenderObject* rObject = renderObjects[Quad];
	glBindVertexArray(rObject->vao);
	float zStep = 1.f / (float)(vData->depth() - 1);
	float uvZ = 0;
	glViewport(0, 0, vData->width(), vData->height());

	// Render to texture	
	for (int zSlice = 0; zSlice < vData->depth(); zSlice++) {
		glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D,
			textureObjects[TEXTURE::AOTex]->id, 0, zSlice);			
		shader->setFloat("UVz", uvZ);

		glDrawElements(
			GL_TRIANGLES,      	// mode
			rObject->size,		// count
			GL_UNSIGNED_SHORT,  // state
			(void*)0           	// element array buffer offset
			);

		uvZ += zStep;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	qDebug() << "AO texture updated";
}

void OpenGLWindow::updateMaskEnabled(ColorGroups& groups) {
	for (int i = 1; i < maskEnabled.size(); i++) {
		maskEnabled[i] = groups[i].displayMask;
	}
	maskEnabled[0] = 0;	// Show TF by default
	queueUpdateMaskEnabled = true;
}

void OpenGLWindow::updateCompressionRatio(float ratio) {
	if (compressionRatio != ratio) {
		compressionRatio = ratio;
		updateScreen(width(), height());
	}
}

void OpenGLWindow::updateFPS(float fps) {
	microsecondsPerFrame = (1.f / fps) * 1000000.f;
}


glm::mat4 getArrowM(const glm::vec3& dest) {
	glm::vec3 start = glm::vec3(0, 0, 1);	//camera direction
	glm::quat q = MathLib::RotationBtwnVectors(start, dest);
	return glm::toMat4(q);
}
//=====================

OpenGLWindow::OpenGLWindow(QObject* parent, World* worldObject,
	LightParams* _lightParams, VisualParams* _visualParams, 
	ScreenshotParams* _ssParams, RenderParams* _renderParams,
	IsoParams* _isoParams)
	: QOpenGLWindow(),
	lightParams(_lightParams),
	visualParams(_visualParams),
	ssParams(_ssParams),
	renderParams(_renderParams),
	isoParams(_isoParams),
	openGLContext(0),
	backFramebuffer(0),
	volumeFramebuffer(0),
	depthbuffer(0)
{
	parentWidget = parent;
	world = worldObject;

	// Create shader objects
	shaders.resize(Shader::COUNT_SHADER);
	for (int i = 0; i < shaders.size(); i++) {
		shaders[i] = new GLExt::ShaderProgram();
	}

	// Create texture objects	
	textureObjects.resize(TEXTURE::COUNT_TEXTURE);	
	for (int i = 0; i < textureObjects.size(); i++) {
		textureObjects[i] = new TextureObject();
	}

	// Create render objects
	renderObjects.resize(COUNT_RENDER);
	for (int i = 0; i < renderObjects.size(); i++) {
		renderObjects[i] = new RenderObject();
	}

	// initialize queue update parameters
	queueInitScene = false;
	queueUpdateROBoxMinMax = false;

	queueUpdateTextureVolume = false;
	queueUpdateTextureMask = false;
	queueUpdateTextureVolumeAO = false;
	queueUpdateTextureVolumeColor = false;
	queueUpdateTextureMaskColor = false;
	queueUpdateTextureAOIntensity = false;
	queueUpdateTextureScreen = false;

	queueUpdateShaderBoxDimensions = false;
	queueUpdateShaderCutPlane = false;
	queueUpdateShaderScreen = false;
	queueUpdateShaderInvVolume = false;

	queueUpdateOverviewImages = false;
	queueScreenshotMainCamera = 0;
	queueUpdateMaskEnabled = false;
	
	compressionRatio = 1;	
	microsecondsPerFrame = 33333;
	backgroundColour = glm::vec4(0, 0, 0, 0);
	maskIntensity = 0;
	maxTexSize = 0;
	maskEnabled.resize(10, 1);
	blendRate = visualParams->blendRate;
	blendSize = visualParams->blendSize;
}
	
OpenGLWindow::~OpenGLWindow() {
	for (auto obj : textureObjects) { delete obj; }
	for (auto obj : renderObjects) { delete obj; }
	for (auto obj : shaders) { delete obj; }

	if (backFramebuffer != 0) { glDeleteFramebuffers(1, &backFramebuffer); }
	if (volumeFramebuffer != 0) { glDeleteFramebuffers(1, &volumeFramebuffer); }
	if (depthbuffer != 0) { glDeleteRenderbuffers(1, &depthbuffer); }
}

void OpenGLWindow::initializeGL() {
	initializeOpenGLFunctions();

	openGLContext = context()->currentContext();		
	glGenFramebuffers(1, &backFramebuffer);
	glGenFramebuffers(1, &volumeFramebuffer);
	glGenRenderbuffers(1, &depthbuffer);

	initShaders();	
	initRenderObjects();
	initTextureObjects();		
}

void OpenGLWindow::resizeGL(int w, int h) {	
	updateScreen(w, h);
	updateSSDimensions(w, h);
	emit resized();
}

void OpenGLWindow::paintGL() {	
	
	queueUpdateShaderInvVolume = true;

	// Update Data in GPU
	if(queueInitScene) { 
		initScene(); 
		queueInitScene = false;
	}
	updateRenderObjects();
	updateTextures();
	updateShaders();
	updateOthers();	

	//Render on screen
	glClear(GL_DEPTH_BUFFER_BIT);
	render();

	// Delay based on setting FPS	
	MathLib::limitRefreshRate(microsecondsPerFrame, timer);

	// Queue the next update
	update();			
}

void OpenGLWindow::updateScreenSection(unsigned int screenSection) {
	glm::vec2 coordMin = world->cams.getSectionMin(screenSection);
	glm::vec2 coordMax = world->cams.getSectionMax(screenSection);
	updateROQuadMinMax(coordMin, coordMax);
}

void OpenGLWindow::render() {
	if (world->cams.getScreenType() == Cameras::SCR_FULL) { renderFull(); }
	else if (world->cams.getScreenType() == Cameras::SCR_STEREO) { renderStereo(); }
	else if (world->cams.getScreenType() == Cameras::SCR_OVERVIEW) { renderOverview(); }
}

void OpenGLWindow::setDrawBuffer(GLenum mode) {
	glDrawBuffer(mode);		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLWindow::render3DScene(CameraScreen& camScreen) {	
	bool objEnabled = camScreen.isObjectEnabled;
	bool volEnabled = camScreen.isVolumeEnabled;
	
	// Scale the viewport to size of texture
	glViewport(0, 0, textureScreenDimensions.x, textureScreenDimensions.y);
	glBindFramebuffer(GL_FRAMEBUFFER, backFramebuffer);
	glClearColor(0, 0, 0, 0);	//Note: This is a setting, not an execution

	//Render on RayStartPos texture(0)
	setDrawBuffer(GL_COLOR_ATTACHMENT0);
	renderTextureRayStartPos(camScreen);
	
	//Render on RayDirAndDist texture(1)
	setDrawBuffer(GL_COLOR_ATTACHMENT1);
	if (volEnabled) renderTextureRayDirAndDist_Volume(camScreen);
	if (objEnabled) renderTextureRayDirAndDist_VertexObjects(camScreen);

	// Render on FinalImage texture(2)
	glClearColor(backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
	setDrawBuffer(GL_COLOR_ATTACHMENT2);	
	glEnable(GL_BLEND);		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (objEnabled) renderTextureVertexObjects(camScreen);
	if (volEnabled) renderTextureRayCast(camScreen);	
	glDisable(GL_BLEND);

	renderTextureOverlay(camScreen);



}

void OpenGLWindow::renderOverviewScene(std::vector<GLubyte>& image, CameraScreen& camScreen) {
	// Load preloaded image from one of the overview cameras
	glm::ivec2 texDimensions = (glm::ivec2)textureScreenDimensions;	
	loadImageToTexture(image, texDimensions, textureObjects[OverviewTex]);

	// Update the lines of frustum in respect to camera direction
	glm::mat4 MVP = world->getVP(camScreen);
	frus.updateLines(camScreen.cam());
	updateROFrustumLines();		
	
	// Render preloaded image on final texture
	glBindFramebuffer(GL_FRAMEBUFFER, backFramebuffer);
	glViewport(0, 0, textureScreenDimensions.x, textureScreenDimensions.y);
	setDrawBuffer(GL_COLOR_ATTACHMENT2);
	
	renderShaderImageTexture(renderObjects[Quad], TEXTURE::OverviewTex, OVERVIEW_BKGD_OPACITY);
	renderShaderImageObject(renderObjects[FrusBox], MVP, TEXTURE::OverviewTex, 1.f);	
	
	// Overlay lines on top of final image	
	renderShaderLinesStippled(renderObjects[FrusLinesFar], MVP, OVERVIEW_PIXEL_GAP, OVERVIEW_LINE_WIDTH);
	renderShaderLinesSolid(renderObjects[FrusLinesNear], MVP, OVERVIEW_LINE_WIDTH);
}

void OpenGLWindow::renderFinalImageToScreen(unsigned int screenSection) {	
	glBindFramebuffer(GL_FRAMEBUFFER, openGLContext->defaultFramebufferObject());
	glm::ivec2 dim = getScreenDimensions();
	glViewport(0, 0, dim.x, dim.y);		// Set the viewport to the size of default framebuffer
	updateScreenSection(screenSection);	// Set the size of quad to desired part of screen
	
	// Copy final image to framebuffer (to location of quad)
	renderShaderImageTexture(
		renderObjects[Quad],
		TEXTURE::FinalTex,
		1.f);							
	
	updateScreenSection(Cameras::SEC_FULL);	// Restore to default size of quad (i.e. full screen)
}

void OpenGLWindow::renderFull() {
	// Draw scenes
	render3DScene(world->cams[Cameras::CAM_MAIN]);
	renderFinalImageToScreen(Cameras::SEC_FULL);
}

void OpenGLWindow::renderStereo() {
	// Draw scenes
	render3DScene(world->cams[Cameras::CAM_STEREO_LEFT]);
	renderFinalImageToScreen(Cameras::SEC_LEFT);
	render3DScene(world->cams[Cameras::CAM_STEREO_RIGHT]);
	renderFinalImageToScreen(Cameras::SEC_RIGHT);
}

void OpenGLWindow::renderOverview() {
	// Overview cameras need to be constantly updated since they move with the volume
	world->cams.updateOverviewCameras(world->volume);

	// Update frustum vertices and box
	frus.updateVertices(world->cams[Cameras::CAM_MAIN]);
	updateROFrustumBox();	

	// Render each of the preloaded overview scenes
	renderOverviewScene(overviewImageTop,
		world->cams[Cameras::CAM_OVERVIEW_TOP]);
	renderFinalImageToScreen(Cameras::SEC_TOP_LEFT);

	renderOverviewScene(overviewImageSide,  
		world->cams[Cameras::CAM_OVERVIEW_SIDE]);
	renderFinalImageToScreen(Cameras::SEC_TOP_RIGHT);

	renderOverviewScene(overviewImageFront,
		world->cams[Cameras::CAM_OVERVIEW_FRONT]);
	renderFinalImageToScreen(Cameras::SEC_BOT_LEFT);

	// Render 3D scene
	render3DScene(world->cams[Cameras::CAM_MAIN]);
	renderFinalImageToScreen(Cameras::SEC_BOT_RIGHT);
}

void OpenGLWindow::renderShaderVertexUniform(RenderObject* rObject, const glm::mat4& MVP, const glm::vec3& color) {
	GLExt::ShaderProgram* shader = shaders[ObjColorUniform];
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setVec3("color", color);
	shader->setFloat("alpha", 1);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size,	// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);
}

void OpenGLWindow::renderShaderRayStart(
	GLExt::ShaderProgram* shader, RenderObject* rObject, 
	const glm::mat4& MVP, const glm::mat4& M, bool rayEnabled) {
	//Note: color = position in volume space, alpha = ray cast enabled/disabled for pixel
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setFloat("rayEnabled", (float)rayEnabled);
	shader->setMat4("MVP", MVP);
	shader->setMat4("M", M);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size,    		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);
}

void OpenGLWindow::renderShaderRayEndStart(RenderObject* rObject,
	const glm::mat4& MVP) {

	GLExt::ShaderProgram* shader = shaders[RayEndAtStart];
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);

}

void OpenGLWindow::renderShaderRayEndObject(RenderObject* rObject,
	const glm::mat4& MVP, const glm::mat4& M) {
	// Note:  color = direction of ray through volume space; alpha = how far the ray travels

	GLExt::ShaderProgram* shader = shaders[RayEndAtObject];
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setMat4("M", M);
		
	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
	);
}

void OpenGLWindow::renderShaderVertexObjectLight(RenderObject* rObject, 
	const glm::mat4& MVP, const glm::mat4& MV, const glm::vec3& color) {
	GLExt::ShaderProgram* shader = shaders[ObjRender];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setMat4("MV", MV);
	shader->setVec3("inColor", color);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);
}

void OpenGLWindow::renderShaderVertexObjectUniform(RenderObject* rObject,
	const glm::mat4& MVP, const glm::vec3& color) {
	GLExt::ShaderProgram* shader = shaders[ObjColorUniform];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setVec3("color", color);
	shader->setFloat("alpha", 1);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);
}

void OpenGLWindow::renderShaderVertexObjectDepth(RenderObject* rObject,
	const glm::mat4& MVP, float depth) {
	GLExt::ShaderProgram* shader = shaders[ObjDepth];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setFloat("depth", depth);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
	);
}


void OpenGLWindow::renderShaderVertexObjectSilhouette(RenderObject* rObject,
	const glm::mat4& MVP, const glm::vec3& color, float depth) {
	GLExt::ShaderProgram* shader = shaders[ObjColorSilhouette];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setVec3("color", color);
	shader->setFloat("alpha", 1);
	shader->setFloat("depth", depth);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
	);
}

void OpenGLWindow::renderShaderImageTexture(RenderObject* rObject,
	int tex_num, float opacity) {
	GLExt::ShaderProgram* shader = shaders[ImgTexture];	
	glBindVertexArray(rObject->vao);
	shader->useProgram();	
	shader->setInt("imageTexture", tex_num);
	shader->setFloat("opacity", opacity);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size, 		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
	);
}

void OpenGLWindow::renderShaderImageObject(RenderObject* rObject,
	const glm::mat4& MVP, int tex_num, float opacity) {
	GLExt::ShaderProgram* shader = shaders[ImgObject];
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);	
	shader->setInt("imageTexture", tex_num);	
	shader->setFloat("opacity", opacity);

	glDrawArrays(GL_TRIANGLES, 0, rObject->size);
}

void OpenGLWindow::renderShaderLinesSolid(RenderObject* rObject,
	const glm::mat4& MVP, float lineWidth) {
	GLExt::ShaderProgram* shader = shaders[ObjColorVertex];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setFloat("alpha", 1);
	glLineWidth(lineWidth);
	glDrawArrays(GL_LINES, 0, rObject->size);
}

void OpenGLWindow::renderShaderLinesStippled(RenderObject* rObject,
	const glm::mat4& MVP, int pixelGap, float lineWidth) {
	GLExt::ShaderProgram* shader = shaders[ObjStippled];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setInt("pixelGap", pixelGap);
	shader->setMat4("MVP", MVP);
	
	//Draw the lines
	glLineWidth(lineWidth);
	glDrawArrays(GL_LINES, 0, rObject->size);
}

void OpenGLWindow::renderShaderRayCast(const glm::vec3& lightPosVS, const glm::vec3& camDirVS) {
	//Note: color & alpha is from ray-casted from RayStartPos, in the direction & distance of RayDirAndDist
	RenderObject* rObject = renderObjects[Quad];
	glBindVertexArray(rObject->vao);
	//GLExt::ShaderProgram* shader = shaders[RayCast];
	GLExt::ShaderProgram* shader = shaders[RayCastAO];
	shader->useProgram();
	shader->setVec3("lightPosVS", lightPosVS);
	shader->setVec3("camDirVS", camDirVS);
	shader->setFloat("mask_t", maskIntensity);	

	shader->setFloat("phong_exponent", lightParams->specularExponent);
	shader->setFloat("intensity_ambient", lightParams->intensityAmbient);
	shader->setFloat("intensity_diffuse", lightParams->intensityDiffuse);
	shader->setFloat("intensity_specular", lightParams->intensitySpecular);
	shader->setVec3("lightColor", lightParams->lightColor);
	shader->setInt("maxSteps", lightParams->rayMaxSteps);
	shader->setFloat("stepSize", lightParams->rayStepSize);

	shader->setVec3("backgroundColor", glm::vec3(backgroundColour));
	shader->setFloat("blendSize", blendSize);
	shader->setFloat("blendRate", blendRate);

	shader->setVec4("isoSurfaceColor", isoParams->isoSurfaceColor);
	shader->setFloat("isoSurfaceThresh", isoParams->isoSurfaceThresh);
	shader->setInt("isoSurfaceEnabled", isoParams->paintVisualMode > 0);

	shader->setVec3("paintMin", isoParams->paintIdxMinNormalized); 
	shader->setVec3("paintMax", isoParams->paintIdxMaxNormalized);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size,		// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
	);
}

void OpenGLWindow::renderTextureRayStartPos(CameraScreen& camScreen) {
	glm::mat4 volumeM = world->volume.M();
	glm::mat4 nearClippingM = world->getClippingMNear(camScreen);
	
	glm::mat4 MVP = world->getMVP(camScreen);
	glm::mat4 rayStartPosMVP = world->getMVP_rayStartPos(camScreen);
	glm::mat4 nearClippingMVP = world->getMVP_nearClipping(camScreen);
				
	// Draw objects that are further away from camera
	// Note: Depth of camera goes from 1 to 0 for RayStartPos							
	// ==== clipping plane	====//
	// Draw position of clipping plane
	// Note: skips drawing over ray-enabled pixels in RayStartPos texture that is behind the clipping plane
	renderShaderRayStart(
		shaders[RayStartFromObject],
		renderObjects[Quad],
		nearClippingMVP,
		nearClippingM,
		false);

	// ==== Enable rays around volume ====//
	// Activate rays in RayStartPos texture around where the box is 
	glDisable(GL_DEPTH_TEST);// Always enable rays in the region around volume
	// ==== volume position ====//	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);	// Only draw front face for rendering efficiency
							// Note: Drawing front is used for starting position of volume
	renderShaderRayStart(
		shaders[RayStartFromObject],
		renderObjects[Box],
		rayStartPosMVP,
		volumeM,
		true);
	glDisable(GL_CULL_FACE);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);		//Enable merge between newly drawn layer and the old one
	glBlendFuncSeparate(	//Take the colour of old layer, and take the alpha of newly drawn layer
		GL_ZERO,
		GL_ONE,
		GL_ONE,
		GL_ZERO);
	glCullFace(GL_FRONT);	// Skip drawing the front for rendering efficiency
							// Note: drawing the back is necessary to enable rays from inside volume
	renderShaderRayStart(
		shaders[RayStartFromObject],
		renderObjects[Box],
		MVP,
		volumeM,
		true);	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void OpenGLWindow::renderTextureRayDirAndDist_Volume(CameraScreen& camScreen) {
	glm::mat4 MVP = world->getMVP(camScreen);
	glm::mat4 volumeM = world->volume.M();
	glm::mat4 farClippingM = world->getClippingMFar(camScreen);
	glm::mat4 farClippingMVP = world->getMVP_farClipping(camScreen);

	// "Draw" direction and distance from back of the scene to the front
	// Note: Depth of camera goes from 0 to 1 for RayDirAndDist
	// Note:  color = direction of rays; alpha = how far the ray travel				
	// ==== clipping plane ====//		
	renderShaderRayEndObject(
		renderObjects[Quad],
		farClippingMVP,
		farClippingM);

	// ==== volume position ====//			
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);	// Draw up to the back of box
	renderShaderRayEndObject(
		renderObjects[Box],
		MVP,
		volumeM);
	glDisable(GL_CULL_FACE);
}

void OpenGLWindow::renderTextureRayDirAndDist_VertexObjects(CameraScreen& camScreen) {			
	glm::mat4 MVP, M;
	glm::mat4 probeM = world->probe.M();
	glm::mat4 silhouetteM = world->probe.M_silhouette();
	glm::mat4 volumeM = world->volume.M();
	glm::mat4 VP = world->getVP(camScreen);
	
	// "Draw" the ray distance up to the surface of object
	// Note: Depth of camera goes from 0 to 1 for RayDirAndDist
	// Note:  color = direction of rays; alpha = how far the ray travel				
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);	// Draw up to the surface of object

	//*** Redundant calculation with renderTextureVertexObjects
	// Nodes	
	for (NN::Node* n : world->nn.nodes) {
		if (!n->flag_disabled) {
			M = volumeM * n->M;
			MVP = VP * M;
			renderShaderRayEndObject(
				renderObjects[Sphere],
				MVP,
				M);
		}
	}

	// Links
	for (NN::Link* l : world->nn.links) {
		if (!l->flag_disabled) {
			M = volumeM * l->M;
			MVP = VP * M;
			renderShaderRayEndObject(
				renderObjects[Cylinder],
				MVP,
				M);

		}
	}
	
	// Probe
	MVP = VP * probeM;
	renderShaderRayEndObject(
		renderObjects[Sphere],
		MVP,
		probeM);
	glDisable(GL_CULL_FACE);
}

void OpenGLWindow::renderTextureVertexObjects(CameraScreen& camScreen) {
	glm::mat4 MVP, MV, M;
	glm::vec3 baseColor;
	glm::mat4 V = camScreen.cam().V();
	glm::mat4 VP = world->getVP(camScreen);
	glm::mat4 volumeM = world->volume.M();
	glm::mat4 probeM = world->probe.M();
	
	//*** Redundant calculation with RayDirAndDist
	glm::vec3 probePosVS = glm::vec3(world->volume.invM() * glm::vec4(world->probe.pos(), 1));
	NN::updateNearestNObjectState(probePosVS, &world->nn);

	// Nodes
	for (NN::Node* n: world->nn.nodes) {
		if (!n->flag_disabled) {
			M = volumeM * n->M;
			MV = V * M;
			MVP = VP * M;
			int colorGroupsIndex = n->groupNum * 4;
			baseColor = glm::vec3(
				colorGroups_RGBA[colorGroupsIndex] / 255.f,
				colorGroups_RGBA[colorGroupsIndex + 1] / 255.f,
				colorGroups_RGBA[colorGroupsIndex + 2] / 255.f);
			baseColor = NN::getTypeColor(baseColor, n->state);
			renderShaderVertexObjectLight(
				renderObjects[Sphere],
				MVP,
				MV,
				baseColor);			
		}
	}

	// Links
	for (NN::Link* l : world->nn.links) {
		if (!l->flag_disabled) {
			M = volumeM * l->M;
			MV = V * M;
			MVP = VP * M;
			int colorGroupsIndex = l->groupNum * 4;
			baseColor = glm::vec3(
				colorGroups_RGBA[colorGroupsIndex] / 255.f,
				colorGroups_RGBA[colorGroupsIndex + 1] / 255.f,
				colorGroups_RGBA[colorGroupsIndex + 2] / 255.f);
			baseColor = NN::getTypeColor(baseColor, l->state);
			renderShaderVertexObjectLight(
				renderObjects[Cylinder],
				MVP,
				MV,
				baseColor);
		}
	}
		
	// Probe
	MVP = world->getMVP(camScreen, probeM);
	MV = world->getMV(camScreen, probeM);
	baseColor = glm::vec3(0, 1, 1);
	renderShaderVertexObjectLight(
		renderObjects[Sphere],
		MVP,
		MV,
		baseColor);
}

void OpenGLWindow::renderTextureRayCast(CameraScreen& camScreen) {
	glm::vec3 lightPosVS = world->volume.getVSfromWS(
		camScreen.cam().getWSfromCS(
			glm::vec4(world->lightCS, 0)));

	glm::vec3 camDirVS = glm::normalize(
		world->volume.getDirVSfromWS(
			camScreen.cam().getWSfromCS(
				glm::vec4(world->camDirCS, 0))));

	// Render ray-cast texture on top of all existing objects
	glDisable(GL_DEPTH_TEST);
	renderShaderRayCast(
		lightPosVS,
		camDirVS);
	glEnable(GL_DEPTH_TEST);
}

void OpenGLWindow::renderTextureOverlay(CameraScreen& camScreen) {
	// Silhouette
	glm::mat4 silhouetteM = world->probe.M_silhouette();
	glm::mat4 silhouette_MVP = world->getMVP(camScreen, silhouetteM);
	glm::mat4 probeM = world->probe.M();
	glm::mat4 probe_MVP = world->getMVP(camScreen, probeM);
	glm::vec3 baseColor = glm::vec3(0.5, 0.5, 0.5);

	glEnable(GL_CULL_FACE);
	//Probe
	glCullFace(GL_FRONT);	// Draw up to the back of box
	renderShaderVertexObjectDepth(
		renderObjects[Sphere],
		probe_MVP,
		0.00);

	renderShaderVertexObjectSilhouette(
		renderObjects[Sphere],
		silhouette_MVP,
		baseColor,
		0.001);

	//Selected Node or Link	
	float s = world->probe.silhouetteScale() / world->probe.scale();
	glm::vec3 selectedColor = glm::vec3(1, 1, 0);
	NN::Node* sNode = world->nn.sNode;
	if (sNode != 0) {
		glm::mat4 nodeM = world->volume.M() * sNode->M;
		glm::mat4 node_MVP = world->getMVP(camScreen, nodeM);
		glm::mat4 nodeMs = nodeM * glm::scale(s, s, s);
		glm::mat4 node_silhouette_MVP = world->getMVP(camScreen, nodeMs);// nodeM* glm::scale(s, s, s));
		renderShaderVertexObjectDepth(
			renderObjects[Sphere],
			node_MVP,
			0.00);
		
		renderShaderVertexObjectSilhouette(
			renderObjects[Sphere],
			node_silhouette_MVP,
			selectedColor,
			0.001);
	}
	NN::Link* sLink = world->nn.sLink;
	if (sLink != 0) {
		glm::mat4 linkM = world->volume.M() * sLink->M;
		glm::mat4 link_MVP = world->getMVP(camScreen, linkM);
		glm::mat4 linkMs = linkM * glm::scale(s, s, s);
		glm::mat4 link_silhouette_MVP = world->getMVP(camScreen, linkMs);// linkM* glm::scale(s, s, s));
		renderShaderVertexObjectDepth(
			renderObjects[Cylinder],
			link_MVP,
			0.00);

		renderShaderVertexObjectSilhouette(
			renderObjects[Cylinder],
			link_silhouette_MVP,
			selectedColor,
			0.001);
	}


	glDisable(GL_CULL_FACE);

	//Arrow
	if (isPointOutsideView(camScreen.getNDCfromWS(world->probe.pos())) &&
		world->cams.isArrowEnabled) {
		glClear(GL_DEPTH_BUFFER_BIT);
		glm::mat4 M = getArrowM(world->probe.pos());
		glm::mat4 MVP = world->getMVP(camScreen, M);
		glm::mat4 MV = world->getMV(camScreen, M);
		baseColor = glm::vec3(1, 1, 0);
		renderShaderVertexObjectLight(
			renderObjects[Arrow],
			MVP,
			MV,
			baseColor);
	}

	//Show magnitude and direction of haptic feedback
	if (world->cams.isForceLineEnabled) {
		glClear(GL_DEPTH_BUFFER_BIT);
		initRO_forceLine();
		renderShaderLinesSolid(renderObjects[ForceLine], world->getVP(camScreen), 4);
	}
}



void OpenGLWindow::saveTextureToImage(std::vector<GLubyte>& image, const glm::ivec2& texDimensions) {
	// Saves the current drawbuffer / framebuffer
	int imageSize = texDimensions.x * texDimensions.y * 4; //pixel: R, G, B, A = 4 bytes
	image.resize(imageSize);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
}

void OpenGLWindow::saveImageToFile(std::vector<GLubyte>& image, const glm::ivec2& texDimensions, std::string filename) {			
	QImage qImage(image.data(), texDimensions.x, texDimensions.y, QImage::Format::Format_RGBA8888);
	qImage = qImage.mirrored();
	qImage.save(QString::fromStdString(filename));
}

void OpenGLWindow::loadImageToTexture(std::vector<GLubyte>& image, const glm::ivec2& texDimensions, TextureObject* tObject) {
	tObject->dim = glm::ivec3(texDimensions.x, texDimensions.y, 0);
	tObject->update(image.data());
}


