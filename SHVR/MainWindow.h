#pragma once

#include <QtWidgets/QMainWindow>
#include <qwidget.h>
#include "ui_MainWindow.h"
#include <QDebug>

#include <qevent.h>	//keyboard
#include <qmenu.h>	//UI
#include <qlayout.h>
#include <qmimedata.h>

#include "StatusBar.h"
#include "Logger.h"
#define ENABLE_LOGGER

#include "ParameterManager.h"
#include "OpenGLWindow.h"
#include "Haptics.h"

#include "MainWindowObjects.h"
#include "Events.h"
#include "EventFilter.h"



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();		
	
protected:
	void closeEvent(QCloseEvent* event) override;

private:
	Ui::MainWindowClass ui;
	QHBoxLayout* widgetLayout;
	Menus* menu;
	WindowContainer* windowContainer;
	MainWindowEvents* mEvent;

	World* world;
	ParameterManager* pMan;
	OpenGLWindow* glWindow;
	Haptics* haptics;
	WidgetManager* wMan;
	EventFilter* eventFilter;

	void initConnections();
	
	QAction* initMenuAction(QString name, QMenu* targetMenu, QWidget* linkedWidget);

	void initUI();
	void initMenus();
	void initEventFilter();

public slots:

	void loadWorld();
	void writeMessage(QString s);
	void handleLoadWorldResults();	
};
