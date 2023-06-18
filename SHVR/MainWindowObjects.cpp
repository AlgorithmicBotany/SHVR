#include "MainWindowObjects.h"


WindowContainer::WindowContainer(QWidget* windowObj, OpenGLWindow* openGLWindow,
	FileOpenWidget* foWidget) :
	parent(windowObj),
	glWindow(openGLWindow) {

	initContainer();
}

WindowContainer::~WindowContainer() {
	delete container;
	//delete eventFilter;
}

void WindowContainer::initContainer() {
	container = QWidget::createWindowContainer(glWindow, parent);	//Create container to house VRCW in MainWindow
	container->setMinimumSize(200, 200);
	container->setFocusPolicy(Qt::NoFocus);
	container->hide();	// Note: Prevent triggering of initializeGL() until volume is loaded
}



//====================//
//===== MENUS ========//
//====================//


Menus::Menus(QWidget* parent) :
	parentWidget(parent) {
	menus.resize(COUNT_MENU);
	actions.resize(COUNT_ACTION);

	// init top bar
	menus[File] = new QMenu("File", parentWidget);
	menus[Window] = new QMenu("Window", parentWidget);
	menus[File_import] = new QMenu("Import...", parentWidget);
	menus[File_export] = new QMenu("Export...", parentWidget);
	menus[File_compare] = new QMenu("Comparing...", parentWidget);
	menus[Setting] = new QMenu("Setting", parentWidget);
	menus[Help] = new QMenu("Help", parentWidget);

	// init Actions			
	actions[ImportTransFunc] = new QAction("Transfer Function", parentWidget);
	actions[ImportScreen] = new QAction("Screen Settings", parentWidget);
	actions[ImportGroup] = new QAction("Group Settings", parentWidget);
	actions[ImportNN] = new QAction("Node Network", parentWidget);
	actions[ImportMaskFromLayers] = new QAction("Mask From Layers", parentWidget);
	actions[ImportMask] = new QAction("Mask", parentWidget);
	actions[ImportOffset] = new QAction("Offsets", parentWidget);
	actions[ImportLight] = new QAction("Light", parentWidget);

	actions[ExportTransFunc] = new QAction("Transfer Function", parentWidget);
	actions[ExportScreen] = new QAction("Screen Settings", parentWidget);
	actions[ExportGroup] = new QAction("Group Settings", parentWidget);
	actions[ExportNN] = new QAction("Node Network", parentWidget);
	actions[ExportVolume] = new QAction("Volume", parentWidget);
	actions[ExportMask] = new QAction("Mask", parentWidget);
	actions[ExportOffset] = new QAction("Offset", parentWidget);
	actions[ExportLight] = new QAction("Light", parentWidget);

	actions[CompareSegmentation] = new QAction("Segmentation", parentWidget);
	actions[CompareTracing] = new QAction("Tracing", parentWidget);

	// Add actions to menus
	menus[File]->addMenu(menus[File_import]);
	menus[File_import]->addAction(actions[ImportTransFunc]);
	menus[File_import]->addAction(actions[ImportScreen]);
	menus[File_import]->addAction(actions[ImportGroup]);
	menus[File_import]->addAction(actions[ImportNN]);
	menus[File_import]->addAction(actions[ImportMaskFromLayers]);
	menus[File_import]->addAction(actions[ImportMask]);
	menus[File_import]->addAction(actions[ImportOffset]);
	menus[File_import]->addAction(actions[ImportLight]);

	menus[File]->addMenu(menus[File_export]);
	menus[File_export]->addAction(actions[ExportTransFunc]);
	menus[File_export]->addAction(actions[ExportScreen]);
	menus[File_export]->addAction(actions[ExportGroup]);
	menus[File_export]->addAction(actions[ExportNN]);
	menus[File_export]->addAction(actions[ExportVolume]);
	menus[File_export]->addAction(actions[ExportMask]);
	menus[File_export]->addAction(actions[ExportOffset]);
	menus[File_export]->addAction(actions[ExportLight]);

	menus[File]->addMenu(menus[File_compare]);
	menus[File_compare]->addAction(actions[CompareSegmentation]);
	menus[File_compare]->addAction(actions[CompareTracing]);
}

void Menus::clear() {
	for (QMenu* m : menus) { delete m; }
	for (QAction* a : actions) { delete a; }
}

void Menus::addMenusToBar(QMenuBar* bar) {
	bar->addMenu(menus[File]);
	bar->addMenu(menus[Window]);
	bar->addMenu(menus[Setting]);
	bar->addMenu(menus[Help]);
}