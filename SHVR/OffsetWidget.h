#pragma once

#include <QWidget>
#include "OffsetVolume.h"
#include "Graph2DOpenGL.h"
#include "ui_OffsetWidget.h"

class OffsetWidget : public QWidget
{
	Q_OBJECT

public:
	OffsetWidget(OffsetVolume* _offsetVolume, QWidget *parent = Q_NULLPTR);
	~OffsetWidget();

//	void initWidgetList();
//
//	void updateWidgets();
//
//	void initConnections();
//
//
//
//signals:
//	void paramUpdated();
//	void graphUpdated();
//
//public slots:
//	void manualUpdate();
//	void autoUpdateAOParams();
//	void autoUpdateGraph();
//	void updateLockIndices();
//	void updateSelectedCP();
//	void updateSelectedCPIdx(int idx);
//	void updateSelectedCP_xValue(double xValue);
//	void updateSelectedCP_yValue(double yValue);

private:
	Ui::OffsetWidget ui;
	OffsetVolume* offsetVolume;
	ObjectList widgetList;
};
