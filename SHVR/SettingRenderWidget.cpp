#include "SettingRenderWidget.h"

SettingRenderQualityWidget::SettingRenderQualityWidget(RenderParams* _renderParams, QWidget *parent)
	: QWidget(parent, Qt::Window), renderParams(_renderParams)
{
	ui.setupUi(this);
	initWidgetList();
	initConnections();
}

SettingRenderQualityWidget::~SettingRenderQualityWidget()
{
}


void SettingRenderQualityWidget::initWidgetList() {
	widgetList.push_back(ui.doubleSpinBox_maxFPS);
	widgetList.push_back(ui.doubleSpinBox_screenCompression);
}
void SettingRenderQualityWidget::updateWidgets() {
	widgetList.blockSignals(true);

	ui.doubleSpinBox_maxFPS->setValue(renderParams->fps);
	ui.doubleSpinBox_screenCompression->setValue(renderParams->compressionRatio);
	ui.spinBox_maxNumberOfCommands->setValue(renderParams->maxNumberOfCommands);

	widgetList.blockSignals(false);
}
void SettingRenderQualityWidget::initConnections() {
	connect(ui.doubleSpinBox_maxFPS, SIGNAL(valueChanged(double)), this, SLOT(updateFPS(double)));
	connect(ui.doubleSpinBox_screenCompression, SIGNAL(valueChanged(double)), this, SLOT(updateCompressionRatio(double)));
	connect(ui.spinBox_maxNumberOfCommands, SIGNAL(valueChanged(int)), this, SLOT(updateMaxNumberOfCommands(int)));
	connect(ui.pushButton_generateMarchingCube, SIGNAL(clicked()), this, SLOT(generateMarchingCube()));
	connect(ui.pushButton_clearMask, SIGNAL(clicked()), this, SLOT(clearMask()));
	connect(ui.pushButton_clearOffset, SIGNAL(clicked()), this, SLOT(clearOffset()));
	connect(ui.pushButton_clearCommandStack, SIGNAL(clicked()), this, SLOT(clearCommandStack()));
}

void SettingRenderQualityWidget::updateFPS(double value) {
	renderParams->fps = value;
	emit paramUpdated();
}

void SettingRenderQualityWidget::updateCompressionRatio(double value) {
	renderParams->compressionRatio = value;
	emit paramUpdated();
}

void SettingRenderQualityWidget::updateMaxNumberOfCommands(int value) {
	renderParams->maxNumberOfCommands = value;
	emit commandStackUpdated();
}

void SettingRenderQualityWidget::generateMarchingCube() {
	emit marchingCubeRequested();
}

void SettingRenderQualityWidget::clearMask() {
	emit clearMaskRequested();
}

void SettingRenderQualityWidget::clearOffset() {
	emit clearOffsetRequested();
}

void SettingRenderQualityWidget::clearCommandStack() {
	emit clearCommandStackRequested();
}