#pragma once

#include <QWidget>
#include "ParameterStructs.h"
#include "CustomWidgets.h"
#include "ui_SettingRenderWidget.h"

class SettingRenderQualityWidget : public QWidget
{
	Q_OBJECT

public:
	SettingRenderQualityWidget(RenderParams* _renderParams, QWidget *parent = Q_NULLPTR);
	~SettingRenderQualityWidget();

	void initWidgetList();
	void updateWidgets();
	void initConnections();

signals: 
	void paramUpdated();
	void commandStackUpdated();
	void marchingCubeRequested();
	void clearMaskRequested();
	void clearOffsetRequested();
	void clearCommandStackRequested();

public slots:
	void updateFPS(double value);
	void updateCompressionRatio(double value);
	void updateMaxNumberOfCommands(int value);
	void generateMarchingCube();
	void clearMask();
	void clearOffset();
	void clearCommandStack();

private:
	Ui::SettingRenderWidget ui;
	ObjectList widgetList;
	RenderParams* renderParams;

};
