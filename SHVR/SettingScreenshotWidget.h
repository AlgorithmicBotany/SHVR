#pragma once

#include <QWidget>
#include <qdesktopservices.h>
#include "CustomWidgets.h"
#include "FileIO.h"
#include "ParameterStructs.h"
#include "ui_SettingScreenshotWidget.h"

class SettingScreenshotWidget : public QWidget
{
	Q_OBJECT

public:
	SettingScreenshotWidget(ScreenshotParams* _ssParams, QWidget* parent = Q_NULLPTR);
	~SettingScreenshotWidget();

	void initWidgetList();
	void initConnections();
	void updateWidgets();
	void updateSSDimensions();

private:
	Ui::SettingScreenshotWidget ui;
	ObjectList widgetList;
	ScreenshotParams* ssParams;

signals:
	void screenshot();

public slots:
	void updateKeepAspectRatio();	
	void updateWidth(int width);
	void updateHeight(int width);
	void takeScreenshot();
	void openSSDirectory();
	void modifySSDirectory();
};
