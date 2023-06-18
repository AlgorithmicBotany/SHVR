#include "MainWindow.h"


//============================

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setAcceptDrops(true);		
	statusInit(statusBar());
		
	world = new World();			// Objects and light in 3D space	
	pMan = new ParameterManager();	// Parameters that affect objects, interaction and visualization 
	wMan = new WidgetManager();		// Widgets to control parameters
	wMan->init(pMan);	
			
	initUI();		
	initMenus();
	initConnections();
	initEventFilter();
	

	std::string defaultConfigFile = "./settings/default.ini";
	if (FileIO::isFilePresent(defaultConfigFile)) {
		pMan->configFile =
			pMan->loadConfigFile(defaultConfigFile);
		loadWorld();
	}


	//LOGGER_START(Logger::DEBUG, "log.txt");
	//LOGGER_WRITE(Logger::INFO, "INFO ");
	//LOGGER_WRITE(Logger::CONFIG, "input file is ");
	//LOGGER_WRITE(Logger::DEBUG, "DEBUG");


	//statusWriteLabel("MODES");
	//statusProgressMinMax(0, 0);
}

MainWindow::~MainWindow() {	
	delete glWindow;
	delete haptics;
	delete pMan;

	delete menu;
	delete windowContainer;
	delete widgetLayout;
	delete world;
	delete mEvent;
	delete eventFilter;
}


void MainWindow::initConnections() {
	connect(glWindow, SIGNAL(initialized()), mEvent, SLOT(loadTextureMapsToOpenGL()));
	connect(glWindow, SIGNAL(resized()), mEvent, SLOT(updateDimensionsToSSWidget()));
	connect(wMan->tfWidget, SIGNAL(graphUpdated()), mEvent, SLOT(loadColorMapTFToOpenGL()));
	connect(wMan->lightWidget, SIGNAL(graphUpdated()), mEvent, SLOT(loadAOIntensityToOpenGL()));
	connect(wMan->lightWidget, SIGNAL(aoParamUpdated()), mEvent, SLOT(updateAOParamsOpenGL()));
	connect(wMan->groupWidget, SIGNAL(maskColorsUpdated()), mEvent, SLOT(loadColorMapMaskToOpenGL()));
	connect(wMan->groupWidget, SIGNAL(maskIntensityUpdated()), mEvent, SLOT(loadMaskIntensityToOpenGL()));
	connect(wMan->groupWidget, SIGNAL(maskUpdated()), mEvent, SLOT(updateMaskEnabledOpenGL()));
	connect(wMan->fileOpenWidget, SIGNAL(configLoaded()), this, SLOT(loadWorld()));
	connect(wMan->ssWidget, SIGNAL(screenshot()), mEvent, SLOT(takeSSOpenGL()));

	connect(wMan->visualWidget, SIGNAL(paramUpdated()), mEvent, SLOT(updateVisualData()));
	connect(wMan->visualWidget, SIGNAL(cutUpdated()), mEvent, SLOT(updateCutPlanes()));
	connect(wMan->probeWidget, SIGNAL(paramUpdated()), mEvent, SLOT(updateProbeScale()));
	connect(wMan->renderWidget, SIGNAL(paramUpdated()), mEvent, SLOT(updateRenderQuality()));
	connect(wMan->renderWidget, SIGNAL(commandStackUpdated()), mEvent, SLOT(updateMaxCommandStackSize()));
	connect(wMan->renderWidget, SIGNAL(marchingCubeRequested()), mEvent, SLOT(marchingCubesSurface()));
	connect(wMan->renderWidget, SIGNAL(clearMaskRequested()), mEvent, SLOT(clearMask()));
	connect(wMan->renderWidget, SIGNAL(clearOffsetRequested()), mEvent, SLOT(clearOffset()));
	connect(wMan->renderWidget, SIGNAL(clearCommandStackRequested()), mEvent, SLOT(clearCommandStack()));
}

void MainWindow::loadWorld() {
	if (FileIO::isFilePresent(pMan->configFile.inputFile)) {
		// Clear world	
		haptics->stop();
		world->volume.clearVolumes();
		pMan->clearSettingFiles();
		
		LoadWorldThread* thr = new LoadWorldThread(mEvent);
		connect(thr, &LoadWorldThread::resultReady, this, &MainWindow::handleLoadWorldResults);
		connect(thr, &LoadWorldThread::finished, thr, &LoadWorldThread::deleteLater);
		thr->start();
	}

	else {
		QString str = "[ERROR] Input file does not exist.";
		qDebug() << str;
		QMainWindow::statusBar()->showMessage(str);
	}

}

void MainWindow::handleLoadWorldResults() {
	glWindow->queueInitScene = true;
	windowContainer->container->show();
	writeMessage("COMPLETE! Loaded image data.");
}

QAction* MainWindow::initMenuAction(QString name, QMenu* targetMenu, QWidget* linkedWidget) {
	QAction* action = new QAction(name, this);
	targetMenu->addAction(action);
	connect(action, SIGNAL(triggered()), linkedWidget, SLOT(show()));
	return action;
}

void MainWindow::initUI() {


	// Graphics widgets
	glWindow = new OpenGLWindow(this, world,
		pMan->lightParams, pMan->visualParams, 
		pMan->ssParams, pMan->renderParams,
		pMan->isoParams);	
	windowContainer = new WindowContainer(this, glWindow, wMan->fileOpenWidget);

	widgetLayout = new QHBoxLayout(ui.centralWidget);
	widgetLayout->addWidget(windowContainer->container);
	
	// Haptics 
	haptics = new Haptics(world, 
		pMan->probeParams, 
		pMan->isoParams, 
		pMan->visualParams,
		pMan->cGroups);	

	mEvent = new MainWindowEvents(world, pMan, wMan,
		glWindow, haptics, this);
}

void MainWindow::initMenus() {
	// Menu bar
	menu = new Menus(this);
	menu->addMenusToBar(ui.menuBar);

	// Link actions
	menu->actions.push_back(initMenuAction("Load File...", menu->menus[Menus::File], wMan->fileOpenWidget));

	// Editor := parameters that require update texture?
	menu->actions.push_back(initMenuAction("TF Editor", menu->menus[Menus::Window], wMan->tfWidget));
	menu->actions.push_back(initMenuAction("Light Editor", menu->menus[Menus::Window], wMan->lightWidget));
	menu->actions.push_back(initMenuAction("Group Editor", menu->menus[Menus::Window], wMan->groupWidget));
	menu->actions.push_back(initMenuAction("Offset Editor", menu->menus[Menus::Window], wMan->offsetWidget));
	//menu->actions.push_back(initMenuAction("Segmentation Editor", menu->menus[Menus::Window], wMan->fileOpenWidget));
	//menu->actions.push_back(initMenuAction("Statistics Editor", menu->menus[Menus::Window], wMan->fileOpenWidget));

	// Settings := parameters that do not require update to texture?
	menu->actions.push_back(initMenuAction("Probe Settings", menu->menus[Menus::Setting], wMan->probeWidget));
	menu->actions.push_back(initMenuAction("Visualization Settings", menu->menus[Menus::Setting], wMan->visualWidget));
	menu->actions.push_back(initMenuAction("Rendering Settings", menu->menus[Menus::Setting], wMan->renderWidget));
	menu->actions.push_back(initMenuAction("Screenshot Settings", menu->menus[Menus::Setting], wMan->ssWidget));

	menu->actions.push_back(initMenuAction("Keyboard Shortcuts", menu->menus[Menus::Help], wMan->helpKeysWidget));
	menu->actions.push_back(initMenuAction("About SHVR", menu->menus[Menus::Help], wMan->helpAboutWidget));


	// File Options
	connect(menu->actions[Menus::ImportMaskFromLayers], SIGNAL(triggered()), mEvent, SLOT(loadMaskFromLayersData()));
	connect(menu->actions[Menus::ExportVolume], SIGNAL(triggered()), mEvent, SLOT(saveVolumeData()));
	connect(menu->actions[Menus::ImportMask], SIGNAL(triggered()), mEvent, SLOT(loadMaskData()));
	connect(menu->actions[Menus::ExportMask], SIGNAL(triggered()), mEvent, SLOT(saveMaskData()));
	connect(menu->actions[Menus::ImportOffset], SIGNAL(triggered()), mEvent, SLOT(loadOffsetData()));
	connect(menu->actions[Menus::ExportOffset], SIGNAL(triggered()), mEvent, SLOT(saveOffsetData()));

	connect(menu->actions[Menus::ImportScreen], SIGNAL(triggered()), mEvent, SLOT(loadGeneralSettings()));
	connect(menu->actions[Menus::ExportScreen], SIGNAL(triggered()), mEvent, SLOT(saveGeneralSettings()));

	connect(menu->actions[Menus::ImportTransFunc], SIGNAL(triggered()), mEvent, SLOT(loadTFData()));
	connect(menu->actions[Menus::ExportTransFunc], SIGNAL(triggered()), mEvent, SLOT(saveTFData()));

	connect(menu->actions[Menus::ImportGroup], SIGNAL(triggered()), mEvent, SLOT(loadColorGroupData()));
	connect(menu->actions[Menus::ExportGroup], SIGNAL(triggered()), mEvent, SLOT(saveColorGroupData()));

	connect(menu->actions[Menus::ImportNN], SIGNAL(triggered()), mEvent, SLOT(loadNNData()));
	connect(menu->actions[Menus::ExportNN], SIGNAL(triggered()), mEvent, SLOT(saveNNData()));

	connect(menu->actions[Menus::ImportLight], SIGNAL(triggered()), mEvent, SLOT(loadLightData()));
	connect(menu->actions[Menus::ExportLight], SIGNAL(triggered()), mEvent, SLOT(saveLightData()));

	connect(menu->actions[Menus::CompareSegmentation], SIGNAL(triggered()), mEvent, SLOT(compareSegmentation()));
	connect(menu->actions[Menus::CompareTracing], SIGNAL(triggered()), mEvent, SLOT(compareTracing()));
}

void MainWindow::writeMessage(QString s) {
	QMainWindow::statusBar()->showMessage(s);
}

void MainWindow::initEventFilter() {
	//handler for mouse clicks && keyboard events from OpenGLWindow
	eventFilter = new EventFilter(glWindow, haptics, pMan, wMan, mEvent);
	glWindow->installEventFilter(eventFilter);
	this->installEventFilter(eventFilter);
}

void MainWindow::closeEvent(QCloseEvent* event) {
	mEvent->closeProgram();
	event->ignore();
}