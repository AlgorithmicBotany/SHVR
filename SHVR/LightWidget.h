#pragma once

//Note: Because "QWidget" cannot be placed into templates, the
// signals and slots are almost identical to TFWidget

#include "Graph2DOpenGL.h"
class AOOpenGLWidget : public Graph2DOpenGL {
public:
	AOOpenGLWidget(QWidget* parent) : Graph2DOpenGL(parent)
	{}
};

#include <QWidget>
#include "ui_LightWidget.h"
#include "CustomWidgets.h"
#include "AmbientOcclusion.h"

class LightWidget : public QWidget
{
	Q_OBJECT

public:
	LightWidget(LightParams* _lightParams, AmbientOcclusion* _ao, QWidget *parent = Q_NULLPTR);
	~LightWidget();

	void initWidgetList();
	void updateWidgets();
	void initConnections();

private:
	ObjectList widgetList;
	Ui::LightWidget ui;
	LightParams* lightParams;
	AmbientOcclusion* ao;
signals:
	void graphUpdated();
	void aoParamUpdated();

public slots:
	void manualUpdate();
	void autoUpdateAOParams();
	void autoUpdateGraph();
	void updateLockIndices();
	void updateSelectedCP();
	void updateSelectedCPIdx(int idx);
	void updateSelectedCP_xValue(double xValue);
	void updateSelectedCP_yValue(double yValue);
	void updateAOMaxSteps(int maxSteps);
	void updateAOStepSize(double stepSize);
	void updateAmbientIntensity(double intensity);
	void updateDiffuseIntensity(double intensity);
	void updateSpecularIntensity(double intensity);
	void updateSpecularExponent(double exponent);
	void updateRayMaxSteps(int maxSteps);
	void updateRayStepSize(double stepSize);
};
