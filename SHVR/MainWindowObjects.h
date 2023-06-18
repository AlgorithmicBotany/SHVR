#pragma once
#include "OpenGLWindow.h"
#include "FileOpenWidget.h"
#include "qwidget.h"

#include "qmenu.h"
#include "qmenubar.h"
#include "qaction.h"
#include "EventFilter.h"

struct WindowContainer {
	OpenGLWindow* glWindow;
	QWidget* parent;
	QWidget* container;

	WindowContainer(QWidget* windowObj, OpenGLWindow* openGLWindow, FileOpenWidget* foWidget);
	~WindowContainer();

private:
	void initContainer();
};


struct Menus {
	enum Menu {
		File,
		Window,
		File_import,
		File_export,
		File_compare,
		Setting,
		Help,
		COUNT_MENU
	};
	enum Action {
		ImportTransFunc,
		ImportScreen,
		ImportGroup,
		ImportNN,
		ImportMaskFromLayers,
		ImportMask,
		ImportOffset,
		ImportLight,

		ExportTransFunc,
		ExportScreen,
		ExportGroup,
		ExportNN,
		ExportVolume,
		ExportMask,
		ExportOffset,
		ExportLight,

		CompareTracing,
		CompareSegmentation,

		COUNT_ACTION
	};

	std::vector<QMenu*> menus;
	std::vector<QAction*> actions;
	QWidget* parentWidget;

	//Functions
	Menus(QWidget* parent);
	void clear();
	void addMenusToBar(QMenuBar* bar);
};
