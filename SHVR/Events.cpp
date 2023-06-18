#include "Events.h"

MainWindowEvents::MainWindowEvents(	World* w, ParameterManager* pm, WidgetManager* wm,
	OpenGLWindow* gl, Haptics* h, QObject* parent):
	QObject(parent), world(w), pMan(pm), wMan(wm),
	glWindow(gl), haptics(h)
{
}

MainWindowEvents ::~MainWindowEvents()
{
}


void MainWindowEvents::toggleHaptics() {
	ProbeParams* probeParams = pMan->probeParams;
	probeParams->hapticsEnabled = !probeParams->hapticsEnabled;
	qDebug() << "toggleHaptics : " << probeParams->hapticsEnabled;
	wMan->probeWidget->updateWidgets();

	QString str = "Toggle haptics: " + QString::number(probeParams->hapticsEnabled);
	statusWriteTimedMessage(str);
}

void MainWindowEvents::rotateCamera(bool slowRate, glm::vec3 rotateAxis) {	
	float angleRadians = pMan->probeParams->rotateAngleRadians;
	if (slowRate) { angleRadians /= 10.f; }
	world->rotateView(angleRadians,	rotateAxis, world->cams[Cameras::CAM_MAIN]);
}

void MainWindowEvents::moveCamera(bool slowRate, glm::vec3 moveDir) {
	float dist = pMan->probeParams->moveDist;
	if (slowRate) { dist /= 10.f; }
	glm::vec3 translateWS = moveDir * dist;
	world->volume.translate(translateWS);
}

void MainWindowEvents::updateCameraScreenType(Cameras::ScreenType type) {
	pMan->visualParams->screenType = type;

	world->cams.updateScreenType(type);
	glWindow->updateScreen(glWindow->width(), glWindow->height());
	wMan->visualWidget->updateWidgets();
}

void MainWindowEvents::loadMaskFromLayersData(QString filename) {
	auto vMask = world->volume.vMask();
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Layers File (*.layer)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		std::vector<unsigned char> tempMask;
		FileIO::loadMaskFromLayerData(filenameUTF8, vMask->dimensions(), tempMask);
		VolumeIO::loadVolumeMask(vMask, tempMask);
		glWindow->queueUpdateTextureMask = true;

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::loadMaskData(QString filename) {
	auto vMask = world->volume.vMask();
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Mask File (*.mask)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();

		std::vector<unsigned char> tempMask;
		FileIO::loadRAW(filenameUTF8, pMan->configFile.dimensions, tempMask);
		VolumeIO::loadVolumeMask(vMask, tempMask);

		//Reapply offset
		glWindow->queueUpdateTextureMask = true;

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::saveMaskData(QString filename) {
	auto vMask = world->volume.vMask();
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Mask File (*.mask)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		std::vector<unsigned char> tempMask;
		VolumeIO::saveVolumeMask(vMask, tempMask);
		FileIO::saveRAW(filenameUTF8, tempMask);

		statusWriteMessage(QString("Saved " + filename));
	}
}

void MainWindowEvents::loadOffsetData(QString filename) {
	auto vData = world->volume.vData();
	auto vOffset = world->volume.vState()->vOffset;
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Offset File (*.off)");	
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		world->volume.vState()->clearVolumeOffset();
		if (pMan->configFile.inputType == Assign2::DataInput::UINT8) {
			std::vector<short> tempOffsets;
			FileIO::loadRAW(filenameUTF8, pMan->configFile.dimensions, tempOffsets);			
			VolumeIO::loadVolume(vOffset, tempOffsets);
		}
		else if (pMan->configFile.inputType == Assign2::DataInput::UINT16) {
			std::vector<int> tempOffsets;
			FileIO::loadRAW(filenameUTF8, pMan->configFile.dimensions, tempOffsets);
			VolumeIO::loadVolume(vOffset, tempOffsets);			
		}

		//Reapply offset
		world->volume.vState()->applyVolumeOffset();
		world->commandStack.clearCommands();
		glWindow->queueUpdateTextureVolume = true;

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::saveOffsetData(QString filename) {
	auto vOffset = world->volume.vState()->vOffset;
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Offset File (*.off)");
		statusWriteMessage(QString("Saving" + filename + "..."));
	}
	if (filename != "") {

		std::string filenameUTF8 = filename.toUtf8();
		if (pMan->configFile.inputType == Assign2::DataInput::UINT8) {
			std::vector<short> tempOffsets;
			VolumeIO::saveVolume(vOffset, tempOffsets);
			FileIO::saveRAW(filenameUTF8, tempOffsets);
		}

		else if (pMan->configFile.inputType == Assign2::DataInput::UINT16) {
			std::vector<int> tempOffsets;
			VolumeIO::saveVolume(vOffset, tempOffsets);
			FileIO::saveRAW(filenameUTF8, tempOffsets);
		}

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::saveVolumeData(QString filename) {
	auto vData = world->volume.vData();
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Volume File (*.raw)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {

		std::string filenameUTF8 = filename.toUtf8();
		if (pMan->configFile.inputType == Assign2::DataInput::UINT8) {
			std::vector<unsigned char> tempVolume;
			VolumeIO::saveVolume(vData, tempVolume);
			FileIO::saveRAW(filenameUTF8, tempVolume);
		}

		else if (pMan->configFile.inputType == Assign2::DataInput::UINT16) {
			std::vector<unsigned short> tempVolume;
			VolumeIO::saveVolume(vData, tempVolume);
			FileIO::saveRAW(filenameUTF8, tempVolume);
		}

		statusWriteMessage(QString("Saved " + filename));
	}
}



void MainWindowEvents::loadGeneralSettings(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Screen File (*.screen)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		FileIO::loadGeneral(filenameUTF8, pMan->generalFile);
		pMan->visualParams->importData(pMan->generalFile);
		pMan->renderParams->importData(pMan->generalFile);
		pMan->probeParams->importData(pMan->generalFile);
		updateGeneralSettings();

		statusWriteMessage(QString("Loaded " + filename));
	}
}
void MainWindowEvents::saveGeneralSettings(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Screen File (*.screen)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {
		pMan->visualParams->volumeM = world->volume.M();
		pMan->visualParams->exportData(pMan->generalFile);
		pMan->renderParams->exportData(pMan->generalFile);
		pMan->probeParams->exportData(pMan->generalFile);

		std::string filenameUTF8 = filename.toUtf8();
		FileIO::saveGeneral(filenameUTF8, pMan->generalFile);

		statusWriteMessage(QString("Saved " + filename));
	}
}


void MainWindowEvents::loadTFData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Transfer Function File (*.tf)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		FileIO::loadTransferFunction(filenameUTF8, pMan->tfFile);
		pMan->tf->importData(pMan->tfFile);

		loadColorMapTFToOpenGL();

		statusWriteMessage(QString("Loaded " + filename));
	}
}


void MainWindowEvents::saveTFData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Transfer Function File (*.tf)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {
		pMan->tf->exportData(pMan->tfFile);
		pMan->isoParams->exportData(pMan->tfFile);

		std::string filenameUTF8 = filename.toUtf8();
		FileIO::saveTransferFunction(filenameUTF8, pMan->tfFile);

		statusWriteMessage(QString("Saved " + filename));
	}
}

void MainWindowEvents::loadColorGroupData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Color Group File (*.group)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		FileIO::loadGroups(filenameUTF8, pMan->groupFile);
		pMan->cGroups->importData(pMan->groupFile);
		wMan->groupWidget->updateWidgets();

		loadMaskIntensityToOpenGL();
		loadColorMapMaskToOpenGL();

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::saveColorGroupData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Color Group File (*.group)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();
		pMan->cGroups->exportData(pMan->groupFile);
		FileIO::saveGroups(filenameUTF8, pMan->groupFile);

		statusWriteMessage(QString("Saved " + filename));
	}
}

void MainWindowEvents::loadLightData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Light File (*.light)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();

		FileIO::loadLight(filenameUTF8, pMan->lightFile);
		pMan->lightParams->importData(pMan->lightFile);
		wMan->lightWidget->updateWidgets();

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::saveLightData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Light File (*.light)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();

		pMan->lightParams->exportData(pMan->lightFile);
		FileIO::saveLight(filenameUTF8, pMan->lightFile);

		statusWriteMessage(QString("Saved " + filename));
	}
}

void MainWindowEvents::loadNNData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Node Network File (*.nn)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();				
				
		FileIO::loadNodeNetwork(filenameUTF8, pMan->nodeFile);
		NN::loadNObjects(&pMan->nodeFile, &world->nn);

		//Update links with world scale
		for (auto l : world->nn.links) {			
			l->scale = pMan->cGroups->at(l->groupNum).linkScale;
			l->updateM(world->worldScale);
		}

		statusWriteMessage(QString("Loaded " + filename));
	}
}

void MainWindowEvents::saveNNData(QString filename) {
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Saving("Node Network File (*.nn)");
		statusWriteMessage(QString("Saving " + filename + "..."));
	}
	if (filename != "") {		
		std::string filenameUTF8 = filename.toUtf8();

		NN::saveNObjects(&pMan->nodeFile, &world->nn);
		FileIO::saveNodeNetwork(filenameUTF8, pMan->nodeFile);		

		statusWriteMessage(QString("Saved " + filename));
	}
}

void MainWindowEvents::updateGeneralSettings() {
	world->volume.updateM(pMan->visualParams->volumeM);
	updateVisualData();
	updateProbeScale();
	updateRenderQuality();
}

void MainWindowEvents::updateDimensionsToSSWidget() {
	wMan->ssWidget->updateSSDimensions();
}


void MainWindowEvents::loadColorMapTFToOpenGL() {
	// Update minimum TF values
	pMan->isoParams->tfIsoMin = pMan->tf->getMinimumVisibleValue();
	pMan->isoParams->tfInValue = pMan->tf->getVolumeInValue(pMan->isoParams->tfIsoMin);

	// Update TF colour map in OpenGL
	int maxTexSize = glWindow->getMaxTexSize();
	std::vector<unsigned char> tf_RGBA;
	pMan->tf->generateLinearTF(maxTexSize, tf_RGBA);
	glWindow->colorTF_RGBA.swap(tf_RGBA);
	glWindow->queueUpdateTextureVolumeColor = true;
}

void MainWindowEvents::loadColorMapMaskToOpenGL() {
	ColorGroups* cGroups = pMan->cGroups;
	std::vector<unsigned char> group_RGBA(4 * pMan->cGroups->size());
	for (int i = 0; i < cGroups->size(); i++) {
		ColorGroup& group = cGroups->at(i);
		group_RGBA[4 * i] = group.r;
		group_RGBA[4 * i + 1] = group.g;
		group_RGBA[4 * i + 2] = group.b;
		group_RGBA[4 * i + 3] = group.a;
	}
	glWindow->backgroundColour = glm::vec4(
		(float)group_RGBA[0] / 255.f,
		(float)group_RGBA[1] / 255.f,
		(float)group_RGBA[2] / 255.f,
		0.f);
	glWindow->colorGroups_RGBA.swap(group_RGBA);
	glWindow->queueUpdateTextureMaskColor = true;
}

void MainWindowEvents::loadAOIntensityToOpenGL() {
	int maxTexSize = glWindow->getMaxTexSize();
	std::vector<unsigned char> ao_A;
	pMan->ao->generateLinearTF(maxTexSize, ao_A);
	glWindow->aoIntensity_A.swap(ao_A);
	glWindow->queueUpdateTextureAOIntensity = true;
	glWindow->queueUpdateTextureVolumeAO = true;
}

void MainWindowEvents::updateAOParamsOpenGL() {
	glWindow->queueUpdateTextureVolumeAO = true;
}

void MainWindowEvents::loadMaskIntensityToOpenGL() {
	glWindow->maskIntensity = pMan->cGroups->maskIntensity;
}

void MainWindowEvents::updateMaskEnabledOpenGL() {
	glWindow->updateMaskEnabled(*pMan->cGroups);
}

void MainWindowEvents::loadTextureMapsToOpenGL() {
	//Note: We want to know the max size of textures before loading in colour map
	loadColorMapTFToOpenGL();
	loadColorMapMaskToOpenGL();
	loadAOIntensityToOpenGL();
	qDebug() << "Update texture maps";
}




bool MainWindowEvents::getSSFileName(const std::string& fileDir, std::string& out_filename) {
	unsigned int i = 1;
	const int MAX_COUNT = 9999;

	std::string filename;
	int numberOfZeros = log10f(MAX_COUNT);
	while (i < MAX_COUNT) {
		float remainingZeros = numberOfZeros - log10f((float)i);
		std::string trailingZeros;
		for (int n = 0; n < remainingZeros; n++) {
			trailingZeros += "0";
		}
		filename = "image" + trailingZeros + std::to_string(i) + ".png";
		if (!FileIO::isFilePresent(fileDir + filename)) {
			out_filename = filename;
			return true;
		}
		i++;
	}
	return false;
}

void MainWindowEvents::takeSSOpenGL(int mode) {
	ScreenshotParams* ssParams = pMan->ssParams;
	std::string filename;
	if (getSSFileName(ssParams->fileDir, filename)) {
		ssParams->fileName = filename;
		glWindow->queueScreenshotMainCamera = mode;		
		//====
		//QString str = "Taking screenshot...";
		////***BUG: This doesn't show up on status screen
		//statusWriteMessage(str);
		//qDebug() << str;
	}
}

void MainWindowEvents::setFromNearClippingPlane() {	
	world->cams[Cameras::CAM_MAIN].updateClipFar(world->cams[Cameras::CAM_MAIN].clippingPlaneNear());
	world->cams[Cameras::CAM_MAIN].updateClipNear(world->cams[Cameras::CAM_MAIN].nearPlane());
	pMan->visualParams->clipPlaneNear = world->cams[Cameras::CAM_MAIN].clippingPlaneNear();
	pMan->visualParams->clipPlaneFar = world->cams[Cameras::CAM_MAIN].clippingPlaneFar();
}

void MainWindowEvents::setFromFarClippingPlane() {
	world->cams[Cameras::CAM_MAIN].updateClipNear(world->cams[Cameras::CAM_MAIN].clippingPlaneFar());
	world->cams[Cameras::CAM_MAIN].updateClipFar(world->cams[Cameras::CAM_MAIN].farPlane());
	pMan->visualParams->clipPlaneNear = world->cams[Cameras::CAM_MAIN].clippingPlaneNear();
	pMan->visualParams->clipPlaneFar = world->cams[Cameras::CAM_MAIN].clippingPlaneFar();
}

void MainWindowEvents::resetClippingPlanes() {
	float boxWidth = world->volume.vData()->boxWidth();
	float boxHeight = world->volume.vData()->boxHeight();
	float boxDepth = world->volume.vData()->boxDepth();
	glm::vec3 bounds = glm::vec3(boxWidth,boxHeight,boxDepth);

	VOBox vo_box;
	vo_box.generate();
	vo_box.scaleVTX(glm::vec3(boxWidth, boxHeight, boxDepth));
	std::vector<float> v_dist;

	//Note: Camera is staying still, but the volume is moving.
	for (auto v : vo_box.vtx) {
		v = world->volume.getWSfromVS(v + 0.5f * bounds);		//Align the dimensions with the given volume box
		float d = world->cams[Cameras::CAM_MAIN].cam().intersectDistance(v);	//Get distance to each point of box
																				//Note: More efficient than matrix mul.
		v_dist.push_back(d / 10);								// (?) Distance seems always off by factor of 10
	}

	world->cams.resetClippingPlanes(v_dist);

	// Save changes in parameter (synchronizes settings with keyboard)
	pMan->visualParams->clipPlaneNear = world->cams[Cameras::CAM_MAIN].clippingPlaneNear();
	pMan->visualParams->clipPlaneFar = world->cams[Cameras::CAM_MAIN].clippingPlaneFar();
}

void MainWindowEvents::updateVisualData() {
	VisualParams* visualParams = pMan->visualParams;
	int screenType_prev = world->cams.getScreenType();
	world->cams.importData(visualParams);

	if (visualParams->queueCameraSave) {
		visualParams->volumeM = world->volume.M();
		visualParams->queueCameraSave = false;
		statusWriteMessage(QString("Quick Save Camera" ));

	}
	if (visualParams->queueCameraLoad) {
		world->volume.updateM(visualParams->volumeM);
		visualParams->queueCameraLoad = false;
		statusWriteMessage(QString("Quick Load Camera"));
	}

	if (screenType_prev != visualParams->screenType) {
		//TODO: Synchronize update of Overview mode with graphics update
		if (visualParams->screenType == Cameras::SCR_OVERVIEW) {
			world->cams.updateOverviewCameras(world->volume);
		}
		glWindow->updateScreen(glWindow->width(), glWindow->height());
	}
}
void MainWindowEvents::updateCutPlanes() {
	glWindow->queueUpdateROBoxMinMax = true;
	glWindow->queueUpdateShaderCutPlane = true;
	glWindow->queueUpdateOverviewImages = true;
}

void MainWindowEvents::updateProbeScale() {
	world->probe.updateScaleM(pMan->probeParams->probeSize);
	world->probe.updateSilhouetteScaleM(pMan->probeParams->silhouetteSize);
}
void MainWindowEvents::updateRenderQuality() {
	glWindow->updateFPS(pMan->renderParams->fps);
	glWindow->updateCompressionRatio(pMan->renderParams->compressionRatio);
}

void MainWindowEvents::initSSDirectory() {
	QDir dir(QDir::currentPath() + "/screenshot");
	if (!dir.exists()) {
		QDir().mkdir("screenshot");
	}
	QString dirStr = dir.absolutePath() + "/";
	pMan->ssParams->fileDir = dirStr.toStdString();
}

template<typename T>
void marchingCubesSurfaceTemplate(std::string filename, VolumeBase* vData, const T* data, float inValue) {
	CIsoSurface<T> iso;

	iso.GenerateSurface(data,
		inValue,
		vData->width() - 1,
		vData->height() - 1,
		vData->depth() - 1,
		vData->widthSpacing(),
		vData->heightSpacing(),
		vData->depthSpacing());

	// How do I only capture only one mask instead of all of them?

	std::ofstream ofs(filename, std::ofstream::out);

	for (int i = 0; i < iso.m_nVertices; ++i)
	{
		auto& p = iso.m_ppt3dVertices[i];

		ofs << "v " << p[0] << " " << p[1] << " " << p[2] << "\n";
	}

	ofs << std::endl;

	for (int i = 0; i < iso.m_nTriangles; ++i)
	{
		unsigned int* f = iso.m_piTriangleIndices + 3 * i;

		ofs << "f "
			<< (f[0] + 1) << " "
			<< (f[1] + 1) << " "
			<< (f[2] + 1) << "\n";
	}

	ofs << std::endl;

	ofs.close();
}

template <typename T>
void mergeVolumeAndMask(
	T* data,
	unsigned char* mask,
	ColorGroups* cGroups,
	size_t size,
	float valueNew,
	std::vector<T>& out_vector) {

	out_vector.resize(size);
	for (int i = 0; i < size; i++) {
		T d = *data;
		unsigned char m = *mask;

		if (cGroups->at(m).a == 0) {
			//Zero out excluded voxels
			d = valueNew;
		}

		out_vector[i] = d;

		//Iterate through pointers
		data++;
		mask++;
	}

}


void MainWindowEvents::marchingCubesSurface() {
	VolumeBase* vData = world->volume.vData();
	VolumeMask* vMask = world->volume.vMask();
	ColorGroups* cGroups = pMan->cGroups;

	qDebug() << "Starting Marching cube";
	QString filename = FileIO::getFileNameWithDialog_Saving("OBJ File (*.obj)");
	if (filename != "") {
		if (pMan->configFile.inputType == Assign2::DataInput::UINT8) {
			// Only keep visible part of volume
			std::vector<unsigned char> vFiltered;
			mergeVolumeAndMask(
				(unsigned char*)vData->data(),
				(unsigned char*)vMask->data(),
				cGroups,
				vData->width() * vData->height() * vData->depth(),
				std::max(pMan->isoParams->tfInValue - 1, 0.f),
				vFiltered);

			marchingCubesSurfaceTemplate(
				filename.toStdString(),
				vData,
				vFiltered.data(),
				pMan->isoParams->tfInValue);
		}
		if (pMan->configFile.inputType == Assign2::DataInput::UINT16) {
			// Only keep visible part of volume
			std::vector<unsigned short> vFiltered;
			mergeVolumeAndMask(
				(unsigned short*)vData->data(),
				(unsigned char*)vMask->data(),
				cGroups,
				vData->width() * vData->height() * vData->depth(),
				std::max(pMan->isoParams->tfInValue - 1, 0.f),
				vFiltered);
			marchingCubesSurfaceTemplate(
				filename.toStdString(),
				vData,
				vFiltered.data(),
				pMan->isoParams->tfInValue);
		}
	}
	qDebug() << "Finished Marching cube";
}


bool MainWindowEvents::openConfirmationMessage(QString text) {
	QMessageBox msgBox;

	msgBox.setInformativeText(text);
	msgBox.setMinimumSize(200, 200);
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		return true;
	}
	else {
		return false;
	}
}



void MainWindowEvents::clearMask() {
	if (openConfirmationMessage(
		"Are you sure you want to clear offset? Your undo stack will be reset, and this action cannot be reversed.")) {
		world->commandStack.clearCommands();
		world->volume.vMask()->setValue(0);
		glWindow->queueUpdateTextureMask = true;
	}
}

void MainWindowEvents::clearOffset() {
	if (openConfirmationMessage(
		"Are you sure you want to clear offset? Your undo stack will be reset, and this action cannot be reversed.")) {
		pMan->initVolumeData(world);
		world->volume.vState()->clearVolumeOffset();
		world->commandStack.clearCommands();
		glWindow->queueUpdateTextureVolume = true;
	}
}

void MainWindowEvents::clearCommandStack() {
	if (openConfirmationMessage(
		"Are you sure you want to clear undo stack? This action cannot be reversed.")) {
		world->commandStack.clearCommands();
	}
}

void MainWindowEvents::updateMaxCommandStackSize() {
	world->commandStack.updateMaxSize(pMan->renderParams->maxNumberOfCommands);
}


void MainWindowEvents::closeProgram() {
	if (openConfirmationMessage("Are you sure you want to close?")) {
		qApp->quit();
	}
}


void MainWindowEvents::initUIParams() {
	initSSDirectory();
	updateGeneralSettings();

	//Haptics
	haptics->updateVolumes(world->volume.vData(), world->volume.vMask());

	if (pMan->isoParams->hapticsIsoMin == 0) {
		pMan->isoParams->hapticsIsoMin = pMan->tf->getMinimumVisibleValue();
	}
	if (pMan->isoParams->paintIsoMin == 0) {
		pMan->isoParams->paintIsoMin = pMan->tf->getMinimumVisibleValue();
	}

	pMan->isoParams->hapticsInValue =
		pMan->tf->getVolumeInValue(pMan->isoParams->hapticsIsoMin);
	pMan->isoParams->paintInValue =
		pMan->tf->getVolumeInValue(pMan->isoParams->paintIsoMin);
	pMan->isoParams->isoSurfaceThresh =
		pMan->isoParams->paintInValue / pMan->tf->dataTypeRange;


	//Visualization	
	updateAOParamsOpenGL();
	updateMaskEnabledOpenGL();
	loadMaskIntensityToOpenGL();

	glWindow->initVolumeTextureParameters(
		pMan->configFile.volumeCompressionEnabled,
		pMan->configFile.inputType);

	//UI
	wMan->tfWidget->updateWidgets();
	wMan->groupWidget->updateWidgets();
	wMan->lightWidget->updateWidgets();

	wMan->probeWidget->updateWidgets();
	wMan->visualWidget->updateWidgets();
	wMan->renderWidget->updateWidgets();
	wMan->ssWidget->updateWidgets();
}

void MainWindowEvents::loadWorld() {
	// Update config
	statusWriteMessage("Loading configurations file...");
	FileIO::updateConfigBasedOnInputFile(pMan->configFile);
	pMan->updateSettingsBasedOnConfigFile();

	// Update world	
	world->initBasedOnConfig(pMan->configFile);

	// Update Managers & UI
	statusWriteMessage("Loading image files... (This may take some time)");
	pMan->initVolumeData(world);

	statusWriteMessage("Initializing mask data...");
	pMan->initMaskData(world);

	statusWriteMessage("Initializing offset data...");
	pMan->initOffsetData(world);
	
	//TODO: Move this to some class? Make code more redundant?
	if (FileIO::isFilePresent(pMan->configFile.nodeFile)) {
		FileIO::loadNodeNetwork(pMan->configFile.nodeFile, pMan->nodeFile);
		NN::loadNObjects(&pMan->nodeFile, &world->nn);

		//Update links with world scale
		for (auto l : world->nn.links) {
			l->scale = pMan->cGroups->at(l->groupNum).linkScale;
			l->updateM(world->worldScale);
		}
	}

	// Initialize the settings from files
	statusWriteMessage("Updating setting files...");
	pMan->init();
	statusWriteMessage("Updating widgets...");
	initUIParams();		//***Update UI windows (decouple signals& load widgets)	

	//***ARE THESE NECESSARY?
	if (pMan->isoParams->hapticsIsoMin == 0) {
		wMan->tfWidget->autoUpdateHapticIsoMin();
	}
	if (pMan->isoParams->paintIsoMin == 0) {
		wMan->tfWidget->autoUpdatePaintIsoMin();
	}
	else {
		wMan->tfWidget->updatePaintIsoMin(pMan->isoParams->paintIsoMin);
	}
	world->commandStack.updateMaxSize(pMan->renderParams->maxNumberOfCommands);

	// Run probe and haptics
	haptics->run();
}

std::vector<unsigned char> MainWindowEvents::loadMaskData_Compare(QString filename) {
	std::vector<unsigned char> tempMask;
	if (filename == "") {
		filename = FileIO::getFileNameWithDialog_Loading("Mask File (*.mask)");
		statusWriteMessage(QString("Loading " + filename + "..."));
	}
	if (filename != "") {
		std::string filenameUTF8 = filename.toUtf8();		
		FileIO::loadRAW(filenameUTF8, pMan->configFile.dimensions, tempMask);
		statusWriteMessage(QString("Loaded " + filename));
	}
	return tempMask;
}

double diceCoeff(unsigned char* a, unsigned char* b, long size) {
	double count_intersect = 0;
	double count_a = 0;
	double count_b = 0;

	// Count intersection, as well as number of voxels painted for volume a and volume b
	for (int i = 0; i < size; i++) {
		if (a[i] > 0 && b[i] > 0) {
			count_intersect++;
			count_b++;
			count_a++;
		}
		else if (a[i] > 0) {
			count_a++;
		}
		else if (b[i] > 0) {
			count_b++;
		}
	}

	// return DICE coefficient
	return 2. * count_intersect / (count_a + count_b);
}

void MainWindowEvents::compareSegmentation() {
	//load ground truth segmentation
	std::vector<unsigned char> groundTruth = loadMaskData_Compare();
	if (groundTruth.size() == 0) {
		return;
	}


	//compare how much of the painting mask is inside of ground truth
	double dice = diceCoeff(groundTruth.data(), (unsigned char*)world->volume.vMask()->data(), groundTruth.size());

	//output results
	statusWriteMessage(QString("Segmentation Comparison: " + QString::number(dice)));
	qDebug() << "COEFF: " << dice;	
}

std::pair<float,float> raySphereIntersect(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 sphereOrigin, float sphereRadius) {
	glm::vec3 posDiff = rayOrigin - sphereOrigin;
	float a = 1;
	float b = 2 * glm::dot(rayDir, posDiff);
	float c = glm::dot(posDiff, posDiff) - sphereRadius * sphereRadius;

	float discriminant = b * b - 4 * a * c;
	float t0 = 0; float t1 = 0;
	if (discriminant > 0) {	//two roots
		t0 = (-b + sqrt(discriminant)) / 2*a;
		t1 = (-b - sqrt(discriminant)) / 2 * a;
	}
	else if (discriminant == 0) { //one root
		t0 = -b / 2 * a;
		t1 = t0;
	}
	//ELSE: no intersect

	return std::make_pair(t0, t1);
}

void getBoxMinMaxIndex(
	const glm::ivec3& idx,
	const glm::ivec3& cellRadius,
	glm::ivec3& idxMin_out,
	glm::ivec3& idxMax_out,
	VolumeBase* vData) {
	////Note: one is added to value to handle data loss from converting from float to int,
	////Note2: idxMax adds two because idxMin uses (>=) while idxMax uses (<)
	idxMin_out.x = std::max(0, idx.x - cellRadius.x);
	idxMax_out.x = std::min(idx.x + cellRadius.x + 1, (int)vData->width());

	idxMin_out.y = std::max(0, idx.y - cellRadius.y);
	idxMax_out.y = std::min(idx.y + cellRadius.y + 1, (int)vData->height());

	idxMin_out.z = std::max(0, idx.z - cellRadius.z);
	idxMax_out.z = std::min(idx.z + cellRadius.z + 1, (int)vData->depth());
}

long index(int x, int y, int z, VolumeBase* vData) {
	return x + y * vData->width() + z * vData->width() * vData->height();
}

float scoreNodePlacement(const glm::vec3& sphereOrigin, 
		const float sphereRadius, 
		const glm::ivec3& cellRadius, 
		const std::vector<unsigned char>& groundTruth, 
		VolumeBase* vData) {

	glm::ivec3 idxNode;
	glm::ivec3 idxNodeLeft;
	glm::ivec3 idxNodeRight;
	glm::ivec3 regionMin;
	glm::ivec3 regionMax;
	glm::vec3 rayOrigin;
	glm::vec3 rayDir(1, 0, 0);
	int count_voxel_in = 0;
	int count_voxel_total = 0;

	// Rough boundary of node	
	vData->getCellExplicit(sphereOrigin, idxNode);
	getBoxMinMaxIndex(idxNode, cellRadius, regionMin, regionMax, vData);

	for (int k = regionMin.z; k < regionMax.z; k++) {
		for (int j = regionMin.y; j < regionMax.y; j++) {
			// start and end index along the x-axis
			vData->getPositionExplicit(glm::ivec3(0, j, k), rayOrigin);
			std::pair<float, float> intersects = 
				raySphereIntersect(rayOrigin, rayDir, sphereOrigin, sphereRadius);

			//find region inside of node
			if (intersects.first > intersects.second) {
				vData->getCellExplicit(rayOrigin + intersects.first * rayDir, idxNodeRight);
				vData->getCellExplicit(rayOrigin + intersects.second * rayDir, idxNodeLeft);
				for (int i = idxNodeLeft.x; i < idxNodeRight.x && i < regionMax.x; i++) {
					glm::ivec3 idx(i, j, k);
					// cal
					if (groundTruth[index(i, j, k, vData)] > 0) {
						count_voxel_in++;
					}
					count_voxel_total++;
				}
			}
		}
	}
	if (count_voxel_total == 0) {
		return 0;	//Was not within volume
	}
	return (float)count_voxel_in / (float)count_voxel_total;
}

void MainWindowEvents::compareTracing() {
	//load ground truth segmentation
	std::vector<unsigned char> groundTruth = loadMaskData_Compare();
	if (groundTruth.size() == 0) {
		return;
	}

	//Nodes
	int count_node_DICE = 0;
	double score_node_DICE = 0;
	int count_node_outside = 0;
	float in_threshold = 0.5;
	for (NN::Node* n : world->nn.nodes) {
		//Skip deleted nodes
		if (n->flag_deleted == true) {
			continue;
		}

		glm::vec3 sphereOrigin = glm::vec3(world->volume.getTranslateM() * glm::vec4(n->pos,1));
		float sphereRadius = n->scale * world->worldScale;
		glm::ivec3 cellRadius = glm::ivec3(world->volume.getCellSize(sphereRadius));
	
		double score_Node = scoreNodePlacement(sphereOrigin, sphereRadius, cellRadius, groundTruth, world->volume.vData());
		if (score_Node < in_threshold) {
			count_node_outside++;
		}
		score_node_DICE += score_Node;
		count_node_DICE++;
	}

	double score_node_DICE_avg = score_node_DICE / (double)count_node_DICE;

	//Links
	int count_link_DICE = 0;
	double score_link_DICE = 0;
	int count_link_outside = 0;
	for (NN::Link* l : world->nn.links) {
		//Skip deleted nodes
		if (l->flag_deleted == true) {
			continue;
		}

		glm::vec3 sphereOrigin = glm::vec3(world->volume.getTranslateM() * glm::vec4(l->position(), 1));
		float sphereRadius = l->scale * world->worldScale;
		glm::ivec3 cellRadius = glm::ivec3(world->volume.getCellSize(sphereRadius));

		double score_link = scoreNodePlacement(sphereOrigin, sphereRadius, cellRadius, groundTruth, world->volume.vData());
		if (score_link < in_threshold) {
			count_link_outside++;
		}
		score_link_DICE += score_link;
		count_link_DICE++;
	}

	double score_link_DICE_avg = score_link_DICE / (double)count_link_DICE;


	//output results
	QString str = QString("[TRACING] Node Score: " + QString::number(score_node_DICE_avg) + "; Out: " +
		QString::number(count_node_outside) + "/" + QString::number(count_node_DICE)) +
		"; Internode Score: " + QString::number(score_link_DICE_avg) + "; Out: " +
		QString::number(count_link_outside) + "/" + QString::number(count_link_DICE);
	statusWriteMessage(str);
	qDebug() << str;

	//compare how many internodes (a node placed at midpoint of each internode with its radius) is within ROI

	//output value
}

//====

//
//void MainWindow::loadLayersData() {
//	// !!DEPRECATED!!
//	//auto& layers = world->volume.vState()->layers;
//	//QString filename = FileIO::getFileNameWithDialog_Loading("Layers File (*.layer)");
//	//if (filename != "") {
//	//	std::string filenameUTF8 = filename.toUtf8();
//	//	FileIO::loadLayerData(filenameUTF8, layers);
//	//	world->volume.vMask()->updateMaskWithLayers(layers);
//	//	glWindow->queueUpdateTextureMask = true;	
//	//}
//
//	loadMaskFromLayersData();
//}

//void MainWindow::saveLayersData() {
//	// !!DEPRECATED!!
//	//auto& layers = world->volume.vState()->layers;
//	//QString filename = FileIO::getFileNameWithDialog_Saving("Layers File (*.layer)");
//	//if (filename != "") {
//	//	std::string filenameUTF8 = filename.toUtf8();
//	//	FileIO::saveLayerData(filenameUTF8, layers);
//	//}
//}