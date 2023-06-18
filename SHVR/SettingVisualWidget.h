#pragma once

#include <QWidget>
#include "ParameterStructs.h"
#include "CustomWidgets.h"
#include "ui_SettingVisualWidget.h"

class SettingVisualWidget : public QWidget
{
	Q_OBJECT

public:
	SettingVisualWidget(VisualParams* _visualParams, QWidget *parent = Q_NULLPTR);
	~SettingVisualWidget();
	
	void initWidgetList();
	void updateWidgets();
	void initConnections();

signals:
	void paramUpdated();
	void cutUpdated();

public slots:
	void saveCamera();
	void loadCamera();

	void updateArrowEnabled(int v);
	void updateHudEnabled(int v);
	void updateVolumeEnabled(int v);
	void updateObjectEnabled(int v);
	void updateForceLineEnabled(int v);
	void updateFOV(double v);

	void updateScreenType(int v);
	void updateClipFar(double v);
	void updateClipNear(double v);

	void updateCutMinX(double v);
	void updateCutMinY(double v);
	void updateCutMinZ(double v);
	void updateCutMaxX(double v);
	void updateCutMaxY(double v);
	void updateCutMaxZ(double v);

private:
	Ui::SettingVisualWidget ui;
	ObjectList widgetList;
	VisualParams* visualParams;
};
