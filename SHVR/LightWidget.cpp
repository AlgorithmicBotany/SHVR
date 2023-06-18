#include "LightWidget.h"

LightWidget::LightWidget(LightParams* _lightParams, AmbientOcclusion* _ao, QWidget *parent)
	: QWidget(parent, Qt::Window), lightParams(_lightParams), ao(_ao)
{
	ui.setupUi(this);
	ui.glWidget_graph->loadGraph(ao);

	initWidgetList();
	initConnections();
}

LightWidget::~LightWidget()
{
}

void LightWidget::initWidgetList() {
	// Graph 
	widgetList.push_back(ui.checkBox_LockIndices);
	widgetList.push_back(ui.pushButton_UpdateGraph);
	widgetList.push_back(ui.glWidget_graph);

	// Steps
	widgetList.push_back(ui.doubleSpinBox_aoStepSize);
	widgetList.push_back(ui.spinBox_aoMaxSteps);

	// Selected CP
	widgetList.push_back(ui.spinBox_SelectedCPIdx);
	widgetList.push_back(ui.doubleSpinBox_xValue);
	widgetList.push_back(ui.doubleSpinBox_xValue);
	widgetList.push_back(ui.doubleSpinBox_yValue);
	widgetList.push_back(ui.doubleSpinBox_yValue);

	// Light params
	widgetList.push_back(ui.doubleSpinBox_ambient);
	widgetList.push_back(ui.doubleSpinBox_diffuse);
	widgetList.push_back(ui.doubleSpinBox_specular);
	widgetList.push_back(ui.doubleSpinBox_specularExponent);
}

void LightWidget::updateWidgets() {
	widgetList.blockSignals(true);

	// Initial values
	ui.doubleSpinBox_aoStepSize->setValue(lightParams->aoStepSize);
	ui.spinBox_aoMaxSteps->setValue(lightParams->aoMaxSteps);
	ui.checkBox_AutoUpdateGraph->setChecked(false);
	ui.checkBox_LockIndices->setChecked(false);
	ui.label_updateStatus->setText("-Updated-");

	// Selected CP
	ui.spinBox_SelectedCPIdx->setMinimum(-1);
	ui.doubleSpinBox_xValue->setMinimum(ao->xValueMin);
	ui.doubleSpinBox_xValue->setMaximum(ao->xValueMax);
	ui.doubleSpinBox_yValue->setMinimum(ao->yValueMin);
	ui.doubleSpinBox_yValue->setMaximum(ao->yValueMax);
	updateSelectedCP();

	// Light params
	ui.doubleSpinBox_ambient->setValue(lightParams->intensityAmbient);
	ui.doubleSpinBox_diffuse->setValue(lightParams->intensityDiffuse);
	ui.doubleSpinBox_specular->setValue(lightParams->intensitySpecular);
	ui.doubleSpinBox_specularExponent->setValue(lightParams->specularExponent);

	ui.spinBox_rayMaxSteps->setValue(lightParams->rayMaxSteps);
	ui.doubleSpinBox_rayStepSize->setValue(lightParams->rayStepSize);

	widgetList.blockSignals(false);
}

void LightWidget::initConnections() {
	//Graph
	connect(ui.checkBox_LockIndices, SIGNAL(stateChanged(int)), this, SLOT(updateLockIndices()));
	connect(ui.pushButton_UpdateGraph, SIGNAL(clicked()), this, SLOT(manualUpdate()));
	connect(ui.glWidget_graph, SIGNAL(updatedGraph()), this, SLOT(autoUpdateGraph()));
	connect(ui.glWidget_graph, SIGNAL(updatedGraph()), this, SLOT(updateSelectedCP()));

	//Steps
	connect(ui.doubleSpinBox_aoStepSize, SIGNAL(valueChanged(double)), this, SLOT(updateAOStepSize(double)));
	connect(ui.spinBox_aoMaxSteps, SIGNAL(valueChanged(int)), this, SLOT(updateAOMaxSteps(int)));

	//Selected CP
	connect(ui.spinBox_SelectedCPIdx, SIGNAL(valueChanged(int)),
		this, SLOT(updateSelectedCPIdx(int)));
	connect(ui.doubleSpinBox_xValue, SIGNAL(valueChanged(double)),
		this, SLOT(updateSelectedCP_xValue(double)));
	connect(ui.doubleSpinBox_yValue, SIGNAL(valueChanged(double)),
		this, SLOT(updateSelectedCP_yValue(double)));

	// Light params
	connect(ui.doubleSpinBox_ambient, SIGNAL(valueChanged(double)),
		this, SLOT(updateAmbientIntensity(double)));
	connect(ui.doubleSpinBox_diffuse, SIGNAL(valueChanged(double)),
		this, SLOT(updateDiffuseIntensity(double)));
	connect(ui.doubleSpinBox_specular, SIGNAL(valueChanged(double)),
		this, SLOT(updateSpecularIntensity(double)));
	connect(ui.doubleSpinBox_specularExponent, SIGNAL(valueChanged(double)),
		this, SLOT(updateSpecularExponent(double)));

	connect(ui.spinBox_rayMaxSteps, SIGNAL(valueChanged(int)),
		this, SLOT(updateRayMaxSteps(int)));
	connect(ui.doubleSpinBox_rayStepSize, SIGNAL(valueChanged(double)),
		this, SLOT(updateRayStepSize(double)));
}


void LightWidget::manualUpdate() {	
	emit aoParamUpdated();
	emit graphUpdated();	
	ui.label_updateStatus->setText("-Updated-");
}

void LightWidget::autoUpdateAOParams() {
	if (ui.checkBox_AutoUpdateGraph->isChecked()) {
		emit aoParamUpdated();
		ui.label_updateStatus->setText("-Updated-");
	}
	else {
		ui.label_updateStatus->setText("-Needs Update-");
	}
}

void LightWidget::autoUpdateGraph() {
	if (ui.checkBox_AutoUpdateGraph->isChecked()) {
		emit graphUpdated();
		ui.label_updateStatus->setText("-Updated-");
	}
	else {
		ui.label_updateStatus->setText("-Needs Update-");
	}
}

void LightWidget::updateLockIndices() {
	ui.glWidget_graph->lockTFIndices = ui.checkBox_LockIndices->isChecked();
}

void LightWidget::updateSelectedCP() {
	ui.spinBox_SelectedCPIdx->setMaximum(ao->controlPoints.size() - 1);

	int idx = ui.glWidget_graph->selectedCPIdx;
	ui.spinBox_SelectedCPIdx->setValue(idx);
	if (idx >= 0) {
		ControlPoint& cp = ao->controlPoints[idx];
		ui.doubleSpinBox_xValue->setValue(cp.values.x);
		ui.doubleSpinBox_yValue->setValue(cp.values.y);
	}
	else {
		ui.doubleSpinBox_xValue->setValue(0);
		ui.doubleSpinBox_yValue->setValue(0);
	}
}

void LightWidget::updateSelectedCPIdx(int idx) {
	ui.glWidget_graph->selectedCPIdx = idx;
	ui.glWidget_graph->update();
	updateSelectedCP();
}

void LightWidget::updateSelectedCP_xValue(double xValue) {
	int selectedCPIdx = ui.glWidget_graph->selectedCPIdx;
	ControlPoint& selectedCP = ao->controlPoints[selectedCPIdx];
	ao->controlPoints[selectedCPIdx].values.x = xValue;
	if (ui.checkBox_LockIndices->isChecked()) {
		ao->boundCPToIdx(selectedCPIdx);
	}
	else {
		ao->boundCPToWindow(selectedCP);
		ui.glWidget_graph->selectedCPIdx = ao->updateCPIdx(selectedCP);
	}
	ui.glWidget_graph->update();
	updateSelectedCP();
	autoUpdateGraph();
}

void LightWidget::updateSelectedCP_yValue(double yValue) {
	int idx = ui.glWidget_graph->selectedCPIdx;
	ao->controlPoints[idx].values.y = yValue;
	ui.glWidget_graph->update();
	updateSelectedCP();
	autoUpdateGraph();
}

void LightWidget::updateAOMaxSteps(int maxSteps) {
	lightParams->aoMaxSteps = maxSteps; 
	autoUpdateAOParams();
}

void LightWidget::updateAOStepSize(double stepSize) {
	lightParams->aoStepSize = stepSize;
	autoUpdateAOParams();
}

void LightWidget::updateAmbientIntensity(double intensity) {
	lightParams->intensityAmbient = intensity;
}

void LightWidget::updateDiffuseIntensity(double intensity) {
	lightParams->intensityDiffuse = intensity;
}

void LightWidget::updateSpecularIntensity(double intensity) {
	lightParams->intensitySpecular = intensity;
}

void LightWidget::updateSpecularExponent(double exponent) {
	lightParams->specularExponent = exponent;
}

void LightWidget::updateRayMaxSteps(int maxSteps) {
	lightParams->rayMaxSteps = maxSteps;
}

void LightWidget::updateRayStepSize(double stepSize) {
	lightParams->rayStepSize = stepSize;
}


