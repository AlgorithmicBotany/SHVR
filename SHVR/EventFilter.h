#pragma once

#include <QObject>
#include <QDebug>
#include "FileOpenWidget.h"
#include "OpenGLWindow.h"
#include "World.h"
#include "NodeNetwork.h"
#include "ParameterManager.h"
#include "WidgetManager.h"
#include "Events.h"
#include "Haptics.h"

class EventFilter : public QObject
{
	Q_OBJECT
public:
	OpenGLWindow* glWindow;	//Used to send commands to VolumeRayCastWindow
	World* world;
	glm::vec3 pointNDC_prev;
	glm::mat4 modelM_prev;
	bool mouseButtonPressed;
	bool modifierPressed;
	FileOpenWidget* fileOpenWidget;
	WidgetManager* wMan;
	ParameterManager* pMan;
	MainWindowEvents* mEvent;
	Haptics* haptics;

	EventFilter(OpenGLWindow* openGLWindow, Haptics* _haptics, 
		ParameterManager* _pMan, WidgetManager* _wMan, MainWindowEvents* _mEvent);
	void startViewing(QEvent* event);
	void panView(QEvent* event);
	void zoomView(QEvent* event);
	void rotateVolumeInView(QEvent* event);

	bool keyPressEvent(QKeyEvent* event);

protected:
	bool eventFilter(QObject* obj, QEvent* event);
};
