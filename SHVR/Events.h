#pragma once

#include <QObject>
#include "ParameterManager.h"
#include "OpenGLWindow.h"
#include "Haptics.h"
#include "WidgetManager.h"


#include <qmessagebox.h>
#include "CIsoSurface.h"

class MainWindowEvents : public QObject
{
	Q_OBJECT

public:
	MainWindowEvents(World* w, ParameterManager* pm, WidgetManager* wm,
		OpenGLWindow* gl, Haptics* h, QObject* parent = Q_NULLPTR);
	~MainWindowEvents();

	//TODO: Transfer rest of keyboard commands in here
	void toggleHaptics();
	void rotateCamera(bool slowRate, glm::vec3 rotateAxis);
	void moveCamera(bool slowRate, glm::vec3 moveDir);

	void updateCameraScreenType(Cameras::ScreenType type);


	void initSSDirectory();
	bool getSSFileName(const std::string& fileDir, std::string& out_filename);
	bool openConfirmationMessage(QString text);
	void initUIParams();
	void closeProgram();

	std::vector<unsigned char>  loadMaskData_Compare(QString filename = "");

private:
	World* world;
	ParameterManager* pMan;
	OpenGLWindow* glWindow;
	Haptics* haptics;
	WidgetManager* wMan;

public slots:
	void takeSSOpenGL(int mode = 1);
	void setFromNearClippingPlane();
	void setFromFarClippingPlane();
	void resetClippingPlanes();
	void updateVisualData();
	void updateCutPlanes();
	void updateRenderQuality();
	void updateProbeScale();

	void loadMaskFromLayersData(QString filename = "");
	void loadMaskData(QString filename = "");
	void saveMaskData(QString filename = "");
	void loadOffsetData(QString filename = "");
	void saveOffsetData(QString filename = "");
	void saveVolumeData(QString filename = "");

	void loadGeneralSettings(QString filename = "");
	void saveGeneralSettings(QString filename = "");

	void saveTFData(QString filename = "");
	void loadTFData(QString filename = "");
	void loadColorGroupData(QString filename = "");
	void saveColorGroupData(QString filename = "");

	void loadLightData(QString filename = "");
	void saveLightData(QString filename = "");
	void loadNNData(QString filename = "");
	void saveNNData(QString filename = "");

	void updateDimensionsToSSWidget();

	void loadColorMapTFToOpenGL();
	void loadColorMapMaskToOpenGL();
	void loadAOIntensityToOpenGL();
	void updateAOParamsOpenGL();
	void loadMaskIntensityToOpenGL();
	void updateMaskEnabledOpenGL();
	void loadTextureMapsToOpenGL();
	void updateGeneralSettings();
	void marchingCubesSurface();
	
	void clearMask();
	void clearOffset();
	void clearCommandStack();
	void updateMaxCommandStackSize();

	void loadWorld();

	
	void compareTracing();
	void compareSegmentation();
};

class LoadWorldThread : public QThread {
	Q_OBJECT
public:
	MainWindowEvents* mEvent;
	LoadWorldThread(MainWindowEvents* _mEvent): mEvent(_mEvent) {}

	void run() override {
		mEvent->loadWorld();
		emit resultReady();
	}

signals:
	void resultReady();
};
