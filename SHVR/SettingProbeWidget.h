#pragma once

#include <QWidget>
#include "ParameterStructs.h"
#include "CustomWidgets.h"
#include "ui_SettingProbeWidget.h"

class SettingProbeWidget : public QWidget
{
	Q_OBJECT

public:
	SettingProbeWidget(ProbeParams* _probeParams, QWidget *parent = Q_NULLPTR);
	~SettingProbeWidget();

	void initWidgetList();
	void updateWidgets();
	void initConnections();

signals:
	void paramUpdated();

public slots:
	void updateHapticsEnabled(int v);
	void updateHapticMode(int v);
	void updatePaintMode(int v);
	void updateHapticDampingForce(double v);
	void updateHapticSpringForce(double v);
	void updateHapticHitThreshold(double v);
	void updatePaintScale(double v);
	void updatePaintSize(double v);
	void updateSilhouetteSize(double v);
	void updateWorkspaceScale(double v);
	void updateSelectedGroup(int v);

private:
	Ui::SettingProbeWidget ui;
	ObjectList widgetList;
	ProbeParams* probeParams;
};
